LDmicro0.2
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
    XCortePerfManual at 7 0
    XDbgCalcCorrecao at 23 9
    XDbgCalcRelacao at 23 10
    XDbgCalcTamMin at 23 11
    XDbgCorteEstat at 23 7
    XDbgDeslReadEnc at 23 1
    XDbgMaqLib at 23 0
    XDbgParamSync at 23 6
    XDbgSimulaEnc at 23 2
    XDbgSrvInitOK at 23 8
    XDbgSrvPosMan at 23 4
    XDbgSrvPosicOK at 23 5
    XDbgStartPosic at 23 3
    XEmergencia at 1 0
    XFaltaFase at 3 0
    XInversorOK at 4 0
    XMbModoAuto at 22 0
    XMesaManAvanca at 8 0
    XMesaManRecua at 9 0
    XPerfManAvanca at 5 0
    XPerfManRecua at 6 0
    XServoOK at 15 0
    XServoProxFim at 16 0
    XSnsFimMaterial at 18 0
    XSnsInfHidr at 11 0
    XSnsInfPiloto at 13 0
    XSnsSupHidr at 10 0
    XSnsSupPiloto at 12 0
    XSnsSupTravaMesa at 14 0
    XSrvMesaSync at 17 0
    XTermBombaHidr at 2 0
    YAvancarPerf at 11 0
    YCorteTravaMesa at 3 0
    YHidrDesce at 4 0
    YHidrSobe at 5 0
    YHidrVentagem at 13 0
    YInvFreioMotor at 6 0
    YLedErro at 17 0
    YLedUser at 18 0
    YLigarBombaHidr at 1 0
    YMbAutoAcel at 32 0
    YMbAutoDesac at 33 0
    YMbCalcCorrecao at 22 11
    YMbCalcRelacao at 22 12
    YMbCalcTamMin at 22 13
    YMbCorteAtivo at 21 3
    YMbCorteIHM at 22 1
    YMbCrtEstatAtivo at 21 4
    YMbDbgAcionouSns at 21 15
    YMbDbgRelPerf at 28 0
    YMbDbgRelSrv at 29 0
    YMbDinamVel at 34 0
    YMbEncFator at 40 0
    YMbEncMesa at 24 0
    YMbEncPerim at 42 0
    YMbEncResol at 41 0
    YMbErrBombaHidr at 20 2
    YMbErrCorte at 20 8
    YMbErrEmergencia at 20 0
    YMbErrFaltaFase at 20 1
    YMbErrInvComunic at 20 5
    YMbErrInversor at 20 3
    YMbErrPosic at 20 7
    YMbErrServo at 20 4
    YMbErrSrvComunic at 20 6
    YMbEstatVel at 46 0
    YMbFerramCrtCima at 21 7
    YMbInvParamSync at 22 5
    YMbLimpaErro at 22 2
    YMbManAcel at 35 0
    YMbManDesac at 36 0
    YMbManVel at 37 0
    YMbMaqCfgInvOK at 21 10
    YMbMaqCfgSrvOK at 21 11
    YMbMaqCortando at 21 13
    YMbMaqCortePerf at 21 14
    YMbMaqDesativada at 20 9
    YMbMaqIniciando at 21 5
    YMbMaqInitOK at 21 6
    YMbMaqLib at 22 3
    YMbMaqModoEstat at 21 12
    YMbMaqOperando at 21 9
    YMbMaqProduzindo at 21 2
    YMbMaqPronta at 21 8
    YMbMesaCurso at 30 0
    YMbModoAutoAtivo at 21 0
    YMbNovoFator at 47 0
    YMbNovoTamMin at 43 0
    YMbOffsetCorte at 31 0
    YMbParamSync at 22 4
    YMbQtd at 50 0
    YMbSrvPosMesa at 39 0
    YMbSrvPosMesaMan at 22 6
    YMbSrvPosicAtivo at 21 1
    YMbSrvVelJog at 44 0
    YMbSrvVelPos at 45 0
    YMbTam at 51 0
    YMbTamMin at 38 0
    YMesaManAvcIHM at 22 9
    YMesaManRecuaIHM at 22 10
    YPerfManAvcIHM at 22 7
    YPerfManRecuaIHM at 22 8
    YPilotoDescer at 2 0
    YRecuarPerf at 12 0
    YSrvEscravoAtivo at 7 0
    YSrvHabilita at 8 0
    EMaqPosAtual at 1 0
    ZEncResetVal at 1 0
END

PROGRAM
RUNG
    COMMENT Programa de Controle da Perfiladeira de Perf - v1.0\r\nDesenvolvido por Marcelo Utikawa da Fonseca em 23/07/2010
END
RUNG
    COMMENT Descrição das flags utilizadas no programa:
END
RUNG
    COMMENT AvancarMesaMan\r\n   Flag que indica que o comando para avanço manual da mesa de corte está ativado
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
    COMMENT CalcCorrAtivo\r\n   Flag indicando que a lógica de cálculo de fator de correção do perfil está ativa
END
RUNG
    COMMENT CalcCorrPosic\r\n   Flag indicando que a mesa terminou o posicionamento ao fim da mesa durante o cálculo de fator de correção do perfil
END
RUNG
    COMMENT CalcCorrZero\r\n   Flag indicando que a mesa retornou à posição inicial após o cálculo do fator de encoder
END
RUNG
    COMMENT CalcRelAtivo\r\n   Flag indicando que a lógica de cálculo de relação entre os encoders está ativa
END
RUNG
    COMMENT CalcRelE1Avanca\r\n   Ativo quando avançando o perfil durante a Etapa 1 do cálculo de relação entre os encoders (relação entre encoders do perfil)
END
RUNG
    COMMENT CalcRelE1Fim\r\n   Ativo quando termina o avanço do perfil durante a Etapa 1 do cálculo de relação entre os encoders (relação entre encoders do perfil)
END
RUNG
    COMMENT CalcRelE1OK\r\n   Ativo quando a Etapa 1 do cálculo de relação entre os encoders (relação entre encoders do perfil) estiver finalizada
END
RUNG
    COMMENT CalcRelE1Start\r\n   Ativo quando iniciando a Etapa 1 do cálculo de relação entre os encoders (relação entre encoders do perfil)
END
RUNG
    COMMENT CalcRelE2Fim\r\n   Ativo quando finalizando a Etapa 2 do cálculo de relação entre os encoders (relação entre encoder da mesa e do servo)
END
RUNG
    COMMENT CalcRelE2OK\r\n   Ativo quando terminada a Etapa 2 do cálculo de relação entre os encoders (relação entre encoder da mesa e do servo), recuando a mesa
END
RUNG
    COMMENT CalcRelE2start\r\n   Ativo quando iniciando a Etapa 2 do cálculo de relação entre os encoders (relação entre encoder da mesa e do servo)
END
RUNG
    COMMENT CalcRelZero\r\n   Ativo após o posicionamento inicial da mesa na posição zero, iniciando então a Etapa 1 do cálculo de relação entre os encoders (relação entre encoders do perfil)
END
RUNG
    COMMENT CalcTamMinAtivo\r\n   Flag indicando que a lógica de cálculo de tamanho mínimo da peça está ativa
END
RUNG
    COMMENT CalcTamMinPosic\r\n   Ativo após o posicionamento da mesa no final do curso
END
RUNG
    COMMENT CalcTamMinZero\r\n   Ativo após o posicionamento inicial da mesa na posição zero, iniciando a lógica de cálculo do tamanho mínimo da peça
END
RUNG
    COMMENT CalcVarPerfil\r\n   Flag ativa 1 ciclo a cada 100 ms, quando é medida a variação do perfil desde o ciclo anterior e permitindo desta forma identificar se o perfil está parado ou em movimento
END
RUNG
    COMMENT CortarPerf\r\n   Indica que deve ser realizado um ciclo de corte do perfil. Flag geral, independente da origem da solicitação (manual, automática, IHM)
END
RUNG
    COMMENT CorteEstatAtivo\r\n   Flag indicando que a lógica de corte estático do perfil está ativo
END
RUNG
    COMMENT CorteEstatPosic\r\n   Ativo quando a mesa está na posição de corte durante a lógica de corte estático
END
RUNG
    COMMENT CorteEstatRetOK\r\n   Ativo após a mesa retornar à posição inicial durante o corte estático
END
RUNG
    COMMENT CorteEstatZero\r\n   Ativo quando a mesa finalizar o posicionamento na posição inicial do curso, permitindo assim iniciar a lógica de corte estático
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
    COMMENT MaqCalcCorrecao\r\n   Flag que solicita a execução da lógica de cálculo do fator de correção do perfil
END
RUNG
    COMMENT MaqCalcRelacao\r\n   Flag que solicita a execução da lógica de cálculo da relação entre encoders
END
RUNG
    COMMENT MaqCalcTamMin\r\n   Flag que solicita a execução da lógica de cálculo do tamanho mínimo da peça
END
RUNG
    COMMENT MaqCfgInvErro\r\n   Indica que houve erro na configuração de parâmetros no inversor devido a falha de comunicação
END
RUNG
    COMMENT MaqCfgInvOK\r\n   Indica que o inversor está configurado e portanto pode ser ativado
END
RUNG
    COMMENT MaqCfgSrvErro\r\n   Indica que houve erro na configuração de parâmetros no servo devido a falha de comunicação
END
RUNG
    COMMENT MaqCfgSrvOK\r\n   Indica que o servo está configurado e portanto pode ser ativado
END
RUNG
    COMMENT MaqCfgSrvParOK\r\n   Indica que os parâmetros do servo foram transmitidos com sucesso
END
RUNG
    COMMENT MaqCfgInvOK\r\n   Indica que a máquina foi configurada e pode iniciar a produção automática. Ativo apenas após a transmissão de todos os parâmetros ao inversor
END
RUNG
    COMMENT MaqConfigOK\r\n   Indica que tanto o inversor como o servo estão configurados
END
RUNG
    COMMENT MaqCortando\r\n   Flag utilizada no modo estático para indicar que está ocorrendo uma operação de corte
END
RUNG
    COMMENT MaqCorteEstat\r\n   Flag que solicita a execução da lógica de corte estático do perfil
END
RUNG
    COMMENT MaqCortePerf\r\n   Flag que solicita o corte do perfil. Flag gerada pela lógica, não é gerada pelo comando de corte manual
END
RUNG
    COMMENT MaqErroPosic\r\n   Flag que ativa quando é solicitado à mesa que se posicione em uma posição impossível como uma posição negativa ou além do final de seu curso
END
RUNG
    COMMENT MaqFimCorte\r\n   Flag usada no modo estático e que indica que já foram cortadas todas as peças possíveis, permitindo assim que a mesa retorne à posição inicial para que a perfiladeira seja ativada
END
RUNG
    COMMENT MaqFimProd\r\n   Indica que terminou a produção atual, iniciando assim o processo de finalização da produção como parar o perfil, recuar a mesa, etc.
END
RUNG
    COMMENT MaqInit\r\n   Ativa enquanto a mesa está buscando a posição inicial durante a inicialização
