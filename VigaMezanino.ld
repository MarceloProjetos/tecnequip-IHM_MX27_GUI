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
COMPILED=N:\windows\Projetos\Maquinas\VigaMezanino\VigaMezanino.hex

IO LIST
    XEmergencia at 1 0
    XTermBombaHidr at 2 0
    XInversorOK at 3 0
    XPerfFimMaterial at 4 0
    XSnsSupHidr at 5 0
    XSnsInfHidr at 6 0
    XSnsSupPiloto at 7 0
    XSnsInfPiloto at 8 0
    XInvFimPosic at 9 0
    XFaltaFase at 10 0
    XPerfManAvanca at 11 0
    XPerfManRecua at 12 0
    XCortePerfManual at 13 0
    XDesbobOK at 14 0
    XCortarCanto at 15 0
    XSnsSupCrtCanto at 16 0
    XSnsInfCrtCanto at 17 0
    XTermCrtCanto at 18 0
    XInvFreioAtivo at 19 0
    YLigarBombaHidr at 1 0
    YHidrDesce at 2 0
    YHidrSobe at 3 0
    YCorteTravaMesa at 4 0
    YPilotoDescer at 5 0
    YInvFreioMotor at 6 0
    YLoopDesbob at 7 0
    YAvancarPerf at 8 0
    YRecuarPerf at 9 0
    YInvPosicao at 10 0
    YInvResetPos at 11 0
    YAvancarCrtCanto at 12 0
    YRecuarCrtCanto at 13 0
    YCrtCantoVentag at 14 0
    YBombaCrtCanto at 15 0
    YMbErrEmergencia at 20 0
    YMbErrFaltaFase at 20 1
    YMbErrBombaHidr at 20 2
    YMbErrInversor at 20 3
    YMbErrDesbob at 20 4
    YMbErrInvComunic at 20 5
    YMbErrCortePerf at 20 6
    YMbModoAutoAtivo at 21 0
    YMbModoAuto at 22 0
    YMbCorteIHM at 22 1
    YMbLimpaErro at 22 2
    YPerfManAvcIHM at 22 3
    YPerfManRecIHM at 22 4
    YMbPerfSync at 22 5
    YMbPerfFatorL at 30 0
    YMbPerfFatorH at 31 0
    YMbAutoAcel at 32 0
    YMbAutoDesac at 33 0
    YMbAutoVel at 34 0
    YMbManAcel at 35 0
    YMbManDesac at 36 0
    YMbManVel at 37 0
    YMbEncFator at 40 0
    YMbEncResol at 41 0
    YMbEncPerim at 42 0
    YMbFacaTam at 45 0
    XDbgModoAuto at 49 0
    YMbQtd at 50 0
    YMbTam at 51 0
    YLedUser at 18 0
END

PROGRAM
RUNG
    COMMENT Programa de Controle da Perfiladeira de Viga de Mezanino - v1.0\r\nDesenvolvido por Marcelo Utikawa da Fonseca em 23/07/2010
END
RUNG
    COMMENT Descrição das flags utilizadas no programa:
END
RUNG
    COMMENT AvancarPerfMan\r\n   Ativa quando acionado botao de avanço manual da perfiladeira e a máquina em modo manual
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
    COMMENT CortarPerfAuto\r\n   Flag gerada pelo loop de produção do modo automático, solicitando o corte do perfil
END
RUNG
    COMMENT CortePronto\r\n   Ativo quando a máquina está pronta para realizar um corte
END
RUNG
    COMMENT CortePerfAtivo\r\n   Flag indicando que um ciclo de corte está sendo realizado
END
RUNG
    COMMENT CortePerfDesce\r\n   Ativo quando, durante um ciclo de corte, a máquina está realizando o movimento de descida
END
RUNG
    COMMENT CortePilotar\r\n   Indica que a mesa foi destravada e assim o piloto deve ser acionado
END
RUNG
    COMMENT CrtCantoAtivo\r\n   Indica que está sendo realizado um ciclo da ferramenta de corte de canto. Desativa apenas quando operador soltar o pedal de comando
