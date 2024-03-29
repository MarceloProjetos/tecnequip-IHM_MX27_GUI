#include <sys/msg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <errno.h>
#include <iconv.h>
#include <sys/ioctl.h>
#include <time.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>

// Para que a funcao atof funcione corretamente
#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "modbus_rtu.h"
#include <DB.h>
#include <crypt.h>

#include "io.h"
#include "maq.h"
#include "version.h"

typedef uint8_t u8;
typedef uint16_t u16;
#include "twl4030-madc.h"

/*** Definições gerais ***/

// Ativar a linha abaixo para desativar a comunicação pela Ethernet
//#define DEBUG_PC_NOETH

// Ativar a linha abaixo para rodar em modo de teste
//#define DEBUG_MODO_TESTE

// Ativar a linha abaixo para desativar a integracao das IHMs com o SQL Server
#define DISABLE_SQL_SERVER

// As definicoes a seguir somente serao consideradas quando a integracao
// das IHMs com o SQL Server estiver ativa.
#ifndef DISABLE_SQL_SERVER

// Ativar a linha abaixo para nao permitir a producao manual
//#define MANUAL_PRODUCTION_NOT_ALLOWED

#endif

// Definicao da Linha/Maquina que este programa vai rodar
// Definicao de mensagem a exibir quando nao existe erro ativo
#ifndef DEBUG_MODO_TESTE

#define MAQ_LINHA   (MaqConfigCurrent->Line   )
#define MAQ_MAQUINA (MaqConfigCurrent->Machine)

#ifndef DEBUG_PC_NOETH
#define MSG_SEM_ERRO (MaqConfigCurrent == &MaqConfigDefault ? "Máquina Não Identificada" : "Sem Erros")
#else
#define MSG_SEM_ERRO "Máquina Desconectada"
#endif

#else

#define MAQ_LINHA   "TESTE"
#define MAQ_MAQUINA "TESTE"

#ifndef DEBUG_PC_NOETH
#define MSG_SEM_ERRO "Máquina em Modo de Teste"
#else
#define MSG_SEM_ERRO "Máquina em Modo de Teste - Desconectada"
#endif

#endif

// Senha master do sistema usada quando não há conexão com o BD
#define SENHA_MASTER          "wFF9jghA.pg"
#define LEMBRETE_SENHA_MASTER "Senha default"

// Tamanho maximo do nome do usuario
#define MAX_USERNAME_SIZE 20

// Definição de tipo de log
#define LOG_TIPO_TODOS   0
#define LOG_TIPO_SISTEMA 1
#define LOG_TIPO_TAREFA  2
#define LOG_TIPO_ERRO    3
#define LOG_TIPO_CONFIG  4

// Definição de estados da máquina
#define MAQ_STATUS_INDETERMINADO 0
#define MAQ_STATUS_PARADA        1
#define MAQ_STATUS_PRODUZINDO    2
#define MAQ_STATUS_MANUAL        3
#define MAQ_STATUS_SETUP         4
#define MAQ_STATUS_MANUTENCAO    5
#define MAQ_STATUS_DESENERGIZADA 6
#define MAQ_STATUS_DESLIGADA     7

// Tempo máximo de inatividade permitido
#define MAQ_IDLE_TIMEOUT 300

// Definicao de mascara de erro para Maquina Desativada
#define MAQ_ERRO_DESATIVADA 0x0400

// Definição de estado das tarefas
#define TRF_ESTADO_NOVA     0
#define TRF_ESTADO_PARCIAL  1
#define TRF_ESTADO_FINAL    2
#define TRF_ESTADO_REMOVIDA 3

// Definição de estado dos modelos
#define MOD_ESTADO_ATIVO    0
#define MOD_ESTADO_REMOVIDO 1

// Definição de origem da tarefa
#define TRF_ORIGEM_ERP    0
#define TRF_ORIGEM_MANUAL 1

