#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <gtk/gtk.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "serial.h"
#include "defines.h"
#include "GtkUtils.h"
#include <comm.h>
#include <net/modbus.h>
#include <DB.h>

extern SERIAL_TX_FNC(SerialTX);
extern SERIAL_RX_FNC(SerialRX);

extern void AbrirLog   ();
extern void AbrirOper  ();
extern void AbrirConfig(unsigned int pos);

struct MB_Device mbdev;
struct strDB     mainDB;
extern int idUser; // Indica usuário que logou se for diferente de zero.
int CurrentWorkArea  = 0; // Variavel que armazena a tela atual.
int PreviousWorkArea = 0; // Variavel que armazena a tela anterior.

// Função que salva um log no banco contendo usuário e data que gerou o evento.
void Log(char *evento, int tipo)
{
  char sql[200];

  if(mainDB.res != NULL) // Banco conectado
    {
    sprintf(sql, "insert into log (ID_Usuario, Tipo, Evento) values ('%d', '%d', '%s')", idUser, tipo, evento);
    DB_Execute(&mainDB, 0, sql);
    }
}

int32_t ihm_connect(char *host, int16_t port)
{
        struct  hostent  *ptrh;  /* pointer to a host table entry       */
        struct  protoent *ptrp;  /* pointer to a protocol table entry   */
        struct  sockaddr_in sad; /* structure to hold an IP address     */
        int32_t sd;              /* socket descriptor                   */

        memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
        sad.sin_family = AF_INET;         /* set family to Internet     */

        /* Check command-line argument for protocol port and extract    */
        /* port number if one is specified.  Otherwise, use the default */
        /* port value given by constant PROTOPORT                       */

        if (port > 0)                   /* test for legal value         */
                sad.sin_port = htons((uint16_t)port);
        else {                          /* print error message and exit */
                fprintf(stderr,"Bad port number %d\n",port);
                return -1;
        }

        /* Convert host name to equivalent IP address and copy to sad. */

        ptrh = gethostbyname(host);
        if ( ((int8_t *)ptrh) == NULL ) {
                fprintf(stderr,"Invalid host: %s\n", host);
                return -1;
        }
        memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);

        /* Map TCP transport protocol name to protocol number. */

        if ( ((int32_t)(ptrp = getprotobyname("tcp"))) == 0) {
                fprintf(stderr, "Cannot map \"tcp\" to protocol number");
                return -1;
        }

        /* Create a socket. */

        sd = socket(AF_INET, SOCK_STREAM, ptrp->p_proto);
        if (sd < 0) {
                fprintf(stderr, "Socket creation failed\n");
                return -1;
        }

        /* Connect the socket to the specified server. */

        if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
                fprintf(stderr,"Connect failed\n");
                return -1;
        }

        return sd;
}

struct PortaSerial *ps;

COMM_FNC(CommTX)
{
  return Transmitir(ps, (char *)data, size, 0);
}

COMM_FNC(CommRX)
{
  return Receber(ps, (char *)data, size);
}

MB_HANDLER_TX(IHM_MB_TX)
{
  uint32_t i, tent = 10;
  int32_t tcp_socket, opts, resp;

  printf("MB Send: ");
  for(i=0; i<size; i++)
    printf("%02x ", data[i]);
  printf("\n");

// Envia a mensagem pela ethernet
  tcp_socket = ihm_connect("192.168.0.232", 502);
  if(tcp_socket >= 0) {
    send(tcp_socket, data, size, 0);

    // Configura socket para o modo non-blocking e retorna zero no erro.
    opts = fcntl(tcp_socket,F_GETFL);
    if (opts < 0)
      return 0;
    if (fcntl(tcp_socket, F_SETFL, opts | O_NONBLOCK) < 0)
      return 0;

    while((resp=recv(tcp_socket, data, MB_BUFFER_SIZE, 0))<=0 && tent--) {
      usleep(10000);
    }
    close(tcp_socket);

    if(resp<=0) {
      size = 0;
      printf("Tempo para resposta esgotado...\n");
    } else {
      size = resp;
      printf("Retorno de MB Send: ");
      for(i=0; i<size; i++)
        printf("%02x ", data[i]);
      printf("\n");
    }
  } else {
    size = 0;
  }

  return size;
}

// Objeto que contem toda a interface GTK
GtkBuilder *builder;

// Timers
gboolean tmrAD(gpointer data)
{
  if(CurrentWorkArea == NTB_ABA_HOME) {
    comm_put(&(struct comm_msg){ COMM_FNC_AIN, { 0x0 } });
  }

  return TRUE;
}

