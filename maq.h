#ifndef MAQ_H
#define MAQ_H

#define MAQ_INPUT_MAX  19
#define MAQ_OUTPUT_MAX 16

#define MAQ_ARQ_CONFIG "maq.config"
#define MAQ_CFG_MAGIC  0x78B2A5F0

typedef struct {
  char *NameIO;
  char *NameFile;
} MaqIO;

typedef struct {
  int    InputMask;
  MaqIO Input [MAQ_INPUT_MAX ];
  MaqIO Output[MAQ_OUTPUT_MAX];
} MaqIOMap;

typedef struct {
  char          *ID;                    ///< ID da maquina (hostname) para que o programa saiba qual maquina esta sendo controlada
  char          *Name;                  ///< Nome da maquina, utilizado para montar a caixa de selecao na tela de configuracao

  char          *Line;                  ///< Codigo do sistema de gestao para linha a que esta maquina pertence
  char          *Machine;               ///< Codigo do sistema de gestao para identificar esta maquina na linha

  char          *ClpAddr;               ///< Endereco do CLP que controla a maquina (IP ou Nome a ser resolvido pelo DNS)

  int            AbaHome;               ///< Numero da aba associada a tela inicial dessa maquina
  int            AbaManut;              ///< Numero da aba associada a tela de manutencao dessa maquina
  int            AbaOperAuto;           ///< Numero da aba associada a tela de operacao dessa maquina
  int            AbaConfigMais;         ///< Numero da aba associada a tela de configuracoes adicionais dessa maquina ou Zero se nao possuir configuracoes adicionais

  int            UseLogin;              ///< Flag que indica se essa maquina exige login (TRUE) ou nao (FALSE)
  int            UseIndet;              ///< Flag que indica se essa maquina pode ficar em estado indeterminado (TRUE) ou nao (FALSE)
  int            UseChaveGeral;         ///< Flag que indica se essa maquina possui controle de Chave Geral (Capacidade de Energizar / Desenergizar o painel)

  int            NeedMaqInit;           ///< Flag que indica se essa maquina exige inicializacao (TRUE) ou nao (FALSE)
  int            MaqModeCV;             ///< Flag que indica se essa maquina suporta corte em movimento (TRUE) ou nao (FALSE)
  int            InverterComandos;      ///< Indica que os comandos da tela de operacao manual (Avancar / Recuar) tem funcao invertida

  MaqIOMap      *IOMap;                 ///< Ponteiro para a estrutura contendo o mapa de I/O dessa maquina

  char         **ErrorList;             ///< Ponteiro com a lista de erros desta maquina
  unsigned int   Alertas;               ///< Mascara indicando quais erros devem ser considerados apenas como alerta e portanto nao exigem a parada da maquina

  int         ( *fncOnInit     )(void); ///< Ponteiro para funcao que deve ser executada quando a maquina for inicializada
  void        ( *fncOnError    )(int ); ///< Ponteiro para funcao que deve ser executada quando ocorrer um erro na maquina
  void        ( *fncOnOperAuto )(void); ///< Ponteiro para funcao que deve ser executada quando a maquina entrar na tela de operacao automatica
  void        ( *fncOnAuto     )(int ); ///< Ponteiro para funcao que deve ser executada quando a maquina entrar ou sair do modo automatico
  void        ( *fncTimerUpdate)(void); ///< Ponteiro para funcao que deve ser executada quando ocorrer um ciclo de atualizacao da maquina (tmrGtkUpdate)
} MaqConfig;

/*** Definicoes e funcoes relacionados a Configuracao da Maquina ***/

// Variaveis
extern MaqConfig  MaqConfigList[];
extern MaqConfig  MaqConfigDefault;
extern MaqConfig *MaqConfigCurrent;

// Funcoes
void        MaqConfig_SetMachine(char *ID);
MaqConfig * MaqConfig_GetMachine(int index);
int         MaqConfig_GetActive (void);

/*** Fim da Configuracao da Maquina ***/

// Modos de operacao
#define MAQ_MODO_MASK   0x0001
#define MAQ_MODO_MANUAL 0x0000
#define MAQ_MODO_AUTO   0x0001

// Flags que disparam acoes na maquina
#define MAQ_MODO_CORTAR 0x0002 // Executa uma operacao de corte
#define MAQ_MODO_LIMPAR 0x0004 // Limpa erros existentes

// Flag solicitando sincronizacao dos parametros do CLP com o Inversor
#define MAQ_MODO_PERF_SYNC 0x0020 // Configura o inversor

