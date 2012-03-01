#ifndef MAQ_H
#define MAQ_H

#define MAQ_ARQ_CONFIG "maq.config"
#define MAQ_CFG_MAGIC  0x78B2A5F0

// Modos de operacao
#define MAQ_MODO_MASK   0x0001
#define MAQ_MODO_MANUAL 0x0000
#define MAQ_MODO_AUTO   0x0001

// Flags que indicam o estado da máquina
#define MAQ_STATUS_PRONTA     0x0004

// Flags de erros
#define MAQ_ERRO_COMUNIC     0x0001
#define MAQ_ERRO_EMERGENCIA  0x0002
#define MAQ_ERRO_FASE        0x0004
#define MAQ_ERRO_UNID_HIDR   0x0008
#define MAQ_ERRO_PRESSAO_AR  0x0020
#define MAQ_ERRO_PRS_INV     0x0040
#define MAQ_ERRO_APLAN_SERVO 0x0080
#define MAQ_ERRO_COMM_SERVO  0x0200
#define MAQ_ERRO_APLAN_POSIC 0x0400
#define MAQ_ERRO_ALERTAS     (MAQ_ERRO_APLAN_POSIC)

// Flags que controlam a operação da máquina
#define MAQ_MODO_LIBERA       0x0002
#define MAQ_MODO_LIMPAR       0x0004
#define MAQ_MODO_SYNC_SERVO   0x0008
#define MAQ_MODO_MASK_LIBERA  (~(MAQ_MODO_LIBERA | MAQ_MODO_LIMPAR | MAQ_MODO_SYNC_SERVO | MAQ_MODO_MASK))

// Flags para controle manual da aplanadora
#define MAQ_FM_APLAN_AVANCAR      0x0001
#define MAQ_FM_APLAN_RECUAR       0x0002
#define MAQ_FM_APLAN_ABRIR        0x0004
#define MAQ_FM_APLAN_FECHAR       0x0008
#define MAQ_FM_APLAN_SUBIR        0x0010
#define MAQ_FM_APLAN_DESCER       0x0020
#define MAQ_FM_APLAN_EXT_SUBIR    0x0040
#define MAQ_FM_APLAN_EXT_EXPANDIR 0x0080

// Flags para operação manual da aplanadora
#define MAQ_APLAN_PARAR         4
#define MAQ_APLAN_AVANCAR       9
#define MAQ_APLAN_RECUAR       10

// Mascara para sincronizacao com CLPs
#define MAQ_SYNC_ENCODER 0x01
#define MAQ_SYNC_APLAN   0x02
#define MAQ_SYNC_TODOS   (MAQ_SYNC_ENCODER | MAQ_SYNC_APLAN)

// Registradores do CLP
#define MAQ_REG_ERROS                 0
#define MAQ_REG_STATUS                1
#define MAQ_REG_FLAGS                 2
#define MAQ_REG_FLAGS_MANUAL          4
#define MAQ_REG_POS_ATUAL             5

#define MAQ_REG_APL_PASSO            10
#define MAQ_REG_APL_ACELMS           11
#define MAQ_REG_APL_VELAUTO          12
#define MAQ_REG_APL_VELMAN           13
#define MAQ_REG_APL_ERRO_POSIC       14
#define MAQ_REG_PASSOS_START         15
#define MAQ_REG_PASSOS_PORTASL       16
#define MAQ_REG_PASSOS_PORTASH       17
#define MAQ_REG_PASSOS_QTD           18
#define MAQ_REG_ENC_FATOR            20
#define MAQ_REG_ENC_RESOL            21
#define MAQ_REG_ENC_PERIM            22

// Configurações gerais
#define MAQ_PASSOS_MAX 6

/*** Estruturas de informacoes da Maquina ***/

// Estrutura contendo os parametros da maquina
struct strMaqParam
{
// Parametros relacionados com o encoder
  struct strMaqParamEncoder {
    float fator;
    unsigned int perimetro, precisao;
  } encoder;

  // Parametros relacionados com a mesa
  struct strMaqParamAplan {
    // Parâmetros da Aplanadora
    unsigned int auto_vel;   // Velocidade maxima usada na velocidade automatica em mm/min
    unsigned int manual_vel; // Velocidade maxima usada na velocidade manual em mm/min
    struct {
      unsigned int passo; // Deslocamento em mm do passo atual
      unsigned int repeticoes; // Número de vezes que o deslocamento atual deve repetir
      unsigned int portas; // Portas que devem ser ligadas para o passo atual
    } passos[MAQ_PASSOS_MAX];
    float        rampa;     // Offset usado para compensar perdas ao sincronizar
  } aplanadora;
} maq_param;

/*** Fim das estruturas de informacoes da maquina ***/

uint16_t MaqLerErros   (void);
char    *MaqStrErro    (uint16_t status);

int      MaqSync       (unsigned int mask);
void     MaqSyncPassos (unsigned int passos);
uint16_t MaqPronta     (void);
void     MaqLimparErro (void);

uint16_t MaqLerModo          (void);
uint16_t MaqLerFlags         (void);
uint16_t MaqLerEstado        (void);
int16_t  MaqLerPosAtual      (void);
uint32_t MaqLerEntradas      (void);
uint32_t MaqLerSaidas        (void);
int16_t  MaqLerAplanErroPosic(void);

void                      MaqConfigFlags        (uint16_t flags);
void                      MaqConfigModo         (uint16_t modo);
void                      MaqLiberar            (uint16_t liberar);

void                      MaqAplanManual    (uint16_t comando);
void                      MaqPrsManual      (uint16_t comando);

void                      MaqConfigEncoder(struct strMaqParamEncoder enc);
struct strMaqParamEncoder MaqLerEncoder   (void);
void                      MaqConfigAplan  (struct strMaqParamAplan aplan);
struct strMaqParamAplan   MaqLerAplan     (void);

int  MaqLerConfig   (void);
void MaqGravarConfig(void);

#endif
