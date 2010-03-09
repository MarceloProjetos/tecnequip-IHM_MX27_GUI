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

  WorkAreaGoTo(NTB_ABA_LOGS);
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
  WorkAreaGoTo(NTB_ABA_HOME);
}
