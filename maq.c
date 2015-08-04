#include "defines.h"
#include "maq.h"

void Log(char *evento, int tipo);

#ifndef DEBUG_PC_NOETH
extern void IPC_Update(void);
#endif

// Funcoes de inicializacao das maquinas
int Banho_Init              (void); // Inicializacao do banho
int Diagonal_Init           (void); // Inicializacao da Diagonal/Travessa
int ProgPrensa_Init         (void); // Inicializacao da Prensa de Mezanino
int ProgPrensaPassoFixo_Init(void); // Inicializacao das Prensas de Passo Fixo

// Funcoes de tratamento de erro das maquinas
void Banho_Erro (int erro); // Tratamento de erro do banho

// Funcoes de configuracao da tela de operacao automatica das maquinas
void ProgPrensa_Produzir         (void); // Configuracao da tela de operacao automatica da prensa de passo variavel (Programacao de Passos / Repeticoes)
void ProgPrensaPassoFixo_Produzir(void); // Configuracao da tela de operacao automatica da prensa de passo fixo (Sem Programacao de Passos / Repeticoes)

// Funcoes de atualizacao das maquinas
void PrensaPassoFixo_Update(void); // Atualizacao da tela da Prensa de Passo Fixo
void PrensaMezanino_Update (void); // Atualizacao da tela da Prensa de Mezanino

// Funcoes de mudanca de modo das maquinas: manual <=> auto
void Diagonal_Auto(int ativo); // Diagonal/Travessa

void MaqErro(int erro)
{
  if(erro == MAQ_ERRO_DESATIVADA) {
    MaqLiberar(1);
  }
}

// Listas de Erro das Maquinas
char *ErrorListDefault[] = {
    "Erro na comunicacao",
    "Parada Acionada",
    "Falta de fase",
    "Erro na unidade hidraulica",
    "Erro no inversor",
    "Erro no desbobinador",
    "Erro de comunicacao - Inversor",
    "Erro no Corte do Perfil",
    "Erro no Tamanho da Peca",
    "Erro no Posicionamento",
    "Maquina Desativada",
//      "Baixa pressao de ar",
    ""
};

char *ErrorListBanho[] = {
    "Erro na comunicacao",
    "Parada Acionada",
    "Falta de Fase",
    "Erro no Termico da Bomba",
    "Erro no Comando da Bomba",
    "Erro no Fluxostato",
    "Erro no Esguicho 1",
    "Erro no Esguicho 2",
    "Erro no Esguicho 3",
    "Erro no Esguicho 4",
    "Erro na Resistencia 1",
    "Erro na Resistencia 2",
    ""
};

char *ErrorListColunaN[] = {
    "Erro na comunicacao",
    "Parada Acionada",
    "Falta de fase",
    "Erro na unidade hidraulica",
    "Erro no inversor",
    "Erro no servo",
    "Erro de comunicacao - Inversor",
    "Erro de comunicacao - Servo",
    "Erro no Posicionamento",
    "Erro no Corte do Perfil",
    "Maquina Desativada",
    ""
};

char *ErrorListPPLeve[] = {
    "Erro na comunicacao",
    "Parada Acionada",
    "Falta de fase",
    "Erro na unidade hidraulica",
    "Termistor do motor aberto",
    "Erro no inversor",
    "Erro no desbobinador",
    "Erro de comunicacao - Inversor",
    "Erro no Corte do Perfil",
    "Erro de Posicionamento",
    "Maquina Desativada",
    ""
};

char *ErrorListAplanPrensa[] = {
    "Erro na comunicação",
    "Parada Acionada",
    "Falta de fase",
    "Erro na unidade hidráulica",
    "Erro na Prensa - Térmico do Martelo",
    "Baixa pressão de ar",
    "Erro na Prensa - Inversor",
    "Erro na Aplanadora - Servomotor",
    "Erro no Desbobinador",
    "Erro de comunicação - Servomotor",
    "Erro de posicionamento da aplanadora",
    "Porta de Segurança da Prensa Aberta",
    ""
};

// Mapas de I/O das Maquinas
MaqIOMap MaqDefaultIOMap = {
  .InputMask  = 0,
  .Input  = {
      { "Emergência"                      , "images/ihm-ent-emergencia.png"      },
      { "Térmico - Hidráulica"            , "images/ihm-ent-hidr-termico.png"    },
      { "Erro no inversor"                , "images/ihm-ent-inversor-erro.png"   },
      { "Fim de Material"                 , "images/ihm-ent-material-fim.png"    },
      { "Sensor de Corte\nNível Superior" , "images/ihm-ent-corte-superior.png"  },
      { "Sensor de Corte\nNível Inferior" , "images/ihm-ent-corte-inferior.png"  },
      { "Sensor de Piloto\nNível Superior", "images/ihm-ent-piloto-superior.png" },
      { "Sensor de Piloto\nNível Inferior", "images/ihm-ent-piloto-inferior.png" },
      { "Posicionamento\nFinalizado"      , "images/ihm-ent-inversor-posic.png"  },
      { "Falta de Fase"                   , "images/ihm-ent-falta-fase.png"      },
      { "Corte Manual"                    , "images/ihm-ent-manual-corte.png"    },
      { "Avanço Manual"                   , "images/ihm-ent-perfil-avancar.png"  },
      { "Recuo Manual"                    , "images/ihm-ent-perfil-recuar.png"   },
      { "Desbob. OK"                      , "images/ihm-manut-desbob.png"        },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
  },

  .Output  = {
      { "Ligar hidráulica", "images/ihm-manut-hidr-ligar.png"     },
      { "Avança corte"    , "images/ihm-manut-corte-avancar.png"  },
      { "Recua corte"     , "images/ihm-manut-corte-recuar.png"   },
      { "Trava da mesa"   , "images/ihm-manut-mesa-trava.png"     },
      { "Avança piloto"   , "images/ihm-manut-piloto-avancar.png" },
      { "Recua piloto"    , "images/ihm-manut-piloto-recuar.png"  },
      { "Freio do motor"  , "images/ihm-manut-motor-freio.png"    },
      { "Desbobinador"    , "images/ihm-manut-desbob.png"         },
      { "Avançar perfil"  , "images/ihm-manut-perfil-avancar.png" },
      { "Recuar perfil"   , "images/ihm-manut-perfil-recuar.png"  },
      { "Posicionar"      , "images/ihm-manut-perfil-posic.png"   },
      { "Zerar encoder"   , "images/ihm-manut-encoder-zerar.png"  },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
  },
};