END
RUNG
    COMMENT MaqInitCorte\r\n   Ativo enquanto espera inicializar a ferramenta de corte, posicionando a ferramenta na posição superior e recuando o piloto
END
RUNG
    COMMENT MaqInitFim\r\n   Ativo quando finalizando a inicialização (posicionamento da mesa na posição inicial)
END
RUNG
    COMMENT MaqInitOK\r\n   Flag que indica que a máquina está inicializada e pode ser operada
END
RUNG
    COMMENT MaqInitStart\r\n   Ativo enquanto configurando o servo para começar a inicialização
END
RUNG
    COMMENT MaqModoAjuste\r\n   Flag que ativa o modo de ajuste da máquina, lógica utilizada para corrigir erro do corte na operação automática
END
RUNG
    COMMENT MaqModoCorrecao\r\n   Flag que ativa o modo de correção que é quando o modo de ajuste não consegue corrigir o erro, exigindo assim intervenção do operador da máquina
END
RUNG
    COMMENT MaqModoEstatico\r\n   Indica que a máquina está operando no modo estático, ou seja, não está fazendo o corte em movimento
END
RUNG
    COMMENT MaqOK\r\n   Indica que a máquina está sem erros
END
RUNG
    COMMENT MaqOperando\r\n   Ativo quando a máquina está realizando qualquer operação como corte, posicionamento, etc. Serve para bloquear duas operações de ocorrerem ao mesmo tempo
END
RUNG
    COMMENT MaqPerfilParado\r\n   Ativo quando o perfil estiver parado, calculado com base na variação de posição dos últimos 100 ms
END
RUNG
    COMMENT MaqPosConfig\r\n   Flag do corte estático que indica que a posição para onde a mesa deve ser deslocada já foi configurada e portanto pode ser iniciado o posicionamento
END
RUNG
    COMMENT MaqPosCorteSet\r\n   Flag utilizada no modo estático e indica que a posição de corte foi configurada, permitindo o posicionamento da mesa para o corte
END
RUNG
    COMMENT MaqProduzindo\r\n   Indica que a máquina está no loop de produção. Somente desativa caso ocorrer algum erro, desativar a produção ou terminar a quantidade de peças programada
END
RUNG
    COMMENT MaqPronta\r\n   Indica que a máquina está sem erros, configurada e inicializada, permitindo portanto a operação manual ou automática
END
RUNG
    COMMENT MaqSincronizado\r\n   Ativo quando a mesa está sincronizada com o movimento do perfil, liberando a operação de corte. Desliga depois do fim da peça atual para que não ocorram dois cortes
END
RUNG
    COMMENT MaqStartPosic\r\n   Sinal que dispara a lógica de posicionamento da mesa
END
RUNG
    COMMENT MaqStartProd\r\n   Flag ativa no início da operação automática, quando a máquina é preparada para o início da operação automática
END
RUNG
    COMMENT MaqUsarPiloto\r\n   Ativa quando o piloto deve ser utilizado durante um ciclo de corte
END
RUNG
    COMMENT MaqVelReduzida\r\n   Flag que ativa a velocidade reduzida do perfil, utilizada para as operações de corte voador quando a velocidade do perfil está alta
END
RUNG
    COMMENT ModoAuto\r\n   Flag ativada pelo ModBUS para configurar o modo automático e iniciar o loop de produção
END
RUNG
    COMMENT ModoAutoOK\r\n   Indica que a máquina está pronta para operar no modo automático, ou seja, sem erros e com o modo automático ativado
END
RUNG
    COMMENT ModoDinam\r\n   Ativo quando o servo está configurado para o modo dinâmico, que é o modo em que ele segue o perfil
END
RUNG
    COMMENT PerfManualOK\r\n   Flag que indica que a perfiladeira está pronta para movimentar em modo manual
END
RUNG
    COMMENT PosCorrConfig\r\n   Flag utilizada na lógica de cálculo de fator de correção do encoder e indica que a posição da mesa foi configurada, permitindo o seu posicionamento
END
RUNG
    COMMENT PosEstatConfig\r\n   Flag do modo estático que indica que a posição da mesa foi configurada, permitindo o seu posicionamento
END
RUNG
    COMMENT PosRelConfig\r\n   Flag da lógica de cálculo da relação entre os encoders e indica que a posição da mesa foi configurada, permitindo o seu posicionamento
END
RUNG
    COMMENT PosTamMinConfig\r\n   Flag da lógica de cálculo do tamanho mínimo da peça e indica que a posição da mesa foi configurada, permitindo o seu posicionamento
END
RUNG
    COMMENT RecuarMesaMan\r\n   Ativa quando acionado botao de recuo manual da mesa e a máquina em modo manual
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
    COMMENT SrvCheckFimProd\r\n   Flag utilizada ao terminar a produção, fazendo com que seja lido o bit de estado do servo a cada 500 ms para identificar se ele recuou
END
RUNG
    COMMENT SrvManualOK\r\n   Flag que indica que a mesa está pronta para movimentar em modo manual
END
RUNG
    COMMENT SrvPosicAtivo\r\n   Ativo quando a mesa está realizando uma operação de posicionamento
END
RUNG
    COMMENT SrvReadInitOK\r\n   Flag utilizada durante a inicialização da máquina, fazendo com que seja lido o bit de estado do servo a cada 1s para identificar se ele alcançou a posição inicial
END
RUNG
    COMMENT SrvReadPosicOK\r\n   Flag utilizada na lógica de posicionamento da mesa, fazendo com que seja lido o bit de estado do servo a cada 500 ms para identificar se ele terminou o posicionamento
END
RUNG
    COMMENT SrvStartPosic\r\n   Flag utilizada na lógica de posicionamento da mesa, indicando que deve ser iniciada a operação. Ativo enquanto o servo está sendo configurado
END
RUNG
    COMMENT StartCfgInvAuto\r\n   Solicita a configuração do inversor para o modo automático
END
RUNG
    COMMENT StartCfgInvMan\r\n   Solicita a configuração do inversor para o modo manual
END
RUNG
    COMMENT StartCfgSrvAuto\r\n   Solicita a configuração do servo para o modo automático
END
RUNG
    COMMENT StartCfgSrvJog\r\n   Solicita a configuração do jog do servo devido a solicitação manual de avançar / recuar / parar
END
RUNG
    COMMENT StartCfgSrvMan\r\n   Solicita a configuração do servo para o modo manual
END
RUNG
    COMMENT StartCfgSrvPar\r\n   Solicita a configuração dos parâmetros do servo
END
RUNG
    COMMENT StartCfgVelMax\r\n   Solicita a configuração da velocidade máxima do inversor, saindo do modo de velocidade reduzida durante a operação automática
END
RUNG
    COMMENT StartCfgVelRed\r\n   Solicita a configuração da velocidade reduzida do inversor, entrando no modo de velocidade reduzida durante a operação automática
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
    COMMENT CfgSrvAutoTO\r\n   Timeout na configuração do servo para modo automático
END
RUNG
    COMMENT CfgSrvIniTO\r\n   Timeout na configuração do servo para começar a inicialização
END
RUNG
    COMMENT CfgSrvJogAvcTO\r\n   Timeout na configuração do servo para avançar em JOG
END
RUNG
    COMMENT CfgSrvJogDesTO\r\n   Timeout na configuração do servo para desligar o JOG
END
RUNG
    COMMENT CfgSrvJogRecTO\r\n   Timeout na configuração do servo para recuar em JOG
END
RUNG
    COMMENT CfgSrvManTO\r\n   Timeout na configuração do servo para modo manual
END
RUNG
    COMMENT CfgSrvJogPosicTO\r\n   Timeout na configuração do servo para iniciar posicionamento
END
RUNG
    COMMENT CfgVelMaxTO\r\n   Timeout na configuração configuração da velocidade máxima do inversor quando saindo do modo de velocidade reduzida durante a operação automática
END
RUNG
    COMMENT CfgVelRedTO\r\n   Timeout na configuração configuração da velocidade reduzida do inversor quando entrando no modo de velocidade reduzida durante a operação automática
END
RUNG
    COMMENT ConfirmaOFF\r\n   Espera para confirmar que o botão de corte do perfil foi desligado e não apenas um faiscamento ao ligar
END
RUNG
    COMMENT CorteEstatTO\r\n   Timeout configurando o modo dinâmico ao sair do corte estático
END
RUNG
    COMMENT EspEncPrfFimTO\r\n   Timeout lendo encoder da perfiladeira no fim da Etapa 1 do cálculo da relação entre encoders
END
RUNG
    COMMENT EspEncPrfIniTO\r\n   Timeout lendo encoder da perfiladeira no início da Etapa 1 do cálculo da relação entre encoders
END
RUNG
    COMMENT EspEncSrvFimTO\r\n   Timeout lendo encoder do servo no fim da Etapa 2 do cálculo da relação entre encoders
END
RUNG
    COMMENT EspEncSrvIniTO\r\n   Timeout lendo encoder do servo no início da Etapa 2 do cálculo da relação entre encoders
END
RUNG
    COMMENT EspFimCfgJog\r\n   Aguarda 50 ms depois de desligar a flag de início de configuração do jog para que não saia da lógica enquanto a flag de máquina pronta não ativa
END
RUNG
    COMMENT EspFimMaterial\r\n   Espera o sinal do sensor de fim de material estabilizar para garantir que acabou o material e não que a chapa amassada, por exemplo, gere uma leitura errada
END
RUNG
    COMMENT EspFimProd\r\n   Gera intervalo de 500 ms entre as leituras do servo para identificar se a mesa recuou no fim da produção
END
RUNG
    COMMENT EspPilotoEstab\r\n   Aguarda 50 ms para garantir que o piloto está em cima
END
RUNG
    COMMENT EspReadInitOK\r\n   Gera intervalo de 1 segundo entre leituras da flag que indica que o servo terminou a inicialização
END
RUNG
    COMMENT EspReadPosicOK\r\n   Gera intervalo de 500 ms entre as leituras da flag que indica que o servo terminou o posicionamento
END
RUNG
    COMMENT EspServoON\r\n   Espera 500 ms para garantir que o servo está ativado
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
    COMMENT ModoDinamTO\r\n   Timeout enquanto configurando o modo dinâmico no servo
END
RUNG
    COMMENT PilotoDesceTO\r\n   Tempo que espera o piloto atuar. Após esse tempo gera erro no piloto
END
RUNG
    COMMENT RecuoCorteTO\r\n   No caso de ocorrer erro durante a operação de corte, aguarda esse tempo para tentar recuar a ferramenta antes de parar o perfil, evitando danos à ferramenta
END
RUNG
    COMMENT SrvEsperaAcel\r\n   Espera 500 ms depois de ativar o modo escravo no servo antes de checar se a velocidade está sincronizada
END
RUNG
    COMMENT SrvEsperaHab\r\n   Tempo que aguarda o sinal de Servo ON ligar antes de gerar erro na máquina
