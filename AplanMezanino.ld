POPTools:1.4
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=19200
PARITY=4
MODBUSID=0
COM=0
IP=192.168.0.235
MASK=255.255.255.0
GW=192.168.0.10
DNS=192.168.0.1
SNTP=9-0:br.pool.ntp.org
X4=1

IO LIST
    EMaqPosAtual at 1 0 type 4096
    XAplanFechada at 6 0 type 1
    XAplanManAvanca at 9 0 type 1
    XAplanManRecua at 10 0 type 1
    XDbgDeslReadEnc at 23 1 type 1
    XDbgLimpaErro at 23 7 type 1
    XDbgMaqLib at 23 0 type 1
    XDbgParamSync at 23 6 type 1
    XDbgSimulaEnc at 23 2 type 1
    XDbgSimulaEncNeg at 23 8 type 1
    XDbgValErro0 at 23 4 type 1
    XDbgValErro48 at 23 5 type 1
    XEmergencia at 1 0 type 1
    XFaltaFase at 4 0 type 1
    XMbModoAuto at 22 0 type 1
    XPressaoArOK at 18 0 type 1
    XPrsAvancoOK at 5 0 type 1
    XSrvInPos at 19 0 type 1
    YAplanManAvcIHM at 24 0 type 2
    YAplanManRecIHM at 24 1 type 2
    YMbAplanAcelMS at 30 0 type 2
    YMbAplanAvancoOK at 21 4 type 2
    YMbAplanVelAuto at 31 0 type 2
    YMbAplanVelMan at 32 0 type 2
    YMbEncFator at 34 0 type 2
    YMbEncPerim at 35 0 type 2
    YMbEncResol at 36 0 type 2
    YMbErrBombaHidr at 20 2 type 2
    YMbErrEmergencia at 20 0 type 2
    YMbErrFaltaFase at 20 1 type 2
    YMbErrPosic at 20 9 type 2
    YMbErrPressaoAr at 20 4 type 2
    YMbErrServo at 20 6 type 2
    YMbErrSrvComunic at 20 8 type 2
    YMbLimpaErro at 22 2 type 2
    YMbMaqCfgSrvOK at 21 3 type 2
    YMbMaqErroPos at 33 0 type 2
    YMbMaqForaPosic at 21 9 type 2
    YMbMaqLib at 22 1 type 2
    YMbMaqOK at 21 1 type 2
    YMbMaqProduzindo at 21 7 type 2
    YMbMaqPronta at 21 2 type 2
    YMbMaqStartProd at 21 8 type 2
    YMbModoAutoAtivo at 21 0 type 2
    YMbParamSync at 22 3 type 2
    YMbPasso1 at 40 0 type 2
    YMbPasso1Rep at 41 0 type 2
    YMbPasso2 at 42 0 type 2
    YMbPasso2Rep at 43 0 type 2
    YMbPasso3 at 44 0 type 2
    YMbPasso3Rep at 45 0 type 2
    YMbPasso4 at 46 0 type 2
    YMbPasso4Rep at 47 0 type 2
    YMbPasso5 at 48 0 type 2
    YMbPasso5Rep at 49 0 type 2
    YMbPasso6 at 50 0 type 2
    YMbPasso6Rep at 51 0 type 2
    YMbPassos at 39 0 type 2
    YMbPortasHigh at 38 0 type 2
    YMbPortasLow at 37 0 type 2
    YMbPosAtual at 25 0 type 2
    YMbPosicOK at 21 14 type 2
    YMbSrvInPos at 21 13 type 2
    YPorta1 at 13 0 type 2
    YPorta2 at 14 0 type 2
    YPorta3 at 15 0 type 2
    YPrsEmbreagem at 11 0 type 2
    YSrvHabilita at 16 0 type 2
    ZEncPerfil at 1 0 type 8192
END

PROGRAM
RUNG
    COMMENT Programa de Controle da Aplanadora da Prensa de Mezanino (Antiga Porta-Palete) - v1.0\r\nDesenvolvido por Marcelo Utikawa da Fonseca em 23/02/2012
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
    COMMENT CortePronto\r\n   Ativo quando a máquina está pronta para realizar um corte, seja pela serra ou pelo modo hidráulico
END
RUNG
    COMMENT CortePerfAtivo\r\n   Flag indicando que um ciclo de corte está sendo realizado
END
RUNG
    COMMENT CortePerfDesce\r\n   Ativo quando, durante um ciclo de corte, a máquina está realizando o movimento de descida, seja pela serra ou pelo modo hidráulico
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
    COMMENT ErroCortePerf\r\n   Caso ocorra um erro durante o ciclo de corte do perfil, esta flag é ativada. Desativa automaticamente quando desligar o comando de corte manual ou encerrar a produção automática
END
RUNG
    COMMENT ErroGeral\r\n   Erro grave na máquina: emergência, falta de fase
END
RUNG
    COMMENT FerramCorteCima\r\n   Indica que as ferramentas de corte de tudo (serra e hidráulica) estão na posição superior, permitindo o movimento do perfil
END
RUNG
    COMMENT InvFreioLib\r\n   Ativo quando o freio da perfiladeira estiver ligado e liberado, permitindo o movimento do perfil
END
RUNG
    COMMENT MaqErroconfig\r\n   Ativo quando ocorrer erro comunicando com o inversor
END
RUNG
    COMMENT MaqCfgInvOK\r\n   Indica que a máquina foi configurada e pode iniciar a produção automática. Ativo apenas após a transmissão de todos os parâmetros ao inversor
END
RUNG
    COMMENT MaqFimPosic\r\n   Ativo após o inversor terminar o posicionamento. Desativa após o reset da medida do encoder
END
RUNG
    COMMENT MaqIniciarPosic\r\n   Ativo durante o posicionamento do perfil no modo automático. Desativa após o termino do posicionamento
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
    COMMENT ModoCorteSerra\r\n   Ativo quando selecionado para realizar o ciclo de corte usando a Serra ao invés do Corte Hidráulico
END
RUNG
    COMMENT PerfManualOK\r\n   Flag que indica que a perfiladeira está pronta para movimentar em modo manual
END
RUNG
    COMMENT RecuarPerfMan\r\n   Ativa quando acionado botao de recuo manual da perfiladeira e a máquina em modo manual