gboolean tmrPowerDown(gpointer data)
{
  char msg[15];
  struct comm_msg cmsg;
  static GtkDialog *dlg = NULL;
  static uint32_t timeout=0;
  GtkDialog *current = *(GtkDialog **)data; // Recebe o ponteiro como parametro nao o endereco apontado.

  if(current != dlg) { // mudou o estado da alimentacao
    dlg = current;
    timeout = 30;
  } else if(dlg != NULL) {
    gdk_threads_enter();
    if(!timeout--) {
      gtk_main_quit();
    } else {
      sprintf(msg, "%d segundos", timeout);
      gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblPowerDownMsg")), msg);

      comm_update();
      while(comm_ready()) {
        comm_get(&cmsg);
        if(cmsg.fnc == COMM_FNC_PWR && cmsg.data.pwr) {
          gtk_dialog_response(dlg, 2);
        }
        comm_update();
      }
    }
    gdk_threads_leave();
  }

  return TRUE;
}

/****************************************************************************
 * Callbacks gerais do GTK
 ***************************************************************************/

// Seleciona a aba correspondente ao botao clicado
void cbFunctionKey(GtkButton *button, gpointer user_data)
{
  uint32_t idx;
  const gchar *nome = gtk_widget_get_name(GTK_WIDGET(button));
  GtkWidget *ntb = GTK_WIDGET(gtk_builder_get_object(builder, "ntbWorkArea"));

  idx = nome[strlen(nome)-1]-'0' - 1;
  gtk_notebook_set_current_page(GTK_NOTEBOOK(ntb), idx);

  switch(idx) {
  case NTB_ABA_CONFIG:
    AbrirConfig(0);
    break;
  case NTB_ABA_OPERAR:
    AbrirOper();
    break;
  case NTB_ABA_LOGS:
    AbrirLog();
    break;
  }
}

// Volta para a aba Home se cancelar desligamento
void cbQuitCancel(GtkButton *button, gpointer user_data)
{
  GtkWidget *ntb = GTK_WIDGET(gtk_builder_get_object(builder, "ntbWorkArea"));
  gtk_notebook_set_current_page(GTK_NOTEBOOK(ntb), 0);
}

void cbLogoff(GtkButton *button, gpointer user_data)
{
  GtkWidget *wnd = GTK_WIDGET(gtk_builder_get_object(builder, "wndLogin"));

  Log("Saida do sistema", LOG_TIPO_SISTEMA);

// Grava zero em idUser para indicar que não há usuário logado
  idUser = 0;

// Carregamento dos usuários cadastrados no MySQL no ComboBox.
  DB_Execute(&mainDB, 0, "select login from usuarios order by ID");
  CarregaCombo(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbLoginUser")), 0, NULL);

  gtk_widget_hide_all(GTK_WIDGET(gtk_builder_get_object(builder, "wndDesktop")));

  AbrirJanelaModal(wnd);
  gtk_grab_add(wnd);
}

/****************************************************************************
 * Thread de comunicacao com o LPC2109 (Power Management)
 ***************************************************************************/
void * ihm_update(void *args)
{
  uint32_t ad_vin=-1, ad_term=-1, ad_vbat=-1;
  struct comm_msg msg;
  GtkProgressBar *pgbVIN, *pgbTERM, *pgbVBAT;
  GtkStatusbar *sts;
  GtkDialog *dlg = NULL;

  gdk_threads_enter();
  g_timeout_add(1000, tmrPowerDown, (void *)&dlg);
  pgbVIN  = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "pgbVIN" ));
  pgbTERM = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "pgbTERM"));
  pgbVBAT = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "pgbVBAT"));
  sts     = GTK_STATUSBAR   (gtk_builder_get_object(builder, "stsMensagem"));
  gdk_threads_leave();

  /****************************************************************************
   * Loop
   ***************************************************************************/
  while (1) {
    usleep(500);
    comm_update();
    if(comm_ready()) {
      comm_get(&msg);
      switch(msg.fnc) {
      case COMM_FNC_AIN: // Resposta do A/D. Divide por 3150 pois representa a tensao em mV.
        if(CurrentWorkArea == NTB_ABA_HOME) {
          gdk_threads_enter();
          if(msg.data.ad.vin != ad_vin) {
            ad_vin = msg.data.ad.vin;
            gtk_progress_bar_set_fraction(pgbVIN , (gdouble)(msg.data.ad.vin )/3150);
          }
          if(msg.data.ad.term != ad_term) {
            ad_term = msg.data.ad.term;
            gtk_progress_bar_set_fraction(pgbTERM, (gdouble)(msg.data.ad.term)/3150);
          }
          if(msg.data.ad.vbat != ad_vbat) {
            ad_vbat = msg.data.ad.vbat;
            gtk_progress_bar_set_fraction(pgbVBAT, (gdouble)(msg.data.ad.vbat)/3150);
          }
          gdk_threads_leave();
        }
        break;

      case COMM_FNC_PWR:
        gdk_threads_enter();
        if(msg.data.pwr) {
          gtk_statusbar_push(sts, gtk_statusbar_get_context_id(sts, "pwr"), "Sistema energizado");
        } else {
          gtk_statusbar_push(sts, gtk_statusbar_get_context_id(sts, "pwr"), "Sistema sem energia");

          dlg = GTK_DIALOG(gtk_builder_get_object(builder, "dlgPowerDown"));
          gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblPowerDownMsg")),
              "30 segundos");
          gtk_widget_show_all(GTK_WIDGET(dlg));
          printf("Resposta: %d\n", gtk_dialog_run(dlg));
          gtk_widget_hide_all(GTK_WIDGET(dlg));
          dlg = NULL;
        }
        gdk_threads_leave();
        break;

      default:
        printf("\nFuncao.: 0x%08x\n", msg.fnc);
        printf("\tds.....: 0x%08x\n", msg.data.ds     );
        printf("\tled....: 0x%08x\n", msg.data.led    );
        printf("\tad.vin.: 0x%08x\n", msg.data.ad.vin );
        printf("\tad.term: 0x%08x\n", msg.data.ad.term);
        printf("\tds.vbat: 0x%08x\n", msg.data.ad.vbat);
      }
    }
  }

  return NULL;
}

