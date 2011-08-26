POPTools:1.2
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=19200
PARITY=4
COM=4
IP=192.168.0.235
MASK=255.255.255.0
GW=192.168.0.10
COMPILED=N:\windows\Maquinas\PrensaPP\AplanadoraPrensaPP.hex

IO LIST
    EMaqPosAtual at 1 0
    XAplanExtRecuada at 7 0
    XAplanFechada at 6 0
    XAplanManAvanca at 9 0
    XAplanManRecua at 10 0
    XBombaHidrLigada at 13 0
    XDbgDeslReadEnc at 23 1
    XDbgLimpaErro at 23 7
    XDbgMaqLib at 23 0
    XDbgParamSync at 23 6
    XDbgPrsLigaMotor at 23 3
    XDbgSimulaEnc at 23 2
    XDbgSimulaEncNeg at 23 8
    XDbgValErro0 at 23 4
    XDbgValErro48 at 23 5
    XDesbobOK at 8 0
    XEmergencia at 1 0
    XFaltaFase at 4 0
    XInversorOK at 5 0
    XMbModoAuto at 22 0
    XPressaoArOK at 18 0
    XPrsAvancoOK at 11 0
    XPrsCameParar at 12 0
    XPrsCmdAjuste at 14 0
    XPrsCmdBM1 at 15 0
    XPrsCmdBM2 at 16 0
    XPrsDeslContinuo at 17 0
    XSrvInPos at 19 0
    XTermBombaHidr at 2 0
    XTermMartelo at 3 0
    YAplanAvancaMesa at 6 0
    YAplanExtDesce at 5 0
    YAplanExtSobe at 4 0
    YAplanManAvcIHM at 24 0
    YAplanManRecIHM at 24 1
    YAplanRecuaMesa at 7 0
    YHidrAbre at 2 0
    YHidrDesce at 9 0
    YHidrFecha at 3 0
    YHidrSobe at 8 0
    YHidrVentagem at 10 0
    YInvLimpaErro at 15 0
    YLedErro at 17 0
    YLedUser at 18 0
    YLigarBombaHidr at 1 0
    YMbAplanAberta at 21 12
    YMbAplanAbre at 24 2
    YMbAplanAcelMS at 31 0
    YMbAplanAvancoOK at 21 4
    YMbAplanDesce at 24 5
    YMbAplanExpandir at 24 7
    YMbAplanExtAvanc at 21 10
    YMbAplanExtCima at 21 11
    YMbAplanExtSubir at 24 6
    YMbAplanFecha at 24 3
    YMbAplanPasso at 30 0
    YMbAplanSobe at 24 4
    YMbAplanVelAuto at 32 0
    YMbAplanVelMan at 33 0
    YMbEncFator at 40 0
    YMbEncPerim at 42 0
    YMbEncResol at 41 0
    YMbErrBombaHidr at 20 2
    YMbErrDesbob at 20 7
    YMbErrEmergencia at 20 0
    YMbErrFaltaFase at 20 1
    YMbErrInversor at 20 5
    YMbErrMartelo at 20 3
    YMbErrPosic at 20 9
    YMbErrPressaoAr at 20 4
    YMbErrServo at 20 6
    YMbErrSrvComunic at 20 8
    YMbLimpaErro at 22 2
    YMbMaqCfgSrvOK at 21 3
    YMbMaqErroPos at 34 0
    YMbMaqForaPosic at 21 9
    YMbMaqLib at 22 1
    YMbMaqOK at 21 1
    YMbMaqProduzindo at 21 7
    YMbMaqPronta at 21 2
    YMbMaqStartProd at 21 8
    YMbModoAutoAtivo at 21 0
    YMbNovoCiclosMil at 38 0
    YMbNovoCiclosUnd at 37 0
    YMbParamSync at 22 3
    YMbPosAtual at 25 0
    YMbPosicOK at 21 14
    YMbPrsCiclosMil at 36 0
    YMbPrsCiclosNovo at 22 5
    YMbPrsCiclosUnid at 35 0
    YMbPrsIniciar at 24 9
    YMbPrsLigado at 21 6
    YMbPrsLigaMotor at 24 8
    YMbPrsParado at 21 5
    YMbPrsParar at 24 10
    YMbPrsSentidoInv at 22 4
    YMbSrvInPos at 21 13
    YPrsAjMartelo at 12 0
    YPrsEmbreagem at 11 0
    YPrsLigaInv at 13 0
    YPrsLigaNormal at 14 0
    YSrvHabilita at 16 0
    ZEncPerfil at 1 0