END
RUNG
    COMMENT SerraOK\r\n   Indica que a serra está ligada e estabilizada, podendo assim realizar um ciclo de corte
END
RUNG
    COMMENT SnsInfCortePerf\r\n   Representa o estado do sensor inferior do tipo de corte selecionado
END
RUNG
    COMMENT SnsSupCortePerf\r\n   Representa o estado do sensor superior do tipo de corte selecionado
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
    COMMENT BombaHidrEstab\r\n   Espera para estabilizar a pressão hidráulica após ligar a bomba de corte do perfil
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
    COMMENT EsperaFreioLib\r\n   Aguarda a liberação mecânica do freio da perfiladeira após ele ser ligado
END
RUNG
    COMMENT FimPosicEstab\r\n   Espera o sinal de fim de posicionamento ficar alto por pelo menos 1 segundo, indicando que a máquina realmente terminou o posicionamento
END
RUNG
    COMMENT MaxCorteCanto\r\n   Indica que depois de 10 segundos o ciclo de corte da ferramenta de canto não terminou
END
RUNG
    COMMENT MaxCortePerf\r\n   Indica que depois de 15 segundos o ciclo de corte da ferramenta de perfil não terminou
END
RUNG
    COMMENT MaxPerfRecua\r\n   Tempo máximo permitido para a perfiladeira recuar no modo manual, evitando problemas com o perfil
END
RUNG
    COMMENT SerraEstab\r\n   Espera para estabilizar a serra após ser ligada
END
RUNG
    COMMENT SerraInat\r\n   Se depois de 5 minutos não houve nenhum ciclo de corte, a serra é desligada automaticamente
END
RUNG
    COMMENT MantemFreioLib\r\n   Depois que o movimento manual da perfiladeira é desligado, mantém-se o freio ativo para permitir a máquina desacelerar antes de ativar o freio novamente
END
RUNG
    COMMENT PulsoSerraPara\r\n   Mantém o sinal de parada rápida da serra ativo por 1 segundo, garantindo que ela receba o sinal
END
RUNG
    COMMENT Descrição dos Contadores utilizados no programa:
END
RUNG
    COMMENT QtdProd\r\n   Contagem de peças que restam produzir antes de finalizar o loop de produção
END
RUNG
    COMMENT Declarando variáveis para evitar erro na compilação
END
RUNG
    OSR
    PARALLEL
        MOVE PortasLow 0
        MOVE PortasHigh 0
    END
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
    CONTACTS XDbgMaqLib 0 0
    COIL YMbMaqLib 0 1 0 1
END
RUNG
    CONTACTS XDbgValErro48 0 5
    MOVE PrsValErro 48
END
RUNG
    CONTACTS XDbgValErro0 0 4
    MOVE PrsValErro 0
END
RUNG
    CONTACTS RPrsPausaSrvOK 1 0
    CONTACTS RMaqCfgSrvErro 1 0
    CONTACTS RMaqProduzindo 1 0
    CONTACTS RMaqSerialBusy 1 0
    TON TEspChecarSrvOK 2000000
    OSR
    PARALLEL
        COIL RPrsChecarSrvOK 0 1 0 0
        COIL RMaqSerialBusy 0 1 0 0
    END
END
RUNG
    COMMENT Lógica para checar estado do servo
END
RUNG
    CONTACTS RPrsChecarSrvOK 0 0
    WRITE_SERVO_YASKAWA 0 (empty) ERR
    TON TEspRespChecar 50000
    READ_SERVO_YASKAWA 0 PrsValErro ERR NONE
    PARALLEL
        COIL RServoOK 0 0 1 0
        COIL RMaqSerialBusy 0 0 1 0
        COIL RPrsChecarSrvOK 0 0 1 0
        SERIES
            EQU PrsValErro 0
            COIL RServoOK 0 1 0 0
        END
    END
END
RUNG
    CONTACTS RCfgSrvErro 0 0
    OSR
    PARALLEL
        COIL RMaqSerialBusy 0 0 1 0
        COIL RSerialReservada 0 0 1 0
        COIL RMaqLerOKSerial 0 0 1 0
        COIL RStartDesabSrv 0 0 1 0
        COIL RStartZerarEnc 0 0 1 0
        COIL RStartCorrigeEnc 0 0 1 0
        COIL RStartCfgSrvJog 0 0 1 0
        COIL RStartCfgSrv 0 0 1 0
    END
END
RUNG
    COMMENT Lógica de checagem de erros da máquina
END
RUNG
    CONTACTS XPressaoArOK 0 0
    TOF TEspPressaoAr 500000
    COIL RPressaoArOK 0 0 0 0
END
RUNG
    CONTACTS RErroGeral 1 0
    CONTACTS RPressaoArOK 0 0
    CONTACTS RServoOK 0 0
    CONTACTS YMbMaqLib 0 1
    COIL RMaqOK 0 0 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    OSF
    PARALLEL
        COIL YMbMaqLib 0 0 1 1
        COIL RAplanLigarMotor 0 0 1 0
        COIL RMaqDesabSrv 0 1 0 0
        COIL RMaqCfgSrvOK 0 0 1 0
    END
END
RUNG
    CONTACTS RMaqCfgSrvErro 0 0
    OSR
    COIL RMaqDesabSrv 0 1 0 0
END
RUNG
    CONTACTS RMaqCfgSrvOK 0 0
    COIL RMaqConfigOK 0 0 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RMaqConfigOK 0 0
    COIL RMaqPronta 0 0 0 0
END
RUNG
    CONTACTS RMaqPronta 0 0
    PARALLEL
        CONTACTS RModoAuto 1 0
        CONTACTS RMaqStartProd 0 0
    END
    COIL RMaqManualOK 0 0 0 0
END
RUNG
    COMMENT *** Lógica de Controle - Aplanadora ***
END
RUNG
    COMMENT Leituras e cálculos relacionados a encoders
END
RUNG
    CONTACTS XDbgDeslReadEnc 1 1
    READ_ENC EMaqPosAtual
END
RUNG
    CONTACTS XDbgSimulaEnc 0 2
    ADD EMaqPosAtual EMaqPosAtual 25
END
RUNG
    CONTACTS XDbgSimulaEncNeg 0 0
    SUB EMaqPosAtual EMaqPosAtual 20
END
RUNG
    COMMENT Converte valor lido do encoder para centésimos de mm.\r\nFórmula: ( ValEnc * EncPerim * 100 / 4 ) / EncResol
