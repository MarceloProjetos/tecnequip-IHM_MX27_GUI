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

// Funcao que sincroniza a estrutura de parametros com o clp
int MaqSync(unsigned int mask)
{
  struct strIPCMQ_Message ipc_msg;

  printf("Sincronizando parametros da maquina:\n");

  if(mask & MAQ_SYNC_PERFIL) {
    printf("maq_param.perfil.dinam_vel.....: %d\n" , maq_param.perfil.dinam_vel);
    printf("maq_param.perfil.estat_vel.....: %d\n" , maq_param.perfil.estat_vel);
    printf("maq_param.perfil.auto_acel.....: %f\n" , maq_param.perfil.auto_acel);
    printf("maq_param.perfil.auto_desacel..: %f\n" , maq_param.perfil.auto_desacel);
    printf("maq_param.perfil.manual_vel....: %d\n" , maq_param.perfil.manual_vel);
    printf("maq_param.perfil.manual_acel...: %f\n" , maq_param.perfil.manual_acel);
    printf("maq_param.perfil.manual_desacel: %f\n" , maq_param.perfil.manual_desacel);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PERF_DINAM_VEL;
    ipc_msg.data.modbus_query.data.write_single_register.val     = maq_param.perfil.dinam_vel;
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PERF_ESTAT_VEL;
    ipc_msg.data.modbus_query.data.write_single_register.val     = maq_param.perfil.estat_vel;
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PERF_AUTO_ACEL;
    ipc_msg.data.modbus_query.data.write_single_register.val     = (unsigned int)(maq_param.perfil.auto_acel*100);
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PERF_AUTO_DESACEL;
    ipc_msg.data.modbus_query.data.write_single_register.val     = (unsigned int)(maq_param.perfil.auto_desacel*100);
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PERF_MAN_VEL;
    ipc_msg.data.modbus_query.data.write_single_register.val     = maq_param.perfil.manual_vel;
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PERF_MAN_ACEL;
    ipc_msg.data.modbus_query.data.write_single_register.val     = (unsigned int)(maq_param.perfil.manual_acel*100);
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PERF_MAN_DESACEL;
    ipc_msg.data.modbus_query.data.write_single_register.val     = (unsigned int)(maq_param.perfil.manual_desacel*100);
    IPCMQ_Main_Enviar(&ipc_msg);

    MaqConfigFlags(MaqLerFlags() | MAQ_MODO_PERF_SYNC);
  }

  if(mask & MAQ_SYNC_ENCODER) {
    printf("maq_param.encoder.fator....: %f\n", maq_param.encoder.fator);
    printf("maq_param.encoder.perimetro: %d\n", maq_param.encoder.perimetro);
    printf("maq_param.encoder.precisao.: %d\n", maq_param.encoder.precisao);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_ENC_FATOR;
    ipc_msg.data.modbus_query.data.write_single_register.val     = (unsigned int)(maq_param.encoder.fator*1000);
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_ENC_RESOL;
    ipc_msg.data.modbus_query.data.write_single_register.val     = maq_param.encoder.precisao;
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_ENC_PERIM;
    ipc_msg.data.modbus_query.data.write_single_register.val     = maq_param.encoder.perimetro;
    IPCMQ_Main_Enviar(&ipc_msg);
  }

  if(mask & MAQ_SYNC_MESA) {
    printf("maq_param.mesa.curso.....: %d\n", maq_param.mesa.curso);
    printf("maq_param.mesa.auto_vel..: %f\n", maq_param.mesa.auto_vel);
    printf("maq_param.mesa.manual_vel: %f\n", maq_param.mesa.manual_vel);
    printf("maq_param.mesa.offset....: %f\n", maq_param.mesa.offset);
    printf("maq_param.mesa.tam_min...: %d\n", maq_param.mesa.tam_min);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_MESA_CURSO;
    ipc_msg.data.modbus_query.data.write_single_register.val     = maq_param.mesa.curso;
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_MESA_AUTO_VEL;
    ipc_msg.data.modbus_query.data.write_single_register.val     = maq_param.mesa.auto_vel;
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_MESA_MAN_VEL;
    ipc_msg.data.modbus_query.data.write_single_register.val     = maq_param.mesa.manual_vel;
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_MESA_OFFSET;
    ipc_msg.data.modbus_query.data.write_single_register.val     = maq_param.mesa.offset * 10;
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_MESA_TAM_MIN;
    ipc_msg.data.modbus_query.data.write_single_register.val     = maq_param.mesa.tam_min;
    IPCMQ_Main_Enviar(&ipc_msg);

    MaqConfigFlags(MaqLerFlags() | MAQ_MODO_MESA_SYNC);
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
//      ret = MaqSync(MAQ_SYNC_TODOS);
      // Devido a problemas de comunicação entre as threads antes de entrar em gtk_main,
      // a função de sincronismo deve ser executada somente após o login do usuário.
      ret = 1;
      }
    }

  close(fd);

  return ret;
}

struct strMaqReply {
  struct MB_Reply modbus_reply;
  uint32_t ready;
};

