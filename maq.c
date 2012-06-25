#include "defines.h"
#include "maq.h"

extern pthread_mutex_t mutex_gui_lock;

extern void IPC_Update(void);

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
  struct MODBUS_Reply modbus_reply;
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
#ifndef DEBUG_PC_NOETH
  pthread_mutex_lock  (&mutex_gui_lock);

  IPCMQ_Main_Enviar(ipc_msg);
  while(!rp->ready) {
    IPC_Update();
    usleep(100);
  }

  pthread_mutex_unlock(&mutex_gui_lock);
#else
  rp->modbus_reply.ExceptionCode = MB_EXCEPTION_NONE;
#endif
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
  ipc_msg.data.modbus_query.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
  ipc_msg.data.modbus_query.data.read_holding_registers.start = reg;
  ipc_msg.data.modbus_query.data.read_holding_registers.quant = 1;

  EnviarMB(&ipc_msg, &rp);

  if(rp.modbus_reply.ExceptionCode != MODBUS_EXCEPTION_NONE) {
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
  ipc_msg.data.modbus_query.function_code = MODBUS_FC_WRITE_SINGLE_REGISTER;
  ipc_msg.data.modbus_query.data.write_single_register.address = reg;
  ipc_msg.data.modbus_query.data.write_single_register.val = val;

  IPCMQ_Main_Enviar(&ipc_msg);
}

uint16_t MaqLerFlags(void)
{
  static uint16_t flags = 0;

  flags = MaqLerRegistrador(MAQ_REG_FLAGS, flags);

  return flags;
}

void MaqConfigFlags(uint16_t flags)
{
  printf("flags = %d\n", flags);
  MaqGravarRegistrador(MAQ_REG_FLAGS, flags);
}

// Funcao que sincroniza a estrutura de parametros com o clp
int MaqSync(unsigned int mask)
{
  static unsigned int first = 1;
  unsigned long rel_motor_perfil = 0;

  printf("Sincronizando parametros da maquina:\n");

  if(mask & MAQ_SYNC_PERFIL) {
    if(maq_param.perfil.diam_rolo) {
      rel_motor_perfil = (unsigned long)((maq_param.perfil.fator * 1000000) / ((float)(3.14159 * maq_param.perfil.diam_rolo)));
    }

    printf("rel_motor_perfil...............: %ld\n", rel_motor_perfil);
    printf("maq_param.perfil.fator.........: %f\n" , maq_param.perfil.fator);
    printf("maq_param.perfil.diam_rolo.....: %d\n" , maq_param.perfil.diam_rolo);
    printf("maq_param.perfil.auto_vel......: %d\n" , maq_param.perfil.auto_vel);
    printf("maq_param.perfil.auto_acel.....: %f\n" , maq_param.perfil.auto_acel);
    printf("maq_param.perfil.auto_desacel..: %f\n" , maq_param.perfil.auto_desacel);
    printf("maq_param.perfil.manual_vel....: %d\n" , maq_param.perfil.manual_vel);
    printf("maq_param.perfil.manual_acel...: %f\n" , maq_param.perfil.manual_acel);
    printf("maq_param.perfil.manual_desacel: %f\n" , maq_param.perfil.manual_desacel);

    MaqGravarRegistrador(MAQ_REG_PERF_AUTO_VEL    ,                maq_param.perfil.auto_vel           );
    MaqGravarRegistrador(MAQ_REG_PERF_AUTO_ACEL   , (unsigned int)(maq_param.perfil.auto_acel     *100));
    MaqGravarRegistrador(MAQ_REG_PERF_AUTO_DESACEL, (unsigned int)(maq_param.perfil.auto_desacel  *100));
    MaqGravarRegistrador(MAQ_REG_PERF_MAN_VEL     ,                maq_param.perfil.manual_vel         );
    MaqGravarRegistrador(MAQ_REG_PERF_MAN_ACEL    , (unsigned int)(maq_param.perfil.manual_acel   *100));
    MaqGravarRegistrador(MAQ_REG_PERF_MAN_DESACEL , (unsigned int)(maq_param.perfil.manual_desacel*100));
    MaqGravarRegistrador(MAQ_REG_PERF_FATOR_LOW   , (rel_motor_perfil    ) & 0XFFFF);
    MaqGravarRegistrador(MAQ_REG_PERF_FATOR_HIGH  , (rel_motor_perfil>>16) & 0XFFFF);

    MaqConfigFlags((first ? 0 : MaqLerFlags()) | MAQ_MODO_PERF_SYNC);
    first = 0;
  }

  if(mask & MAQ_SYNC_ENCODER) {
    printf("maq_param.encoder.fator....: %f\n", maq_param.encoder.fator);
    printf("maq_param.encoder.perimetro: %d\n", maq_param.encoder.perimetro);
    printf("maq_param.encoder.precisao.: %d\n", maq_param.encoder.precisao);

    MaqGravarRegistrador(MAQ_REG_ENC_FATOR, (unsigned int)(maq_param.encoder.fator*1000));
    MaqGravarRegistrador(MAQ_REG_ENC_RESOL,                maq_param.encoder.precisao   );
    MaqGravarRegistrador(MAQ_REG_ENC_PERIM,                maq_param.encoder.perimetro  );
  }

  if(mask & MAQ_SYNC_CORTE) {
    printf("maq_param.corte.tam_faca...: %d\n", maq_param.corte.tam_faca);

    MaqGravarRegistrador(MAQ_REG_CRT_FACA, maq_param.corte.tam_faca);
  }

  return 1;
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
      ret = MaqSync(MAQ_SYNC_TODOS);
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
      "Erro no inversor",
      "Erro no desbobinador",
      "Erro de comunicação - Inversor",
      "Erro no Corte do Perfil",
//      "Baixa pressão de ar",
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
  ipc_msg.data.modbus_query.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
  ipc_msg.data.modbus_query.data.read_holding_registers.start = MAQ_REG_ERROS;
  ipc_msg.data.modbus_query.data.read_holding_registers.quant = 1;

  EnviarMB(&ipc_msg, &rp);

  if(rp.modbus_reply.ExceptionCode != MODBUS_EXCEPTION_NONE)
    return 1; // Erro de comunicacao

  erro = CONV_PCHAR_UINT16(rp.modbus_reply.reply.read_holding_registers.data);
  printf("Erro lido: %04x\n", erro);

  return erro << 1;
}

