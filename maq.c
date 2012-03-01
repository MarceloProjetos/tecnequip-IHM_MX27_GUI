#include "defines.h"
#include "maq.h"

extern pthread_mutex_t mutex_gui_lock;

// Função que retorna checksum de ponteiro
unsigned long CalcCheckSum(void *ptr, unsigned int tam)
{
  unsigned char *cptr = (unsigned char *)(ptr);
  unsigned long checksum = 0;

  while(tam--)
    {
    checksum += *cptr;
    cptr++;
    }

  return checksum;
}

struct strMaqReply {
  struct MB_Reply modbus_reply;
  uint32_t ready;
};

void retMaqMB(void *dt, void *res)
{
  struct strMaqReply *rp = (struct strMaqReply *)res;

  rp->modbus_reply = ((union uniIPCMQ_Data *)dt)->modbus_reply;
  rp->ready = 1; // Recebida resposta
}

void EnviarMB(struct strIPCMQ_Message *ipc_msg, struct strMaqReply *rp)
{
#ifdef DEBUG_PC_NOETH
  rp->modbus_reply.ExceptionCode = MB_EXCEPTION_NONE;
  return;
#endif
  pthread_mutex_lock  (&mutex_gui_lock);

  IPCMQ_Main_Enviar(ipc_msg);
  while(!rp->ready) {
    IPC_Update();
    usleep(100);
  }

  pthread_mutex_unlock(&mutex_gui_lock);
}

// Funcao que sincroniza a estrutura de parametros com o clp. Retorna default_value se erro
uint16_t MaqLerRegistrador(uint16_t reg, uint16_t default_value)
{
  struct strMaqReply rp;
  struct strIPCMQ_Message ipc_msg;

  memset(&rp, 0, sizeof(rp));

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = retMaqMB;
  ipc_msg.res   = (void *)&rp;
  ipc_msg.data.modbus_query.function_code = MB_FC_READ_HOLDING_REGISTERS;
  ipc_msg.data.modbus_query.data.read_holding_registers.start = reg;
  ipc_msg.data.modbus_query.data.read_holding_registers.quant = 1;

  EnviarMB(&ipc_msg, &rp);

  if(rp.modbus_reply.ExceptionCode != MB_EXCEPTION_NONE) {
    return default_value; // Erro de comunicacao
  }

  return CONV_PCHAR_UINT16(rp.modbus_reply.reply.read_holding_registers.data);
}

void MaqGravarRegistrador(uint16_t reg, uint16_t val)
{
  struct strIPCMQ_Message ipc_msg;

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = NULL;
  ipc_msg.res   = NULL;
  ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
  ipc_msg.data.modbus_query.data.write_single_register.address = reg;
  ipc_msg.data.modbus_query.data.write_single_register.val = val;

  IPCMQ_Main_Enviar(&ipc_msg);
}

int MaqSync(unsigned int mask)
{
  printf("Sincronizando parametros da maquina:\n");

  if(mask & MAQ_SYNC_ENCODER) {
    printf("maq_param.encoder.fator....: %f\n", maq_param.encoder.fator);
    printf("maq_param.encoder.perimetro: %d\n", maq_param.encoder.perimetro);
    printf("maq_param.encoder.precisao.: %d\n", maq_param.encoder.precisao);

    MaqGravarRegistrador(MAQ_REG_ENC_FATOR, (unsigned int)(maq_param.encoder.fator*10000));
    MaqGravarRegistrador(MAQ_REG_ENC_RESOL, maq_param.encoder.precisao);
    MaqGravarRegistrador(MAQ_REG_ENC_PERIM, maq_param.encoder.perimetro);
  }

  if(mask & MAQ_SYNC_APLAN) {
    printf("maq_param.aplanadora.auto_vel....: %d\n", maq_param.aplanadora.auto_vel);
    printf("maq_param.aplanadora.manual_vel..: %d\n", maq_param.aplanadora.manual_vel);
    printf("maq_param.aplanadora.rampa.......: %f\n", maq_param.aplanadora.rampa);

    MaqGravarRegistrador(MAQ_REG_APL_VELAUTO, maq_param.aplanadora.auto_vel);
    MaqGravarRegistrador(MAQ_REG_APL_VELMAN , maq_param.aplanadora.manual_vel);
    MaqGravarRegistrador(MAQ_REG_APL_ACELMS , (unsigned int)(maq_param.aplanadora.rampa*1000));

    MaqConfigFlags(MaqLerFlags() | MAQ_MODO_SYNC_SERVO);
  }

  return 1;
}