END
RUNG
    COMMENT Sendo:\r\nValEnc = Valor lido do encoder
END
RUNG
    COMMENT EncPerim = Perímetro da roda do encoder em mm\r\n100 = Converte mm para centésimos de mm
END
RUNG
    COMMENT 4 = Fator de multiplicação do encoder na leitura, assumindo x4\r\nEncResol = Resolução do encoder em pulsos por rotação
END
RUNG
    MUL MaqPosAtual EMaqPosAtual YMbEncPerim
END
RUNG
    MUL MaqPosAtual MaqPosAtual 100
END
RUNG
    DIV MaqPosAtual MaqPosAtual 4
END
RUNG
    GRT YMbEncResol 0
    DIV MaqPosAtual MaqPosAtual YMbEncResol
END
RUNG
    MUL MaqPosAtual MaqPosAtual YMbEncFator
END
RUNG
    DIV MaqPosAtual MaqPosAtual 10000
END
RUNG
    ADD MaqPosAtual MaqPosAtual MaqPosOffset
END
RUNG
    MOVE YMbPosAtual MaqPosAtual
END
RUNG
    COMMENT Lógica de controle da Aplanadora - Lógicas Gerais
END
RUNG
    CONTACTS XAplanFechada 0 0
    TON TEspAplanFechar 200000
    PARALLEL
        COIL RAplanFechada 0 0 0 0
        SERIES
            OSR
            PARALLEL
                COIL YAplanManAvcIHM 0 0 1 0
                COIL YAplanManRecIHM 0 0 1 1
            END
        END
    END
END
RUNG
    CONTACTS RAplanFechada 0 0
    CONTACTS RPrsAvancoOK 0 0
    COIL RAplanAvancoOK 0 0 0 0
END
RUNG
    COMMENT Recebe retorno de um comando pela serial, carregando o retorno em AplanRetSerial
END
RUNG
    CONTACTS RMaqLerOKSerial 0 0
    TON TEspRetSerial 50000
    READ_SERVO_YASKAWA 0 AplanRetSerial OK
    PARALLEL
        COIL RMaqLerOKSerial 0 0 1 0
        SERIES
            LES AplanRetSerial 0
            COIL RMaqCfgSrvErro 0 1 0 0
        END
    END
END
RUNG
    COMMENT *** Lógicas relacionadas aos passos da aplanadora ***
END
RUNG
    COMMENT Lógica que carrega as portas utilizadas no passo atual
END
RUNG
    PARALLEL
        SERIES
            EQU PassoAtual 1
            CHECK_BIT PortasLow 1 0
        END
        SERIES
            EQU PassoAtual 2
            CHECK_BIT PortasLow 1 5
        END
        SERIES
            EQU PassoAtual 3
            CHECK_BIT PortasLow 1 10
        END
        SERIES
            EQU PassoAtual 4
            CHECK_BIT PortasHigh 1 0
        END
        SERIES
            EQU PassoAtual 5
            CHECK_BIT PortasHigh 1 5
        END
        SERIES
            EQU PassoAtual 6
            CHECK_BIT PortasHigh 1 10
        END
    END
    CONTACTS RMaqProduzindo 0 0
    COIL YPorta1 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            EQU PassoAtual 1
            CHECK_BIT PortasLow 1 1
        END
        SERIES
            EQU PassoAtual 2
            CHECK_BIT PortasLow 1 6
        END
        SERIES
            EQU PassoAtual 3
            CHECK_BIT PortasLow 1 11
        END
        SERIES
            EQU PassoAtual 4
            CHECK_BIT PortasHigh 1 1
        END
        SERIES
            EQU PassoAtual 5
            CHECK_BIT PortasHigh 1 6
        END
        SERIES
            EQU PassoAtual 6
            CHECK_BIT PortasHigh 1 11
        END
    END
    CONTACTS RMaqProduzindo 0 0
    COIL YPorta2 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            EQU PassoAtual 1
            CHECK_BIT PortasLow 1 2
        END
        SERIES
            EQU PassoAtual 2
            CHECK_BIT PortasLow 1 7
        END
        SERIES
            EQU PassoAtual 3
            CHECK_BIT PortasLow 1 12
        END
        SERIES
            EQU PassoAtual 4
            CHECK_BIT PortasHigh 1 2
        END
        SERIES
            EQU PassoAtual 5
            CHECK_BIT PortasHigh 1 7
        END
        SERIES
            EQU PassoAtual 6
            CHECK_BIT PortasHigh 1 12
        END
    END
    CONTACTS RMaqProduzindo 0 0
    COIL YPorta3 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            EQU PassoAtual 1
            CHECK_BIT PortasLow 1 3
        END
        SERIES
            EQU PassoAtual 2
            CHECK_BIT PortasLow 1 8
        END
        SERIES
            EQU PassoAtual 3
            CHECK_BIT PortasLow 1 13
        END
        SERIES
            EQU PassoAtual 4
            CHECK_BIT PortasHigh 1 3
        END
        SERIES
            EQU PassoAtual 5
            CHECK_BIT PortasHigh 1 8
        END
        SERIES
            EQU PassoAtual 6
            CHECK_BIT PortasHigh 1 13
        END
    END
    CONTACTS RMaqProduzindo 0 0
    COIL RPorta4 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            EQU PassoAtual 1
            CHECK_BIT PortasLow 1 4
        END
        SERIES
            EQU PassoAtual 2
            CHECK_BIT PortasLow 1 9
        END
        SERIES
            EQU PassoAtual 3
            CHECK_BIT PortasLow 1 14
        END
        SERIES
            EQU PassoAtual 4
            CHECK_BIT PortasHigh 1 4
        END
        SERIES
            EQU PassoAtual 5
            CHECK_BIT PortasHigh 1 9
        END
        SERIES
            EQU PassoAtual 6
            CHECK_BIT PortasHigh 1 14
        END
    END
    CONTACTS RMaqProduzindo 0 0
    COIL RPorta5 0 0 0 0
END
RUNG
    COMMENT Cálculo do maior passo entre os utilizados no programa atual
END
RUNG
    MOVE PassoMaior Passo1
END
RUNG
    GRT PassosQtd 1
    GRT Passo2 PassoMaior
    MOVE PassoMaior Passo2
