POPTools:1.2
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=9600
PARITY=1
COM=4
IP=192.168.0.235
MASK=255.255.255.0
GW=192.168.0.10
COMPILED=C:\Documents and Settings\utikawa\Desktop\ColunaN\ColunaN.hex

IO LIST
    XEmergencia at 1 0
    XTermBombaHidr at 2 0
    XInversorOK at 3 0
    XSnsSupHidr at 5 0
    XSnsInfHidr at 6 0
    XSnsSupPiloto at 7 0
    XSnsInfPiloto at 8 0
    XInvFimPosic at 9 0
    XFaltaFase at 10 0
    XCortePerfManual at 11 0
    XPerfManAvanca at 12 0
    XPerfManRecua at 13 0
    XDesbobOK at 14 0
    YLigarBombaHidr at 1 0
    YHidrDesce at 2 0
    YHidrSobe at 3 0
    YCorteTravaMesa at 4 0
    YPilotoDescer at 5 0
    YInvFreioMotor at 7 0
    YPerfOK at 8 0
    YAvancarPerf at 9 0
    YRecuarPerf at 10 0
    YInvPosicao at 11 0
    YInvResetPos at 12 0
    YHidrVentagem at 13 0
    YMbErrEmergencia at 20 0
    YMbErrFaltaFase at 20 1
    YMbErrBombaHidr at 20 2
    YMbErrInversor at 20 3
    YMbDesbobOK at 20 4
    YMbErrInvComunic at 20 5
    XMbModoAuto at 20 15
    YMbModoAutoAtivo at 21 0
    YMbMaqProduzindo at 21 2
    YMbCorteAtivo at 21 3
    YMbMaqIniciando at 21 5
    YMbMaqInitOK at 21 6
    YMbFerramCrtCima at 21 7
    YMbMaqPronta at 21 8
    YMbMaqOperando at 21 9
    YMbMaqCfgInvOK at 21 10
    YMbMaqCortePerf at 21 14
    YMbCorteIHM at 22 0
    YPerfManAvcIHM at 22 1
    YPerfManRecuaIHM at 22 2
    YMbLimpaErro at 22 3
    YMbInvParamSync at 22 4
    YMbPerfFatorL at 30 0
    YMbPerfFatorH at 31 0
    YMbAutoAcel at 32 0
    YMbAutoDesac at 33 0
    YMbAutoVel at 34 0
    YMbManAcel at 35 0
    YMbManDesac at 36 0
    YMbManVel at 37 0
    YMbTamFaca at 39 0
    YMbFator at 40 0
    YMbQtd at 50 0
    YMbTam at 51 0
    YLedErro at 17 0
    YLedUser at 18 0
END

PROGRAM
RUNG
    COMMENT Programa de Controle da Perfiladeira de Perf - v1.0\r\nDesenvolvido por Marcelo Utikawa da Fonseca em 23/07/2010
END
RUNG
    COMMENT Descrição das flags utilizadas no programa:
END
RUNG
    COMMENT AvancarPerfMan\r\n   Flag que indica que o comando para avanço manual da perfiladeira está ativado
END
RUNG
    COMMENT BloqueiaRecuo\r\n   Se ficar acionado o recuo manual da perfiladeira por mais que 3 segundos, desliga a perfiladeira
END
RUNG
    COMMENT BombaHidrOK\r\n   Indica que a bomba hidráulica está ligada e estabilizada, podendo assim realizar um ciclo de corte
END
RUNG
    COMMENT CortarPerf\r\n   Indica que deve ser realizado um ciclo de corte do perfil. Flag geral, independente da origem da solicitação (manual, automática, IHM)
END
RUNG
    COMMENT CortePerfAtivo\r\n   Flag indicando que um ciclo de corte está sendo realizado
END
RUNG
    COMMENT CortePerfDesce\r\n   Ativo quando, durante um ciclo de corte, a máquina está realizando o movimento de descida
END
RUNG
    COMMENT CortePilotar\r\n   Flag que dispara o piloto durante operação de corte
END
RUNG
    COMMENT CortePilotoCima\r\n   Ativo quando o piloto está recuado, ou seja, sensor inferior desligado e superior ligado
END
RUNG
    COMMENT CortePronto\r\n   Ativo quando a máquina está pronta para realizar um corte
END
RUNG
    COMMENT CorteRealizado\r\n   Indica que o perfil foi cortado na última operação de corte. Assim, mesmo que houve algum erro no corte, ele ocorreu depois que o perfil foi cortado e o encoder zerado
END
RUNG
    COMMENT ErroCortePerf\r\n   Caso ocorra um erro durante o ciclo de corte do perfil, esta flag é ativada. Desativa automaticamente quando desligar o comando de corte manual ou encerrar a produção automática
END
RUNG
    COMMENT ErroCortePiloto\r\n   Indica que o erro ocorrido no corte foi devido a falha na tentativa de pilotar o perfil
END
RUNG
    COMMENT ErroGeral\r\n   Erro grave na máquina: emergência, falta de fase
END
RUNG
    COMMENT FerramCorteCima\r\n   Indica que as ferramentas de corte de tudo (serra e hidráulica) estão na posição superior, permitindo o movimento do perfil
END
RUNG
    COMMENT InvAtivar\r\n   Flag que indica que o inversor deve ser ativado. Flag gerada pela lógica, não é gerada pelo comando de avanço manual