END
RUNG
    COMMENT CrtCantoDesce\r\n   Ativo quando, durante um ciclo da ferramenta de corte de canto, a máquina está realizando o movimento de descida
END
RUNG
    COMMENT CrtCantoOK\r\n   Indica que a bomba hidráulica do corte de canto está ligada e estabilizada, podendo assim realizar um ciclo de corte
END
RUNG
    COMMENT ErroCortePerf\r\n   Ativa se ocorrer um erro durante o ciclo de corte do perfil. Desativa após 3 segundos se o erro foi causado por problema no piloto, senão deve ser desativado pela flag YMbLimpaErro
END
RUNG
    COMMENT ErroCortePiloto\r\n   Indica que occoreu erro durante tentativa de pilotagem do perfil, interrompendo o processo de corte
END
RUNG
    COMMENT ErroGeral\r\n   Erro grave na máquina: emergência, falta de fase
END
RUNG
    COMMENT FerramCorteCima\r\n   Indica que a ferramenta de corte e piloto estão na posição superior, permitindo o movimento do perfil
END
RUNG
    COMMENT ForcaRecuoCorte\r\n   Indica a necessidade de recuo da ferramenta de corte ou piloto por não estarem na posição superior enquanto fora da operação de corte
END
RUNG
    COMMENT InvFreioAtivo\r\n   Ativo quando a máquina está com o freio acionado
END
RUNG
    COMMENT InvFreioLib\r\n   Ativo quando o freio da perfiladeira estiver ligado e liberado, permitindo o movimento do perfil
END
RUNG
    COMMENT MaqConfigErro\r\n   Ativo quando ocorrer erro comunicando com o inversor
END
RUNG
    COMMENT MaqConfigOK\r\n   Indica que a máquina foi configurada e pode iniciar a produção automática. Ativo apenas após a transmissão de todos os parâmetros ao inversor
END
RUNG
    COMMENT MaqFimPosic\r\n   Ativo após o inversor terminar o posicionamento. Desativa após o reset da medida do encoder
END
RUNG
    COMMENT MaqIniciarPosic\r\n   Ativo durante o posicionamento do perfil no modo automático. Desativa após o termino do posicionamento
END
RUNG
    COMMENT MaqModoAjuste\r\n   Indica que a máquina está em modo de ajuste, ou seja, ocorreu erro ao pilotar no modo automático e a máquina está aguardando o operador solicitar nova tentativa de corte
END
RUNG
    COMMENT MaqOK\r\n   Indica que a máquina está sem qualquer erro e pode operar
END
RUNG
    COMMENT MaqProduzindo\r\n   Indica que a máquina está no loop de produção. Somente desativa caso ocorrer algum erro, desativar a produção ou terminar a quantidade de peças programada
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
    COMMENT SnsInfCortePerf\r\n   Representa o estado do sensor inferior da ferramenta de corte
END
RUNG
    COMMENT SnsSupCortePerf\r\n   Representa o estado do sensor superior da ferramenta de corte
END
RUNG
    COMMENT SnsSupTravaMesa\r\n   Representa o estado do sensor da trava da mesa. Ativo quando a mesa estiver destravada
END
RUNG
    COMMENT StartConfigAuto\r\n   Solicita a configuração do inversor para o modo automático
END
RUNG
    COMMENT StartConfigMan\r\n   Solicita a configuração do inversor para o modo manual
END
RUNG
    COMMENT Descrição dos Timers utilizados no programa:
END
RUNG
    COMMENT BombaHidrEstab\r\n   Espera para estabilizar a pressão hidráulica após ligar a bomba hidráulica
END
RUNG
    COMMENT BombaHidrInat\r\n   Se depois de 5 minutos não houve nenhum ciclo de corte, a bomba é desligada automaticamente
END
RUNG
    COMMENT ConfigAutoTO\r\n   Timeout na configuração do inversor para modo automático
END
RUNG
    COMMENT ConfigManTO\r\n   Timeout na configuração do inversor para modo manual