END
RUNG
    GRT PassosQtd 2
    GRT Passo3 PassoMaior
    MOVE PassoMaior Passo3
END
RUNG
    GRT PassosQtd 3
    GRT Passo4 PassoMaior
    MOVE PassoMaior Passo4
END
RUNG
    GRT PassosQtd 4
    GRT Passo5 PassoMaior
    MOVE PassoMaior Passo5
END
RUNG
    GRT PassosQtd 5
    GRT Passo6 PassoMaior
    MOVE PassoMaior Passo6
END
RUNG
    COMMENT Lógica de cálculo do offset e seleção de número de repetições para o passo atual
END
RUNG
    EQU PassoAtual 1
    PARALLEL
        SUB PassoOffset PassoMaior Passo1
        MOVE PassoAtualRep Passo1Rep
    END
END
RUNG
    EQU PassoAtual 2
    PARALLEL
        SUB PassoOffset PassoMaior Passo2
        MOVE PassoAtualRep Passo2Rep
    END
END
RUNG
    EQU PassoAtual 3
    PARALLEL
        SUB PassoOffset PassoMaior Passo3
        MOVE PassoAtualRep Passo3Rep
    END
END
RUNG
    EQU PassoAtual 4
    PARALLEL
        SUB PassoOffset PassoMaior Passo4
        MOVE PassoAtualRep Passo4Rep
    END
END
RUNG
    EQU PassoAtual 5
    PARALLEL
        SUB PassoOffset PassoMaior Passo5
        MOVE PassoAtualRep Passo5Rep
    END
END
RUNG
    EQU PassoAtual 6
    PARALLEL
        SUB PassoOffset PassoMaior Passo6
        MOVE PassoAtualRep Passo6Rep
    END
END
RUNG
    COMMENT *** Fim das lógicas relacionadas aos passos da aplanadora ***
END
RUNG
    COMMENT Configura o erro atual como posição do encoder da aplanadora para fazer um reposicionamento
END
RUNG
    CONTACTS RAplanCorrigeEnc 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    CONTACTS RMaqSerialBusy 1 0
    PARALLEL
        COIL RStartCorrigeEnc 0 1 0 0
        COIL RMaqSerialBusy 0 1 0 0
    END
END
RUNG
    MUL MaqPosAtualCorr MaqPosAtual FatorCorrServo
END
RUNG
    DIV MaqPosAtualCorr MaqPosAtualCorr 10000
END
RUNG
    CONTACTS RStartCorrigeEnc 0 0
    WRITE_SERVO_YASKAWA 0 MaqPosAtualCorr ZSET%d
    PARALLEL
        SERIES
            OSR
            COIL RMaqLerOKSerial 0 1 0 0
        END
        SERIES
            CONTACTS RMaqLerOKSerial 1 0
            PARALLEL
                COIL RStartCorrigeEnc 0 0 1 0
                COIL RAplanCorrigeEnc 0 0 1 0
                COIL RMaqSerialBusy 0 0 1 0
            END
        END
    END
END
RUNG
    COMMENT Lógica para desativar o servo
END
RUNG
    CONTACTS RMaqDesabSrv 0 0
    CONTACTS RMaqSerialBusy 1 0
    PARALLEL
        COIL RStartDesabSrv 0 1 0 0
        COIL RMaqDesabSrv 0 0 1 0
        COIL RStartDesabSVOFF 0 0 1 0
        COIL RMaqSerialBusy 0 1 0 0
    END
END
RUNG
    CONTACTS RStartDesabSrv 0 0
    PARALLEL
        SERIES
            TON TEspFimDesabSrv 2000000
            PARALLEL
                COIL RMaqCfgSrvErro 0 1 0 0
                COIL RStartDesabSrv 0 0 1 0
            END
        END
        SERIES
            CONTACTS RStartDesabSVOFF 1 0
            WRITE_SERVO_YASKAWA 0 (empty) SKIP
            PARALLEL
                SERIES
                    OSR
                    COIL RMaqLerOKSerial 0 1 0 0
                END
                SERIES
                    CONTACTS RMaqLerOKSerial 1 0
                    COIL RStartDesabSVOFF 0 1 0 0
                END
            END
        END
        SERIES
            CONTACTS RStartDesabSVOFF 0 0
            WRITE_SERVO_YASKAWA 0 (empty) SVOFF
            PARALLEL
                SERIES
                    OSR
                    COIL RMaqLerOKSerial 0 1 0 0
                END
                SERIES
                    CONTACTS RMaqLerOKSerial 1 0
                    PARALLEL
                        COIL RStartDesabSrv 0 0 1 0
                        COIL RMaqCfgSrvOK 0 0 1 0
                        COIL RMaqSerialBusy 0 0 1 0
                    END
                END
            END
        END
    END
END
RUNG
    COMMENT Lógica para resetar encoder da Aplanadora
END
RUNG
    CONTACTS RAplanZerarEnc 0 0
    CONTACTS RSrvMotorParado 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    CONTACTS RMaqSerialBusy 1 0
    PARALLEL
        COIL RStartZerarEnc 0 1 0 0
        COIL RMaqSerialBusy 0 1 0 0
        MOVE MaqPosOffset PassoOffset
        MOVE EncPosZero MaqPosOffset
    END
END
RUNG
    CONTACTS RStartZerarEnc 0 0
    OSR
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqForaPosic 1 0
    ADD EncPosZero EncPosZero DifPosic
END
RUNG
    CONTACTS RStartZerarEnc 0 0
    WRITE_SERVO_YASKAWA 0 EncPosZero ZSET%d
    PARALLEL
        SERIES
            OSR
            COIL RMaqLerOKSerial 0 1 0 0
        END
        SERIES
            CONTACTS RMaqLerOKSerial 1 0
            PARALLEL
                COIL RAplanZerarEnc 0 0 1 0
                COIL RStartZerarEnc 0 0 1 0
                COIL RMaqSerialBusy 0 0 1 0
                SERIES
                    CONTACTS RMaqCfgSrvErro 1 0
                    PARALLEL
                        RESET_ENC ZEncPerfil
                        SERIES
                            CONTACTS XDbgDeslReadEnc 0 1
                            MOVE EMaqPosAtual 0
                        END
                    END
                END
            END
        END
    END
END
RUNG
    COMMENT Lógica de controle manual da mesa do corte voador (JOG)