// Definição de tipo de permissão
#define PERM_TIPO_INT  0
#define PERM_TIPO_BOOL 1

// Definição de permissões
#define PERM_ACESSO_CONFIG   0
#define PERM_ACESSO_MANUT    1
#define PERM_ACESSO_OPER     2
#define PERM_ACESSO_LOGS     3
#define PERM_CADASTRO_MANUAL 4

#define PERM_NONE  0
#define PERM_READ  1
#define PERM_WRITE 2

// Definições de máscaras para acertar entradas NF na tela de manutenção
#define MASK_SERVO_DIGIN 0x000C
#define MASK_GERAL_DIGIN 0x0815

// Indica se a funcao MaquinaEspera verifica se o estado é manual ou nao
#define CHECAR_ESPERA 0
#define CHECAR_MANUAL 1

// Numeracao das abas que contem a tela (ntbWorkArea)
#define NTB_ABA_HOME            0
#define NTB_ABA_MANUT           1
#define NTB_ABA_CONFIG          2
#define NTB_ABA_OPERAR          3
#define NTB_ABA_MANUAL          4
#define NTB_ABA_LOGS            5
#define NTB_ABA_TAREFA          7
#define NTB_ABA_MODBUS          8
#define NTB_ABA_DATA            9
#define NTB_ABA_EXECUTAR       10
#define NTB_ABA_VIRTUAL_KB     11
#define NTB_ABA_MESSAGEBOX     12
#define NTB_ABA_LOGIN          13
#define NTB_ABA_DADOS_PEDIDO   14
#define NTB_ABA_INDETERMINADO  15
#define NTB_ABA_ESPERA         16
#define NTB_ABA_MUDAR_SENHA    17
#define NTB_ABA_CALC_FATOR     18
#define NTB_ABA_POWERDOWN      19
#define NTB_ABA_HOME_BANHO     20
#define NTB_ABA_HOME_PRENSA    21
#define NTB_ABA_PROG_PARAM     22
#define NTB_ABA_PRENSA_PROD    23
#define NTB_ABA_PRENSA_CADPROG 24
#define NTB_ABA_PRENSAPF_PROD  25 // Prensa de Passo Fixo - diferente da prensa normal, onde se programam os passos
#define NTB_ABA_MATERIAL       26
#define NTB_ABA_MATERIAL_ADD   27
#define NTB_ABA_REGISTRA_PECA  28
#define NTB_ABA_MOVIMENTAR     29
#define NTB_ABA_NONE           99

#define NTB_ABA_CONFIG_DIAGONAL 1
#define NTB_ABA_CONFIG_COLN     2
#define NTB_ABA_CONFIG_PRENSA   3

// Modos de corte da peça
#define MODO_CORTE_HIDR  0
#define MODO_CORTE_SERRA 1

// Definições para cast de variáveis, evitando problemas com alinhamento.
#define CONV_PCHAR_UINT16(data) (((uint16_t)(*(data+1))<<8) | (uint16_t)(*data))

// defines que permitem selecionar o ponto de referencia para insercao da imagem
#define LOADPB_REFPOS_UP      0x00
#define LOADPB_REFPOS_DOWN    0x01
#define LOADPB_REFPOS_LEFT    0x00
#define LOADPB_REFPOS_RIGHT   0x02
#define LOADPB_REFPOS_DEFAULT (LOADPB_REFPOS_UP | LOADPB_REFPOS_LEFT)

/*** Fim das definições gerais ***/

// Funções e Estruturas relacionadas com o estado da IHM

#define BOARD_AD_TEMP  0
#define BOARD_AD_VBAT 12
#define BOARD_AD_VIN  10

#define BOARD_BATT_PRECHARGE 0
#define BOARD_BATT_CHARGING  2
#define BOARD_BATT_FULL      1
#define BOARD_BATT_ERROR     3

typedef struct strBoardStatus BoardStatus;