END
RUNG
    COMMENT CrtCantoEstab\r\n   Espera para estabilizar a pressão hidráulica após ligar a bomba de corte de canto
END
RUNG
    COMMENT EspTravaSair\r\n   Tempo de espera para destravar a mesa visto que a trava atual não possui sensor
END
RUNG
    COMMENT EsperaFreioLib\r\n   Aguarda a liberação mecânica do freio da perfiladeira após ele ser ligado
END
RUNG
    COMMENT EsperaInit\r\n   Tempo de espera de 10 segundos após ligar o CLP para ativar a flag solicitando configuração manual do inversor, garantindo que ele já está respondendo
END
RUNG
    COMMENT EsperaPilotar\r\nTempo de espera de 50 ms para pilotar, garantindo o completo retorno do piloto entre as tentativas
END
RUNG
    COMMENT FimPosicEstab\r\n   Espera o sinal de fim de posicionamento ficar alto por pelo menos 1 segundo, indicando que a máquina realmente terminou o posicionamento
END
RUNG
    COMMENT LimpaErroCorte\r\n   Aguarda 3 segundos para limpar a flag de erro de corte se o erro foi causado pelo piloto
END
RUNG
    COMMENT MaxCorteCanto\r\n   Indica que depois de 10 segundos o ciclo de corte da ferramenta de canto não terminou
END
RUNG
    COMMENT MaxCortePerf\r\n   Indica que depois de 15 segundos o ciclo de corte do perfil não terminou
END
RUNG
    COMMENT MaxPerfRecua\r\n   Tempo máximo permitido para a perfiladeira recuar no modo manual, evitando problemas com o perfil
END
RUNG
    COMMENT PilotoDesceTO\r\n   Tempo máximo de espera para o piloto descer
END
RUNG
    COMMENT RecuoCorteTO\r\n   Tempo de espera para o perfil recuar após um erro, antes de interromper o processo
END
RUNG
    COMMENT EspTravaEntrar\r\n   Tempo de espera para travar a mesa visto que a trava atual não possui sensor
END
RUNG
    COMMENT MantemFreioLib\r\n   Depois que o movimento manual da perfiladeira é desligado, mantém-se o freio ativo para permitir a máquina desacelerar antes de ativar o freio novamente
END
RUNG
    COMMENT Descrição dos Contadores utilizados no programa:
END
RUNG
    COMMENT NumTentPiloto\r\n   Número de tentativas de pilotagem antes de gerar erro do piloto
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
        COIL YLoopDesbob 1 0 0 0
    END
END
RUNG
    CONTACTS RErroGeral 1 0
    CONTACTS XInversorOK 0 0
    CONTACTS XTermBombaHidr 0 0
    CONTACTS XDesbobOK 0 0
    COIL RMaqOK 0 0 0 0
END
RUNG
    COMMENT Lógica de controle da ferramenta de corte de canto
END
RUNG
    CONTACTS RErroGeral 1 0
    CONTACTS XCortarCanto 0 0
    CONTACTS RCrtCantoAtivo 1 0
    CONTACTS XTermCrtCanto 0 0
    PARALLEL
        COIL RCrtCantoAtivo 0 1 0 0
        COIL YBombaCrtCanto 0 1 0 0
    END
END
RUNG
    CONTACTS YBombaCrtCanto 0 0
    TON TCrtCantoEstab 1500000
    PARALLEL
        SERIES
            OSR
            COIL RCrtCantoDesce 0 1 0 0
        END
        COIL RCrtCantoOK 0 0 0 0
        COIL YCrtCantoVentag 0 0 0 0
    END
END
RUNG
    CONTACTS RCrtCantoOK 0 0
    CONTACTS RCrtCantoDesce 0 0
    PARALLEL
        COIL YAvancarCrtCanto 0 0 0 0
        SERIES
            CONTACTS XSnsInfCrtCanto 0 0
            COIL RCrtCantoDesce 0 0 1 0
        END
    END
