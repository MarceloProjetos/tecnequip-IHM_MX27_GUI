#include "modbus_rtu.h"

#ifdef  __cplusplus
extern "C" {
#endif

/* Table of CRC values for high-order byte */
static unsigned char auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ;

/* Table of CRC values for low-order byte */
static unsigned char auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
} ;

void Modbus_RTU_Init(struct MODBUS_Device *dev)
{
  memset(dev, 0, sizeof(struct MODBUS_Device));

  // Inicializa todas as strings para vazio.
  dev->identification.VendorName          = \
  dev->identification.ProductCode         = \
  dev->identification.MajorMinorRevision  = \
  dev->identification.VendorURL           = \
  dev->identification.ProductName         = \
  dev->identification.ModelName           = \
  dev->identification.UserApplicationName = "";
}

struct MODBUS_PDU Modbus_RTU_Validate(unsigned char *buf, unsigned int size, unsigned int mode_tcp)
{
  unsigned int len, protocol, offset = 0, min_packet_size, valid_packet = 0;
  unsigned short int crc16_calc, crc16_real;
  struct MODBUS_PDU msg;

  memset(&msg, 0, sizeof(msg));

  // O codigo a seguir existe APENAS para efeito de compatibilidade entre a POP-7 e as IHMs
  // que comunicam pela rede mas sem usar a extensao TCP do protocolo ModBUS.
  // Deve deixar de existir em um momento futuro...
#ifdef MODBUS_FORCE_NO_TCP_MODE
  mode_tcp = 0;
#endif

  if(mode_tcp) {
    min_packet_size = MODBUS_TCP_OVERHEAD + 2; // MODBUS_TCP_OVERHEAD (Transaction + Protocol + Length) + Unit ID + Function Code
  } else {
    min_packet_size = 4; // Slave Address + Function Code + CRC16
  }

  if(size >= min_packet_size) {
    if(mode_tcp) {
      protocol = ((unsigned short int)(buf[2])<<8) | buf[3];
      len      = ((unsigned short int)(buf[4])<<8) | buf[5];
      // Tamanho do buffer deve ser o tamanho dos dados + 6 bytes de overhead do ModBUS/TCP
   	  valid_packet = (size == len+MODBUS_TCP_OVERHEAD) && !protocol;
    } else {
      crc16_calc = Modbus_RTU_CRC16(buf, size-2);
      crc16_real = (unsigned short int)(buf[size-2]) | (((unsigned short int)(buf[size-1]))<<8);

      valid_packet = (crc16_calc == crc16_real);
    }

    if(valid_packet) {
      if(mode_tcp) {
        msg.TransactionId = ((unsigned short int)(buf[offset+0])<<8) | buf[offset+1];
        offset += MODBUS_TCP_OVERHEAD; // Descarta o trecho relacionado ao adicional do TCP no ModBUS
      } else {
        size -= 2; // Descarta o CRC do buffer
        msg.TransactionId = 0;
      }

      msg.Id           = buf[offset++];
      msg.FunctionCode = buf[offset++];
      if(msg.FunctionCode == 0x08 || msg.FunctionCode == 0x2B)
        msg.FunctionCode = (msg.FunctionCode<<8) | buf[offset++];
      msg.Data         = buf  + offset;
      msg.ds           = size - offset; // total - tamanho utilizado
#ifdef MODBUS_DEBUG
    } else {
      if(mode_tcp) {
        printf("MODBUS: Pacote invalido: len = %d, protocol = %d, size = %d\n", len, protocol, size);
      } else {
        printf("MODBUS: Erro de CRC. Recebido 0x%x, Calculado: 0x%x\n", crc16_real, crc16_calc);
      }
  #endif
    }
#ifdef MODBUS_DEBUG
  } else {
    printf("MODBUS: Poucos dados. size: %d, min_packet_size = %d\n", size, min_packet_size);
#endif
  }

  return msg;
}

