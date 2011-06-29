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
#define MAQ_STATUS_PRS_LIGADA 0x0040

// Flags de erros
#define MAQ_ERRO_COMUNIC     0x0001
#define MAQ_ERRO_EMERGENCIA  0x0002
#define MAQ_ERRO_FASE        0x0004
#define MAQ_ERRO_UNID_HIDR   0x0008
#define MAQ_ERRO_PRS_MARTELO 0x0010
#define MAQ_ERRO_PRESSAO_AR  0x0020
#define MAQ_ERRO_PRS_INV     0x0040
#define MAQ_ERRO_APLAN_SERVO 0x0080
#define MAQ_ERRO_DESBOB      0x0100
#define MAQ_ERRO_COMM_SERVO  0x0200
#define MAQ_ERRO_APLAN_POSIC 0x0400
#define MAQ_ERRO_ALERTAS     (MAQ_ERRO_APLAN_POSIC)

// Flags que controlam a operação da máquina
#define MAQ_MODO_LIBERA       0x0002
#define MAQ_MODO_LIMPAR       0x0004
#define MAQ_MODO_SYNC_SERVO   0x0008
#define MAQ_MODO_PRS_SENTIDO  0x0010
#define MAQ_MODO_PRS_CICLOS   0x0020
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
#define MAQ_FM_PRS_LIGAR          0x0100
#define MAQ_FM_PRS_INICIAR        0x0200
#define MAQ_FM_PRS_PARAR          0x0400

// Flags para operação manual da aplanadora
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
#define MAQ_PRS_LIGAR           0
#define MAQ_PRS_DESLIGAR        1
#define MAQ_PRS_INICIAR         2
#define MAQ_PRS_PARAR           3

// Mascara para sincronizacao com CLPs
#define MAQ_SYNC_PRENSA  0x01
#define MAQ_SYNC_ENCODER 0x02
#define MAQ_SYNC_APLAN   0x04
#define MAQ_SYNC_TODOS   (MAQ_SYNC_PRENSA | MAQ_SYNC_ENCODER | MAQ_SYNC_APLAN)

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
#define MAQ_REG_PRS_CICLOS_UNID      15
#define MAQ_REG_PRS_CICLOS_MIL       16
#define MAQ_REG_PRS_CICLOS_NOVO_UNID 17
#define MAQ_REG_PRS_CICLOS_NOVO_MIL  18
#define MAQ_REG_ENC_FATOR            20
#define MAQ_REG_ENC_RESOL            21
#define MAQ_REG_ENC_PERIM            22

/*** Estruturas de informacoes da Maquina ***/

// Estrutura contendo os parametros da maquina
struct strMaqParam
{
// Parametros relacionados com o encoder
  struct strMaqParamEncoder {
    float fator;
    unsigned int perimetro, precisao;
  } encoder;

// Parametros relacionados com a prensa
  struct strMaqParamPrensa {
    // Parâmetros da Prensa
    unsigned int sentido; // % da velocidade maxima usada na velocidade automatica
    unsigned int ciclos; // % da velocidade maxima usada na velocidade automatica
    unsigned int ciclos_ferram; // % da velocidade maxima usada na velocidade manual
    unsigned int ciclos_lub; // % da velocidade maxima usada na velocidade automatica
    } prensa;

  // Parametros relacionados com a mesa
    struct strMaqParamAplan {
      // Parâmetros da Aplanadora
      unsigned int auto_vel;   // Velocidade maxima usada na velocidade automatica em mm/min
      unsigned int manual_vel; // Velocidade maxima usada na velocidade manual em mm/min
      unsigned int passo;      // Curso da mesa em mm
      float        rampa;     // Offset usado para compensar perdas ao sincronizar
      } aplanadora;
} maq_param;

/*** Fim das estruturas de informacoes da maquina ***/

uint16_t MaqLerErros   (void);
char    *MaqStrErro    (uint16_t status);

int      MaqSync       (unsigned int mask);
uint16_t MaqPronta     (void);
void     MaqLimparErro (void);

uint16_t MaqLerModo          (void);
uint16_t MaqLerFlags         (void);
uint16_t MaqLerEstado        (void);
int16_t  MaqLerPosAtual      (void);
uint32_t MaqLerEntradas      (void);
uint32_t MaqLerSaidas        (void);
uint16_t MaqLerPrsCiclos     (void);
int16_t  MaqLerAplanErroPosic(void);

void                      MaqConfigFlags        (uint16_t flags);
void                      MaqConfigModo         (uint16_t modo);
void                      MaqLiberar            (uint16_t liberar);
void                      MaqConfigPrsCiclos    (uint32_t val);
void                      MaqConfigPrsSentidoInv(uint16_t val);

void                      MaqAplanManual    (uint16_t comando);
void                      MaqPrsManual      (uint16_t comando);

void                      MaqConfigPrensa (struct strMaqParamPrensa pr);
struct strMaqParamPrensa  MaqLerPrensa    (void);
void                      MaqConfigEncoder(struct strMaqParamEncoder enc);
struct strMaqParamEncoder MaqLerEncoder   (void);
void                      MaqConfigAplan  (struct strMaqParamAplan aplan);
struct strMaqParamAplan   MaqLerAplan     (void);

int  MaqLerConfig   (void);
void MaqGravarConfig(void);

#endif