END
RUNG
    CONTACTS RCrtCantoOK 0 0
    CONTACTS RCrtCantoDesce 1 0
    PARALLEL
        COIL YRecuarCrtCanto 0 0 0 0
        SERIES
            CONTACTS XSnsSupCrtCanto 0 0
            COIL YBombaCrtCanto 0 0 1 0
        END
    END
END
RUNG
    CONTACTS RCrtCantoOK 0 0
    PARALLEL
        TON TMaxCorteCanto 10000000
        CONTACTS RErroGeral 0 0
        CONTACTS XTermCrtCanto 0 0
    END
    PARALLEL
        COIL YBombaCrtCanto 0 0 1 0
        COIL RCrtCantoDesce 0 0 1 0
    END
END
RUNG
    CONTACTS XCortarCanto 1 0
    CONTACTS RCrtCantoAtivo 0 0
    CONTACTS YBombaCrtCanto 1 0
    COIL RCrtCantoAtivo 0 0 1 0
END
RUNG
    COMMENT Lógica de controle do corte do perfil - Lógicas Gerais
END
RUNG
    CONTACTS XSnsSupHidr 0 0
    COIL RSnsSupCortePerf 0 0 0 0
END
RUNG
    CONTACTS XSnsInfHidr 0 0
    COIL RSnsInfCortePerf 0 0 0 0
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RInvFreioAtivo 0 0
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
    CONTACTS XSnsInfPiloto 1 0
    CONTACTS XSnsSupPiloto 0 0
    TON TEspPilotoEstab 50000
    COIL RCortePilotoCima 0 0 0 0
END
RUNG
    CONTACTS YCorteTravaMesa 0 0
    TON TEspTravaSair 500000
    TOF TEspTravaEntrar 500000
    COIL RSnsSupTravaMesa 0 0 0 0
END
RUNG
    COMMENT Lógica de controle do corte do perfil - Controle da Bomba Hidráulica
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS YLigarBombaHidr 1 0
    PARALLEL
        CONTACTS RCortePerfAtivo 0 0
        CONTACTS RForcaRecuoCorte 0 0
    END
    COIL YLigarBombaHidr 0 1 0 0
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
    CONTACTS YLigarBombaHidr 0 0
    TON TBombaHidrEstab 5000000
    COIL RBombaHidrOK 0 0 0 0
END
RUNG
    COMMENT Lógica de controle do corte do perfil - Operação de Corte
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RErroCortePerf 1 0
    CONTACTS RCortarPerf 0 0
    CONTACTS RMaqOperando 1 0
    CONTACTS RCortePerfAtivo 1 0
    CONTACTS YAvancarPerf 1 0
    CONTACTS YRecuarPerf 1 0
    PARALLEL
        COIL RCortePerfAtivo 0 1 0 0
        COIL YCorteTravaMesa 0 1 0 0
        COIL RCortarPerf 0 0 1 0
        COIL YMbCorteIHM 0 0 1 1
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
    COIL RCortePerfDesce 0 0 1 0
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
    CONTACTS RCortePronto 0 0
    PARALLEL
        SERIES
            CONTACTS RCortePerfAtivo 0 0
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
            CONTACTS RSnsSupCortePerf 1 0
        END
        CONTACTS RForcaRecuoCorte 0 0
    END
    CONTACTS RCortePerfDesce 1 0
    COIL YHidrSobe 0 0 0 0
END
RUNG
    CONTACTS RCortePerfAtivo 1 0
    CONTACTS RErroCortePerf 1 0
    CONTACTS RSnsSupCortePerf 1 0
    COIL RForcaRecuoCorte 0 0 0 0
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
            PARALLEL
                CONTACTS YMbModoAutoAtivo 1 0
                CONTACTS RMaqModoAjuste 0 0
            END
            CONTACTS XPerfManAvanca 1 0
            CONTACTS XPerfManRecua 1 0
            PARALLEL
                SERIES
                    CONTACTS XCortePerfManual 0 0
                    OSR
                END
                CONTACTS YMbCorteIHM 0 1
                SERIES
                    CONTACTS RCortePerfAtivo 1 0
                    CONTACTS RRepeteSemPiloto 0 0
                END
            END
        END
        SERIES
            CONTACTS RMaqProduzindo 0 0
            CONTACTS RCortarPerfAuto 0 0
        END
    END
    PARALLEL
        SERIES
            OSR
            CONTACTS RCortePerfAtivo 1 0
            COIL RCortarPerf 0 1 0 0
        END
        COIL YMbCorteIHM 0 0 1 1
    END