uint16_t MaqLerEstado(void)
{
  static uint16_t status = 0;

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
  ipc_msg.data.modbus_query.function_code = MODBUS_FC_READ_DISCRETE_INPUTS;
  ipc_msg.data.modbus_query.data.read_discrete_inputs.start = 0;
  ipc_msg.data.modbus_query.data.read_discrete_inputs.quant = 18;

  EnviarMB(&ipc_msg, &rp);

  if(rp.modbus_reply.ExceptionCode != MODBUS_EXCEPTION_NONE)
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
  ipc_msg.data.modbus_query.function_code = MODBUS_FC_READ_COILS;
  ipc_msg.data.modbus_query.data.read_coils.start = 0;
  ipc_msg.data.modbus_query.data.read_coils.quant = 16;

  EnviarMB(&ipc_msg, &rp);

  if(rp.modbus_reply.ExceptionCode != MODBUS_EXCEPTION_NONE)
    return 0; // Erro de comunicacao

  buf = rp.modbus_reply.reply.read_coils.data;
  val = ((uint32_t)(buf[1]) << 8) | buf[0];
  printf("output: %04x\n", val);

  return val;
}

uint16_t MaqLerProdQtd(void)
{
  static uint16_t qtd = 0;

  qtd = MaqLerRegistrador(MAQ_REG_PROD_QTD, qtd);
  printf("Restando %d pecas\n", qtd);

  return qtd;
}

void MaqConfigEstado(uint16_t estado)
{
  MaqGravarRegistrador(MAQ_REG_STATUS, estado);
}

void MaqConfigModo(uint16_t modo)
{
  uint16_t flags = MaqLerFlags();

  printf("modo = %d\n", modo);

  MaqConfigFlags((flags & ~MAQ_MODO_MASK) | modo);
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
  uint16_t flags = MaqLerFlags();
  flags |= MAQ_MODO_LIMPAR;
  MaqConfigFlags(flags);
}

void MaqCortar()
{
  uint16_t flags = MaqLerFlags();
  flags |= MAQ_MODO_CORTAR;
  MaqConfigFlags(flags);
}

void MaqPerfManual(uint16_t cmd)
{
  uint16_t flags = MaqLerFlags();

  flags &= ~MAQ_MODO_PERF_MASK;
  if     (cmd == PERF_AVANCA)
    flags |= MAQ_MODO_PERF_AVANCA;
  else if(cmd == PERF_RECUA)
    flags |= MAQ_MODO_PERF_RECUA;

  MaqConfigFlags(flags);
}

void MaqConfigProdQtd(uint16_t qtd)
{
  MaqGravarRegistrador(MAQ_REG_PROD_QTD, qtd);
}

void MaqConfigProdTam(uint16_t tam)
{
  MaqGravarRegistrador(MAQ_REG_PROD_TAM, tam);
}

void MaqConfigPerfil(struct strMaqParamPerfil pf)
{
  maq_param.perfil = pf;

  MaqSync(MAQ_SYNC_PERFIL);
}

struct strMaqParamPerfil MaqLerPerfil()
{
  return maq_param.perfil;
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

void MaqConfigCorte(struct strMaqParamCorte corte)
{
  maq_param.corte = corte;

  MaqSync(MAQ_SYNC_CORTE);
}

struct strMaqParamCorte MaqLerCorte()
{
  return maq_param.corte;
}
