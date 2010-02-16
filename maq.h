#ifndef MAQ_H
#define MAQ_H

#define MAQ_ARQ_CONFIG "maq.config"
#define MAQ_CFG_MAGIC  0x78B2A5F0

// Modos de operacao
#define MAQ_MODO_MANUAL 0x0000
#define MAQ_MODO_AUTO   0x8000

// Mascara para sincronizacao com CLPs
#define MAQ_SYNC_PERFIL  0x01
#define MAQ_SYNC_ENCODER 0x02
#define MAQ_SYNC_CORTE   0x04
#define MAQ_SYNC_TODOS   (MAQ_SYNC_PERFIL | MAQ_SYNC_ENCODER | MAQ_SYNC_CORTE)

// Registradores do CLP
#define MAQ_REG_STATUS          0

#define MAQ_REG_PROD_QTD        200
#define MAQ_REG_PROD_TAM        201

#define MAQ_REG_PERF_FATOR_LOW  400
#define MAQ_REG_PERF_FATOR_HIGH 401
#define MAQ_REG_PERF_ACEL       402
#define MAQ_REG_PERF_DESACEL    403
#define MAQ_REG_PERF_VEL_MAX    404
#define MAQ_REG_PERF_VEL_MANUAL 405
#define MAQ_REG_ENC_FATOR       410
#define MAQ_REG_ENC_RESOL       411
#define MAQ_REG_ENC_PERIM       412
#define MAQ_REG_CRT_FACA        420

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
    unsigned int vel_manual; // % da velocidade maxima usada na velocidade manual
    unsigned int vel_max; // Velocidade em Hz
    float acel; // Tempo de segundos
    float desacel; // Tempo em segundos
    float fator; // Relacao de transmissao da maquina.
    unsigned int diam_rolo; // Diametro primitivo do rolo da perfiladeira
    } perfil;
} maq_param;

/*** Fim das estruturas de informacoes da maquina ***/

uint16_t MaqLerModo   (void);
uint16_t MaqLerProdQtd(void);

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

#endif