struct strBoardStatus {
  float ExternalVoltage;
  float BatteryVoltage;
  float Temperature;

  int BatteryState;
  int HasExternalPower;

  int    dev;
  struct twl4030_madc_user_parms *par;
};

// Variavel indicando que houve atividade
extern int atividade;

int  WorkAreaGet (void);
void WorkAreaGoPrevious(void);
void WorkAreaGoTo(int NewWorkArea);

// Funcao utilizada para registrar um evento
void Log(char *evento, int tipo);

void SetMaqStatus(unsigned int NewStatus);

#define MSSQL_DONT_SYNC   0x00
#define MSSQL_USE_SYNC    0x01
#define MSSQL_DONT_REPORT 0x02

struct strDB * MSSQL_Connect(void);
int            MSSQL_Execute(int nres, char *sql, unsigned int flags);
void           MSSQL_Close  (void);

char         * MSSQL_UTF2ISO(char *data);
char         * MSSQL_DateFromTimeT(time_t t, char *data);
char         * MSSQL_GetData(int nres, unsigned int pos);

/*** Definições para Comunicação entre Threads ***/

key_t fd_rd;
key_t fd_wr;

#define IPCMQ_MAX_BUFSIZE  MODBUS_BUFFER_SIZE
#define IPCMQ_MESSAGE_SIZE (sizeof(struct strIPCMQ_Message) - sizeof(long))

#define IPCMQ_FNC_TEXT   0x01
#define IPCMQ_FNC_POWER  0x02
#define IPCMQ_FNC_MODBUS 0x03
#define IPCMQ_FNC_STATUS 0x04
#define IPCMQ_FNC_BATT   0x05

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
      union MODBUS_FCD_Data data;
    } modbus_query;
    struct MODBUS_Reply modbus_reply;
    char text[IPCMQ_MAX_BUFSIZE];
    BoardStatus bs;
    int batt_level;
  } data;
};

void IPCMQ_Main_Enviar    (struct strIPCMQ_Message *msg);
int  IPCMQ_Main_Receber   (struct strIPCMQ_Message *msg, int tipo);
void IPCMQ_Threads_Enviar (struct strIPCMQ_Message *msg);
int  IPCMQ_Threads_Receber(struct strIPCMQ_Message *msg);

/*** Fim das definições para Comunicação entre Threads ***/

// Variavel indicando que a tela de desligamento esta ativada
extern uint32_t OnPowerDown;

// Variavel com a hora de desligamento anterior
extern time_t system_Shutdown;

// Ultimo material selecionado como em uso
extern long   system_last_in_use;

// Prototipos de Funcoes
void AbrirData  (GtkEntry *entry, GCallback cb);
int  GetUserPerm(unsigned int perm);
void ShowMessageBox(char *msg, int modoErro);

void cbIconPress (GtkEntry *entry, GtkEntryIconPosition icon_pos, GdkEvent *event, gpointer user_data);

/*** Definicoes de Materiais ***/

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))
#define CTOI(x) (min(max(0, x - '0'), 9))

/*** Estruturas de materiais ***/
// Enumeracao com os tipos de materiais
enum enumTipoEstoque {
	enumTipoEstoque_Revenda              =  0,
	enumTipoEstoque_MateriaPrima         =  1,
	enumTipoEstoque_Embalagem            =  2,
	enumTipoEstoque_ProdutoEmProcesso    =  3,
	enumTipoEstoque_ProdutoAcabado       =  4,
	enumTipoEstoque_Subproduto           =  5,
	enumTipoEstoque_ProdutoIntermediario =  6,
	enumTipoEstoque_MaterialUsoConsumo   =  7,
	enumTipoEstoque_AtivoImobilizado     =  8,
	enumTipoEstoque_Servicos             =  9,
	enumTipoEstoque_OutrosInsumos        = 10,
	enumTipoEstoque_Outros               = 99,
};

struct strMaterial {
	// ID do material no banco de dados
	int  id;