END
RUNG
    PARALLEL
        CONTACTS RMaqManualOK 0 0
        SERIES
            CONTACTS RStartCfgSrvJog 0 0
            TOF TEspFimCfgJog 50000
            CONTACTS RMaqOK 0 0
        END
    END
    CONTACTS RAplanAvancoOK 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    COIL RAplanManualOK 0 0 0 0
END
RUNG
    CONTACTS RAplanManualOK 0 0
    PARALLEL
        CONTACTS XAplanManAvanca 0 0
        CONTACTS YAplanManAvcIHM 0 0
    END
    CONTACTS XAplanManRecua 1 0
    CONTACTS YAplanManRecIHM 1 1
    PARALLEL
        COIL RAplanAvancarMan 0 0 0 0
        SERIES
            OSR
            COIL RMaqCfgSrvJog 0 1 0 0
        END
    END
END
RUNG
    CONTACTS RAplanManualOK 0 0
    CONTACTS XAplanManAvanca 1 0
    CONTACTS YAplanManAvcIHM 1 0
    PARALLEL
        CONTACTS XAplanManRecua 0 0
        CONTACTS YAplanManRecIHM 0 1
    END
    PARALLEL
        COIL RAplanRecuarMan 0 0 0 0
        SERIES
            OSR
            COIL RMaqCfgSrvJog 0 1 0 0
        END
    END
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RAplanAvancarMan 0 0
            OSF
            CONTACTS RAplanRecuarMan 1 0
        END
        SERIES
            CONTACTS RAplanRecuarMan 0 0
            OSF
            CONTACTS RAplanAvancaMan 1 0
        END
    END
    COIL RMaqCfgSrvJog 0 1 0 0
END
RUNG
    CONTACTS RMaqCfgSrvJog 0 0
    CONTACTS RMaqSerialBusy 1 0
    CONTACTS RMaqCfgSrvErro 1 0
    CONTACTS RStartCfgSrvJog 1 0
    PARALLEL
        COIL RStartCfgSrvJog 0 1 0 0
        COIL RMaqCfgSrvJog 0 0 1 0
        COIL RMaqSerialBusy 0 1 0 0
        COIL RCfgJogFrente 0 0 1 0
        COIL RCfgJogRecua 0 0 1 0
        COIL RCfgJogParar 0 0 1 0
        SERIES
            CONTACTS RAplanAvancarMan 0 0
            CONTACTS RAplanRecuarMan 1 0
            COIL RCfgJogFrente 0 1 0 0
        END
        SERIES
            CONTACTS RAplanRecuarMan 0 0
            CONTACTS RAplanAvancarMan 1 0
            COIL RCfgJogRecua 0 1 0 0
        END
        SERIES
            CONTACTS RCfgJogFrente 1 0
            CONTACTS RCfgJogRecua 1 0
            COIL RCfgJogParar 0 1 0 0
        END
    END
END
RUNG
    CONTACTS RStartCfgSrvJog 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    CONTACTS RCfgJogFrente 0 0
    PARALLEL
        SERIES
            OSR
            COIL RMaqCfgSrvOK 0 0 1 0
        END
        SERIES
            TON TCfgSrvJogAvcTO 1000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            WRITE_SERVO_YASKAWA 0 AplanVelManual JOGP%d
            CONTACTS RMaqCfgSrvErro 1 0
            PARALLEL
                SERIES
                    OSR
                    COIL RMaqLerOKSerial 0 1 0 0
                END
                SERIES
                    CONTACTS RMaqLerOKSerial 1 0
                    PARALLEL
                        COIL RStartCfgSrvJog 0 0 1 0
                        COIL RMaqCfgSrvOK 0 1 0 0
                    END
                END
            END
        END
    END
END
RUNG
    CONTACTS RStartCfgSrvJog 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    CONTACTS RCfgJogRecua 0 0
    PARALLEL
        SERIES
            OSR
            COIL RMaqCfgSrvOK 0 0 1 0
        END
        SERIES
            TON TCfgSrvJogRecTO 1000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            WRITE_SERVO_YASKAWA 0 AplanVelManual JOGN%d
            CONTACTS RMaqCfgSrvErro 1 0
            PARALLEL
                SERIES
                    OSR
                    COIL RMaqLerOKSerial 0 1 0 0
                END
                SERIES
                    CONTACTS RMaqLerOKSerial 1 0
                    PARALLEL
                        COIL RStartCfgSrvJog 0 0 1 0
                        COIL RMaqCfgSrvOK 0 1 0 0
                    END
                END
            END
        END
    END
END
RUNG
    CONTACTS RStartCfgSrvJog 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    CONTACTS RCfgJogParar 0 0
    PARALLEL
        SERIES
            OSR
            COIL RMaqCfgSrvOK 0 0 1 0
        END
        SERIES
            TON TCfgSrvJogDesTO 1000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            WRITE_SERVO_YASKAWA 0 (empty) SKIP
            CONTACTS RMaqCfgSrvErro 1 0
            PARALLEL
                SERIES
                    OSR
                    COIL RMaqLerOKSerial 0 1 0 0
                END
                SERIES
                    CONTACTS RMaqLerOKSerial 1 0
                    PARALLEL
                        COIL RStartCfgSrvJog 0 0 1 0
                        COIL RMaqCfgSrvOK 0 1 0 0
                    END
                END
            END
        END
    END
END
RUNG
    CONTACTS RStartCfgSrvJog 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    OSF
    PARALLEL
        COIL RStartCfgSrvJog 0 0 1 0
        COIL RMaqSerialBusy 0 0 1 0
    END
END
RUNG
    COMMENT *** Fim da Lógica - Aplanadora ***
END
RUNG
    COMMENT Lógica de configuração do servo
END
RUNG
    CONTACTS XDbgParamSync 0 6
    OSR
    COIL YMbParamSync 0 1 0 3
END
RUNG
    PARALLEL
        CONTACTS YMbParamSync 0 3
        SERIES
            CONTACTS RMaqOK 0 0
            OSR
            CONTACTS RMaqCfgSrvOK 1 0
        END
    END
    PARALLEL
        COIL RMaqCfgSrv 0 1 0 0
        COIL YMbParamSync 0 0 1 3
    END
