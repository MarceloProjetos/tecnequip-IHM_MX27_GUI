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

// Funcao que sincroniza a estrutura de parametros com o clp
int MaqSync(unsigned int mask)
{
  struct strIPCMQ_Message ipc_msg;
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

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PERF_AUTO_VEL;
    ipc_msg.data.modbus_query.data.write_single_register.val     = maq_param.perfil.auto_vel;
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

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PERF_FATOR_LOW;
    ipc_msg.data.modbus_query.data.write_single_register.val     = (rel_motor_perfil   ) & 0XFFFF;
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PERF_FATOR_HIGH;
    ipc_msg.data.modbus_query.data.write_single_register.val     = (rel_motor_perfil>>16) & 0XFFFF;
    IPCMQ_Main_Enviar(&ipc_msg);

    MaqInvParamSync();
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

  if(mask & MAQ_SYNC_CORTE) {
    printf("maq_param.corte.faca.......: %d\n", maq_param.corte.tam_faca);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_CRT_FACA;
    ipc_msg.data.modbus_query.data.write_single_register.val     = maq_param.corte.tam_faca;
    IPCMQ_Main_Enviar(&ipc_msg);
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

uint16_t MaqLerModo(void)
{
  static uint16_t modo = 0;

  modo = MaqLerRegistrador(MAQ_REG_FLAGS, modo);

  printf("Modo lido: %04x\n", modo);
  return (modo & 1) << 12;
}

char *MaqStrErro(uint16_t status)
{
  uint32_t i;
  char *msg_erro[] = {
      "Erro na comunicação",
      "Emergência",
      "Falta de fase",
      "Erro na unidade hidráulica",
      "Erro no inversor",
      "Erro no desbobinador",
      "Erro de comunicação - Inversor",
      "Erro de configuração",
//      "Baixa pressão de ar",
  };

  if(!status) // Sem erro, retorna string nula
    return NULL;

  for(i=0; !((status>>i)&1); i++); // Busca primeiro bit ligado

  if(i < ARRAY_SIZE(msg_erro)) {
    return msg_erro[i];
  } else {
    return "Erro indefinido!";
  }
}

uint16_t MaqLerStatus(void)
{
  uint16_t status = MaqLerRegistrador(MAQ_REG_STATUS, 0xffff);

  if(status == 0xffff) { // Erro de comunicacao
    printf("Erro de comunicacao\n");
    return 1;
  }

  status &= MAQ_MASK_STATUS;
  printf("status: %d\n", status);
  return status << 1;
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

uint16_t MaqLerProdQtd(void)
{
  static uint16_t qtd = 0;

  qtd = MaqLerRegistrador(MAQ_REG_PROD_QTD, qtd);

  printf("Restando %d pecas\n", qtd);
  return qtd;
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

void MaqConfigModo(uint16_t modo)
{
  struct strIPCMQ_Message ipc_msg;

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = NULL;
  ipc_msg.res   = NULL;
  ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
  ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_STATUS;
  ipc_msg.data.modbus_query.data.write_single_register.val = modo;

  IPCMQ_Main_Enviar(&ipc_msg);
}

void MaqConfigCMD(uint16_t cmd)
{
  MaqGravarRegistrador(MAQ_REG_CMD, cmd);
}

void MaqPerfCortar()
{
  MaqConfigCMD(MAQ_CMD_CORTAR);
}

void MaqPerfAvancar()
{
  MaqConfigCMD(MAQ_CMD_AVANCA);
}

void MaqPerfRecuar()
{
  MaqConfigCMD(MAQ_CMD_RECUA);
}

void MaqPerfParar()
{
  MaqConfigCMD(0);
}

void MaqLimparErro()
{
  MaqConfigCMD(MAQ_CMD_LIMPAR);
}

void MaqInvParamSync()
{
  MaqConfigCMD(MAQ_CMD_SYNC);
}

void MaqConfigProdQtd(uint16_t qtd)
{
  MaqGravarRegistrador(MAQ_REG_PROD_QTD, qtd);
}

void MaqConfigProdTam(uint16_t tam)
{
  MaqGravarRegistrador(MAQ_REG_PROD_TAM, tam);
}
