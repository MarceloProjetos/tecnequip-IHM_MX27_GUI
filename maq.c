#include "defines.h"
#include "maq.h"

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
  unsigned long rel_motor_perfil = 0;

  printf("Sincronizando parametros da maquina:\n");

  if(mask & MAQ_SYNC_PERFIL) {
    if(maq_param.perfil.diam_rolo) {
      rel_motor_perfil = (unsigned long)((maq_param.perfil.fator * 1000000) / ((float)(3.14159 * maq_param.perfil.diam_rolo)));
    }

    printf("rel_motor_perfil...........: %ld\n", rel_motor_perfil);
    printf("maq_param.perfil.fator.....: %f\n" , maq_param.perfil.fator);
    printf("maq_param.perfil.diam_rolo.: %d\n" , maq_param.perfil.diam_rolo);
    printf("maq_param.perfil.vel_max...: %d\n" , maq_param.perfil.vel_max);
    printf("maq_param.perfil.vel_manual: %d\n" , maq_param.perfil.vel_manual);
    printf("maq_param.perfil.acel......: %f\n" , maq_param.perfil.acel);
    printf("maq_param.perfil.desacel...: %f\n" , maq_param.perfil.desacel);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PERF_ACEL;
    ipc_msg.data.modbus_query.data.write_single_register.val     = (unsigned int)(maq_param.perfil.acel*100);
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PERF_DESACEL;
    ipc_msg.data.modbus_query.data.write_single_register.val     = (unsigned int)(maq_param.perfil.desacel*100);
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PERF_VEL_MAX;
    ipc_msg.data.modbus_query.data.write_single_register.val     = maq_param.perfil.vel_max;
    IPCMQ_Main_Enviar(&ipc_msg);

    ipc_msg.mtype = IPCMQ_FNC_MODBUS;
    ipc_msg.fnc   = NULL;
    ipc_msg.res   = NULL;
    ipc_msg.data.modbus_query.function_code = MB_FC_WRITE_SINGLE_REGISTER;
    ipc_msg.data.modbus_query.data.write_single_register.address = MAQ_REG_PERF_VEL_MANUAL;
    ipc_msg.data.modbus_query.data.write_single_register.val     = maq_param.perfil.vel_manual;
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
  char *buf;
  uint16_t modo;
  struct strIPCMQ_Message ipc_msg;

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = NULL;
  ipc_msg.res   = NULL;
  ipc_msg.data.modbus_query.function_code = MB_FC_READ_HOLDING_REGISTERS;
  ipc_msg.data.modbus_query.data.read_holding_registers.start = MAQ_REG_STATUS;
  ipc_msg.data.modbus_query.data.read_holding_registers.quant = 1;

  IPCMQ_Main_Enviar(&ipc_msg);
  while(IPCMQ_Main_Receber(&ipc_msg, IPCMQ_FNC_MODBUS) < 0)
    usleep(100);

  buf = (char *)&(ipc_msg.data.modbus_reply.reply.read_holding_registers.val);
  modo = ((uint16_t)(buf[0]) << 8) | buf[1];

  printf("Modo lido: %04x\n", modo);
  return modo;
}

void retMaqLerProdQtd(void *dt, void *res)
{
  uint16_t            *qtd  = (uint16_t *)res;
  union uniIPCMQ_Data *data = (union uniIPCMQ_Data *)dt;
  char                *buf  = (char *)&(data->modbus_reply.reply.read_holding_registers.val);

  *qtd = ((uint16_t)(buf[2]) << 8) | buf[3];
}

uint16_t MaqLerProdQtd(void)
{
  volatile uint16_t qtd = 0xffff;
  struct strIPCMQ_Message ipc_msg;

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = retMaqLerProdQtd;
  ipc_msg.res   = (void *)&qtd;
  ipc_msg.data.modbus_query.function_code = MB_FC_READ_HOLDING_REGISTERS;
  ipc_msg.data.modbus_query.data.read_holding_registers.start = MAQ_REG_PROD_QTD;
  ipc_msg.data.modbus_query.data.read_holding_registers.quant = 1;

  IPCMQ_Main_Enviar(&ipc_msg);
  while(qtd == 0xffff) {
    IPC_Update();
    usleep(100);
  }

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
