#include <gtk/gtk.h>
#include <pthread.h>
#include <string.h>
#include "serial.h"
#include "comm.h"

#define DEBUG_PC

extern SERIAL_TX_FNC(SerialTX);
extern SERIAL_RX_FNC(SerialRX);

struct PortaSerial *ps;

COMM_FNC(CommTX)
{
  return Transmitir(ps, (char *)data, size, 0);
}

COMM_FNC(CommRX)
{
  return Receber(ps, (char *)data, size);
}

// Objeto que contem toda a interface GTK
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

/****************************************************************************
 * Thread de comunicacao com o LPC2109 (Power Management)
 ***************************************************************************/
void * ihm_update(void *args)
{
  struct comm_msg msg;

  /****************************************************************************
   * Loop
   ***************************************************************************/
  while (1) {
    comm_update();
    if(comm_ready()) {
      comm_get(&msg);
      printf("\nFuncao.: 0x%08x\n", msg.fnc);
      printf("\tds.....: 0x%08x\n", msg.data.ds     );
      printf("\tled....: 0x%08x\n", msg.data.led    );
      printf("\tad.vin.: 0x%08x\n", msg.data.ad.vin );
      printf("\tad.term: 0x%08x\n", msg.data.ad.term);
      printf("\tds.vbat: 0x%08x\n", msg.data.ad.vbat);
    }
  }

  return NULL;
}

//Inicia a aplicacao
int main(int argc, char *argv[])
{
  pthread_t tid;
  GtkWidget *wnd;

  ps = SerialInit("/dev/ttyUSB0");
  if(ps == NULL) {
    printf("Erro abrindo porta serial!\n");
    return 1;
  }

  SerialConfig(ps, 115200, 8, 1, SerialParidadeNenhum, 0);

  ps->txf = SerialTX;
  ps->rxf = SerialRX;

  comm_init(CommTX, CommRX);
  comm_put (&(struct comm_msg){ COMM_FNC_LED, { 0xA } });
  comm_put (&(struct comm_msg){ COMM_FNC_AIN, { 0x0 } });

  /* init threads */
  g_thread_init (NULL);
  gdk_threads_init ();

  gdk_threads_enter();

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

  pthread_create (&tid, NULL, ihm_update, NULL);

  //Inicia o loop principal de eventos (GTK MainLoop)
  gtk_main ();

  SerialClose(ps);

  gdk_threads_leave();

  return 0;
}