struct MODBUS_Reply Modbus_RTU_ReceiveReply(struct MODBUS_Device *dev, struct MODBUS_PDU msg)
{
  struct MODBUS_Reply r;
  unsigned char tmp;
  unsigned int i;

  if(msg.Data != NULL) {
    r.TransactionId = msg.TransactionId;
    r.FunctionCode  = msg.FunctionCode & 0xFF ? msg.FunctionCode & 0x7FFF : msg.FunctionCode & 0x7F;
    r.ExceptionCode = MODBUS_EXCEPTION_NONE;
    switch(msg.FunctionCode) {
    case MODBUS_FC_READ_COILS:
      r.reply.read_coils.size =  msg.Data[0];
      memcpy(r.reply.read_coils.data, &msg.Data[1], msg.Data[0]);
      break;

    case MODBUS_FC_READ_DISCRETE_INPUTS:
      r.reply.read_discrete_inputs.size =  msg.Data[0];
      memcpy(r.reply.read_discrete_inputs.data, &msg.Data[1], msg.Data[0]);
      break;

    case MODBUS_FC_READ_HOLDING_REGISTERS:
      r.reply.read_holding_registers.size =  msg.Data[0];
	  for(i=1; i<=msg.Data[0]; i+=2) {
		tmp = msg.Data[i];
		msg.Data[i  ] = msg.Data[i+1];
		msg.Data[i+1] = tmp;
	  }
	  memcpy(r.reply.read_holding_registers.data, &msg.Data[1], msg.Data[0]);
      break;

    case MODBUS_FC_READ_INPUT_REGISTERS:
      r.reply.read_input_registers.size =  msg.Data[0];
	  for(i=1; i<=msg.Data[0]; i+=2) {
		tmp = msg.Data[i];
		msg.Data[i  ] = msg.Data[i+1];
		msg.Data[i+1] = tmp;
	  }
      memcpy(r.reply.read_input_registers.data, &msg.Data[1], msg.Data[0]);
      break;

    case MODBUS_FC_WRITE_SINGLE_COIL:
      r.reply.write_single_coil.output = (msg.Data[0]<<8) | (msg.Data[1]);
      r.reply.write_single_coil.val    = msg.Data[2] ? 1 : 0;
      break;

    case MODBUS_FC_WRITE_SINGLE_REGISTER:
      r.reply.write_single_register.address = (msg.Data[0]<<8) | (msg.Data[1]);
      r.reply.write_single_register.val     = (msg.Data[2]<<8) | (msg.Data[3]);
      break;

    case MODBUS_FC_WRITE_MULTIPLE_COILS:
      r.reply.write_multiple_coils.start = (msg.Data[0]<<8) | (msg.Data[1]);
      r.reply.write_multiple_coils.quant = (msg.Data[2]<<8) | (msg.Data[3]);
      break;

    case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
      r.reply.write_multiple_registers.start = (msg.Data[0]<<8) | (msg.Data[1]);
      r.reply.write_multiple_registers.quant = (msg.Data[2]<<8) | (msg.Data[3]);
      break;

    case MODBUS_FC_MASK_WRITE_REGISTER:
      r.reply.mask_write_register.address = (msg.Data[0]<<8) | (msg.Data[1]);
      r.reply.mask_write_register.and     = (msg.Data[2]<<8) | (msg.Data[3]);
      r.reply.mask_write_register.or      = (msg.Data[4]<<8) | (msg.Data[5]);
      break;

    case MODBUS_FC_RW_MULTIPLE_REGISTERS:
      r.reply.rw_multiple_registers.size =  msg.Data[0];
	  for(i=1; i<=msg.Data[0]; i+=2) {
		tmp = msg.Data[i];
		msg.Data[i  ] = msg.Data[i+1];
		msg.Data[i+1] = tmp;
	  }
      memcpy(r.reply.rw_multiple_registers.data, &msg.Data[1], msg.Data[0]);
      break;

    case MODBUS_FC_READ_EXCEPTION_STATUS:
      r.reply.read_exception_status.status = msg.Data[0];
      break;

    case MODBUS_FC_READ_DEVICE_IDENTIFICATION:
      memcpy(r.reply.reply_buffer, &msg.Data[4], msg.ds - 4);
      break;

    default:
      r.ExceptionCode = msg.Data[0];
    }
  } else {
    r.FunctionCode  = msg.FunctionCode;
    r.ExceptionCode = MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE;
  }