END
RUNG
    COMMENT Controle da Perfiladeira - Lógica de Saídas
END
RUNG
    PARALLEL
        CONTACTS RMaqIniciarPosic 0 0
        SERIES
            PARALLEL
                CONTACTS RAvancarPerfMan 0 0
                CONTACTS RRecuarPerfMan 0 0
            END
            TOF TMantemFreioLib 2000000
        END
    END
    CONTACTS RMaqOK 0 0
    CONTACTS RCortePerfAtivo 1 0
    COIL YInvFreioMotor 0 0 0 0
END
RUNG
    CONTACTS YInvFreioMotor 0 0
    CONTACTS XInvFreioAtivo 0 0
    COIL RInvFreioLib 0 0 0 0
END
RUNG
    CONTACTS YInvFreioMotor 1 0
    CONTACTS XInvFreioAtivo 1 0
    COIL RInvFreioAtivo 0 0 0 0
END
RUNG
    CONTACTS RInvFreioLib 0 0
    PARALLEL
        CONTACTS RMaqIniciarPosic 0 0
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
    CONTACTS RMaqOK 0 0
    PARALLEL
        CONTACTS YMbModoAutoAtivo 1 0
        CONTACTS RMaqModoAjuste 0 0
    END
    CONTACTS RFerramCorteCima 0 0
    CONTACTS RCortePerfAtivo 1 0
    CONTACTS RMaqConfigOK 0 0
    COIL RPerfManualOK 0 0 0 0
END
RUNG
    CONTACTS RPerfManualOK 0 0
    PARALLEL
        CONTACTS XPerfManAvanca 0 0
        CONTACTS YPerfManAvcIHM 0 3
    END
    CONTACTS XPerfManRecua 1 0
    CONTACTS YPerfManRecIHM 1 4
    COIL RAvancarPerfMan 0 0 0 0
END
RUNG
    CONTACTS RPerfManualOK 0 0
    CONTACTS XPerfManAvanca 1 0
    CONTACTS YPerfManAvcIHM 1 3
    PARALLEL
        CONTACTS XPerfManRecua 0 0
        CONTACTS YPerfManRecIHM 0 4
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
    COMMENT Lógica de configuração do inversor em cada modo de operação
END
RUNG
    CONTACTS RStartConfigAuto 0 0
    CONTACTS RMaqConfigErro 1 0
    PARALLEL
        SERIES
            OSR
            COIL RMaqConfigOK 0 0 1 0
        END
        SERIES
            TON TConfigAutoTO 5000000
            COIL RMaqConfigErro 0 1 0 0
        END
        SERIES
            WRITE_USS WInvModoAuto 0 600 0 0
            WRITE_USS WInvAutoVel 0 105 0 0
            WRITE_USS WInvAutoAcel 0 102 0 0
            WRITE_USS WInvAutoDesac 0 103 0 0
            WRITE_USS WInvTam 0 613 0 1
            PARALLEL
                COIL RStartConfigAuto 0 0 1 0
                COIL RMaqConfigOK 0 1 0 0
            END
        END
    END