char *MaqStrErro(uint16_t erro)
{
  uint32_t i;
  char *msg_erro[] = {
      "Erro na comunicação",
      "Emergência Acionada",
      "Falta de fase",
      "Erro na unidade hidráulica",
      "Erro no inversor",
      "Erro no servomotor",
      "Erro de comunicação - Inversor",
      "Erro de comunicação - Servomotor",
      "Erro no Posicionamento",
      "Erro no Corte do Perfil",
      "Máquina Desativada",
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

void retMaqMB(void *dt, void *res)
{
  struct strMaqReply *rp = (struct strMaqReply *)res;

  rp->modbus_reply = ((union uniIPCMQ_Data *)dt)->modbus_reply;
  rp->ready = 1; // Recebida resposta
}

uint16_t MaqLerRegistrador(uint16_t reg)
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

  if(rp.modbus_reply.ExceptionCode != MB_EXCEPTION_NONE)
    return 0; // Erro de comunicacao

  return CONV_PCHAR_UINT16(rp.modbus_reply.reply.read_holding_registers.data);
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
  return MaqLerRegistrador(MAQ_REG_FLAGS);
}

uint16_t MaqLerEstado(void)
{
  uint16_t status;

  status = MaqLerRegistrador(MAQ_REG_STATUS);
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
  ipc_msg.data.modbus_query.data.read_discrete_inputs.quant = 18;

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

uint16_t MaqLerProdQtd(void)
{
  volatile uint16_t qtd;

  qtd = MaqLerRegistrador(MAQ_REG_PROD_QTD);
  printf("Restando %d pecas\n", qtd);

  return qtd;
}

uint16_t MaqLerNovoFator(void)
{
  uint16_t fator;

  fator = MaqLerRegistrador(MAQ_REG_ENC_FATOR_NOVO);
  printf("Novo fator: %.03f\n", (float)(fator)/1000);

  return fator;
}

uint16_t MaqLerNovoRelEnc(void)
{
  uint16_t fator;

  fator = MaqLerRegistrador(MAQ_REG_ENC_REL_NOVO);
  printf("Novo fator: %.03f\n", (float)(fator)/1000);

  return fator;
}

uint16_t MaqLerNovoTamMin(void)
{
  uint16_t tammin;

  tammin = MaqLerRegistrador(MAQ_REG_NOVO_TAM_MIN);
  printf("Novo Tamanho Minimo: %d\n", tammin);

  return tammin;
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
  // Se bloqueou a máquina, desliga todas as outras flags.
  MaqConfigFlags(liberar ? MaqLerFlags() | MAQ_MODO_LIBERA : 0);

  printf("maquina liberada ? %d\n", liberar);
}

void MaqLimparErro()
{
  uint16_t modo = MaqLerFlags(), erro = MaqLerErros();

  modo |= MAQ_MODO_LIMPAR;
  MaqConfigFlags(modo);

  if(erro == MAQ_ERRO_DESATIVADA)
    MaqLiberar(1);
}

void MaqCortar()
{
  uint16_t modo = MaqLerFlags();
  modo |= MAQ_MODO_CORTAR;
  MaqConfigFlags(modo);
}

void MaqMesaPosic(uint16_t pos)
{
  uint16_t modo;

  MaqConfigPosMesa(pos);

  modo = MaqLerFlags();
  modo |= MAQ_MODO_POSIC;
  MaqConfigFlags(modo);
}

void MaqCalcFatorPerfil()
{
  uint16_t modo = MaqLerFlags();
  modo |= MAQ_MODO_CALC_FATOR;
  MaqConfigFlags(modo);
}

void MaqCalcRelEnc()
{
  uint16_t modo = MaqLerFlags();
  modo |= MAQ_MODO_CALC_REL;
  MaqConfigFlags(modo);
}

void MaqCalcTamMin()
{
  uint16_t modo = MaqLerFlags();
  modo |= MAQ_MODO_CALC_MIN;
  MaqConfigFlags(modo);
}

uint16_t MaqOperando()
{
  return MaqLerEstado() & MAQ_STATUS_OPERANDO ? TRUE : FALSE;
}

void MaqPerfManual(uint16_t cmd)
{
  uint16_t modo = MaqLerFlags();

  modo &= ~MAQ_MODO_PERF_MASK;
  if     (cmd == PERF_AVANCA)
    modo |= MAQ_MODO_PERF_AVANCA;
  else if(cmd == PERF_RECUA)
    modo |= MAQ_MODO_PERF_RECUA;

  MaqConfigFlags(modo);
}

void MaqMesaManual(uint16_t cmd)
{
  uint16_t modo = MaqLerFlags();

  modo &= ~MAQ_MODO_MESA_MASK;
  if     (cmd == MESA_AVANCA)
    modo |= MAQ_MODO_MESA_AVANCA;
  else if(cmd == MESA_RECUA)
    modo |= MAQ_MODO_MESA_RECUA;

  MaqConfigFlags(modo);
}

void MaqConfigPosMesa(uint16_t pos)
{
  struct strIPCMQ_Message ipc_msg;

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = NULL;
  ipc_msg.res   = NULL;
  ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
  ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_MESA_POS;
  ipc_msg.data.modbus_query.data.write_single_register.val = pos;

  IPCMQ_Main_Enviar(&ipc_msg);
}

void MaqConfigProdQtd(uint16_t qtd)
{
  struct strIPCMQ_Message ipc_msg;

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = NULL;
  ipc_msg.res   = NULL;
  ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
  ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PROD_QTD;
  ipc_msg.data.modbus_query.data.write_single_register.val = qtd;

  IPCMQ_Main_Enviar(&ipc_msg);
}

void MaqConfigProdTam(uint16_t tam)
{
  struct strIPCMQ_Message ipc_msg;

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = NULL;
  ipc_msg.res   = NULL;
  ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
  ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PROD_TAM;
  ipc_msg.data.modbus_query.data.write_single_register.val = tam;

  IPCMQ_Main_Enviar(&ipc_msg);
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

void MaqConfigMesa(struct strMaqParamMesa mesa)
{
  maq_param.mesa = mesa;

  MaqSync(MAQ_SYNC_MESA);
}

struct strMaqParamMesa MaqLerMesa()
{
  return maq_param.mesa;
}
