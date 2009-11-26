// Para que a funcao atof funcione corretamente
#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include "defines.h"
#include "GtkUtils.h"

/*** Funcoes e variáveis de suporte ***/

int idUser=0; // Indica usuário que logou se for diferente de zero.

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

void ConfigBotoesTarefa(GtkWidget *wdg, gboolean modo)
{
  gtk_widget_set_sensitive(lookup_widget(wdg, "btnExecTarefa"  ), modo);
  gtk_widget_set_sensitive(lookup_widget(wdg, "btnEditarTarefa"), modo);
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

// função que carrega a lista de eventos em um TreeView
void CarregaListaLogs(GtkWidget *tvw)
{
  int i, tipo;
  const int tam = 3;
  char *valores[tam+1], sql[300], *data_ini, *data_fim;

  valores[tam] = NULL;

  TV_Limpar(tvw);

  tipo = gtk_combo_box_get_active(GTK_COMBO_BOX(lookup_widget(GTK_WIDGET(tvw), "cmbLogFiltro")));
  data_ini = (char *)gtk_entry_get_text(GTK_ENTRY(lookup_widget(GTK_WIDGET(tvw), "entLogDataInicial")));
  data_fim = (char *)gtk_entry_get_text(GTK_ENTRY(lookup_widget(GTK_WIDGET(tvw), "entLogDataFinal")));

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

// Função que abre a janela de data, carregando a data de entry e
// associando a função cb como callback do botão de aplicar data.
void AbrirData(GtkEntry *entry, GCallback cb)
{
  gint v1, v2, v3;
  char *data, tmp[30];
  GtkWidget *obj, *wnd = create_wndData();

// Carrega o ponteiro para o botão de aplicar tarefa.
  obj = lookup_widget(wnd, "btnDataAplicar");

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
  obj = lookup_widget(wnd, "cldData");
  gtk_calendar_select_month(GTK_CALENDAR(obj), v2-1, v1);
  gtk_calendar_select_day  (GTK_CALENDAR(obj), v3);

// Carrega a hora
  strncpy(tmp, data+11, 2);
  tmp[2] = 0;
  v1 = atoi(tmp);

// Carrega os minutos
  strncpy(tmp, data+14, 2);
  tmp[2] = 0;
  v2 = atoi(tmp);

// Carrega a hora nos spins buttons
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(wnd, "spbHora")), v1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(wnd, "spbMin" )), v2);

// Exibe a janela.
  AbrirJanelaModal(wnd);
  gtk_grab_add(wnd);
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
AbrirOper                              (GtkButton       *button,
                                        gpointer         user_data)
{
  char *campos[] = { "Número", "Cliente", "Pedido", "Modelo", "Total", "Produzidas", "Tamanho", "Data", "Comentários", "" };
  GtkWidget *wnd, *tvw;

  if(!GetUserPerm(PERM_ACESSO_OPER))
    {
    MessageBox("Sem permissão para operar a máquina!");
    return;
    }

  wnd = (GtkWidget *)(create_wndOperacao());
  tvw = lookup_widget(wnd, "tvwTarefas");

  TV_Config(tvw, campos, GTK_TREE_MODEL(gtk_list_store_new((sizeof(campos)/sizeof(campos[0]))-1, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING)));

  CarregaListaTarefas(tvw);

  AbrirJanelaModal(wnd);
  gtk_grab_add(wnd);
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
ModeloSelecionado                      (GtkComboBox     *combobox,
                                        gpointer         user_data)
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

  if(CarregaCampos(combobox, lst_campos, lst_botoes, "modelos", "nome"))
    pilotar = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "pilotar")));
  else
    pilotar = 0;

  lst = gtk_radio_button_get_group(GTK_RADIO_BUTTON(lookup_widget(GTK_WIDGET(combobox), "rbtModPilotarSim")));
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

void
AplicarModelo                          (GtkButton       *button,
                                        gpointer         user_data)
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

  if(!LerValoresWidgets(GTK_WIDGET(button), campos, valores))
    return; // Ocorreu algum erro lendo os campos. Retorna.