END
RUNG
    CONTACTS RStartConfigMan 0 0
    CONTACTS RMaqConfigErro 1 0
    PARALLEL
        CONTACTS RMaqProduzindo 1 0
        CONTACTS RMaqModoAjuste 0 0
    END
    PARALLEL
        SERIES
            OSR
            COIL RMaqConfigOK 0 0 1 0
        END
        SERIES
            TON TConfigManTO 5000000
            COIL RMaqConfigErro 0 1 0 0
        END
        SERIES
            WRITE_USS WInvModoMan 0 600 0 0
            WRITE_USS WInvManVel 0 105 0 0
            WRITE_USS WInvManAcel 0 102 0 0
            WRITE_USS WInvManDesac 0 103 0 0
            PARALLEL
                COIL RStartConfigMan 0 0 1 0
                COIL RMaqConfigOK 0 1 0 0
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
            TON TEsperaInit 10000000
            OSR
        END
    END
    PARALLEL
        COIL RStartConfigMan 0 1 0 0
        COIL RStartConfigAuto 0 0 1 0
        SERIES
            CONTACTS RMaqProduzindo 1 0
            COIL YMbModoAutoAtivo 0 0 1 0
        END
    END
END
RUNG
    CONTACTS RModoAuto 0 0
    OSR
    CONTACTS RMaqProduzindo 1 0
    PARALLEL
        COIL RStartConfigAuto 0 1 0 0
        COIL RStartConfigMan 0 0 1 0
        COIL YInvResetPos 0 1 0 0
        MOVE CQtdProd YMbQtd
    END
END
RUNG
    CONTACTS RMaqConfigOK 0 0
    OSR
    CONTACTS RMaqProduzindo 1 0
    CONTACTS RModoAutoOK 0 0
    CONTACTS XPerfFimMaterial 0 0
    GRT CQtdProd 0
    PARALLEL
        COIL RMaqProduzindo 0 1 0 0
        COIL YInvPosicao 0 1 0 0
        COIL YInvResetPos 0 0 1 0
    END
END
RUNG
    PARALLEL
        CONTACTS XInvFimPosic 0 0
        CONTACTS RMaqModoAjuste 0 0
    END
    CONTACTS RMaqProduzindo 0 0
    TON TFimPosicEstab 1000000
    PARALLEL
        SERIES
            CONTACTS RMaqIniciarPosic 0 0
            COIL RMaqFimPosic 0 1 0 0
        END
        SERIES
            OSF
            COIL RMaqFimPosic 0 0 1 0
        END
    END
END
RUNG
    PARALLEL
        CONTACTS RMaqOK 1 0
        SERIES
            CONTACTS RErroCortePerf 0 0
            CONTACTS RErroCortePiloto 1 0
        END
    END
    CONTACTS RMaqProduzindo 0 0
    COIL RMaqProduzindo 0 0 1 0
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    OSF
    PARALLEL
        COIL RCortarPerfAuto 0 0 1 0
        COIL YInvResetPos 0 0 1 0
        COIL YInvPosicao 0 0 1 0
        COIL YMbModoAutoAtivo 0 0 1 0
        COIL RMaqModoAjuste 0 0 1 0
    END
END
RUNG
    COMMENT Lógica de operação automática da perfiladeira - Loop de Produção
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqFimPosic 1 0
    CONTACTS YInvResetPos 1 0
    CONTACTS RMaqConfigOK 0 0
    COIL RMaqIniciarPosic 0 0 0 0
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqFimPosic 0 0
    CONTACTS YInvFreioMotor 1 0
    OSR
    CONTACTS RMaqModoAjuste 1 0
    COIL RCortarPerfAuto 0 1 0 0
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqFimPosic 0 0
    CONTACTS RCortePerfAtivo 0 0
    PARALLEL
        COIL RCortarPerfAuto 0 0 1 0
        SERIES
            OSF
            CONTACTS RErroCortePerf 1 0
            PARALLEL
                COIL YInvResetPos 0 1 0 0
                SERIES
                    CONTACTS RMaqModoAjuste 0 0
                    PARALLEL
                        COIL RMaqModoAjuste 0 0 1 0
                        SERIES
                            CONTACTS RModoAuto 0 0
                            COIL RStartConfigAuto 0 1 0 0
                        END
                    END
                END
            END
        END
    END
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RCortePerfAtivo 1 0
    CONTACTS RErroCortePiloto 0 0
    OSR
    PARALLEL
        COIL RMaqModoAjuste 0 1 0 0
        COIL RErroCortePerf 0 0 1 0
        COIL RErroCortePiloto 0 0 1 0
        COIL RStartConfigMan 0 1 0 0
    END
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS YInvResetPos 0 0
    CONTACTS RMaqFimPosic 1 0
    PARALLEL
        COIL YInvResetPos 0 0 1 0
        SUB YMbQtd YMbQtd 1
        SERIES
            PARALLEL
                SERIES
                    CTD CQtdProd 1
                    OSF
                END
                CONTACTS XPerfFimMaterial 1 0
                CONTACTS RModoAuto 1 0
            END
            COIL RMaqProduzindo 0 0 1 0
        END
    END
