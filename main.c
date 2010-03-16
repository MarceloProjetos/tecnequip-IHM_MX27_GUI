#include "defines.h"
#include <gtk/gtk.h>
#include "GtkUtils.h"
#include <time.h>

extern SERIAL_TX_FNC(SerialTX);
extern SERIAL_RX_FNC(SerialRX);

extern void AbrirLog   ();
extern void AbrirOper  ();
extern void AbrirConfig(unsigned int pos);

struct MB_Device mbdev;
struct strDB     mainDB;
extern int idUser; // Indica usuário que logou se for diferente de zero.

pthread_mutex_t mutex_ipcmq_rd = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_ipcmq_wr = PTHREAD_MUTEX_INITIALIZER;

// Função que salva um log no banco contendo usuário e data que gerou o evento.
void Log(char *evento, int tipo)
{
  char sql[200];

  if(mainDB.res != NULL) // Banco conectado
    {
    sprintf(sql, "insert into log (ID_Usuario, Tipo, Evento) values ('%d', '%d', '%s')", idUser, tipo, evento);
    DB_Execute(&mainDB, 3, sql);
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
int32_t tcp_socket = -1;

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
  uint32_t i, tent = 50;
  int32_t resp;

  printf("MB Send: ");
  for(i=0; i<size; i++)
    printf("%02x ", data[i]);
  printf("\n");

// Envia a mensagem pela ethernet
  send(tcp_socket, data, size, 0);

  while((resp=recv(tcp_socket, data, MB_BUFFER_SIZE, 0))<=0 && tent--) {
    usleep(10000);
  }

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

  return size;
}

// Objeto que contem toda a interface GTK
GtkBuilder *builder;

extern key_t fd_rd;
extern key_t fd_wr;

void IPCMQ_Main_Enviar(struct strIPCMQ_Message *msg)
{
  pthread_mutex_lock(&mutex_ipcmq_wr);
  msgsnd(fd_wr, msg, IPCMQ_MESSAGE_SIZE, 0);
  pthread_mutex_unlock(&mutex_ipcmq_wr);
}

int IPCMQ_Main_Receber(struct strIPCMQ_Message *msg, int tipo)
{
  int ret;

  pthread_mutex_lock(&mutex_ipcmq_rd);
  ret = msgrcv(fd_rd, msg, IPCMQ_MESSAGE_SIZE, tipo, IPC_NOWAIT);
  pthread_mutex_unlock(&mutex_ipcmq_rd);

  return ret;
}

void IPCMQ_Threads_Enviar(struct strIPCMQ_Message *msg)
{
  pthread_mutex_lock(&mutex_ipcmq_rd);
  msgsnd(fd_rd, msg, IPCMQ_MESSAGE_SIZE, 0);
  pthread_mutex_unlock(&mutex_ipcmq_rd);
}

int IPCMQ_Threads_Receber(struct strIPCMQ_Message *msg)
{
  int ret;

  pthread_mutex_lock(&mutex_ipcmq_wr);
  ret = msgrcv(fd_wr, msg, IPCMQ_MESSAGE_SIZE, 0, IPC_NOWAIT);
  pthread_mutex_unlock(&mutex_ipcmq_wr);

  return ret;
}

// Timers
gboolean tmrAD(gpointer data)
{
  if(WorkAreaGet() == NTB_ABA_HOME) {
    comm_put(&(struct comm_msg){ COMM_FNC_AIN, { 0x0 } });
  }

  return TRUE;
}

gboolean tmrPowerDown(gpointer data)
{
  char msg[15];
  struct comm_msg cmsg;
  static uint32_t timeout=30;
  static GtkDialog *dlg = NULL; // Recebe o ponteiro como parametro nao o endereco apontado.

  gdk_threads_enter();

  if(dlg == NULL)
    dlg = GTK_DIALOG(gtk_builder_get_object(builder, "dlgPowerDown"));

  if(!GTK_WIDGET_VISIBLE(GTK_WIDGET(dlg))) {
    timeout = 30;
  } else if(!timeout--) {
    gtk_dialog_response(dlg, 0);
  } else {
    sprintf(msg, "%d segundos", timeout);
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblPowerDownMsg")), msg);

    comm_update();
    if(comm_ready()) {
      comm_get(&cmsg);
      if(cmsg.data.pwr) {
        timeout = 30;
        gtk_dialog_response(dlg, 2);
      }
    }
  }

  gdk_threads_leave();

  return TRUE;
}

void IPC_Update(void)
{
  struct strIPCMQ_Message rcv;

  while(IPCMQ_Main_Receber(&rcv, 0) >= 0)
    switch(rcv.mtype) {
    case IPCMQ_FNC_TEXT:
      printf("Recebida mensagem de texto: %s\n", rcv.data.text);
      break;

    case IPCMQ_FNC_MODBUS:
      switch(rcv.data.modbus_reply.FunctionCode) {
      case MB_FC_READ_DEVICE_IDENTIFICATION:
        printf("Identificador %d: %s\n", rcv.data.modbus_reply.reply.read_device_identification.object_id, rcv.data.modbus_reply.reply.read_device_identification.data);
        break;
      default:
        printf("Funcao desconhecida do modbus: %d\n", rcv.data.modbus_reply.FunctionCode);
      }

       if(rcv.fnc != NULL) {
        (*rcv.fnc)(&rcv.data, rcv.res);
      }

      break;

    default:
      printf("Mensagem de tipo desconhecido: %ld\n", rcv.mtype);
    }
}

gboolean tmrGtkUpdate(gpointer data)
{
  time_t now;
  char tmp[40];
  uint32_t val, i;
  GtkWidget *wdg;
  struct tm *timeptr;
  static GtkLabel *lbl = NULL;
  static GdkPixbuf *pb_on = NULL, *pb_off = NULL;
  static int ciclos = 0, current_status = 0, last_status = 0;

  if(pb_on == NULL) { // Inicializa pixbufs
    pb_on  = gdk_pixbuf_new_from_file("images/ihm-status-on.png" , NULL);
    pb_off = gdk_pixbuf_new_from_file("images/ihm-status-off.png", NULL);
  }

  if(ciclos++ == 4) {
    ciclos = 0;

    // Leitura do estado dos CLPs, exibindo mensagem de erro caso houver
    current_status = MaqLerStatus();
    if(last_status != current_status && current_status) { // houve mudanca e com erro
      gtk_label_set_label(GTK_LABEL(gtk_builder_get_object(builder, "lblMessageBox")), MaqStrErro(current_status));
      WorkAreaGoTo(NTB_ABA_MESSAGEBOX);
    }
    last_status = current_status;

    // Atualiza a hora da tela inicial
    if(lbl == NULL)
      lbl = GTK_LABEL(gtk_builder_get_object(builder, "lblHora"));

    now = time(NULL);
    timeptr = localtime(&now);

    sprintf(tmp, "%02d/%02d/%d, %.2d:%.2d",
        timeptr->tm_mday,
        timeptr->tm_mon + 1,
        1900 + timeptr->tm_year,
        timeptr->tm_hour,
        timeptr->tm_min);
    if(strcmp(tmp, gtk_label_get_text(lbl)))
      gtk_label_set_label(lbl, tmp);
  } else if(ciclos == 3) { // Divide as tarefas nos diversos ciclos para nao sobrecarregar
    if(WorkAreaGet() == NTB_ABA_MANUT) {
      val = MaqLerSaidas();
      for(i=0;;i++) { // Loop eterno, finaliza quando acabarem as saidas
        sprintf(tmp, "tglManutSai%02d", i);
        wdg = GTK_WIDGET(gtk_builder_get_object(builder, tmp));
        if(wdg == NULL) // Acabaram as saidas
          break; // Sai do loop

        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wdg), (val>>i)&1);
      }
    }
  } else if(ciclos == 2) { // Divide as tarefas nos diversos ciclos para nao sobrecarregar
    if(WorkAreaGet() == NTB_ABA_MANUT) {
      val = MaqLerEntradas();
      for(i=0;;i++) { // Loop eterno, finaliza quando acabarem as entradas
        sprintf(tmp, "imgStatusEnt%02d", i);
        wdg = GTK_WIDGET(gtk_builder_get_object(builder, tmp));
        if(wdg == NULL) // Acabaram as saidas
          break; // Sai do loop

        gtk_image_set_from_pixbuf(GTK_IMAGE(wdg), (val>>i)&1 ? pb_on : pb_off);
      }
    }
  }

  IPC_Update();

  return TRUE;
}

