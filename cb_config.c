// Para que a funcao atof funcione corretamente
#include <stdio.h>
#include <stdlib.h>

#include <net/modbus.h>

#include <gtk/gtk.h>

#include "defines.h"
#include "GtkUtils.h"

// Estrutura que representa o ModBus
extern struct MB_Device mbdev;

/*** Funcoes e variáveis de suporte ***/

int idUser=0; // Indica usuário que logou se for diferente de zero.
extern int CurrentWorkArea;  // Variavel que armazena a tela atual.
extern int PreviousWorkArea; // Variavel que armazena a tela anterior.

// Função que salva um log no banco contendo usuário e data que gerou o evento.
extern void Log(char *evento, int tipo);

// Vetor de erros da máquina e função que retorna o erro atual conforme prioridade
char *StrErro[] =
  {
  "Emergencia",
  "Problema no servo",
  "Problema no Posicionamento",
  "Falha seguindo Perfil",
  "Erro na configuração",
  "Tempo máximo de inatividade alcançado",
  "Tempo máximo de atuação do cilindro excedida",
  "Supervisor indicando falta de fase",
  "Erro no sistema hidráulico",
  "Falha na comunicação",
  };

char * LerStrErro(unsigned int erro)
{
  unsigned int i;

  for(i=0;erro>>i;i++)
    if((erro>>i)&1)
      return StrErro[i];

  return "Desconhecido";
}

char * Crypto(char *str)
{
  return (char *)(crypt(str,"bc")+2);
}

int GetUserPerm(char *permissao)
{
  char sql[300];

  sprintf(sql, "select p.valor from permissoes as p, lista_permissoes as l, usuarios as u where p.ID_perm = l.ID and p.ID_user = u.ID and l.nome = '%s' and u.id = '%d'", permissao, idUser);
  DB_Execute(&mainDB, 0, sql);

// Se existe permissão para o usuário atual, retorna seu valor.
  if(DB_GetNextRow(&mainDB, 0)>0)
    return atoi(DB_GetData(&mainDB, 0, 0));
  else
    {
// Não encontrou a permissão para usuário atual, carregando default.
    sprintf(sql, "select valor from lista_permissoes where nome='%s'", permissao);
    DB_Execute(&mainDB, 0, sql);
    if(DB_GetNextRow(&mainDB, 0)>0)
      return atoi(DB_GetData(&mainDB, 0, 0)); // Retornando valor default
    }

// Não existe a permissão. Retorna zero.
  return 0;
}

#if 0
// Função que é executada a cada 200 ms e que serve para checar o estado da máquina.
gint sync_cv(gpointer dados)
{
  static unsigned int erro_ant = CV_ERRO_NENHUM, num_erros_timeout=0;
  char str[100];

  MQ.MQ_Data.funcao = CV_MQFNC_ERRO;
  switch(MQ_Transfer(&MQ))
    {
    default: // Erro não foi de timeout, limpa erros.
      num_erros_timeout=0;
      break;

    case MQ_ERRO_NENHUM:
      num_erros_timeout=0;
      // Verifica se o processo filho está pedindo encerramento
      if(MQ.MQ_Data.funcao == CV_MQFNC_FINAL)
        {
        MessageBox("Erro no Corte Voador! Saindo...");
        gtk_main_quit();
        return FALSE;
        }

      if(MQ.MQ_Data.data.di[0]!=erro_ant)
        {
        erro_ant = MQ.MQ_Data.data.di[0];

        if(erro_ant & CV_ERRO_CONFIG)
          {
          AbrirConfig(NULL, NULL); // Abre a janela de configuração da máquina
          sprintf(str, "Erro na configuração da máquina! Favor reconfigurar");
          }
        else if(erro_ant != CV_ERRO_NENHUM && erro_ant != CV_ERRO_ESCRAVO)
          {
          EstadoBotoes((GtkWidget *)(dados), (guint)(BTN_CFG | BTN_INIT)); // Habilita apenas config e init.
          sprintf(str, "Erro: %s", LerStrErro(erro_ant));
          }
        else
          {
          EstadoBotoes((GtkWidget *)(dados), (guint)(BTN_CFG | BTN_INIT | BTN_OPER | BTN_MANUAL)); // Habilita tudo.
          strcpy(str, "Sem erros");
          }

        // Registra o erro ocorrido
        Log(str, LOG_TIPO_ERRO);

        MessageBox(str);
        }

      break;

    case MQ_ERRO_TIMEOUT:
      if(num_erros_timeout++ > 10) // Se ocorreram mais que 10 erros de comunicação seguidos
        {
        // Registra o erro ocorrido
        Log("Erro entre os processos", LOG_TIPO_ERRO);

        MessageBox("Processo de controle da máquina não está respondendo! Saindo...");
        gtk_main_quit();
        return FALSE;
        }
    }

  return TRUE;
}

int MaquinaEspera(int checar_manual)
{
  MQ.MQ_Data.funcao = CV_MQFNC_STATUS;
  if(MQ_Transfer(&MQ)==MQ_ERRO_NENHUM)
    {
    if(MQ.MQ_Data.data.di[0] != CV_ST_ESPERA && (checar_manual ? MQ.MQ_Data.data.di[0] != CV_ST_MANUAL : 1))
      MessageBox("Existe algum erro na máquina! Reinicialize.");
    else
      return 1;
    }

  return 0;
}

gint AtualInfoProd(gpointer dados)
{
  static int DeveAtualizar = 1; // Faz com que na primeira vez a tela seja atualizada.
  static long InfoTotal;
  long InfoProd;
  float InfoPos;
  char tmp[20];
  GtkWidget *lblInfoPos, *lblInfoProd;

  if(DeveAtualizar)
    InfoTotal = atol(gtk_label_get_text(GTK_LABEL(lookup_widget((GtkWidget *)(dados), "lblMsgTotal"))));

  lblInfoPos = lookup_widget((GtkWidget *)(dados), "lblMsgPos");
  if(lblInfoPos==NULL)
    {
    DeveAtualizar=1;
    return FALSE;
    }

  MQ.MQ_Data.funcao = CV_MQFNC_TAM_ATUAL;
  if(MQ_Transfer(&MQ)==MQ_ERRO_NENHUM)
    {
    // Carrega o valor atual
    InfoPos   = atof(gtk_label_get_text(GTK_LABEL(lblInfoPos)));

    // Atualiza o campo se houve alteração desde a última atualização
    if(MQ.MQ_Data.data.df[0] != InfoPos || DeveAtualizar)
      {
      sprintf(tmp, "%.03f", MQ.MQ_Data.data.df[0]);
      gtk_label_set_text(GTK_LABEL(lblInfoPos), tmp);
      }
    }

  MQ.MQ_Data.funcao = CV_MQFNC_QTDPROD;
  if(MQ_Transfer(&MQ)==MQ_ERRO_NENHUM)
    {
    // Encontra a referência do objeto e retorna se não existir
    lblInfoProd  = lookup_widget((GtkWidget *)(dados), "lblMsgProd");
    if(lblInfoProd==NULL) // Não existe! A janela deve ter sido fechada
      {
      DeveAtualizar=1;
      return FALSE;
      }

    // Carrega o valor atual
    InfoProd  = atol(gtk_label_get_text(GTK_LABEL(lblInfoProd)));

    // Atualiza os campos se houve alteração desde a última atualização
    if(MQ.MQ_Data.data.dl[0] != InfoProd || DeveAtualizar)
      {
      // Atualiza a contagem de peças produzidas
      sprintf(tmp, "%d", MQ.MQ_Data.data.di[0]);
      gtk_label_set_text(GTK_LABEL(lblInfoProd), tmp);

      // Atualiza o saldo a produzir se não for a primeira vez
      if(DeveAtualizar == 0)
        {
        sprintf(tmp, "%d", InfoTotal - MQ.MQ_Data.data.di[0]);
        gtk_label_set_text(GTK_LABEL(lookup_widget((GtkWidget *)(dados), "lblMsgSaldo")), tmp);
        }
      }
    }

  DeveAtualizar = 0;

  return TRUE;
}

void
cbSairLoop                             (GtkObject       *object,
                                        gpointer         user_data)
{
  *(unsigned int *)(user_data) = 0; // Força saída do loop
}

int EnviaComando(struct strMQ *MQS, struct strMQD *MQD, void *userdata)
{
  int ret = 1;
  unsigned char msg[50];
  unsigned int espera=0, i, FicarEmLoop = 1;
  GtkWidget *wdg = create_wndMsg(), *lbl;

  switch(MQD->funcao)
    {
    case CV_MQFNC_CORTAR:
      strcpy(msg, "Cortando");
      break;

    case CV_MQFNC_RELENC:
      strcpy(msg, "Calculando fator entre encoders");
      break;

    case CV_MQFNC_INIT:
      strcpy(msg, "Inicializando");
      break;

    case CV_MQFNC_POSIC:
      strcpy(msg, "Posicionando");
      break;

    case CV_MQFNC_CALC_FATOR:
      strcpy(msg, "Calculando fator de correção");
      break;

    case CV_MQFNC_PRODUZ:
      gtk_widget_show_all(lookup_widget(wdg, "frmMsgInfo"));

      gtk_label_set_text(GTK_LABEL(lookup_widget(wdg, "lblMsgProd" )), "0");

      sprintf(msg, "%d", MQD->data.dl[0]);
      gtk_label_set_text(GTK_LABEL(lookup_widget(wdg, "lblMsgTotal")), msg);
      gtk_label_set_text(GTK_LABEL(lookup_widget(wdg, "lblMsgSaldo")), msg);

      // Funcao para atualizacao do campos de informação
      g_timeout_add(100,(GtkFunction)AtualInfoProd, (gpointer)(wdg));

      sprintf(msg, "Produzindo %d peças de %.03f mm", MQD->data.dl[0], MQD->data.df[1]);
      break;
    }

  lbl = lookup_widget(wdg, "lblMsg");
  gtk_label_set_text(GTK_LABEL(lbl), msg);

  g_signal_connect ((gpointer) wdg, "destroy",
            G_CALLBACK (cbSairLoop),
            (gpointer)(&FicarEmLoop));

  gtk_window_set_modal(GTK_WINDOW(wdg), 1);
  gtk_widget_show_now(wdg);

  MQS->MQ_Data.id     = MQD->id;
  MQS->MQ_Data.ndata  = MQD->ndata;
  MQS->MQ_Data.funcao = MQD->funcao;

  for(i=0; i<sizeof(MQD->data); i++)
    MQS->MQ_Data.data.dc[i] = MQD->data.dc[i];

  MQ_Send(MQS);

  while(FicarEmLoop)
    {
    gtk_main_iteration_do(FALSE);

    if(espera++>1000)
      {
      espera=0;

      MQS->MQ_Data.funcao = CV_MQFNC_STATUS;
      if(MQ_Transfer(MQS)==MQ_ERRO_NENHUM)
        {
        if(MQS->MQ_Data.data.di[0]==CV_ST_ERRO) // Ocorreu um erro
          {
          ret = 0;
          break;
          }
        else if(MQS->MQ_Data.data.di[0]==CV_ST_ESPERA) // Operação finalizada
          break;
        }
      }
    }

  if(!FicarEmLoop) // Saiu forçado do loop! Devemos retornar erro.
    {
    // Envia comando para interromper operação atual.
    MQ.MQ_Data.funcao = CV_MQFNC_PARAR;
    MQ_Send(&MQ);

    ret = 0;
    }

  gtk_widget_destroy(wdg);

  return ret;
}

