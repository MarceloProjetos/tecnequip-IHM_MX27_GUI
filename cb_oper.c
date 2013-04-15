// Para que a funcao atof funcione corretamente
#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include "defines.h"
#include "maq.h"
#include "GtkUtils.h"

/*** Funcoes e variáveis de suporte ***/

extern int idUser; // Indica usuário que logou se for diferente de zero.

#define TRF_TEXT_SIZE 10

// Função que salva um log no banco contendo usuário e data que gerou o evento.
void Log(char *evento, int tipo);

void CarregaListaTarefas(GtkWidget *tvw);

void cbExecTarefa(GtkButton *button, gpointer user_data);

/*** Fim das funcoes e variáveis de suporte ***/

// Estrutura que armazena os dados das tarefas do sistema
struct strTask {
  char Produto[51]; // Campo no banco: nvchar(50)
  unsigned int Tamanho, Qtd, QtdProd, ID, Origem;
  int Pedido, OrdemProducao, RomNumero, RomItem;
  struct strTask *Next;
} *lstTask = NULL;

// Aloca memoria para nova tarefa
struct strTask * AllocNewTask(void)
{
  struct strTask *currTask;
  static struct strTask *lastTask;

  currTask = (struct strTask *)malloc(sizeof(struct strTask));
  memset(currTask, 0, sizeof(struct strTask));

  if(lstTask == NULL) {
    lstTask = currTask;
  } else {
    lastTask->Next = currTask;
  }

  lastTask = currTask;

  return currTask;
}

// Limpa as tarefas atuais
void ClearTasks(void)
{
  static struct strTask *currTask;

  while(lstTask != NULL) {
    currTask = lstTask;
    lstTask = lstTask->Next;
    free(currTask);
  }
}

// Retorna a tarefa indicada pelo indice
struct strTask * GetTask(unsigned int idx)
{
  struct strTask *Task = lstTask;

  while(idx-- && Task != NULL)
    Task = Task->Next;

  return Task;
}

// Definições para o log de produção
#define LOGPROD_START 0x01
#define LOGPROD_END   0x02

// Função para salvar log na tabela de log de ordem de produção do sistema
// Além disso, adiciona uma entrada no log de eventos indicando a produção
void LogProd(struct strTask *Task, int LogMode)
{
  static unsigned int QtdProdStart;
  char sql[500], agora[100], evento[100];

  if(LogMode == LOGPROD_START) {
    QtdProdStart = Task->QtdProd;

    sprintf(sql, "insert into LOG_ORDEM_PRODUCAO (LINHA, MAQUINA, OPERADOR, PEDIDO, ORDEM_PRODUCAO, ROMANEIO, ITEM, PRODUTO, TAMANHO, MANUAL) values ('%s','%s','%d','%d','%d','%d','%d','%s','%d','%d')",
        MAQ_LINHA, MAQ_MAQUINA, idUser, Task->Pedido, Task->OrdemProducao, Task->RomNumero, Task->RomItem, Task->Produto, Task->Tamanho, Task->Origem);

    sprintf(evento, "Produzindo %d peca(s) de %d mm", Task->Qtd - Task->QtdProd, Task->Tamanho);
  } else {
    sprintf(sql, "update LOG_ORDEM_PRODUCAO set DATA_FINAL='%s', PRODUZIDO='%d' where ID=(select MAX(ID) from LOG_ORDEM_PRODUCAO where LINHA='%s' and MAQUINA='%s')",
        MSSQL_DateFromTimeT(time(NULL), agora), Task->QtdProd - QtdProdStart, MAQ_LINHA, MAQ_MAQUINA);

    sprintf(evento, "Produzida(s) %d peca(s)", Task->QtdProd - QtdProdStart);
  }

  MSSQL_Execute(0, sql, MSSQL_USE_SYNC);

  Log(evento, LOG_TIPO_TAREFA);

  MSSQL_Close();
}