// Flag solicitando sincronizacao do status do inversor com o CLP
#define MAQ_MODO_INV_SYNC  0x0080 // Leitura de dados do inversor

// Flag solicitando sincronizacao do status do servo da mesa com o CLP
// Apenas para maquinas que possuem Corte Voador
#define MAQ_MODO_MESA_SYNC  0x0100 // Configura o servo

// Flag que libera a maquina para operacao
#define MAQ_MODO_LIBERA 0x0040 // Ativa a maquina

// Flag que liga a chave gera da maquina
#define MAQ_MODO_LIGAR  0x0800 // Liga a chave geral da maquina

// Mascara que indica quais flags devem ser desligadas quando se desativa a maquina
#define MAQ_MODO_MASK_LIBERA  (~(MAQ_MODO_LIBERA | MAQ_MODO_LIMPAR | MAQ_MODO_MASK))

// Flags para controle manual da perfiladeira
#define MAQ_MODO_PERF_MASK   0x0018
#define MAQ_MODO_PERF_AVANCA 0x0008
#define MAQ_MODO_PERF_RECUA  0x0010

// Flags para controle manual da mesa (Para maquinas com Corte Voador)
#define MAQ_MODO_MESA_MASK   0x0600
#define MAQ_MODO_MESA_AVANCA 0x0200
#define MAQ_MODO_MESA_RECUA  0x0400

// Flags Para configurar sentido de rotacao e numero de ciclos das prensas
#define MAQ_MODO_PRS_SENTIDO  0x4000
#define MAQ_MODO_PRS_CICLOS   0x8000

// Flag para carregamento de furacao intermediaria (Exclusiva para Diagonal/Travessa)
#define MAQ_LOAD_QTD_FUROS   0x2000

// Flags de operacoes manuais
#define OPER_PERF_PARAR  0
#define OPER_PERF_AVANCA 1
#define OPER_PERF_RECUA  2
#define OPER_MESA_PARAR  3
#define OPER_MESA_AVANCA 4
#define OPER_MESA_RECUA  5
#define OPER_CORTAR      6

// Flags que indicam o estado da maquina
#define MAQ_STATUS_PRONTA     0x0004
#define MAQ_STATUS_PRS_LIGADA 0x0040
#define MAQ_STATUS_INITOK     0x0040
#define MAQ_STATUS_DESLIGADA  0x8000

// Mascara para sincronizacao com CLPs
#define MAQ_SYNC_PERFIL  0x01
#define MAQ_SYNC_ENCODER 0x02
#define MAQ_SYNC_CORTE   0x04
#define MAQ_SYNC_CUSTOM  0x08
#define MAQ_SYNC_TODOS   (MAQ_SYNC_PERFIL | MAQ_SYNC_ENCODER | MAQ_SYNC_CORTE | MAQ_SYNC_CUSTOM)

// Flags para controle manual da aplanadora
#define MAQ_FM_APLAN_SUBIR        0x0100
#define MAQ_FM_APLAN_DESCER       0x0200
#define MAQ_FM_APLAN_ABRIR        0x0400
#define MAQ_FM_APLAN_FECHAR       0x0800
#define MAQ_FM_APLAN_EXT_SUBIR    0x1000
#define MAQ_FM_APLAN_EXT_DESCER   0x2000
#define MAQ_FM_APLAN_EXT_EXPANDIR 0x4000
#define MAQ_FM_APLAN_EXT_RETRAIR  0x8000

// Flags para controle manual da prensa
#define MAQ_FM_PRS_LIGAR          0x1000
#define MAQ_FM_PRS_PARAR          0x2000

// Comandos da aplanadora
#define MAQ_APLAN_ABRIR         0
#define MAQ_APLAN_FECHAR        1
#define MAQ_APLAN_SUBIR         2
#define MAQ_APLAN_DESCER        3
#define MAQ_APLAN_PARAR         4
#define MAQ_APLAN_EXT_SUBIR     5
#define MAQ_APLAN_EXT_DESCER    6
#define MAQ_APLAN_EXT_EXPANDIR  7
#define MAQ_APLAN_EXT_RETRAIR   8
#define MAQ_APLAN_AVANCAR       9
#define MAQ_APLAN_RECUAR       10

// Flags para operação manual da prensa
#define MAQ_PRS_LIGAR          20
#define MAQ_PRS_DESLIGAR       21
#define MAQ_PRS_INICIAR        22
#define MAQ_PRS_PARAR          23