END
RUNG
    COMMENT InvFreioLib\r\n   Ativo quando o freio da perfiladeira estiver ligado e liberado, permitindo o movimento do perfil
END
RUNG
    COMMENT MaqCfgInvErro\r\n   Indica que houve erro na configuração de parâmetros no inversor devido a falha de comunicação
END
RUNG
    COMMENT MaqCfgInvOK\r\n   Indica que o inversor está configurado e portanto pode ser ativado
END
RUNG
    COMMENT MaqCfgInvOK\r\n   Indica que a máquina foi configurada e pode iniciar a produção automática. Ativo apenas após a transmissão de todos os parâmetros ao inversor
END
RUNG
    COMMENT MaqConfigOK\r\n   Indica que tanto o inversor como o servo estão configurados
END
RUNG
    COMMENT MaqCortePerf\r\n   Flag que solicita o corte do perfil. Flag gerada pela lógica, não é gerada pelo comando de corte manual
END
RUNG
    COMMENT MaqInit\r\n   Ativa enquanto a mesa está buscando a posição inicial durante a inicialização
END
RUNG
    COMMENT MaqInitCorte\r\n   Ativo enquanto espera inicializar a ferramenta de corte, posicionando a ferramenta na posição superior e recuando o piloto
END
RUNG
    COMMENT MaqInitOK\r\n   Flag que indica que a máquina está inicializada e pode ser operada
END
RUNG
    COMMENT MaqModoCorrecao\r\n   Flag que ativa o modo de correção que é quando o modo de ajuste não consegue corrigir o erro, exigindo assim intervenção do operador da máquina
END
RUNG
    COMMENT MaqOK\r\n   Indica que a máquina está sem erros
END
RUNG
    COMMENT MaqOperando\r\n   Ativo quando a máquina está realizando qualquer operação como corte, posicionamento, etc. Serve para bloquear duas operações de ocorrerem ao mesmo tempo
END
RUNG
    COMMENT MaqProduzindo\r\n   Indica que a máquina está no loop de produção. Somente desativa caso ocorrer algum erro, desativar a produção ou terminar a quantidade de peças programada
END
RUNG
    COMMENT MaqPronta\r\n   Indica que a máquina está sem erros, configurada e inicializada, permitindo portanto a operação manual ou automática
END
RUNG
    COMMENT MaqUsarPiloto\r\n   Ativa quando o piloto deve ser utilizado durante um ciclo de corte
END
RUNG
    COMMENT ModoAuto\r\n   Flag ativada pelo ModBUS para configurar o modo automático e iniciar o loop de produção
END
RUNG
    COMMENT ModoAutoOK\r\n   Indica que a máquina está pronta para operar no modo automático, ou seja, sem erros e com o modo automático ativado
END
RUNG
    COMMENT PerfManualOK\r\n   Flag que indica que a perfiladeira está pronta para movimentar em modo manual
END
RUNG
    COMMENT RecuarPerfMan\r\n   Ativa quando acionado botao de recuo manual da perfiladeira e a máquina em modo manual
END
RUNG
    COMMENT RepeteSemPiloto\r\n   Flag que indica que, ao final de um ciclo de corte, a máquina deve realizar um novo ciclo sem usar o piloto caso ele tenha causado erro na operação. Apenas utilizado no modo manual
END
RUNG
    COMMENT SnsInfCortePerf\r\n   Representa o estado do sensor inferior da ferramenta de corte
END
RUNG
    COMMENT SnsSupCortePerf\r\n   Representa o estado do sensor superior da ferramenta de corte
END
RUNG
    COMMENT StartCfgInvAuto\r\n   Solicita a configuração do inversor para o modo automático
END
RUNG
    COMMENT StartCfgInvMan\r\n   Solicita a configuração do inversor para o modo manual
END
RUNG
    COMMENT SubQtdProd\r\n   Flag que é ativada pela lógica indicando que deve ser subtraída uma peça da quantidade a ser produzida
END
RUNG
    COMMENT Descrição dos Timers utilizados no programa:
END
RUNG
    COMMENT BombaHidrEstab\r\n   Espera para estabilizar a pressão hidráulica após ligar a bomba de corte do perfil
END
RUNG
    COMMENT BombaHidrInat\r\n   Se depois de 5 minutos não houve nenhum ciclo de corte, a bomba é desligada automaticamente
END
RUNG
    COMMENT CfgInvAutoTO\r\n   Timeout na configuração do inversor para modo automático
END
RUNG
    COMMENT CfgInvManTO\r\n   Timeout na configuração do inversor para modo manual
END
RUNG
    COMMENT ConfirmaOFF\r\n   Espera para confirmar que o botão de corte do perfil foi desligado e não apenas um faiscamento ao ligar
END
RUNG
    COMMENT EspFimMaterial\r\n   Espera o sinal do sensor de fim de material estabilizar para garantir que acabou o material e não que a chapa amassada, por exemplo, gere uma leitura errada
END
RUNG
    COMMENT EspPilotoEstab\r\n   Aguarda 50 ms para garantir que o piloto está em cima
END
RUNG
    COMMENT EspSistemaOK\r\n   Espera 10 segundos antes de ativar flags para fazer a configuração inicial do servo e do inversor, garantindo que eles já estão ligados e respondendo
END
RUNG
    COMMENT EspSnsInfON\r\n   Espera o sinal do sensor inferior estabilizar
END
RUNG
    COMMENT EspSnsSupON\r\n   Espera o sinal do sensor superior estabilizar