// Checa se os dados são válidos.
  if(!ChecarModelo(atol(valores[2]), atol(valores[3])))
    return;

  obj = lookup_widget(GTK_WIDGET(button), "cmbModNome");
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

      sprintf(sql, "Alterado modelo '%s'", LerComboAtivo(obj));
      Log(sql, LOG_TIPO_CONFIG);

      MessageBox("Modelo alterado com sucesso!");
      }
    }
  else
    {
    wnd = lookup_widget(GTK_WIDGET(button), "wndConfig");
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
               "values ('%s', '%d', '%s', '%s', '%s')",
               nome, BuscaStringLista(opt_piloto, valores[1], FALSE), valores[2], valores[3], valores[4], MOD_ESTADO_ATIVO);
        DB_Execute(&mainDB, 0, sql);

        CarregaItemCombo(GTK_COMBO_BOX(obj), nome);
        gtk_combo_box_set_active(GTK_COMBO_BOX(obj), 0);
        ModeloSelecionado(GTK_COMBO_BOX(obj), NULL);

        sprintf(sql, "Adicionado modelo '%s'", LerComboAtivo(obj));
        Log(sql, LOG_TIPO_CONFIG);

        MessageBox("Modelo adicionado com sucesso!");
        }
      }
    }

  gtk_widget_destroy (dialog);
}

void
RemoverModelo                          (GtkButton       *button,
                                        gpointer         user_data)
{
  char sql[100];
  GtkWidget *dialog, *obj;

  obj = lookup_widget(GTK_WIDGET(button), "cmbModNome");
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
      ExcluiItemCombo(GTK_COMBO_BOX(obj), gtk_combo_box_get_active(GTK_COMBO_BOX(obj)));
      gtk_combo_box_set_active(GTK_COMBO_BOX(obj), 0);

      sprintf(sql, "Removendo modelo '%s'", LerComboAtivo(GTK_COMBO_BOX(obj)));
      Log(sql, LOG_TIPO_CONFIG);

      MessageBox("Modelo removido com sucesso!");
      }

    gtk_widget_destroy (dialog);
    }
}

GtkWidget *ptr_tosco = NULL;

void SaindoTarefa (GtkObject *object, gpointer user_data)
{
  gtk_grab_remove(GTK_WIDGET(object));
  if(ptr_tosco)
    {
    CarregaListaTarefas(ptr_tosco);
    ptr_tosco = NULL;
    }

  if(user_data != NULL)
    free((int *)(user_data));
}

void
AplicarTarefa                          (GtkButton       *button,
                                        gpointer         user_data)
{
  int qtdprod;
  char *valores[30], sql[800], tmp[100];

  char *campos[] =
    {
    "cbeTarefaCliente",
    "cmbModNome",
    "entTarefaQtd",
    "entTarefaTam",
    "entTarefaData",
    "txvTarefaComent",
    "entTarefaPedido",
    ""
    };

  GtkWidget *dialog;

  if(!LerValoresWidgets(GTK_WIDGET(button), campos, valores))
    return; // Ocorreu algum erro lendo os campos. Retorna.

// Carrega o ID, passo e quantidade produzida do modelo selecionado
  sprintf(sql, "select ID, passo, tam_max, tam_min from modelos where nome='%s'", valores[1]);
  DB_Execute(&mainDB, 1, sql);
  DB_GetNextRow(&mainDB, 1);

// Carrega a quantidade produzida do modelo selecionado se estiver alterando a tarefa
  if(user_data != NULL)
    {
    sprintf(sql, "select qtdprod from tarefas where ID='%d'", *(int*)(user_data));
    DB_Execute(&mainDB, 3, sql);
    DB_GetNextRow(&mainDB, 3);
    qtdprod = atol(DB_GetData(&mainDB, 3, 0));
    }
  else
    qtdprod = 0;

// Checa se os dados são válidos.
  if(!ChecarTarefa(atol(valores[2]), qtdprod, atol(valores[3]), atol(DB_GetData(&mainDB, 1, 1)), atol(DB_GetData(&mainDB, 1, 2)), atol(DB_GetData(&mainDB, 1, 3))))
    return;

// Carrega o ID do cliente selecionado
  sprintf(sql, "select ID from clientes where nome='%s'", valores[0]);
  DB_Execute(&mainDB, 2, sql);
  if(DB_GetNextRow(&mainDB, 2)<=0) // Cliente não existe. Adicionando...
    {
    sprintf(tmp, "insert into clientes (nome) values ('%s')", valores[0]);
    DB_Execute(&mainDB, 2, tmp);

    // Refazendo o select do cliente.
    DB_Execute(&mainDB, 2, sql);
    DB_GetNextRow(&mainDB, 2);
    }

  if(user_data != NULL)
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
            atoi(DB_GetData(&mainDB, 2, 0)), valores[6], atoi(DB_GetData(&mainDB, 1, 0)), atoi(valores[2]),
            atof(valores[3]), valores[4], valores[5], *(int*)(user_data));

      DB_Execute(&mainDB, 0, sql);
      }

    gtk_widget_destroy (dialog);
    }
  else
    {
    sprintf(sql, "insert into tarefas (ID_Cliente, Pedido, ID_Modelo, qtd, tamanho, data, coments, ID_User, Origem) "
          "values ('%d', '%s', '%d', '%d', '%f', '%s', '%s', '%d', '%d')",
          atoi(DB_GetData(&mainDB, 2, 0)), valores[6], atoi(DB_GetData(&mainDB, 1, 0)), atoi(valores[2]),
          atof(valores[3]), valores[4], valores[5], idUser, TRF_ORIGEM_MANUAL);

    DB_Execute(&mainDB, 0, sql);
    }

  gtk_widget_destroy(lookup_widget(GTK_WIDGET(button), "wndTarefa"));
}