END

PROGRAM
RUNG
    COMMENT Programa de Controle da Aplanadora e Prensa da Linha PP (Porta-Palete) - v1.0\r\nDesenvolvido por Marcelo Utikawa da Fonseca em 07/02/2011
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
    COIL RPrsChecarSrvOK 0 0 0 0
END
RUNG
    CONTACTS RPrsChecarSrvOK 0 0
    PARALLEL
        SERIES
            CONTACTS RMaqSerialBusy 1 0
            TON TEspChecarON 1000000
            TOF TEspChecarOFF 1000000
            PARALLEL
                COIL RPrsPausaSrvOK 0 0 0 0
                COIL RSerialReservada 0 0 1 0
            END
        END
        SERIES
            PARALLEL
                CONTACTS RMaqSerialBusy 1 0
                CONTACTS RSerialReservada 0 0
            END
            PARALLEL
                SERIES
                    OSR
                    PARALLEL
                        COIL RSerialReservada 0 1 0 0
                        COIL RMaqSerialBusy 0 1 0 0
                    END
                END
                SERIES
                    WRITE_SERVO_YASKAWA 0 (empty) ERR
                    TON TEspRespChecar 50000
                    READ_SERVO_YASKAWA 0 PrsValErro ERR NONE
                    PARALLEL
                        COIL RServoOK 0 0 1 0
                        SERIES
                            OSR
                            COIL RMaqSerialBusy 0 0 1 0
                        END
                        SERIES
                            EQU PrsValErro 0
                            COIL RServoOK 0 1 0 0
                        END
                    END
                END
            END
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
    CONTACTS XPressaoArOK 0 0
    TOF TEspPressaoAr 500000
    COIL RPressaoArOK 0 0 0 0
END
RUNG
    CONTACTS RErroGeral 1 0
    CONTACTS XInversorOK 0 0
    CONTACTS RErroBombaHidr 1 0
    CONTACTS XTermMartelo 0 0
    CONTACTS RPressaoArOK 0 0
    PARALLEL
        CONTACTS XDesbobOK 0 0
        SHORT
    END
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
        COIL YPrsEmbreagem 0 0 1 0
    END
END
RUNG
    CONTACTS RMaqCfgSrvErro 0 0
    OSR
    COIL RMaqDesabSrv 0 1 0 0
END
RUNG
    PARALLEL
        CONTACTS RMaqOK 1 0
        CONTACTS RMaqCfgSrvErro 0 0
    END
    COIL YLedErro 0 0 0 0
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
    CONTACTS RMaqManualOK 0 0
    CONTACTS YMbAplanAbre 0 2
    COIL RAplanAbre 0 0 0 0
END
RUNG
    CONTACTS RMaqManualOK 0 0
    CONTACTS YMbAplanFecha 0 3
    COIL RAplanFecha 0 0 0 0
END
RUNG
    CONTACTS RMaqManualOK 0 0
    CONTACTS YMbAplanSobe 0 4
    COIL RAplanSobe 0 0 0 0
END
RUNG
    CONTACTS RMaqManualOK 0 0
    CONTACTS YMbAplanDesce 0 5
    COIL RAplanDesce 0 0 0 0
END
RUNG
    CONTACTS YMbAplanExtSubir 0 6
    PARALLEL
        SERIES
            OSR
            CONTACTS RMaqManualOK 0 0
            COIL RAplanExtSubir 0 1 0 0
        END
        SERIES
            OSF
            CONTACTS RMaqManualOK 0 0
            COIL RAplanExtSubir 0 0 1 0
        END
    END
END
RUNG
    CONTACTS YMbAplanExpandir 0 7
    PARALLEL
        SERIES
            OSR
            CONTACTS RMaqManualOK 0 0
            COIL RAplanExpandir 0 1 0 0
        END
        SERIES
            OSF
            CONTACTS RMaqManualOK 0 0
            COIL RAplanExpandir 0 0 1 0
        END
    END
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
        MOVE EncPosZero 0
    END