//Inicia a aplicacao
int main(int argc, char *argv[])
{
  pthread_t tid;
  GtkWidget *wnd;
  GtkComboBox *cmb;
  char *campos_log   [] = { "Data", "Usuário", "Evento", "" };
  char *campos_tarefa[] = { "Número", "Cliente", "Pedido", "Modelo", "Total", "Produzidas", "Tamanho", "Data", "Comentários", "" };

#ifdef DEBUG_PC
  ps = SerialInit("/dev/ttyUSB0");
#else
  ps = SerialInit("/dev/ttymxc2");
#endif

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

// Inicializacao do ModBus
  mbdev.identification.Id = 0x02;
  mbdev.hl                = NULL;
  mbdev.hl_size           = 0;
  mbdev.mode              = MB_MODE_MASTER;
  mbdev.TX                = IHM_MB_TX;

  // Limpa a estrutura do banco, zerando ponteiros, etc...
  DB_Clear(&mainDB);

  // Carrega configuracoes do arquivo de configuracao e conecta ao banco
  if(!DB_LerConfig(&mainDB, DB_ARQ_CONFIG)) // Se ocorrer erro abrindo o arquivo, carrega defaults
    {
    mainDB.server  = "192.168.0.2";
    mainDB.user    = "root";
    mainDB.passwd  = "y1cGH3WK20";
    mainDB.nome_db = "cv";
    }

  /* init threads */
  g_thread_init (NULL);
  gdk_threads_init ();

  gdk_threads_enter();

  gtk_init( &argc, &argv );

  //Carrega a interface a partir do arquivo glade
  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "IHM.glade", NULL);
  wnd = GTK_WIDGET(gtk_builder_get_object(builder, "wndLogin"));
  cmb = GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbLoginUser"));

  //Conecta Sinais aos Callbacks
  gtk_builder_connect_signals(builder, NULL);

//  g_object_unref (G_OBJECT (builder));

  // Configura TreeView da tela de Tarefas
  TV_Config(GTK_WIDGET(gtk_builder_get_object(builder, "tvwTarefas")), campos_tarefa,
    GTK_TREE_MODEL(gtk_list_store_new((sizeof(campos_tarefa)/sizeof(campos_tarefa[0]))-1,
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING)));

  // Configura TreeView da tela de Logs
  TV_Config(GTK_WIDGET(gtk_builder_get_object(builder, "tvwLog")), campos_log,
      GTK_TREE_MODEL(gtk_list_store_new((sizeof(campos_log)/sizeof(campos_log[0]))-1,
          G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING)));

  // Configura o ComboBoxEntry para usar o modo de texto. Pelo Glade nao funciona!
  gtk_combo_box_entry_set_text_column(GTK_COMBO_BOX_ENTRY(gtk_builder_get_object(builder, "cbeTarefaCliente")), 0);

  gtk_widget_show_all(wnd);

  // Iniciando os timers
  g_timeout_add(500, tmrAD, NULL);

  pthread_create (&tid, NULL, ihm_update, NULL);

  if(DB_Init(&mainDB)) { // Se inicializar o banco, entra no loop do GTK.
    // Carregamento no ComboBox dos usuários cadastrados no MySQL.
    DB_Execute(&mainDB, 0, "select login from usuarios order by ID");
    CarregaCombo(cmb,0, NULL);
  } else {
    MessageBox("Erro inicializando banco de dados");
    // Carregamento de usuário Master para acesso de emergência.
    CarregaItemCombo(cmb, "Master");
    gtk_combo_box_set_active(cmb, 0);
  }

  gtk_main(); //Inicia o loop principal de eventos (GTK MainLoop)

  SerialClose(ps);
  DB_Close(&mainDB);

  gdk_threads_leave();

  return 0;
}