/****************************************************************************
 * Funcoes que tratam os dados recebidos do Modbus
 ***************************************************************************/
void ReadID(void *dt, void *res)
{
  union uniIPCMQ_Data *data = (union uniIPCMQ_Data *)dt;

  printf("recebido: %s\n", data->modbus_reply.reply.read_device_identification.data);
}

/****************************************************************************
 * Callbacks gerais do GTK
 ***************************************************************************/

// Seleciona a aba correspondente ao botao clicado
void cbFunctionKey(GtkButton *button, gpointer user_data)
{
  uint32_t idx;
  const gchar *nome = gtk_widget_get_name(GTK_WIDGET(button));
  struct strIPCMQ_Message ipc_msg;

  ipc_msg.mtype = IPCMQ_FNC_MODBUS;
  ipc_msg.fnc   = ReadID;
  ipc_msg.res   = NULL;
  ipc_msg.data.modbus_query.function_code = MB_FC_READ_DEVICE_IDENTIFICATION;
  ipc_msg.data.modbus_query.data.read_device_identification.id_code   = 0x01;
  ipc_msg.data.modbus_query.data.read_device_identification.object_id = 0x01;
  IPCMQ_Main_Enviar(&ipc_msg);

  idx = nome[strlen(nome)-1]-'0';
  WorkAreaGoTo(idx);

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

void cbLogoff(GtkButton *button, gpointer user_data)
{
  GtkWidget *wnd = GTK_WIDGET(gtk_builder_get_object(builder, "wndLogin"));
  struct strIPCMQ_Message ipc_msg;

  ipc_msg.mtype = IPCMQ_FNC_TEXT;
  ipc_msg.data.text[0] = 0;
  IPCMQ_Main_Enviar(&ipc_msg);

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
 * Thread de comunicacao com o LPC2109 (Power Management) e CLPs (ModBus)
 ***************************************************************************/

void * ihm_update(void *args)
{
  uint32_t ad_vin=-1, ad_term=-1, ad_vbat=-1, rp;
  struct comm_msg msg;
  GtkDialog      *dlg;
  GtkStatusbar   *sts;
  GtkProgressBar *pgbVIN, *pgbTERM, *pgbVBAT;

  struct strIPCMQ_Message ipc_msg;

  ipc_msg.fnc   = NULL;
  ipc_msg.res   = NULL;
  ipc_msg.mtype = IPCMQ_FNC_TEXT;
  strcpy(ipc_msg.data.text, "mensagem");
  IPCMQ_Threads_Enviar(&ipc_msg);

  gdk_threads_enter();

  pgbVIN  = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "pgbVIN"      ));
  pgbTERM = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "pgbTERM"     ));
  pgbVBAT = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "pgbVBAT"     ));
  sts     = GTK_STATUSBAR   (gtk_builder_get_object(builder, "stsMensagem" ));
  dlg     = GTK_DIALOG      (gtk_builder_get_object(builder, "dlgPowerDown"));

  gdk_threads_leave();

  /****************************************************************************
   * Loop
   ***************************************************************************/
  while (1) {
    usleep(500);
    // Loop de checagem de mensagens vindas da CPU LPC2109
    comm_update();
    if(comm_ready()) {
      comm_get(&msg);
      switch(msg.fnc) {
      case COMM_FNC_AIN: // Resposta do A/D. Divide por 3150 pois representa a tensao em mV.
        if(WorkAreaGet() == NTB_ABA_HOME) {
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

          gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblPowerDownMsg")),
              "30 segundos");
          gtk_widget_show_all(GTK_WIDGET(dlg));
          rp = gtk_dialog_run(dlg);
          printf("Resposta: %d\n", rp);
          if(!rp) // Clicado ok, desligar!
            gtk_main_quit();
          gtk_widget_hide_all(GTK_WIDGET(dlg));
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

    // Loop de checagem de mensagens vindas da thread principal
    if(IPCMQ_Threads_Receber(&ipc_msg) >= 0) {
      switch(ipc_msg.mtype) {
      case IPCMQ_FNC_TEXT:
        strcpy(ipc_msg.data.text, "resposta");
        IPCMQ_Threads_Enviar(&ipc_msg);
        break;

      case IPCMQ_FNC_MODBUS:
        ipc_msg.data.modbus_reply = MB_Send(&mbdev,
                                            ipc_msg.data.modbus_query.function_code,
                                            &ipc_msg.data.modbus_query.data);
        IPCMQ_Threads_Enviar(&ipc_msg);

        break;
      }
    }
  }

  return NULL;
}

uint32_t IHM_Init(int argc, char *argv[])
{
  int32_t opts;
  pthread_t tid;
  GtkWidget *wnd;
  GtkComboBox *cmb;
  char *campos_log   [] = { "Data", "Usuário", "Evento", "" };
  char *campos_tarefa[] = { "Número", "Cliente", "Pedido", "Modelo", "Total", "Produzidas", "Tamanho", "Data", "Comentários", "" };

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

  // Cria filas de mensagens para comunicacao entre a thread ihm_update e o main
  fd_rd = msgget(IPC_PRIVATE, IPC_CREAT);
  fd_wr = msgget(IPC_PRIVATE, IPC_CREAT);
  if(fd_wr < 0) {
    printf("erro criando fila fd_wr (%d): %s\n", errno, strerror(errno));
    return 4;
  }

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

#ifndef DEBUG_PC
  tcp_socket = ihm_connect("192.168.0.235", 502);
  if(tcp_socket >= 0) {
    // Configura socket para o modo non-blocking e retorna se houver erro.
    opts = fcntl(tcp_socket,F_GETFL);
    if (opts < 0)
      return 2;
    if (fcntl(tcp_socket, F_SETFL, opts | O_NONBLOCK) < 0)
      return 2;
  } else {
    return 2;
  }
#endif

  if(!MaqLerConfig()) {
    printf("Erro carregando configuracao\n");
    return 3;
  }

  // Limpa a estrutura do banco, zerando ponteiros, etc...
  DB_Clear(&mainDB);

#ifndef DEBUG_PC
  // Carrega configuracoes do arquivo de configuracao e conecta ao banco
  if(!DB_LerConfig(&mainDB, DB_ARQ_CONFIG)) // Se ocorrer erro abrindo o arquivo, carrega defaults
#endif
    {
    mainDB.server  = "192.168.0.2";
    mainDB.user    = "root";
    mainDB.passwd  = "y1cGH3WK20";
    mainDB.nome_db = "cv";
    }

  gtk_widget_show_all(wnd);

  // Iniciando os timers
  g_timeout_add( 500, tmrAD       , NULL);
  g_timeout_add( 500, tmrGtkUpdate, NULL);
  g_timeout_add(1000, tmrPowerDown, NULL);

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

  // Configura a máquina para modo manual.
  MaqConfigModo(MAQ_MODO_MANUAL);

  gtk_main(); //Inicia o loop principal de eventos (GTK MainLoop)

  SerialClose(ps);
  DB_Close(&mainDB);

  pthread_mutex_destroy(&mutex_ipcmq_rd);
  pthread_mutex_destroy(&mutex_ipcmq_wr);

  gdk_threads_leave();

  msgctl(fd_rd, IPC_RMID, NULL);
  msgctl(fd_wr, IPC_RMID, NULL);

  close(tcp_socket);

  MaqGravarConfig();

  return 0;
}

//Inicia a aplicacao
int main(int argc, char *argv[])
{
  char tmp[10];
  GtkWidget *wdg;
  uint32_t ret;

  // Se ocorrer erro abrindo o programa, cria janela para avisar o usuario.
  if((ret = IHM_Init(argc, argv)) != 0) {
    printf("terminou com erro %d\n", ret);
    sprintf(tmp, "%02d", ret);

    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblInitErrorCode")), tmp);
    wdg = GTK_WIDGET(gtk_builder_get_object(builder, "wndInitError"));
    gtk_widget_show_all(wdg);

    gtk_main(); //Inicia o loop principal de eventos (GTK MainLoop)
  }

  return ret;
}