END
RUNG
    CONTACTS RStartZerarEnc 0 0
    OSR
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqForaPosic 1 0
    MOVE EncPosZero DifPosic
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
    COMMENT Lógica de controle da Aplanadora - Controle da Bomba Hidráulica
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS YLigarBombaHidr 1 0
    PARALLEL
        CONTACTS RAplanAbre 0 0
        CONTACTS RAplanFecha 0 0
        CONTACTS RAplanSobe 0 0
        CONTACTS RAplanDesce 0 0
    END
    COIL YLigarBombaHidr 0 1 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS YLigarBombaHidr 0 0
            CONTACTS XBombaHidrLigada 1 0
            TON TBombaHidrTO 5000000
        END
        CONTACTS XTermBombaHidr 1 0
    END
    COIL RErroBombaHidr 0 0 0 0
END
RUNG
    CONTACTS YLigarBombaHidr 0 0
    TON TBombaHidrEstab 3000000
    COIL RBombaHidrOK 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RBombaHidrOK 0 0
            CONTACTS RAplanSobe 1 0
            CONTACTS RAplanDesce 1 0
            CONTACTS RAplanAbre 1 0
            CONTACTS RAplanFecha 1 0
            TON TBombaHidrInat 300000000
        END
        CONTACTS RMaqOK 1 0
    END
    COIL YLigarBombaHidr 0 0 1 0
END
RUNG
    COMMENT Lógica de controle do corte do perfil - Mapeamento entre relés internos para entradas e saídas
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RAplanAbre 0 0
    CONTACTS YHidrFecha 1 0
    COIL YHidrAbre 0 0 0 0
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RAplanFecha 0 0
    PARALLEL
        SERIES
            CONTACTS RAplanFechada 1 0
            COIL YHidrFecha 0 0 0 0
        END
        SERIES
            CONTACTS RAplanFechada 0 0
            COIL YMbAplanFecha 0 0 1 3
        END
    END
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RAplanSobe 0 0
    CONTACTS YHidrDesce 1 0
    COIL YHidrSobe 0 0 0 0
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RAplanDesce 0 0
    COIL YHidrDesce 0 0 0 0
END
RUNG
    PARALLEL
        CONTACTS YHidrAbre 0 0
        CONTACTS YHidrFecha 0 0
        CONTACTS YHidrDesce 0 0
        CONTACTS YHidrSobe 0 0
    END
    COIL YHidrVentagem 0 0 0 0
END
RUNG
    CONTACTS RAplanExtSubir 0 0
    PARALLEL
        SERIES
            OSR
            PARALLEL
                COIL YAplanExtSobe 0 1 0 0
                COIL YAplanExtDesce 0 0 1 0
            END
        END
        SERIES
            OSF
            PARALLEL
                COIL YAplanExtSobe 0 0 1 0
                COIL YAplanExtDesce 0 1 0 0
            END
        END
    END
END
RUNG
    PARALLEL
        CONTACTS YAplanExtSobe 0 0
        CONTACTS YAplanExtDesce 0 0
    END
    TON TEspAtuarExt 1000000
    PARALLEL
        COIL YAplanExtSobe 0 0 1 0
        COIL YAplanExtDesce 0 0 1 0
    END
END
RUNG
    CONTACTS RAplanExpandir 0 0
    PARALLEL
        SERIES
            OSR
            PARALLEL
                COIL YAplanAvancaMesa 0 1 0 0
                COIL YAplanRecuaMesa 0 0 1 0
            END
        END
        SERIES
            OSF
            PARALLEL
                COIL YAplanAvancaMesa 0 0 1 0
                COIL YAplanRecuaMesa 0 1 0 0
            END
        END
    END
END
RUNG
    CONTACTS XAplanExtRecuada 0 0
    PARALLEL
        OSR
        OSF
    END
    PARALLEL
        COIL YAplanAvancaMesa 0 0 1 0
        COIL YAplanRecuaMesa 0 0 1 0
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
                        SERIES
                            TON TEspEntreJogs 500000
                            COIL RStartCfgSrvJog 0 0 1 0
                        END
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
    COMMENT *** Lógica de Controle - Prensa ***