// Timer de producao
gboolean tmrExec(gpointer data)
{
  static int qtd, iniciando = 1;
  char tmp[30], sql[300];
  int qtdprod, status;
  struct strTask *Task = (struct strTask *)data;
  static GtkLabel *lblSaldo = NULL, *lblProd = NULL;

  if(iniciando) {
    iniciando = 0;
    qtd = atoi(gtk_label_get_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecTotal" ))));
    lblProd  = GTK_LABEL(gtk_builder_get_object(builder, "lblExecProd" ));
    lblSaldo = GTK_LABEL(gtk_builder_get_object(builder, "lblExecSaldo"));
  }

  qtdprod = MaqLerProdQtd(); // Retorna quantidade de pecas restantes.

  if((MaqLerEstado() & MAQ_MODO_MASK) == MAQ_MODO_MANUAL) {
    iniciando = 1;
    MaqConfigModo(MAQ_MODO_MANUAL);

    // Configura o estado da máquina
    SetMaqStatus(MAQ_STATUS_MANUAL);

    // Carrega a quantidade de peças da tarefa
    qtd = Task->Qtd;

    // Subtrai o total do numero de pecas restantes para encontrar quantas foram produzidas
    qtdprod = qtd - qtdprod;

    // Atualiza a tarefa com a quantidade produzida
    Task->QtdProd = qtdprod;

//      if(status == CV_ST_ESPERA)
//    MessageBox("Tarefa executada sem erros!");
//      else
//        MessageBox("Erro encontrado enquanto produzindo!");

    if(qtdprod >= qtd) // Quantidade produzida maior ou igual ao total
      status = TRF_ESTADO_FINAL;
    else if(qtdprod) // Quantidade produzida diferente de zero
      status = TRF_ESTADO_PARCIAL;
    else // Nenhuma peça produzida
      status = TRF_ESTADO_NOVA;

    // Executa a instrução SQL necessária para atualizar o estado da tarefa.
    if(Task->Origem == TRF_ORIGEM_MANUAL) {
      sprintf(sql, "update tarefas set estado='%d', qtdprod='%d' where ID='%d'", status, qtdprod, Task->ID);
      DB_Execute(&mainDB, 0, sql);
    }

    // Adiciona log de produção
    LogProd(Task, LOGPROD_END);

    WorkAreaGoTo(MaqConfigCurrent->AbaHome);
    return FALSE;
  } else {
    sprintf(tmp, "%d", qtd - qtdprod);
    gtk_label_set_text(lblProd, tmp);

    sprintf(tmp, "%d", qtdprod);
    gtk_label_set_text(lblSaldo, tmp);
  }

  return TRUE;
}

void ConfigBotoesTarefa(GtkWidget *wdg, gboolean modo)
{
  struct strTask *Task = NULL;
  char txt_id[10];

  TV_GetSelected(wdg, 0, txt_id);
  if(txt_id != NULL)
    Task = GetTask(atoi(txt_id)-1);

  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnExecTarefa"   )), modo);
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnEditarTarefa" )), modo && Task && (Task->Origem == TRF_ORIGEM_MANUAL));
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnRemoverTarefa")), modo && Task && (Task->Origem == TRF_ORIGEM_MANUAL));
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

void InsertLocalTask(void)
{
  char *tmp;
  struct strTask *currTask;

  // Aloca memoria para nova tarefa
  currTask = AllocNewTask();

  currTask->Origem = TRF_ORIGEM_MANUAL;

  // Salva dados da tarefa atual
  currTask->ID      = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "ID"     )));
  currTask->Tamanho = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "Tamanho")));
  currTask->Qtd     = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "Qtd"    )));
  currTask->QtdProd = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "QtdProd")));

  // Carrega os dados referentes ao pedido
  tmp = DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "Pedido"));
  currTask->Pedido        = tmp ? atoi(tmp) : 0;

  currTask->OrdemProducao = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "OrdemProducao")));
  currTask->RomNumero     = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "RomNumero"    )));
  currTask->RomItem       = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "RomItem"      )));
}

