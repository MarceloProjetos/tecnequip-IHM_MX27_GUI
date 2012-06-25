#ifndef MODBUS_H
#define MODBUS_H

#include <string.h> // Para memset
//#define MB_DEBUG

#ifdef  __cplusplus
extern "C" {
#endif

// Codigos de excecoes
#define MODBUS_EXCEPTION_NONE                 0x00
#define MODBUS_EXCEPTION_ILLEGAL_FUNCTION     0x01
#define MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS 0x02
#define MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE   0x03
#define MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE 0x04

// Codigos de funcoes
#define MODBUS_FC_READ_COILS                 0x0001
#define MODBUS_FC_READ_DISCRETE_INPUTS       0x0002
#define MODBUS_FC_READ_HOLDING_REGISTERS     0x0003
#define MODBUS_FC_READ_INPUT_REGISTERS       0x0004
#define MODBUS_FC_WRITE_SINGLE_COIL          0x0005
#define MODBUS_FC_WRITE_SINGLE_REGISTER      0x0006
#define MODBUS_FC_READ_EXCEPTION_STATUS      0x0007
#define MODBUS_FC_WRITE_MULTIPLE_COILS       0x000F
#define MODBUS_FC_WRITE_MULTIPLE_REGISTERS   0x0010
#define MODBUS_FC_MASK_WRITE_REGISTER        0x0016
#define MODBUS_FC_RW_MULTIPLE_REGISTERS      0x0017
#define MODBUS_FC_READ_DEVICE_IDENTIFICATION 0x2B0E

// Modos de operacao do ModBus
#define MODBUS_MODE_MASTER     0x00
#define MODBUS_MODE_SLAVE      0x01
#define MODBUS_MODE_TCP_MASTER 0x10
#define MODBUS_MODE_TCP_SLAVE  0x11

// Ativando a linha abaixo o protocolo não vai usar o padrão ModBUS/TCP.
// Utilizado apenas para compatibilidade entre IHMs antigas, não usar!
//#define MODBUS_FORCE_NO_TCP_MODE

// Indica se operando em modo ModBUS/TCP
#define MODBUS_MODE_IS_TCP(mode) ((mode>>4) & 1UL)

#define MODBUS_TCP_OVERHEAD 6
#define MODBUS_BUFFER_SIZE  (260 + MODBUS_TCP_OVERHEAD)

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

struct MODBUS_PDU {
  unsigned char   Id;
  unsigned short int  TransactionId;
  unsigned short int  FunctionCode;
  unsigned char  *Data;
  unsigned int  ds;
};

// Estruturas com os dados formatados para cada um dos Function Codes existentes
struct MODBUS_FCD_read_coils {
unsigned short int start;
unsigned short int quant;
};

struct MODBUS_FCD_read_discrete_inputs {
unsigned short int start;
unsigned short int quant;
};

struct MODBUS_FCD_read_input_registers {
unsigned short int start;
unsigned short int quant;
};

struct MODBUS_FCD_read_holding_registers {
unsigned short int start;
unsigned short int quant;
};

struct MODBUS_FCD_write_single_coil {
unsigned short int output;
unsigned char  val;
};

struct MODBUS_FCD_write_single_register {
unsigned short int address;
unsigned short int val;
};

struct MODBUS_FCD_write_multiple_coils {
unsigned short int start;
unsigned short int quant;
unsigned char  size;
unsigned char *val;
};

struct MODBUS_FCD_write_multiple_registers {
unsigned short int start;
unsigned short int quant;
unsigned char  size;
unsigned char *val;
};

struct MODBUS_FCD_mask_write_register {
unsigned short int address;
unsigned short int and;
unsigned short int or;
};

struct MODBUS_FCD_rw_multiple_registers {
unsigned short int start_read;
unsigned short int quant_read;
unsigned short int start_write;
unsigned short int quant_write;
unsigned char  size;
unsigned char *val;
};

struct MODBUS_FCD_read_device_identification {
unsigned char id_code;
unsigned char object_id;
};

union MODBUS_FCD_Data {
  struct MODBUS_FCD_read_coils					read_coils;
  struct MODBUS_FCD_read_discrete_inputs		read_discrete_inputs;
  struct MODBUS_FCD_read_input_registers		read_input_registers;
  struct MODBUS_FCD_read_holding_registers		read_holding_registers;
  struct MODBUS_FCD_write_single_coil			write_single_coil;
  struct MODBUS_FCD_write_single_register		write_single_register;
  struct MODBUS_FCD_write_multiple_coils		write_multiple_coils;
  struct MODBUS_FCD_write_multiple_registers	write_multiple_registers;
  struct MODBUS_FCD_mask_write_register			mask_write_register;
  struct MODBUS_FCD_rw_multiple_registers		rw_multiple_registers;
  struct MODBUS_FCD_read_device_identification	read_device_identification;
};

#define MODBUS_REMAINING_BUFFER(s) (MODBUS_BUFFER_SIZE - sizeof(s) + sizeof(s.data))

struct MODBUS_Reply {
  unsigned char  Id;
  unsigned short int TransactionId;
  unsigned short int FunctionCode; // Para retorno do status
  unsigned char  ExceptionCode; // Para retornar o erro
  union {
    struct {
      unsigned char size;
      unsigned char data[1];
    } read_coils;

    struct {
      unsigned char size;
      unsigned char data[1];
    } read_discrete_inputs;

    struct {
      unsigned char size;
      unsigned char data[1];
    } read_holding_registers;

    struct {
      unsigned char size;
      unsigned char data[1];
    } read_input_registers;

    struct MODBUS_FCD_write_single_coil        write_single_coil;

    struct MODBUS_FCD_write_single_register    write_single_register;

    struct MODBUS_FCD_write_multiple_coils     write_multiple_coils;

    struct MODBUS_FCD_write_multiple_registers write_multiple_registers;

    struct MODBUS_FCD_mask_write_register      mask_write_register;

    struct {
      unsigned char size;
      unsigned char data[1];
    } rw_multiple_registers;

    struct {
      unsigned char status;
    } read_exception_status;

    struct {
      unsigned char id_code;
      unsigned char object_id;
      char  data[1];
    } read_device_identification;

    unsigned char reply_buffer[MODBUS_BUFFER_SIZE];
  } reply;
};

struct MODBUS_Device;

// Handler para funcoes que interpretam os Function Codes
#define MODBUS_HANDLER_FC_PTR(fnc) unsigned int (*fnc)(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
#define MODBUS_HANDLER_FC(fnc)     unsigned int   fnc (struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)

#define MODBUS_HANDLER_TX_PTR(fnc) unsigned int (*fnc)(unsigned char *data, unsigned int size)
#define MODBUS_HANDLER_TX(fnc)     unsigned int   fnc (unsigned char *data, unsigned int size)

struct MODBUS_Handler {
  unsigned short int FunctionCode;
  MODBUS_HANDLER_FC_PTR(FunctionHandler);
};

// Estrutura que representa o ModBus
struct MODBUS_Device {
  struct {
    unsigned char  Id;
    char  *VendorName;
    char  *ProductCode;
    char  *MajorMinorRevision;
    char  *VendorURL;
    char  *ProductName;
    char  *ModelName;
    char  *UserApplicationName;
  } identification;
  struct MODBUS_Handler *hl;
  unsigned int hl_size, mode;
  MODBUS_HANDLER_TX_PTR(TX); // Ponteiro para funcao que faz a transmissao do pacote
};

void            	Modbus_RTU_Init     (struct MODBUS_Device *dev);

struct MODBUS_PDU   Modbus_RTU_Validate (unsigned char *buf, unsigned int size, unsigned int mode_tcp);
struct MODBUS_Reply Modbus_RTU_Send     (struct MODBUS_Device *dev, unsigned short int TransactionId, unsigned short int FunctionCode, union MODBUS_FCD_Data *data);
unsigned int    	Modbus_RTU_SendReply(struct MODBUS_Device *dev, struct MODBUS_Reply *msg);
unsigned int    	Modbus_RTU_Receive  (struct MODBUS_Device *dev, struct MODBUS_PDU    msg);
struct MODBUS_Reply Modbus_RTU_ReceiveReply(struct MODBUS_Device *dev, struct MODBUS_PDU msg);

unsigned short int 	Modbus_RTU_CRC16(unsigned char *puchMsg, unsigned short int usDataLen);

#ifdef  __cplusplus
}
#endif

#endif