/*** Fim das funcoes de suporte ***/

void
Operar                                 (GtkButton       *button,
                                        gpointer         user_data)
{
  int qtd, qtdprod, status;
  GtkWidget *wdg;
  struct strMQD MQD;
  char tmp[30], sql[300];

  if(!MaquinaEspera(CHECAR_ESPERA))
    return;

// Carrega os dados da tarefa selecionada
  wdg = lookup_widget(GTK_WIDGET(button), "tvwTarefas");

  TV_GetSelected(GTK_TREE_VIEW(wdg), 4, tmp); // Quantidade total
  MQD.data.dl[0] = atol(tmp);

  TV_GetSelected(GTK_TREE_VIEW(wdg), 5, tmp); // Subtrai quantidade já produzida
  MQD.data.dl[0] -= atol(tmp);

  TV_GetSelected(GTK_TREE_VIEW(wdg), 6, tmp);
  MQD.data.df[1] = atof(tmp);

  MQD.funcao = CV_MQFNC_PRODUZ;

  sprintf(sql, "Produzindo %d peças de %.03f mm", MQD.data.dl[0], MQD.data.df[1]);
  Log(sql, LOG_TIPO_TAREFA);
  EnviaComando(&MQ, &MQD, NULL);

  MQ.MQ_Data.funcao = CV_MQFNC_STATUS;
  if(MQ_Transfer(&MQ)==MQ_ERRO_NENHUM)
    {
    // Salva o estado atual da máquina
    status = MQ.MQ_Data.data.di[0];

    MQ.MQ_Data.funcao = CV_MQFNC_QTDPROD;
    if(MQ_Transfer(&MQ)==MQ_ERRO_NENHUM)
      qtdprod = MQ.MQ_Data.data.di[0];

// Recupera o ID da tarefa em execução
    TV_GetSelected(GTK_TREE_VIEW(wdg), 0, tmp);

    sprintf(sql, "select Qtd, QtdProd from tarefas where ID='%d'", atol(tmp));
    DB_Execute(&mainDB, 0, sql);
    DB_GetNextRow(&mainDB, 0);

// Adiciona o número de peças já produzidas ao número de peças produzidas nesta operação
    qtdprod += atol(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "QtdProd")));

// Carrega a quantidade de peças da tarefa
    qtd = atol(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "Qtd")));

    if(status == CV_ST_ESPERA)
      MessageBox("Tarefa executada sem erros!");
    else
      MessageBox("Erro encontrado enquanto produzindo!");

    if(qtdprod >= qtd) // Quantidade produzida maior ou igual ao total
      status = TRF_ESTADO_FINAL;
    else if(qtdprod) // Quantidade produzida diferente de zero
      status = TRF_ESTADO_PARCIAL;
    else // Nenhuma peça produzida
      status = TRF_ESTADO_NOVA;

    // Executa a instrução SQL necessária para atualizar o estado da tarefa.
    sprintf(sql, "update tarefas set estado='%d', QtdProd='%d' where ID='%d'", status, qtdprod, atol(tmp));
    DB_Execute(&mainDB, 0, sql);

    CarregaListaTarefas(wdg);
    }
}

void
CalcRelEnc                             (GtkButton       *button,
                                        gpointer         user_data)
{
  struct strMQD MQD;

  if(!MaquinaEspera(CHECAR_ESPERA))
    return;

  Log("Calculando relação entre encoders", LOG_TIPO_CONFIG);

  MQD.funcao = CV_MQFNC_RELENC;
  EnviaComando(&MQ, &MQD, NULL);
}

void
Inicializar                            (GtkButton       *button,
                                        gpointer         user_data)
{
  struct strMQD MQD;

  if(!GetUserPerm(PERM_ACESSO_OPER))
    {
    MessageBox("Sem permissão para operar a máquina!");
    return;
    }

  Log("Inicializando máquina", LOG_TIPO_TAREFA);

  MQD.funcao = CV_MQFNC_INIT;
  EnviaComando(&MQ, &MQD, NULL);

  MQ.MQ_Data.funcao = CV_MQFNC_STATUS;
  if(MQ_Transfer(&MQ)==MQ_ERRO_NENHUM)
    {
    if(MQ.MQ_Data.data.di[0]==CV_ST_ESPERA)
      {
      EstadoBotoes(GTK_WIDGET(button), (guint)(BTN_CFG | BTN_INIT | BTN_OPER | BTN_MANUAL)); // Habilita tudo.
      MessageBox("Maquina pronta para operar!");
      }
    else
      {
      EstadoBotoes(GTK_WIDGET(button), (guint)(BTN_CFG | BTN_INIT)); // Desabilita operacao.
      MessageBox("Erro encontrado enquanto inicializando!");
      }
    }
}

void
SaiManual                              (GtkObject       *object,
                                        gpointer         user_data)
{
  gtk_grab_remove(GTK_WIDGET(object));
}


void
PosicManual                            (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *wdg;
  struct strMQD MQD;

  if(!MaquinaEspera(CHECAR_MANUAL))
    return;

  wdg = lookup_widget(GTK_WIDGET(button), "entPosic");

  MQD.funcao = CV_MQFNC_POSIC;
  MQD.data.df[0] = atof((unsigned char *)(gtk_entry_get_text(GTK_ENTRY(wdg))));

  EnviaComando(&MQ, &MQD, NULL);
}

gint AtualEnc(gpointer dados)
{
  static int DeveAtualizar=1;
  long ValEnc;
  char tmp[20];
  GtkWidget *lbl;

  lbl = lookup_widget((GtkWidget *)(dados), "lblEncMesa");
  if(lbl==NULL)
    {
// Configura maquina para modo de espera
    MQ.MQ_Data.funcao = CV_MQFNC_MANUAL;
    MQ.MQ_Data.data.di[0] = 0;
    MQ_Transfer(&MQ);

    DeveAtualizar=1;
    return FALSE;
    }

  MQ.MQ_Data.funcao = CV_MQFNC_LERENC;
  MQ.MQ_Data.data.dl[0] = CV_ENC_MESA;
  if(MQ_Transfer(&MQ)==MQ_ERRO_NENHUM)
    {
    ValEnc = atol(gtk_label_get_text(GTK_LABEL(lbl)));

    if(MQ.MQ_Data.data.dl[0] != ValEnc || DeveAtualizar)
      {
      DeveAtualizar=0;
      sprintf(tmp, "%d", MQ.MQ_Data.data.dl[0]);
      gtk_label_set_text(GTK_LABEL(lbl), tmp);
      }
    }

  return TRUE;
}

void UpdateImageBoxes(GtkWidget *ref, char *nome, unsigned long valor)
{
  int i = 0;
  GtkWidget *obj;
  char nome_obj[20];

  while(1) // Sai apenas quando não encontra o objeto
    {
    sprintf(nome_obj, "%s%02d", nome, i);
    obj = lookup_widget(ref, nome_obj);

    // Acabaram os controles, saindo
    if(obj == NULL)
      break;

    gtk_image_set_from_stock(GTK_IMAGE(obj), (valor>>i)&1 ? "gtk-apply" : "gtk-media-record", GTK_ICON_SIZE_BUTTON);

    i++; // Próximo controle.
    }
}

void UpdateToggleButtons(GtkWidget *ref, char *nome, unsigned long valor)
{
  int i = 0;
  GtkWidget *obj;
  char nome_obj[20];

  while(1) // Sai apenas quando não encontra o objeto
    {
    sprintf(nome_obj, "%s%02d", nome, i);
    obj = lookup_widget(ref, nome_obj);

    // Acabaram os controles, saindo
    if(obj == NULL)
      break;

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(obj), (valor>>i)&1);

    i++; // Próximo controle.
    }
}

gint AtualIOs(gpointer dados)
{
  unsigned long val;
  GtkWidget *obj, *ref = (GtkWidget *)dados;
  char txt[20];

  obj = lookup_widget(ref, "ntbManut");
  if(obj == NULL) // objeto não existe, janela foi fechada!
    return FALSE; // Retorna FALSE para que esta função não seja recarregada.

// Checa a aba atual para atualizar apenas ela.
  switch(gtk_notebook_get_current_page(GTK_NOTEBOOK(obj)))
    {
    case 0: // Entradas e saídas digitais
      MQ.MQ_Data.ndata      = 2;
      MQ.MQ_Data.funcao     = CV_MQFNC_GETREGS;
      MQ.MQ_Data.data.dl[0] = CV_MQREG_PIO_DIGIN;
      MQ.MQ_Data.data.dl[1] = CV_MQREG_PIO_DIGOUT;

      MQ_Transfer(&MQ);

      UpdateImageBoxes   (ref, "imgManutSai", MQ.MQ_Data.data.dl[1]);
      UpdateToggleButtons(ref, "tglManutSai", MQ.MQ_Data.data.dl[1]);
      UpdateImageBoxes   (ref, "imgManutEnt", MQ.MQ_Data.data.dl[0]^MASK_GERAL_DIGIN);

      break;

    case 1: // Perfiladeira (Inversor Nord)
      MQ.MQ_Data.ndata      = 3;
      MQ.MQ_Data.funcao     = CV_MQFNC_GETREGS;
      MQ.MQ_Data.data.dl[0] = CV_MQREG_PERF_VIN;
      MQ.MQ_Data.data.dl[1] = CV_MQREG_PERF_AOUT;
      MQ.MQ_Data.data.dl[2] = CV_MQREG_PERF_TORQUE;

      MQ_Transfer(&MQ);

      if(MQ.MQ_Data.data.dl[0] == CV_PERF_USSDATA_INVAL)
        txt[0] = 0;
      else
        sprintf(txt, "%d", MQ.MQ_Data.data.dl[0]);

      gtk_entry_set_text(GTK_ENTRY(lookup_widget(ref, "entManutInvTensao")), txt);

      if(MQ.MQ_Data.data.dl[1] == CV_PERF_USSDATA_INVAL)
        txt[0] = 0;
      else
        sprintf(txt, "%.01f", (float)(MQ.MQ_Data.data.dl[1])/10);

      gtk_entry_set_text(GTK_ENTRY(lookup_widget(ref, "entManutInvCorrente")), txt);

      if(MQ.MQ_Data.data.dl[2] == CV_PERF_USSDATA_INVAL)
        txt[0] = 0;
      else
        sprintf(txt, "%d", MQ.MQ_Data.data.dl[2]);

      gtk_entry_set_text(GTK_ENTRY(lookup_widget(ref, "entManutInvTorque")), txt);

      MQ.MQ_Data.ndata      = 2;
      MQ.MQ_Data.funcao     = CV_MQFNC_GETREGS;
      MQ.MQ_Data.data.dl[0] = CV_MQREG_PERF_DIGIN;
      MQ.MQ_Data.data.dl[1] = CV_MQREG_PERF_DIGOUT;

      MQ_Transfer(&MQ);

      UpdateImageBoxes(ref, "imgManutInvEnt", MQ.MQ_Data.data.dl[0]);
      UpdateImageBoxes(ref, "imgManutInvSai", MQ.MQ_Data.data.dl[1]);

      break;

    case 2: // Mesa (Servomotor Yaskawa)
      MQ.MQ_Data.ndata      = 3;
      MQ.MQ_Data.funcao     = CV_MQFNC_GETREGS;
      MQ.MQ_Data.data.dl[0] = CV_MQREG_SERVO_VELRPM;
      MQ.MQ_Data.data.dl[1] = CV_MQREG_SERVO_VELMM;
      MQ.MQ_Data.data.dl[2] = CV_MQREG_SERVO_TORQUE;

      MQ_Transfer(&MQ);

      sprintf(txt, "%d", MQ.MQ_Data.data.dl[0]);
      gtk_entry_set_text(GTK_ENTRY(lookup_widget(ref, "entManutServoVelMotor")), txt);

      sprintf(txt, "%d", MQ.MQ_Data.data.dl[1]);
      gtk_entry_set_text(GTK_ENTRY(lookup_widget(ref, "entManutServoVelMesa")), txt);

      sprintf(txt, "%.02f", (float)(MQ.MQ_Data.data.dl[2])/100);
      gtk_entry_set_text(GTK_ENTRY(lookup_widget(ref, "entManutServoTorque")), txt);

      MQ.MQ_Data.ndata      = 2;
      MQ.MQ_Data.funcao     = CV_MQFNC_GETREGS;
      MQ.MQ_Data.data.dl[0] = CV_MQREG_SERVO_DIGIN;
      MQ.MQ_Data.data.dl[1] = CV_MQREG_SERVO_STATUS;

      MQ_Transfer(&MQ);

      UpdateImageBoxes(ref, "imgManutServoEnt", MQ.MQ_Data.data.dl[0]^MASK_SERVO_DIGIN);
      UpdateImageBoxes(ref, "imgManutServoSai", MQ.MQ_Data.data.dl[1]);

      break;
    }

  return TRUE;
}