void CarregaListaTarefas(GtkWidget *tvw)
{
  int i, TaskIndex = 1;
  struct strTask *currTask;
  const int tam = 9;
  struct strDB *mssqlDB;
  char *valores[tam+1], sql[500];

  valores[tam] = NULL;

  // Limpa a lista anterior
  ClearTasks();

  // Exclui os itens atuais
  TV_Limpar(tvw);

  // Carrega as tarefas do MySQL
  sprintf(sql, "select t.ID, c.nome, t.Pedido, m.nome, t.qtd as Qtd, t.qtdprod as QtdProd, t.tamanho as Tamanho, t.data, t.coments, t.OrdemProducao, t.RomNumero, t.RomItem from modelos as m, tarefas as t, clientes as c where c.ID = t.ID_Cliente and m.ID = t.ID_Modelo and (t.estado='%d' or t.estado='%d') and m.estado='%d' order by data",
      TRF_ESTADO_NOVA, TRF_ESTADO_PARCIAL, MOD_ESTADO_ATIVO);
  DB_Execute(&mainDB, 0, sql);
  while(DB_GetNextRow(&mainDB, 0)>0)
    {
    valores[0] = (char*)malloc(10);
    sprintf(valores[0], "%d", TaskIndex++);
    for(i = 1; i<tam; i++)
      {
      valores[i] = DB_GetData(&mainDB, 0, i);
      if(valores[i] == NULL)
        valores[i] = "";
      }

    // Insere uma nova tarefa a partir do registro atual
    InsertLocalTask();

    TV_Adicionar(tvw, valores);
    free(valores[0]);
    }

  // Carrega as tarefas do sistema (SQL Server) se conseguir a conexao.
  mssqlDB = MSSQL_Connect();
  if(mssqlDB && (mssqlDB->status == DB_FLAGS_CONNECTED)) {
    sprintf(sql, "select ORDEM_PRODUCAO, CLIENTE, PEDIDO, TIPO, QUANTIDADE, PRODUZIDO, TAMANHO, DATA_INICIO, MATERIAL_DESCRICAO, ROMANEIO, ITEM, CODIGO from ORDEM_PRODUCAO where LINHA='%s' and MAQUINA='%s' and QUANTIDADE <> PRODUZIDO order by MATERIAL", MAQ_LINHA, MAQ_MAQUINA);
    MSSQL_Execute(0, sql, MSSQL_DONT_SYNC);

    while(DB_GetNextRow(mssqlDB, 0)>0) {
      valores[0] = (char*)malloc(10);
      sprintf(valores[0], "%d", TaskIndex++);
      for(i = 1; i<tam; i++) {
        valores[i] = MSSQL_GetData(0, i);
        if(valores[i] == NULL) {
          valores[i] = "";
        } else if((i==1 || i==3) && strlen(valores[i]) > TRF_TEXT_SIZE) {
            valores[i][TRF_TEXT_SIZE] = 0;
        }
      }

      // Aloca memoria para nova tarefa
      currTask = AllocNewTask();

      currTask->Origem = TRF_ORIGEM_ERP;

      // Salva dados da tarefa atual
      currTask->OrdemProducao = atoi(MSSQL_GetData(0, DB_GetFieldNumber(mssqlDB, 0, "ORDEM_PRODUCAO")));
      currTask->Pedido        = atoi(MSSQL_GetData(0, DB_GetFieldNumber(mssqlDB, 0, "PEDIDO"        )));
      currTask->RomNumero     = atoi(MSSQL_GetData(0, DB_GetFieldNumber(mssqlDB, 0, "ROMANEIO"      )));
      currTask->RomItem       = atoi(MSSQL_GetData(0, DB_GetFieldNumber(mssqlDB, 0, "ITEM"          )));
      currTask->Tamanho       = atoi(MSSQL_GetData(0, DB_GetFieldNumber(mssqlDB, 0, "TAMANHO"       )));
      currTask->Qtd           = atoi(MSSQL_GetData(0, DB_GetFieldNumber(mssqlDB, 0, "QUANTIDADE"    )));
      currTask->QtdProd       = atoi(MSSQL_GetData(0, DB_GetFieldNumber(mssqlDB, 0, "PRODUZIDO"     )));

      strcpy(currTask->Produto, MSSQL_GetData(0, DB_GetFieldNumber(mssqlDB, 0, "CODIGO")));

      // Adiciona linha da tarefa no GtkTreeView
      TV_Adicionar(tvw, valores);

      for(i = 0; i<tam; i++) {
        if(strlen(valores[i]))
            free(valores[i]);
      }
    }
  } else {
    strcpy(sql, "Erro ao carregar tarefas do sistema!");
    MessageBox(sql);
    Log(sql, LOG_TIPO_SISTEMA);
  }

  MSSQL_Close();
  ConfigBotoesTarefa(GTK_WIDGET(tvw), FALSE);
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

void IniciarDadosTarefa()
{
  char tmp[100];
  time_t agora;

  CarregaComboModelosTarefa(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbTarefaModNome")));

  DB_Execute(&mainDB, 0, "select nome from clientes order by ID");
  CarregaCombo(&mainDB, GTK_COMBO_BOX(gtk_builder_get_object(builder, "cbeTarefaCliente")), 0, NULL);

  agora = time(NULL);
  strftime (tmp, 100, "%Y-%m-%d %H:%M:%S", localtime(&agora));
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entTarefaData")), tmp);
}

void LimparDadosTarefa()
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
}

int AplicarTarefa()
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
    return FALSE; // Ocorreu algum erro lendo os campos. Retorna.

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
    return FALSE;

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

  return TRUE;
}

