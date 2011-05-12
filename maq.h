#ifndef MAQ_H
#define MAQ_H

#define MAQ_ARQ_CONFIG "maq.config"
#define MAQ_CFG_MAGIC  0x78B2A5F0

// Modos de operacao
#define MAQ_MODO_MASK   0x0001
#define MAQ_MODO_MANUAL 0x0000
#define MAQ_MODO_AUTO   0x0001

// Flags que indicam o estado da máquina
#define MAQ_STATUS_OPERANDO 0x0200
#define MAQ_STATUS_INITOK   0x0040

// Flags de erros
#define MAQ_ERRO_COMUNIC    0x0001
#define MAQ_ERRO_EMERGENCIA 0x0002
#define MAQ_ERRO_FASE       0x0004
#define MAQ_ERRO_UNID_HIDR  0x0008
#define MAQ_ERRO_INVERSOR   0x0010
#define MAQ_ERRO_SERVO      0x0020
#define MAQ_ERRO_COMM_INV   0x0040
#define MAQ_ERRO_COMM_SERVO 0x0080
#define MAQ_ERRO_POSIC      0x0100
#define MAQ_ERRO_CORTE      0x0200
#define MAQ_ERRO_DESATIVADA 0x0400
#define MAQ_ERRO_ALERTAS    (MAQ_ERRO_POSIC | MAQ_ERRO_CORTE)

// Flags que controlam a operação da máquina
#define MAQ_MODO_CORTAR     0x0002
#define MAQ_MODO_LIMPAR     0x0004
#define MAQ_MODO_LIBERA     0x0008
#define MAQ_MODO_POSIC      0x0040
#define MAQ_MODO_CALC_FATOR 0x0800
#define MAQ_MODO_CALC_REL   0x1000
#define MAQ_MODO_CALC_MIN   0x2000

// Flag solicitando sincronizacao dos parametros do CLP com o Servo e Inversor
#define MAQ_MODO_MESA_SYNC  0x0010
#define MAQ_MODO_PERF_SYNC  0x0020

// Flags para controle manual da perfiladeira
#define MAQ_MODO_PERF_MASK   0x0180
#define MAQ_MODO_PERF_AVANCA 0x0080
#define MAQ_MODO_PERF_RECUA  0x0100

// Flags para controle manual da mesa
#define MAQ_MODO_MESA_MASK   0x0600
#define MAQ_MODO_MESA_AVANCA 0x0200
#define MAQ_MODO_MESA_RECUA  0x0400

#define PERF_PARAR  0
#define PERF_AVANCA 1
#define PERF_RECUA  2

#define MESA_PARAR  0
#define MESA_AVANCA 1
#define MESA_RECUA  2

// Mascara para sincronizacao com CLPs
#define MAQ_SYNC_PERFIL  0x01
#define MAQ_SYNC_ENCODER 0x02
#define MAQ_SYNC_MESA    0x04
#define MAQ_SYNC_TODOS   (MAQ_SYNC_PERFIL | MAQ_SYNC_ENCODER | MAQ_SYNC_MESA)

// Registradores do CLP
#define MAQ_REG_ERROS              0
#define MAQ_REG_STATUS             1
#define MAQ_REG_FLAGS              2

#define MAQ_REG_PROD_QTD          30
#define MAQ_REG_PROD_TAM          31

#define MAQ_REG_MESA_CURSO        10
#define MAQ_REG_MESA_OFFSET       11
#define MAQ_REG_PERF_AUTO_ACEL    12
#define MAQ_REG_PERF_AUTO_DESACEL 13
#define MAQ_REG_PERF_DINAM_VEL    14
#define MAQ_REG_PERF_MAN_ACEL     15
#define MAQ_REG_PERF_MAN_DESACEL  16
#define MAQ_REG_PERF_MAN_VEL      17
#define MAQ_REG_MESA_TAM_MIN      18
#define MAQ_REG_MESA_POS          19
#define MAQ_REG_ENC_FATOR         20
#define MAQ_REG_ENC_RESOL         21
#define MAQ_REG_ENC_PERIM         22
#define MAQ_REG_NOVO_TAM_MIN      23
#define MAQ_REG_MESA_MAN_VEL      24
#define MAQ_REG_MESA_AUTO_VEL     25
#define MAQ_REG_PERF_ESTAT_VEL    26
#define MAQ_REG_ENC_FATOR_NOVO    27
#define MAQ_REG_ENC_REL_NOVO      28