// Registradores do CLP
#define MAQ_REG_ERROS              0
#define MAQ_REG_STATUS             1
#define MAQ_REG_FLAGS              2

#define MAQ_REG_POS_ATUAL          4

#define MAQ_REG_INV_TENSAO         5
#define MAQ_REG_INV_CORRENTE       6
#define MAQ_REG_INV_TORQUE         7
#define MAQ_REG_INV_INPUT          8
#define MAQ_REG_INV_OUTPUT         9

#define MAQ_REG_PROD_QTD          30
#define MAQ_REG_PROD_TAM          31

#define MAQ_REG_PERF_FATOR_LOW    10
#define MAQ_REG_PERF_FATOR_HIGH   11
#define MAQ_REG_PERF_AUTO_ACEL    12
#define MAQ_REG_PERF_AUTO_DESACEL 13
#define MAQ_REG_PERF_AUTO_VEL     14
#define MAQ_REG_PERF_MAN_ACEL     15
#define MAQ_REG_PERF_MAN_DESACEL  16
#define MAQ_REG_PERF_MAN_VEL      17
#define MAQ_REG_ENC_FATOR         20
#define MAQ_REG_ENC_RESOL         21
#define MAQ_REG_ENC_PERIM         22
#define MAQ_REG_CRT_FACA          25

// Registradores especificos das Prensas
#define MAQ_REG_APL_PASSO            18
#define MAQ_REG_APL_ERRO_POSIC       19
#define MAQ_REG_PRS_CICLOS_UNID      23
#define MAQ_REG_PRS_CICLOS_MIL       24
#define MAQ_REG_PRS_CICLOS_NOVO_UNID 25
#define MAQ_REG_PRS_CICLOS_NOVO_MIL  26
#define MAQ_REG_PRENSA_MANUAL        31

// Registradores especificos da Diagonal / Travessa
#define MAQ_REG_DIAG_DISTANCIA    24

// Registradores especificos da Coluna N
#define MAQ_REG_COLN_PERF_DINAM_VEL  18
#define MAQ_REG_COLN_MESA_POS        19
#define MAQ_REG_COLN_OFFSET          23
#define MAQ_REG_COLN_TAM_MIN         24
#define MAQ_REG_COLN_MESA_CURSO      26
#define MAQ_REG_COLN_MESA_AUTO_VEL   27
#define MAQ_REG_COLN_MESA_MANUAL_VEL 28
#define MAQ_REG_COLN_NOVO_VALOR      29

/*** Definicoes de IDs das Maquinas ***/

#define MAQ_ID_DEFAULT        "IhmTeste"
#define MAQ_ID_PERF_PP_NORMAL "IhmPP"
#define MAQ_ID_PERF_PP_PESADO "IhmPPPesado"
#define MAQ_ID_PERF_PP_LEVE   "IhmPPLeve"
#define MAQ_ID_APLAN_MEZANINO "IhmAplanMez"
#define MAQ_ID_PERF_TUBO      "IhmTubo"
#define MAQ_ID_PERF_COLUNA_N  "IhmColN"
#define MAQ_ID_TRAV_DIAGONAL  "IhmDiagTrav"
#define MAQ_ID_VIGA_MEZANINO  "IhmVigaMez"
#define MAQ_ID_COL_MEZANINO   "IhmColMez"
#define MAQ_ID_PERF_SIGMA     "IhmSigma"
#define MAQ_ID_BANHO          "IhmBanho"
#define MAQ_ID_APLAN_PP       "IhmAplanPP"
#define MAQ_ID_APLAN_COLUNA_N "IhmAplanN"

#define MAQ_ID_IS(id) ((MaqConfigCurrent != NULL) ? (!strcmp(MaqConfigCurrent->ID, id)) : FALSE)

/*** Estruturas de informacoes da Maquina ***/

// Estrutura contendo os parametros da maquina
struct strMaqParam
{
// Parametros relacionados com o encoder
  struct strMaqParamEncoder {
    float fator;
    unsigned int perimetro, precisao;
  } encoder;

// Parametros relacionados com a ferramenta de corte
  struct strMaqParamCorte {
    unsigned int tam_faca;
  } corte;

// Parametros relacionados com a perfiladeira
  struct strMaqParamPerfil {
    // Parâmetros do inversor
    unsigned int auto_vel; // % da velocidade maxima usada na velocidade automatica
    float auto_acel; // Tempo em segundos
    float auto_desacel; // Tempo em segundos
    unsigned int manual_vel; // % da velocidade maxima usada na velocidade manual
    float manual_acel; // Tempo em segundos
    float manual_desacel; // Tempo em segundos
    float fator; // Relacao de transmissao da maquina.
    unsigned int diam_rolo; // Diametro primitivo do rolo da perfiladeira
    } perfil;