  return r;
}

struct MODBUS_Reply Modbus_RTU_Send(struct MODBUS_Device *dev, unsigned short int TransactionId, unsigned short int FunctionCode, union MODBUS_FCD_Data *data)
{
  unsigned short int crc16;
  unsigned int size=0, i, mode_tcp = MODBUS_MODE_IS_TCP(dev->mode);
  struct MODBUS_Reply r;
  //struct MODBUS_PDU msg;
  unsigned char buf[MODBUS_BUFFER_SIZE];

  memset(r.reply.reply_buffer, 0, ARRAY_SIZE(r.reply.reply_buffer));
  r.ExceptionCode = MODBUS_EXCEPTION_ILLEGAL_FUNCTION; // Invalid Function Code

#ifdef MODBUS_FORCE_NO_TCP_MODE
  mode_tcp = 0;
#endif

  if(mode_tcp) {
    buf[size++] = (TransactionId>>8) & 0xFF;
    buf[size++] = (TransactionId>>0) & 0xFF;

    // Protocol
    buf[size++] = 0;
    buf[size++] = 0;

    // Length. Preenchido no final
    buf[size++] = 0;
    buf[size++] = 0;
  }

  buf[size++] = dev->identification.Id; // Id
  if(FunctionCode & 0xFF00) // Possui SubFunctionCode
    buf[size++] = (FunctionCode>>8) & 0xFF;
  buf[size++] = FunctionCode & 0xFF;