void
AbrirManut                             (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *wnd;

  if(!GetUserPerm(PERM_ACESSO_MANUT))
    {
    MessageBox("Sem permissão para acesso à manutenção!");
    return;
    }

  wnd = (GtkWidget *)(create_wndManut());

// Funcao para atualizacao do estado dos I/Os
  g_timeout_add(1500,(GtkFunction)AtualIOs, (gpointer)(wnd));

  AbrirJanelaModal(wnd);
  gtk_grab_add(wnd);
}

void
CalcFatorMesa                          (GtkButton       *button,
                                        gpointer         user_data)
{
  struct strMQD MQD;

  if(!MaquinaEspera(CHECAR_ESPERA))
    return;

  Log("Calculando fator de correção da mesa", LOG_TIPO_CONFIG);

  MQD.funcao = CV_MQFNC_CALC_FATOR;
  EnviaComando(&MQ, &MQD, NULL);
  LerDadosConfig(GTK_WIDGET(button));
}

void
cbManualMesaRecua                      (GtkButton       *button,
                                        gpointer         user_data)
{
  MQ.MQ_Data.funcao = CV_MQFNC_JOG;
  MQ.MQ_Data.data.di[0] = CV_JOG_REVERSO;
  MQ_Transfer(&MQ);
}

void
cbManualMesaParar                      (GtkButton       *button,
                                        gpointer         user_data)
{
  MQ.MQ_Data.funcao = CV_MQFNC_JOG;
  MQ.MQ_Data.data.di[0] = CV_JOG_DESLIGA;
  MQ_Transfer(&MQ);
}

void
cbManualMesaAvanca                     (GtkButton       *button,
                                        gpointer         user_data)
{
  MQ.MQ_Data.funcao = CV_MQFNC_JOG;
  MQ.MQ_Data.data.di[0] = CV_JOG_AVANTE;
  MQ_Transfer(&MQ);
}

void
cbManualMesaCortar                     (GtkButton       *button,
                                        gpointer         user_data)
{
  struct strMQD MQD;

  if(!MaquinaEspera(CHECAR_MANUAL))
    return;

  MQD.funcao = CV_MQFNC_CORTAR;
  EnviaComando(&MQ, &MQD, NULL);
}

void
cbMsgParar                             (GtkButton       *button,
                                        gpointer         user_data)
{
  MQ.MQ_Data.funcao = CV_MQFNC_PARAR;
// Se enviou o comando para interromper operação, desabilita o botão para não
// permitir reenvio.
  if(MQ_Send(&MQ)==MQ_ERRO_NENHUM)
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
}

void
cbOperacaoManual                       (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *wnd;

  if(!GetUserPerm(PERM_ACESSO_OPER))
    {
    MessageBox("Sem permissão para operar a máquina!");
    return;
    }

  wnd = (GtkWidget *)(create_wndManual());

// Configura maquina para operar em modo manual
  MQ.MQ_Data.funcao = CV_MQFNC_MANUAL;
  MQ.MQ_Data.data.di[0] = 1;
  MQ_Transfer(&MQ);

// Funcao para atualizacao do valor dos encoders
  g_timeout_add(100,(GtkFunction)AtualEnc, (gpointer)(wnd));

  AbrirJanelaModal(wnd);
  gtk_grab_add(wnd);
}

void
cbManualPerfRecua                      (GtkButton       *button,
                                        gpointer         user_data)
{
  MQ.MQ_Data.funcao = CV_MQFNC_PERF_JOG;
  MQ.MQ_Data.data.di[0] = CV_JOG_REVERSO;
  MQ_Transfer(&MQ);
}


void
cbManualPerfParar                      (GtkButton       *button,
                                        gpointer         user_data)
{
  MQ.MQ_Data.funcao = CV_MQFNC_PERF_JOG;
  MQ.MQ_Data.data.di[0] = CV_JOG_DESLIGA;
  MQ_Transfer(&MQ);
}


void
cbManualPerfAvanca                     (GtkButton       *button,
                                        gpointer         user_data)
{
  MQ.MQ_Data.funcao = CV_MQFNC_PERF_JOG;
  MQ.MQ_Data.data.di[0] = CV_JOG_AVANTE;
  MQ_Transfer(&MQ);
}

#else
void CarregaComboClientes()
{
  GtkWidget *obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbClientes"));

// Carregamento dos clientes cadastrados no MySQL no ComboBox.
  DB_Execute(&mainDB, 0, "select nome from clientes order by ID");
  CarregaCombo(GTK_COMBO_BOX(obj),0, NULL);
}

void CarregaComboUsuarios()
{
  GtkWidget *obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbCadUserLogin"));

// O primeiro item é para inserção de novo usuário.
// Carregamento dos usuário cadastrados no MySQL no ComboBox.
  DB_Execute(&mainDB, 0, "select login from usuarios order by ID");
  CarregaCombo(GTK_COMBO_BOX(obj),0, "<Novo Usuário>");
}

void CarregaComboModelos()
{
  char sql[100];
  GtkWidget *obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbModNome"));

// O primeiro item é para inserção de novo modelo.
// Carregamento dos modelos cadastrados no MySQL no ComboBox.
  sprintf(sql, "select nome from modelos where estado='%d' order by ID", MOD_ESTADO_ATIVO);
  DB_Execute(&mainDB, 0, sql);
  CarregaCombo(GTK_COMBO_BOX(obj), 0, "<Novo Modelo>");
}

void CarregaDadosBanco()
{
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoIP"   )), mainDB.server );
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoUser" )), mainDB.user   );
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoSenha")), mainDB.passwd );
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoNome" )), mainDB.nome_db);
}

void GravaDadosBanco()
{
  char *server, *user, *passwd, *nome_db;

  server  = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoIP"   )));
  user    = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoUser" )));
  passwd  = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoSenha")));
  nome_db = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoNome" )));

// Se houve alteração nos dados de conexão, exige reinício do programa
  if(strcmp(server , mainDB.server ) ||
     strcmp(user   , mainDB.user   ) ||
     strcmp(passwd , mainDB.passwd ) ||
     strcmp(nome_db, mainDB.nome_db))
    {
    DB_Close(&mainDB);

    mainDB.server  = server ;
    mainDB.user    = user   ;
    mainDB.passwd  = passwd ;
    mainDB.nome_db = nome_db;

    if(!DB_Init(&mainDB))
      MessageBox("Configurações do banco inválidas! Saindo...");
    else
      {
      MessageBox("Configurações do banco alteradas com sucesso! Saindo...");
      DB_GravarConfig(&mainDB, DB_ARQ_CONFIG);
      }

    gtk_main_quit();
    }
}

char *lista_ent[] =
  {
    "entMesaCurso",
    "entMesaPerim",
    "entMesaFator",
    "entMesaResol",
    "entMesaVelMax",
    "entMesaVelJOG",
    "spbConfigPerfAcel",
    "spbConfigPerfDesacel",
    "spbConfigPerfVel",
    "rbtConfigModoNormal",
    ""
  };

int GravarDadosConfig()
{
  unsigned int nova_operacao;
  GtkWidget *obj;
  char **valor_ent, *lista_oper[] = { "Normal", "Estático", "" };
  valor_ent = (char **)(malloc(10*sizeof(char[10])));
/*
  // Carrega o valor dos widgets conforme a lista fornecida
  LerValoresWidgets(wdg, lista_ent, valor_ent);

  if(!BuscaStringLista(lista_oper, valor_ent[9])) // 0: Modo Normal
    nova_operacao = CV_OPER_NORMAL;
  else
    nova_operacao = CV_OPER_ESTAT;

  MQ.MQ_Data.ndata      = 2;
  MQ.MQ_Data.funcao     = CV_MQFNC_PUTREGS;
  MQ.MQ_Data.data.dl[0] = CV_MQREG_VELMAX;
  MQ.MQ_Data.data.dl[1] = CV_MQREG_VELJOG;
  MQ.MQ_Data.data.dl[2] = atol(valor_ent[4]);
  MQ.MQ_Data.data.dl[3] = atol(valor_ent[5]);

  if(MQ_Send(&MQ) != MQ_ERRO_NENHUM)
    return 1;

  MQ.MQ_Data.ndata      = 2;
  MQ.MQ_Data.funcao     = CV_MQFNC_PUTREGS;
  MQ.MQ_Data.data.dl[0] = CV_MQREG_FATOR;
  MQ.MQ_Data.data.dl[1] = CV_MQREG_RESOL;
  MQ.MQ_Data.data.df[2] = atof(valor_ent[2]);
  MQ.MQ_Data.data.dl[3] = atol(valor_ent[3]);

  if(MQ_Send(&MQ) != MQ_ERRO_NENHUM)
    return 1;

  MQ.MQ_Data.ndata      = 2;
  MQ.MQ_Data.funcao     = CV_MQFNC_PUTREGS;
  MQ.MQ_Data.data.dl[0] = CV_MQREG_CURSO;
  MQ.MQ_Data.data.dl[1] = CV_MQREG_PERIM;
  MQ.MQ_Data.data.df[2] = atof(valor_ent[0]);
  MQ.MQ_Data.data.df[3] = atof(valor_ent[1]);

  if(MQ_Send(&MQ) != MQ_ERRO_NENHUM)
    return 1;

  MQ.MQ_Data.ndata      = 2;
  MQ.MQ_Data.funcao     = CV_MQFNC_PUTREGS;
  MQ.MQ_Data.data.dl[0] = CV_MQREG_PERF_ACEL;
  MQ.MQ_Data.data.dl[1] = CV_MQREG_PERF_DESACEL;
  MQ.MQ_Data.data.df[2] = atof(valor_ent[6]);
  MQ.MQ_Data.data.df[3] = atof(valor_ent[7]);

  if(MQ_Send(&MQ) != MQ_ERRO_NENHUM)
    return 1;

  MQ.MQ_Data.ndata      = 2;
  MQ.MQ_Data.funcao     = CV_MQFNC_PUTREGS;
  MQ.MQ_Data.data.dl[0] = CV_MQREG_PERF_VELMAX;
  MQ.MQ_Data.data.dl[1] = CV_MQREG_OPERACAO;
  MQ.MQ_Data.data.df[2] = atof(valor_ent[8]);
  MQ.MQ_Data.data.dl[3] = nova_operacao;

  if(MQ_Send(&MQ) != MQ_ERRO_NENHUM)
    return 1;
*/
  GravaDadosBanco();

  return 0;
}