END
RUNG
    COMMENT SrvEsperaPosic\r\n   Espera 50 ms depois de ativar o posicionamento da mesa antes de checar se o posicionamento terminou
END
RUNG
    COMMENT SrvFimRefTO\r\n   Timeout na comunicação com o servo quando desligando a flag de inicialização do servo
END
RUNG
    COMMENT SrvInitAutoTO\r\n   Timeout enquanto comunicando com o servo para configurar o modo automático durante inicialização
END
RUNG
    COMMENT SrvStartRefTO\r\n   Timeout na comunicação com o servo quando ligando a flag de inicialização do servo
END
RUNG
    COMMENT TmrCalcPerfil\r\n   Intervalo entre os cálculos de deslocamento de perfil para identificar se ele está parado ou em movimento
END
RUNG
    COMMENT Descrição dos Contadores utilizados no programa:
END
RUNG
    COMMENT AguardarMesaOK\r\n   Contagem para espera da mesa recuar ao ponto de referência durante inicialização antes de abortar e fazer um posicionamento
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
    CONTACTS XDbgMaqLib 0 0
    COIL YMbMaqLib 0 1 0 3
END
RUNG
    CONTACTS RErroGeral 1 0
    CONTACTS XInversorOK 0 0
    CONTACTS XTermBombaHidr 0 0
    PARALLEL
        CONTACTS XServoOK 0 0
        SERIES
            CONTACTS YSrvHabilita 1 0
            TOF TSrvEsperaHab 200000
        END
    END
    CONTACTS YMbMaqLib 0 3
    COIL RMaqOK 0 0 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    OSF
    PARALLEL
        COIL YMbMaqLib 0 0 1 3
        COIL YSrvHabilita 0 0 1 0
        COIL RMaqInitStart 0 0 1 0
        COIL RMaqInit 0 0 1 0
        COIL RMaqInitFim 0 0 1 0
        COIL RMaqInitOK 0 0 1 0
    END
END
RUNG
    PARALLEL
        CONTACTS RMaqOK 1 0
        CONTACTS RMaqCfgInvErro 0 0
        CONTACTS RMaqCfgSrvErro 0 0
    END
    COIL YLedErro 0 0 0 0
END
RUNG
    CONTACTS RMaqCfgInvOK 0 0
    CONTACTS RMaqCfgSrvOK 0 0
    COIL RMaqConfigOK 0 0 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RMaqInitOK 0 0
    CONTACTS RMaqConfigOK 0 0
    COIL RMaqPronta 0 0 0 0
END
RUNG
    PARALLEL
        CONTACTS RCortePerfAtivo 0 0
        CONTACTS RSrvPosicAtivo 0 0
        CONTACTS RCorteEstatAtivo 0 0
        CONTACTS RCalcCorrAtivo 0 0
        CONTACTS RCalcRelAtivo 0 0
        CONTACTS RCalcTamMinAtivo 0 0
    END
    COIL RMaqOperando 0 0 0 0
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
    COMMENT Converte valor lido do encoder para décimos de mm.\r\nFórmula: ( ValEnc * EncPerim * 10 / 4 ) / EncResol
END
RUNG
    COMMENT Sendo:\r\nValEnc = Valor lido do encoder
END
RUNG
    COMMENT EncPerim = Perímetro da roda do encoder em mm\r\n10 = Converte mm para décimos de mm
END
RUNG
    COMMENT 4 = Fator de multiplicação do encoder na leitura, assumindo x4\r\nEncResol = Resolução do encoder em pulsos por rotação
END
RUNG
    MUL MaqPosAtual EMaqPosAtual YMbEncPerim
END
RUNG
    MUL MaqPosAtual MaqPosAtual 10
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
    DIV MaqPosAtual MaqPosAtual 1000
END
RUNG
    ADD PosProxCorte MaqPosCorte MaqPecaTam
END
RUNG
    SUB DifPosCorte PosProxCorte MaqPosAtual
END
RUNG
    SUB DifPosCorteInv 0 DifPosCorte
END
RUNG
    CONTACTS RCalcVarPerfil 1 0
    TON TTmrCalcPerfil 100000
    PARALLEL
        COIL RCalcVarPerfil 0 0 0 0
        SUB PerfilVariacao PerfilPosAnt MaqPosAtual
        COIL RMaqPerfilParado 0 0 1 0
        SERIES
            GEQ PerfilVariacao -1
            LEQ PerfilVariacao 1
            COIL RMaqPerfilParado 0 1 0 0
        END
        MOVE PerfilPosAnt MaqPosAtual
    END
END
RUNG
    DIV YMbEncMesa MaqPosAtual 10
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
        CONTACTS RMaqProduzindo 0 0
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
            CONTACTS YMbModoAutoAtivo 1 0
            CONTACTS RCortePerfAtivo 1 0
            TON TBombaHidrInat 300000000
        END
        SERIES
            CONTACTS RMaqOK 1 0
            PARALLEL
                CONTACTS RCortePerfAtivo 1 0
                CONTACTS RModoAuto 1 0
            END
        END
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
    PARALLEL
        CONTACTS RMaqOperando 1 0
        CONTACTS RCorteEstatAtivo 0 0
    END
    CONTACTS RCortePerfAtivo 1 0
    PARALLEL
        COIL RCortePerfAtivo 0 1 0 0
        COIL YCorteTravaMesa 0 1 0 0
        COIL RCortarPerf 0 0 1 0
        COIL YMbCorteIHM 0 0 1 1
        COIL RMaqCortePerf 0 0 1 0
        COIL RCorteRealizado 0 0 1 0
        MOVE CNumTentPiloto 0
        COIL YMbDbgAcionouSns 0 0 1 15
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
        SERIES
            CONTACTS YMbModoAutoAtivo 0 0
            CONTACTS XServoProxFim 0 0
            CONTACTS RMaqModoEstatico 1 0
            CONTACTS RMaqModoAjuste 1 0
            CONTACTS RCorteRealizado 1 0
        END
    END
    PARALLEL
        SERIES
            PARALLEL
                TON TRecuoCorteTO 2000000
                CONTACTS RErroGeral 0 0
            END
            PARALLEL
                COIL RCortePerfAtivo 0 0 1 0
                COIL YCorteTravaMesa 0 0 1 0
            END
        END
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
    CONTACTS RCortePerfAtivo 0 0
    CONTACTS RCortePronto 0 0
    CONTACTS XSnsSupTravaMesa 0 0
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
            CTU CNumTentPiloto 1
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
        COIL YMbDbgAcionouSns 0 1 0 15
        SERIES
            CONTACTS RSnsSupCortePerf 0 0
            COIL RInvAtivar 0 0 1 0
        END
        SERIES
            PARALLEL
                CONTACTS XServoProxFim 1 0
                CONTACTS YMbModoAutoAtivo 1 0
                CONTACTS RMaqModoAjuste 0 0
                CONTACTS RMaqStartProd 0 0
                CONTACTS RMaqModoEstatico 0 0
            END
            PARALLEL
                COIL RCorteRealizado 0 1 0 0
                MOVE MaqPosCorte 0
                RESET_ENC ZEncResetVal
            END
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
    CONTACTS XSnsSupTravaMesa 1 0
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
                    PARALLEL
                        SERIES
                            CONTACTS XSnsInfPiloto 0 0
                            CONTACTS XSnsSupPiloto 1 0
                        END
                        SERIES
                            CONTACTS RMaqUsarPiloto 1 0
                            CONTACTS XSnsSupTravaMesa 0 0
                        END
                    END
                    CONTACTS RCortePerfDesce 1 0
                    CONTACTS RSnsSupCortePerf 1 0
                END
                SERIES
                    CONTACTS YMbModoAutoAtivo 0 0
                    CONTACTS XServoProxFim 0 0
                    CONTACTS RMaqModoEstatico 1 0
                    CONTACTS RMaqModoAjuste 1 0
                    CONTACTS RCorteRealizado 1 0
                END
            END
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
    CONTACTS RMaqStartProd 1 0
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
    OSR
    CONTACTS RCortePerfAtivo 1 0
    COIL RCortarPerf 0 1 0 0
END
RUNG
    COMMENT Controle da Perfiladeira - Lógica de Saídas
END
RUNG
    PARALLEL
        SERIES
            PARALLEL
                SERIES
                    CONTACTS YMbModoAutoAtivo 0 0
                    CONTACTS RCortePronto 0 0
                END
                SERIES
                    PARALLEL
                        CONTACTS RCalcRelE1Avanca 0 0
                        CONTACTS RCalcTamMinAtivo 0 0
                    END
                    CONTACTS RFerramCorteCima 0 0
                END
            END
            PARALLEL
                CONTACTS RInvAtivar 0 0
                SERIES
                    CONTACTS YInvFreioMotor 0 0
                    CONTACTS RMaqPerfilParado 1 0
                END
            END
        END
        SERIES
            PARALLEL
                CONTACTS RAvancarPerfMan 0 0
                CONTACTS RRecuarPerfMan 0 0
            END
            TOF TMantemFreioLib 5000000
            CONTACTS RCortePerfAtivo 1 0
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
    COMMENT Lógica de controle manual da mesa do corte voador (JOG)
END
RUNG
    PARALLEL
        CONTACTS RMaqPronta 0 0
        SERIES
            CONTACTS RStartCfgSrvJog 0 0
            TOF TEspFimCfgJog 50000
            CONTACTS RMaqOK 0 0
        END
    END
    CONTACTS YMbModoAutoAtivo 1 0
    CONTACTS RFerramCorteCima 0 0
    CONTACTS RMaqOperando 1 0
    CONTACTS RMaqCfgSrvErro 1 0
    COIL RSrvManualOK 0 0 0 0
END
RUNG
    CONTACTS RSrvManualOK 0 0
    PARALLEL
        CONTACTS XMesaManAvanca 0 0
        CONTACTS YMesaManAvcIHM 0 9
    END
    CONTACTS XMesaManRecua 1 0
    CONTACTS YMesaManRecuaIHM 1 10
    PARALLEL
        COIL RAvancarMesaMan 0 0 0 0
        SERIES
            OSR
            COIL RStartCfgSrvJog 0 1 0 0
        END
    END
END
RUNG
    CONTACTS RSrvManualOK 0 0
    CONTACTS XMesaManAvanca 1 0
    CONTACTS YMesaManAvcIHM 1 9
    PARALLEL
        CONTACTS XMesaManRecua 0 0
        CONTACTS YMesaManRecuaIHM 0 10
    END
    PARALLEL
        COIL RRecuarMesaMan 0 0 0 0
        SERIES
            OSR
            COIL RStartCfgSrvJog 0 1 0 0
        END
    END
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RAvancarMesaMan 0 0
            OSF
            CONTACTS RRecuarMesaMan 1 0
        END
        SERIES
            CONTACTS RRecuarMesaMan 0 0
            OSF
            CONTACTS RAvancarMesaMan 1 0
        END
    END
    COIL RStartCfgSrvJog 0 1 0 0