void MaqSyncPassos(unsigned int passos)
{
  unsigned int i, low=0, high=0;
  printf("Sincronizando passos:\n");

  if(passos > MAQ_PASSOS_MAX) {
    passos = MAQ_PASSOS_MAX;
  }

  for(i=0; i<passos; i++) {
    MaqGravarRegistrador(MAQ_REG_PASSOS_START+i*2  , maq_param.aplanadora.passos[i].passo);
    MaqGravarRegistrador(MAQ_REG_PASSOS_START+i*2+1, maq_param.aplanadora.passos[i].repeticoes);
    if(i<MAQ_PASSOS_MAX/2) {
      low  |= maq_param.aplanadora.passos[i].portas<<(i*5);
    } else {
      high |= maq_param.aplanadora.passos[i].portas<<(i*5);
    }
  }

  printf("low = %d, high = %d\n", low, high);
  MaqGravarRegistrador(MAQ_REG_PASSOS_PORTASL, low   );
  MaqGravarRegistrador(MAQ_REG_PASSOS_PORTASH, high  );
  MaqGravarRegistrador(MAQ_REG_PASSOS_QTD    , passos);
}

// Funcao que grava a estrutura de configuracao no disco
void MaqGravarConfig(void)
{
  long fd = open(MAQ_ARQ_CONFIG, O_WRONLY | O_CREAT, 0666);
  unsigned long checksum, magico = MAQ_CFG_MAGIC;

  if(fd<0)
    return;

// Grava o número mágico para identificação do arquivo
  write(fd, &magico, sizeof(magico));

// Grava os dados
  write(fd, &maq_param, sizeof(maq_param));

// Calcula e grava o checksum dos dados
  checksum = CalcCheckSum((void *)(&maq_param), sizeof(maq_param));
  write(fd, &checksum, sizeof(checksum));

  close(fd);
}

// Funcao que le a estrutura de configuracao do disco
int MaqLerConfig(void)
{
  struct strMaqParam maq_tmp;
  long fd = open(MAQ_ARQ_CONFIG, O_RDONLY), ret=0;
  unsigned long val;

  if(fd<0)
    return ret;

  read(fd, &val, sizeof(val));
  if(val == MAQ_CFG_MAGIC)
    {
    read(fd, &maq_tmp, sizeof(maq_tmp));

    read(fd, &val, sizeof(val));
    if(val == CalcCheckSum((void *)(&maq_tmp), sizeof(maq_tmp)))
      {
      memcpy(&maq_param, &maq_tmp, sizeof(maq_tmp));

// Grava o resultado da funcao que sincroniza os dados em ret.
// Assim a funcao retorna OK se os dados foram efetivamente gravados.
//      ret = MaqSync(MAQ_SYNC_TODOS);
      // Devido a problemas de comunicação entre as threads antes de entrar em gtk_main,
      // a função de sincronismo deve ser executada somente após o login do usuário.
      ret = 1;
      }
    }

  close(fd);

  return ret;
}

char *MaqStrErro(uint16_t erro)
{
  uint32_t i;
  char *msg_erro[] = {
      "Erro na comunicação",
      "Emergência Acionada",
      "Falta de fase",
      "Erro na unidade hidráulica",
      "Erro na Prensa - Térmico do Martelo",
      "Baixa pressão de ar",
      "Erro na Prensa - Inversor",
      "Erro na Aplanadora - Servomotor",
      "Erro no Desbobinador",
      "Erro de comunicação - Servomotor",
      "Erro de posicionamento da aplanadora",
  };

  if(!erro) // Sem erro, retorna string nula
    return NULL;

  for(i=0; !((erro>>i)&1); i++); // Busca primeiro bit ligado

  if(i < ARRAY_SIZE(msg_erro)) {
    return msg_erro[i];
  } else {
    return "Erro indefinido!";
  }
}