void LerDadosConfig()
{
  char tmp[100], *opt_operacao[] = { "Normal", "Estático" };
  unsigned int i, operacao;
  GtkWidget *obj;
  GSList *lst;
  char valor_ent[9][10];
/*
// Carrega os parâmetros de curso da mesa, perímetro do encoder, fator de correção
// e resolução do encoder
  MQ.MQ_Data.ndata      = 4;
  MQ.MQ_Data.funcao     = CV_MQFNC_GETREGS;
  MQ.MQ_Data.data.dl[0] = CV_MQREG_CURSO;
  MQ.MQ_Data.data.dl[1] = CV_MQREG_PERIM;
  MQ.MQ_Data.data.dl[2] = CV_MQREG_FATOR;
  MQ.MQ_Data.data.dl[3] = CV_MQREG_RESOL;

  if(MQ_Transfer(&MQ)==MQ_ERRO_NENHUM)
    {
    sprintf(valor_ent[0], "%.02f", MQ.MQ_Data.data.df[0]);
    sprintf(valor_ent[1], "%.02f", MQ.MQ_Data.data.df[1]);
    sprintf(valor_ent[2], "%.04f", MQ.MQ_Data.data.df[2]);
    sprintf(valor_ent[3],    "%d", MQ.MQ_Data.data.dl[3]);

    for(i=0; i<4; i++)
      {
      obj = lookup_widget(wdg, lista_ent[i]);
      gtk_entry_set_alignment(GTK_ENTRY(obj),1); // Alinha a esquerda
      gtk_entry_set_text(GTK_ENTRY(obj), valor_ent[i]);
      }
    }

// Carrega os parâmetros de velocidade máxima e manual
  MQ.MQ_Data.ndata      = 2;
  MQ.MQ_Data.funcao     = CV_MQFNC_GETREGS;
  MQ.MQ_Data.data.dl[0] = CV_MQREG_VELMAX;
  MQ.MQ_Data.data.dl[1] = CV_MQREG_VELJOG;

  if(MQ_Transfer(&MQ)==MQ_ERRO_NENHUM)
    {
    sprintf(valor_ent[4], "%ld", MQ.MQ_Data.data.dl[0]);
    sprintf(valor_ent[5], "%ld", MQ.MQ_Data.data.dl[1]);

    for(i=4; i<6; i++)
      {
      obj = lookup_widget(wdg, lista_ent[i]);
      gtk_entry_set_alignment(GTK_ENTRY(obj),1); // Alinha a esquerda
      gtk_entry_set_text(GTK_ENTRY(obj), valor_ent[i]);
      }
    }

// Carrega os parâmetros da perfiladeira
  MQ.MQ_Data.ndata      = 3;
  MQ.MQ_Data.funcao     = CV_MQFNC_GETREGS;
  MQ.MQ_Data.data.dl[0] = CV_MQREG_PERF_ACEL;
  MQ.MQ_Data.data.dl[1] = CV_MQREG_PERF_DESACEL;
  MQ.MQ_Data.data.dl[2] = CV_MQREG_PERF_VELMAX;

  if(MQ_Transfer(&MQ)==MQ_ERRO_NENHUM)
    {
    for(i=6; i<9; i++)
      {
      obj = lookup_widget(wdg, lista_ent[i]);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(obj), MQ.MQ_Data.data.df[i-6]);
      }
    }

// Carrega os parâmetros gerais
  MQ.MQ_Data.ndata      = 1;
  MQ.MQ_Data.funcao     = CV_MQFNC_GETREGS;
  MQ.MQ_Data.data.dl[0] = CV_MQREG_OPERACAO;

  if(MQ_Transfer(&MQ)==MQ_ERRO_NENHUM)
    {
    obj = lookup_widget(wdg, lista_ent[i]);
    lst = gtk_radio_button_get_group(GTK_RADIO_BUTTON(obj));
    while(lst)
      {
      if(!strcmp(gtk_button_get_label(GTK_BUTTON(lst->data)), opt_operacao[MQ.MQ_Data.data.dl[0]]))
        {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lst->data), TRUE);
        break;
        }

      lst = lst->next;
      }
    }
*/
  if(mainDB.res != NULL) // Somente carrega se há conexão com o DB
    {
    // Carrega clientes cadastrados
    CarregaComboClientes();

    // Carrega dados da aba de usuários
    CarregaComboUsuarios();

    // Carrega dados da aba de modelos
    CarregaComboModelos();
    }

// Carrega dados da aba de Banco de Dados
  CarregaDadosBanco();
}

void cbClienteSelecionado(GtkComboBox *combobox, gpointer user_data)
{
  gboolean estado;

  if(gtk_combo_box_get_active(combobox)) {
    estado = TRUE;
  } else {
    estado = FALSE;
  }

  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnClienteRemover")), estado);
}

void cbRemoverCliente(GtkButton *button, gpointer user_data)
{
  int id;
  char sql[100], tmp[10];
  GtkWidget *dialog;
  GtkComboBox *obj = GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbClientes"));

  if(!gtk_combo_box_get_active(obj)) // O primeiro item não pode ser excluído.
    return;

  sprintf(sql, "select ID from clientes where nome='%s'", LerComboAtivo(obj));
  DB_Execute(&mainDB, 0, sql);
  DB_GetNextRow(&mainDB, 0);
  id = atoi(DB_GetData(&mainDB, 0, 0));

  dialog = gtk_message_dialog_new (NULL,
           GTK_DIALOG_DESTROY_WITH_PARENT,
           GTK_MESSAGE_QUESTION,
           GTK_BUTTONS_YES_NO,
           "Tem certeza que deseja excluir o cliente '%s'?",
           LerComboAtivo(obj));

  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
    {
    // Substitui o cliente sendo excluído das tarefas pelo cliente "Nenhum".
    sprintf(sql, "update tarefas set ID_Cliente='1' where ID_Cliente='%d'", id);
    DB_Execute(&mainDB, 0, sql);

    sprintf(sql, "delete from clientes where nome='%s'", LerComboAtivo(obj));
    DB_Execute(&mainDB, 0, sql);

    ExcluiItemCombo(GTK_COMBO_BOX(obj), gtk_combo_box_get_active(GTK_COMBO_BOX(obj)));
    gtk_combo_box_set_active(GTK_COMBO_BOX(obj), 0);

    sprintf(sql, "Removendo cliente '%s'", LerComboAtivo(obj));
    Log(sql, LOG_TIPO_CONFIG);

    MessageBox("Cliente removido com sucesso!");
    }

  gtk_widget_destroy (dialog);
}

void cbLoginUserSelected(GtkComboBox *combobox, gpointer user_data)
{
  char *lst_campos[] =
    {
      "entNome"    , "nome",
      "entSenha"   , "senha",
      "entLembrete", "lembrete",
      ""           , ""
    };

  char *lst_botoes[] = { "btnExcluir", "btnPerm", "" };

  if(gtk_combo_box_get_active(combobox)>=0)
    CarregaCampos(combobox, lst_campos, lst_botoes, "usuarios", "login");
}

void cbAplicarUsuario(GtkButton *button, gpointer user_data)
{
  char sql[400], *nome, *senha, *lembrete, *login;
  GtkWidget *dialog, *obj, *wnd, *entry;

  obj = GTK_WIDGET(gtk_builder_get_object(builder, "entNome"));
  nome = (char *)(gtk_entry_get_text(GTK_ENTRY(obj)));

  obj = GTK_WIDGET(gtk_builder_get_object(builder, "entSenha"));
  senha = Crypto((char *)(gtk_entry_get_text(GTK_ENTRY(obj))));

  obj = GTK_WIDGET(gtk_builder_get_object(builder, "entLembrete"));
  lembrete = (char *)(gtk_entry_get_text(GTK_ENTRY(obj)));

  obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbCadUserLogin"));
  if(gtk_combo_box_get_active(GTK_COMBO_BOX(obj)) != 0) // Não é o primeiro item, alterar usuário.
    {
    dialog = gtk_message_dialog_new (NULL,
              GTK_DIALOG_DESTROY_WITH_PARENT,
              GTK_MESSAGE_QUESTION,
              GTK_BUTTONS_YES_NO,
              "Aplicar as alterações ao usuário '%s'?",
              LerComboAtivo(GTK_COMBO_BOX(obj)));

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
      {
      sprintf(sql, "update usuarios set nome='%s', senha='%s', lembrete='%s' where login='%s'",
        nome, senha, lembrete, LerComboAtivo(GTK_COMBO_BOX(obj)));
      DB_Execute(&mainDB, 0, sql);

      sprintf(sql, "Alterado o usuário %s", LerComboAtivo(GTK_COMBO_BOX(obj)));
      Log(sql, LOG_TIPO_SISTEMA);

      MessageBox("Usuário alterado com sucesso!");
      }
    }
  else
    {
    wnd = GTK_WIDGET(gtk_builder_get_object(builder, "wndConfig"));
    dialog = gtk_dialog_new_with_buttons ("Digite o login do usuário:",
          GTK_WINDOW(wnd),
          GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL | GTK_DIALOG_NO_SEPARATOR,
          GTK_STOCK_OK,
          GTK_RESPONSE_OK,
          GTK_STOCK_CANCEL,
          GTK_RESPONSE_CANCEL,
          NULL);

    // TODO: Ler do banco de dados o tamanho do campo de login.
    entry = gtk_entry_new_with_max_length(10);
    gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), entry);
    AbrirJanelaModal(dialog);

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
      {
      login = (char *)(gtk_entry_get_text(GTK_ENTRY(entry)));
      sprintf(sql, "select ID from usuarios where login='%s'", login);
      DB_Execute(&mainDB, 0, sql);
      if(DB_GetNextRow(&mainDB, 0)>0)
        {
        wnd = gtk_message_dialog_new (NULL,
                  GTK_DIALOG_DESTROY_WITH_PARENT,
                  GTK_MESSAGE_ERROR,
                  GTK_BUTTONS_OK,
                  "O usuário '%s' já existe!",
                  login);

        gtk_dialog_run(GTK_DIALOG(wnd));
        gtk_widget_destroy (wnd);
        }
      else // O usuário não existe. Realizando inserção.
        {
        sprintf(sql, "insert into usuarios (nome, senha, lembrete, login) "
               "values ('%s', '%s', '%s', '%s')",
               nome, senha, lembrete, login);
        DB_Execute(&mainDB, 0, sql);

        CarregaItemCombo(GTK_COMBO_BOX(obj), login);
        gtk_combo_box_set_active(GTK_COMBO_BOX(obj), 0);
        cbLoginUserSelected(GTK_COMBO_BOX(obj), NULL);

        sprintf(sql, "Adicionado o usuário %s", login);
        Log(sql, LOG_TIPO_SISTEMA);

        MessageBox("Usuário adicionado com sucesso!");
        }
      }
    }

  gtk_widget_destroy (dialog);
}