END
RUNG
    CONTACTS RStartCfgSrvJog 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    CONTACTS RAvancarMesaMan 0 0
    PARALLEL
        SERIES
            OSR
            COIL RMaqCfgSrvOK 0 0 1 0
        END
        SERIES
            TON TCfgSrvJogAvcTO 5000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            READ_MODBUS SrvRegConfig 3 15030 0
            SET_BIT SrvRegConfig 1 5
            SET_BIT SrvRegConfig 0 6
            WRITE_MODBUS SrvRegConfig 3 15030 0
            PARALLEL
                COIL RStartCfgSrvJog 0 0 1 0
                COIL RMaqCfgSrvOK 0 1 0 0
            END
        END
    END
END
RUNG
    CONTACTS RStartCfgSrvJog 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    CONTACTS RRecuarMesaMan 0 0
    PARALLEL
        SERIES
            OSR
            COIL RMaqCfgSrvOK 0 0 1 0
        END
        SERIES
            TON TCfgSrvJogRecTO 5000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            READ_MODBUS SrvRegConfig 3 15030 0
            SET_BIT SrvRegConfig 0 5
            SET_BIT SrvRegConfig 1 6
            WRITE_MODBUS SrvRegConfig 3 15030 0
            PARALLEL
                COIL RStartCfgSrvJog 0 0 1 0
                COIL RMaqCfgSrvOK 0 1 0 0
            END
        END
    END
END
RUNG
    CONTACTS RStartCfgSrvJog 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    CONTACTS RAvancarMesaMan 1 0
    CONTACTS RRecuarMesaMan 1 0
    PARALLEL
        SERIES
            OSR
            COIL RMaqCfgSrvOK 0 0 1 0
        END
        SERIES
            TON TCfgSrvJogDesTO 5000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            READ_MODBUS SrvRegConfig 3 15030 0
            SET_BIT SrvRegConfig 0 5
            SET_BIT SrvRegConfig 0 6
            WRITE_MODBUS SrvRegConfig 3 15030 0
            PARALLEL
                COIL RStartCfgSrvJog 0 0 1 0
                COIL RMaqCfgSrvOK 0 1 0 0
            END
        END
    END
END
RUNG
    COMMENT Lógica de posicionamento manual da mesa do corte voador
END
RUNG
    CONTACTS XDbgSrvPosMan 0 4
    COIL YMbSrvPosMesaMan 0 1 0 6
END
RUNG
    CONTACTS RMaqPronta 0 0
    CONTACTS RSrvManualOK 0 0
    CONTACTS RAvancarMesaMan 1 0
    CONTACTS RRecuarMesaMan 1 0
    CONTACTS YMbSrvPosMesaMan 0 6
    PARALLEL
        SERIES
            CONTACTS RSrvPosicAtivo 1 0
            PARALLEL
                COIL RMaqStartPosic 0 1 0 0
                MUL VSrvPosMesa YMbSrvPosMesa 10
            END
        END
        COIL YMbSrvPosMesaMan 0 0 1 6
    END
END
RUNG
    CONTACTS RMaqOK 0 0
    PARALLEL
        CONTACTS XDbgStartPosic 0 3
        CONTACTS RMaqStartPosic 0 0
    END
    PARALLEL
        CONTACTS RMaqOperando 1 0
        CONTACTS RCorteEstatAtivo 0 0
        CONTACTS RCalcCorrAtivo 0 0
        CONTACTS RCalcRelAtivo 0 0
        CONTACTS RCalcTamMinAtivo 0 0
    END
    CONTACTS RSrvPosicAtivo 1 0
    PARALLEL
        COIL RSrvStartPosic 0 1 0 0
        COIL RSrvPosicAtivo 0 1 0 0
        COIL RMaqStartPosic 0 0 1 0
        MUL SrvPosMesa VSrvPosMesa 100
    END
END
RUNG
    CONTACTS RSrvStartPosic 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    PARALLEL
        SERIES
            TON TCfgSrvPosicTO 5000000
            PARALLEL
                COIL RMaqCfgSrvErro 0 1 0 0
                COIL RSrvStartPosic 0 0 1 0
                COIL RSrvPosicAtivo 0 0 1 0
            END
        END
        SERIES
            WRITE_MODBUS SrvPosMesa 3 15036 1
            READ_MODBUS SrvRegConfig 3 15030 0
            SET_BIT SrvRegConfig 0 2
            SET_BIT SrvRegConfig 1 10
            WRITE_MODBUS SrvRegConfig 3 15030 0
            TON TSrvEsperaPosic 50000
            COIL RSrvStartPosic 0 0 1 0
        END
    END
END
RUNG
    CONTACTS XDbgSrvPosicOK 0 5
    MOVE SrvRegConfig 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RSrvPosicAtivo 0 0
    CONTACTS RSrvStartPosic 1 0
    CONTACTS RSrvReadPosicOK 1 0
    READ_MODBUS SrvRegConfig 3 15030 0
    PARALLEL
        SERIES
            TON TEspReadPosicOK 500000
            COIL RSrvReadPosicOK 0 0 0 0
        END
        SERIES
            CHECK_BIT SrvRegConfig 0 10
            COIL RSrvPosicAtivo 0 0 1 0
        END
    END
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    COIL RSrvStartPosic 0 0 1 0
END
RUNG
    COMMENT Cálculo do fator de correção do encoder do perfil
END
RUNG
    CONTACTS RMaqPronta 0 0
    PARALLEL
        CONTACTS RMaqCalcCorrecao 0 0
        CONTACTS XDbgCalcCorrecao 0 9
    END
    PARALLEL
        SERIES
            CONTACTS RCalcCorrAtivo 1 0
            COIL RCalcCorrAtivo 0 1 0 0
        END
        COIL RMaqCalcCorrecao 0 0 1 0
    END
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcCorrAtivo 0 0
    CONTACTS RPosCorrConfig 1 0
    PARALLEL
        SERIES
            CONTACTS RCalcCorrZero 1 0
            CONTACTS RCalcCorrPosic 1 0
            MOVE VSrvPosMesa 0
        END
        SERIES
            CONTACTS RCalcCorrZero 0 0
            CONTACTS RCalcCorrPosic 1 0
            MOVE VSrvPosMesa SrvMesaCurso
        END
        SERIES
            CONTACTS RCalcCorrZero 0 0
            CONTACTS RCalcCorrPosic 0 0
            MOVE VSrvPosMesa 0
        END
        COIL RPosCorrConfig 0 1 0 0
    END
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCalcCorrAtivo 0 0
    COIL RPosCorrConfig 0 0 1 0
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCalcCorrPosic 0 0
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcCorrAtivo 0 0
    CONTACTS RCalcCorrZero 0 0
    COIL RCalcCorrAtivo 0 0 1 0
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCalcCorrPosic 1 0
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcCorrAtivo 0 0
    CONTACTS RCalcCorrZero 0 0
    PARALLEL
        COIL RCalcCorrPosic 0 1 0 0
        SUB CalcCorrPosDif CalcCorrPosIni MaqPosAtual
        MUL YMbNovoFator SrvMesaCurso 1000
        SERIES
            NEQ CalcCorrPosDif 0
            DIV YMbNovoFator YMbNovoFator CalcCorrPosDif
        END
    END
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCalcCorrZero 1 0
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcCorrAtivo 0 0
    PARALLEL
        COIL RCalcCorrZero 0 1 0 0
        MOVE CalcCorrPosIni MaqPosAtual
    END
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcCorrAtivo 0 0
    CONTACTS RCalcCorrZero 1 0
    CONTACTS RPosCorrConfig 0 0
    CONTACTS RSrvPosicAtivo 1 0
    COIL RMaqStartPosic 0 1 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcCorrAtivo 0 0
    CONTACTS RCalcCorrPosic 1 0
    CONTACTS RPosCorrConfig 0 0
    CONTACTS RSrvPosicAtivo 1 0
    CONTACTS RCalcCorrZero 0 0
    COIL RMaqStartPosic 0 1 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcCorrAtivo 0 0
    CONTACTS RCalcCorrPosic 0 0
    CONTACTS RPosCorrConfig 0 0
    CONTACTS RSrvPosicAtivo 1 0
    COIL RMaqStartPosic 0 1 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcCorrAtivo 0 0
    OSF
    PARALLEL
        COIL RCalcCorrAtivo 0 0 1 0
        COIL RCalcCorrPosic 0 0 1 0
        COIL RCalcCorrZero 0 0 1 0
    END
END
RUNG
    COMMENT Cálculo da relação entre os encoders do perfil e do servo\r\nSão duas etapas: cálculo da relação entre os encoders do perfil (mesa e perfiladeira) e cálculo da relação entre os encoders do perfil (mesa) e do servo
END
RUNG
    CONTACTS RMaqPronta 0 0
    PARALLEL
        CONTACTS RMaqCalcRelacao 0 0
        CONTACTS XDbgCalcRelacao 0 10
    END
    PARALLEL
        SERIES
            CONTACTS RCalcRelAtivo 1 0
            COIL RCalcRelAtivo 0 1 0 0
        END
        COIL RMaqCalcRelacao 0 0 1 0
    END
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcRelAtivo 0 0
    CONTACTS RPosRelConfig 1 0
    PARALLEL
        SERIES
            CONTACTS RCalcRelZero 1 0
            MOVE VSrvPosMesa 0
        END
        SERIES
            CONTACTS RCalcRelZero 0 0
            CONTACTS RCalcRelE2Fim 1 0
            MOVE VSrvPosMesa SrvMesaCurso
        END
        SERIES
            CONTACTS RCalcRelZero 0 0
            CONTACTS RCalcRelE2Fim 0 0
            MOVE VSrvPosMesa 0
        END
        COIL RPosRelConfig 0 1 0 0
    END
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCalcRelAtivo 0 0
    COIL RPosRelConfig 0 0 1 0
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCalcRelE2OK 0 0
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcRelAtivo 0 0
    PARALLEL
        COIL RCalcRelAtivo 0 0 1 0
        MOVE YMbDbgRelPerf CalcRelPerfFtr
        MOVE YMbDbgRelSrv CalcRelSrvFtr
    END
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCalcRelE1OK 0 0
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcRelAtivo 0 0
    PARALLEL
        COIL RCalcRelE2Start 0 0 1 0
        COIL RCalcRelE2Fim 0 1 0 0
    END
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCalcRelZero 1 0
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcRelAtivo 0 0
    PARALLEL
        COIL RCalcRelZero 0 1 0 0
        COIL RCalcRelE1Start 0 1 0 0
    END
END
RUNG
    CONTACTS RCalcRelAtivo 0 0
    CONTACTS RCalcRelE1Start 0 0
    PARALLEL
        SERIES
            TON TEspEncPrfIniTO 2000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            READ_MODBUS CalcRelPerfIni 3 15044 1
            PARALLEL
                COIL RCalcRelE1Avanca 0 1 0 0
                COIL RCalcRelE1Start 0 0 1 0
                MOVE CalcRelMesaIni MaqPosAtual
            END
        END
    END