END
RUNG
    COMMENT EspFreioLib\r\n   Aguarda um tempo para garantir que o freio abriu
END
RUNG
    COMMENT EsperaLedOFF\r\n   Intervalo em que o led de CPU fica desligado
END
RUNG
    COMMENT EsperaLedON\r\n   Intervalo em que o led de CPU fica ligado
END
RUNG
    COMMENT EsperaPilotar\r\n   Tempo que espera antes de o piloto atuar. Assim garante que o piloto recuou antes de fazer uma nova tentativa
END
RUNG
    COMMENT MantemFreioLib\r\n   Depois que o movimento manual da perfiladeira é desligado, mantém-se o freio ativo para permitir a máquina desacelerar antes de ativar o freio novamente
END
RUNG
    COMMENT MaxCortePerf\r\n   Indica que depois de 5 segundos o ciclo de corte da ferramenta de perfil não terminou
END
RUNG
    COMMENT MaxPerfRecua\r\n   Tempo máximo permitido para a perfiladeira recuar no modo manual, evitando problemas com o perfil
END
RUNG
    COMMENT PilotoDesceTO\r\n   Tempo que espera o piloto atuar. Após esse tempo gera erro no piloto
END
RUNG
    COMMENT RecuoCorteTO\r\n   No caso de ocorrer erro durante a operação de corte, aguarda esse tempo para tentar recuar a ferramenta antes de parar o perfil, evitando danos à ferramenta
END
RUNG
    COMMENT Descrição dos Contadores utilizados no programa:
END
RUNG
    COMMENT NumTentPiloto\r\n   Tentativas de pilotar durante o corte antes de gerar erro
END
RUNG
    COMMENT QtdProd\r\n   Contagem de peças que restam produzir antes de finalizar o loop de produção
END
RUNG
    COMMENT Lógica para piscar o led de status, permitindo ao usuário identificar se a POP está funcionando
END
RUNG
    CONTACTS YLedUser 1 0
    TON TEsperaLedON 1000000
    TOF TEsperaLedOFF 1000000
    COIL YLedUser 0 0 0 0
END
RUNG
    COMMENT Checagens gerais da Máquina
END
RUNG
    PARALLEL
        CONTACTS XEmergencia 1 0
        CONTACTS XFaltaFase 1 0
    END
    PARALLEL
        COIL RErroGeral 0 0 0 0
        SERIES
            OSR
            COIL RMaqInitOK 0 0 1 0
        END
    END
END
RUNG
    CONTACTS RErroGeral 1 0
    COIL YPerfOK 0 0 0 0
END
RUNG
    CONTACTS RErroGeral 1 0
    CONTACTS XInversorOK 0 0
    CONTACTS XTermBombaHidr 0 0
    CONTACTS XDesbobOK 0 0
    COIL RMaqOK 0 0 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    OSF
    PARALLEL
        COIL RMaqInit 0 0 1 0
        COIL RMaqInitOK 0 0 1 0
        COIL YPerfManAvcIHM 0 0 1 1
        COIL YPerfManRecuaIHM 0 0 1 2
    END
END
RUNG
    PARALLEL
        CONTACTS RMaqOK 1 0
        CONTACTS RMaqCfgInvErro 0 0
    END
    COIL YLedErro 0 0 0 0
END
RUNG
    CONTACTS RMaqCfgInvOK 0 0
    COIL RMaqConfigOK 0 0 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RMaqInitOK 0 0
    CONTACTS RMaqConfigOK 0 0
    COIL RMaqPronta 0 0 0 0
END
RUNG
    CONTACTS RCortePerfAtivo 0 0
    COIL RMaqOperando 0 0 0 0
END
RUNG
    COMMENT Lógica de controle do corte do perfil - Lógicas Gerais
END
RUNG
    CONTACTS XSnsSupHidr 0 0
    TON TEspSnsSupON 50000
    COIL RSnsSupCortePerf 0 0 0 0
END
RUNG
    CONTACTS XSnsInfHidr 0 0
    TON TEspSnsInfON 50000
    COIL RSnsInfCortePerf 0 0 0 0
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    COIL RCortePronto 0 0 0 0
END
RUNG
    CONTACTS RSnsSupCortePerf 0 0
    CONTACTS RSnsInfCortePerf 1 0
    CONTACTS XSnsSupPiloto 0 0
    CONTACTS XSnsInfPiloto 1 0
    COIL RFerramCorteCima 0 0 0 0
END
RUNG
    COMMENT Lógica de controle do corte do perfil - Controle da Bomba Hidráulica
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS YLigarBombaHidr 1 0
    PARALLEL
        CONTACTS RCortePerfAtivo 0 0
        CONTACTS RMaqInitCorte 0 0
    END
    COIL YLigarBombaHidr 0 1 0 0
END
RUNG
    CONTACTS YLigarBombaHidr 0 0
    TON TBombaHidrEstab 5000000
    COIL RBombaHidrOK 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RBombaHidrOK 0 0
            CONTACTS RCortePerfAtivo 1 0
            TON TBombaHidrInat 300000000
        END
        CONTACTS RMaqOK 1 0
    END
    COIL YLigarBombaHidr 0 0 1 0
END
RUNG
    COMMENT Lógica de controle do corte do perfil - Operação de Corte