	// Codigo do material
	char codigo[21]; // Campo no banco: VARCHAR(20)
	enum enumTipoEstoque tipo; // Tipo de material

	// Codigo do produto
	char produto[101]; // Campo no banco: VARCHAR(100)

	// Descricao do produto
	char descricao[101]; // Campo no banco: VARCHAR(100)

	// Flag que indica se o material esta selecionado para uso (TRUE) ou nao (FALSE)
	int  inUse;

	// Largura e espessura para efeito de calculo de consumo de material (em Kg)
	int    largura;
	double espessura;

	// Local de armazenamento do material
	char local[21]; // Campo no banco: VARCHAR(20)

	// Peso do material
	double peso;

	// Comprimento do material
	int  tamanho;

	// Numero de pecas no lote
	int  quantidade;

	// ID da tarefa que foi utilizada para gerar esse material
	int idTarefa;

	// Ponteiro para o proximo material
	struct strMaterial *Next;
};

// Funcoes da lista de materiais
struct strMaterial * AllocNewMaterial(void);
void ClearMaterials(void);
struct strMaterial * GetMaterial(unsigned int idx);
struct strMaterial * GetMaterialInUse(void);
struct strMaterial * GetMaterialByTask(int idTask);
struct strMaterial * GetMaterialByID(int id);

// Funcoes para trabalhar com os materiais
int material_getDV(char *strCodigo, enum enumTipoEstoque tipo);
int material_checaDV(char *strCodigo, int dv, enum enumTipoEstoque tipo);
void material_select(struct strMaterial *material);
double material_CalculaPeso(struct strMaterial *material, unsigned int tamanho);
void material_registraConsumo(struct strMaterial *materialConsumido, struct strMaterial *materialProduzido, unsigned int qtd, unsigned int tam, unsigned int tamPerda);
void material_registraPerda(struct strMaterial *materialConsumido, unsigned int qtd, unsigned int tamPerda, double pesoPerda);
struct strMaterial *material_copiar(struct strMaterial *dst, struct strMaterial *src, int isFullCopy);
void material_ajustarInventario(struct strMaterial *materialOrigem, struct strMaterial *materialDestino, unsigned int qtd);
void material_ajustarEstoque(struct strMaterial *material, double peso);

// Funcoes de tela
void CarregaComboLocais(GtkComboBox *cmb);
struct strMaterial *InsertMaterial(void);
void CarregaListaMateriais(GtkWidget *tvw);
void AbrirCadastroMaterial(struct strMaterial *material, int canEdit, int showDetails, int (*fnc)(struct strMaterial, int, int, void *), void *data);
void GravarMaterial(struct strMaterial *material);
gboolean ChecarMaterial(struct strMaterial material, int dv, int isFullCheck);
void AbrirRegistrarPeca(struct strMaterial *materialProduzido);

// Funcoes de Monitoramento
extern void monitor_Init();
extern void monitor_SendEstado();
extern void monitor_Set_Status(long torque, long current, long temperature);
extern void monitor_Set_User(char *user);
extern void monitor_Set_OpMode(unsigned int opmode);
extern void monitor_Clear_Status(void);
extern void monitor_enviaMsgMatCadastro(struct strMaterial *material);
extern void monitor_enviaMsgMatProducao(struct strMaterial *material, struct strMaterial *materialProd, struct strMaterial *materialPerda);
extern void monitor_enviaMsgAjusteInventario(struct strMaterial *materialSaida, struct strMaterial *materialEntrada);
extern void monitor_enviaMsgAjusteEstoque(struct strMaterial *materialOriginal, struct strMaterial *material);
extern void monitor_enviaMsgTransferencia(struct strMaterial *materialOrigem, struct strMaterial *materialDestino, struct strMaterial *materialNovaOrigem);

// Funcoes uteis
extern char   *floatToString(char *dst, double val);
extern double  StringToFloat(char *val);