void cbSairTarefa(GtkButton *button, gpointer user_data)
{
  LimparDadosTarefa();
  CarregaListaTarefas(GTK_WIDGET(gtk_builder_get_object(builder, "tvwTarefas")));
  WorkAreaGoTo(NTB_ABA_OPERAR);
}

void cbAplicarTarefa(GtkButton *button, gpointer user_data)
{
  if(AplicarTarefa() == TRUE)
    cbSairTarefa(NULL, NULL);
}

void cbAdicionarTarefa(GtkButton *button, gpointer user_data)
{
  IniciarDadosTarefa();
  WorkAreaGoTo(NTB_ABA_TAREFA);
}

void cbEditarTarefa(GtkButton *button, gpointer user_data)
{
  int i;
  GtkWidget *obj;
  struct strTask *Task;
  char **valor, tmp[30], *campos[] = { "lblTarefaNumero", "cbeTarefaCliente", "entTarefaPedido", "cmbTarefaModNome", "entTarefaQtd", "0", "entTarefaTam", "entTarefaData", "txvTarefaComent", "" };

  IniciarDadosTarefa();

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
      if(!i) { // ID da Tarefa
        Task = GetTask(atoi(tmp)-1);
        if(Task != NULL)
          sprintf(tmp, "%d", Task->ID);
      }
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

  WorkAreaGoTo(NTB_ABA_TAREFA);
}

void cbRemoverTarefa(GtkButton *button, gpointer user_data)
{
  struct strTask *Task;
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
    Task = GetTask(atoi(tmp)-1);
    sprintf(sql, "update tarefas set estado='%d' where ID='%d'", TRF_ESTADO_REMOVIDA, Task->ID);
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

  WorkAreaGoTo(NTB_ABA_TAREFA);
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
    WorkAreaGoTo(MaqConfigCurrent->AbaHome);
    MessageBox("Sem permissão para operar a máquina!");
    return;
    }

  CarregaListaTarefas(GTK_WIDGET(gtk_builder_get_object(builder, "tvwTarefas")));
}

void LimparDadosPedido(void)
{
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entPedidoNum"    )), "");
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entPedidoOP"     )), "");
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entPedidoRomNum" )), "");
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entPedidoRomItem")), "");
}