END
RUNG
    CONTACTS RMaqPronta 0 0
    CONTACTS RErroCortePerf 1 0
    CONTACTS RCortarPerf 0 0
    CONTACTS RMaqOperando 1 0
    CONTACTS RCortePerfAtivo 1 0
    PARALLEL
        COIL RCortePerfAtivo 0 1 0 0
        COIL YCorteTravaMesa 0 1 0 0
        COIL RCortarPerf 0 0 1 0
        COIL YMbCorteIHM 0 0 1 1
        COIL RMaqCortePerf 0 0 1 0
        COIL RCorteRealizado 0 0 1 0
        MOVE CNumTentPiloto 0
        COIL RMaqUsarPiloto 0 1 0 0
        SERIES
            CONTACTS RRepeteSemPiloto 0 0
            COIL RMaqUsarPiloto 0 0 1 0
        END
        COIL RRepeteSemPiloto 0 0 1 0
    END
END
RUNG
    CONTACTS RCortePerfAtivo 0 0
    OSR
    CONTACTS YMbModoAutoAtivo 1 0
    CONTACTS XCortePerfManual 0 0
    COIL RRepeteSemPiloto 0 1 0 0
END
RUNG
    CONTACTS RCortePerfAtivo 0 0
    PARALLEL
        SERIES
            CONTACTS RCortePronto 0 0
            TON TMaxCortePerf 5000000
        END
        CONTACTS RMaqOK 1 0
    END
    PARALLEL
        COIL RCortePerfAtivo 0 0 1 0
        COIL YCorteTravaMesa 0 0 1 0
        COIL RErroCortePerf 0 1 0 0
        COIL RCortePerfDesce 0 0 1 0
        SERIES
            OSR
            CONTACTS RCortePilotar 1 0
            COIL YCorteTravaMesa 0 0 1 0
        END
        COIL RCortePilotar 0 0 1 0
        COIL YPilotoDescer 0 0 1 0
    END
END
RUNG
    CONTACTS XSnsInfPiloto 1 0
    CONTACTS XSnsSupPiloto 0 0
    TON TEspPilotoEstab 50000
    COIL RCortePilotoCima 0 0 0 0
END
RUNG
    CONTACTS YCorteTravaMesa 0 0
    TON TEspTravaON 100000
    TOF TEspTravaOFF 100000
    COIL RSnsSupTravaMesa 1 0 0 0
END
RUNG
    CONTACTS RCortePerfAtivo 0 0
    CONTACTS RCortePronto 0 0
    CONTACTS RSnsSupTravaMesa 0 0
    OSR
    PARALLEL
        SERIES
            CONTACTS RMaqUsarPiloto 0 0
            COIL RCortePilotar 0 1 0 0
        END
        SERIES
            CONTACTS RMaqUsarPiloto 1 0
            COIL RCortePerfDesce 0 1 0 0
        END
    END
END
RUNG
    CONTACTS RCortePerfAtivo 0 0
    CONTACTS RCortePronto 0 0
    CONTACTS RCortePilotar 0 0
    CONTACTS RCortePilotoCima 0 0
    CONTACTS YPilotoDescer 1 0
    CONTACTS RErroCortePerf 1 0
    TON TEsperaPilotar 50000
    COIL YPilotoDescer 0 1 0 0
END
RUNG
    CONTACTS RCortePerfAtivo 0 0
    CONTACTS YPilotoDescer 0 0
    TON TPilotoDesceTO 500000
    PARALLEL
        CONTACTS XSnsInfPiloto 1 0
        CONTACTS XSnsSupPiloto 0 0
    END
    PARALLEL
        COIL YPilotoDescer 0 0 1 0
        SERIES
            CTU CNumTentPiloto 3
            PARALLEL
                COIL RErroCortePerf 0 1 0 0
                COIL RErroCortePiloto 0 1 0 0
                COIL RCortePilotar 0 0 1 0
                MOVE CNumTentPiloto 0
            END
        END
    END
END
RUNG
    CONTACTS RCortePerfAtivo 0 0
    CONTACTS RCortePronto 0 0
    CONTACTS XSnsInfPiloto 0 0
    CONTACTS XSnsSupPiloto 1 0
    OSR
    COIL RCortePerfDesce 0 1 0 0
END
RUNG
    CONTACTS RCortePerfAtivo 0 0
    CONTACTS RCortePronto 0 0
    PARALLEL
        SERIES
            CONTACTS XSnsInfPiloto 0 0
            CONTACTS XSnsSupPiloto 1 0
        END
        CONTACTS RMaqUsarPiloto 1 0
    END
    CONTACTS RCortePerfDesce 0 0
    CONTACTS RSnsInfCortePerf 0 0
    PARALLEL
        COIL RCortePerfDesce 0 0 1 0
        SERIES
            PARALLEL
                CONTACTS YMbModoAutoAtivo 1 0
                CONTACTS RMaqModoCorrecao 0 0
            END
            COIL RCorteRealizado 0 1 0 0
        END
    END