void CarregaComboModelosTarefa(GtkComboBox *cmb)
{
  char sql[100];
  GtkTreeIter iter;
  GtkListStore *store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

  sprintf(sql, "select nome, passo, tam_max, tam_min from modelos where estado='%d' order by ID", MOD_ESTADO_ATIVO);
  DB_Execute(&mainDB, 0, sql);

  gtk_combo_box_set_model(cmb, GTK_TREE_MODEL(store));

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

void
AdicionarTarefa                        (GtkButton       *button,
                                        gpointer         user_data)
{
  char tmp[100];
  time_t agora;
  GtkWidget *wnd, *obj;

  wnd = create_wndTarefa();

  ptr_tosco = lookup_widget(GTK_WIDGET(button), "tvwTarefas");

  obj = lookup_widget(wnd, "cmbModNome");
  CarregaComboModelosTarefa(GTK_COMBO_BOX(obj));

  obj = lookup_widget(wnd, "cbeTarefaCliente");
  DB_Execute(&mainDB, 0, "select nome from clientes order by ID");
  CarregaCombo(GTK_COMBO_BOX(obj), 0, NULL);

// Carrega o ponteiro para o botão de aplicar tarefa.
  obj = lookup_widget(wnd, "btnTarefaAplicar");

// Conexão dos sinais de callback
  g_signal_connect ((gpointer) wnd, "destroy",
    G_CALLBACK (SaindoTarefa), NULL);
  g_signal_connect ((gpointer) obj, "clicked",
    G_CALLBACK (AplicarTarefa), NULL);

  agora = time(NULL);
  strftime (tmp, 100, "%Y-%m-%d %H:%M:%S", localtime(&agora));
  gtk_entry_set_text(GTK_ENTRY(lookup_widget(wnd, "entTarefaData")), tmp);

  AbrirJanelaModal(wnd);
  gtk_grab_add(wnd);
}


void
EditarTarefa                           (GtkButton       *button,
                                        gpointer         user_data)
{
  int *ptrID, i;
  GtkWidget *wnd, *obj;
  char **valor, tmp[30], *campos[] = { "cbeTarefaCliente", "entTarefaPedido", "cmbModNome", "entTarefaQtd", "0", "entTarefaTam", "entTarefaData", "txvTarefaComent", "" };

  wnd = create_wndTarefa();

  obj = lookup_widget(wnd, "cbeTarefaCliente");
  DB_Execute(&mainDB, 0, "select nome from clientes order by ID");
  CarregaCombo(GTK_COMBO_BOX(obj), 0, NULL);

  obj = lookup_widget(wnd, "cmbModNome");
  DB_Execute(&mainDB, 0, "select nome from modelos");
  CarregaCombo(GTK_COMBO_BOX(obj), 0, NULL);

// Aloca o ponteiro para os IDs.
// Deve ser desalocado pela função de callback do sinal destroy da janela.
  ptrID = (int *)(malloc(sizeof(int)));

// Carrega os dados da tarefa selecionada
  obj = lookup_widget(GTK_WIDGET(button), "tvwTarefas");
  ptr_tosco = obj;

  TV_GetSelected(GTK_TREE_VIEW(obj), 0, tmp);
  *ptrID = atoi(tmp); // ID da Tarefa

// Diminui em 1 o número de campos devido ao elemento vazio no final.
  i=sizeof(campos)/sizeof(campos[0])-1;
  valor = (char **)(malloc(i * sizeof(char *)));
  while(i>0)
    {
    i--;
    if(strcmp(campos[i], "0")) // Se campo for diferente de zero devemos considerá-lo.
      {
      TV_GetSelected(GTK_TREE_VIEW(obj), i+1, tmp);
      valor[i] = (char *)(malloc(strlen(tmp)+1));
      strcpy(valor[i], tmp);
      }
    else
      valor[i] = NULL;
    }

  GravarValoresWidgets(wnd, campos, valor);

// Desaloca a memória em valor.
  i=sizeof(campos)/sizeof(campos[0])-1;
  while(i--)
    free(valor[i]);

  free(valor);

// Carrega o ponteiro para o botão de aplicar tarefa.
  obj = lookup_widget(wnd, "btnTarefaAplicar");

// Conexão dos sinais de callback
  g_signal_connect ((gpointer) wnd, "destroy",
    G_CALLBACK (SaindoTarefa), (gpointer)(ptrID));
  g_signal_connect ((gpointer) obj, "clicked",
    G_CALLBACK (AplicarTarefa), (gpointer)(ptrID));

  AbrirJanelaModal(wnd);
  gtk_grab_add(wnd);
}

void
RemoverTarefa                          (GtkButton       *button,
                                        gpointer         user_data)
{
  char sql[100], tmp[10];
  GtkWidget *dialog;

  TV_GetSelected(GTK_TREE_VIEW(lookup_widget(GTK_WIDGET(button), "tvwTarefas")), 0, tmp);

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
    CarregaListaTarefas(lookup_widget(GTK_WIDGET(button), "tvwTarefas"));
    }

  gtk_widget_destroy (dialog);
}