void cbConfirmarDadosPedido(GtkButton *button, gpointer user_data)
{
  char tmp[50];
  struct strTask *newTask = (struct strTask *)user_data;
  static struct strTask *currTask = NULL;

  if(newTask != NULL) { // Tarefa alterada!
    currTask = newTask;

    // Carrega os dados da tarefa atual nos campos da tela
    sprintf(tmp, "%d", currTask->Pedido);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entPedidoNum"    )), tmp);

    sprintf(tmp, "%d", currTask->OrdemProducao);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entPedidoOP"     )), tmp);

    sprintf(tmp, "%d", currTask->RomNumero);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entPedidoRomNum" )), tmp);

    sprintf(tmp, "%d", currTask->RomItem);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entPedidoRomItem")), tmp);
  } else if(currTask != NULL) { // Clicado botao, verifica os dados
    // Atualiza tarefa
    currTask->Pedido        = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entPedidoNum"    ))));
    currTask->OrdemProducao = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entPedidoOP"     ))));
    currTask->RomNumero     = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entPedidoRomNum" ))));
    currTask->RomItem       = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entPedidoRomItem"))));

    // Ao menos o pedido deve ser informado...
    // Claro que muitas vezes sera informado um pedido falso, apenas para poder produzir.
    // Mas temos que fazer a nossa parte!
    if(!currTask->Pedido) {
      MessageBox("Pedido deve ser informado!");
    } else {
      // Limpa os dados atuais
      LimparDadosPedido();

      // Executa a tarefa com os dados atualizados
      cbExecTarefa(NULL, (gpointer)currTask);
    }
  }
}

void cbCancelarDadosPedido(GtkButton *button, gpointer user_data)
{
  WorkAreaGoTo(MaqConfigCurrent->AbaHome);
  LimparDadosPedido();
}

void cbExecTarefa(GtkButton *button, gpointer user_data)
{
  int qtd, tam, DadosOK = 0;
  GtkWidget *wdg;
  struct strTask *Task = (struct strTask *)user_data;
  char tmp[30], sql[300];

  if(MaqConfigCurrent->NeedMaqInit && !(MaqLerEstado() & MAQ_STATUS_INITOK)) {
    MessageBox("Máquina não inicializada!");
    return;
  }

  if(button != NULL) { // Operacao normal
    // Carrega id da tarefa selecionada
    wdg = GTK_WIDGET(gtk_builder_get_object(builder, "tvwTarefas"));
    TV_GetSelected(wdg, 0, tmp);

    // Carrega os dados da tarefa selecionada
    Task = GetTask(atoi(tmp)-1);
  } else if(Task == NULL) { // Produzindo pela operacao manual
    // Carrega os dados da ultima tarefa adicionada
    DB_Execute(&mainDB, 0, "select * from tarefas where id = (select max(id) from tarefas)");
    DB_GetNextRow(&mainDB, 0);

    // Limpa lista de tarefas e adiciona a tarefa atual à lista
    ClearTasks();
    InsertLocalTask();
    Task = GetTask(0);
  } else {
    DadosOK = 1;
  }

  // Se for tarefa manual, exibe tela pedindo os dados adicionais
  if(Task->Origem == TRF_ORIGEM_MANUAL && !DadosOK) {
    // Passa a tarefa para a tela de dados do pedido.
    cbConfirmarDadosPedido(NULL, (gpointer)Task);

    WorkAreaGoTo(NTB_ABA_DADOS_PEDIDO);

    return;
  } else if(Task->Origem == TRF_ORIGEM_MANUAL) { // Tarefa manual. Os dados do pedido podem ter sido alterados, atualizar!
    sprintf(sql, "update tarefas set Pedido='%d', OrdemProducao='%d', RomNumero='%d', RomItem='%d' where ID='%d'",
        Task->Pedido, Task->OrdemProducao, Task->RomNumero, Task->RomItem, Task->ID);
    DB_Execute(&mainDB, 0, sql);
  }

  qtd = Task->Qtd - Task->QtdProd;
  tam = Task->Tamanho;

  sprintf(tmp, "%d", qtd);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecTotal")), tmp);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecSaldo")), tmp);

  sprintf(tmp, "%d", tam);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecPos" )), tmp);

  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecProd")), "0");

  MaqConfigProdQtd(qtd);
  MaqConfigProdTam(tam);

  sprintf(sql, "Produzindo %d peças de %d mm", qtd, tam);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecMsg")), sql);
  LogProd(Task, LOGPROD_START);

  MaqConfigModo(MAQ_MODO_AUTO);
  g_timeout_add(1000, tmrExec, (gpointer)Task);

  // Configura o estado da máquina
  SetMaqStatus(MAQ_STATUS_PRODUZINDO);

  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnExecParar" )), TRUE);
  WorkAreaGoTo(NTB_ABA_EXECUTAR);
}