END
RUNG
    CONTACTS RCortePerfAtivo 0 0
    CONTACTS RCortePronto 0 0
    CONTACTS XSnsInfPiloto 0 0
    CONTACTS XSnsSupPiloto 1 0
    CONTACTS RCortePerfDesce 1 0
    CONTACTS RSnsSupCortePerf 0 0
    PARALLEL
        COIL YPilotoDescer 0 0 1 0
        COIL RCortePilotar 0 0 1 0
    END
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RCortePerfDesce 1 0
            CONTACTS RSnsSupCortePerf 0 0
            OSR
            CONTACTS RMaqUsarPiloto 1 0
        END
        SERIES
            CONTACTS RErroCortePerf 1 0
            CONTACTS RCortePilotoCima 0 0
            OSR
            CONTACTS RMaqUsarPiloto 0 0
        END
        SERIES
            CONTACTS RErroCortePerf 0 0
            CONTACTS YPilotoDescer 1 0
            CONTACTS RCortePilotoCima 0 0
            CONTACTS RSnsSupCortePerf 0 0
            OSR
        END
    END
    CONTACTS RCortePerfAtivo 0 0
    CONTACTS RCortePilotar 1 0
    COIL YCorteTravaMesa 0 0 1 0
END
RUNG
    CONTACTS RCortePerfAtivo 0 0
    CONTACTS RSnsSupTravaMesa 1 0
    CONTACTS YCorteTravaMesa 1 0
    OSR
    COIL RCortePerfAtivo 0 0 1 0
END
RUNG
    CONTACTS RCortePerfAtivo 0 0
    OSF
    PARALLEL
        COIL RCortePilotar 0 0 1 0
        COIL RCortePerfDesce 0 0 1 0
    END
END
RUNG
    PARALLEL
        SERIES
            CONTACTS XCortePerfManual 0 0
            TOF TConfirmaOFF 100000
            OSF
        END
        SERIES
            CONTACTS RCortePerfAtivo 0 0
            OSF
            CONTACTS RErroCortePiloto 1 0
        END
    END
    COIL RRepeteSemPiloto 0 0 1 0
END
RUNG
    COMMENT Lógica de controle do corte do perfil - Mapeamento entre relés internos para entradas e saídas
END
RUNG
    CONTACTS RCortePerfAtivo 0 0
    CONTACTS RCortePronto 0 0
    PARALLEL
        SERIES
            CONTACTS XSnsInfPiloto 0 0
            CONTACTS XSnsSupPiloto 1 0
        END
        CONTACTS RMaqUsarPiloto 1 0
    END
    CONTACTS RCortePerfDesce 0 0
    COIL YHidrDesce 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RMaqInitCorte 0 0
            CONTACTS RCortePronto 0 0
        END
        SERIES
            CONTACTS RCortePerfAtivo 0 0
            CONTACTS RCortePronto 0 0
            PARALLEL
                SERIES
                    CONTACTS XSnsInfPiloto 0 0
                    CONTACTS XSnsSupPiloto 1 0
                END
                SERIES
                    CONTACTS RMaqUsarPiloto 1 0
                    CONTACTS RSnsSupTravaMesa 0 0
                END
            END
            CONTACTS RCortePerfDesce 1 0
            CONTACTS RSnsSupCortePerf 1 0
        END
    END
    COIL YHidrSobe 0 0 0 0
END
RUNG
    PARALLEL
        CONTACTS YHidrDesce 0 0
        CONTACTS YHidrSobe 0 0
    END
    COIL YHidrVentagem 0 0 0 0
END
RUNG
    CONTACTS RErroCortePerf 0 0
    CONTACTS RCortarPerf 1 0
    CONTACTS RMaqProduzindo 1 0
    CONTACTS RCortePerfAtivo 1 0
    PARALLEL
        COIL RErroCortePerf 0 0 1 0
        COIL RErroCortePiloto 0 0 1 0
    END
END
RUNG
    CONTACTS RMaqOK 0 0
    PARALLEL
        SERIES
            CONTACTS YMbModoAutoAtivo 1 0
            CONTACTS XPerfManAvanca 1 0
            CONTACTS XPerfManRecua 1 0
            PARALLEL
                SERIES
                    CONTACTS XCortePerfManual 0 0
                    OSR
                END
                CONTACTS YMbCorteIHM 0 1
                CONTACTS RMaqCortePerf 0 0
                SERIES
                    CONTACTS RCortePerfAtivo 1 0
                    CONTACTS RRepeteSemPiloto 0 0
                END
            END
        END
        SERIES
            CONTACTS YMbModoAutoAtivo 0 0
            CONTACTS RMaqCortePerf 0 0
        END
    END
    PARALLEL
        SERIES
            OSR
            CONTACTS RCortePerfAtivo 1 0
            COIL RCortarPerf 0 1 0 0
        END
        COIL YMbCorteIHM 0 0 1 0
    END
END
RUNG
    COMMENT Controle da Perfiladeira - Lógica de Saídas
END
RUNG
    PARALLEL
        SERIES
            CONTACTS YMbModoAutoAtivo 0 0
            CONTACTS RInvAtivar 0 0
        END
        SERIES
            PARALLEL
                CONTACTS RAvancarPerfMan 0 0
                CONTACTS RRecuarPerfMan 0 0
            END
            TOF TMantemFreioLib 5000000
        END
    END
    CONTACTS RMaqPronta 0 0
    COIL YInvFreioMotor 0 0 0 0
END
RUNG
    CONTACTS YInvFreioMotor 0 0
    TON TEsperaFreioLib 500000
    COIL RInvFreioLib 0 0 0 0
END
RUNG
    CONTACTS RInvFreioLib 0 0
    PARALLEL
        CONTACTS RInvAtivar 0 0
        CONTACTS RAvancarPerfMan 0 0
    END
    COIL YAvancarPerf 0 0 0 0