void cbExcluirUsuario(GtkButton *button, gpointer user_data)
{
  int id;
  char sql[100], *ativo;
  GtkWidget *dialog, *obj;

  obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbCadUserLogin"));
  if(gtk_combo_box_get_active(GTK_COMBO_BOX(obj)) != 0) // Não é o primeiro item, exclui usuário.
    {
    ativo = LerComboAtivo(GTK_COMBO_BOX(obj));
    sprintf(sql, "select ID from usuarios where login='%s'", ativo);
    DB_Execute(&mainDB, 0 ,sql);
    id = atoi(DB_GetData(&mainDB, 0, 0));
    if(id == idUser) // Tentando remover usuário atual!
      {
      sprintf(sql, "O usuário '%s' é o usuário atual, não pode ser excluído!", ativo);
      MessageBox(sql);
      return; // Não pode excluir o usuário atual. Retorna!
      }

    dialog = gtk_message_dialog_new (NULL,
              GTK_DIALOG_DESTROY_WITH_PARENT,
              GTK_MESSAGE_QUESTION,
              GTK_BUTTONS_YES_NO,
              "Tem certeza que deseja excluir o usuário '%s'?",
              ativo);

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
      {
      sprintf(sql, "Removendo usuário %s", ativo);
      Log(sql, LOG_TIPO_SISTEMA);

      sprintf(sql, "delete from usuarios where login='%s'", ativo);
      DB_Execute(&mainDB, 0, sql);
      ExcluiItemCombo(GTK_COMBO_BOX(obj), gtk_combo_box_get_active(GTK_COMBO_BOX(obj)));
      gtk_combo_box_set_active(GTK_COMBO_BOX(obj), 0);

      MessageBox("Usuário removido com sucesso!");
      }

    gtk_widget_destroy (dialog);
    }
}

void SaindoUserPerms(gpointer user_data)
{
  guint signal_id;
  gulong handler_id;
  GtkWidget *wdg;
  GtkContainer *container = GTK_CONTAINER(gtk_builder_get_object(builder, "tblPerms"));
  GList *start, *lst = gtk_container_get_children(container);

  start = lst;
  while(lst) {
    gtk_container_remove(container, GTK_WIDGET(lst->data));
    lst = lst->next;
  }

  signal_id = g_signal_lookup("clicked", GTK_TYPE_BUTTON);

  wdg = GTK_WIDGET(gtk_builder_get_object(builder, "btnUserPermsOK"));
  handler_id = g_signal_handler_find((gpointer)wdg, G_SIGNAL_MATCH_ID, signal_id, 0, NULL, NULL, NULL);
  g_signal_handler_disconnect((gpointer)wdg, handler_id);

  wdg = GTK_WIDGET(gtk_builder_get_object(builder, "btnUserPermsCancel"));
  handler_id = g_signal_handler_find((gpointer)wdg, G_SIGNAL_MATCH_ID, signal_id, 0, NULL, NULL, NULL);
  g_signal_handler_disconnect((gpointer)wdg, handler_id);

  gtk_grab_remove(GTK_WIDGET(gtk_builder_get_object(builder, "wndUserPerms")));
  free((int *)(user_data));
  g_list_free(start);
}

void cbUserPermsCancel(GtkButton *button, gpointer user_data)
{
  gtk_widget_hide_all(GTK_WIDGET(gtk_builder_get_object(builder, "wndUserPerms")));
  SaindoUserPerms(user_data);
}

void cbUserPermsAtual(GtkButton *button, gpointer user_data)
{
  int i=0, val_perm, inserir = 0;
  char sql[100];
  GList *lst = gtk_container_get_children(GTK_CONTAINER(gtk_builder_get_object(builder, "tblPerms")));

  sprintf(sql,"delete from permissoes where ID_User=%d", *(int *)(user_data));
  DB_Execute(&mainDB, 0, sql);

  while(lst)
    {
    if(GTK_IS_ENTRY(lst->data))
      {
      inserir = 1;
      val_perm = atoi(gtk_entry_get_text(GTK_ENTRY(lst->data)));
      }
    else if(GTK_IS_CHECK_BUTTON(lst->data))
      {
      inserir = 1;
      val_perm = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lst->data));
      }

    if(inserir)
      {
      sprintf(sql,"insert into permissoes values (%d, %d, %d)",
        *(int *)(user_data),
        *((int *)(user_data)+i+1),
        val_perm);
      DB_Execute(&mainDB, 0, sql);

      i++;
      inserir = 0;
      }

    lst = lst->next;
    }

  gtk_widget_hide_all(GTK_WIDGET(gtk_builder_get_object(builder, "wndUserPerms")));
  SaindoUserPerms(user_data);
}

void cbAbrirUserPerms(GtkButton *button, gpointer user_data)
{
  char sql[100], tmp[30];
  int i=0, tam, id_user, val_perm;
  GtkWidget *wnd, *tbl, *wdg, *combo;
// Ponteiro para uma lista de IDs para a futura inserção / alteração dos dados no banco.
  int *ptrIDs;

  wnd = GTK_WIDGET(gtk_builder_get_object(builder, "wndUserPerms"));
  tbl = GTK_WIDGET(gtk_builder_get_object(builder, "tblPerms"));

  combo = GTK_WIDGET(gtk_builder_get_object(builder, "cmbCadUserLogin"));
  sprintf(sql, "select ID from usuarios where login='%s'", LerComboAtivo(GTK_COMBO_BOX(combo)));

  DB_Execute(&mainDB, 0, sql);
  DB_GetNextRow(&mainDB, 0);

  id_user = atoi((char *)DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "ID")));

  DB_Execute(&mainDB, 0, "select ID, descricao, valor, tipo from lista_permissoes order by ID");
  DB_GetNextRow(&mainDB, 0);

  tam = DB_GetCount(&mainDB, 0);

  if(tam>0)
    {
    // Aloca o ponteiro para os IDs.
    // Deve ser desalocado pela função de callback do sinal destroy da janela.
    ptrIDs = (int *)(malloc(sizeof(int)*(tam+1)));
    gtk_table_resize(GTK_TABLE(tbl), tam, 2);

// Conexão dos sinais de callback
    wdg = GTK_WIDGET(gtk_builder_get_object(builder, "btnUserPermsOK"));
    g_signal_connect ((gpointer) wdg, "clicked",
      G_CALLBACK (cbUserPermsAtual ), (gpointer)(ptrIDs));

    wdg = GTK_WIDGET(gtk_builder_get_object(builder, "btnUserPermsCancel"));
    g_signal_connect ((gpointer) wdg, "clicked",
      G_CALLBACK (cbUserPermsCancel), (gpointer)(ptrIDs));

// A primeira posicao contém o ID do usuário.
    *ptrIDs = id_user;

    do
      {
      *(ptrIDs+tam-i) = atoi((char *)DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "ID")));

      Asc2Utf(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "descricao")), tmp);
      wdg = gtk_label_new(tmp);
      gtk_table_attach_defaults(GTK_TABLE(tbl), wdg, 0, 1, i, i+1);

      sprintf(sql, "select valor from permissoes where ID_user=%d and ID_perm=%d", id_user, *(ptrIDs+tam-i));
      DB_Execute(&mainDB, 1, sql);
      if(DB_GetNextRow(&mainDB, 1)>0) // Carrega o valor da permissão para este usuário
        strcpy(tmp, DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "valor")));
      else // A permissão do usuário não existe. Carrega o valor default da permissão
        strcpy(tmp, DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "valor")));

      switch(atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "tipo"))))
        {
        case PERM_TIPO_BOOL:
          val_perm = atoi(tmp);
          wdg      = gtk_check_button_new();
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wdg), val_perm);

          break;

        default:
        case PERM_TIPO_INT:
          wdg = gtk_entry_new_with_max_length(5); // Tamanho de um inteiro
          gtk_entry_set_text(GTK_ENTRY(wdg), tmp);

          break;
        }

      gtk_table_attach_defaults(GTK_TABLE(tbl), wdg, 1, 2, i, i+1);

      i++;
      } while(DB_GetNextRow(&mainDB, 0)>0);

    AbrirJanelaModal(wnd);
    gtk_grab_add(wnd);
    }
}

gboolean ChecarModelo(int passo, int tam_max)
{
  char *msg = NULL;
  GtkWidget *dlg;

  if(passo <= 0)
    msg = "O passo tem que ser maior que zero!";
  if(tam_max < passo)
    msg = "O Tamanho máximo deve ser maior que o passo!";

  if(msg==NULL)
    return TRUE;

  dlg = gtk_message_dialog_new (NULL,
           GTK_DIALOG_DESTROY_WITH_PARENT,
           GTK_MESSAGE_ERROR,
           GTK_BUTTONS_OK,
           "%s",
           msg
           );

  gtk_dialog_run(GTK_DIALOG(dlg));
  gtk_widget_destroy (dlg);

  return FALSE;
}

void cbConfigModeloSelecionado(GtkComboBox *combobox, gpointer user_data)
{
  GSList *lst;
  int pilotar;

  char *lst_campos[] =
    {
      "entModPasso" , "passo",
      "entModTamMax", "tam_max",
      "entModTamMin", "tam_min",
      ""            , ""
    };

  char *lst_botoes[] = { "btnModRemover", "" }, *opt_piloto[] = { "Não", "Sim" };

  if(gtk_combo_box_get_active(combobox)<0)
    return; // Sem item ativo.

  if(CarregaCampos(combobox, lst_campos, lst_botoes, "modelos", "nome"))
    pilotar = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "pilotar")));
  else
    pilotar = 0;

  lst = gtk_radio_button_get_group(GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "rbtModPilotarSim")));
  while(lst)
    {
    if(!strcmp(gtk_button_get_label(GTK_BUTTON(lst->data)), opt_piloto[pilotar]))
      {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lst->data), TRUE);
      break;
      }

    lst = lst->next;
    }
}