END
RUNG
    CONTACTS RCalcRelAtivo 0 0
    CONTACTS RCalcRelE1Avanca 0 0
    PARALLEL
        SERIES
            OSR
            PARALLEL
                COIL RInvAtivar 0 1 0 0
                ADD CalcRelPosFim CalcRelMesaIni SrvMesaCurso
            END
        END
        SERIES
            CONTACTS RInvFreioLib 0 0
            PARALLEL
                SERIES
                    GEQ MaqPosAtual CalcRelPosFim
                    COIL RInvAtivar 0 0 1 0
                END
                SERIES
                    OSF
                    PARALLEL
                        COIL RCalcRelE1Avanca 0 0 1 0
                        COIL RCalcRelE1Fim 0 1 0 0
                    END
                END
            END
        END
    END
END
RUNG
    CONTACTS RCalcRelAtivo 0 0
    CONTACTS RCalcRelE1Fim 0 0
    PARALLEL
        SERIES
            TON TEspEncPrfFimTO 2000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            READ_MODBUS CalcRelPerfFim 3 15044 1
            PARALLEL
                COIL RCalcRelE1OK 0 1 0 0
                COIL RCalcRelE1Fim 0 0 1 0
                SUB CalcRelMesaDif MaqPosAtual CalcRelMesaIni
                SUB CalcRelPerfDif CalcRelPerfFim CalcRelPerfIni
                MUL CalcRelMesaDif CalcRelMesaDif 1000
                SERIES
                    NEQ CalcRelPerfDif 0
                    DIV CalcRelPerfFtr CalcRelMesaDif CalcRelPerfDif
                END
            END
        END
    END
END
RUNG
    CONTACTS RCalcRelAtivo 0 0
    CONTACTS RCalcRelE1OK 0 0
    CONTACTS RCalcRelE2Start 1 0
    CONTACTS RCalcRelE2Fim 1 0
    CONTACTS RCalcRelE2OK 1 0
    PARALLEL
        SERIES
            TON TEspEncSrvIniTO 2000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            READ_MODBUS CalcRelSrvIni 3 15046 1
            PARALLEL
                COIL RCalcRelE2Start 0 1 0 0
                MOVE CalcRelMesaIni MaqPosAtual
            END
        END
    END
END
RUNG
    CONTACTS RCalcRelAtivo 0 0
    CONTACTS RCalcRelE2Fim 0 0
    PARALLEL
        SERIES
            TON TEspEncSrvFimTO 2000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            READ_MODBUS CalcRelSrvFim 3 15046 1
            PARALLEL
                COIL RCalcRelE2OK 0 1 0 0
                COIL RCalcRelE2Fim 0 0 1 0
                SUB CalcRelMesaDif MaqPosAtual CalcRelMesaIni
                SUB CalcRelSrvDif CalcRelSrvFim CalcRelSrvIni
                MUL CalcRelMesaDif CalcRelMesaDif 1000
                SERIES
                    NEQ CalcRelSrvDif 0
                    DIV CalcRelSrvFtr CalcRelMesaDif CalcRelSrvDif
                END
            END
        END
    END
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcRelAtivo 0 0
    CONTACTS RCalcRelZero 1 0
    CONTACTS RPosRelConfig 0 0
    CONTACTS RSrvPosicAtivo 1 0
    COIL RMaqStartPosic 0 1 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcRelAtivo 0 0
    CONTACTS RCalcRelE1OK 0 0
    CONTACTS RCalcRelE2Start 0 0
    CONTACTS RPosRelConfig 0 0
    CONTACTS RSrvPosicAtivo 1 0
    COIL RMaqStartPosic 0 1 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcRelAtivo 0 0
    CONTACTS RCalcRelE1OK 0 0
    CONTACTS RCalcRelE2OK 0 0
    CONTACTS RPosRelConfig 0 0
    CONTACTS RSrvPosicAtivo 1 0
    COIL RMaqStartPosic 0 1 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    CONTACTS RCalcRelAtivo 0 0
    OSF
    PARALLEL
        COIL RCalcRelAtivo 0 0 1 0
        COIL RCalcRelZero 0 0 1 0
        COIL RCalcRelE1Start 0 0 1 0
        COIL RCalcRelE1Avanca 0 0 1 0
        COIL RCalcRelE1Fim 0 0 1 0
        COIL RCalcRelE1OK 0 0 1 0
        COIL RCalcRelE2Start 0 0 1 0
        COIL RCalcRelE2Fim 0 0 1 0
        COIL RCalcRelE2OK 0 0 1 0
    END
END
RUNG
    COMMENT Cálculo do tamanho mínimo da peça\r\nDescobre a menor peça possível de se fazer em modo dinâmico, considerando os parâmetros atuais
END
RUNG
    CONTACTS RMaqPronta 0 0
    PARALLEL
        CONTACTS RMaqCalcTamMin 0 0
        CONTACTS XDbgCalcTamMin 0 11
    END
    PARALLEL
        SERIES
            CONTACTS RCalcTamMinAtivo 1 0
            COIL RCalcTamMinAtivo 0 1 0 0
        END
        COIL RMaqCalcTamMin 0 0 1 0
    END
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcTamMinAtivo 0 0
    CONTACTS RPosTamMinConfig 1 0
    PARALLEL
        SERIES
            CONTACTS RCalcTamMinZero 1 0
            CONTACTS RCalcTamMinPosic 1 0
            MOVE VSrvPosMesa 0
        END
        SERIES
            CONTACTS RCalcTamMinZero 0 0
            CONTACTS RCalcTamMinPosic 1 0
            MOVE VSrvPosMesa SrvMesaCurso
        END
        SERIES
            CONTACTS RCalcTamMinZero 0 0
            CONTACTS RCalcTamMinPosic 0 0
            MOVE VSrvPosMesa 0
        END
        COIL RPosTamMinConfig 0 1 0 0
    END
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCalcTamMinAtivo 0 0
    COIL RPosTamMinConfig 0 0 1 0
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCalcTamMinPosic 0 0
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcTamMinAtivo 0 0
    CONTACTS RCalcTamMinZero 0 0
    PARALLEL
        COIL RCalcTamMinAtivo 0 0 1 0
        COIL RInvAtivar 0 0 1 0
        COIL RStartCfgInvMan 0 1 0 0
        SUB YMbNovoTamMin MaqPosAtual CalcTamMinIni
    END
END
RUNG
    CONTACTS RSrvStartPosic 0 0
    OSF
    CONTACTS RCalcTamMinAtivo 0 0
    MOVE CalcTamMinIni MaqPosAtual
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCalcTamMinPosic 1 0
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcTamMinAtivo 0 0
    CONTACTS RCalcTamMinZero 0 0
    COIL RCalcTamMinPosic 0 1 0 0
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCalcTamMinZero 1 0
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcTamMinAtivo 0 0
    PARALLEL
        COIL RCalcTamMinZero 0 1 0 0
        COIL RStartCfgInvAuto 0 1 0 0
    END
END
RUNG
    CONTACTS RMaqCfgInvOK 0 0
    OSR
    CONTACTS RCalcTamMinAtivo 0 0
    COIL RInvAtivar 0 1 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcTamMinAtivo 0 0
    CONTACTS RCalcTamMinZero 1 0
    CONTACTS RPosTamMinConfig 0 0
    CONTACTS RSrvPosicAtivo 1 0
    COIL RMaqStartPosic 0 1 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcTamMinAtivo 0 0
    CONTACTS RCalcTamMinPosic 1 0
    CONTACTS RPosTamMinConfig 0 0
    CONTACTS RSrvPosicAtivo 1 0
    CONTACTS RCalcTamMinZero 0 0
    CONTACTS RInvFreioLib 0 0
    COIL RMaqStartPosic 0 1 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcTamMinAtivo 0 0
    CONTACTS RCalcTamMinPosic 0 0
    CONTACTS RPosTamMinConfig 0 0
    CONTACTS RSrvPosicAtivo 1 0
    COIL RMaqStartPosic 0 1 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCalcTamMinAtivo 0 0
    OSF
    PARALLEL
        COIL RCalcTamMinAtivo 0 0 1 0
        COIL RCalcTamMinPosic 0 0 1 0
        COIL RCalcTamMinZero 0 0 1 0
        COIL RInvAtivar 0 0 1 0
    END
END
RUNG
    COMMENT Lógica de inicialização da máquina
END
RUNG
    CONTACTS XDbgParamSync 0 6
    COIL YMbParamSync 0 1 0 4
END
RUNG
    CONTACTS YMbParamSync 0 4
    PARALLEL
        COIL RStartCfgSrvPar 0 1 0 0
        COIL RMaqCfgSrvParOK 0 0 1 0
        COIL YMbParamSync 0 0 1 4
    END
END
RUNG
    CONTACTS RStartCfgSrvPar 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    CONTACTS RMaqProduzindo 1 0
    PARALLEL
        SERIES
            OSR
            COIL RMaqCfgSrvParOK 0 0 1 0
        END
        SERIES
            TON TCfgSrvIniTO 5000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            WRITE_MODBUS SrvMesaCurso 3 15048 1
            WRITE_MODBUS SrvVelJog 3 15034 1
            WRITE_MODBUS SrvVelPos 3 15032 1
            PARALLEL
                COIL RStartCfgSrvPar 0 0 1 0
                COIL RMaqCfgSrvParOK 0 1 0 0
            END
        END
    END
END
RUNG
    COMMENT Inicializa Servo
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RMaqConfigOK 0 0
    CONTACTS RMaqInitOK 1 0
    CONTACTS RMaqInit 1 0
    CONTACTS RMaqInitStart 1 0
    CONTACTS RMaqInitFim 1 0
    COIL RMaqInitStart 0 1 0 0
END
RUNG
    CONTACTS RMaqInitStart 0 0
    CONTACTS RFerramCorteCima 1 0
    COIL RMaqInitCorte 0 0 0 0
END
RUNG
    CONTACTS RMaqInitStart 0 0
    CONTACTS RFerramCorteCima 0 0
    CONTACTS YSrvHabilita 1 0
    PARALLEL
        SERIES
            TON TSrvInitAutoTO 2000000
            COIL RMaqInitStart 0 0 1 0
        END
        SERIES
            READ_MODBUS SrvRegConfig 3 15030 0
            SET_BIT SrvRegConfig 1 2
            WRITE_MODBUS SrvRegConfig 3 15030 0
            COIL YSrvHabilita 0 1 0 0
        END
    END