END
RUNG
    CONTACTS RInvFreioLib 0 0
    CONTACTS RRecuarPerfMan 0 0
    CONTACTS YAvancarPerf 1 0
    COIL YRecuarPerf 0 0 0 0
END
RUNG
    COMMENT Lógica de controle manual da perfiladeira
END
RUNG
    CONTACTS RMaqPronta 0 0
    PARALLEL
        CONTACTS YMbModoAutoAtivo 1 0
        CONTACTS RMaqModoCorrecao 0 0
    END
    CONTACTS RFerramCorteCima 0 0
    CONTACTS RMaqOperando 1 0
    CONTACTS RMaqCfgInvOK 0 0
    COIL RPerfManualOK 0 0 0 0
END
RUNG
    CONTACTS RPerfManualOK 0 0
    PARALLEL
        CONTACTS XPerfManAvanca 0 0
        CONTACTS YPerfManAvcIHM 0 7
    END
    CONTACTS XPerfManRecua 1 0
    CONTACTS YPerfManRecuaIHM 1 8
    COIL RAvancarPerfMan 0 0 0 0
END
RUNG
    CONTACTS RPerfManualOK 0 0
    CONTACTS XPerfManAvanca 1 0
    CONTACTS YPerfManAvcIHM 1 7
    PARALLEL
        CONTACTS XPerfManRecua 0 0
        CONTACTS YPerfManRecuaIHM 0 8
    END
    PARALLEL
        SERIES
            CONTACTS RBloqueiaRecuo 1 0
            COIL RRecuarPerfMan 0 0 0 0
        END
        SERIES
            PARALLEL
                CONTACTS RInvFreioLib 0 0
                CONTACTS RBloqueiaRecuo 0 0
            END
            TON TMaxPerfRecua 3000000
            COIL RBloqueiaRecuo 0 0 0 0
        END
    END
END
RUNG
    COMMENT Lógica de inicialização da máquina
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RMaqConfigOK 0 0
    CONTACTS RMaqInitOK 1 0
    CONTACTS RMaqInit 1 0
    COIL RMaqInit 0 1 0 0
END
RUNG
    CONTACTS RMaqInit 0 0
    CONTACTS RFerramCorteCima 1 0
    COIL RMaqInitCorte 0 0 0 0
END
RUNG
    CONTACTS RMaqInit 0 0
    CONTACTS RFerramCorteCima 0 0
    COIL RMaqInit 0 0 1 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RMaqInit 0 0
    OSF
    PARALLEL
        COIL RMaqInit 0 0 1 0
        SERIES
            CONTACTS RMaqOK 0 0
            COIL RMaqInitOK 0 1 0 0
        END
    END
END
RUNG
    COMMENT Lógica de configuração do inversor em cada modo de operação
END
RUNG
    CONTACTS YMbInvParamSync 0 5
    PARALLEL
        COIL YMbInvParamSync 0 0 1 5
        COIL RStartCfgInvMan 0 1 0 0
    END
END
RUNG
    CONTACTS RStartCfgInvAuto 0 0
    CONTACTS RMaqCfgInvErro 1 0
    PARALLEL
        SERIES
            OSR
            COIL RMaqCfgInvOK 0 0 1 0
        END
        SERIES
            TON TCfgInvAutoTO 5000000
            COIL RMaqCfgInvErro 0 1 0 0
        END
        SERIES
            WRITE_USS WInvAutoVel 0 105 0 0
            WRITE_USS WInvAutoVel 0 104 0 0
            WRITE_USS WInvAutoAcel 0 102 0 0
            WRITE_USS WInvAutoDesac 0 103 0 0
            WRITE_USS InvTamPeca 0 0 0 0
            PARALLEL
                COIL RStartCfgInvAuto 0 0 1 0
                COIL RMaqCfgInvOK 0 1 0 0
            END
        END
    END
END
RUNG
    CONTACTS RStartCfgInvMan 0 0
    CONTACTS RMaqCfgInvErro 1 0
    PARALLEL
        CONTACTS YMbModoAutoAtivo 1 0
        CONTACTS RMaqModoCorrecao 0 0
    END
    PARALLEL
        SERIES
            OSR
            COIL RMaqCfgInvOK 0 0 1 0
        END
        SERIES
            TON TCfgInvManTO 5000000
            COIL RMaqCfgInvErro 0 1 0 0
        END
        SERIES
            WRITE_USS WInvManVel 0 105 0 0
            WRITE_USS WInvManVel 0 104 0 0
            WRITE_USS WInvManAcel 0 102 0 0
            WRITE_USS WInvManDesac 0 103 0 0
            PARALLEL
                COIL RStartCfgInvMan 0 0 1 0
                COIL RMaqCfgInvOK 0 1 0 0
            END
        END
    END
END
RUNG
    COMMENT Lógica de operação automática da perfiladeira - Checagens Gerais
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RModoAuto 0 0
    COIL RModoAutoOK 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RModoAuto 0 0
            OSF
        END
        SERIES
            TON TEspSistemaOK 10000000
            OSR
        END
    END
    PARALLEL
        COIL RStartCfgInvMan 0 1 0 0
        COIL RStartCfgInvAuto 0 0 1 0
    END
END
RUNG
    CONTACTS RModoAuto 0 0
    OSR
    CONTACTS RMaqProduzindo 1 0
    PARALLEL
        COIL RStartCfgInvAuto 0 1 0 0
        COIL RStartCfgInvMan 0 0 1 0
        MOVE CQtdProd YMbQtd
    END