void cbAplicarModelo(GtkButton *button, gpointer user_data)
{
  int i;
  char sql[400], *nome;

  char *valores[30] = { "", "", "", "" }, *opt_piloto[] = { "Não", "Sim", "" };
  char *campos[] =
    {
    "cmbModNome",
    "rbtModPilotarSim",
    "entModPasso",
    "entModTamMax",
    "entModTamMin",
    ""
    };

  GtkWidget *dialog, *obj, *wnd, *entry;

  if(!LerValoresWidgets(campos, valores))
    return; // Ocorreu algum erro lendo os campos. Retorna.

// Checa se os dados são válidos.
  if(!ChecarModelo(atol(valores[2]), atol(valores[3])))
    return;

  obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbModNome"));
  if(gtk_combo_box_get_active(GTK_COMBO_BOX(obj)) != 0) // Não é o primeiro item, alterar usuário.
    {
    dialog = gtk_message_dialog_new (NULL,
              GTK_DIALOG_DESTROY_WITH_PARENT,
              GTK_MESSAGE_QUESTION,
              GTK_BUTTONS_YES_NO,
              "Aplicar as alterações ao modelo '%s'?",
              valores[0]);

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
      {
      sprintf(sql, "update modelos set pilotar='%d', passo='%s', tam_max='%s', tam_min='%s' where nome='%s'",
        BuscaStringLista(opt_piloto, valores[1], FALSE), valores[2], valores[3], valores[4], valores[0]);
      DB_Execute(&mainDB, 0, sql);

      gtk_combo_box_set_active(GTK_COMBO_BOX(obj), 0);

      sprintf(sql, "Alterado modelo %s", valores[0]);
      Log(sql, LOG_TIPO_CONFIG);

      MessageBox("Modelo alterado com sucesso!");
      }
    }
  else
    {
    wnd = GTK_WIDGET(gtk_builder_get_object(builder, "wndDesktop"));
    dialog = gtk_dialog_new_with_buttons ("Digite o nome do modelo:",
          GTK_WINDOW(wnd),
          GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL | GTK_DIALOG_NO_SEPARATOR,
          GTK_STOCK_OK,
          GTK_RESPONSE_OK,
          GTK_STOCK_CANCEL,
          GTK_RESPONSE_CANCEL,
          NULL);

    // TODO: Ler do banco de dados o tamanho do campo de nome.
    entry = gtk_entry_new_with_max_length(20);
    gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), entry);
    AbrirJanelaModal(dialog);

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
      {
      nome = (char *)(gtk_entry_get_text(GTK_ENTRY(entry)));
      sprintf(sql, "select ID from modelos where nome='%s'", nome);
      DB_Execute(&mainDB, 0, sql);
      if(DB_GetNextRow(&mainDB, 0)>0)
        {
        wnd = gtk_message_dialog_new (NULL,
                  GTK_DIALOG_DESTROY_WITH_PARENT,
                  GTK_MESSAGE_ERROR,
                  GTK_BUTTONS_OK,
                  "O modelo '%s' já existe!",
                  nome);

        gtk_dialog_run(GTK_DIALOG(wnd));
        gtk_widget_destroy (wnd);
        }
      else // O modelo não existe. Realizando inserção.
        {
        sprintf(sql, "insert into modelos (nome, pilotar, passo, tam_max, tam_min, estado) "
               "values ('%s', '%d', '%s', '%s', '%s', '%d')",
               nome, BuscaStringLista(opt_piloto, valores[1], FALSE), valores[2], valores[3], valores[4], MOD_ESTADO_ATIVO);
        DB_Execute(&mainDB, 0, sql);

        CarregaItemCombo(GTK_COMBO_BOX(obj), nome);
        gtk_combo_box_set_active(GTK_COMBO_BOX(obj), 0);
        cbConfigModeloSelecionado(GTK_COMBO_BOX(obj), NULL);

        sprintf(sql, "Adicionado modelo %s", nome);
        Log(sql, LOG_TIPO_CONFIG);

        MessageBox("Modelo adicionado com sucesso!");
        }
      }
    }

  gtk_widget_destroy (dialog);
}

void cbRemoverModelo(GtkButton *button, gpointer user_data)
{
  char sql[100];
  GtkWidget *dialog, *obj;

  obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbModNome"));
  if(gtk_combo_box_get_active(GTK_COMBO_BOX(obj)) != 0) // Não é o primeiro item, exclui modelo.
    {
    dialog = gtk_message_dialog_new (NULL,
              GTK_DIALOG_DESTROY_WITH_PARENT,
              GTK_MESSAGE_QUESTION,
              GTK_BUTTONS_YES_NO,
              "Tem certeza que deseja excluir o modelo '%s'?",
              LerComboAtivo(GTK_COMBO_BOX(obj)));

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
      {
      sprintf(sql, "update modelos set estado='%d' where nome='%s'", MOD_ESTADO_REMOVIDO, LerComboAtivo(GTK_COMBO_BOX(obj)));
      DB_Execute(&mainDB, 0, sql);

      sprintf(sql, "Removendo modelo %s", LerComboAtivo(GTK_COMBO_BOX(obj)));
      Log(sql, LOG_TIPO_CONFIG);

      ExcluiItemCombo(GTK_COMBO_BOX(obj), gtk_combo_box_get_active(GTK_COMBO_BOX(obj)));
      gtk_combo_box_set_active(GTK_COMBO_BOX(obj), 0);

      MessageBox("Modelo removido com sucesso!");
      }

    gtk_widget_destroy (dialog);
    }
}

void AbrirConfig(unsigned int pos)
{
  GtkWidget *wnd;

  if(mainDB.res != NULL) // Banco de dados conectado!
    if(!GetUserPerm(PERM_ACESSO_CONFIG))
      {
      MessageBox("Sem permissão para acesso à configuração!");
      return;
      }

  wnd = GTK_WIDGET(gtk_builder_get_object(builder, "ntbWorkArea"));
  gtk_notebook_set_current_page(GTK_NOTEBOOK(wnd), NTB_ABA_CONFIG);

// Se pos != 0, foi chamada a janela forçadamente e portanto esta ação não pode ser cancelada!
  if(pos) {
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnConfigCancel")), FALSE);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbConfig")), pos);
  }

  LerDadosConfig();
}

void cbConfigOk(GtkButton *button, gpointer user_data)
{
// A máquina foi configurada, devemos remover o erro de configuração.
//  MQ.MQ_Data.funcao = CV_MQFNC_LIMPA_ERRO;
//  MQ.MQ_Data.data.di[0] = CV_ERRO_CONFIG;
//  MQ_Transfer(&MQ);

  Log("Alterada configuração da máquina", LOG_TIPO_CONFIG);

  GravarDadosConfig();
  gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbWorkArea")), NTB_ABA_HOME);
}

void cbConfigVoltar(GtkButton *button, gpointer user_data)
{
  gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbWorkArea")), NTB_ABA_HOME);
}

void cbLoginOk(GtkButton *button, gpointer user_data)
{
  int pos = 5; // Posição da aba de configuração do banco, iniciando de zero.
  char sql[100], *lembrete = "";
  GtkWidget *wnd;

  if(mainDB.res==NULL) // Banco não conectado!
    {
    if(!strcmp(Crypto((char *)(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entLoginSenha"))))), SENHA_MASTER)) // Senha correta
      {
      idUser=1; // Grava 1 para indicar que foi logado

// Exibe a janela de configuração na aba de configuração do banco de dados
      // Cria a janela principal
      wnd = GTK_WIDGET(gtk_builder_get_object(builder, "wndDesktop"));
      AbrirJanelaModal(wnd);
      gtk_grab_add(wnd);

      AbrirConfig(pos);

      return;
      }
    else
      lembrete = LEMBRETE_SENHA_MASTER;
    }
  else
    {
    sprintf(sql, "select senha, lembrete, ID from usuarios where login='%s'",
      LerComboAtivo(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbLoginUser"))));

    DB_Execute(&mainDB, 0, sql);
    if(DB_GetNextRow(&mainDB, 0)>0)
      if(!strcmp(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "senha")),Crypto((char *)(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entLoginSenha")))))))
        {
        // Carrega o ID do usuário que está logando.
        idUser = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "ID")));
        Log("Entrada no sistema", LOG_TIPO_SISTEMA);

        // Oculta a janela de login.
        gtk_widget_hide_all(GTK_WIDGET(gtk_builder_get_object(builder, "wndLogin")));

        // Cria a janela principal
        wnd = GTK_WIDGET(gtk_builder_get_object(builder, "wndDesktop"));

        // Configura o estado inicial dos botoes.
        // Habilita os botões conforme estado atual da máquina.
//        if(MaquinaEspera(CHECAR_MANUAL))
//          EstadoBotoes(wnd, BTN_TODOS);
//        else
//          EstadoBotoes(wnd, BTN_CFG | BTN_INIT);

        AbrirJanelaModal(wnd);
        gtk_grab_add(wnd);
        gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbWorkArea")), 0);

// Funcao para captura de mensagens do processo do Corte Voador
//        g_timeout_add(200,(GtkFunction)sync_cv, (gpointer *)(wnd));

        return;
        }
      else // Erro durante login. Gera log informando esta falha.
        {
        lembrete = DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "lembrete"));

        // Carrega Id para associar o log a este usuário
        idUser = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "ID")));

        // Insere o log de erro no login para o usuário selecionado
        Log("Erro durante login", LOG_TIPO_ERRO);

        // Volta para zero pois o usuário não logou.
        idUser = 0;
        }
    }

  // Se lembrete for nulo, seleciona para texto em branco.
  if(lembrete == NULL)
    lembrete = "";

  // Gera a string de erro utilizando a variável sql.
  sprintf(sql, "Erro durante o login! Lembrete da senha: %s", lembrete);
  MessageBox(sql);
}

void cbBancoTestar(GtkButton *button, gpointer user_data)
{
  struct strDB tmpDB;

  tmpDB.server  = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoIP"   )));
  tmpDB.user    = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoUser" )));
  tmpDB.passwd  = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoSenha")));
  tmpDB.nome_db = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoNome" )));

  if(DB_Init(&tmpDB)) // Se inicializar o banco, precisamos verificar se existem as tabelas.
    {
// DB_Execute() retorna -1 se ocorrer um erro ou zero no sucesso.
// Se algum retornar algo diferente de zero existe um erro com o banco!
    if(DB_Execute(&tmpDB, 0, "select * from usuarios") ||
       DB_Execute(&tmpDB, 1, "select * from tarefas" ) ||
       DB_Execute(&tmpDB, 2, "select * from modelos" ) ||
       DB_Execute(&tmpDB, 3, "select * from clientes"))
        MessageBox("Conexão OK mas banco de dados inválido ou com erro!");
    else
        MessageBox("Testes executados com sucesso");
    }
  else
    MessageBox("Erro conectando ao banco de dados!");

  DB_Close(&tmpDB);
}

void cbManutAtualSaida(GtkToggleButton *togglebutton, gpointer user_data)
{
  const gchar *nome = gtk_widget_get_name(GTK_WIDGET(togglebutton));
  char nome_img[30];
  union MB_FCD_Data data;
  struct MB_Reply rp;

  data.write_single_coil.output = atoi(&nome[strlen(nome)-2]);
  data.write_single_coil.val    = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));

  rp = MB_Send(&mbdev, MB_FC_WRITE_SINGLE_COIL, &data);

  sprintf(nome_img, "imgManutSai%02d", data.write_single_coil.output-1);
  gtk_image_set_from_stock(GTK_IMAGE(gtk_builder_get_object(builder, nome_img)),
      data.write_single_coil.val ? "gtk-apply" : "gtk-media-record", GTK_ICON_SIZE_BUTTON);

  if(rp.ExceptionCode != MB_EXCEPTION_NONE)
    printf("Erro escrevendo saida. Exception Code: %02x\n", rp.ExceptionCode);
}