END
RUNG
    CONTACTS RMaqInitStart 0 0
    CONTACTS RFerramCorteCima 0 0
    CONTACTS YSrvHabilita 0 0
    TON TEspServoON 500000
    PARALLEL
        SERIES
            TON TSrvStartRefTO 2000000
            COIL RMaqInitStart 0 0 1 0
        END
        SERIES
            READ_MODBUS SrvRegConfig 3 15030 0
            SET_BIT SrvRegConfig 1 4
            WRITE_MODBUS SrvRegConfig 3 15030 0
            PARALLEL
                COIL RMaqInitStart 0 0 1 0
                COIL RMaqInit 0 1 0 0
                MOVE CAguardaMesaOK 0
            END
        END
    END
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RMaqInit 0 0
    CONTACTS RSrvReadInitOK 1 0
    READ_MODBUS SrvRegFlags 3 15000 0
    READ_MODBUS SrvRegStatus 3 15031 0
    PARALLEL
        SERIES
            TON TEspReadInitOK 1000000
            COIL RSrvReadInitOK 0 0 0 0
        END
        SERIES
            PARALLEL
                SERIES
                    CHECK_BIT SrvRegFlags 1 2
                    PARALLEL
                        CHECK_BIT SrvRegStatus 1 6
                        CTU CAguardaMesaOK 5
                    END
                END
                CONTACTS XDbgSrvInitOK 0 8
            END
            PARALLEL
                COIL RMaqInitFim 0 1 0 0
                COIL RMaqInit 0 0 1 0
                MOVE CAguardaMesaOK 0
            END
        END
    END
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RMaqInitFim 0 0
    CONTACTS RSrvStartPosic 1 0
    CONTACTS RSrvPosicAtivo 1 0
    PARALLEL
        SERIES
            TON TSrvFimRefTO 2000000
            COIL RMaqInitFim 0 0 1 0
        END
        SERIES
            READ_MODBUS SrvRegConfig 3 15030 0
            SET_BIT SrvRegConfig 0 4
            WRITE_MODBUS SrvRegConfig 3 15030 0
            PARALLEL
                COIL RMaqStartPosic 0 1 0 0
                MOVE VSrvPosMesa 0
            END
        END
    END
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RMaqInitFim 0 0
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    PARALLEL
        COIL RMaqInitFim 0 0 1 0
        SERIES
            CONTACTS RMaqOK 0 0
            COIL RMaqInitOK 0 1 0 0
        END
    END
END
RUNG
    COMMENT Lógica de corte estático do perfil (Posiciona a mesa para corte com a perfiladeira desligada)
END
RUNG
    CONTACTS XDbgCorteEstat 0 7
    OSR
    COIL RMaqCorteEstat 0 1 0 0
END
RUNG
    CONTACTS RMaqPronta 0 0
    CONTACTS RMaqCorteEstat 0 0
    CONTACTS RCorteEstatAtivo 1 0
    PARALLEL
        COIL RCorteEstatAtivo 0 1 0 0
        COIL RMaqPosConfig 0 0 1 0
        COIL RMaqCorteEstat 0 0 1 0
    END
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCorteEstatAtivo 0 0
    CONTACTS RMaqPosConfig 1 0
    PARALLEL
        SERIES
            CONTACTS RCorteEstatZero 1 0
            CONTACTS RCorteEstatPosic 1 0
            MOVE VSrvPosMesa 0
        END
        SERIES
            CONTACTS RCorteEstatZero 0 0
            CONTACTS RCorteEstatPosic 1 0
            PARALLEL
                SERIES
                    PARALLEL
                        LES DifPosCorteInv 0
                        GRT DifPosCorteInv SrvMesaCurso
                    END
                    PARALLEL
                        COIL RMaqErroPosic 0 1 0 0
                        COIL RCorteEstatAtivo 0 0 1 0
                    END
                END
                SERIES
                    GEQ DifPosCorteInv 0
                    LEQ DifPosCorteInv SrvMesaCurso
                    MOVE VSrvPosMesa DifPosCorteInv
                END
            END
        END
        SERIES
            CONTACTS RCorteEstatZero 0 0
            CONTACTS RCorteEstatPosic 0 0
            MOVE VSrvPosMesa 0
        END
        COIL RMaqPosConfig 0 1 0 0
    END
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCorteEstatAtivo 0 0
    COIL RMaqPosConfig 0 0 1 0
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCorteEstatPosic 0 0
    CONTACTS RMaqOK 0 0
    CONTACTS RCorteEstatAtivo 0 0
    CONTACTS RCorteEstatZero 0 0
    COIL RCorteEstatRetOK 0 1 0 0
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCorteEstatPosic 1 0
    CONTACTS RMaqOK 0 0
    CONTACTS RCorteEstatAtivo 0 0
    CONTACTS RCorteEstatZero 0 0
    COIL RCorteEstatPosic 0 1 0 0
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCorteEstatZero 1 0
    CONTACTS RMaqOK 0 0
    CONTACTS RCorteEstatAtivo 0 0
    COIL RCorteEstatZero 0 1 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCorteEstatAtivo 0 0
    CONTACTS RCorteEstatZero 1 0
    CONTACTS RMaqPosConfig 0 0
    CONTACTS RSrvPosicAtivo 1 0
    COIL RMaqStartPosic 0 1 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCorteEstatAtivo 0 0
    CONTACTS RCorteEstatPosic 1 0
    CONTACTS RMaqPosConfig 0 0
    CONTACTS RSrvPosicAtivo 1 0
    CONTACTS RCorteEstatZero 0 0
    COIL RMaqStartPosic 0 1 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCorteEstatAtivo 0 0
    CONTACTS RCorteEstatPosic 0 0
    OSR
    COIL RMaqCortePerf 0 1 0 0
END
RUNG
    CONTACTS RCortePerfAtivo 0 0
    OSF
    CONTACTS RMaqOK 0 0
    CONTACTS RCorteEstatAtivo 0 0
    CONTACTS RCorteEstatPosic 0 0
    CONTACTS RSrvPosicAtivo 1 0
    PARALLEL
        SERIES
            PARALLEL
                CONTACTS RMaqModoAjuste 1 0
                CONTACTS RErroCortePerf 1 0
            END
            COIL RMaqStartPosic 0 1 0 0
        END
        SERIES
            CONTACTS RMaqStartPosic 1 0
            COIL RCorteEstatRetOK 0 1 0 0
        END
    END
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCorteEstatAtivo 0 0
    CONTACTS RCorteEstatRetOK 0 0
    PARALLEL
        SERIES
            TON TCorteEstatTO 2000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            READ_MODBUS SrvRegConfig 3 15030 0
            SET_BIT SrvRegConfig 1 2
            WRITE_MODBUS SrvRegConfig 3 15030 0
            PARALLEL
                COIL RCorteEstatAtivo 0 0 1 0
                COIL YSrvEscravoAtivo 0 0 1 0
            END
        END
    END
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RCorteEstatAtivo 0 0
    OSF
    PARALLEL
        COIL RCorteEstatAtivo 0 0 1 0
        COIL RCorteEstatPosic 0 0 1 0
        COIL RCorteEstatRetOK 0 0 1 0
        COIL RCorteEstatZero 0 0 1 0
    END
END
RUNG
    COMMENT Lógica de configuração da mesa em cada modo de operação
END
RUNG
    CONTACTS RStartCfgSrvAuto 0 0
    CONTACTS RMaqCfgSrvParOK 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    PARALLEL
        SERIES
            OSR
            COIL RMaqCfgSrvOK 0 0 1 0
        END
        SERIES
            TON TCfgSrvAutoTO 5000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            READ_MODBUS SrvRegConfig 3 15030 0
            SET_BIT SrvRegConfig 1 2
            WRITE_MODBUS SrvRegConfig 3 15030 0
            PARALLEL
                COIL RStartCfgSrvAuto 0 0 1 0
                COIL RMaqCfgSrvOK 0 1 0 0
            END
        END
    END
END
RUNG
    CONTACTS RStartCfgSrvMan 0 0
    CONTACTS RMaqCfgSrvParOK 0 0
    CONTACTS RMaqCfgSrvErro 1 0
    CONTACTS YMbModoAutoAtivo 1 0
    PARALLEL
        SERIES
            OSR
            COIL RMaqCfgSrvOK 0 0 1 0
        END
        SERIES
            TON TCfgSrvManTO 5000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            READ_MODBUS SrvRegConfig 3 15030 0
            SET_BIT SrvRegConfig 0 2
            WRITE_MODBUS SrvRegConfig 3 15030 0
            PARALLEL
                COIL RStartCfgSrvMan 0 0 1 0
                COIL RMaqCfgSrvOK 0 1 0 0
            END
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
    CONTACTS RMaqCfgSrvOK 0 0
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
    CONTACTS RMaqCfgSrvOK 0 0
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
    COMMENT Lógica de configuração da velocidade do inversor para modo normal / reduzido\r\nConfigurado quando necessário sincronizar em modo automático e o inversor está produzindo em velocidade superior à máxima suportada
END
RUNG
    CONTACTS RMaqVelReduzida 0 0
    PARALLEL
        SERIES
            OSR
            PARALLEL
                COIL RStartCfgVelRed 0 1 0 0
                COIL RStartCfgVelMax 0 0 1 0
            END
        END
        SERIES
            OSF
            PARALLEL
                COIL RStartCfgVelMax 0 1 0 0
                COIL RStartCfgVelRed 0 0 1 0
            END
        END
    END
END
RUNG
    CONTACTS RStartCfgVelMax 0 0
    CONTACTS RMaqCfgInvErro 1 0
    CONTACTS RMaqCfgSrvOK 0 0
    PARALLEL
        SERIES
            TON TCfgVelMaxTO 3000000
            COIL RMaqCfgInvErro 0 1 0 0
        END
        SERIES
            WRITE_USS WInvAutoVel 0 105 0 0
            WRITE_USS WInvAutoVel 0 104 0 0
            COIL RStartCfgVelMax 0 0 1 0
        END
    END
END
RUNG
    CONTACTS RStartCfgVelRed 0 0
    CONTACTS RMaqCfgInvErro 1 0
    CONTACTS RMaqCfgSrvOK 0 0
    PARALLEL
        SERIES
            TON TCfgVelRedTO 3000000
            COIL RMaqCfgInvErro 0 1 0 0
        END
        SERIES
            WRITE_USS WInvMaxSyncVel 0 105 0 0
            WRITE_USS WInvMaxSyncVel 0 104 0 0
            COIL RStartCfgVelRed 0 0 1 0
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
        COIL RStartCfgSrvMan 0 1 0 0
        COIL RStartCfgSrvAuto 0 0 1 0
    END
END
RUNG
    CONTACTS RModoAuto 0 0
    OSR
    CONTACTS RMaqStartProd 1 0
    CONTACTS RMaqProduzindo 1 0
    CONTACTS RMaqFimProd 1 0
    CONTACTS RMaqErroPosic 1 0
    PARALLEL
        COIL RStartCfgInvAuto 0 1 0 0
        COIL RStartCfgInvMan 0 0 1 0
        COIL RStartCfgSrvAuto 0 1 0 0
        COIL RStartCfgSrvMan 0 0 1 0
        MOVE CQtdProd YMbQtd
        COIL RMaqModoEstatico 0 0 1 0
        SERIES
            PARALLEL
                LES MaqPecaTam MaqPecaTamMin
                CONTACTS XSnsFimMaterial 1 0
            END
            COIL RMaqModoEstatico 0 1 0 0
        END
    END