END
RUNG
    COMMENT Tratamento de entradas do Came Eletrônico
END
RUNG
    CONTACTS XPrsCameParar 0 0
    TON TEspCameON 50000
    TOF TEspCameOFF 50000
    COIL RPrsCameParar 0 0 0 0
END
RUNG
    CONTACTS XPrsAvancoOK 0 0
    TON TEspAvancoON 50000
    TOF TEspAvancoOFF 50000
    COIL RPrsAvancoOK 0 0 0 0
END
RUNG
    COMMENT Lógica que controla o motor da prensa
END
RUNG
    CONTACTS XDbgPrsLigaMotor 0 3
    OSR
    COIL YMbPrsLigaMotor 0 1 0 8
END
RUNG
    PARALLEL
        CONTACTS RErroGeral 0 0
        CONTACTS XInversorOK 1 0
    END
    COIL YMbPrsLigaMotor 0 0 1 8
END
RUNG
    CONTACTS RPrsMotorLigado 1 0
    TON TEspMotorParar 120000000
    COIL RPrsMotorParado 0 0 0 0
END
RUNG
    CONTACTS YMbPrsSentidoInv 0 4
    PARALLEL
        OSR
        OSF
    END
    COIL RPrsNovoSentido 0 1 0 0
END
RUNG
    PARALLEL
        CONTACTS RMaqPronta 0 0
        CONTACTS RPrsMotorLigado 0 0
    END
    CONTACTS YMbPrsLigaMotor 0 8
    PARALLEL
        CONTACTS RPrsMotorParado 0 0
        CONTACTS RPrsNovoSentido 1 0
    END
    PARALLEL
        SERIES
            CONTACTS YMbPrsSentidoInv 1 4
            COIL YPrsLigaNormal 0 0 0 0
        END
        SERIES
            CONTACTS YMbPrsSentidoInv 0 4
            COIL YPrsLigaInv 0 0 0 0
        END
        COIL RPrsNovoSentido 0 0 1 0
    END
END
RUNG
    PARALLEL
        CONTACTS YPrsLigaNormal 0 0
        CONTACTS YPrsLigaInv 0 0
    END
    PARALLEL
        COIL RPrsMotorLigado 0 0 0 0
        SERIES
            OSR
            PARALLEL
                COIL YMbPrsIniciar 0 0 1 9
                COIL RPrsIniciarCiclo 0 0 1 0
            END
        END
    END
END
RUNG
    CONTACTS RPrsMotorLigado 0 0
    TON TEspPrsMotorOK 15000000
    COIL RPrsMotorOK 0 0 0 0
END
RUNG
    COMMENT Controle da Embreagem
END
RUNG
    CONTACTS RMaqPronta 0 0
    CONTACTS RPrsMotorOK 0 0
    PARALLEL
        SERIES
            CONTACTS YPrsLigaNormal 0 0
            CONTACTS RPrsIniciarCiclo 0 0
        END
        SERIES
            CONTACTS XPrsCmdAjuste 0 0
            CONTACTS RMaqProduzindo 1 0
        END
    END
    PARALLEL
        COIL YMbPrsParar 0 0 1 10
        COIL RPrsIniciarCiclo 0 0 1 0
        SERIES
            CONTACTS XPrsDeslContinuo 0 0
            CONTACTS RBloquearPrensa 1 0
            PARALLEL
                SERIES
                    CONTACTS RMaqProduzindo 1 0
                    COIL RPrsPararMartelo 0 1 0 0
                END
                COIL YPrsEmbreagem 0 1 0 0
            END
        END
    END
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RPrsCameParar 0 0
            OSR
            PARALLEL
                CONTACTS RPrsPararMartelo 0 0
                SERIES
                    CONTACTS RAplanLigarMotor 0 0
                    CONTACTS YSrvHabilita 1 0
                END
            END
            PARALLEL
                CONTACTS XPrsCmdAjuste 1 0
                CONTACTS RMaqProduzindo 0 0
            END
        END
        SERIES
            CONTACTS XPrsCmdAjuste 0 0
            OSF
            CONTACTS RMaqProduzindo 1 0
        END
    END
    PARALLEL
        COIL YPrsEmbreagem 0 0 1 0
        COIL RPrsPararMartelo 0 0 1 0
        COIL RPrsIniciarCiclo 0 0 1 0
    END