void cbNotebookWorkAreaChanged(GtkNotebook *ntb, GtkNotebookPage *page, guint arg1, gpointer user_data)
{
  int i;
  char nome[30];

  PreviousWorkArea = CurrentWorkArea;
  CurrentWorkArea  = arg1;

  for(i=0; i<6; i++) {
    sprintf(nome, "btnF%d", i+1);
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, nome)), CurrentWorkArea<6);
  }
}

void WorkAreaGoPrevious()
{
  gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbWorkArea")), PreviousWorkArea);
}

void cbDebugModBus(GtkButton *button, gpointer user_data)
{
  gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbWorkArea")), NTB_ABA_MODBUS);
}

// função que carrega a lista de eventos em um TreeView
void CarregaListaLogs(GtkWidget *tvw)
{
  int i, tipo;
  const int tam = 3;
  char *valores[tam+1], sql[300], *data_ini, *data_fim;

  valores[tam] = NULL;

  TV_Limpar(tvw);

  tipo = gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbLogFiltro")));
  data_ini = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entLogDataInicial")));
  data_fim = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entLogDataFinal")));

  if(tipo == LOG_TIPO_TODOS)
    sprintf(sql, "select l.data, u.nome, l.evento from log as l, usuarios as u where u.ID = l.ID_Usuario and (Data between '%s' and '%s') order by data desc", data_ini, data_fim);
  else
    sprintf(sql, "select l.data, u.nome, l.evento from log as l, usuarios as u where u.ID = l.ID_Usuario and (Data between '%s' and '%s') and Tipo='%d' order by data desc", data_ini, data_fim, tipo);

  DB_Execute(&mainDB, 0, sql);

  while(DB_GetNextRow(&mainDB, 0)>0)
    {
    for(i = 0; i<tam; i++)
      {
      valores[i] = DB_GetData(&mainDB, 0, i);
      if(valores[i] == NULL)
        valores[i] = "";
      }

    TV_Adicionar(tvw, valores);
    }
}

void cbDebugModBusVoltar(GtkButton *button, gpointer user_data)
{
  gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbWorkArea")), NTB_ABA_HOME);
}

// Funcoes que aumentam/reduzem horas e minutos.
void DataAtualizaHora(int32_t inc)
{
  int32_t val;
  char tmp[10];
  GtkEntry *entry = GTK_ENTRY(gtk_builder_get_object(builder, "entDataHora"));

  val = atoi(gtk_entry_get_text(entry)) + inc;
  if(val > 23)
    val = 23;
  else if(val < 0)
    val = 0;

  sprintf(tmp, "%d", val);
  gtk_entry_set_text(entry, tmp);
}

void cbDataAumentaHora(GtkButton *button, gpointer user_data)
{
  DataAtualizaHora(+1);
}

void cbDataReduzHora(GtkButton *button, gpointer user_data)
{
  DataAtualizaHora(-1);
}

void DataAtualizaMinuto(int32_t inc)
{
  int32_t val;
  char tmp[10];
  GtkEntry *entry = GTK_ENTRY(gtk_builder_get_object(builder, "entDataMinuto"));

  val = atoi(gtk_entry_get_text(entry)) + inc;
  if(val > 59)
    val = 59;
  else if(val < 0)
    val = 0;

  sprintf(tmp, "%d", val);
  gtk_entry_set_text(entry, tmp);
}

void cbDataAumentaMinuto(GtkButton *button, gpointer user_data)
{
  DataAtualizaMinuto(+1);
}

void cbDataReduzMinuto(GtkButton *button, gpointer user_data)
{
  DataAtualizaMinuto(-1);
}

// Funcao executada quando for clicado voltar na tela de data
void cbDataVoltar(GtkButton *button, gpointer user_data)
{
  guint signal_id;
  gulong handler_id;
  GtkWidget *obj = GTK_WIDGET(gtk_builder_get_object(builder, "btnDataAplicar"));

  // Exclui o callback do botao de Aplicar
  signal_id = g_signal_lookup("clicked", GTK_TYPE_BUTTON);
  handler_id = g_signal_handler_find(obj, G_SIGNAL_MATCH_ID, signal_id, 0, NULL, NULL, NULL);
  g_signal_handler_disconnect(obj, handler_id);

  WorkAreaGoPrevious();
}

// Função que abre a janela de data, carregando a data de entry e
// associando a função cb como callback do botão de aplicar data.
void AbrirData(GtkEntry *entry, GCallback cb)
{
  gint v1, v2, v3;
  char *data, tmp[30];
  GtkWidget *obj;

// Carrega o ponteiro para o botão de aplicar tarefa.
  obj = GTK_WIDGET(gtk_builder_get_object(builder, "btnDataAplicar"));

// Conexão dos sinais de callback
  g_signal_connect ((gpointer) obj, "clicked",  cb,
              (gpointer)(entry));

// Lê a data atualmente inserida no entry box.
  data = (char *)gtk_entry_get_text(entry);

// Carrega o ano
  strncpy(tmp, data, 4);
  tmp[4] = 0;
  v1 = atoi(tmp);

// Carrega o mês
  strncpy(tmp, data+5, 2);
  tmp[2] = 0;
  v2 = atoi(tmp);

// Carrega o dia
  strncpy(tmp, data+8, 2);
  tmp[2] = 0;
  v3 = atoi(tmp);

// Carrega a data no calendário.
  obj = GTK_WIDGET(gtk_builder_get_object(builder, "cldData"));
  gtk_calendar_select_month(GTK_CALENDAR(obj), v2-1, v1);
  gtk_calendar_select_day  (GTK_CALENDAR(obj), v3);

// Carrega a hora
  strncpy(tmp, data+11, 2);
  tmp[2] = 0;
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entDataHora"  )), tmp);

// Carrega os minutos
  strncpy(tmp, data+14, 2);
  tmp[2] = 0;
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entDataMinuto")), tmp);

// Exibe a janela.
  gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbWorkArea")), NTB_ABA_DATA);
}

void cbAplicarDataLog(GtkButton *button, gpointer user_data)
{
  guint y, m, d, hora, min;
  char tmp[100];
  guint signal_id;
  gulong handler_id;

  // Exclui o callback do botao
  signal_id = g_signal_lookup("clicked", GTK_TYPE_BUTTON);
  handler_id = g_signal_handler_find(button, G_SIGNAL_MATCH_ID, signal_id, 0, NULL, NULL, NULL);
  g_signal_handler_disconnect(button, handler_id);

  gtk_calendar_get_date(GTK_CALENDAR(gtk_builder_get_object(builder, "cldData")), &y, &m, &d);
  hora = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entDataHora"  ))));
  min  = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entDataMinuto"))));

  sprintf(tmp, "%d-%02d-%02d %02d:%02d:00", y, m+1, d, hora, min);
  gtk_entry_set_text((GtkEntry *)(user_data), tmp);

  CarregaListaLogs(GTK_WIDGET(gtk_builder_get_object(builder, "tvwLog")));

  gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbWorkArea")), NTB_ABA_LOGS);
}

void cbLogSelDataInicial(GtkButton *button, gpointer user_data)
{
  AbrirData(GTK_ENTRY(gtk_builder_get_object(builder, "entLogDataInicial")), G_CALLBACK (cbAplicarDataLog));
}

void cbLogSelDataFinal(GtkButton *button, gpointer user_data)
{
  AbrirData(GTK_ENTRY(gtk_builder_get_object(builder, "entLogDataFinal")), G_CALLBACK (cbAplicarDataLog));
}

void cbLogFiltrar(GtkComboBox *combobox, gpointer user_data)
{
  CarregaListaLogs(GTK_WIDGET(gtk_builder_get_object(builder, "tvwLog")));
}

void AbrirLog()
{
  char tmp[50];
  time_t DataInicial, DataFinal;

  if(!GetUserPerm(PERM_ACESSO_LOGS))
    {
    MessageBox("Sem permissão para acesso aos registros!");
    return;
    }

  DataFinal = time(NULL);
  strftime (tmp, 100, "%Y-%m-%d %H:%M:%S", localtime(&DataFinal));
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entLogDataFinal")), tmp);

  DataInicial = DataFinal - 604800; // 604.800 segundos = 7 dias
  strftime (tmp, 100, "%Y-%m-%d %H:%M:%S", localtime(&DataInicial));
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entLogDataInicial")), tmp);

// Selecionando o item do combobox faz com que a lista de eventos seja carregada.
  gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbLogFiltro")), 0);
  CarregaListaLogs(GTK_WIDGET(gtk_builder_get_object(builder, "tvwLog")));
}

void cbLogVoltar(GtkButton *button, gpointer user_data)
{
  gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbWorkArea")), NTB_ABA_HOME);
}

void ConfigBotoesTarefa(GtkWidget *wdg, gboolean modo)
{
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnExecTarefa"   )), modo);
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnEditarTarefa" )), modo);
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnRemoverTarefa")), modo);
}

void CarregaListaTarefas(GtkWidget *tvw)
{
  int i;
  const int tam = 9;
  char *valores[tam+1], sql[300];

  valores[tam] = NULL;

  TV_Limpar(tvw);

  sprintf(sql, "select t.ID, c.nome, t.Pedido, m.nome, t.qtd, t.qtdprod, t.tamanho, t.data, t.coments from modelos as m, tarefas as t, clientes as c where c.ID = t.ID_Cliente and m.ID = t.ID_Modelo and (t.estado='%d' or t.estado='%d') and m.estado='%d' order by data", TRF_ESTADO_NOVA, TRF_ESTADO_PARCIAL, MOD_ESTADO_ATIVO);
  DB_Execute(&mainDB, 0, sql);
  while(DB_GetNextRow(&mainDB, 0)>0)
    {
    for(i = 0; i<tam; i++)
      {
      valores[i] = DB_GetData(&mainDB, 0, i);
      if(valores[i] == NULL)
        valores[i] = "";
      }

    TV_Adicionar(tvw, valores);
    }

  ConfigBotoesTarefa(tvw, FALSE);
}

gboolean ChecarTarefa(int qtd, int qtdprod, int tamanho, int passo, int max, int min)
{
  char tmp[100];
  char *msg = NULL;
  GtkWidget *dlg;

  if(qtd<1) // Quantidade deve ser ao menos 1.
    msg = "A quantidade deve ser pelo menos 1!";
  else if(qtd<qtdprod) // Quantidade deve ser maior que o total já produzido.
    {
    sprintf(tmp, "A quantidade deve ser maior que %d", qtdprod);
    msg = tmp;
    }
  else if(tamanho < min || tamanho > max)
    {
    sprintf(tmp, "O tamanho deve estar entre %d e %d", min, max);
    msg = tmp;
    }
  else if(((int)(tamanho/passo)*passo) != tamanho) // Deve ser múltiplo do passo e maior que zero.
    {
    sprintf(tmp, "O tamanho deve ser múltiplo de %d", passo);
    msg = tmp;
    }

  if(msg==NULL)
    return TRUE;

  dlg = gtk_message_dialog_new (NULL,
           GTK_DIALOG_DESTROY_WITH_PARENT,
           GTK_MESSAGE_ERROR,
           GTK_BUTTONS_OK,
           "%s",
           msg
           );

  gtk_dialog_run(GTK_DIALOG(dlg));
  gtk_widget_destroy (dlg);

  return FALSE;
}