uint16_t MaqLerErros(void)
{
  uint16_t erro;
  struct strMaqReply rp;
  struct strIPCMQ_Message ipc_msg;

  memset(&rp, 0, sizeof(rp));

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = retMaqMB;
  ipc_msg.res   = (void *)&rp;
  ipc_msg.data.modbus_query.function_code = MB_FC_READ_HOLDING_REGISTERS;
  ipc_msg.data.modbus_query.data.read_holding_registers.start = MAQ_REG_ERROS;
  ipc_msg.data.modbus_query.data.read_holding_registers.quant = 1;

  EnviarMB(&ipc_msg, &rp);

  if(rp.modbus_reply.ExceptionCode != MB_EXCEPTION_NONE)
    return 1; // Erro de comunicacao

  erro = CONV_PCHAR_UINT16(rp.modbus_reply.reply.read_holding_registers.data);
  printf("Erro lido: %04x\n", erro);

  return erro << 1;
}

uint16_t MaqLerFlags(void)
{
  static uint16_t flags = 0;
  flags = MaqLerRegistrador(MAQ_REG_FLAGS, flags);
  return flags;
}

uint16_t MaqLerFlagsManual(void)
{
  static uint16_t flags = 0;
  flags = MaqLerRegistrador(MAQ_REG_FLAGS_MANUAL, flags);
  return flags;
}

uint16_t MaqLerEstado(void)
{
  static uint16_t status;

  status = MaqLerRegistrador(MAQ_REG_STATUS, status);
  printf("status: %d\n", status);

  return status;
}

uint32_t MaqLerEntradas(void)
{
  uint32_t val;
  unsigned char *buf;
  struct strMaqReply rp;
  struct strIPCMQ_Message ipc_msg;

  memset(&rp, 0, sizeof(rp));

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = retMaqMB;
  ipc_msg.res   = (void *)&rp;
  ipc_msg.data.modbus_query.function_code = MB_FC_READ_DISCRETE_INPUTS;
  ipc_msg.data.modbus_query.data.read_discrete_inputs.start = 0;
  ipc_msg.data.modbus_query.data.read_discrete_inputs.quant = 19;

  EnviarMB(&ipc_msg, &rp);

  if(rp.modbus_reply.ExceptionCode != MB_EXCEPTION_NONE)
    return 0; // Erro de comunicacao

  buf  = rp.modbus_reply.reply.read_discrete_inputs.data;
  val  = ((uint32_t)(buf[2]) << 16) | ((uint32_t)(buf[1]) << 8) | buf[0];
  val &= 0x7FFFF;
  printf("input: %05x\n", val);

  return val;
}

uint32_t MaqLerSaidas(void)
{
  volatile uint32_t val;
  unsigned char *buf;
  struct strMaqReply rp;
  struct strIPCMQ_Message ipc_msg;

  memset(&rp, 0, sizeof(rp));

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = retMaqMB;
  ipc_msg.res   = (void *)&rp;
  ipc_msg.data.modbus_query.function_code = MB_FC_READ_COILS;
  ipc_msg.data.modbus_query.data.read_coils.start = 0;
  ipc_msg.data.modbus_query.data.read_coils.quant = 16;

  EnviarMB(&ipc_msg, &rp);

  if(rp.modbus_reply.ExceptionCode != MB_EXCEPTION_NONE)
    return 0; // Erro de comunicacao

  buf = rp.modbus_reply.reply.read_coils.data;
  val = ((uint32_t)(buf[1]) << 8) | buf[0];
  printf("output: %04x\n", val);

  return val;
}

int16_t MaqLerPosAtual(void)
{
  static int16_t pos;

  pos = MaqLerRegistrador(MAQ_REG_POS_ATUAL, pos);

  return pos;
}

