/*** Definições gerais ***/

//#define DEBUG_PC

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
#define NTB_ABA_HOME   0
#define NTB_ABA_CONFIG 1
#define NTB_ABA_OPERAR 2
#define NTB_ABA_LOGS   4
#define NTB_ABA_TAREFA 6
#define NTB_ABA_MODBUS 7
#define NTB_ABA_DATA   8

/*** Fim das definições gerais ***/
