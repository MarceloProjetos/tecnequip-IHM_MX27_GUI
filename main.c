#include <gtk/gtk.h>
#include <string.h>

GtkBuilder *builder;

// Timers
gboolean tmrProgress(gpointer data)
{
  static gboolean first_time = TRUE;
  static GtkProgressBar *pgbAD1, *pgbAD2, *pgbAD3;

  if(first_time) {
    first_time = FALSE;
    pgbAD1 = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "pgbAD1")),
    pgbAD2 = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "pgbAD2"));
    pgbAD3 = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "pgbAD3"));
  }

#ifdef DEBUG_PC
  gtk_progress_bar_set_fraction(pgbAD1, 0.2);
  gtk_progress_bar_set_fraction(pgbAD2, 0.3);
  gtk_progress_bar_set_fraction(pgbAD3, 0.1);
#else
  gtk_progress_bar_set_fraction(pgbAD1, 0.6);
  gtk_progress_bar_set_fraction(pgbAD2, 0.5);
  gtk_progress_bar_set_fraction(pgbAD3, 0.4);
#endif

  return TRUE;
}

// Callbacks
void cbFunctionKey(GtkButton *button, gpointer user_data)
{
  const gchar *nome = gtk_widget_get_name(GTK_WIDGET(button));
  GtkWidget *ntb = GTK_WIDGET(gtk_builder_get_object(builder, "ntbWorkArea"));
  gtk_notebook_set_current_page(GTK_NOTEBOOK(ntb), nome[strlen(nome)-1]-'0' - 1);
}

void cbQuitCancel(GtkButton *button, gpointer user_data)
{
  GtkWidget *ntb = GTK_WIDGET(gtk_builder_get_object(builder, "ntbWorkArea"));
  gtk_notebook_set_current_page(GTK_NOTEBOOK(ntb), 0);
}

//Inicia a aplicacao
int main(int argc, char *argv[])
{
  GtkWidget *wnd;

  gtk_init( &argc, &argv );

  //Carrega a interface a partir do arquivo glade
  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "IHM.glade", NULL);
  wnd = GTK_WIDGET(gtk_builder_get_object(builder, "wndDesktop"));

  //Conecta Sinais aos Callbacks
  gtk_builder_connect_signals(builder, NULL);

//  g_object_unref (G_OBJECT (builder));

  gtk_widget_show_all(wnd);

  // Iniciando os timers
  g_timeout_add(500, tmrProgress, NULL);

  //Inicia o loop principal de eventos (GTK MainLoop)
  gtk_main ();

  return 0;
}