END
RUNG
    COMMENT Mapeamento de variáveis do ModBUS
END
RUNG
    CONTACTS XDbgModoAuto 0 0
    PARALLEL
        COIL YMbModoAuto 0 1 0 0
        SERIES
            OSR
            PARALLEL
                MOVE YMbQtd 3
                MOVE YMbTam 1000
            END
        END
    END
END
RUNG
    CONTACTS YMbModoAuto 0 0
    COIL RModoAuto 0 0 0 0
END
RUNG
    CONTACTS RModoAuto 0 0
    OSR
    COIL YMbModoAutoAtivo 0 1 0 0
END
RUNG
    CONTACTS YMbLimpaErro 0 2
    CONTACTS RMaqOK 0 0
    PARALLEL
        COIL YMbLimpaErro 0 0 1 2
        SERIES
            CONTACTS RMaqProduzindo 1 0
            PARALLEL
                COIL RMaqConfigErro 0 0 1 0
                COIL RErroCortePerf 0 0 1 0
                COIL RErroCortePiloto 0 0 1 0
            END
        END
    END
END
RUNG
    CONTACTS YMbPerfSync 0 5
    CONTACTS RModoAuto 1 0
    PARALLEL
        COIL RStartConfigMan 0 1 0 0
        COIL YMbPerfSync 0 0 1 5
    END
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
    COIL YMbErrDesbob 0 0 0 4
END
RUNG
    CONTACTS RMaqConfigErro 0 0
    COIL YMbErrInvComunic 0 0 0 5
END
RUNG
    CONTACTS RErroCortePerf 0 0
    COIL YMbErrCortePerf 0 0 0 6
END
RUNG
    COMMENT Declaração de variáveis do ModBUS
END
RUNG
    OSR
    PARALLEL
        COIL YMbPerfFatorL 0 0 1 0
        COIL YMbPerfFatorH 0 0 1 0
        COIL YMbAutoAcel 0 0 1 0
        COIL YMbAutoDesac 0 0 1 0
        COIL YMbAutoVel 0 0 1 0
        COIL YMbManAcel 0 0 1 0
        COIL YMbManDesac 0 0 1 0
        COIL YMbManVel 0 0 1 0
        COIL YMbEncFator 0 0 1 0
        COIL YMbEncResol 0 0 1 0
        COIL YMbEncPerim 0 0 1 0
        COIL YMbFacaTam 0 0 1 0
        COIL YMbTam 0 0 1 0
        COIL YMbQtd 0 0 1 0
    END
END
RUNG
    OSR
    PARALLEL
        MOVE YMbPerfFatorL 4678
        MOVE YMbPerfFatorH 2
        MOVE YMbTam 0
        MOVE YMbAutoVel 100
        MOVE YMbAutoAcel 200
        MOVE YMbAutoDesac 150
        MOVE YMbManVel 30
        MOVE YMbManAcel 400
        MOVE YMbManDesac 100
        MOVE YMbFacaTam 5
        MOVE WInvModoAuto 1
        MOVE WInvModoMan 0
        MOVE YMbEncFator 1000
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
    COMMENT Calculando tamanho da peça para enviar ao inversor
END
RUNG
    ADD WInvTam YMbTam YMbFacaTam
END
RUNG
    MUL WInvTam WInvTam YMbEncFator
END
RUNG
    DIV WInvTam WInvTam 1000
END
RUNG
    MUL WInvTam WInvTam WMbPerfFator
END
RUNG
    DIV WInvTam WInvTam 1000
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
