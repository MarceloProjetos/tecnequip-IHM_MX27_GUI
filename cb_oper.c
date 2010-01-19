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

extern int idUser; // Indica usuário que logou se for diferente de zero.
extern int CurrentWorkArea;  // Variavel que armazena a tela atual.
extern int PreviousWorkArea; // Variavel que armazena a tela anterior.

// Função que salva um log no banco contendo usuário e data que gerou o evento.
extern void Log(char *evento, int tipo);

/*** Fim das funcoes e variáveis de suporte ***/

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

void cbExecTarefa(GtkButton *button, gpointer user_data)
{
  MessageBox("Executando...");
}