void cbSairTarefa(GtkButton *button, gpointer user_data)
{
  char *campos[] =
    {
    "lblTarefaNumero",
    "cbeTarefaCliente",
    "cmbTarefaModNome",
    "entTarefaQtd",
    "entTarefaTam",
    "txvTarefaComent",
    "entTarefaPedido",
    ""
    };
  char *valor[] =
    {
    "",
    "Nenhum",
    "",
    "",
    "",
    "",
    "",
    "",
    };

  GravarValoresWidgets(campos, valor);

  CarregaListaTarefas(GTK_WIDGET(gtk_builder_get_object(builder, "tvwTarefas")));
  gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbWorkArea")), NTB_ABA_OPERAR);
}

void cbAplicarTarefa(GtkButton *button, gpointer user_data)
{
  int qtdprod;
  char *valores[30], sql[800], tmp[100];

  char *campos[] =
    {
    "lblTarefaNumero",
    "cbeTarefaCliente",
    "cmbTarefaModNome",
    "entTarefaQtd",
    "entTarefaTam",
    "entTarefaData",
    "txvTarefaComent",
    "entTarefaPedido",
    ""
    };

  GtkWidget *dialog;

  if(!LerValoresWidgets(campos, valores))
    return; // Ocorreu algum erro lendo os campos. Retorna.

// Carrega o ID, passo e quantidade produzida do modelo selecionado
  sprintf(sql, "select ID, passo, tam_max, tam_min from modelos where nome='%s'", valores[2]);
  DB_Execute(&mainDB, 1, sql);
  DB_GetNextRow(&mainDB, 1);

// Carrega a quantidade produzida do modelo selecionado se estiver alterando a tarefa
  if(atoi(valores[0]))
    {
    sprintf(sql, "select qtdprod from tarefas where ID='%d'", atoi(valores[0]));
    DB_Execute(&mainDB, 3, sql);
    DB_GetNextRow(&mainDB, 3);
    qtdprod = atol(DB_GetData(&mainDB, 3, 0));
    }
  else
    qtdprod = 0;

// Checa se os dados são válidos.
  if(!ChecarTarefa(atol(valores[3]), qtdprod, atol(valores[4]), atol(DB_GetData(&mainDB, 1, 1)), atol(DB_GetData(&mainDB, 1, 2)), atol(DB_GetData(&mainDB, 1, 3))))
    return;

// Carrega o ID do cliente selecionado
  sprintf(sql, "select ID from clientes where nome='%s'", valores[1]);
  DB_Execute(&mainDB, 2, sql);
  if(DB_GetNextRow(&mainDB, 2)<=0) // Cliente não existe. Adicionando...
    {
    sprintf(tmp, "insert into clientes (nome) values ('%s')", valores[1]);
    DB_Execute(&mainDB, 2, tmp);

    // Refazendo o select do cliente.
    DB_Execute(&mainDB, 2, sql);
    DB_GetNextRow(&mainDB, 2);
    }

  if(atoi(valores[0]) != 0)
    {
    dialog = gtk_message_dialog_new (NULL,
              GTK_DIALOG_DESTROY_WITH_PARENT,
              GTK_MESSAGE_QUESTION,
              GTK_BUTTONS_YES_NO,
              "Aplicar as alterações a esta tarefa?"
              );

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
      {
      sprintf(sql, "update tarefas set ID_Cliente='%d', Pedido='%s', ID_Modelo='%d', qtd='%d', tamanho='%f', "
            "data='%s', coments='%s' where ID='%d'",
            atoi(DB_GetData(&mainDB, 2, 0)), valores[7], atoi(DB_GetData(&mainDB, 1, 0)), atoi(valores[3]),
            atof(valores[4]), valores[5], valores[6], atoi(valores[0]));

      DB_Execute(&mainDB, 0, sql);
      }

    gtk_widget_destroy (dialog);
    }
  else
    {
    sprintf(sql, "insert into tarefas (ID_Cliente, Pedido, ID_Modelo, qtd, tamanho, data, coments, ID_User, Origem) "
          "values ('%d', '%s', '%d', '%d', '%f', '%s', '%s', '%d', '%d')",
          atoi(DB_GetData(&mainDB, 2, 0)), valores[7], atoi(DB_GetData(&mainDB, 1, 0)), atoi(valores[3]),
          atof(valores[4]), valores[5], valores[6], idUser, TRF_ORIGEM_MANUAL);

    DB_Execute(&mainDB, 0, sql);
    }

  cbSairTarefa(NULL, NULL);
}

void CarregaComboModelosTarefa(GtkComboBox *cmb)
{
  char sql[100];
  GtkTreeIter iter;
  GtkListStore *store = GTK_LIST_STORE(gtk_combo_box_get_model(cmb));

  sprintf(sql, "select nome, passo, tam_max, tam_min from modelos where estado='%d' order by ID", MOD_ESTADO_ATIVO);
  DB_Execute(&mainDB, 0, sql);

  gtk_list_store_clear(store);

  while(DB_GetNextRow(&mainDB, 0)>0)
    {
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
        0, DB_GetData(&mainDB, 0, 0),
        1, DB_GetData(&mainDB, 0, 1),
        2, DB_GetData(&mainDB, 0, 2),
        3, DB_GetData(&mainDB, 0, 3),
        -1);
    }

  gtk_combo_box_set_wrap_width(cmb,4);
  gtk_combo_box_set_active(cmb, 0);
}

void cbAdicionarTarefa(GtkButton *button, gpointer user_data)
{
  char tmp[100];
  time_t agora;

  CarregaComboModelosTarefa(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbTarefaModNome")));

  DB_Execute(&mainDB, 0, "select nome from clientes order by ID");
  CarregaCombo(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cbeTarefaCliente")), 0, NULL);

  agora = time(NULL);
  strftime (tmp, 100, "%Y-%m-%d %H:%M:%S", localtime(&agora));
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entTarefaData")), tmp);

  gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbWorkArea")), NTB_ABA_TAREFA);
}

void cbEditarTarefa(GtkButton *button, gpointer user_data)
{
  int i;
  GtkWidget *obj;
  char **valor, tmp[30], *campos[] = { "lblTarefaNumero", "cbeTarefaCliente", "entTarefaPedido", "cmbTarefaModNome", "entTarefaQtd", "0", "entTarefaTam", "entTarefaData", "txvTarefaComent", "" };

  DB_Execute(&mainDB, 0, "select nome from clientes order by ID");
  CarregaCombo(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cbeTarefaCliente")), 0, NULL);

  CarregaComboModelosTarefa(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbTarefaModNome")));

// Carrega os dados da tarefa selecionada
  obj = GTK_WIDGET(gtk_builder_get_object(builder, "tvwTarefas"));

// Diminui em 1 o número de campos devido ao elemento vazio no final.
  i=sizeof(campos)/sizeof(campos[0])-1;
  valor = (char **)(malloc(i * sizeof(char *)));
  while(i>0)
    {
    i--;
    if(strcmp(campos[i], "0")) // Se campo for diferente de zero devemos considerá-lo.
      {
      TV_GetSelected(obj, i, tmp);
      valor[i] = (char *)(malloc(strlen(tmp)+1));
      strcpy(valor[i], tmp);
      }
    else
      valor[i] = NULL;
    }

  GravarValoresWidgets(campos, valor);

// Desaloca a memória em valor.
  i=sizeof(campos)/sizeof(campos[0])-1;
  while(i--)
    free(valor[i]);

  free(valor);

  gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbWorkArea")), NTB_ABA_TAREFA);
}

void cbRemoverTarefa(GtkButton *button, gpointer user_data)
{
  char sql[100], tmp[10];
  GtkWidget *dialog;

  TV_GetSelected(GTK_WIDGET(gtk_builder_get_object(builder, "tvwTarefas")), 0, tmp);

  dialog = gtk_message_dialog_new (NULL,
           GTK_DIALOG_DESTROY_WITH_PARENT,
           GTK_MESSAGE_QUESTION,
           GTK_BUTTONS_YES_NO,
           "Tem certeza que deseja excluir a tarefa %d ?",
           atoi(tmp));

  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
    {
    sprintf(sql, "update tarefas set estado='%d' where ID='%d'", TRF_ESTADO_REMOVIDA, atoi(tmp));
    DB_Execute(&mainDB, 0, sql);

    // Recarrega a lista de tarefas.
    CarregaListaTarefas(GTK_WIDGET(gtk_builder_get_object(builder, "tvwTarefas")));
    }

  gtk_widget_destroy (dialog);
}

void cbAplicarDataTarefa(GtkButton *button, gpointer user_data)
{
  guint y, m, d, hora, min;
  char tmp[100];
  guint signal_id;
  gulong handler_id;

  // Exclui o callback do botao
  signal_id = g_signal_lookup("clicked", GTK_TYPE_BUTTON);
  handler_id = g_signal_handler_find(button, G_SIGNAL_MATCH_ID, signal_id, 0, NULL, NULL, NULL);
  g_signal_handler_disconnect(button, handler_id);

  gtk_calendar_get_date(GTK_CALENDAR(gtk_builder_get_object(builder, "cldData")), &y, &m, &d);
  hora = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entDataHora"  ))));
  min  = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entDataMinuto"))));

  sprintf(tmp, "%d-%02d-%02d %02d:%02d:00", y, m+1, d, hora, min);
  gtk_entry_set_text((GtkEntry *)(user_data), tmp);

  gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbWorkArea")), NTB_ABA_TAREFA);
}

void cbSelecDataTarefa(GtkButton *button, gpointer user_data)
{
  AbrirData(GTK_ENTRY(gtk_builder_get_object(builder, "entTarefaData")), G_CALLBACK (cbAplicarDataTarefa));
}

void cbTarefaSelecionada(GtkTreeView *treeview, gpointer user_data)
{
  ConfigBotoesTarefa(GTK_WIDGET(treeview), TRUE);
}

void cbTarefaModeloSelec(GtkComboBox *combobox, gpointer user_data)
{
  GtkWidget *wdg;
  char sql[100], strval[100];

  if(gtk_combo_box_get_active(combobox)<0)
    return;

  sprintf(sql, "select passo, tam_max, tam_min from modelos where nome='%s'", LerComboAtivo(combobox));
  DB_Execute(&mainDB, 0, sql);
  DB_GetNextRow(&mainDB, 0);

  sprintf(strval, "%ld mm", atol(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "passo"))));

  wdg = GTK_WIDGET(gtk_builder_get_object(builder, "lblTarefaPasso"));
  gtk_label_set_text(GTK_LABEL(wdg), strval);

  sprintf(strval, "Maior que %ld mm e menor que %ld mm",
        atol(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "tam_min"))),
        atol(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "tam_max"))));

  wdg = GTK_WIDGET(gtk_builder_get_object(builder, "lblTarefaLimites"));
  gtk_label_set_text(GTK_LABEL(wdg), strval);
}

void AbrirOper()
{
  if(!GetUserPerm(PERM_ACESSO_OPER))
    {
    MessageBox("Sem permissão para operar a máquina!");
    return;
    }

  CarregaListaTarefas(GTK_WIDGET(gtk_builder_get_object(builder, "tvwTarefas")));
}

#endif