  switch(FunctionCode) {
    case MODBUS_FC_READ_COILS:
      buf[size++] = (data->read_coils.start>>8)&0xFF;
      buf[size++] = (data->read_coils.start   )&0xFF;
      buf[size++] = (data->read_coils.quant>>8)&0xFF;
      buf[size++] = (data->read_coils.quant   )&0xFF;
      break;

    case MODBUS_FC_READ_DISCRETE_INPUTS:
      buf[size++] = (data->read_discrete_inputs.start>>8)&0xFF;
      buf[size++] = (data->read_discrete_inputs.start   )&0xFF;
      buf[size++] = (data->read_discrete_inputs.quant>>8)&0xFF;
      buf[size++] = (data->read_discrete_inputs.quant   )&0xFF;
      break;

    case MODBUS_FC_READ_HOLDING_REGISTERS:
      buf[size++] = (data->read_holding_registers.start>>8)&0xFF;
      buf[size++] = (data->read_holding_registers.start   )&0xFF;
      buf[size++] = (data->read_holding_registers.quant>>8)&0xFF;
      buf[size++] = (data->read_holding_registers.quant   )&0xFF;
      break;

    case MODBUS_FC_READ_INPUT_REGISTERS:
      buf[size++] = (data->read_input_registers.start>>8)&0xFF;
      buf[size++] = (data->read_input_registers.start   )&0xFF;
      buf[size++] = (data->read_input_registers.quant>>8)&0xFF;
      buf[size++] = (data->read_input_registers.quant   )&0xFF;
      break;

    case MODBUS_FC_WRITE_SINGLE_COIL:
      buf[size++] = (data->write_single_coil.output>>8)&0xFF;
      buf[size++] = (data->write_single_coil.output   )&0xFF;
      buf[size++] =  data->write_single_coil.val ? 0xFF : 0x00;
      buf[size++] = 0x00;
      break;

    case MODBUS_FC_WRITE_SINGLE_REGISTER:
      buf[size++] = (data->write_single_register.address >> 8) & 0xFF;
      buf[size++] = (data->write_single_register.address     ) & 0xFF;
      buf[size++] = (data->write_single_register.val     >> 8) & 0xFF;
      buf[size++] = (data->write_single_register.val         ) & 0xFF;
      break;

    case MODBUS_FC_WRITE_MULTIPLE_COILS:
      buf[size++]  = (data->write_multiple_coils.start>>8)&0xFF;
      buf[size++]  = (data->write_multiple_coils.start   )&0xFF;
      buf[size++]  = (data->write_multiple_coils.quant>>8)&0xFF;
      buf[size++]  = (data->write_multiple_coils.quant   )&0xFF;
      buf[size++]  =  data->write_multiple_coils.size;
      for(i=0; i<data->write_multiple_coils.size; i++)
        buf[size++] = data->write_multiple_coils.val[i];
      break;

    case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
      buf[size++]  = (data->write_multiple_registers.start>>8)&0xFF;
      buf[size++]  = (data->write_multiple_registers.start   )&0xFF;
      buf[size++]  = (data->write_multiple_registers.quant>>8)&0xFF;
      buf[size++]  = (data->write_multiple_registers.quant   )&0xFF;
      buf[size++]  =  data->write_multiple_registers.size;
      for(i=0; i<data->write_multiple_registers.size; i+=2) {
        buf[size++] = data->write_multiple_registers.val[i+1];
        buf[size++] = data->write_multiple_registers.val[i  ];
	  }
      break;

    case MODBUS_FC_MASK_WRITE_REGISTER:
      buf[size++] = (data->mask_write_register.address >> 8) & 0xFF;
      buf[size++] = (data->mask_write_register.address     ) & 0xFF;
      buf[size++] = (data->mask_write_register.and     >> 8) & 0xFF;
      buf[size++] = (data->mask_write_register.and         ) & 0xFF;
      buf[size++] = (data->mask_write_register.or      >> 8) & 0xFF;
      buf[size++] = (data->mask_write_register.or          ) & 0xFF;
      break;

    case MODBUS_FC_RW_MULTIPLE_REGISTERS:
      buf[size++]  = (data->rw_multiple_registers.start_read  >> 8) & 0xFF;
      buf[size++]  = (data->rw_multiple_registers.start_read      ) & 0xFF;
      buf[size++]  = (data->rw_multiple_registers.quant_read  >> 8) & 0xFF;
      buf[size++]  = (data->rw_multiple_registers.quant_read      ) & 0xFF;
      buf[size++]  = (data->rw_multiple_registers.start_write >> 8) & 0xFF;
      buf[size++]  = (data->rw_multiple_registers.start_write     ) & 0xFF;
      buf[size++]  = (data->rw_multiple_registers.quant_write >> 8) & 0xFF;
      buf[size++]  = (data->rw_multiple_registers.quant_write     ) & 0xFF;
      buf[size++]  =  data->rw_multiple_registers.size;
      for(i=0; i<data->rw_multiple_registers.size; i+=2) {
        buf[size++] = data->rw_multiple_registers.val[i+1];
        buf[size++] = data->rw_multiple_registers.val[i  ];
	  }
      break;

    case MODBUS_FC_READ_EXCEPTION_STATUS: // Nao existem dados para este Function Code
      break;

    case MODBUS_FC_READ_DEVICE_IDENTIFICATION:
      buf[size++] = data->read_device_identification.id_code;
      buf[size++] = data->read_device_identification.object_id;
      break;

    default:
      r.FunctionCode = FunctionCode; // Invalid Function Code
      return r;
    }

  if(mode_tcp) {
    buf[5] = size - MODBUS_TCP_OVERHEAD;
  } else {
    crc16 = Modbus_RTU_CRC16(buf, size);

    buf[size++] = (crc16   ) & 0xFF;
    buf[size++] = (crc16>>8) & 0xFF;
  }