END
RUNG
    CONTACTS RMaqCfgSrv 0 0
    CONTACTS RMaqSerialBusy 1 0
    CONTACTS RMaqCfgSrvErro 1 0
    PARALLEL
        COIL RStartCfgSrv 0 1 0 0
        COIL RMaqCfgSrv 0 0 1 0
        COIL RMaqSerialBusy 0 1 0 0
    END
END
RUNG
    CONTACTS RStartCfgSrv 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    CONTACTS RMaqProduzindo 1 0
    PARALLEL
        SERIES
            OSR
            COIL RMaqCfgSrvOK 0 0 1 0
        END
        SERIES
            OSF
            PARALLEL
                COIL RCfgSrvParP1OK 0 0 1 0
                COIL RCfgSrvParP2OK 0 0 1 0
                COIL RCfgSrvParP3OK 0 0 1 0
                COIL RCfgSrvParP4OK 0 0 1 0
                COIL RCfgSrvParP5OK 0 0 1 0
                COIL RCfgSrvParP6OK 0 0 1 0
                COIL RCfgSrvParP7OK 0 0 1 0
                COIL RMaqSerialBusy 0 0 1 0
            END
        END
        SERIES
            TON TCfgSrvIniTO 15000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            CONTACTS RCfgSrvParP1OK 1 0
            WRITE_SERVO_YASKAWA 0 (empty) RES
            TON TCfgSrvParP1Fim 5000000
            PARALLEL
                SERIES
                    OSR
                    COIL RMaqLerOKSerial 0 1 0 0
                END
                SERIES
                    CONTACTS RMaqLerOKSerial 1 0
                    COIL RCfgSrvParP1OK 0 1 0 0
                END
            END
        END
        SERIES
            CONTACTS RCfgSrvParP1OK 0 0
            WRITE_SERVO_YASKAWA 0 (empty) ARES
            PARALLEL
                SERIES
                    OSR
                    COIL RMaqLerOKSerial 0 1 0 0
                END
                SERIES
                    CONTACTS RMaqLerOKSerial 1 0
                    COIL RCfgSrvParP2OK 0 1 0 0
                END
            END
        END
        SERIES
            CONTACTS RCfgSrvParP2OK 0 0
            WRITE_SERVO_YASKAWA 0 AplanVelMax SPD%d
            PARALLEL
                SERIES
                    OSR
                    COIL RMaqLerOKSerial 0 1 0 0
                END
                SERIES
                    CONTACTS RMaqLerOKSerial 1 0
                    COIL RCfgSrvParP3OK 0 1 0 0
                END
            END
        END
        SERIES
            CONTACTS RCfgSrvParP3OK 0 0
            WRITE_SERVO_YASKAWA 0 AplanRampa ACC%d
            PARALLEL
                SERIES
                    OSR
                    COIL RMaqLerOKSerial 0 1 0 0
                END
                SERIES
                    CONTACTS RMaqLerOKSerial 1 0
                    COIL RCfgSrvParP4OK 0 1 0 0
                END
            END
        END
        SERIES
            CONTACTS RCfgSrvParP4OK 0 0
            WRITE_SERVO_YASKAWA 0 AplanRampa DEC%d
            PARALLEL
                SERIES
                    OSR
                    COIL RMaqLerOKSerial 0 1 0 0
                END
                SERIES
                    CONTACTS RMaqLerOKSerial 1 0
                    COIL RCfgSrvParP5OK 0 1 0 0
                END
            END
        END
        SERIES
            CONTACTS RCfgSrvParP5OK 0 0
            WRITE_SERVO_YASKAWA 0 AplanPassoCorr POST000=%d
            TON TEspCfgSrvPar5 200000
            PARALLEL
                SERIES
                    OSR
                    COIL RMaqLerOKSerial 0 1 0 0
                END
                SERIES
                    CONTACTS RMaqLerOKSerial 1 0
                    COIL RCfgSrvParP6OK 0 1 0 0
                END
            END
        END
        SERIES
            CONTACTS RCfgSrvParP6OK 0 0
            WRITE_SERVO_YASKAWA 0 AplanVelAuto SPDT000=%d
            TON TEspCfgSrvPar6 200000
            PARALLEL
                SERIES
                    OSR
                    COIL RMaqLerOKSerial 0 1 0 0
                END
                SERIES
                    CONTACTS RMaqLerOKSerial 1 0
                    COIL RCfgSrvParP7OK 0 1 0 0
                END
            END
        END
        SERIES
            CONTACTS RCfgSrvParP7OK 0 0
            WRITE_SERVO_YASKAWA 0 (empty) SVON
            PARALLEL
                SERIES
                    OSR
                    COIL RMaqLerOKSerial 0 1 0 0
                END
                SERIES
                    CONTACTS RMaqLerOKSerial 1 0
                    PARALLEL
                        COIL RStartCfgSrv 0 0 1 0
                        COIL RMaqCfgSrvOK 0 1 0 0
                        COIL RAplanZerarEnc 0 1 0 0
                    END
                END
            END
        END
    END
END
RUNG
    COMMENT Lógica de operação automática da aplanadora - Checagens Gerais
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RModoAuto 0 0
    COIL RModoAutoOK 0 0 0 0
END
RUNG
    PARALLEL
        CONTACTS RMaqOK 1 0
        CONTACTS RMaqCfgSrvErro 0 0
        SERIES
            CONTACTS YSrvHabilita 0 0
            CONTACTS XSrvInPos 0 0
            TON TEspDeslSrvInPos 3000000
        END
        SERIES
            CONTACTS XAplanFechada 1 0
            CONTACTS RMaqProduzindo 0 0
        END
    END
    PARALLEL
        CONTACTS RMaqProduzindo 0 0
        CONTACTS RMaqStartProd 0 0
    END
    PARALLEL
        COIL RMaqProduzindo 0 0 1 0
        COIL RMaqStartProd 0 0 1 0
        COIL YMbModoAutoAtivo 0 0 1 0
        SERIES
            CONTACTS YSrvHabilita 0 0
            CONTACTS XSrvInPos 0 0
            COIL RMaqErroPosic 0 1 0 0
        END
    END