int16_t MaqLerAplanErroPosic(void)
{
  static int16_t erro;

  erro = MaqLerRegistrador(MAQ_REG_APL_ERRO_POSIC, erro);
  printf("Ultimo erro: %d\n", erro);

  return erro;
}

void MaqConfigFlags(uint16_t flags)
{
  struct strMaqReply rp;
  struct strIPCMQ_Message ipc_msg;

  memset(&rp, 0, sizeof(rp));

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = NULL;
  ipc_msg.res   = NULL;
  ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
  ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_FLAGS;
  ipc_msg.data.modbus_query.data.write_single_register.val = flags;

  IPCMQ_Main_Enviar(&ipc_msg);
}

void MaqConfigFlagsManual(uint16_t flags)
{
  struct strMaqReply rp;
  struct strIPCMQ_Message ipc_msg;

  memset(&rp, 0, sizeof(rp));

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = NULL;
  ipc_msg.res   = NULL;
  ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
  ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_FLAGS_MANUAL;
  ipc_msg.data.modbus_query.data.write_single_register.val = flags;

  IPCMQ_Main_Enviar(&ipc_msg);
}

void MaqConfigEstado(uint16_t estado)
{
  struct strIPCMQ_Message ipc_msg;

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = NULL;
  ipc_msg.res   = NULL;
  ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
  ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_STATUS;
  ipc_msg.data.modbus_query.data.write_single_register.val = estado;

  IPCMQ_Main_Enviar(&ipc_msg);
}

void MaqConfigModo(uint16_t modo)
{
  modo = (MaqLerFlags() & ~MAQ_MODO_MASK) | (modo & MAQ_MODO_MASK);
  MaqConfigFlags(modo);

  printf("modo = %d\n", modo);
}

void MaqLiberar(uint16_t liberar)
{
  uint16_t flags = MaqLerFlags();

  // Se bloqueou a máquina, desliga todas as flags indicadas pela máscara.
  MaqConfigFlags(liberar ? flags | MAQ_MODO_LIBERA : flags & MAQ_MODO_MASK_LIBERA);

  printf("maquina liberada ? %d\n", liberar);
}

void MaqLimparErro()
{
  uint16_t modo = MaqLerFlags();
  modo |= MAQ_MODO_LIMPAR;
  if(MaqLerErros() & MAQ_ERRO_APLAN_SERVO) {
    modo |= MAQ_MODO_SYNC_SERVO;
  }
  MaqConfigFlags(modo);
}

void MaqAplanManual(uint16_t comando)
{
  uint16_t flags = MaqLerFlagsManual();

  switch(comando) {
    case MAQ_APLAN_AVANCAR:
        flags |=  MAQ_FM_APLAN_AVANCAR;
        flags &= ~MAQ_FM_APLAN_RECUAR;
        break;

    case MAQ_APLAN_RECUAR:
        flags &= ~MAQ_FM_APLAN_AVANCAR;
        flags |=  MAQ_FM_APLAN_RECUAR;
      break;

    case MAQ_APLAN_PARAR:
        flags &= ~MAQ_FM_APLAN_AVANCAR;
        flags &= ~MAQ_FM_APLAN_RECUAR;
      break;
}
  MaqConfigFlagsManual(flags);
}

uint16_t MaqPronta()
{
  return MaqLerEstado() & MAQ_STATUS_PRONTA ? TRUE : FALSE;
}

void MaqConfigEncoder(struct strMaqParamEncoder enc)
{
  maq_param.encoder = enc;

  MaqSync(MAQ_SYNC_ENCODER);
}

struct strMaqParamEncoder MaqLerEncoder()
{
  return maq_param.encoder;
}

void MaqConfigAplan(struct strMaqParamAplan aplan)
{
  maq_param.aplanadora = aplan;

  MaqSync(MAQ_SYNC_APLAN);
}

struct strMaqParamAplan MaqLerAplan()
{
  return maq_param.aplanadora;
}
