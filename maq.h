#ifndef MAQ_H
#define MAQ_H

#define MAQ_ARQ_CONFIG "maq.config"
#define MAQ_CFG_MAGIC  0x78B2A5F0

typedef struct {
  char *ID;
  char *Name;
  char *Line;
  char *Machine;
} MaqConfig;

/*** Definicoes e funcoes relacionados a Configuracao da Maquina ***/

// Variaveis
extern MaqConfig  MaqConfigList[];
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
#define MAQ_SYNC_TODOS   (MAQ_SYNC_PERFIL | MAQ_SYNC_ENCODER | MAQ_SYNC_CORTE)

// Registradores do CLP
#define MAQ_REG_ERROS              0
#define MAQ_REG_STATUS             1
#define MAQ_REG_FLAGS              2

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
} maq_param;

/*** Fim das estruturas de informacoes da maquina ***/

uint16_t MaqLerErros   (void);
char    *MaqStrErro    (uint16_t status);

uint16_t MaqLerModo    (void);
uint16_t MaqLerProdQtd (void);
uint32_t MaqLerEntradas(void);
uint32_t MaqLerSaidas  (void);

void                      MaqConfigModo   (uint16_t  modo);
void                      MaqConfigProdQtd(uint16_t quant);
void                      MaqConfigProdTam(uint16_t   tam);

void                      MaqConfigPerfil (struct strMaqParamPerfil pf);
struct strMaqParamPerfil  MaqLerPerfil    (void);
void                      MaqConfigEncoder(struct strMaqParamEncoder enc);
struct strMaqParamEncoder MaqLerEncoder   (void);
void                      MaqConfigCorte  (struct strMaqParamCorte corte);
struct strMaqParamCorte   MaqLerCorte     (void);

int  MaqLerConfig   (void);
void MaqGravarConfig(void);

void     MaqLimparErro(void);
void     MaqLiberar   (uint16_t liberar);
uint16_t MaqLerEstado (void);
void     MaqPerfManual(uint16_t cmd);
void     MaqCortar    (void);

#endif