END
RUNG
    CONTACTS RModoAuto 1 0
    CONTACTS RMaqStartProd 1 0
    CONTACTS RMaqProduzindo 1 0
    CONTACTS YMbModoAutoAtivo 0 0
    COIL YMbModoAutoAtivo 0 0 1 0
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    OSF
    PARALLEL
        COIL YMbModoAutoAtivo 0 0 1 0
        COIL RAplanLigarMotor 0 0 1 0
        COIL RMaqInicioAuto 0 0 1 0
        COIL RMaqPosicionando 0 0 1 0
    END
END
RUNG
    COMMENT Lógica de operação automática da perfiladeira - Iniciando Produção
END
RUNG
    CONTACTS RModoAuto 0 0
    OSR
    CONTACTS RMaqStartProd 1 0
    CONTACTS RMaqProduzindo 1 0
    CONTACTS RMaqErroPosic 1 0
    CONTACTS RMaqConfigOK 0 0
    GRT YMbQtd 0
    PARALLEL
        COIL RMaqStartProd 0 1 0 0
        COIL RAplanZerarEnc 0 1 0 0
        MOVE PassoAtual 1
    END
END
RUNG
    CONTACTS RMaqStartProd 0 0
    CONTACTS XAplanFechada 0 0
    TON TEspStartProd 100000
    OSR
    COIL RMaqStartProd 0 0 1 0
END
RUNG
    CONTACTS RMaqStartProd 0 0
    OSF
    PARALLEL
        SERIES
            CONTACTS RModoAutoOK 0 0
            PARALLEL
                COIL RMaqProduzindo 0 1 0 0
                COIL RMaqInicioAuto 0 1 0 0
                MOVE CPassosRep PassoAtualRep
            END
        END
        SERIES
            CONTACTS RModoAutoOK 1 0
            COIL YMbModoAutoAtivo 0 0 1 0
        END
    END
END
RUNG
    COMMENT Lógica de operação automática da linha de Porta-Palete - Loop de Produção
END
RUNG
    CONTACTS XPrsAvancoOK 0 0
    COIL RPrsAvancoOK 0 0 0 0
END
RUNG
    SUB DifPosic MaqPosAtual AplanPasso
END
RUNG
    PARALLEL
        LES DifPosic -75
        GRT DifPosic 75
    END
    COIL RMaqForaPosic 0 0 0 0
END
RUNG
    LES DifPosic 30
    GRT DifPosic -30
    COIL RMaqPosicOK 0 0 0 0
END
RUNG
    CONTACTS RPrsAvancoOK 0 0
    PARALLEL
        SERIES
            OSR
            CONTACTS RMaqProduzindo 0 0
            CONTACTS RMaqErroPosic 1 0
            PARALLEL
                COIL RAplanLigarMotor 0 1 0 0
                COIL RMaqInicioAuto 0 0 1 0
            END
        END
        SERIES
            OSF
            CONTACTS RMaqProduzindo 0 0
            PARALLEL
                SERIES
                    CONTACTS RMaqForaPosic 0 0
                    CONTACTS RMaqInicioAuto 1 0
                    PARALLEL
                        COIL RMaqErroPosic 0 1 0 0
                        DIV YMbMaqErroPos DifPosic 10
                    END
                END
                SERIES
                    CONTACTS RMaqErroPosic 1 0
                    PARALLEL
                        COIL RAplanZerarEnc 0 1 0 0
                        SERIES
                            CONTACTS RMaqInicioAuto 1 0
                            CTD CPassosRep 1
                            OSF
                            PARALLEL
                                SERIES
                                    OSF
                                    MOVE CPassosRep PassoAtualRep
                                END
                                ADD PassoAtual PassoAtual 1
                                SERIES
                                    GRT PassoAtual PassosQtd
                                    PARALLEL
                                        MOVE PassoAtual 1
                                        SUB YMbQtd YMbQtd 1
                                    END
                                END
                            END
                        END
                    END
                END
                COIL RAplanLigarMotor 0 0 1 0
                COIL RMaqPosicionando 0 0 1 0
            END
        END
    END
END
RUNG
    CONTACTS RMaqErroPosic 0 0
    OSF
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RAplanAvancoOK 0 0
    COIL RAplanLigarMotor 0 1 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS XSrvInPos 0 0
            OSR
        END
        SERIES
            CONTACTS RMaqPosicOK 0 0
            OSF
            CONTACTS XSrvInPos 0 0
            CONTACTS RMaqPosicionando 0 0
        END
    END
    CONTACTS RMaqProduzindo 0 0
    PARALLEL
        COIL RAplanLigarMotor 0 0 1 0
        SERIES
            CONTACTS RMaqPosicOK 1 0
            CONTACTS RAplanAvancoOK 0 0
            COIL RAplanCorrigeEnc 0 1 0 0
        END
    END
END
RUNG
    CONTACTS RAplanCorrigeEnc 0 0
    OSF
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RAplanAvancoOK 0 0
    COIL RAplanLigarMotor 0 1 0 0
END
RUNG
    CONTACTS RAplanLigarMotor 0 0
    CONTACTS RAplanCorrigeEnc 1 0
    CONTACTS RAplanZerarEnc 1 0
    PARALLEL
        COIL YSrvHabilita 0 0 0 0
        SERIES
            OSR
            COIL RMaqPosicionando 0 1 0 0
        END
        SERIES
            TOF TEspSrvParar 100000
            COIL RSrvMotorParado 1 0 0 0
        END
    END
END
RUNG
    COMMENT Peça produzida. Se terminou, interrompe produção
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RPrsAvancoOK 0 0
            OSR
            CONTACTS RModoAuto 1 0
        END
        SERIES
            EQU YMbQtd 0
            OSR
        END
        SERIES
            CONTACTS RModoAuto 0 0
            OSF
            CONTACTS YPrsEmbreagem 1 0
        END
    END
    CONTACTS RMaqProduzindo 0 0
    COIL RMaqProduzindo 0 0 1 0
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
    CONTACTS XDbgLimpaErro 0 0
    OSR
    COIL YMbLimpaErro 0 1 0 2
END
RUNG
    CONTACTS YMbLimpaErro 0 2
    PARALLEL
        COIL RMaqCfgSrvErro 0 0 1 0
        COIL RMaqErroPosic 0 0 1 0
        COIL YMbLimpaErro 0 0 1 2
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
    CONTACTS RErroBombaHidr 0 0
    COIL YMbErrBombaHidr 0 0 0 2
END
RUNG
    CONTACTS RPressaoArOK 1 0
    COIL YMbErrPressaoAr 0 0 0 4