END
RUNG
    PARALLEL
        CONTACTS RMaqOK 1 0
        CONTACTS RMaqCfgSrvErro 0 0
        CONTACTS RMaqCfgInvErro 0 0
        SERIES
            CONTACTS RMaqModoEstatico 1 0
            CONTACTS XSnsFimMaterial 1 0
            TON TEspFimMaterial 500000
        END
        SERIES
            CONTACTS RMaqErroPosic 0 0
            CONTACTS RMaqStartProd 0 0
        END
        SERIES
            CONTACTS RErroCortePerf 0 0
            OSR
            CONTACTS RErroCortePiloto 1 0
            CONTACTS XServoProxFim 1 0
        END
    END
    PARALLEL
        CONTACTS RMaqProduzindo 0 0
        CONTACTS RMaqStartProd 0 0
        CONTACTS RMaqFimProd 0 0
    END
    PARALLEL
        COIL RMaqProduzindo 0 0 1 0
        COIL RMaqStartProd 0 0 1 0
        COIL RMaqFimProd 0 0 1 0
        COIL RMaqCorteEstat 0 0 1 0
        COIL RMaqStartPosic 0 0 1 0
        COIL YSrvEscravoAtivo 0 0 1 0
        COIL RMaqCortePerf 0 0 1 0
        COIL YMbModoAutoAtivo 0 0 1 0
        COIL RMaqModoEstatico 0 0 1 0
    END
END
RUNG
    CONTACTS RModoAuto 1 0
    CONTACTS RMaqStartProd 1 0
    CONTACTS RMaqProduzindo 1 0
    CONTACTS RMaqFimProd 1 0
    CONTACTS YMbModoAutoAtivo 0 0
    COIL YMbModoAutoAtivo 0 0 1 0
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    OSF
    PARALLEL
        COIL RMaqCortePerf 0 0 1 0
        SERIES
            CONTACTS RMaqFimProd 1 0
            PARALLEL
                COIL RInvAtivar 0 0 1 0
                COIL YMbModoAutoAtivo 0 0 1 0
            END
        END
        COIL RModoDinam 0 0 1 0
        COIL RMaqModoAjuste 0 0 1 0
        COIL RMaqModoCorrecao 0 0 1 0
        COIL RMaqCorteEstat 0 0 1 0
        COIL RMaqStartPosic 0 0 1 0
        COIL RMaqVelReduzida 0 0 1 0
        COIL RMaqModoEstatico 0 0 1 0
        COIL RSubQtdProd 0 0 1 0
        COIL RMaqPosCorteSet 0 0 1 0
        COIL RStartCfgInvAuto 0 0 1 0
        COIL RStartCfgInvMan 0 1 0 0
    END
END
RUNG
    COMMENT Lógica de operação automática da perfiladeira - Iniciando Produção
END
RUNG
    CONTACTS RMaqConfigOK 0 0
    OSR
    CONTACTS RMaqProduzindo 1 0
    CONTACTS RModoAutoOK 0 0
    GRT CQtdProd 0
    COIL RMaqStartProd 0 1 0 0
END
RUNG
    CONTACTS RMaqStartProd 0 0
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RCorteEstatAtivo 1 0
    PARALLEL
        SERIES
            GRT DifPosCorteInv SrvMesaCurso
            COIL RMaqErroPosic 0 1 0 0
        END
        SERIES
            LEQ DifPosCorte 0
            CONTACTS RMaqErroPosic 1 0
            COIL RMaqCorteEstat 0 1 0 0
        END
        SERIES
            GRT DifPosCorte 0
            COIL RMaqStartProd 0 0 1 0
        END
    END
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RMaqStartProd 0 0
    OSR
    CONTACTS RSrvPosicAtivo 1 0
    PARALLEL
        MOVE VSrvPosMesa 0
        COIL RMaqStartPosic 0 1 0 0
    END
END
RUNG
    CONTACTS RMaqStartProd 0 0
    CONTACTS RCorteEstatAtivo 0 0
    OSF
    PARALLEL
        COIL RMaqStartProd 0 0 1 0
        SERIES
            CONTACTS RErroCortePerf 1 0
            COIL RSubQtdProd 0 1 0 0
        END
    END
END
RUNG
    CONTACTS RMaqStartProd 0 0
    OSF
    PARALLEL
        SERIES
            CONTACTS RModoAutoOK 0 0
            CONTACTS RErroCortePerf 1 0
            PARALLEL
                COIL RMaqProduzindo 0 1 0 0
                SERIES
                    CONTACTS RMaqModoEstatico 1 0
                    COIL RInvAtivar 0 1 0 0
                END
            END
        END
        SERIES
            CONTACTS RModoAutoOK 1 0
            COIL YMbModoAutoAtivo 0 0 1 0
        END
    END
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RModoDinam 1 0
    CONTACTS RMaqModoAjuste 1 0
    CONTACTS RMaqModoEstatico 1 0
    PARALLEL
        SERIES
            TON TModoDinamTO 5000000
            COIL RMaqCfgSrvErro 0 1 0 0
        END
        SERIES
            READ_MODBUS SrvRegConfig 3 15030 0
            SET_BIT SrvRegConfig 1 2
            WRITE_MODBUS SrvRegConfig 3 15030 0
            COIL RModoDinam 0 1 0 0
        END
    END
END
RUNG
    MUL ProxFimCurso SrvMesaCurso 3
END
RUNG
    DIV ProxFimCurso ProxFimCurso 4
END
RUNG
    COMMENT Lógica de operação automática da perfiladeira - Loop de Produção - Modo Dinâmico
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqModoEstatico 1 0
    LES DifPosCorte MaqPosReduzVel
    CONTACTS RMaqModoAjuste 1 0
    CONTACTS YSrvEscravoAtivo 1 0
    GRT YMbAutoVel MaxInvSyncVel
    COIL RMaqVelReduzida 0 1 0 0
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqModoEstatico 1 0
    LEQ DifPosCorte YMbOffsetCorte
    CONTACTS RMaqModoAjuste 1 0
    CONTACTS YSrvEscravoAtivo 1 0
    PARALLEL
        COIL YSrvEscravoAtivo 0 1 0 0
        COIL RCorteRealizado 0 0 1 0
    END
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqModoEstatico 1 0
    CONTACTS YSrvEscravoAtivo 0 0
    TON TSrvEsperaAcel 500000
    CONTACTS XSrvMesaSync 0 0
    OSR
    CONTACTS RMaqSincronizado 1 0
    CONTACTS RCortePerfAtivo 1 0
    PARALLEL
        COIL RMaqCortePerf 0 1 0 0
        COIL RMaqSincronizado 0 1 0 0
    END
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqModoEstatico 1 0
    CONTACTS RCortePerfAtivo 1 0
    CONTACTS RMaqModoAjuste 1 0
    PARALLEL
        SERIES
            CONTACTS XServoProxFim 0 0
            CONTACTS RCorteRealizado 1 0
        END
        GRT DifPosCorteInv ProxFimCurso
    END
    PARALLEL
        COIL RMaqModoAjuste 0 1 0 0
        COIL YSrvEscravoAtivo 0 0 1 0
        COIL RMaqVelReduzida 0 0 1 0
    END
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqModoEstatico 1 0
    CONTACTS RMaqModoAjuste 1 0
    CONTACTS RMaqSincronizado 0 0
    CONTACTS RCortePerfAtivo 0 0
    PARALLEL
        COIL RMaqCortePerf 0 0 1 0
        SERIES
            OSF
            PARALLEL
                COIL YSrvEscravoAtivo 0 0 1 0
                COIL RMaqVelReduzida 0 0 1 0
                SERIES
                    PARALLEL
                        CONTACTS RErroCortePiloto 0 0
                        SERIES
                            CONTACTS RErroCortePerf 0 0
                            CONTACTS XServoProxFim 0 0
                        END
                    END
                    COIL RMaqModoAjuste 0 1 0 0
                END
            END
        END
    END
END
RUNG
    COMMENT Lógica de operação automática da perfiladeira - Loop de Produção - Modo Estático
END
RUNG
    LES MaqPecaTam SrvMesaCurso
    MOVE PosLimiteCorte ProxFimCurso
END
RUNG
    GEQ MaqPecaTam SrvMesaCurso
    MOVE PosLimiteCorte 0
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqModoEstatico 0 0
    LES DifPosCorteInv PosLimiteCorte
    CONTACTS RMaqCortando 1 0
    CONTACTS RInvAtivar 1 0
    COIL RInvAtivar 0 1 0 0
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqModoEstatico 0 0
    GEQ DifPosCorteInv PosLimiteCorte
    CONTACTS RMaqCortando 1 0
    PARALLEL
        COIL RMaqCortando 0 1 0 0
        COIL RInvAtivar 0 0 1 0
        COIL RPosEstatConfig 0 0 1 0
    END
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqModoEstatico 0 0
    CONTACTS RMaqCortando 0 0
    CONTACTS RInvFreioLib 1 0
    CONTACTS RPosEstatConfig 1 0
    PARALLEL
        SERIES
            CONTACTS RMaqPosCorteSet 1 0
            PARALLEL
                ADD PosNovoCorte DifPosCorteInv MaqPecaTam
                COIL RMaqPosCorteSet 0 1 0 0
            END
        END
        SERIES
            LES PosNovoCorte MaqPecaTam
            PARALLEL
                COIL RMaqFimCorte 0 1 0 0
                MOVE VSrvPosMesa 0
                COIL RInvAtivar 0 1 0 0
            END
        END
        SERIES
            CONTACTS RMaqFimCorte 1 0
            PARALLEL
                SUB PosNovoCorte PosNovoCorte MaqPecaTam
                MOVE VSrvPosMesa PosNovoCorte
            END
        END
        COIL RMaqStartPosic 0 1 0 0
        COIL RPosEstatConfig 0 1 0 0
    END
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    OSF
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqModoEstatico 0 0
    CONTACTS RMaqModoAjuste 1 0
    PARALLEL
        SERIES
            CONTACTS RMaqFimCorte 0 0
            PARALLEL
                COIL RMaqCortando 0 0 1 0
                COIL RMaqPosCorteSet 0 0 1 0
                COIL RMaqFimCorte 0 0 1 0
                COIL RPosEstatConfig 0 0 1 0
            END
        END
        SERIES
            CONTACTS RMaqCortando 0 0
            COIL RMaqCortePerf 0 1 0 0
        END
    END
END
RUNG
    PARALLEL
        CONTACTS RCortePerfAtivo 0 0
        CONTACTS RMaqModoAjuste 0 0
    END
    OSF
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqModoEstatico 0 0
    CONTACTS RMaqCortando 0 0
    PARALLEL
        SERIES
            CONTACTS RErroCortePerf 0 0
            COIL RMaqModoAjuste 0 1 0 0
        END
        SERIES
            CONTACTS RMaqModoAjuste 1 0
            PARALLEL
                COIL RPosEstatConfig 0 0 1 0
                COIL RSubQtdProd 0 1 0 0
            END
        END
    END