END
RUNG
    CONTACTS RPrsMotorLigado 0 0
    OSF
    COIL YPrsEmbreagem 0 0 1 0
END
RUNG
    COMMENT Controle de número de ciclos da prensa
END
RUNG
    CONTACTS YMbPrsCiclosNovo 0 5
    PARALLEL
        COIL YMbPrsCiclosNovo 0 0 1 5
        MUL PrsCiclos YMbNovoCiclosMil 1000
        ADD PrsCiclos PrsCiclos YMbNovoCiclosUnd
    END
END
RUNG
    CONTACTS RPrsCameParar 0 0
    OSR
    ADD PrsCiclos PrsCiclos 1
END
RUNG
    DIV YMbPrsCiclosMil PrsCiclos 1000
END
RUNG
    MUL PrsCiclosMil YMbPrsCiclosMil 1000
END
RUNG
    SUB YMbPrsCiclosUnid PrsCiclos PrsCiclosMil
END
RUNG
    COMMENT Controle do Bi-Manual
END
RUNG
    CONTACTS XPrsCmdBM1 0 0
    TOF TEspCmdBM1 500000
    PARALLEL
        SERIES
            CONTACTS RPrsCmdBM1Blq 1 0
            COIL RPrsCmdBM1OK 0 0 0 0
        END
        SERIES
            TON TTOCmdBM1 500000
            COIL RPrsCmdBM1Blq 0 0 0 0
        END
    END
END
RUNG
    CONTACTS XPrsCmdBM2 0 0
    TOF TEspCmdBM2 500000
    PARALLEL
        SERIES
            CONTACTS RPrsCmdBM2Blq 1 0
            COIL RPrsCmdBM2OK 0 0 0 0
        END
        SERIES
            TON TTOCmdBM2 500000
            COIL RPrsCmdBM2Blq 0 0 0 0
        END
    END
END
RUNG
    CONTACTS RPrsCmdBM1OK 0 0
    CONTACTS RPrsCmdBM2OK 0 0
    CONTACTS XPrsDeslContinuo 0 0
    COIL RPrsCmdBMOK 0 0 0 0
END
RUNG
    COMMENT Lógica para iniciar prensa
END
RUNG
    CONTACTS RMaqPronta 0 0
    CONTACTS RPrsMotorOK 0 0
    CONTACTS YPrsLigaNormal 0 0
    PARALLEL
        CONTACTS YMbPrsIniciar 0 9
        CONTACTS RPrsCmdBMOK 0 0
    END
    PARALLEL
        COIL RPrsIniciarCiclo 0 1 0 0
        COIL YMbPrsIniciar 0 0 1 9
        COIL RBloquearPrensa 0 0 1 0
    END
END
RUNG
    COMMENT Lógica para parar a prensa
END
RUNG
    CONTACTS YPrsEmbreagem 0 0
    PARALLEL
        CONTACTS XPrsDeslContinuo 1 0
        CONTACTS YMbPrsParar 0 10
    END
    OSR
    PARALLEL
        COIL RPrsPararMartelo 0 1 0 0
        COIL YMbPrsParar 0 0 1 10
    END
END
RUNG
    CONTACTS XPrsDeslContinuo 0 0
    OSF
    COIL RBloquearPrensa 0 1 0 0
END
RUNG
    COMMENT Lógica que libera o ajuste de martelo
END
RUNG
    CONTACTS YMbModoAutoAtivo 1 0
    CONTACTS RMaqOK 0 0
    CONTACTS YPrsEmbreagem 1 0
    COIL YPrsAjMartelo 0 0 0 0
END
RUNG
    COMMENT *** Fim da Lógica - Prensa ***
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
    COMMENT Lógica de operação automática da perfiladeira - Checagens Gerais
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RModoAuto 0 0
    CONTACTS YMbPrsSentidoInv 1 4
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
        COIL RPrsPararMartelo 0 1 0 0
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
    CONTACTS YMbPrsSentidoInv 1 4
    PARALLEL
        COIL RMaqStartProd 0 1 0 0
        COIL YMbAplanExtSubir 0 0 1 6
        COIL YMbAplanExpandir 0 0 1 7
        COIL RAplanZerarEnc 0 1 0 0
    END