END
RUNG
    CONTACTS RServoOK 1 0
    COIL YMbErrServo 0 0 0 6
END
RUNG
    CONTACTS RMaqCfgSrvErro 0 0
    COIL YMbErrSrvComunic 0 0 0 8
END
RUNG
    CONTACTS RMaqErroPosic 0 0
    COIL YMbErrPosic 0 0 0 9
END
RUNG
    COMMENT Flags que indicam o estado atual da máquina - Somente Leitura
END
RUNG
    CONTACTS RMaqOK 0 0
    COIL YMbMaqOK 0 0 0 1
END
RUNG
    CONTACTS RMaqPronta 0 0
    COIL YMbMaqPronta 0 0 0 2
END
RUNG
    CONTACTS RMaqCfgSrvOK 0 0
    COIL YMbMaqCfgSrvOK 0 0 0 3
END
RUNG
    CONTACTS RAplanAvancoOK 0 0
    COIL YMbAplanAvancoOK 0 0 0 4
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    COIL YMbMaqProduzindo 0 0 0 7
END
RUNG
    CONTACTS RMaqStartProd 0 0
    COIL YMbMaqStartProd 0 0 0 8
END
RUNG
    CONTACTS RMaqForaPosic 0 0
    COIL YMbMaqForaPosic 0 0 0 9
END
RUNG
    CONTACTS XSrvInPos 0 0
    COIL YMbSrvInPos 0 0 0 13
END
RUNG
    CONTACTS RMaqPosicOK 0 0
    COIL YMbPosicOK 0 0 0 14
END
RUNG
    COMMENT Declaração de variáveis do ModBUS
END
RUNG
    OSR
    PARALLEL
        RESET_ENC ZEncPerfil
        COIL YMbAplanAcelMS 0 0 1 0
        COIL YMbAplanVelAuto 0 0 1 0
        COIL YMbAplanVelMan 0 0 1 0
        COIL YMbEncFator 0 0 1 0
        COIL YMbEncResol 0 0 1 0
        COIL YMbEncPerim 0 0 1 0
        COIL YMbMaqErroPos 0 0 1 0
        COIL YMbPosAtual 0 0 1 0
    END
END
RUNG
    OSR
    PARALLEL
        MOVE AplanVelMax 5934
        MOVE FatorCorrServo 10875
        MOVE YMbAplanVelAuto 100
        MOVE YMbAplanVelMan 30
        MOVE YMbAplanAcelMS 100
        MOVE YMbEncFator 10000
        MOVE YMbEncResol 2500
        MOVE YMbEncPerim 400
    END
END
RUNG
    OSR
    PARALLEL
        COIL YMbPassos 0 0 1 0
        COIL YMbPortasLow 0 0 1 0
        COIL YMbPortasHigh 0 0 1 0
        COIL YMbPasso1 0 0 1 0
        COIL YMbPasso2 0 0 1 0
        COIL YMbPasso3 0 0 1 0
        COIL YMbPasso4 0 0 1 0
        COIL YMbPasso5 0 0 1 0
        COIL YMbPasso6 0 0 1 0
        COIL YMbPasso1Rep 0 0 1 0
        COIL YMbPasso2Rep 0 0 1 0
        COIL YMbPasso3Rep 0 0 1 0
        COIL YMbPasso4Rep 0 0 1 0
        COIL YMbPasso5Rep 0 0 1 0
        COIL YMbPasso6Rep 0 0 1 0
    END
END
RUNG
    OSR
    PARALLEL
        MOVE YMbPassos 3
        MOVE YMbPortasLow 14467
        MOVE YMbPortasHigh 4161
        MOVE YMbPasso1 7000
        MOVE YMbPasso2 16000
        MOVE YMbPasso3 12000
        MOVE YMbPasso4 0
        MOVE YMbPasso5 0
        MOVE YMbPasso6 0
        MOVE YMbPasso1Rep 2
        MOVE YMbPasso2Rep 1
        MOVE YMbPasso3Rep 5
        MOVE YMbPasso4Rep 0
        MOVE YMbPasso5Rep 0
        MOVE YMbPasso6Rep 0
    END
END
RUNG
    COMMENT Calculando variáveis conforme os valores recebidos pelo ModBUS
END
RUNG
    COMMENT Cálculo de variáveis relacionadas com o programa da máquina
END
RUNG
    CONTACTS RMaqProduzindo 1 0
    PARALLEL
        MOVE PassosQtd YMbAplanPassos
        SERIES
            GRT PassosQtd 6
            MOVE PassosQtd 6
        END
    END
END
RUNG
    CONTACTS RMaqProduzindo 1 0
    PARALLEL
        MOVE Passo1 YMbPasso1
        MOVE Passo2 YMbPasso2
        MOVE Passo3 YMbPasso3
        MOVE Passo4 YMbPasso4
        MOVE Passo5 YMbPasso5
        MOVE Passo6 YMbPasso6
        MOVE Passo1Rep YMbPasso1Rep
        MOVE Passo2Rep YMbPasso2Rep
        MOVE Passo3Rep YMbPasso3Rep
        MOVE Passo4Rep YMbPasso4Rep
        MOVE Passo5Rep YMbPasso5Rep
        MOVE Passo6Rep YMbPasso6Rep
    END
END
RUNG
    CONTACTS RMaqProduzindo 1 0
    PARALLEL
        MOVE PortasLow YMbPortasLow
        MOVE PortasHigh YMbPortasHigh
    END
END
RUNG
    COMMENT Calculando parâmetros de velocidade para enviar ao servo
END
RUNG
    MUL AplanVelAuto AplanVelMax YMbAplanVelAuto
END
RUNG
    DIV AplanVelAuto AplanVelAuto 100
END
RUNG
    MUL AplanVelManual AplanVelMax YMbAplanVelMan
END
RUNG
    DIV AplanVelManual AplanVelManual 100
END
RUNG
    GRT YMbAplanAcelMS 0
    DIV AplanRampa AplanVelMax YMbAplanAcelMS
END
RUNG
    EQU AplanRampa 0
    MOVE AplanRampa 1
END
RUNG
    MOVE AplanPasso PassoMaior
END
RUNG
    MUL AplanPassoCorr AplanPasso FatorCorrServo
END
RUNG
    DIV AplanPassoCorr AplanPassoCorr 10000
END
