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
  char          *ID;
  char          *Name;

  char          *Line;
  char          *Machine;

  char          *ClpAddr;

  int            AbaHome;
  int            AbaManut;
  int            AbaConfigMais;

  int            UseLogin;
  int            UseIndet;

  MaqIOMap      *IOMap;

  char         **ErrorList;
  unsigned int   Alertas;

  int         ( *fncOnInit )(void);
  void        ( *fncOnError)(int );
  void        ( *fncOnAuto )(int );
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
#define MAQ_MODO_CORTAR 0x0002
#define MAQ_MODO_LIMPAR 0x0004

// Flag solicitando sincronizacao dos parametros do CLP com o Inversor
#define MAQ_MODO_PERF_SYNC 0x0020

// Flag solicitando sincronizacao do status do inversor com o CLP
#define MAQ_MODO_INV_SYNC  0x0080

// Flag que libera a maquina para operacao
#define MAQ_MODO_LIBERA 0x0040

#define MAQ_MODO_MASK_LIBERA  (~(MAQ_MODO_LIBERA | MAQ_MODO_LIMPAR | MAQ_MODO_MASK))

// Flags para controle manual da perfiladeira
#define MAQ_MODO_PERF_MASK   0x0018
#define MAQ_MODO_PERF_AVANCA 0x0008
#define MAQ_MODO_PERF_RECUA  0x0010

#define PERF_PARAR  0
#define PERF_AVANCA 1
#define PERF_RECUA  2

// Mascara para sincronizacao com CLPs
#define MAQ_SYNC_PERFIL  0x01
#define MAQ_SYNC_ENCODER 0x02
#define MAQ_SYNC_CORTE   0x04
#define MAQ_SYNC_CUSTOM  0x08
#define MAQ_SYNC_TODOS   (MAQ_SYNC_PERFIL | MAQ_SYNC_ENCODER | MAQ_SYNC_CORTE | MAQ_SYNC_CUSTOM)

// Registradores do CLP
#define MAQ_REG_ERROS              0
#define MAQ_REG_STATUS             1
#define MAQ_REG_FLAGS              2

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

#define MAQ_REG_DIAG_DISTANCIA    24

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
        unsigned int dist_prensa_corte;
      } diagonal;
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

uint16_t MaqLerRegistrador   (uint16_t reg, uint16_t default_value);
void     MaqGravarRegistrador(uint16_t reg, uint16_t val);

void                      MaqConfigModo   (uint16_t  modo);
void                      MaqConfigFlags  (uint16_t flags);

void                      MaqConfigProdQtd(uint16_t quant);
void                      MaqConfigProdTam(uint16_t   tam);

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

void     MaqLimparErro(void);
void     MaqLiberar   (uint16_t liberar);
uint16_t MaqLerEstado (void);
void     MaqPerfManual(uint16_t cmd);
void     MaqCortar    (void);

uint16_t MaqInvSyncOK     (void);
void     MaqInvSync       (void);

uint16_t MaqLerInvTensao  (void);
uint16_t MaqLerInvCorrente(void);
uint16_t MaqLerInvTorque  (void);
uint16_t MaqLerInvInput   (void);
uint16_t MaqLerInvOutput  (void);

#endif
