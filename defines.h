#include <sys/msg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <ctype.h>

// Para que a funcao atof funcione corretamente
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "serial.h"
#include <comm.h>
#include <net/modbus.h>
#include <DB.h>

#include "maq.h"

/*** Definições gerais ***/

// Ativar a linha abaixo para rodar o programa em um PC
#define DEBUG_PC

// Ativar a linha abaixo para não conectar à POP
#define DEBUG_PC_NOETH

// Senha master do sistema usada quando não há conexão com o BD
#define SENHA_MASTER          "wFF9jghA.pg"
#define LEMBRETE_SENHA_MASTER "Senha default"

// Definição de tipo de log
#define LOG_TIPO_TODOS   0
#define LOG_TIPO_SISTEMA 1
#define LOG_TIPO_TAREFA  2
#define LOG_TIPO_ERRO    3
#define LOG_TIPO_CONFIG  4

// Definição de estado das tarefas
#define TRF_ESTADO_NOVA     0
#define TRF_ESTADO_PARCIAL  1
#define TRF_ESTADO_FINAL    2
#define TRF_ESTADO_REMOVIDA 3

// Definição de estado dos modelos
#define MOD_ESTADO_ATIVO    0
#define MOD_ESTADO_REMOVIDO 1

// Definição de origem da tarefa
#define TRF_ORIGEM_MANUAL 0
#define TRF_ORIGEM_ERP    1

// Definição de tipo de permissão
#define PERM_TIPO_INT  0
#define PERM_TIPO_BOOL 1

// Definição de permissões
#define PERM_ACESSO_CONFIG "acesso_config"
#define PERM_ACESSO_MANUT  "acesso_manut"
#define PERM_ACESSO_OPER   "acesso_oper_auto"
#define PERM_ACESSO_LOGS   "acesso_logs"

// Definições de máscaras para acertar entradas NF na tela de manutenção
#define MASK_SERVO_DIGIN 0x000C
#define MASK_GERAL_DIGIN 0x0815

// Indica se a funcao MaquinaEspera verifica se o estado é manual ou nao
#define CHECAR_ESPERA 0
#define CHECAR_MANUAL 1

// Numeracao das abas que contem a tela (ntbWorkArea)
#define NTB_ABA_HOME        0
#define NTB_ABA_MANUT       1
#define NTB_ABA_CONFIG      2
#define NTB_ABA_OPERAR      3
#define NTB_ABA_MANUAL      4
#define NTB_ABA_LOGS        5
#define NTB_ABA_TAREFA      7
#define NTB_ABA_MODBUS      8
#define NTB_ABA_DATA        9
#define NTB_ABA_EXECUTAR   10
#define NTB_ABA_VIRTUAL_KB 11
#define NTB_ABA_MESSAGEBOX 12
#define NTB_ABA_LOGIN      13

// Modos de corte da peça
#define MODO_CORTE_HIDR  0
#define MODO_CORTE_SERRA 1

/*** Fim das definições gerais ***/

int  WorkAreaGet (void);
void WorkAreaGoTo(int NewWorkArea);

/*** Definições para Comunicação entre Threads ***/

key_t fd_rd;
key_t fd_wr;

#define IPCMQ_MAX_BUFSIZE  MB_BUFFER_SIZE
#define IPCMQ_MESSAGE_SIZE (sizeof(struct strIPCMQ_Message) - sizeof(long))

#define IPCMQ_FNC_TEXT   0x01
#define IPCMQ_FNC_POWER  0x02
#define IPCMQ_FNC_MODBUS 0x03

struct strIPCMQ_Message {
  long mtype;
  void (*fnc)(void *, void *);
  void  *res;
  union uniIPCMQ_Data {
    struct {
      uint8_t status;
    } power;
    struct {
      uint32_t function_code;
      union MB_FCD_Data data;
    } modbus_query;
    struct MB_Reply modbus_reply;
    char text[IPCMQ_MAX_BUFSIZE];
  } data;
};

void IPCMQ_Main_Enviar    (struct strIPCMQ_Message *msg);
int  IPCMQ_Main_Receber   (struct strIPCMQ_Message *msg, int tipo);
void IPCMQ_Threads_Enviar (struct strIPCMQ_Message *msg);
int  IPCMQ_Threads_Receber(struct strIPCMQ_Message *msg);

/*** Fim das definições para Comunicação entre Threads ***/