END
RUNG
    CONTACTS RMaqStartProd 0 0
    CONTACTS RPrsMotorLigado 1 0
    COIL YMbPrsLigaMotor 0 1 0 8
END
RUNG
    CONTACTS RMaqStartProd 0 0
    CONTACTS XAplanFechada 1 0
    COIL YMbAplanFecha 0 1 0 3
END
RUNG
    CONTACTS RMaqStartProd 0 0
    TON TEspExtRepouso 3000000
    CONTACTS RPrsMotorOK 0 0
    CONTACTS XAplanFechada 0 0
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
                COIL RPrsIniciarCiclo 0 1 0 0
                COIL RMaqInicioAuto 0 1 0 0
                COIL RBloquearPrensa 0 0 1 0
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
                        COIL RPrsPararMartelo 0 1 0 0
                        DIV YMbMaqErroPos DifPosic 10
                    END
                END
                SERIES
                    CONTACTS RMaqErroPosic 1 0
                    COIL RAplanZerarEnc 0 1 0 0
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
            CONTACTS RMaqPosicOK 0 0
            CONTACTS YPrsEmbreagem 1 0
            COIL RPrsIniciarCiclo 0 1 0 0
        END
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
            TOF TEspSrvParar 300000
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
        END
        SERIES
            CONTACTS RModoAuto 0 0
            OSF
            CONTACTS YPrsEmbreagem 1 0
        END
    END
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RModoAuto 1 0
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
        SERIES
            TOF TEspInvLimpaErr 100000
            COIL YInvLimpaErro 0 0 0 0
        END
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
    CONTACTS XTermMartelo 1 0
    COIL YMbErrMartelo 0 0 0 3
END
RUNG
    CONTACTS RPressaoArOK 1 0
    COIL YMbErrPressaoAr 0 0 0 4
END
RUNG
    CONTACTS XInversorOK 1 0
    COIL YMbErrInversor 0 0 0 5
END
RUNG
    CONTACTS RServoOK 1 0
    COIL YMbErrServo 0 0 0 6
END
RUNG
    CONTACTS XDesbobOK 1 0
    OPEN
    COIL YMbErrDesbob 0 0 0 7
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
    CONTACTS RPrsMotorParado 0 0
    COIL YMbPrsParado 0 0 0 5
END
RUNG
    CONTACTS RPrsMotorLigado 0 0
    COIL YMbPrsLigado 0 0 0 6
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
    CONTACTS XAplanFechada 1 0
    COIL YMbAplanAberta 0 0 0 12
END
RUNG
    CONTACTS RAplanExtSubir 0 0
    COIL YMbAplanExtCima 0 0 0 11
END
RUNG
    CONTACTS XAplanExtRecuada 1 0
    COIL YMbAplanExtAvanc 0 0 0 10
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
        COIL YMbAplanPasso 0 0 1 0
        COIL YMbEncFator 0 0 1 0
        COIL YMbEncResol 0 0 1 0
        COIL YMbEncPerim 0 0 1 0
        COIL YMbNovoCiclosMil 0 0 1 0
        COIL YMbNovoCiclosUnd 0 0 1 0
        COIL YMbPrsCiclosMil 0 0 1 0
        COIL YMbPrsCiclosUnid 0 0 1 0
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
        MOVE YMbAplanPasso 320
        MOVE YMbEncFator 10000
        MOVE YMbEncResol 2500
        MOVE YMbEncPerim 400
        MOVE PrsCiclos 0
        MOVE YMbNovoCiclosMil 0
        MOVE YMbNovoCiclosUnd 0
    END
END
RUNG
    COMMENT Calculando variáveis conforme os valores recebidos pelo ModBUS
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
    MUL AplanPasso YMbAplanPasso 100
END
RUNG
    MUL AplanPassoCorr AplanPasso FatorCorrServo
END
RUNG
    DIV AplanPassoCorr AplanPassoCorr 10000
END