END
RUNG
    PARALLEL
        CONTACTS RMaqOK 1 0
        CONTACTS RMaqCfgInvErro 0 0
        SERIES
            CONTACTS RErroCortePerf 0 0
            OSR
            CONTACTS RErroCortePiloto 1 0
        END
    END
    CONTACTS RMaqProduzindo 0 0
    PARALLEL
        COIL RMaqProduzindo 0 0 1 0
        COIL RMaqCortePerf 0 0 1 0
        COIL YMbModoAutoAtivo 0 0 1 0
    END
END
RUNG
    CONTACTS RModoAuto 1 0
    CONTACTS RMaqProduzindo 1 0
    CONTACTS YMbModoAutoAtivo 0 0
    COIL YMbModoAutoAtivo 0 0 1 0
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    OSF
    PARALLEL
        COIL RMaqCortePerf 0 0 1 0
        COIL YMbModoAutoAtivo 0 0 1 0
        COIL RMaqModoCorrecao 0 0 1 0
        COIL RSubQtdProd 0 0 1 0
        COIL RStartCfgInvAuto 0 0 1 0
        COIL RStartCfgInvMan 0 1 0 0
    END
END
RUNG
    CONTACTS RMaqConfigOK 0 0
    OSR
    CONTACTS RMaqProduzindo 1 0
    CONTACTS RModoAutoOK 0 0
    GRT CQtdProd 0
    COIL RMaqProduzindo 0 1 0 0
END
RUNG
    PARALLEL
        CONTACTS XInvFimPosic 0 0
        CONTACTS RMaqModoCorrecao 0 0
    END
    CONTACTS RMaqProduzindo 0 0
    TON TFimPosicEstab 1000000
    PARALLEL
        SERIES
            CONTACTS RInvAtivar 0 0
            COIL RMaqFimPosic 0 1 0 0
        END
        SERIES
            OSF
            COIL RMaqFimPosic 0 0 1 0
        END
    END
END
RUNG
    COMMENT Lógica de operação automática da perfiladeira - Loop de Produção
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    COIL YInvPosicao 0 0 0 0
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqFimPosic 1 0
    CONTACTS YInvResetPos 1 0
    CONTACTS RMaqConfigOK 0 0
    COIL RInvAtivar 0 0 0 0
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqFimPosic 0 0
    CONTACTS RInvFreioLib 1 0
    OSR
    CONTACTS RCortePerfAtivo 1 0
    COIL RMaqCortePerf 0 1 0 0
END
RUNG
    PARALLEL
        CONTACTS RCortePerfAtivo 0 0
        CONTACTS RMaqModoCorrecao 0 0
    END
    OSF
    CONTACTS RMaqProduzindo 0 0
    PARALLEL
        SERIES
            CONTACTS RErroCortePerf 0 0
            COIL RMaqModoCorrecao 0 1 0 0
        END
        SERIES
            CONTACTS RMaqModoCorrecao 1 0
            COIL RSubQtdProd 0 1 0 0
        END
    END
END
RUNG
    COMMENT Modo de Correção\r\nSe houve algum problema durante a produção da peça, entra neste modo para tentar corrigir o problema
END
RUNG
    CONTACTS RMaqModoCorrecao 0 0
    PARALLEL
        SERIES
            OSR
            PARALLEL
                COIL RStartCfgInvMan 0 1 0 0
                COIL RStartCfgInvAuto 0 0 1 0
            END
        END
        SERIES
            OSF
            CONTACTS RMaqProduzindo 0 0
            PARALLEL
                COIL RStartCfgInvAuto 0 1 0 0
                COIL RStartCfgInvMan 0 0 1 0
            END
        END
    END
END
RUNG
    CONTACTS RMaqModoCorrecao 0 0
    CONTACTS RMaqCfgInvOK 0 0
    CONTACTS XCortePerfManual 0 0
    CONTACTS RCortePerfAtivo 1 0
    OSR
    PARALLEL
        COIL RMaqCortePerf 0 1 0 0
        COIL RErroCortePerf 0 0 1 0
        COIL RErroCortePiloto 0 0 1 0
    END
END
RUNG
    CONTACTS RMaqModoCorrecao 0 0
    CONTACTS RCortePerfAtivo 0 0
    OSF
    CONTACTS RErroCortePerf 1 0
    COIL RMaqModoCorrecao 0 0 1 0
END
RUNG
    COMMENT Peça produzida. Realiza contagem e, se terminou, interrompe produção
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RSubQtdProd 0 0
            OSR
        END
        SERIES
            CONTACTS RMaqModoCorrecao 0 0
            OSF
        END
    END
    CONTACTS RMaqProduzindo 0 0
    PARALLEL
        COIL RSubQtdProd 0 0 1 0
        SERIES
            PARALLEL
                SERIES
                    CTD CQtdProd 1
                    OSF
                END
                CONTACTS RModoAuto 1 0
            END
            COIL RMaqProduzindo 0 0 1 0
        END
    END
END
RUNG
    PARALLEL
        CONTACTS RMaqProduzindo 0 0
        SERIES
            CONTACTS RMaqProduzindo 0 0
            OSF
        END
    END
    MOVE YMbQtd CQtdProd
END
RUNG
    COMMENT Mapeamento de variáveis do ModBUS
