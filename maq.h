#ifndef MAQ_H
#define MAQ_H

#define MAQ_ARQ_CONFIG "maq.config"
#define MAQ_CFG_MAGIC  0x78B2A5F0

// Modos de operacao
#define MAQ_MODO_MANUAL 0x0000
#define MAQ_MODO_AUTO   0x8000

// Mascara para desligar os bits que nao pertencem ao modo ou status
#define MAQ_MASK_MODO   0xF000
#define MAQ_MASK_STATUS 0x0FFF

// Definicao dos bits de CMD
#define MAQ_CMD_CORTAR 0x01
#define MAQ_CMD_AVANCA 0x02
#define MAQ_CMD_RECUA  0x04
#define MAQ_CMD_LIMPAR 0x08
#define MAQ_CMD_SYNC   0x10

// Mascara para sincronizacao com CLPs
#define MAQ_SYNC_PERFIL  0x01
#define MAQ_SYNC_ENCODER 0x02
#define MAQ_SYNC_CORTE   0x04
#define MAQ_SYNC_TODOS   (MAQ_SYNC_PERFIL | MAQ_SYNC_ENCODER | MAQ_SYNC_CORTE)

// Registradores do CLP
#define MAQ_REG_STATUS          0
#define MAQ_REG_CMD             2

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
#define MAQ_REG_CRT_FACA          19
#define MAQ_REG_ENC_FATOR         20
#define MAQ_REG_ENC_RESOL         21
#define MAQ_REG_ENC_PERIM         22

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

uint16_t MaqLerStatus (void);
char    *MaqStrErro(uint16_t status);

uint16_t MaqLerModo    (void);
uint16_t MaqLerProdQtd (void);
uint32_t MaqLerEntradas(void);
uint32_t MaqLerSaidas  (void);

void                      MaqConfigModo   (uint16_t  modo);
void                      MaqConfigProdQtd(uint16_t quant);
void                      MaqConfigProdTam(uint16_t   tam);
void                      MaqPerfParar    ();
void                      MaqPerfCortar   ();
void                      MaqLimparErro   ();
void                      MaqPerfRecuar   ();
void                      MaqPerfAvancar  ();
void                      MaqInvParamSync ();

void                      MaqConfigPerfil (struct strMaqParamPerfil pf);
struct strMaqParamPerfil  MaqLerPerfil    (void);
void                      MaqConfigEncoder(struct strMaqParamEncoder enc);
struct strMaqParamEncoder MaqLerEncoder   (void);
void                      MaqConfigCorte  (struct strMaqParamCorte corte);
struct strMaqParamCorte   MaqLerCorte     (void);

int  MaqLerConfig   (void);
void MaqGravarConfig(void);

#endif