  size = (*dev->TX)(buf, size);

  if(size) {
    r = Modbus_RTU_ReceiveReply(dev, Modbus_RTU_Validate(buf, size, mode_tcp));
  } else {
    r.FunctionCode  = FunctionCode;
    r.ExceptionCode = MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE;
  }

  return r;
}

unsigned int Modbus_RTU_SendReply(struct MODBUS_Device *dev, struct MODBUS_Reply *msg)
{
  unsigned short int crc16;
  unsigned int i, size = 0, mode_tcp = MODBUS_MODE_IS_TCP(dev->mode);
  unsigned char buf[MODBUS_BUFFER_SIZE];

#ifdef MODBUS_FORCE_NO_TCP_MODE
  mode_tcp = 0;
#endif

  if(mode_tcp) {
    buf[size++] = (msg->TransactionId>>8) & 0xFF;
    buf[size++] = (msg->TransactionId>>0) & 0xFF;

    // Protocol
    buf[size++] = 0;
    buf[size++] = 0;

    // Length. Preenchido no final
    buf[size++] = 0;
    buf[size++] = 0;
  }

  buf[size++] = msg->Id;

  if(msg->ExceptionCode == MODBUS_EXCEPTION_NONE) {
    if(msg->FunctionCode & 0xFF00) // Possui SubFunctionCode
      buf[size++] = (msg->FunctionCode>>8) & 0xFF;
    buf[size++] = msg->FunctionCode & 0xFF;

    switch(msg->FunctionCode) {
    case MODBUS_FC_READ_COILS:
      buf[size++] = msg->reply.read_coils.size;
      for(i=0; i<msg->reply.read_coils.size; i++)
        buf[size++] = msg->reply.read_coils.data[i];
      break;

    case MODBUS_FC_READ_DISCRETE_INPUTS:
      buf[size++] = msg->reply.read_discrete_inputs.size;
      for(i=0; i<msg->reply.read_discrete_inputs.size; i++)
        buf[size++] = msg->reply.read_discrete_inputs.data[i];
      break;

    case MODBUS_FC_READ_HOLDING_REGISTERS:
      buf[size++] = msg->reply.read_holding_registers.size;
      for(i=0; i<msg->reply.read_holding_registers.size; i+=2) {
        buf[size++] = msg->reply.read_holding_registers.data[i+1];
        buf[size++] = msg->reply.read_holding_registers.data[i  ];
      }
      break;

    case MODBUS_FC_READ_INPUT_REGISTERS:
      buf[size++] = msg->reply.read_input_registers.size;
      for(i=0; i<msg->reply.read_input_registers.size; i+=2) {
        buf[size++] = msg->reply.read_input_registers.data[i+1];
        buf[size++] = msg->reply.read_input_registers.data[i  ];
      }
      break;

    case MODBUS_FC_WRITE_SINGLE_COIL:
      buf[size++] = (msg->reply.write_single_coil.output>>8) & 0xFF;
      buf[size++] = (msg->reply.write_single_coil.output   ) & 0xFF;
      buf[size++] = msg->reply.write_single_coil.val ? 0xFF : 0x00;
      buf[size++] = 0x00;
      break;

    case MODBUS_FC_WRITE_SINGLE_REGISTER:
      buf[size++] = (msg->reply.write_single_register.address >> 8) & 0xFF;
      buf[size++] = (msg->reply.write_single_register.address     ) & 0xFF;
      buf[size++] = (msg->reply.write_single_register.val     >> 8) & 0xFF;
      buf[size++] = (msg->reply.write_single_register.val         ) & 0xFF;
      break;

    case MODBUS_FC_WRITE_MULTIPLE_COILS:
      buf[size++] = (msg->reply.write_multiple_coils.start>>8) & 0xFF;
      buf[size++] = (msg->reply.write_multiple_coils.start   ) & 0xFF;
      buf[size++] = (msg->reply.write_multiple_coils.quant>>8) & 0xFF;
      buf[size++] = (msg->reply.write_multiple_coils.quant   ) & 0xFF;
      break;

    case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
      buf[size++]  = (msg->reply.write_multiple_registers.start>>8)&0xFF;
      buf[size++]  = (msg->reply.write_multiple_registers.start   )&0xFF;
      buf[size++]  = (msg->reply.write_multiple_registers.quant>>8)&0xFF;
      buf[size++]  = (msg->reply.write_multiple_registers.quant   )&0xFF;
      break;

    case MODBUS_FC_MASK_WRITE_REGISTER:
      buf[size++] = (msg->reply.mask_write_register.address >> 8) & 0xFF;
      buf[size++] = (msg->reply.mask_write_register.address     ) & 0xFF;
      buf[size++] = (msg->reply.mask_write_register.and     >> 8) & 0xFF;
      buf[size++] = (msg->reply.mask_write_register.and         ) & 0xFF;
      buf[size++] = (msg->reply.mask_write_register.or      >> 8) & 0xFF;
      buf[size++] = (msg->reply.mask_write_register.or          ) & 0xFF;
      break;

    case MODBUS_FC_RW_MULTIPLE_REGISTERS:
      buf[size++] = msg->reply.rw_multiple_registers.size;
      for(i=0; i<msg->reply.rw_multiple_registers.size; i+=2) {
        buf[size++] = msg->reply.rw_multiple_registers.data[i+1];
        buf[size++] = msg->reply.rw_multiple_registers.data[i  ];
      }
      break;

    case MODBUS_FC_READ_EXCEPTION_STATUS:
      buf[size++] = msg->reply.read_exception_status.status;
      break;

    case MODBUS_FC_READ_DEVICE_IDENTIFICATION:
      buf[size++] = msg->reply.read_device_identification.id_code;
      buf[size++] = 0x82; // ??? - Passando como stream e individual a nivel regular
      buf[size++] = 0x00;
      buf[size++] = 0x00;
      buf[size++] = 1;
      buf[size++] = msg->reply.read_device_identification.object_id;
      buf[size++] = strlen(msg->reply.read_device_identification.data);
      strcpy((char *)(buf+size), (char *)msg->reply.read_device_identification.data);
      size += strlen(msg->reply.read_device_identification.data);
      break;
    }
  } else {
    buf[size++] = ((msg->FunctionCode & 0xFF00) ? msg->FunctionCode>>8 : msg->FunctionCode) | 0x80;
    buf[size++] = msg->ExceptionCode;
  }

  if(mode_tcp) {
    buf[5] = size - MODBUS_TCP_OVERHEAD;
  } else {
    crc16 = Modbus_RTU_CRC16(buf, size);

    buf[size++] = (crc16   ) & 0xFF;
    buf[size++] = (crc16>>8) & 0xFF;
  }

  (*dev->TX)(buf, size);

  return 0;
}