/*** Estruturas de informacoes da Maquina ***/

// Estrutura contendo os parametros da maquina
struct strMaqParam
{
// Parametros relacionados com o encoder
  struct strMaqParamEncoder {
    float fator;
    unsigned int perimetro, precisao;
  } encoder;

// Parametros relacionados com a perfiladeira
  struct strMaqParamPerfil {
    // Parâmetros do inversor
    unsigned int dinam_vel; // % da velocidade maxima usada na velocidade automatica (Dinâmico)
    unsigned int estat_vel; // % da velocidade maxima usada na velocidade automatica (Estático)
    float auto_acel; // Tempo em segundos
    float auto_desacel; // Tempo em segundos
    unsigned int manual_vel; // % da velocidade maxima usada na velocidade manual
    float manual_acel; // Tempo em segundos
    float manual_desacel; // Tempo em segundos
    } perfil;

  // Parametros relacionados com a mesa
    struct strMaqParamMesa {
      // Parâmetros da mesa
      float        auto_vel;   // Velocidade maxima usada na velocidade automatica em mm/min
      float        manual_vel; // Velocidade maxima usada na velocidade manual em mm/min
      unsigned int curso;      // Curso da mesa em mm
      float        offset;     // Offset usado para compensar perdas ao sincronizar
      unsigned int tam_min;    // Tamanho minimo da peca para modo dinamico
      } mesa;
} maq_param;

/*** Fim das estruturas de informacoes da maquina ***/

uint16_t MaqLerErros   (void);
char    *MaqStrErro    (uint16_t status);

int      MaqSync       (unsigned int mask);
uint16_t MaqOperando   (void);
void     MaqLimparErro (void);

uint16_t MaqLerModo    (void);
uint16_t MaqLerProdQtd (void);
uint32_t MaqLerEntradas(void);
uint32_t MaqLerSaidas  (void);

void                      MaqConfigFlags    (uint16_t flags);
void                      MaqConfigModo     (uint16_t modo);
void                      MaqLiberar        (uint16_t liberar);
void                      MaqConfigPosMesa  (uint16_t pos);
void                      MaqConfigProdQtd  (uint16_t quant);
void                      MaqConfigProdTam  (uint16_t tam);

void                      MaqMesaPosic      (uint16_t pos);
void                      MaqPerfManual     (uint16_t cmd);
void                      MaqMesaManual     (uint16_t cmd);
void                      MaqCortar         (void);
uint16_t                  MaqLerNovoFator   (void);
uint16_t                  MaqLerNovoRelEnc  (void);
uint16_t                  MaqLerNovoTamMin  (void);
void                      MaqCalcFatorPerfil(void);
void                      MaqCalcRelEnc     (void);
void                      MaqCalcTamMin     (void);

void                      MaqConfigPerfil (struct strMaqParamPerfil pf);
struct strMaqParamPerfil  MaqLerPerfil    (void);
void                      MaqConfigEncoder(struct strMaqParamEncoder enc);
struct strMaqParamEncoder MaqLerEncoder   (void);
void                      MaqConfigMesa   (struct strMaqParamMesa mesa);
struct strMaqParamMesa    MaqLerMesa      (void);

uint16_t  MaqLerFlags    (void);
uint16_t  MaqLerEstado   (void);
int       MaqLerConfig   (void);
void      MaqGravarConfig(void);

#endif