END
RUNG
    CONTACTS RMaqModoEstatico 0 0
    OSF
    PARALLEL
        COIL RMaqCortando 0 0 1 0
        COIL RMaqFimCorte 0 0 1 0
        COIL RPosEstatConfig 0 0 1 0
    END
END
RUNG
    COMMENT Modo de Ajuste e Correção\r\nSe houve algum problema durante a produção da peça, entra neste modo para tentar corrigir o problema
END
RUNG
    CONTACTS RMaqModoAjuste 0 0
    OSR
    PARALLEL
        COIL RInvAtivar 0 0 1 0
        COIL RModoDinam 0 0 1 0
        SERIES
            CONTACTS RMaqModoEstatico 1 0
            PARALLEL
                COIL RErroCortePerf 0 0 1 0
                COIL RErroCortePiloto 0 0 1 0
            END
        END
        SERIES
            CONTACTS RMaqModoEstatico 0 0
            COIL RMaqModoCorrecao 0 1 0 0
        END
    END
END
RUNG
    CONTACTS RMaqModoAjuste 0 0
    PARALLEL
        SERIES
            CONTACTS RMaqModoCorrecao 1 0
            CONTACTS RCorteEstatAtivo 0 0
        END
        SERIES
            CONTACTS RMaqModoCorrecao 0 0
            CONTACTS RSrvPosicAtivo 0 0
        END
    END
    OSF
    CONTACTS RMaqProduzindo 0 0
    PARALLEL
        SERIES
            CONTACTS RMaqErroPosic 1 0
            CONTACTS RErroCortePerf 1 0
            PARALLEL
                SERIES
                    CONTACTS RMaqModoEstatico 1 0
                    COIL RInvAtivar 0 1 0 0
                END
                COIL RMaqModoAjuste 0 0 1 0
                COIL RMaqModoCorrecao 0 0 1 0
            END
        END
        SERIES
            PARALLEL
                CONTACTS RMaqErroPosic 0 0
                CONTACTS RErroCortePerf 0 0
            END
            COIL RMaqModoCorrecao 0 1 0 0
        END
    END
END
RUNG
    CONTACTS RMaqModoAjuste 0 0
    CONTACTS RMaqModoCorrecao 1 0
    CONTACTS RCorteEstatAtivo 1 0
    COIL RMaqCorteEstat 0 1 0 0
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
    CONTACTS RSrvPosicAtivo 1 0
    OSR
    PARALLEL
        SERIES
            CONTACTS RErroCortePerf 0 0
            COIL RMaqCortePerf 0 1 0 0
        END
        SERIES
            CONTACTS RMaqErroPosic 0 0
            COIL RMaqModoCorrecao 0 0 1 0
        END
        COIL RMaqErroPosic 0 0 1 0
        COIL RErroCortePerf 0 0 1 0
        COIL RErroCortePiloto 0 0 1 0
    END
END
RUNG
    CONTACTS RMaqModoCorrecao 0 0
    CONTACTS RCortePerfAtivo 0 0
    OSF
    CONTACTS RErroCortePerf 1 0
    PARALLEL
        MOVE VSrvPosMesa 0
        COIL RMaqStartPosic 0 1 0 0
    END
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
            PARALLEL
                CONTACTS YSrvEscravoAtivo 0 0
                CONTACTS RMaqModoAjuste 0 0
            END
            OSF
        END
    END
    CONTACTS RMaqProduzindo 0 0
    PARALLEL
        COIL RSubQtdProd 0 0 1 0
        COIL RMaqSincronizado 0 0 1 0
        SERIES
            PARALLEL
                SERIES
                    CTD CQtdProd 1
                    OSF
                END
                CONTACTS RModoAuto 1 0
            END
            PARALLEL
                COIL RMaqProduzindo 0 0 1 0
                COIL RMaqFimProd 0 1 0 0
                SERIES
                    CONTACTS RMaqModoEstatico 0 0
                    PARALLEL
                        MOVE VSrvPosMesa 0
                        COIL RMaqStartPosic 0 1 0 0
                    END
                END
            END
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
    CONTACTS RMaqFimProd 0 0
    CONTACTS RSrvCheckFimProd 1 0
    READ_MODBUS SrvRegStatus 3 15031 0
    PARALLEL
        SERIES
            TON TEspFimProd 500000
            PARALLEL
                COIL RSrvCheckFimProd 0 0 0 0
                COIL RInvAtivar 0 0 1 0
            END
        END
        SERIES
            CHECK_BIT SrvRegStatus 1 6
            PARALLEL
                COIL RMaqFimProd 0 0 1 0
                COIL YMbModoAutoAtivo 0 0 1 0
                COIL RInvAtivar 0 0 1 0
            END
        END
    END
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
    CONTACTS YMbLimpaErro 0 2
    PARALLEL
        COIL RMaqCfgInvErro 0 0 1 0
        COIL RMaqCfgSrvErro 0 0 1 0
        COIL RMaqErroPosic 0 0 1 0
        COIL YMbLimpaErro 0 0 1 2
    END
END
RUNG
    CONTACTS YMbCalcCorrecao 0 11
    PARALLEL
        COIL RMaqCalcCorrecao 0 1 0 0
        COIL YMbCalcCorrecao 0 0 1 11
    END
END
RUNG
    CONTACTS YMbCalcRelacao 0 12
    PARALLEL
        COIL RMaqCalcRelacao 0 1 0 0
        COIL YMbCalcRelacao 0 0 1 12
    END
END
RUNG
    CONTACTS YMbCalcTamMin 0 13
    PARALLEL
        COIL RMaqCalcTamMin 0 1 0 0
        COIL YMbCalcTamMin 0 0 1 13
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
    CONTACTS XServoOK 1 0
    COIL YMbErrServo 0 0 0 4
END
RUNG
    CONTACTS RMaqCfgInvErro 0 0
    COIL YMbErrInvComunic 0 0 0 5
END
RUNG
    CONTACTS RMaqCfgSrvErro 0 0
    COIL YMbErrSrvComunic 0 0 0 6
END
RUNG
    CONTACTS RMaqErroPosic 0 0
    COIL YMbErrPosic 0 0 0 7
END
RUNG
    CONTACTS RErroCortePerf 0 0
    COIL YMbErrCorte 0 0 0 8
END
RUNG
    CONTACTS YMbMaqLib 1 3
    COIL YMbMaqDesativada 0 0 0 9
END
RUNG
    COMMENT Flags que indicam o estado atual da máquina - Somente Leitura
END
RUNG
    CONTACTS RSrvPosicAtivo 0 0
    COIL YMbSrvPosicAtivo 0 0 0 1
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
    CONTACTS RCorteEstatAtivo 0 0
    COIL YMbCrtEstatAtivo 0 0 0 4
END
RUNG
    PARALLEL
        CONTACTS RMaqInitStart 0 0
        CONTACTS RMaqInit 0 0
        CONTACTS RMaqInitFim 0 0
    END
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
    CONTACTS RMaqCfgSrvOK 0 0
    COIL YMbMaqCfgSrvOK 0 0 0 11
END
RUNG
    CONTACTS RMaqModoEstatico 0 0
    COIL YMbMaqModoEstat 0 0 0 12
END
RUNG
    CONTACTS RMaqCortando 0 0
    COIL YMbMaqCortando 0 0 0 13
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
        RESET_ENC ZEncResetVal
        MOVE MaqPosCorte 0
        COIL YMbOffsetCorte 0 0 1 0
        COIL YMbAutoAcel 0 0 1 0
        COIL YMbAutoDesac 0 0 1 0
        COIL YMbDinamVel 0 0 1 0
        COIL YMbEstatVel 0 0 1 0
        COIL YMbManAcel 0 0 1 0
        COIL YMbManDesac 0 0 1 0
        COIL YMbManVel 0 0 1 0
        COIL YMbEncFator 0 0 1 0
        COIL YMbEncResol 0 0 1 0
        COIL YMbEncPerim 0 0 1 0
        COIL YMbTam 0 0 1 0
        COIL YMbQtd 0 0 1 0
        COIL YMbEncMesa 0 0 1 0
    END
END
RUNG
    OSR
    PARALLEL
        MOVE YMbTam 3000
        MOVE YMbQtd 30
        MOVE YMbDinamVel 60
        MOVE YMbEstatVel 70
        MOVE YMbAutoAcel 200
        MOVE YMbAutoDesac 75
        MOVE YMbManVel 30
        MOVE YMbManAcel 400
        MOVE YMbManDesac 100
        MOVE MaxInvSyncVel 30
        MOVE WInvModoAuto 1
        MOVE WInvModoMan 0
        MOVE YMbEncFator 1000
        MOVE YMbEncResol 2500
        MOVE YMbEncPerim 400
        MOVE YMbOffsetCorte 4
    END
END
RUNG
    OSR
    PARALLEL
        MOVE MaqPosReduzVel 2500
        COIL YMbMesaCurso 0 0 1 0
        MOVE YMbMesaCurso 800
        COIL YMbSrvPosMesa 0 0 1 0
        MOVE YMbSrvPosMesa 0
        COIL YMbSrvVelJog 0 0 1 0
        MOVE YMbSrvVelJog 50
        COIL YMbSrvVelPos 0 0 1 0
        MOVE YMbSrvVelPos 100
        COIL YMbNovoFator 0 0 1 0
        MOVE YMbNovoFator 1000
        COIL YMbTamMin 0 0 1 0
        MUL YMbTamMin YMbMesaCurso 2
        COIL YMbNovoTamMin 0 0 1 0
        COIL YMbDbgRelPerf 0 0 1 0
        COIL YMbDbgRelSrv 0 0 1 0
    END
END
RUNG
    COMMENT Calculando variáveis conforme os valores recebidos pelo ModBUS
END
RUNG
    COMMENT Convertendo tamanho recebido da peça de mm para décimos de mm
END
RUNG
    MUL MaqPecaTam YMbTam 10
END
RUNG
    COMMENT Convertendo o tamanho do curso da mesa para décimos de milímetro para manter tudo na mesma unidade internamente
END
RUNG
    MUL SrvMesaCurso YMbMesaCurso 10
END
RUNG
    MUL MaqPecaTamMin YMbTamMin 10
END
RUNG
    MUL ZEncResetVal SrvMesaCurso 2
END
RUNG
    COMMENT Calculando parâmetros de velocidade para enviar ao servo
END
RUNG
    MUL SrvVelPos YMbSrvVelPos 22000
END
RUNG
    DIV SrvVelPos SrvVelPos 100
END
RUNG
    MUL SrvVelJog YMbSrvVelJog 5000
END
RUNG
    DIV SrvVelJog SrvVelJog 100
END
RUNG
    COMMENT Calculando parametros de velocidade para enviar ao inversor
END
RUNG
    MUL WInvAutoVel YMbDinamVel 60
END
RUNG
    CONTACTS RMaqModoEstatico 0 0
    MUL WInvAutoVel YMbEstatVel 60
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
    MUL WInvMaxSyncVel MaxInvSyncVel 60
END
RUNG
    DIV WInvMaxSyncVel WInvMaxSyncVel 10
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