    // Parametros especificos das maquinas, nao sendo utilizados por todas
    struct strMaqParamCustom {
      struct { // Parametros especificos da Diagonal / Travessa
        unsigned int dist_prensa_corte; // Distancia em mm entre a prensa e o corte
        unsigned int qtd_furos_interm ; // Quantidade de furos intermediarios na peca
      } diagonal;
      struct { // Parametros especificos da Coluna N
        unsigned int dinam_vel; // Velocidade do perfil em modo dinamico
        unsigned int curso; // Curso de deslocamento da mesa
        unsigned int auto_vel; // Velocidade da mesa em modo automatico
        unsigned int manual_vel; // Velocidade da mesa em modo manual
        float        offset; // Indica quanto antes da posicao de corte a mesa deve partir
        unsigned int tam_min; // Menor peca possivel de ser feita em modo dinamico
      } coln;
      struct { // Parametros das Prensas (especificamente PP e N)
		// Parametros da Prensa
		unsigned int sentido; // Sentido de rotacao do motor da prensa
		unsigned int ciclos; // numero atual de ciclos realizados pela prensa
		unsigned int ciclos_ferram; // Indica o numero de ciclos a partir do qual sera exibido o aviso de manutencao da ferramenta
		unsigned int ciclos_lub; // Indica a cada quantos ciclos deve ser exibido o aviso de lubrificacao
		// Parametros da Aplanadora
		unsigned int passo;      // Avanco da chapa (em mm) para cada ciclo da prensa
      } prensa;
    } custom;
} maq_param;

/*** Fim das estruturas de informacoes da maquina ***/

int      MaqInit       (void);
void     MaqError      (int error);
void     MaqAuto       (int ativo);

uint16_t MaqLerErros   (void);
char    *MaqStrErro    (uint16_t status);

uint16_t MaqLerModo    (void);
uint16_t MaqLerFlags   (void);

uint16_t MaqLerProdQtd (void);
uint32_t MaqLerEntradas(void);
uint32_t MaqLerSaidas  (void);

int16_t MaqLerPosAtual(void);
int16_t MaqLerAplanErroPosic(void);

uint16_t MaqLerRegistrador   (uint16_t reg, uint16_t default_value);
void     MaqGravarRegistrador(uint16_t reg, uint16_t val);

void     MaqSetDateTime      (struct tm *t);
void     MaqGetIpAddress     (char *iface, char *ipaddr);

void     MaqConfigModo       (uint16_t  modo);
void     MaqConfigFlags      (uint16_t flags);

void     MaqConfigProdQtd    (uint16_t quant);
void     MaqConfigProdTam    (uint16_t   tam);

void     MaqConfigChaveGeral (uint16_t ligar);
uint16_t MaqEstadoChaveGeral(void);

void                      MaqConfigPerfil (struct strMaqParamPerfil pf);
struct strMaqParamPerfil  MaqLerPerfil    (void);
void                      MaqConfigEncoder(struct strMaqParamEncoder enc);
struct strMaqParamEncoder MaqLerEncoder   (void);
void                      MaqConfigCorte  (struct strMaqParamCorte corte);
struct strMaqParamCorte   MaqLerCorte     (void);
void                      MaqConfigCustom (struct strMaqParamCustom custom);
struct strMaqParamCustom  MaqLerCustom    (void);

int MaqLerConfig   (void);
int MaqGravarConfig(void);

uint16_t MaqLerPrsCiclos(void);
void MaqConfigPrsCiclos(uint32_t val);
void MaqConfigPrsSentidoInv(uint16_t val);

void     MaqLimparErro (void);
void     MaqLiberar    (uint16_t liberar);
uint16_t MaqLerEstado  (void);
void     MaqPerfManual (uint16_t cmd);
void     MaqMesaManual (uint16_t cmd);
void     MaqPrsManual  (uint16_t cmd);
void     MaqAplanManual(uint16_t cmd);
void     MaqCortar     (void);

uint16_t MaqInvSyncOK     (void);
void     MaqInvSync       (void);

uint16_t MaqLerInvTensao  (void);
uint16_t MaqLerInvCorrente(void);
uint16_t MaqLerInvTorque  (void);
uint16_t MaqLerInvInput   (void);
uint16_t MaqLerInvOutput  (void);

#endif