// Mapas de I/O da Aplanadora da Prensa de Coluna N
MaqIOMap MaqIOMapPrensaColN = {
  .InputMask  = 0,
  .Input  = {
      { "Emergência"                      , "images/ihm-ent-emergencia.png"      },
      { "Térmico - Hidráulica"            , "images/ihm-ent-hidr-termico.png"    },
      { "Térmico - Martelo"               , "images/ihm-ent-hidr-termico.png"    },
      { "Falta de Fase"                   , "images/ihm-ent-falta-fase.png"      },
      { "Inversor OK"                     , "images/ihm-ent-inversor-erro.png"   },
      { "Aplanadora Fechada"              , "images/ihm-ent-.png" },
      { "Extensao Recuada"                , "images/ihm-ent-.png" },
      { "Desbob. OK"                      , "images/ihm-manut-desbob.png"        },
      { "Avança Chapa"                    , "images/ihm-ent-perfil-avancar.png"  },
      { "Recua Chapa"                     , "images/ihm-ent-perfil-recuar.png"   },
      { "Alim. Prensa OK"                 , "images/ihm-ent-.png" },
      { "Ponto de Parada\nPrensa"         , "images/ihm-ent-.png" },
      { "Bomba Hidr. Ligada"              , "images/ihm-ent-.png" },
      { "Ajuste - Prensa"                 , "images/ihm-ent-.png" },
      { "Bi-Manual 1"                     , "images/ihm-ent-inversor-posic.png"  },
      { "Bi-Manual 2"                     , "images/ihm-ent-.png" },
      { "Desliga Continuo"                , "images/ihm-ent-.png" },
      { "Pressao Ar OK"                   , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
  },

  .Output  = {
      { "Ligar hidráulica", "images/ihm-manut-hidr-ligar.png"     },
      { "Aplan. Fecha"    , "images/ihm-manut-.png" },
      { "Apĺan. Abre"     , "images/ihm-manut-.png" },
      { "Embreagem"       , "images/ihm-manut-.png" },
      { "Ajuste Martelo"  , "images/ihm-manut-.png" },
      { "Liga Lubrif."    , "images/ihm-manut-.png" },
      { "Liga Prensa Inv" , "images/ihm-manut-.png" },
      { "Liga Prensa Norm", "images/ihm-manut-hidr-ligar.png"     },
      { "Habilita Servo"  , "images/ihm-manut-.png" },
      { "Reset Servo"     , "images/ihm-manut-.png" },
      { "Libera Servo"    , "images/ihm-manut-.png" },
      { "Chave Geral"     , "images/ihm-manut-perfil-recuar.png"  },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
  },
};

// Mapas de I/O da Aplanadora da Prensa de Porta-Palete
MaqIOMap MaqIOMapPrensaPP = {
  .InputMask  = 0,
  .Input  = {
      { "Emergência"                      , "images/ihm-ent-emergencia.png"      },
      { "Térmico - Hidráulica"            , "images/ihm-ent-hidr-termico.png"    },
      { "Falta de Fase"                   , "images/ihm-ent-falta-fase.png"      },
      { "Erro no Servo"                   , "images/ihm-ent-inversor-erro.png"   },
      { "Aplanadora Fechada"              , "images/ihm-ent-.png" },
      { "Bomba Hidr. Ligada"              , "images/ihm-ent-.png" },
      { "Avança Chapa"                    , "images/ihm-ent-perfil-avancar.png"  },
      { "Recua Chapa"                     , "images/ihm-ent-perfil-recuar.png"   },
      { "Desbob. OK"                      , "images/ihm-manut-desbob.png"        },
      { "Alim. Prensa OK"                 , "images/ihm-ent-.png" },
      { "Ferram. Liberada"                , "images/ihm-ent-.png" },
      { "Posicionamento\nFinalizado"      , "images/ihm-ent-inversor-posic.png"  },
      { "Abrir Aplanadora"                , "images/ihm-ent-.png" },
      { "Fechar Aplanadora"               , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
  },

  .Output  = {
      { "Ligar hidráulica", "images/ihm-manut-hidr-ligar.png"     },
      { "Mesa Sobe"       , "images/ihm-manut-.png" },
      { "Mesa Desce"      , "images/ihm-manut-.png" },
      { "Apĺan. Abre"     , "images/ihm-manut-.png" },
      { "Aplan. Fecha"    , "images/ihm-manut-.png" },
      { "Aplan. Desce"    , "images/ihm-manut-.png" },
      { "Aplan. Sobe"     , "images/ihm-manut-.png" },
      { "Ventagem"        , "images/ihm-manut-hidr-ligar.png"     },
      { "Porta Prensa"    , "images/ihm-manut-.png" },
      { "Porta Faca"      , "images/ihm-manut-.png" },
      { "Porta Pistao"    , "images/ihm-manut-.png" },
      { "Recuar perfil"   , "images/ihm-manut-perfil-recuar.png"  },
      { "Avançar perfil"  , "images/ihm-manut-perfil-avancar.png" },
      { "Zerar encoder"   , "images/ihm-manut-encoder-zerar.png"  },
      { "Ligar Prensa"    , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
  },
};

// Mapas de I/O da Aplanadora da Prensa de Mezanino
MaqIOMap MaqIOMapPrensaMez = {
  .InputMask  = 0,
  .Input  = {
      { "Emergência"                      , "images/ihm-ent-emergencia.png"      },
      { "Térmico - Hidráulica"            , "images/ihm-ent-hidr-termico.png"    },
      { "Falta de Fase"                   , "images/ihm-ent-falta-fase.png"      },
      { "Erro no Servo"                   , "images/ihm-ent-inversor-erro.png"   },
      { "Aplanadora Fechada"              , "images/ihm-ent-.png" },
      { "Bomba Hidr. Ligada"              , "images/ihm-ent-.png" },
      { "Avança Chapa"                    , "images/ihm-ent-perfil-avancar.png"  },
      { "Recua Chapa"                     , "images/ihm-ent-perfil-recuar.png"   },
      { "Desbob. OK"                      , "images/ihm-manut-desbob.png"        },
      { "Alim. Prensa OK"                 , "images/ihm-ent-.png" },
      { "Ferram. Liberada"                , "images/ihm-ent-.png" },
      { "Posicionamento\nFinalizado"      , "images/ihm-ent-inversor-posic.png"  },
      { "Abrir Aplanadora"                , "images/ihm-ent-.png" },
      { "Fechar Aplanadora"               , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
  },

  .Output  = {
      { "Ligar hidráulica", "images/ihm-manut-hidr-ligar.png"     },
      { "Mesa Sobe"       , "images/ihm-manut-.png" },
      { "Mesa Desce"      , "images/ihm-manut-.png" },
      { "Apĺan. Abre"     , "images/ihm-manut-.png" },
      { "Aplan. Fecha"    , "images/ihm-manut-.png" },
      { "Aplan. Desce"    , "images/ihm-manut-.png" },
      { "Aplan. Sobe"     , "images/ihm-manut-.png" },
      { "Ventagem"        , "images/ihm-manut-hidr-ligar.png"     },
      { "Porta Prensa"    , "images/ihm-manut-.png" },
      { "Porta Faca"      , "images/ihm-manut-.png" },
      { "Porta Pistao"    , "images/ihm-manut-.png" },
      { "Recuar perfil"   , "images/ihm-manut-perfil-recuar.png"  },
      { "Avançar perfil"  , "images/ihm-manut-perfil-avancar.png" },
      { "Zerar encoder"   , "images/ihm-manut-encoder-zerar.png"  },
      { "Ligar Prensa"    , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
  },
};

// Mapas de I/O da Diagonal / Travessa
MaqIOMap MaqIOMapColunaN = {
  .InputMask  = 0,
  .Input  = {
      { "Emergência"                      , "images/ihm-ent-emergencia.png"      },
      { "Térmico - Hidráulica"            , "images/ihm-ent-hidr-termico.png"    },
      { "Falta de Fase"                   , "images/ihm-ent-falta-fase.png"      },
      { "Erro no inversor"                , "images/ihm-ent-inversor-erro.png"   },
      { "Avança Perfil"                   , "images/ihm-ent-perfil-avancar.png"  },
      { "Recua Perfil"                    , "images/ihm-ent-perfil-recuar.png"   },
      { "Corte Manual"                    , "images/ihm-ent-manual-corte.png"    },
      { "Avança Mesa"                     , "images/ihm-ent-perfil-avancar.png"  },
      { "Recua Mesa"                      , "images/ihm-ent-perfil-recuar.png"   },
      { "Sensor de Corte\nNível Superior" , "images/ihm-ent-corte-superior.png"  },
      { "Sensor de Corte\nNível Inferior" , "images/ihm-ent-corte-inferior.png"  },
      { "Sensor de Piloto\nNível Superior", "images/ihm-ent-piloto-superior.png" },
      { "Sensor de Piloto\nNível Inferior", "images/ihm-ent-piloto-inferior.png" },
      { "Trava da Mesa"                   , "images/ihm-manut-mesa-trava.png"    },
      { "Servo OK"                        , "images/ihm-ent-inversor-erro.png"   },
      { "Próximo ao Fim"                  , "images/ihm-ent-.png" },
      { "Vel. Sinc."                      , "images/ihm-ent-inversor-posic.png"  },
      { "Fim de Material"                 , "images/ihm-ent-material-fim.png"    },
      { "Reservado"                       , "images/ihm-ent-.png" },
  },

  .Output  = {
      { "Ligar hidráulica", "images/ihm-manut-hidr-ligar.png"     },
      { "Avança piloto"   , "images/ihm-manut-piloto-avancar.png" },
      { "Trava da mesa"   , "images/ihm-manut-mesa-trava.png"     },
      { "Avança corte"    , "images/ihm-manut-corte-avancar.png"  },
      { "Recua corte"     , "images/ihm-manut-corte-recuar.png"   },
      { "Freio do motor"  , "images/ihm-manut-motor-freio.png"    },
      { "Escravo"         , "images/ihm-ent-inversor-posic.png"   },
      { "Hab. Servo"      , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Avançar perfil"  , "images/ihm-manut-perfil-avancar.png" },
      { "Recuar perfil"   , "images/ihm-manut-perfil-recuar.png"  },
      { "Ventagem"        , "images/ihm-manut-hidr-ligar.png"     },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
  },
};

// Mapas de I/O do Tubo
MaqIOMap MaqIOMapTubo = {
  .InputMask  = 0,
  .Input  = {
      { "Emergência"                      , "images/ihm-ent-emergencia.png"      },
      { "Térmico - Hidráulica"            , "images/ihm-ent-hidr-termico.png"    },
      { "Erro no inversor"                , "images/ihm-ent-inversor-erro.png"   },
      { "Pressão Ar OK"                   , "images/ihm-ent-falta-fase.png"      },
      { "Fim de Material"                 , "images/ihm-ent-material-fim.png"    },
      { "Sensor de Corte\nNível Superior" , "images/ihm-ent-corte-superior.png"  },
      { "Sensor de Corte\nNível Inferior" , "images/ihm-ent-corte-inferior.png"  },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Posicionamento\nFinalizado"      , "images/ihm-ent-inversor-posic.png"  },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Avanço Manual"                   , "images/ihm-ent-perfil-avancar.png"  },
      { "Recuo Manual"                    , "images/ihm-ent-perfil-recuar.png"   },
      { "Corte Manual"                    , "images/ihm-ent-manual-corte.png"    },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Falta de Fase"                   , "images/ihm-ent-falta-fase.png"      },
      { "Reservado"                       , "images/ihm-ent-.png" },
  },

  .Output  = {
      { "Ligar hidráulica", "images/ihm-manut-hidr-ligar.png"     },
      { "Avança corte"    , "images/ihm-manut-corte-avancar.png"  },
      { "Recua corte"     , "images/ihm-manut-corte-recuar.png"   },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Freio do motor"  , "images/ihm-manut-motor-freio.png"    },
      { "Avançar perfil"  , "images/ihm-manut-perfil-avancar.png" },
      { "Recuar perfil"   , "images/ihm-manut-perfil-recuar.png"  },
      { "Posicionar"      , "images/ihm-manut-perfil-posic.png"   },
      { "Zerar encoder"   , "images/ihm-manut-encoder-zerar.png"  },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
  },
};

// Mapa de I/O da Diagonal / Travessa
MaqIOMap MaqIOMapDiagonal = {
  .InputMask  = 0,
  .Input  = {
      { "Emergência"                      , "images/ihm-ent-emergencia.png"      },
      { "Térmico\nHidráulica Corte"       , "images/ihm-ent-hidr-termico.png"    },
      { "Térmico\nHidráulica Mesa"        , "images/ihm-ent-hidr-termico.png"    },
      { "Erro no Servo"                   , "images/ihm-ent-inversor-erro.png"    },
      { "Erro no inversor"                , "images/ihm-ent-inversor-erro.png"   },
      { "Falta de Fase"                   , "images/ihm-ent-falta-fase.png"      },
      { "Fim da Mesa\nLado Direito"       , "images/ihm-ent-corte-superior.png"  },
      { "Fim da Mesa\nLado Esquerdo"      , "images/ihm-ent-corte-inferior.png"  },
      { "Referência - Mesa"               , "images/ihm-ent-piloto-superior.png" },
      { "Mesa Posicionada"                , "images/ihm-ent-inversor-posic.png"  },
      { "Posicionamento\nFinalizado"      , "images/ihm-ent-inversor-posic.png"  },
      { "Sensor de Corte\nNível Superior" , "images/ihm-ent-corte-superior.png"  },
      { "Sensor de Corte\nNível Inferior" , "images/ihm-ent-corte-inferior.png"  },
      { "Sensor de Prensa\nNível Superior", "images/ihm-ent-corte-superior.png"  },
      { "Sensor de Prensa\nNível Inferior", "images/ihm-ent-corte-inferior.png"  },
      { "Avanço Manual"                   , "images/ihm-ent-perfil-avancar.png"  },
      { "Recuo Manual"                    , "images/ihm-ent-perfil-recuar.png"   },
      { "Corte Manual"                    , "images/ihm-ent-manual-corte.png"    },
      { "Reservado"                       , "images/ihm-ent-.png" },
  },

  .Output  = {
      { "Hidráulica Corte", "images/ihm-manut-hidr-ligar.png"     },
      { "Hidráulica Mesa" , "images/ihm-manut-hidr-ligar.png"     },
      { "Avança corte"    , "images/ihm-manut-corte-avancar.png"  },
      { "Recua corte"     , "images/ihm-manut-corte-recuar.png"   },
      { "Ventagem corte"  , "images/ihm-manut-.png"               },
      { "Avança prensa"   , "images/ihm-manut-piloto-avancar.png" },
      { "Recua prensa"    , "images/ihm-manut-piloto-recuar.png"  },
      { "Ventagem prensa" , "images/ihm-manut-.png"               },
      { "Posicionar Mesa" , "images/ihm-manut-motor-freio.png"    },
      { "Zerar enc. Mesa" , "images/ihm-manut-encoder-zerar.png"  },
      { "Avançar perfil"  , "images/ihm-manut-perfil-avancar.png" },
      { "Recuar perfil"   , "images/ihm-manut-perfil-recuar.png"  },
      { "Reservado Servo" , "images/ihm-manut-"                   },
      { "Zerar enc. Servo", "images/ihm-manut-encoder-zerar.png"  },
      { "Reservado Servo" , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
  },
};

// Mapa de I/O da Coluna de Mezanino
MaqIOMap MaqIOMapColunaMezanino = {
  .InputMask  = 0,
  .Input  = {
      { "Emergência"                      , "images/ihm-ent-emergencia.png"      },
      { "Térmico - Hidráulica"            , "images/ihm-ent-hidr-termico.png"    },
      { "Erro no inversor"                , "images/ihm-ent-inversor-erro.png"   },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Sensor de Corte\nNível Superior" , "images/ihm-ent-corte-superior.png"  },
      { "Sensor de Corte\nNível Inferior" , "images/ihm-ent-corte-inferior.png"  },
      { "Sensor de Piloto\nNível Superior", "images/ihm-ent-piloto-superior.png" },
      { "Sensor de Piloto\nNível Inferior", "images/ihm-ent-piloto-inferior.png" },
      { "Posicionamento\nFinalizado"      , "images/ihm-ent-inversor-posic.png"  },
      { "Falta de Fase"                   , "images/ihm-ent-falta-fase.png"      },
      { "Corte Manual"                    , "images/ihm-ent-manual-corte.png"    },
      { "Avanço Manual"                   , "images/ihm-ent-perfil-avancar.png"  },
      { "Recuo Manual"                    , "images/ihm-ent-perfil-recuar.png"   },
      { "Desbob. OK"                      , "images/ihm-manut-desbob.png"        },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
  },

  .Output  = {
      { "Ligar hidráulica", "images/ihm-manut-hidr-ligar.png"     },
      { "Avança corte"    , "images/ihm-manut-corte-avancar.png"  },
      { "Recua corte"     , "images/ihm-manut-corte-recuar.png"   },
      { "Trava da mesa"   , "images/ihm-manut-mesa-trava.png"     },
      { "Avança piloto"   , "images/ihm-manut-piloto-avancar.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Freio do motor"  , "images/ihm-manut-motor-freio.png"    },
      { "Desbobinador"    , "images/ihm-manut-desbob.png"         },
      { "Avançar perfil"  , "images/ihm-manut-perfil-avancar.png" },
      { "Recuar perfil"   , "images/ihm-manut-perfil-recuar.png"  },
      { "Posicionar"      , "images/ihm-manut-perfil-posic.png"   },
      { "Zerar encoder"   , "images/ihm-manut-encoder-zerar.png"  },
      { "Ventagem"        , "images/ihm-manut-hidr-ligar.png"     },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
  },
};

// Mapa de I/O da Viga de Mezanino
MaqIOMap MaqIOMapVigaMezanino = {
  .InputMask  = 0,
  .Input  = {
      { "Emergência"                      , "images/ihm-ent-emergencia.png"      },
      { "Térmico - Hidráulica"            , "images/ihm-ent-hidr-termico.png"    },
      { "Erro no inversor"                , "images/ihm-ent-inversor-erro.png"   },
      { "Fim de Material"                 , "images/ihm-ent-material-fim.png"    },
      { "Sensor de Corte\nNível Superior" , "images/ihm-ent-corte-superior.png"  },
      { "Sensor de Corte\nNível Inferior" , "images/ihm-ent-corte-inferior.png"  },
      { "Sensor de Piloto\nNível Superior", "images/ihm-ent-piloto-superior.png" },
      { "Sensor de Piloto\nNível Inferior", "images/ihm-ent-piloto-inferior.png" },
      { "Posicionamento\nFinalizado"      , "images/ihm-ent-inversor-posic.png"  },
      { "Falta de Fase"                   , "images/ihm-ent-falta-fase.png"      },
      { "Avanço Manual"                   , "images/ihm-ent-perfil-avancar.png"  },
      { "Recuo Manual"                    , "images/ihm-ent-perfil-recuar.png"   },
      { "Corte Manual"                    , "images/ihm-ent-manual-corte.png"    },
      { "Desbob. OK"                      , "images/ihm-manut-desbob.png"        },
      { "Cortar Canto"                    , "images/ihm-ent-manual-corte.png"    },
      { "Sensor de Canto\nNível Superior" , "images/ihm-ent-corte-superior.png"  },
      { "Sensor de Canto\nNível Inferior" , "images/ihm-ent-corte-inferior.png"  },
      { "Térmico - Canto"                 , "images/ihm-ent-hidr-termico.png"    },
      { "Freio acionado"                  , "images/ihm-manut-motor-freio.png"   },
  },

  .Output  = {
      { "Ligar hidráulica", "images/ihm-manut-hidr-ligar.png"     },
      { "Avança corte"    , "images/ihm-manut-corte-avancar.png"  },
      { "Recua corte"     , "images/ihm-manut-corte-recuar.png"   },
      { "Trava da mesa"   , "images/ihm-manut-mesa-trava.png"     },
      { "Avança piloto"   , "images/ihm-manut-piloto-avancar.png" },
      { "Freio do motor"  , "images/ihm-manut-motor-freio.png"    },
      { "Desbobinador"    , "images/ihm-manut-desbob.png"         },
      { "Avançar perfil"  , "images/ihm-manut-perfil-avancar.png" },
      { "Recuar perfil"   , "images/ihm-manut-perfil-recuar.png"  },
      { "Posicionar"      , "images/ihm-manut-perfil-posic.png"   },
      { "Zerar encoder"   , "images/ihm-manut-encoder-zerar.png"  },
      { "Avança canto"    , "images/ihm-manut-corte-avancar.png"  },
      { "Recua canto"     , "images/ihm-manut-corte-recuar.png"   },
      { "Ventagem canto"  , "images/ihm-manut-hidr-ligar.png"     },
      { "Ligar canto"     , "images/ihm-manut-hidr-ligar.png"     },
  },
};

// Mapa de I/O do Sigma
MaqIOMap MaqIOMapSigma = {
  .InputMask  = 0,
  .Input  = {
      { "Emergência"                      , "images/ihm-ent-emergencia.png"      },
      { "Térmico - Hidráulica"            , "images/ihm-ent-hidr-termico.png"    },
      { "Erro no inversor"                , "images/ihm-ent-inversor-erro.png"   },
      { "Falta de Fase"                   , "images/ihm-ent-falta-fase.png"      },
      { "Posicionamento\nFinalizado"      , "images/ihm-ent-inversor-posic.png"  },
      { "Fim de Material"                 , "images/ihm-ent-material-fim.png"    },
      { "Sensor de Corte\nNível Superior" , "images/ihm-ent-corte-superior.png"  },
      { "Sensor de Corte\nNível Inferior" , "images/ihm-ent-corte-inferior.png"  },
      { "Recuo Manual"                    , "images/ihm-ent-perfil-recuar.png"   },
      { "Avanço Manual"                   , "images/ihm-ent-perfil-avancar.png"  },
      { "Corte Manual"                    , "images/ihm-ent-manual-corte.png"    },
      { "Térmico - Canto"                 , "images/ihm-ent-hidr-termico.png"    },
      { "Cortar Canto"                    , "images/ihm-ent-manual-corte.png"    },
      { "Sensor de Canto\nNível Superior" , "images/ihm-ent-corte-superior.png"  },
      { "Sensor de Canto\nNível Inferior" , "images/ihm-ent-corte-inferior.png"  },
      { "Desbob. OK"                      , "images/ihm-manut-desbob.png"        },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Reservado"                       , "images/ihm-ent-.png" },
      { "Freio acionado"                  , "images/ihm-manut-motor-freio.png"   },
  },

  .Output  = {
      { "Ligar hidráulica", "images/ihm-manut-hidr-ligar.png"     },
      { "Avança corte"    , "images/ihm-manut-corte-avancar.png"  },
      { "Recua corte"     , "images/ihm-manut-corte-recuar.png"   },
      { "Ventagem"        , "images/ihm-manut-hidr-ligar.png"     },
      { "Ligar canto"     , "images/ihm-manut-hidr-ligar.png"     },
      { "Avança canto"    , "images/ihm-manut-corte-avancar.png"  },
      { "Recua canto"     , "images/ihm-manut-corte-recuar.png"   },
      { "Ventagem canto"  , "images/ihm-manut-hidr-ligar.png"     },
      { "Freio do motor"  , "images/ihm-manut-motor-freio.png"    },
      { "Avançar perfil"  , "images/ihm-manut-perfil-avancar.png" },
      { "Recuar perfil"   , "images/ihm-manut-perfil-recuar.png"  },
      { "Posicionar"      , "images/ihm-manut-perfil-posic.png"   },
      { "Zerar encoder"   , "images/ihm-manut-encoder-zerar.png"  },
      { "Desbobinador"    , "images/ihm-manut-desbob.png"         },
      { "Reservado"       , "images/ihm-manut-.png" },
      { "Reservado"       , "images/ihm-manut-.png" },
  },
};

MaqIOMap MaqBanhoIOMap = {
  .InputMask  = 0,
  .Input  = {
      { "Emergência"                      , "images/ihm-ent-emergencia.png"    },
      { "Fluxostato"                      , "images/ihm-ent-.png"              },
      { "Térmico Bomba Circ."             , "images/ihm-ent-hidr-termico.png"  },
      { "Esguicho 1 OK"                   , "images/ihm-ent-inversor-erro.png" },
      { "Esguicho 2 OK"                   , "images/ihm-ent-inversor-erro.png" },
      { "Esguicho 3 OK"                   , "images/ihm-ent-inversor-erro.png" },
      { "Reservado"                       , "images/ihm-ent-.png"              },
      { "Reservado"                       , "images/ihm-ent-.png"              },
      { "Reservado"                       , "images/ihm-ent-.png"              },
      { "Bomba de Circ.\nLigada"          , "images/ihm-ent-.png"              },
      { "Resistência 1\nLigada"           , "images/ihm-ent-.png"              },
      { "Resistência 2\nLigada"           , "images/ihm-ent-.png"              },
      { "Reservado"                       , "images/ihm-ent-.png"              },
      { "Reservado"                       , "images/ihm-ent-.png"              },
      { "Falta de Fase"                   , "images/ihm-ent-falta-fase.png"    },
      { "Reservado"                       , "images/ihm-ent-.png"              },
      { "Reservado"                       , "images/ihm-ent-.png"              },
      { "Reservado"                       , "images/ihm-ent-.png"              },
      { "Termostato"                      , "images/ihm-ent-.png"              },
  },

  .Output  = {
      { "Ligar Bomba Circ."  , "images/ihm-manut-hidr-ligar.png" },
      { "Ligar Resistência 1", "images/ihm-manut-.png"           },
      { "Ligar Resistência 2", "images/ihm-manut-.png"           },
      { "Ligar Esguicho 1"   , "images/ihm-manut-.png"           },
      { "Reset Esguicho 1"   , "images/ihm-manut-.png"           },
      { "Ligar Esguicho 2"   , "images/ihm-manut-.png"           },
      { "Reset Esguicho 2"   , "images/ihm-manut-.png"           },
      { "Ligar Esguicho 3"   , "images/ihm-manut-.png"           },
      { "Reset Esguicho 3"   , "images/ihm-manut-.png"           },
      { "Reservado"          , "images/ihm-manut-.png"           },
      { "Reservado"          , "images/ihm-manut-.png"           },
      { "Ventilador Secagem" , "images/ihm-manut-.png"           },
      { "Sirene"             , "images/ihm-manut-.png"           },
      { "Aviso Sonoro Erro"  , "images/ihm-manut-.png"           },
      { "Parar Linha"        , "images/ihm-manut-.png"           },
      { "Aviso Pintura"      , "images/ihm-manut-.png"           },
  },
};

// Mapa de I/O do Porta-Palete Leve
MaqIOMap MaqIOMapPPLeve = {
    .InputMask  = 0,
    .Input  = {
        { "Emergência"                      , "images/ihm-ent-emergencia.png"      },
        { "Térmico - Hidráulica"            , "images/ihm-ent-hidr-termico.png"    },
        { "Falta de Fase"                   , "images/ihm-ent-falta-fase.png"      },
        { "Erro no inversor"                , "images/ihm-ent-inversor-erro.png"   },
        { "Fim de Material"                 , "images/ihm-ent-material-fim.png"    },
        { "Sensor de Corte\nNível Superior" , "images/ihm-ent-corte-superior.png"  },
        { "Sensor de Corte\nNível Inferior" , "images/ihm-ent-corte-inferior.png"  },
        { "Sensor de Piloto\nNível Superior", "images/ihm-ent-piloto-superior.png" },
        { "Sensor de Piloto\nNível Inferior", "images/ihm-ent-piloto-inferior.png" },
        { "Bomba Hidr. Ligada"              , "images/ihm-ent-.png"                },
        { "Avanço Manual"                   , "images/ihm-ent-perfil-avancar.png"  },
        { "Recuo Manual"                    , "images/ihm-ent-perfil-recuar.png"   },
        { "Corte Manual"                    , "images/ihm-ent-manual-corte.png"    },
        { "Desbob. OK"                      , "images/ihm-manut-desbob.png"        },
        { "Termistor"                       , "images/ihm-ent-.png"                },
        { "Sensor de Furo\nPrincipal"       , "images/ihm-ent-.png"                },
        { "Sensor de Furo\nAuxiliar"        , "images/ihm-ent-.png"                },
        { "Reservado"                       , "images/ihm-ent-.png"                },
        { "Reservado"                       , "images/ihm-ent-.png"                },
    },

    .Output  = {
        { "Ligar hidráulica", "images/ihm-manut-hidr-ligar.png"     },
        { "Avança corte"    , "images/ihm-manut-corte-avancar.png"  },
        { "Recua corte"     , "images/ihm-manut-corte-recuar.png"   },
        { "Trava da mesa"   , "images/ihm-manut-mesa-trava.png"     },
        { "Avança piloto"   , "images/ihm-manut-piloto-avancar.png" },
        { "Freio do motor"  , "images/ihm-manut-motor-freio.png"    },
        { "Desbobinador"    , "images/ihm-manut-desbob.png"         },
        { "Avançar perfil"  , "images/ihm-manut-perfil-avancar.png" },
        { "Recuar perfil"   , "images/ihm-manut-perfil-recuar.png"  },
        { "Reservado"       , "images/ihm-manut-.png"               },
        { "Reservado"       , "images/ihm-manut-.png"               },
        { "Reservado"       , "images/ihm-manut-.png"               },
        { "Reservado"       , "images/ihm-manut-.png"               },
        { "Reservado"       , "images/ihm-manut-.png"               },
        { "Reservado"       , "images/ihm-manut-.png"               },
        { "Reservado"       , "images/ihm-manut-.png"               },
  },
};

// Mapa de I/O do Porta-Palete Pesado
MaqIOMap MaqIOMapPPPesado = {
  .InputMask  = 0,
  .Input  = {
      { "Emergência"                      , "images/ihm-ent-emergencia.png"      },
      { "Térmico - Hidráulica"            , "images/ihm-ent-hidr-termico.png"    },
      { "Falta de Fase"                   , "images/ihm-ent-falta-fase.png"      },
      { "Erro no inversor 1"              , "images/ihm-ent-inversor-erro.png"   },
      { "Erro no inversor 2"              , "images/ihm-ent-inversor-erro.png"   },
      { "Fim de Material"                 , "images/ihm-ent-material-fim.png"    },
      { "Sensor de Corte\nNível Superior" , "images/ihm-ent-corte-superior.png"  },
      { "Sensor de Corte\nNível Inferior" , "images/ihm-ent-corte-inferior.png"  },
      { "Sensor de Piloto\nNível Superior", "images/ihm-ent-piloto-superior.png" },
      { "Sensor de Piloto\nNível Inferior", "images/ihm-ent-piloto-inferior.png" },
      { "Bomba Hidr. Ligada"              , "images/ihm-ent-.png"                },
      { "Avanço Manual"                   , "images/ihm-ent-perfil-avancar.png"  },
      { "Recuo Manual"                    , "images/ihm-ent-perfil-recuar.png"   },
      { "Corte Manual"                    , "images/ihm-ent-manual-corte.png"    },
      { "Sensor de Furo\nPrincipal"       , "images/ihm-ent-.png"                },
      { "Sensor de Furo\nAuxiliar"        , "images/ihm-ent-.png"                },
      { "Reservado"                       , "images/ihm-ent-.png"                },
      { "Reservado"                       , "images/ihm-ent-.png"                },
      { "Reservado"                       , "images/ihm-ent-.png"                },
  },

  .Output  = {
      { "Ligar hidráulica", "images/ihm-manut-hidr-ligar.png"     },
      { "Avança corte"    , "images/ihm-manut-corte-avancar.png"  },
      { "Recua corte"     , "images/ihm-manut-corte-recuar.png"   },
      { "Trava da mesa"   , "images/ihm-manut-mesa-trava.png"     },
      { "Avança piloto"   , "images/ihm-manut-piloto-avancar.png" },
      { "Avançar perfil"  , "images/ihm-manut-perfil-avancar.png" },
      { "Recuar perfil"   , "images/ihm-manut-perfil-recuar.png"  },
      { "Velocidade Alta" , "images/ihm-manut-.png"               },
      { "Velocidade Baixa", "images/ihm-manut-.png"               },
      { "Ligar Inversor"  , "images/ihm-manut-.png"               },
      { "Reservado"       , "images/ihm-manut-.png"               },
      { "Reservado"       , "images/ihm-manut-.png"               },
      { "Reservado"       , "images/ihm-manut-.png"               },
      { "Reservado"       , "images/ihm-manut-.png"               },
      { "Reservado"       , "images/ihm-manut-.png"               },
      { "Reservado"       , "images/ihm-manut-.png"               },
  },
};

// Mapa de I/O do Porta-Palete Normal
MaqIOMap MaqIOMapPPNormal = {
  .InputMask  = 0,
  .Input  = {
      { "Emergência"                      , "images/ihm-ent-emergencia.png"      },
      { "Térmico - Hidráulica"            , "images/ihm-ent-hidr-termico.png"    },
      { "Falta de Fase"                   , "images/ihm-ent-falta-fase.png"      },
      { "Erro no inversor"                , "images/ihm-ent-inversor-erro.png"   },
      { "Bomba Hidr. Ligada"              , "images/ihm-ent-.png"                },
      { "Erro no inversor 2"              , "images/ihm-ent-inversor-erro.png"   },
      { "Fim de Material"                 , "images/ihm-ent-material-fim.png"    },
      { "Avanço Manual"                   , "images/ihm-ent-perfil-avancar.png"  },
      { "Recuo Manual"                    , "images/ihm-ent-perfil-recuar.png"   },
      { "Corte Manual"                    , "images/ihm-ent-manual-corte.png"    },
      { "Sensor de Piloto\nNível Superior", "images/ihm-ent-piloto-superior.png" },
      { "Sensor de Piloto\nNível Inferior", "images/ihm-ent-piloto-inferior.png" },
      { "Sensor de Corte\nNível Superior" , "images/ihm-ent-corte-superior.png"  },
      { "Sensor de Corte\nNível Inferior" , "images/ihm-ent-corte-inferior.png"  },
      { "Placa Fusível"                   , "images/ihm-ent-.png"                },
      { "Reservado"                       , "images/ihm-ent-.png"                },
      { "Sensor de Furo\nPrincipal"       , "images/ihm-ent-.png"                },
      { "Sensor de Furo\nAuxiliar"        , "images/ihm-ent-.png"                },
      { "Reservado"                       , "images/ihm-ent-.png"                },
  },

  .Output  = {
      { "Ligar hidráulica", "images/ihm-manut-hidr-ligar.png"     },
      { "Avança corte"    , "images/ihm-manut-corte-avancar.png"  },
      { "Recua corte"     , "images/ihm-manut-corte-recuar.png"   },
      { "Avança piloto"   , "images/ihm-manut-piloto-avancar.png" },
      { "Trava da mesa"   , "images/ihm-manut-mesa-trava.png"     },
      { "Avançar perfil"  , "images/ihm-manut-perfil-avancar.png" },
      { "Recuar perfil"   , "images/ihm-manut-perfil-recuar.png"  },
      { "Velocidade Alta" , "images/ihm-manut-.png"               },
      { "Velocidade Baixa", "images/ihm-manut-.png"               },
      { "Reservado"       , "images/ihm-manut-.png"               },
      { "Reservado"       , "images/ihm-manut-.png"               },
      { "Reservado"       , "images/ihm-manut-.png"               },
      { "Reservado"       , "images/ihm-manut-.png"               },
      { "Reservado"       , "images/ihm-manut-.png"               },
      { "Reservado"       , "images/ihm-manut-.png"               },
      { "Reservado"       , "images/ihm-manut-.png"               },
  },
};

// Funcoes de parametros de maquinas
MaqConfig MaqConfigList[] = {
    { // Porta-Palete Pesado
        .ID               = MAQ_ID_PERF_PP_PESADO,
        .Name             = "Porta-Palete Pesado",
        .Line             = "PPPES",
        .Machine          = "PPPES",
        .ClpAddr          = "192.168.2.253",
        .AbaHome          = NTB_ABA_HOME,
        .AbaManut         = NTB_ABA_MANUT,
		.AbaOperAuto      = NTB_ABA_OPERAR,
        .AbaConfigMais    = 0,
        .UseLogin         = TRUE,
        .UseIndet         = TRUE,
        .NeedMaqInit      = FALSE,
        .MaqModeCV        = FALSE,
        .InverterComandos = FALSE,
        .IOMap            = &MaqIOMapPPPesado,
        .fncOnInit        = NULL,
        .fncOnError       = MaqErro,
        .fncOnOperAuto    = NULL,
        .fncOnAuto        = NULL,
        .fncTimerUpdate   = NULL,
        .ErrorList        = ErrorListPPLeve,
        .Alertas          = 0x100, // Erro de Corte
    },
    { // Porta-Palete Leve
        .ID               = MAQ_ID_PERF_PP_LEVE,
        .Name             = "Porta-Palete Leve",
        .Line             = "PPLEV",
        .Machine          = "PPLEV",
        .ClpAddr          = "192.168.2.251",
        .AbaHome          = NTB_ABA_HOME,
        .AbaManut         = NTB_ABA_MANUT,
		.AbaOperAuto      = NTB_ABA_OPERAR,
        .AbaConfigMais    = 0,
        .UseLogin         = TRUE,
        .UseIndet         = TRUE,
        .NeedMaqInit      = FALSE,
        .MaqModeCV        = FALSE,
        .InverterComandos = FALSE,
        .IOMap            = &MaqIOMapPPLeve,
        .fncOnInit        = NULL,
        .fncOnError       = MaqErro,
        .fncOnOperAuto    = NULL,
        .fncOnAuto        = NULL,
        .fncTimerUpdate   = NULL,
        .ErrorList        = ErrorListPPLeve,
        .Alertas          = 0,
    },
    { // Aplanadora da Prensa de Mezanino
        .ID               = MAQ_ID_APLAN_MEZANINO,
        .Name             = "Aplanadora da Prensa de Mezanino",
        .Line             = "MZAPL",
        .Machine          = "MZAPL",
        .ClpAddr          = "192.168.2.249",
        .AbaHome          = NTB_ABA_HOME_PRENSA,
        .AbaManut         = NTB_ABA_MANUT,
		.AbaOperAuto      = NTB_ABA_PRENSA_CADPROG,
        .AbaConfigMais    = 0,
        .UseLogin         = TRUE,
        .UseIndet         = TRUE,
        .NeedMaqInit      = FALSE,
        .MaqModeCV        = FALSE,
        .InverterComandos = FALSE,
        .IOMap            = &MaqIOMapPrensaMez,
        .fncOnInit        = ProgPrensa_Init,
        .fncOnError       = MaqErro,
        .fncOnOperAuto    = ProgPrensa_Produzir,
        .fncOnAuto        = NULL,
        .fncTimerUpdate   = PrensaMezanino_Update,
        .ErrorList        = ErrorListDefault,
        .Alertas          = 0x200, // Erro de Posicionamento
    },
    { // Tubo Antiga
        .ID               = MAQ_ID_PERF_TUBO,
        .Name             = "Tubo",
        .Line             = "TUBO",
        .Machine          = "TUBO",
        .ClpAddr          = "192.168.2.247",
        .AbaHome          = NTB_ABA_HOME,
        .AbaManut         = NTB_ABA_MANUT,
		.AbaOperAuto      = NTB_ABA_OPERAR,
        .AbaConfigMais    = 0,
        .UseLogin         = TRUE,
        .UseIndet         = TRUE,
        .NeedMaqInit      = FALSE,
        .MaqModeCV        = FALSE,
        .InverterComandos = TRUE,
        .IOMap            = &MaqIOMapTubo,
        .fncOnInit        = NULL,
        .fncOnError       = NULL,
        .fncOnOperAuto    = NULL,
        .fncOnAuto        = NULL,
        .fncTimerUpdate   = NULL,
        .ErrorList        = ErrorListDefault,
        .Alertas          = 0,
    },
    { // Coluna N
        .ID               = MAQ_ID_PERF_COLUNA_N,
        .Name             = "Coluna N",
        .Line             = "COLN",
        .Machine          = "COLN",
        .ClpAddr          = "192.168.2.245",
        .AbaHome          = NTB_ABA_HOME,
        .AbaManut         = NTB_ABA_MANUT,
		.AbaOperAuto      = NTB_ABA_OPERAR,
        .AbaConfigMais    = NTB_ABA_CONFIG_COLN,
        .UseLogin         = TRUE,
        .UseIndet         = TRUE,
        .NeedMaqInit      = TRUE,
        .MaqModeCV        = TRUE,
        .InverterComandos = TRUE,
        .IOMap            = &MaqIOMapColunaN,
        .fncOnInit        = NULL,
        .fncOnError       = MaqErro,
        .fncOnOperAuto    = NULL,
        .fncOnAuto        = NULL,
        .fncTimerUpdate   = NULL,
        .ErrorList        = ErrorListColunaN,
        .Alertas          = (3UL << 8), // Erro de Posicionamento e Corte
    },
    { // Diagonal e Travessa
        .ID               = MAQ_ID_TRAV_DIAGONAL,
        .Name             = "Diagonal / Travessa",
        .Line             = "TRDIA",
        .Machine          = "TRDIA",
        .ClpAddr          = "192.168.2.243",
        .AbaHome          = NTB_ABA_HOME,
        .AbaManut         = NTB_ABA_MANUT,
		.AbaOperAuto      = NTB_ABA_OPERAR,
        .AbaConfigMais    = NTB_ABA_CONFIG_DIAGONAL,
        .UseLogin         = TRUE,
        .UseIndet         = TRUE,
        .NeedMaqInit      = FALSE,
        .MaqModeCV        = FALSE,
        .InverterComandos = TRUE,
        .IOMap            = &MaqIOMapDiagonal,
        .fncOnInit        = Diagonal_Init,
        .fncOnError       = NULL,
        .fncOnOperAuto    = NULL,
        .fncOnAuto        = Diagonal_Auto,
        .fncTimerUpdate   = NULL,
        .ErrorList        = ErrorListDefault,
        .Alertas          = (1UL << 9), // Erro de Posicionamento
    },
    { // Viga do Mezanino
        .ID               = MAQ_ID_VIGA_MEZANINO,
        .Name             = "Viga do Mezanino",
        .Line             = "MZVIG",
        .Machine          = "MZVIG",
        .ClpAddr          = "192.168.2.241",
        .AbaHome          = NTB_ABA_HOME,
        .AbaManut         = NTB_ABA_MANUT,
		.AbaOperAuto      = NTB_ABA_OPERAR,
        .AbaConfigMais    = 0,
        .UseLogin         = TRUE,
        .UseIndet         = TRUE,
        .NeedMaqInit      = FALSE,
        .MaqModeCV        = FALSE,
        .InverterComandos = FALSE,
        .IOMap            = &MaqIOMapVigaMezanino,
        .fncOnInit        = NULL,
        .fncOnError       = NULL,
        .fncOnOperAuto    = NULL,
        .fncOnAuto        = NULL,
        .fncTimerUpdate   = NULL,
        .ErrorList        = ErrorListDefault,
        .Alertas          = 0,
    },
    { // Coluna do Mezanino
        .ID               = MAQ_ID_COL_MEZANINO,
        .Name             = "Coluna do Mezanino",
        .Line             = "MZCOL",
        .Machine          = "MZCOL",
        .ClpAddr          = "192.168.2.239",
        .AbaHome          = NTB_ABA_HOME,
        .AbaManut         = NTB_ABA_MANUT,
		.AbaOperAuto      = NTB_ABA_OPERAR,
        .AbaConfigMais    = 0,
        .UseLogin         = TRUE,
        .UseIndet         = TRUE,
        .NeedMaqInit      = FALSE,
        .MaqModeCV        = FALSE,
        .InverterComandos = FALSE,
        .IOMap            = &MaqIOMapColunaMezanino,
        .fncOnInit        = NULL,
        .fncOnError       = NULL,
        .fncOnOperAuto    = NULL,
        .fncOnAuto        = NULL,
        .fncTimerUpdate   = NULL,
        .ErrorList        = ErrorListDefault,
        .Alertas          = 0,
    },
    { // Viga Sigma
        .ID               = MAQ_ID_PERF_SIGMA,
        .Name             = "Perfiladeira Sigma",
        .Line             = "PPSIG",
        .Machine          = "PPSIG",
        .ClpAddr          = "192.168.2.237",
        .AbaHome          = NTB_ABA_HOME,
        .AbaManut         = NTB_ABA_MANUT,
		.AbaOperAuto      = NTB_ABA_OPERAR,
        .AbaConfigMais    = 0,
        .UseLogin         = TRUE,
        .UseIndet         = TRUE,
        .NeedMaqInit      = FALSE,
        .MaqModeCV        = FALSE,
        .InverterComandos = FALSE,
        .IOMap            = &MaqIOMapSigma,
        .fncOnInit        = NULL,
        .fncOnError       = NULL,
        .fncOnOperAuto    = NULL,
        .fncOnAuto        = NULL,
        .fncTimerUpdate   = NULL,
        .ErrorList        = ErrorListDefault,
        .Alertas          = 0,
    },
    { // Banho
        .ID               = MAQ_ID_BANHO,
        .Name             = "Banho",
        .Line             = "BANHO",
        .Machine          = "BANHO",
        .ClpAddr          = "192.168.2.235",
        .AbaHome          = NTB_ABA_HOME_BANHO,
        .AbaManut         = NTB_ABA_MANUT,
		.AbaOperAuto      = NTB_ABA_OPERAR,
        .AbaConfigMais    = 0,
        .UseLogin         = FALSE,
        .UseIndet         = FALSE,
        .NeedMaqInit      = FALSE,
        .MaqModeCV        = FALSE,
        .InverterComandos = FALSE,
        .IOMap            = &MaqBanhoIOMap,
        .fncOnInit        = Banho_Init,
        .fncOnError       = Banho_Erro,
        .fncOnOperAuto    = NULL,
        .fncOnAuto        = NULL,
        .fncTimerUpdate   = NULL,
        .ErrorList        = ErrorListBanho,
        .Alertas          = 0,
    },
    { // Porta-Palete Normal
        .ID               = MAQ_ID_PERF_PP_NORMAL,
        .Name             = "Porta-Palete",
        .Line             = "PPNRM",
        .Machine          = "PPNRM",
        .ClpAddr          = "192.168.2.233",
        .AbaHome          = NTB_ABA_HOME,
        .AbaManut         = NTB_ABA_MANUT,
		.AbaOperAuto      = NTB_ABA_OPERAR,
        .AbaConfigMais    = 0,
        .UseLogin         = TRUE,
        .UseIndet         = TRUE,
        .NeedMaqInit      = FALSE,
        .MaqModeCV        = FALSE,
        .InverterComandos = FALSE,
        .IOMap            = &MaqIOMapPPNormal,
        .fncOnInit        = NULL,
        .fncOnError       = MaqErro,
        .fncOnOperAuto    = NULL,
        .fncOnAuto        = NULL,
        .fncTimerUpdate   = NULL,
        .ErrorList        = ErrorListPPLeve,
        .Alertas          = 0x100, // Erro de Corte
    },
	{
        .ID               = MAQ_ID_APLAN_PP,
        .Name             = "Aplanadora da Prensa de Porta-Palete",
        .Line             = "PPAPL",
        .Machine          = "PPAPL",
        .ClpAddr          = "192.168.2.231",
        .AbaHome          = NTB_ABA_HOME_PRENSA,
        .AbaManut         = NTB_ABA_MANUT,
        .AbaOperAuto      = NTB_ABA_PRENSAPF_PROD,
        .AbaConfigMais    = NTB_ABA_CONFIG_PRENSA,
        .UseLogin         = TRUE,
        .UseIndet         = TRUE,
        .NeedMaqInit      = FALSE,
        .MaqModeCV        = FALSE,
        .InverterComandos = FALSE,
        .IOMap            = &MaqIOMapPrensaPP,
        .fncOnInit        = ProgPrensaPassoFixo_Init,
        .fncOnError       = MaqErro,
        .fncOnOperAuto    = ProgPrensaPassoFixo_Produzir,
        .fncOnAuto        = NULL,
        .fncTimerUpdate   = PrensaPassoFixo_Update,
        .ErrorList        = ErrorListAplanPrensa,
        .Alertas          = 0x200, // Erro de Posicionamento
	},
	{
        .ID               = MAQ_ID_APLAN_COLUNA_N,
        .Name             = "Aplanadora da Prensa de Coluna N",
        .Line             = "CNAPL",
        .Machine          = "CNAPL",
        .ClpAddr          = "192.168.2.229",
        .AbaHome          = NTB_ABA_HOME_PRENSA,
        .AbaManut         = NTB_ABA_MANUT,
        .AbaOperAuto      = NTB_ABA_PRENSAPF_PROD,
        .AbaConfigMais    = NTB_ABA_CONFIG_PRENSA,
        .UseLogin         = TRUE,
        .UseIndet         = TRUE,
        .NeedMaqInit      = FALSE,
        .MaqModeCV        = FALSE,
        .InverterComandos = FALSE,
        .IOMap            = &MaqIOMapPrensaColN,
        .fncOnInit        = ProgPrensaPassoFixo_Init,
        .fncOnError       = MaqErro,
        .fncOnOperAuto    = ProgPrensaPassoFixo_Produzir,
        .fncOnAuto        = NULL,
        .fncTimerUpdate   = PrensaPassoFixo_Update,
        .ErrorList        = ErrorListAplanPrensa,
        .Alertas          = 0x600, // Erro de Posicionamento e da Porta de Seguranca da Prensa
	},
    { // Final
        .ID = NULL
    }
};

MaqConfig MaqConfigDefault = {
    .ID               = MAQ_ID_DEFAULT,
    .Name             = "Maquina de Teste",
    .Line             = "TESTE",
    .Machine          = "TESTE",
//    .ClpAddr          = "192.168.1.254",
    .ClpAddr          = "192.168.0.192",
    .AbaHome          = NTB_ABA_HOME_PRENSA,
    .AbaManut         = NTB_ABA_MANUT,
    .AbaOperAuto      = NTB_ABA_PRENSAPF_PROD,
    .AbaConfigMais    = NTB_ABA_CONFIG_PRENSA,
    .UseLogin         = TRUE,
    .UseIndet         = TRUE,
    .NeedMaqInit      = FALSE,
    .MaqModeCV        = FALSE,
    .InverterComandos = FALSE,
    .IOMap            = &MaqIOMapPrensaPP,
    .fncOnInit        = ProgPrensaPassoFixo_Init,
    .fncOnError       = MaqErro,
    .fncOnOperAuto    = ProgPrensaPassoFixo_Produzir,
    .fncOnAuto        = NULL,
    .fncTimerUpdate   = PrensaPassoFixo_Update,
    .ErrorList        = ErrorListAplanPrensa,
    .Alertas          = 0x200, // Erro de Posicionamento
};

MaqConfig *MaqConfigCurrent = &MaqConfigDefault;

extern GtkBuilder *builder;

void MaqConfig_SetMachine(char *ID)
{
  int i;
  char tmp[100];
  GtkWidget *wdg;
  GtkImage  *img;

  // Procura a maquina
  MaqConfigCurrent = &MaqConfigDefault;
  for(i=0; MaqConfigList[i].ID != NULL; i++) {
    if(!strcmp(MaqConfigList[i].ID, ID)) {
      MaqConfigCurrent = &MaqConfigList[i];
      break;
    }
  }

  // Agora configura as entradas e saidas da tela de manutencao

  // Loop de entradas, finaliza quando acabar objetos ou atingir limite
  for(i = 0; i < MAQ_INPUT_MAX; i++) {
    sprintf(tmp, "lblManutEnt%02d", i);
    wdg = GTK_WIDGET(gtk_builder_get_object(builder, tmp));
    if(wdg == NULL) // Acabaram as entradas
      break; // Sai do loop

    gtk_label_set_label(GTK_LABEL(wdg), MaqConfigCurrent->IOMap->Input[i].NameIO);

    sprintf(tmp, "imgManutEnt%02d", i);
    img = GTK_IMAGE(gtk_builder_get_object(builder, tmp));
    if(img) {
      gtk_image_set_from_file(img, MaqConfigCurrent->IOMap->Input[i].NameFile);
    }
  }

  // Loop de saidas, finaliza quando acabar objetos ou atingir limite
  for(i = 0; i < MAQ_OUTPUT_MAX; i++) {
    sprintf(tmp, "lblManutSai%02d", i);
    wdg = GTK_WIDGET(gtk_builder_get_object(builder, tmp));
    if(wdg == NULL) // Acabaram as saidas
      break; // Sai do loop

    gtk_label_set_label(GTK_LABEL(wdg), MaqConfigCurrent->IOMap->Output[i].NameIO);

    sprintf(tmp, "imgManutSai%02d", i);
    img = GTK_IMAGE(gtk_builder_get_object(builder, tmp));
    if(img) {
      gtk_image_set_from_file(img, MaqConfigCurrent->IOMap->Output[i].NameFile);
    }
  }
}

MaqConfig * MaqConfig_GetMachine(int index)
{
  int i;

  for(i=0; MaqConfigList[i].ID != NULL; i++) {
    if(i == index) {
      return &MaqConfigList[i];
    }
  }

  return &MaqConfigDefault;
}

int MaqConfig_GetActive(void)
{
  int i;

  for(i=0; MaqConfigList[i].ID != NULL; i++) {
    if(MaqConfigCurrent == &MaqConfigList[i]) {
      return i;
    }
  }

  return -1;
}

int MaqInit(void)
{
  if(MaqConfigCurrent == NULL) return 0;

  if(MaqConfigCurrent->fncOnInit != NULL)
    return (*MaqConfigCurrent->fncOnInit)();

  return 1;
}

void MaqError(int error)
{
  if(MaqConfigCurrent && MaqConfigCurrent->fncOnError != NULL)
    (*MaqConfigCurrent->fncOnError)(error);
}

void MaqAuto(int ativo)
{
  if(MaqConfigCurrent && MaqConfigCurrent->fncOnAuto != NULL)
    (*MaqConfigCurrent->fncOnAuto)(ativo);
}

// Função que retorna checksum de ponteiro
unsigned long CalcCheckSum(void *ptr, unsigned int tam)
{
  unsigned char *cptr = (unsigned char *)(ptr);
  unsigned long checksum = 0;

  while(tam--)
    {
    checksum += *cptr;
    cptr++;
    }

  return checksum;
}

struct strMaqReply {
  struct MODBUS_Reply modbus_reply;
  uint32_t ready;
};

void retMaqMB(void *dt, void *res)
{
  struct strMaqReply *rp = (struct strMaqReply *)res;

  rp->modbus_reply = ((union uniIPCMQ_Data *)dt)->modbus_reply;
  rp->ready = 1; // Recebida resposta
}

void EnviarMB(struct strIPCMQ_Message *ipc_msg, struct strMaqReply *rp)
{
#ifndef DEBUG_PC_NOETH
  IPCMQ_Main_Enviar(ipc_msg);
  while(!rp->ready) {
    IPC_Update();
    usleep(100);
  }
#else
  rp->modbus_reply.ExceptionCode = MODBUS_EXCEPTION_NONE;
#endif
}

// Funcao que sincroniza a estrutura de parametros com o clp. Retorna default_value se erro
uint16_t MaqLerRegistrador(uint16_t reg, uint16_t default_value)
{
  uint16_t ret = default_value;
  struct strMaqReply rp;
  struct strIPCMQ_Message ipc_msg;

  memset(&rp, 0, sizeof(rp));

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = retMaqMB;
  ipc_msg.res   = (void *)&rp;
  ipc_msg.data.modbus_query.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
  ipc_msg.data.modbus_query.data.read_holding_registers.start = reg;
  ipc_msg.data.modbus_query.data.read_holding_registers.quant = 1;

  EnviarMB(&ipc_msg, &rp);

  if(rp.modbus_reply.ExceptionCode == MODBUS_EXCEPTION_NONE) { // Sem erro de comunicacao
    ret = CONV_PCHAR_UINT16(rp.modbus_reply.reply.read_holding_registers.data);
    printf("Lido %d (0x%04x) de %d\n", ret, ret, reg);
  }

  return ret;
}

void MaqGravarRegistrador(uint16_t reg, uint16_t val)
{
  struct strIPCMQ_Message ipc_msg;

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = NULL;
  ipc_msg.res   = NULL;
  ipc_msg.data.modbus_query.function_code = MODBUS_FC_WRITE_SINGLE_REGISTER;
  ipc_msg.data.modbus_query.data.write_single_register.address = reg;
  ipc_msg.data.modbus_query.data.write_single_register.val = val;

  printf("Escrevendo %d (0x%04x) em %d\n", val, val, reg);

  IPCMQ_Main_Enviar(&ipc_msg);
}

uint16_t MaqLerFlags(void)
{
  static uint16_t flags = 0;

  flags = MaqLerRegistrador(MAQ_REG_FLAGS, flags);

  return flags;
}

void MaqConfigFlags(uint16_t flags)
{
  printf("flags = %d\n", flags);
  MaqGravarRegistrador(MAQ_REG_FLAGS, flags);
}

// Funcao que sincroniza a estrutura de parametros com o clp
int MaqSync(unsigned int mask)
{
  static unsigned int SyncFlags = 0;
  unsigned long rel_motor_perfil = 0;

  printf("Sincronizando parametros da maquina:\n");

  if(mask & MAQ_SYNC_PERFIL) {
    if(maq_param.perfil.diam_rolo) {
      rel_motor_perfil = (unsigned long)((maq_param.perfil.fator * 1000000) / ((float)(3.14159 * maq_param.perfil.diam_rolo)));
    }

    printf("rel_motor_perfil...........................: %ld\n", rel_motor_perfil);
    printf("maq_param.perfil.fator.....................: %f\n" , maq_param.perfil.fator);
    printf("maq_param.perfil.diam_rolo.................: %d\n" , maq_param.perfil.diam_rolo);
    printf("maq_param.perfil.auto_vel..................: %d\n" , maq_param.perfil.auto_vel);
    printf("maq_param.perfil.auto_acel.................: %f\n" , maq_param.perfil.auto_acel);
    printf("maq_param.perfil.auto_desacel..............: %f\n" , maq_param.perfil.auto_desacel);
    printf("maq_param.perfil.manual_vel................: %d\n" , maq_param.perfil.manual_vel);
    printf("maq_param.perfil.manual_acel...............: %f\n" , maq_param.perfil.manual_acel);
    printf("maq_param.perfil.manual_desacel............: %f\n" , maq_param.perfil.manual_desacel);

    MaqGravarRegistrador(MAQ_REG_PERF_AUTO_VEL    ,                maq_param.perfil.auto_vel           );
    MaqGravarRegistrador(MAQ_REG_PERF_AUTO_ACEL   , (unsigned int)(maq_param.perfil.auto_acel     *100));
    MaqGravarRegistrador(MAQ_REG_PERF_AUTO_DESACEL, (unsigned int)(maq_param.perfil.auto_desacel  *100));
    MaqGravarRegistrador(MAQ_REG_PERF_MAN_VEL     ,                maq_param.perfil.manual_vel         );
    MaqGravarRegistrador(MAQ_REG_PERF_MAN_ACEL    , (unsigned int)(maq_param.perfil.manual_acel   *100));
    MaqGravarRegistrador(MAQ_REG_PERF_MAN_DESACEL , (unsigned int)(maq_param.perfil.manual_desacel*100));
    MaqGravarRegistrador(MAQ_REG_PERF_FATOR_LOW   , (rel_motor_perfil    ) & 0XFFFF);
    MaqGravarRegistrador(MAQ_REG_PERF_FATOR_HIGH  , (rel_motor_perfil>>16) & 0XFFFF);

    SyncFlags |= MAQ_MODO_PERF_SYNC;
  }

  if(mask & MAQ_SYNC_ENCODER) {
    printf("maq_param.encoder.fator....................: %f\n", maq_param.encoder.fator);
    printf("maq_param.encoder.perimetro................: %d\n", maq_param.encoder.perimetro);
    printf("maq_param.encoder.precisao.................: %d\n", maq_param.encoder.precisao);

    MaqGravarRegistrador(MAQ_REG_ENC_FATOR, (unsigned int)(maq_param.encoder.fator * 10000UL));
    MaqGravarRegistrador(MAQ_REG_ENC_RESOL,                maq_param.encoder.precisao   );
    MaqGravarRegistrador(MAQ_REG_ENC_PERIM,                maq_param.encoder.perimetro  );

  }

  if(mask & MAQ_SYNC_CORTE) {
    printf("maq_param.corte.tam_faca...................: %d\n", maq_param.corte.tam_faca);

    MaqGravarRegistrador(MAQ_REG_CRT_FACA, maq_param.corte.tam_faca);
  }

  if(mask & MAQ_SYNC_CUSTOM) {
    switch(MaqConfigCurrent->AbaConfigMais) {
    case NTB_ABA_CONFIG_DIAGONAL:
      printf("maq_param.custom.diagonal.dist_prensa_corte: %d\n", maq_param.custom.diagonal.dist_prensa_corte);
      printf("maq_param.custom.diagonal.qtd_furos_interm.: %d\n", maq_param.custom.diagonal.qtd_furos_interm);

      MaqGravarRegistrador(MAQ_REG_DIAG_DISTANCIA, maq_param.custom.diagonal.dist_prensa_corte);

      // Para configurar este parametro, devemos carregar o valor no registrador de quantidade e ativar a flag
      // no registrador de flags pois todos os registradores ja estao ocupados
      MaqGravarRegistrador(MAQ_REG_PROD_QTD, maq_param.custom.diagonal.qtd_furos_interm + 1);
      SyncFlags |= MAQ_LOAD_QTD_FUROS;

      break;

    case NTB_ABA_CONFIG_COLN:
      printf("maq_param.custom.coln.dinam_vel............: %d\n", maq_param.custom.coln.dinam_vel);
      printf("maq_param.custom.coln.curso................: %d\n", maq_param.custom.coln.curso);
      printf("maq_param.custom.coln.auto_vel.............: %d\n", maq_param.custom.coln.auto_vel);
      printf("maq_param.custom.coln.manual_vel...........: %d\n", maq_param.custom.coln.manual_vel);
      printf("maq_param.custom.coln.offset...............: %f\n", maq_param.custom.coln.offset);
      printf("maq_param.custom.coln.tam_min..............: %d\n", maq_param.custom.coln.tam_min);

      MaqGravarRegistrador(MAQ_REG_COLN_PERF_DINAM_VEL ,                maq_param.custom.coln.dinam_vel      );
      MaqGravarRegistrador(MAQ_REG_COLN_MESA_CURSO     ,                maq_param.custom.coln.curso          );
      MaqGravarRegistrador(MAQ_REG_COLN_MESA_AUTO_VEL  ,                maq_param.custom.coln.auto_vel       );
      MaqGravarRegistrador(MAQ_REG_COLN_MESA_MANUAL_VEL,                maq_param.custom.coln.manual_vel     );
      MaqGravarRegistrador(MAQ_REG_COLN_OFFSET         , (unsigned int)(maq_param.custom.coln.offset    * 10));
      MaqGravarRegistrador(MAQ_REG_COLN_TAM_MIN        ,                maq_param.custom.coln.tam_min        );

      SyncFlags |= MAQ_MODO_MESA_SYNC;
      break;

    case NTB_ABA_CONFIG_PRENSA:
      printf("maq_param.custom.prensa.passo...........: %d\n", maq_param.custom.prensa.passo        );
      printf("maq_param.custom.prensa.sentido.........: %d\n", maq_param.custom.prensa.sentido      );
      printf("maq_param.custom.prensa.ciclos..........: %d\n", maq_param.custom.prensa.ciclos       );
      printf("maq_param.custom.prensa.ciclos_ferram...: %d\n", maq_param.custom.prensa.ciclos_ferram);
      printf("maq_param.custom.prensa.ciclos_lub......: %d\n", maq_param.custom.prensa.ciclos_lub   );

	  MaqConfigPrsCiclos    (maq_param.custom.prensa.ciclos );
	  MaqConfigPrsSentidoInv(maq_param.custom.prensa.sentido);

      MaqGravarRegistrador(MAQ_REG_APL_PASSO  , maq_param.custom.prensa.passo);

      break;
    }
  }

  if(SyncFlags) {
    MaqConfigFlags(MaqLerFlags() | SyncFlags);
  }

  return 1;
}

struct strParamDB {
  char         *grupo;
  char         *nome;
  unsigned int *ValInt;
  float        *ValFloat;
} ParamDB[] = {
    { "Encoder", "FatorCorr"  , NULL                        , &maq_param.encoder.fator         },
    { "Encoder", "Perimetro"  , &maq_param.encoder.perimetro, NULL                             },
    { "Encoder", "Precisao"   , &maq_param.encoder.precisao , NULL                             },
    { "Corte"  , "TamFaca"    , &maq_param.corte.tam_faca   , NULL                             },
    { "Perfil" , "AutoVel"    , &maq_param.perfil.auto_vel  , NULL                             },
    { "Perfil" , "AutoAcel"   , NULL                        , &maq_param.perfil.auto_acel      },
    { "Perfil" , "AutoDesacel", NULL                        , &maq_param.perfil.auto_desacel   },
    { "Perfil" , "ManVel"     , &maq_param.perfil.manual_vel, NULL                             },
    { "Perfil" , "ManAcel"    , NULL                        , &maq_param.perfil.manual_acel    },
    { "Perfil" , "ManDesacel" , NULL                        , &maq_param.perfil.manual_desacel },
    { "Perfil" , "FatorMaq"   , NULL                        , &maq_param.perfil.fator          },
    { "Perfil" , "DiamRolo"   , &maq_param.perfil.diam_rolo , NULL                             },
    { NULL     , NULL         , NULL                        , NULL                             },
};

struct strParamDB ParamDB_Diagonal[] = {
    { "Diagonal", "DistPrensaCorte", &maq_param.custom.diagonal.dist_prensa_corte, NULL        },
    { "Diagonal", "QtdFurosInterm" , &maq_param.custom.diagonal.qtd_furos_interm , NULL        },
    { NULL      , NULL             , NULL                                        , NULL        },
};

struct strParamDB ParamDB_ColunaN[] = {
    { "ColunaN", "PerfDinamVel", &maq_param.custom.coln.dinam_vel , NULL                          },
    { "ColunaN", "MesaCurso"   , &maq_param.custom.coln.curso     , NULL                          },
    { "ColunaN", "MesaAutoVel" , &maq_param.custom.coln.auto_vel  , NULL                          },
    { "ColunaN", "MesaManVel"  , &maq_param.custom.coln.manual_vel, NULL                          },
    { "ColunaN", "Offset"      , NULL                             , &maq_param.custom.coln.offset },
    { "ColunaN", "TamMin"      , &maq_param.custom.coln.tam_min   , NULL                          },
    { NULL     , NULL          , NULL                             , NULL                          },
};

struct strParamDB ParamDB_Prensa	[] = {
    { "Prensa", "Passo"       , &maq_param.custom.prensa.passo        , NULL                      },
    { "Prensa", "Sentido"     , &maq_param.custom.prensa.sentido      , NULL                      },
    { "Prensa", "Ciclos"      , &maq_param.custom.prensa.ciclos       , NULL                      },
    { "Prensa", "CiclosLub"   , &maq_param.custom.prensa.ciclos_lub   , NULL                      },
    { "Prensa", "CiclosFerram", &maq_param.custom.prensa.ciclos_ferram, NULL                      },
    { NULL     , NULL         , NULL                                  , NULL                      },
};

struct strCustomParamDB {
  int AbaConfigMais;
  struct strParamDB *ParamDB;
} CustomParamDB[] = {
    { NTB_ABA_CONFIG_DIAGONAL, ParamDB_Diagonal },
    { NTB_ABA_CONFIG_COLN    , ParamDB_ColunaN  },
    { NTB_ABA_CONFIG_PRENSA  , ParamDB_Prensa   },
    { 0                      , NULL             },
};

extern struct strDB mainDB;

#define PARAMDB_MASK_SQLSERVER 1
#define PARAMDB_MASK_MYSQL     2

int ParamDB_Load(struct strParamDB *ParamDB)
{
  float        ValFloat;
  unsigned int ValInt;

  char sql[300];
  int i, ret = 1;
  struct strDB *sDB = MSSQL_Connect();

  for(i=0; ParamDB[i].nome != NULL; i++) {
    sprintf(sql, "select VALOR_INT, VALOR_FLOAT from PARAMETRO where LINHA='%s' and MAQUINA='%s' and GRUPO='%s' and PARAMETRO='%s'",
        MAQ_LINHA, MAQ_MAQUINA, ParamDB[i].grupo, ParamDB[i].nome);

    ValInt   = 0;
    ValFloat = 0;

    if(MSSQL_Execute(0, sql, MSSQL_DONT_SYNC) < 0) {
      if(DB_Execute(&mainDB, 3, sql) < 0) {
        ret = 0;
      } else {
        if(DB_GetNextRow(&mainDB, 3) > 0) {
          char *val = DB_GetData(&mainDB, 3, 1);
          char *pos_comma = strchr(val, '.');
          if(pos_comma != NULL) *pos_comma = ',';
          ValFloat = atof(val);

          ValInt   = atoi(DB_GetData(&mainDB, 3, 0));
        } else {
          ret = 0;
        }
      }
    } else {
      DB_GetNextRow(sDB, 0);
      ValInt   = atoi(MSSQL_GetData(0, 0));
      ValFloat = atof(MSSQL_GetData(0, 1));
    }

    if(ParamDB[i].ValInt != NULL) {
      *ParamDB[i].ValInt = ValInt;
    }

    if(ParamDB[i].ValFloat != NULL) {
      *ParamDB[i].ValFloat = ValFloat;
    }
  }

  MSSQL_Close();

  return ret;
}

int ParamDB_Save(struct strParamDB *ParamDB, unsigned int mask)
{
  int i, ret = 1, erro_insert;
  char sql[300], val[20], *campo_val = "";

  for(i=0; ParamDB[i].nome != NULL; i++) {
    if(ParamDB[i].ValInt != NULL) {
      campo_val = "VALOR_INT";
      sprintf(val, "%u", *ParamDB[i].ValInt);
    }

    if(ParamDB[i].ValFloat != NULL) {
      char *pos_comma;
      campo_val = "VALOR_FLOAT";
      sprintf(val, "%f", *ParamDB[i].ValFloat);
      pos_comma = strchr(val, ',');
      if(pos_comma != NULL) *pos_comma = '.';
    }

    erro_insert = 0;

    sprintf(sql, "insert into PARAMETRO (LINHA, MAQUINA, GRUPO, PARAMETRO, %s) values ('%s', '%s', '%s', '%s', '%s')",
        campo_val, MAQ_LINHA, MAQ_MAQUINA, ParamDB[i].grupo, ParamDB[i].nome, val);

    if(!(mask & PARAMDB_MASK_SQLSERVER) || MSSQL_Execute(0, sql, MSSQL_DONT_SYNC) < 0)
      erro_insert++;

    if(!(mask & PARAMDB_MASK_MYSQL) || DB_Execute(&mainDB, 3, sql) < 0)
      erro_insert++;

    if(erro_insert == 2) // Erro no SQL Server e MySQL
      ret = 0; // indica erro mas continua, os outros parametros podem ser salvos
  }

  return ret;
}

// Funcao que grava a estrutura de configuracao no BD
int MaqGravarConfig(void)
{
  int i, mask = 0, ret = 1;
  char sql[300];

  sprintf(sql, "delete from PARAMETRO where LINHA='%s' and MAQUINA='%s'",
      MAQ_LINHA, MAQ_MAQUINA);

  if(MSSQL_Execute(0, sql, MSSQL_DONT_SYNC) >= 0) {
    mask |= PARAMDB_MASK_SQLSERVER;
  }

  if(DB_Execute(&mainDB, 3, sql) >= 0) {
    mask |= PARAMDB_MASK_MYSQL;
  }

  for(i=0; CustomParamDB[i].ParamDB != NULL; i++) {
    if(CustomParamDB[i].AbaConfigMais == MaqConfigCurrent->AbaConfigMais) {
      ret = ParamDB_Save(CustomParamDB[i].ParamDB, mask);
      break;
    }
  }

  if(ret) {
    ret = ParamDB_Save(ParamDB, mask);
  }

  if(!ret) {
    Log("Erro ao gravar parametros", LOG_TIPO_CONFIG);
  }

  return ret;
}

// Funcao que le a estrutura de configuracao do BD
int MaqLerConfig(void)
{
  int i;
  long ret = 1;

  for(i=0; CustomParamDB[i].ParamDB != NULL; i++) {
    if(CustomParamDB[i].AbaConfigMais == MaqConfigCurrent->AbaConfigMais) {
      ret = ParamDB_Load(CustomParamDB[i].ParamDB);
      break;
    }
  }

  if(ParamDB_Load(ParamDB) == 0) {
    ret = 0;
  }

  // Grava o resultado da funcao que sincroniza os dados em ret.
  // Assim a funcao retorna OK se os dados foram efetivamente gravados.
  if(MaqSync(MAQ_SYNC_TODOS) == 0) {
    ret = 0;
  }

  if(!ret) {
    Log("Erro ao ler parametros", LOG_TIPO_CONFIG);
  }

  return ret;
}

char *MaqStrErro(uint16_t erro)
{
  uint32_t i, size;

  if(!erro || MaqConfigCurrent == NULL) // Sem erro ou maquina nula, retorna string nula
    return NULL;

  // Busca fim da lista de erros
  for(size = 0; MaqConfigCurrent->ErrorList[size][0]; size++);

  for(i=0; !((erro>>i)&1); i++); // Busca primeiro bit ligado

  if(i < size) {
    return MaqConfigCurrent->ErrorList[i];
  } else {
    return "Erro indefinido!";
  }
}

uint16_t MaqLerErros(void)
{
  uint16_t erro;
  struct strMaqReply rp;
  struct strIPCMQ_Message ipc_msg;

  memset(&rp, 0, sizeof(rp));

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = retMaqMB;
  ipc_msg.res   = (void *)&rp;
  ipc_msg.data.modbus_query.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
  ipc_msg.data.modbus_query.data.read_holding_registers.start = MAQ_REG_ERROS;
  ipc_msg.data.modbus_query.data.read_holding_registers.quant = 1;

  EnviarMB(&ipc_msg, &rp);

  if(rp.modbus_reply.ExceptionCode != MODBUS_EXCEPTION_NONE)
    return 1; // Erro de comunicacao

  erro = CONV_PCHAR_UINT16(rp.modbus_reply.reply.read_holding_registers.data);
  printf("Erro lido: %04x\n", erro);

  return erro << 1;
}

uint16_t MaqLerEstado(void)
{
  static uint16_t status = 0;

  status = MaqLerRegistrador(MAQ_REG_STATUS, status);
  printf("status: %d\n", status);

  return status;
}

uint32_t MaqLerEntradas(void)
{
  uint32_t val;
  unsigned char *buf;
  struct strMaqReply rp;
  struct strIPCMQ_Message ipc_msg;

  memset(&rp, 0, sizeof(rp));

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = retMaqMB;
  ipc_msg.res   = (void *)&rp;
  ipc_msg.data.modbus_query.function_code = MODBUS_FC_READ_DISCRETE_INPUTS;
  ipc_msg.data.modbus_query.data.read_discrete_inputs.start = 0;
  ipc_msg.data.modbus_query.data.read_discrete_inputs.quant = 18;

  EnviarMB(&ipc_msg, &rp);

  if(rp.modbus_reply.ExceptionCode != MODBUS_EXCEPTION_NONE)
    return 0; // Erro de comunicacao

  buf  = rp.modbus_reply.reply.read_discrete_inputs.data;
  val  = ((uint32_t)(buf[2]) << 16) | ((uint32_t)(buf[1]) << 8) | buf[0];
  val &= 0x7FFFF;
  printf("input: %05x\n", val);

  return val;
}

uint32_t MaqLerSaidas(void)
{
  volatile uint32_t val;
  unsigned char *buf;
  struct strMaqReply rp;
  struct strIPCMQ_Message ipc_msg;

  memset(&rp, 0, sizeof(rp));

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = retMaqMB;
  ipc_msg.res   = (void *)&rp;
  ipc_msg.data.modbus_query.function_code = MODBUS_FC_READ_COILS;
  ipc_msg.data.modbus_query.data.read_coils.start = 0;
  ipc_msg.data.modbus_query.data.read_coils.quant = 16;

  EnviarMB(&ipc_msg, &rp);

  if(rp.modbus_reply.ExceptionCode != MODBUS_EXCEPTION_NONE)
    return 0; // Erro de comunicacao

  buf = rp.modbus_reply.reply.read_coils.data;
  val = ((uint32_t)(buf[1]) << 8) | buf[0];
  printf("output: %04x\n", val);

  return val;
}

uint16_t MaqLerPrsCiclos(void)
{
  uint16_t ciclos;

  ciclos  = MaqLerRegistrador(MAQ_REG_PRS_CICLOS_MIL , maq_param.custom.prensa.ciclos/1000)*1000;
  ciclos += MaqLerRegistrador(MAQ_REG_PRS_CICLOS_UNID, maq_param.custom.prensa.ciclos%1000);

  printf("Executados %d ciclos\n", ciclos);

  return ciclos;
}

int16_t MaqLerPosAtual(void)
{
  static int16_t pos;

  pos = MaqLerRegistrador(MAQ_REG_POS_ATUAL, pos);

  return pos;
}

int16_t MaqLerAplanErroPosic(void)
{
  static int16_t erro;

  erro = MaqLerRegistrador(MAQ_REG_APL_ERRO_POSIC, erro);
  printf("Ultimo erro: %d\n", erro);

  return erro;
}

uint16_t MaqInvSyncOK(void)
{
  uint16_t SyncOK = (MaqLerFlags() & MAQ_MODO_INV_SYNC) != 0;
  printf("SyncOK: %d\n", SyncOK);

  return SyncOK;
}

void MaqInvSync(void)
{
  uint16_t flags = MaqLerFlags();
  flags |= MAQ_MODO_INV_SYNC;
  MaqConfigFlags(flags);
}

uint16_t MaqLerInvTensao(void)
{
  static uint16_t val = 0;

  val = MaqLerRegistrador(MAQ_REG_INV_TENSAO, val);
  printf("Inversor - Tensao: %d\n", val);

  return val;
}

uint16_t MaqLerInvCorrente(void)
{
  static uint16_t val = 0;

  val = MaqLerRegistrador(MAQ_REG_INV_CORRENTE, val);
  printf("Inversor - Corrente: %d\n", val);

  return val;
}

uint16_t MaqLerInvTorque(void)
{
  static uint16_t val = 0;

  val = MaqLerRegistrador(MAQ_REG_INV_TORQUE, val);
  printf("Inversor - Torque: %d\n", val);

  return val;
}

uint16_t MaqLerInvInput(void)
{
  static uint16_t val = 0;

  val = MaqLerRegistrador(MAQ_REG_INV_INPUT, val);
  printf("Inversor - Input: %d\n", val);

  return val;
}

uint16_t MaqLerInvOutput(void)
{
  static uint16_t val = 0;

  val = MaqLerRegistrador(MAQ_REG_INV_OUTPUT, val);
  printf("Inversor - Output: %d\n", val);

  return val;
}

uint16_t MaqLerProdQtd(void)
{
  static uint16_t qtd = 0;

  qtd = MaqLerRegistrador(MAQ_REG_PROD_QTD, qtd);
  printf("Restando %d pecas\n", qtd);

  return qtd;
}

void MaqConfigEstado(uint16_t estado)
{
  MaqGravarRegistrador(MAQ_REG_STATUS, estado);
}

void MaqConfigModo(uint16_t modo)
{
  uint16_t flags = MaqLerFlags();

  printf("modo = %d\n", modo);

  MaqConfigFlags((flags & ~MAQ_MODO_MASK) | modo);

  MaqAuto(modo == MAQ_MODO_AUTO);
}

void MaqLiberar(uint16_t liberar)
{
  uint16_t flags = MaqLerFlags();

  // Se bloqueou a máquina, desliga todas as flags indicadas pela máscara.
  MaqConfigFlags(liberar ? flags | MAQ_MODO_LIBERA : flags & MAQ_MODO_MASK_LIBERA);

  printf("maquina liberada ? %d\n", liberar);
}

void MaqLimparErro()
{
  uint16_t flags = MaqLerFlags();
  flags |= MAQ_MODO_LIMPAR;
  MaqConfigFlags(flags);
}

void MaqCortar()
{
  uint16_t flags = MaqLerFlags();
  flags |= MAQ_MODO_CORTAR;
  MaqConfigFlags(flags);
}

void MaqPerfManual(uint16_t cmd)
{
  uint16_t flags = MaqLerFlags();

  flags &= ~MAQ_MODO_PERF_MASK;
  if     (cmd == OPER_PERF_AVANCA)
    flags |= MAQ_MODO_PERF_AVANCA;
  else if(cmd == OPER_PERF_RECUA)
    flags |= MAQ_MODO_PERF_RECUA;

  MaqConfigFlags(flags);
}

void MaqMesaManual(uint16_t cmd)
{
  uint16_t flags = MaqLerFlags();

  flags &= ~MAQ_MODO_MESA_MASK;
  if     (cmd == OPER_MESA_AVANCA)
    flags |= MAQ_MODO_MESA_AVANCA;
  else if(cmd == OPER_MESA_RECUA)
    flags |= MAQ_MODO_MESA_RECUA;

  MaqConfigFlags(flags);
}

void MaqAplanManual(uint16_t cmd)
{
  static uint16_t flags;
  unsigned int mask_or, mask_and;

  switch(cmd) {
    case MAQ_APLAN_ABRIR:
    	mask_or  =  MAQ_FM_APLAN_ABRIR;
    	mask_and = ~MAQ_FM_APLAN_FECHAR;
        break;

    case MAQ_APLAN_FECHAR:
    	mask_or  =  MAQ_FM_APLAN_FECHAR;
    	mask_and = ~MAQ_FM_APLAN_ABRIR;
      break;

    case MAQ_APLAN_SUBIR:
    	mask_or  =  MAQ_FM_APLAN_SUBIR;
    	mask_and = ~MAQ_FM_APLAN_DESCER;
        break;

    case MAQ_APLAN_DESCER:
    	mask_or  =  MAQ_FM_APLAN_DESCER;
    	mask_and = ~MAQ_FM_APLAN_SUBIR;
      break;

    case MAQ_APLAN_EXT_SUBIR:
    	mask_or  =  MAQ_FM_APLAN_EXT_SUBIR;
    	mask_and = ~MAQ_FM_APLAN_EXT_DESCER;
        break;

    case MAQ_APLAN_EXT_DESCER:
    	mask_or  =  MAQ_FM_APLAN_EXT_DESCER;
    	mask_and = ~MAQ_FM_APLAN_EXT_SUBIR;
      break;

    case MAQ_APLAN_EXT_EXPANDIR:
    	mask_or  =  MAQ_FM_APLAN_EXT_EXPANDIR;
    	mask_and = ~MAQ_FM_APLAN_EXT_RETRAIR;
        break;

    case MAQ_APLAN_EXT_RETRAIR:
    	mask_or  =  MAQ_FM_APLAN_EXT_RETRAIR;
    	mask_and = ~MAQ_FM_APLAN_EXT_EXPANDIR;
      break;

    case MAQ_APLAN_PARAR:
    	mask_or  =  0x0000;
    	mask_and = ~(MAQ_FM_APLAN_ABRIR        | MAQ_FM_APLAN_FECHAR      |
    				 MAQ_FM_APLAN_SUBIR        | MAQ_FM_APLAN_DESCER      |
					 MAQ_FM_APLAN_EXT_SUBIR    | MAQ_FM_APLAN_EXT_DESCER  |
    				 MAQ_FM_APLAN_EXT_EXPANDIR | MAQ_FM_APLAN_EXT_RETRAIR
    				 );
      break;

    default:
    	return;
  }

  flags = (MaqLerRegistrador(MAQ_REG_PRENSA_MANUAL, flags) & mask_and) | mask_or;

  MaqGravarRegistrador(MAQ_REG_PRENSA_MANUAL, flags);
}

void MaqPrsManual(uint16_t cmd)
{
  uint16_t flags = MaqLerFlags();

  switch(cmd) {
    case MAQ_PRS_LIGAR:
        flags |=  MAQ_FM_PRS_LIGAR;
        break;

    case MAQ_PRS_DESLIGAR:
        flags &= ~MAQ_FM_PRS_LIGAR;
      break;

    case MAQ_PRS_INICIAR:
    	// Por motivo de seguranca, a prensa somente inicia por comando do operador (Pedal / Bi-Manual)
        return;

    case MAQ_PRS_PARAR:
        flags |=  MAQ_FM_PRS_PARAR;
        break;
  }

  MaqConfigFlags(flags);
}

uint16_t MaqPronta()
{
  return MaqLerEstado() & MAQ_STATUS_PRONTA ? TRUE : FALSE;
}

void MaqConfigPrsCiclos(uint32_t val)
{
  uint16_t modo = MaqLerFlags(), ciclos_mil = val/1000;

  MaqGravarRegistrador(MAQ_REG_PRS_CICLOS_NOVO_MIL , ciclos_mil);
  MaqGravarRegistrador(MAQ_REG_PRS_CICLOS_NOVO_UNID, val - (uint32_t)(ciclos_mil)*1000);

  modo |= MAQ_MODO_PRS_CICLOS;
  MaqConfigFlags(modo);
}

void MaqConfigPrsSentidoInv(uint16_t val)
{
  uint16_t modo = MaqLerFlags();

  if(val)
    modo |=  MAQ_MODO_PRS_SENTIDO;
  else
    modo &= ~MAQ_MODO_PRS_SENTIDO;

  MaqConfigFlags(modo);
}

void MaqConfigProdQtd(uint16_t qtd)
{
  MaqGravarRegistrador(MAQ_REG_PROD_QTD, qtd);
}

void MaqConfigProdTam(uint16_t tam)
{
  MaqGravarRegistrador(MAQ_REG_PROD_TAM, tam);
}

void MaqConfigPerfil(struct strMaqParamPerfil pf)
{
  maq_param.perfil = pf;

  MaqSync(MAQ_SYNC_PERFIL);
}

struct strMaqParamPerfil MaqLerPerfil()
{
  return maq_param.perfil;
}

void MaqConfigEncoder(struct strMaqParamEncoder enc)
{
  maq_param.encoder = enc;

  MaqSync(MAQ_SYNC_ENCODER);
}

struct strMaqParamEncoder MaqLerEncoder()
{
  return maq_param.encoder;
}

void MaqConfigCorte(struct strMaqParamCorte corte)
{
  maq_param.corte = corte;

  MaqSync(MAQ_SYNC_CORTE);
}

struct strMaqParamCorte MaqLerCorte()
{
  return maq_param.corte;
}

void MaqConfigCustom(struct strMaqParamCustom custom)
{
  maq_param.custom = custom;

  MaqSync(MAQ_SYNC_CUSTOM);
}

struct strMaqParamCustom MaqLerCustom(void)
{
  return maq_param.custom;
}

void MaqSetDateTime(struct tm *t)
{
  time_t rawtime;
  char cWriteBuffer[16];

  if(t == NULL) {
    time ( &rawtime );
    t = localtime ( &rawtime );
  }

  t->tm_year += 1900;
  t->tm_mon++;
  t->tm_sec = t->tm_sec > 59 ? 59 : t->tm_sec;

  memset(cWriteBuffer, 0, sizeof(cWriteBuffer));

  memcpy(&cWriteBuffer[0], &t->tm_mday, 1);
  memcpy(&cWriteBuffer[1], &t->tm_mon , 1);
  memcpy(&cWriteBuffer[2], &t->tm_year, 2);
  memcpy(&cWriteBuffer[4], &t->tm_hour, 1);
  memcpy(&cWriteBuffer[5], &t->tm_min , 1);
  memcpy(&cWriteBuffer[6], &t->tm_sec , 1);
  memcpy(&cWriteBuffer[7], &t->tm_wday, 1);
  memcpy(&cWriteBuffer[8], &t->tm_yday, 2);

  // Registradores de data/hora da POP-7: 32 a 36
  MaqGravarRegistrador(32, *((uint16_t *)&cWriteBuffer[0]));
  MaqGravarRegistrador(33, *((uint16_t *)&cWriteBuffer[2]));
  MaqGravarRegistrador(34, *((uint16_t *)&cWriteBuffer[4]));
  MaqGravarRegistrador(35, *((uint16_t *)&cWriteBuffer[6]));
  MaqGravarRegistrador(36, *((uint16_t *)&cWriteBuffer[8]));
}

void MaqGetIpAddress(char *iface, char *ipaddr)
{
  struct ifaddrs *ifaddr, *ifa;
  int s;
  char host[NI_MAXHOST];

  // Se interface ou buffer para o endereco ip for nulo, retorna...
  if(iface == NULL) return;

  // Inicializa com string vazia para o caso de nada ser encontrado.
  if(ipaddr != NULL) {
    ipaddr[0] = 0;
  }

  if (getifaddrs(&ifaddr) != -1) {
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;

        s = getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

        if(!strcmp(ifa->ifa_name, iface) && (ifa->ifa_addr->sa_family == AF_INET)) {
            if (s == 0 && ipaddr != NULL) {
              strcpy(ipaddr, host);
            }

            printf("Endereco IP IHM: <%s>\n", host);
            if(MaqConfigCurrent != NULL) {
              printf("Endereco IP CLP: <%s>\n", MaqConfigCurrent->ClpAddr);
            }

            break;
        }
    }
  }

  freeifaddrs(ifaddr);
}