void
AplicarDataTarefa                      (GtkButton       *button,
                                        gpointer         user_data)
{
  guint y, m, d, hora, min;
  char tmp[100];

  gtk_calendar_get_date(GTK_CALENDAR(lookup_widget(GTK_WIDGET(button), "cldData")), &y, &m, &d);
  hora = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(GTK_WIDGET(button), "spbHora")));
  min  = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(GTK_WIDGET(button), "spbMin" )));

  sprintf(tmp, "%d-%02d-%02d %02d:%02d:00", y, m+1, d, hora, min);
  gtk_entry_set_text((GtkEntry *)(user_data), tmp);

  gtk_widget_destroy(lookup_widget(GTK_WIDGET(button),"wndData"));
}

void
SelecDataTarefa                        (GtkButton       *button,
                                        gpointer         user_data)
{
  AbrirData(GTK_ENTRY(lookup_widget(GTK_WIDGET(button), "entTarefaData")), G_CALLBACK (AplicarDataTarefa));
}

void
SaindoData                             (GtkObject       *object,
                                        gpointer         user_data)
{
  gtk_grab_remove(GTK_WIDGET(object));
}

void
TarefaSelecionada                      (GtkTreeView     *treeview,
                                        gpointer         user_data)
{
  ConfigBotoesTarefa(GTK_WIDGET(treeview), TRUE);
}

void
ClienteSelecionado                     (GtkComboBox     *combobox,
                                        gpointer         user_data)
{
  gboolean estado;

  if(gtk_combo_box_get_active(combobox)) {
    estado = TRUE;
  } else {
    estado = FALSE;
  }

  gtk_widget_set_sensitive(lookup_widget(GTK_WIDGET(combobox), "btnClienteRemover"), estado);
}


void
cbTarefaModeloSelec                    (GtkComboBox     *combobox,
                                        gpointer         user_data)
{
  GtkWidget *wdg;
  char sql[100], strval[100];

  sprintf(sql, "select passo, tam_max, tam_min from modelos where nome='%s'", LerComboAtivo(combobox));
  DB_Execute(&mainDB, 0, sql);
  DB_GetNextRow(&mainDB, 0);

  sprintf(strval, "%d mm", atol(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "passo"))));

  wdg = lookup_widget(GTK_WIDGET(combobox), "lblTarefaPasso");
  gtk_label_set_text(GTK_LABEL(wdg), strval);

  sprintf(strval, "Maior que %d mm e menor que %d mm",
        atol(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "tam_min"))),
        atol(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "tam_max"))));

  wdg = lookup_widget(GTK_WIDGET(combobox), "lblTarefaLimites");
  gtk_label_set_text(GTK_LABEL(wdg), strval);
}


void
cbFecharLogin                          (GtkObject       *object,
                                        gpointer         user_data)
{
  if(!idUser)
    gtk_main_quit();
  else
    Log("Entrada no sistema", LOG_TIPO_SISTEMA);
}