void cbExecParar(GtkButton *button, gpointer user_data)
{
  atividade++;
  MaqConfigModo(MAQ_MODO_MANUAL);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecMsg")), "Terminando...");
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
}

void cbOperarProduzir(GtkButton *button, gpointer user_data)
{
  char *ObjQtd = MaqConfigCurrent->MaqModeCV ? "entCVOperarQtd" : "entOperarQtd";
  char *ObjTam = MaqConfigCurrent->MaqModeCV ? "entCVOperarTam" : "entOperarTam";

  if(!GetUserPerm(PERM_ACESSO_OPER))
    {
    MessageBox("Sem permissão para operar a máquina!");
    return;
    }

  IniciarDadosTarefa();

  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entTarefaQtd")),
      gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, ObjQtd))));

  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entTarefaTam")),
      gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, ObjTam))));

  if(AplicarTarefa()) {
    LimparDadosTarefa();

    // Passando nulos executa ultima tarefa adicionada.
    cbExecTarefa(NULL, NULL);
  } else {
    LimparDadosTarefa();
  }
}

gboolean cbMaquinaButtonPress(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  static GdkEventButton last = { 0 };

// Se soltou o botao antes de 500 ms do clique, interpreta o clique e abre tela de manutencao
  if(event->type == GDK_BUTTON_RELEASE && (event->time - last.time) < 500)
    WorkAreaGoTo(MaqConfigCurrent->AbaManut);

  last = *event;
  return TRUE;
}

void OperarManual(unsigned int operacao)
{
  // Configura o estado da máquina
  SetMaqStatus(MAQ_STATUS_MANUAL);

  // Configura a máquina para modo manual.
  MaqConfigModo(MAQ_MODO_MANUAL);

  switch(operacao) {
    case OPER_PERF_AVANCA:
    case OPER_PERF_RECUA:
    case OPER_PERF_PARAR:
      MaqPerfManual(operacao);
      break;

    case OPER_MESA_AVANCA:
    case OPER_MESA_RECUA:
    case OPER_MESA_PARAR:
      MaqMesaManual(operacao);
      break;

    case OPER_CORTAR:
      MaqCortar();
      break;
  }
}

void cbManualPerfAvancar(GtkButton *button, gpointer user_data)
{
  if(MaqConfigCurrent->InverterComandos) {
    OperarManual(OPER_PERF_RECUA);
  } else {
    OperarManual(OPER_PERF_AVANCA);
  }
}

void cbManualPerfRecuar(GtkButton *button, gpointer user_data)
{
  if(MaqConfigCurrent->InverterComandos) {
    OperarManual(OPER_PERF_AVANCA);
  } else {
    OperarManual(OPER_PERF_RECUA);
  }
}

void cbManualPerfParar(GtkButton *button, gpointer user_data)
{
  OperarManual(OPER_PERF_PARAR);
}

void cbManualMesaAvancar(GtkButton *button, gpointer user_data)
{
  if(MaqConfigCurrent->InverterComandos) {
    OperarManual(OPER_MESA_RECUA);
  } else {
    OperarManual(OPER_MESA_AVANCA);
  }
}

void cbManualMesaRecuar(GtkButton *button, gpointer user_data)
{
  if(MaqConfigCurrent->InverterComandos) {
    OperarManual(OPER_MESA_AVANCA);
  } else {
    OperarManual(OPER_MESA_RECUA);
  }
}

void cbManualMesaParar(GtkButton *button, gpointer user_data)
{
  OperarManual(OPER_MESA_PARAR);
}