unsigned int Modbus_RTU_Receive(struct MODBUS_Device *dev, struct MODBUS_PDU msg)
{
  unsigned int i, fc = 0;
  unsigned char tmp;
  struct MODBUS_Reply reply;
  union MODBUS_FCD_Data data;

  // Erro de CRC. Nao envia resposta pois pode ter sido, inclusive, ruido na linha.
  if(msg.Data == NULL)
    return MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE;
  else if(msg.Id != dev->identification.Id && !MODBUS_MODE_IS_TCP(dev->mode))
    return MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE;

  reply.TransactionId = msg.TransactionId;
  reply.Id            = msg.Id;
  reply.FunctionCode  = msg.FunctionCode;
  reply.ExceptionCode = MODBUS_EXCEPTION_ILLEGAL_FUNCTION;

  for(fc=0; fc<dev->hl_size; fc++) {
    if(msg.FunctionCode == dev->hl[fc].FunctionCode) {
      reply.ExceptionCode = MODBUS_EXCEPTION_NONE;
      switch(msg.FunctionCode) {
      case MODBUS_FC_READ_COILS:
        data.read_coils.start = (msg.Data[0]<<8) | (msg.Data[1]);
        data.read_coils.quant = (msg.Data[2]<<8) | (msg.Data[3]);
        if(!data.read_discrete_inputs.quant)
          reply.ExceptionCode = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
        break;

      case MODBUS_FC_READ_DISCRETE_INPUTS:
        data.read_discrete_inputs.start = (msg.Data[0]<<8) | (msg.Data[1]);
        data.read_discrete_inputs.quant = (msg.Data[2]<<8) | (msg.Data[3]);
        if(!data.read_discrete_inputs.quant)
          reply.ExceptionCode = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
        break;

      case MODBUS_FC_READ_HOLDING_REGISTERS:
        data.read_holding_registers.start = (msg.Data[0]<<8) | (msg.Data[1]);
        data.read_holding_registers.quant = (msg.Data[2]<<8) | (msg.Data[3]);
        if(!data.read_holding_registers.quant || data.read_holding_registers.quant > 125)
          reply.ExceptionCode = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
        break;

      case MODBUS_FC_READ_INPUT_REGISTERS:
        data.read_input_registers.start = (msg.Data[0]<<8) | (msg.Data[1]);
        data.read_input_registers.quant = (msg.Data[2]<<8) | (msg.Data[3]);
        if(!data.read_input_registers.quant || data.read_input_registers.quant > 125)
          reply.ExceptionCode = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
        break;

      case MODBUS_FC_WRITE_SINGLE_COIL:
        data.write_single_coil.output = (msg.Data[0]<<8) | (msg.Data[1]);
        data.write_single_coil.val    = msg.Data[2] ? 1 : 0;
        if((msg.Data[2] != 0xFF && msg.Data[2] != 0x00) || msg.Data[3] != 0x00)
          reply.ExceptionCode = MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE;
        break;

      case MODBUS_FC_WRITE_SINGLE_REGISTER:
        data.write_single_register.address = (msg.Data[0]<<8) | (msg.Data[1]);
        data.write_single_register.val     = (msg.Data[2]<<8) | (msg.Data[3]);
        break;

      case MODBUS_FC_WRITE_MULTIPLE_COILS:
        data.write_multiple_coils.start = (msg.Data[0]<<8) | (msg.Data[1]);
        data.write_multiple_coils.quant = (msg.Data[2]<<8) | (msg.Data[3]);
        data.write_multiple_coils.size  =  msg.Data[4];
        data.write_multiple_coils.val   =  msg.Data+5;
        break;

      case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
        data.write_multiple_registers.start = (msg.Data[0]<<8) | (msg.Data[1]);
        data.write_multiple_registers.quant = (msg.Data[2]<<8) | (msg.Data[3]);
        data.write_multiple_registers.size  =  msg.Data[4];
        data.write_multiple_registers.val   =  msg.Data+5;
		for(i=0; i<data.write_multiple_registers.size; i+=2) {
			tmp = data.write_multiple_registers.val[i];
			data.write_multiple_registers.val[i  ] = data.write_multiple_registers.val[i+1];
			data.write_multiple_registers.val[i+1] = tmp;
		}
        break;

      case MODBUS_FC_MASK_WRITE_REGISTER:
        data.mask_write_register.address = (msg.Data[0]<<8) | (msg.Data[1]);
        data.mask_write_register.and     = (msg.Data[2]<<8) | (msg.Data[3]);
        data.mask_write_register.or      = (msg.Data[4]<<8) | (msg.Data[5]);
        data.mask_write_register.or &= ~data.mask_write_register.and;
        break;

      case MODBUS_FC_RW_MULTIPLE_REGISTERS:
        data.rw_multiple_registers.start_read  = (msg.Data[0]<<8) | (msg.Data[1]);
        data.rw_multiple_registers.quant_read  = (msg.Data[2]<<8) | (msg.Data[3]);
        data.rw_multiple_registers.start_write = (msg.Data[4]<<8) | (msg.Data[5]);
        data.rw_multiple_registers.quant_write = (msg.Data[6]<<8) | (msg.Data[7]);
        data.rw_multiple_registers.size        =  msg.Data[8];
        data.rw_multiple_registers.val         =  msg.Data+9;
		for(i=0; i<data.rw_multiple_registers.size; i+=2) {
			tmp = data.rw_multiple_registers.val[i];
			data.rw_multiple_registers.val[i  ] = data.rw_multiple_registers.val[i+1];
			data.rw_multiple_registers.val[i+1] = tmp;
		}
        break;

      case MODBUS_FC_READ_EXCEPTION_STATUS: // Nao existem dados para este Function Code
        break;

      default:
        reply.ExceptionCode = MODBUS_EXCEPTION_ILLEGAL_FUNCTION;
      }

      if(reply.ExceptionCode == MODBUS_EXCEPTION_NONE) {
        reply.ExceptionCode = (*dev->hl[fc].FunctionHandler)(dev, &data, &reply);
        reply.FunctionCode = msg.FunctionCode; // Atualiza Function Code com codigo enviado.
        reply.Id = msg.Id; // Atualiza Id com o valor recebido do handler.
      }

      break; // Encontrou handler, para a busca.
    }
  }

  // Funcao nao suportada pelos handlers, verifica se manipulamos diretamente.
  if(reply.ExceptionCode == MODBUS_EXCEPTION_ILLEGAL_FUNCTION) {
    switch(msg.FunctionCode) {
    case MODBUS_FC_READ_DEVICE_IDENTIFICATION:
      reply.ExceptionCode = MODBUS_EXCEPTION_NONE;
      reply.reply.read_device_identification.id_code   = msg.Data[0];
      reply.reply.read_device_identification.object_id = msg.Data[1];

      switch(reply.reply.read_device_identification.object_id) {
      case 0x00:
        strcpy(reply.reply.read_device_identification.data, dev->identification.VendorName);
        break;

      case 0x01:
        strcpy(reply.reply.read_device_identification.data, dev->identification.ProductCode);
        break;

      case 0x02:
        strcpy(reply.reply.read_device_identification.data, dev->identification.MajorMinorRevision);
        break;

      case 0x03:
        strcpy(reply.reply.read_device_identification.data, dev->identification.VendorURL);
        break;

      case 0x04:
        strcpy(reply.reply.read_device_identification.data, dev->identification.ProductName);
        break;

      case 0x05:
        strcpy(reply.reply.read_device_identification.data, dev->identification.ModelName);
        break;

      case 0x06:
        strcpy(reply.reply.read_device_identification.data, dev->identification.UserApplicationName);
        break;
      }
      break;
    }
  }

  Modbus_RTU_SendReply(dev, &reply);

  return reply.ExceptionCode; // Invalid Function Code
}

unsigned short int Modbus_RTU_CRC16(unsigned char *puchMsg, unsigned short int usDataLen)
{
  unsigned char uchCRCHi = 0xFF ; /* high byte of CRC initialized */
  unsigned char uchCRCLo = 0xFF ; /* low byte of CRC initialized */
  unsigned int uIndex ; /* will index into CRC lookup table */

  while (usDataLen--) /* pass through message buffer */
    {
    uIndex = uchCRCHi ^ *puchMsg++ ; /* calculate the CRC */
    uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
    uchCRCLo = auchCRCLo[uIndex] ;
    }

  //return (uchCRCHi << 8 | uchCRCLo) ;
  return (uchCRCLo << 8 | uchCRCHi) ;
}

#ifdef  __cplusplus
}
#endif