void
cbAbrirLog                             (GtkButton       *button,
                                        gpointer         user_data)
{
  int i;
  time_t DataInicial, DataFinal;
  char tmp[50], *campos[] = { "Data", "Usuário", "Evento", "" },
      *TiposLog[] = {
        "Todos",
        "Sistema",
        "Tarefas",
        "Erros",
        "Configuração",
        ""
      };

  GtkWidget *wnd, *tvw, *cmb;

  if(!GetUserPerm(PERM_ACESSO_LOGS))
    {
    MessageBox("Sem permissão para acesso aos registros!");
    return;
    }

  wnd = (GtkWidget *)(create_wndLog());
  tvw = lookup_widget(wnd, "tvwLog");

  TV_Config(tvw, campos, GTK_TREE_MODEL(gtk_list_store_new((sizeof(campos)/sizeof(campos[0]))-1, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING)));

  DataFinal = time(NULL);
  strftime (tmp, 100, "%Y-%m-%d %H:%M:%S", localtime(&DataFinal));
  gtk_entry_set_text(GTK_ENTRY(lookup_widget(wnd, "entLogDataFinal")), tmp);

  DataInicial = DataFinal - 604800; // 604.800 segundos = 7 dias
  strftime (tmp, 100, "%Y-%m-%d %H:%M:%S", localtime(&DataInicial));
  gtk_entry_set_text(GTK_ENTRY(lookup_widget(wnd, "entLogDataInicial")), tmp);

  cmb = lookup_widget(wnd, "cmbLogFiltro");
  for(i=0; TiposLog[i][0]; i++)
    CarregaItemCombo(GTK_COMBO_BOX(cmb), TiposLog[i]);

// Selecionando o item do combobox faz com que a lista de eventos seja carregada.
  gtk_combo_box_set_active(GTK_COMBO_BOX(cmb), 0);

  AbrirJanelaModal(wnd);
  gtk_grab_add(wnd);
}

void
cbLogFiltrar                           (GtkComboBox     *combobox,
                                        gpointer         user_data)
{
  CarregaListaLogs(lookup_widget(GTK_WIDGET(combobox), "tvwLog"));
}

void
AplicarDataLog                         (GtkButton       *button,
                                        gpointer         user_data)
{
  guint y, m, d, hora, min;
  char tmp[100];

  gtk_calendar_get_date(GTK_CALENDAR(lookup_widget(GTK_WIDGET(button), "cldData")), &y, &m, &d);
  hora = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(GTK_WIDGET(button), "spbHora")));
  min  = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(GTK_WIDGET(button), "spbMin" )));

  sprintf(tmp, "%d-%02d-%02d %02d:%02d:00", y, m+1, d, hora, min);
  gtk_entry_set_text((GtkEntry *)(user_data), tmp);

  gtk_widget_destroy(lookup_widget(GTK_WIDGET(button),"wndData"));

  CarregaListaLogs(lookup_widget((GtkWidget *)(user_data), "tvwLog"));
}

void
cbLogSelDataInicial                    (GtkButton       *button,
                                        gpointer         user_data)
{
  AbrirData(GTK_ENTRY(lookup_widget(GTK_WIDGET(button), "entLogDataInicial")), G_CALLBACK (AplicarDataLog));
}

void
cbLogSelDataFinal                      (GtkButton       *button,
                                        gpointer         user_data)
{
  AbrirData(GTK_ENTRY(lookup_widget(GTK_WIDGET(button), "entLogDataFinal")), G_CALLBACK (AplicarDataLog));
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

void
cbManutAtualSaidas                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  unsigned long i=0, val=0;
  GtkWidget *obj;
  char nome_obj[20];

  while(1) // Sai apenas quando não encontra o objeto
    {
    sprintf(nome_obj, "tglManutSai%02d", i);
    obj = lookup_widget(GTK_WIDGET(togglebutton), nome_obj);

    // Acabaram os controles, saindo
    if(obj == NULL)
      break;

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(obj)))
      val |= 1UL<<i;

    i++; // Próximo controle.
    }

  PIO_DO_Escrever(val);
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

      sprintf(sql, "Alterado o usuário '%s'", LerComboAtivo(GTK_COMBO_BOX(obj)));
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

// Oculta a janela de login. Isso deve ocorrer após identificar o usuário pois caso
// contrário o sistema é encerrado.
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

#endif