void cbManualCortar(GtkButton *button, gpointer user_data)
{
  OperarManual(OPER_CORTAR);
}

// defines que permitem selecionar o ponto de referencia para insercao da imagem
#define LOADPB_REFPOS_UP      0x00
#define LOADPB_REFPOS_DOWN    0x01
#define LOADPB_REFPOS_LEFT    0x00
#define LOADPB_REFPOS_RIGHT   0x02
#define LOADPB_REFPOS_DEFAULT (LOADPB_REFPOS_UP | LOADPB_REFPOS_LEFT)

void LoadIntoPixbuf(GdkPixbuf *pb, char *file, gint x, gint y, gdouble scale_x, gdouble scale_y, gint refpos)
{
  GdkPixbuf *pbtmp;
  gint width, height;
  static gint last_x = 0, last_y = 0;

// Verifica se devemos usar a ultima coordenada ou a passada como parametro
  if(x<0)
    x = last_x;
  if(y<0)
    y = last_y;

// Carrega a nova imagem
  pbtmp  = gdk_pixbuf_new_from_file(file, NULL);

// Carrega as dimensoes da nova imagem
  width  = gdk_pixbuf_get_width (pbtmp)*scale_x;
  height = gdk_pixbuf_get_height(pbtmp)*scale_y;

// Recalcula as coordenadas de acordo com o ponto de referencia passado como parametro
  if(refpos & LOADPB_REFPOS_DOWN)
    y = gdk_pixbuf_get_height(pb) - height - y;
  if(refpos & LOADPB_REFPOS_RIGHT)
    x = gdk_pixbuf_get_width (pb) - width  - x;

// Agrega as duas imagens e remove a referencia a nova imagem
  gdk_pixbuf_composite(pbtmp, pb,
      x, y, width, height, x, y, scale_x, scale_y,
      GDK_INTERP_BILINEAR, 255);
  g_object_unref(pbtmp);

// Atualiza as coordenadas atuais
  last_x = x + width ;
  last_y = y + height;
}

#define PERF_ALTURA_MESA 98

gboolean cbDesenharMaquina(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
//  GdkPixbuf *pbtmp;
  GtkStyle *style;
  GtkAllocation allocation;
  static GdkPixbuf *pb = NULL;
  if(pb == NULL) {
    gtk_widget_get_allocation(widget, &allocation);
    pb = gdk_pixbuf_new_from_file_at_scale("images/bg01.png",
        allocation.width, allocation.height,
        FALSE, NULL);

    LoadIntoPixbuf(pb, "images/maq-desbob.png"      ,  0,                0, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);

    LoadIntoPixbuf(pb, "images/maq-perf-ini.png"    , 10,                0, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);
    LoadIntoPixbuf(pb, "images/maq-perf-mesa.png"   , -1,                0, 1.25, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);
    LoadIntoPixbuf(pb, "images/maq-perf-fim.png"    , -1,                0, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);

    LoadIntoPixbuf(pb, "images/maq-perf-prensa.png" , 20, PERF_ALTURA_MESA, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);
    LoadIntoPixbuf(pb, "images/maq-perf-guia.png"   , -1, PERF_ALTURA_MESA, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);
    LoadIntoPixbuf(pb, "images/maq-perf-castelo.png", -1, PERF_ALTURA_MESA, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);
    LoadIntoPixbuf(pb, "images/maq-perf-castelo.png", -1, PERF_ALTURA_MESA, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);
    LoadIntoPixbuf(pb, "images/maq-perf-castelo.png", -1, PERF_ALTURA_MESA, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);
    LoadIntoPixbuf(pb, "images/maq-perf-guia.png"   , -1, PERF_ALTURA_MESA, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);
  }

  style = gtk_widget_get_style(widget);
  gdk_draw_pixbuf(gtk_widget_get_window(widget),
                  style->fg_gc[gtk_widget_get_state(widget)],
                  pb,
                  0, 0, 0, 0, -1, -1,
                  GDK_RGB_DITHER_NONE, 0, 0);

  return TRUE;
}