END
RUNG
    CONTACTS XMbModoAuto 0 0
    PARALLEL
        COIL RModoAuto 0 0 0 0
        SERIES
            OSR
            COIL YMbModoAutoAtivo 0 1 0 0
        END
    END
END
RUNG
    CONTACTS YMbLimpaErro 0 3
    PARALLEL
        COIL RMaqCfgInvErro 0 0 1 0
        COIL YMbLimpaErro 0 0 1 3
    END
END
RUNG
    COMMENT Flags de erro da máquina conforme nível de prioridade.\r\nBit 0 -> erro mais crítico / Bit 15 -> erro menos crítico
END
RUNG
    CONTACTS XEmergencia 1 0
    COIL YMbErrEmergencia 0 0 0 0
END
RUNG
    CONTACTS XFaltaFase 1 0
    COIL YMbErrFaltaFase 0 0 0 1
END
RUNG
    CONTACTS XTermBombaHidr 1 0
    COIL YMbErrBombaHidr 0 0 0 2
END
RUNG
    CONTACTS XInversorOK 1 0
    COIL YMbErrInversor 0 0 0 3
END
RUNG
    CONTACTS XDesbobOK 1 0
    COIL YMbDesbobOK 0 0 0 4
END
RUNG
    CONTACTS RMaqCfgInvErro 0 0
    COIL YMbErrInvComunic 0 0 0 5
END
RUNG
    COMMENT Flags que indicam o estado atual da máquina - Somente Leitura
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    COIL YMbMaqProduzindo 0 0 0 2
END
RUNG
    CONTACTS RCortePerfAtivo 0 0
    COIL YMbCorteAtivo 0 0 0 3
END
RUNG
    CONTACTS RMaqInit 0 0
    COIL YMbMaqIniciando 0 0 0 5
END
RUNG
    CONTACTS RMaqInitOK 0 0
    COIL YMbMaqInitOK 0 0 0 6
END
RUNG
    CONTACTS RFerramCorteCima 0 0
    COIL YMbFerramCrtCima 0 0 0 7
END
RUNG
    CONTACTS RMaqPronta 0 0
    COIL YMbMaqPronta 0 0 0 8
END
RUNG
    CONTACTS RMaqOperando 0 0
    COIL YMbMaqOperando 0 0 0 9
END
RUNG
    CONTACTS RMaqCfgInvOK 0 0
    COIL YMbMaqCfgInvOK 0 0 0 10
END
RUNG
    CONTACTS RMaqCortePerf 0 0
    COIL YMbMaqCortePerf 0 0 0 14
END
RUNG
    COMMENT Declaração de variáveis do ModBUS
END
RUNG
    OSR
    PARALLEL
        COIL YMbAutoVel 0 0 1 0
        COIL YMbAutoAcel 0 0 1 0
        COIL YMbAutoDesac 0 0 1 0
        COIL YMbManAcel 0 0 1 0
        COIL YMbManDesac 0 0 1 0
        COIL YMbManVel 0 0 1 0
        COIL YMbFator 0 0 1 0
        COIL YMbPerfFatorH 0 0 1 0
        COIL YMbPerfFatorL 0 0 1 0
        COIL YMbTamFaca 0 0 1 0
        COIL YMbTam 0 0 1 0
        COIL YMbQtd 0 0 1 0
    END
END
RUNG
    OSR
    PARALLEL
        MOVE YMbFator 1000
        MOVE YMbTamFaca 0
        MOVE YMbTam 3000
        MOVE YMbQtd 30
        MOVE YMbAutoVel 100
        MOVE YMbAutoAcel 200
        MOVE YMbAutoDesac 75
        MOVE YMbManVel 30
        MOVE YMbManAcel 400
        MOVE YMbManDesac 100
        MOVE WInvModoAuto 1
        MOVE WInvModoMan 0
    END
END
RUNG
    COMMENT Calculando variáveis conforme os valores recebidos pelo ModBUS
END
RUNG
    COMMENT Calculando fator de relação entre o número de voltas do motor e do encoder
END
RUNG
    MUL WMbPerfFator YMbPerfFatorH 65536
END
RUNG
    ADD WMbPerfFator WMbPerfFator YMbPerfFatorL
END
RUNG
    COMMENT Convertendo tamanho recebido da peça de mm para voltas do motor (Inversor NORD)
END
RUNG
    ADD InvTamPeca YMbTam YMbTamFaca
END
RUNG
    MUL InvTamPeca InvTamPeca YMbFator
END
RUNG
    DIV InvTamPeca InvTamPeca 1000
END
RUNG
    MUL InvTamPeca InvTamPeca WMbPerfFator
END
RUNG
    DIV InvTamPeca InvTamPeca 1000
END
RUNG
    COMMENT Calculando parametros de velocidade para enviar ao inversor
END
RUNG
    MUL WInvAutoVel YMbAutoVel 60
END
RUNG
    DIV WInvAutoVel WInvAutoVel 10
END
RUNG
    MUL WInvManVel YMbManVel 60
END
RUNG
    DIV WInvManVel WInvManVel 10
END
RUNG
    COMMENT Calculando parametros de aceleração / desaceleração para enviar ao inversor
END
RUNG
    MOVE WInvAutoAcel YMbAutoAcel
END
RUNG
    MOVE WInvAutoDesac YMbAutoDesac
END
RUNG
    MOVE WInvManAcel YMbManAcel
END
RUNG
    MOVE WInvManDesac YMbManDesac
END
