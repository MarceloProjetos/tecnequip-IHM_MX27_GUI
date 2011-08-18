#include "defines.h"
#include <gtk/gtk.h>
#include "GtkUtils.h"
#include <time.h>
#include <sys/time.h>

extern SERIAL_TX_FNC(SerialTX);
extern SERIAL_RX_FNC(SerialRX);

extern void AbrirLog   ();
extern void AbrirOper  ();
extern void AbrirConfig(unsigned int pos);

struct MB_Device mbdev;
struct strDB     mainDB;
extern int idUser; // Indica usuário que logou se for diferente de zero.

pthread_mutex_t mutex_ipcmq_rd  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_ipcmq_wr  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_gui_lock  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_comm_lock = PTHREAD_MUTEX_INITIALIZER;

// Função que salva um log no banco contendo usuário e data que gerou o evento.
void Log(char *evento, int tipo)
{
  char sql[200];

  if(mainDB.res != NULL && idUser) // Banco conectado
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

        if ( ((ptrp = getprotobyname("tcp"))) == NULL) {
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
#ifndef DEBUG_PC
  return Transmitir(ps, (char *)data, size, 0);
#else
  return 0;
#endif
}

COMM_FNC(CommRX)
{
#ifndef DEBUG_PC
  return Receber(ps, (char *)data, size);
#else
  return 0;
#endif
}

MB_HANDLER_TX(IHM_MB_TX)
{
  struct timeval tv;
  static struct timeval tv_last;
  static uint32_t primeiro = 1;

  uint32_t i, tent = 50;
  int32_t resp, wait_usec, wait;

#ifdef DEBUG_PC_NOETH
  return 0;
#endif

  if(primeiro) {
    primeiro = 0;
    gettimeofday(&tv_last, NULL);
  }

  gettimeofday(&tv, NULL);
  wait_usec =  tv.tv_usec - tv_last.tv_usec;
  wait      = (tv.tv_sec  - tv_last.tv_sec) * 1000000;

  if(wait_usec < 0)
    wait += 1000000;
  wait += wait_usec;

//  if(wait < 250000 && wait > 0)
//    usleep(250000 - wait);

  gettimeofday(&tv, NULL);
  printf("\n%3d.%04ld - MB Send: ", (int)tv.tv_sec, (long)tv.tv_usec);
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
    gettimeofday(&tv, NULL);
    printf("%3d.%04ld - Tempo para resposta esgotado...\n", (int)tv.tv_sec, (long)tv.tv_usec);
  } else {
    size = resp;
    gettimeofday(&tv, NULL);
    printf("%3d.%04ld - Retorno de MB Send: ", (int)tv.tv_sec, (long)tv.tv_usec);
    for(i=0; i<size; i++)
      printf("%02x ", data[i]);
    printf("\n");
  }

  tv_last = tv;

  return size;
}

// Objeto que contem toda a interface GTK
GtkBuilder *builder;

// Variavel indicando que a tela de desligamento esta ativada
uint32_t OnPowerDown = 0;

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
  if(WorkAreaGet() == NTB_ABA_HOME || WorkAreaGet() == NTB_ABA_MANUT) {
    pthread_mutex_lock  (&mutex_comm_lock);
    comm_put(&(struct comm_msg){ COMM_FNC_AIN, { 0x0 } });
    pthread_mutex_unlock(&mutex_comm_lock);
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
      if(cmsg.fnc == COMM_FNC_PWR && cmsg.data.pwr) {
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

  while(IPCMQ_Main_Receber(&rcv, 0) >= 0) {
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
}

char *lista_botoes[] = {
    "btnExecIniciarParar",
    "",
};

gboolean cbDesenharMaquina(GtkWidget *widget, GdkEventExpose *event, gpointer data);

gboolean tmrGtkUpdate(gpointer data)
{
  time_t now;
  char tmp[40], *msg_error;
  static uint32_t last_flags = -1, ult_aviso_lub = -1;
  uint32_t val, i, ciclos_prensa, estado, current_flags = 0;
  GtkWidget *wdg;
  struct tm *timeptr;
  static GtkLabel *lbl = NULL;
  static GdkPixbuf *pb_on = NULL, *pb_off = NULL;
  static int ciclos = 0, current_status = 0, last_status = 0, estado_init = -1;

  if(!OnPowerDown) {
    if(pb_on == NULL) { // Inicializa pixbufs
      pb_on  = gdk_pixbuf_new_from_file("images/ihm-status-on.png" , NULL);
      pb_off = gdk_pixbuf_new_from_file("images/ihm-status-off.png", NULL);
    }

    if(ciclos++ == 4) {
      ciclos = 0;

      // Leitura do estado dos CLPs, exibindo mensagem de erro caso houver
      current_status = MaqLerErros();
      if(last_status != current_status && current_status) { // houve mudanca e com erro
        msg_error = MaqStrErro(current_status);
        Log(msg_error, LOG_TIPO_ERRO);
        gtk_label_set_label(GTK_LABEL(gtk_builder_get_object(builder, "lblMensagens" )), msg_error);
        gtk_label_set_label(GTK_LABEL(gtk_builder_get_object(builder, "lblMessageBox")), msg_error);
        WorkAreaGoTo(NTB_ABA_MESSAGEBOX);
        // Se o erro não for apenas um alerta, desativa a máquina.
        if(current_status & ~MAQ_ERRO_ALERTAS)
          MaqLiberar(0);
      } else if (last_status != current_status) {
        gtk_label_set_label(GTK_LABEL(gtk_builder_get_object(builder, "lblMensagens" )), "Sem Erros");
        MaqLiberar(1);
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

      if(idUser) { // Apenas realiza leitura de ciclos depois do login
        ciclos_prensa = MaqLerPrsCiclos();
        if(maq_param.prensa.ciclos != ciclos_prensa) {
          maq_param.prensa.ciclos = ciclos_prensa;
          if(ult_aviso_lub > maq_param.prensa.ciclos) {
            if(maq_param.prensa.ciclos_lub > 0) {
              ult_aviso_lub = maq_param.prensa.ciclos - (maq_param.prensa.ciclos % maq_param.prensa.ciclos_lub);
            } else {
              ult_aviso_lub = 0;
            }
          }

          printf("Último aviso de lubrificação: %d\n", ult_aviso_lub);
          printf("Ciclos atual: %d\n", maq_param.prensa.ciclos);
          printf("Próximo aviso de lubrificação: %d\n", ult_aviso_lub+maq_param.prensa.ciclos_lub);

          sprintf(tmp, "%d", maq_param.prensa.ciclos);
          gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblConfigPrsCiclos")), tmp);

          if(maq_param.prensa.ciclos >= ult_aviso_lub+maq_param.prensa.ciclos_lub) {
            ult_aviso_lub = maq_param.prensa.ciclos;
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "lblExecAvisoLub")), 1);
          } else if(maq_param.prensa.ciclos >= ult_aviso_lub + (maq_param.prensa.ciclos_lub/10)) {
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "lblExecAvisoLub")), 0);
          }

          if(maq_param.prensa.ciclos >= maq_param.prensa.ciclos_ferram) {
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "lblExecAvisoFerram")), 1);
          } else {
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "lblExecAvisoFerram")), 0);
          }

          if(!(maq_param.prensa.ciclos%50))
            MaqGravarConfig();
        }
      }
    } else if(ciclos == 1) { // Divide as tarefas nos diversos ciclos para nao sobrecarregar
      estado = MaqLerEstado();
      val = estado & MAQ_STATUS_PRONTA ? TRUE : FALSE;
      if(val != estado_init) {
        estado_init = val;
        for(i=0; lista_botoes[i][0] != 0; i++) {
          wdg = GTK_WIDGET(gtk_builder_get_object(builder, lista_botoes[i]));
          gtk_widget_set_sensitive(wdg, estado_init);
        }
      }

      // Leitura das flags de estado da máquina para atualização da imagem na tela principal
      current_flags = (estado >> 10) & 0x7;
      if(last_flags != current_flags) {
        last_flags = current_flags;
        printf("current_flags = %d\n", current_flags);
        cbDesenharMaquina(NULL, NULL, (gpointer)(&current_flags));
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
  const gchar *nome = gtk_buildable_get_name(GTK_BUILDABLE(button));
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

  case NTB_ABA_MANUT:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbManut")), 0);
    gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "btnManutVoltar")), 0);
    break;

  case NTB_ABA_LOGS:
    AbrirLog();
    break;
  }
}

void cbLogoff(GtkButton *button, gpointer user_data)
{
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

  WorkAreaGoTo(NTB_ABA_LOGIN);
}

/****************************************************************************
 * Thread de comunicacao com o LPC2109 (Power Management) e CLPs (ModBus)
 ***************************************************************************/

void * ihm_update(void *args)
{
  char tmp[25];
  int32_t  batt_level, curr_batt_level = -1;
  uint32_t ad_vin=-1, ad_vbat=-1, i, ciclos = 0;
#ifndef DEBUG_PC
  uint32_t ad_term=-1, rp, ChangedAD = 0;
  struct comm_msg msg;
#endif
  GtkDialog      *dlg;
  GtkLabel       *lbl;
  GtkProgressBar *pgbVIN, *pgbTERM, *pgbVBAT;
  GtkImage       *imgBatt;
  GdkPixbuf      *pbBatt[4];

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
  imgBatt = GTK_IMAGE       (gtk_builder_get_object(builder, "imgBateria"  ));
  lbl     = GTK_LABEL       (gtk_builder_get_object(builder, "lblTemp"     ));
  dlg     = GTK_DIALOG      (gtk_builder_get_object(builder, "dlgPowerDown"));

  for(i=0; i<4; i++) {
    sprintf(tmp, "images/ihm-battery-%d.png", i);
    pbBatt[i] = gdk_pixbuf_new_from_file(tmp, NULL);
  }

  gdk_threads_leave();

  /****************************************************************************
   * Loop
   ***************************************************************************/
  while (1) {
    usleep(500);
    /*** Loop de checagem de mensagens vindas da CPU LPC2109 ***/
#ifndef DEBUG_PC
    comm_update();
    if(comm_ready()) {
      comm_get(&msg);
      switch(msg.fnc) {
      case COMM_FNC_AIN: // Resposta do A/D. Divide por 3150 pois representa a tensao em mV.
        if(!pthread_mutex_trylock(&mutex_gui_lock)) {
          gdk_threads_enter();

          sprintf(tmp, "%.01f °C", (float)(msg.data.ad.temp)/1000);
          if(strcmp(tmp, gtk_label_get_text(lbl)))
            gtk_label_set_text(lbl, tmp);

          if(msg.data.ad.vin != ad_vin) {
            ChangedAD = 1;
            ad_vin = msg.data.ad.vin;
          }

          if(msg.data.ad.term != ad_term) {
            ChangedAD = 1;
            ad_term = msg.data.ad.term;
          }
          if(msg.data.ad.vbat != ad_vbat) {
            ChangedAD = 1;
            ad_vbat = msg.data.ad.vbat;
            // Bateria com tensao inferior a 3V, sistema deve desligar!
            if(ad_vbat < 3000 && ad_vin < 8000)
              gtk_main_quit();
          }

          if((WorkAreaGet() == NTB_ABA_MANUT) && ChangedAD) {
            ChangedAD = 0;

            gtk_progress_bar_set_fraction(pgbVIN , (gdouble)(ad_vin)/35000);
            sprintf(tmp, "%.02f Volts", (gdouble)(ad_vin)/1000);
            gtk_progress_bar_set_text(pgbVIN , tmp);

            gtk_progress_bar_set_fraction(pgbTERM, (gdouble)(ad_term)/3150);
            sprintf(tmp, "%.02f Volts", (gdouble)(ad_term)/1000);
            gtk_progress_bar_set_text(pgbTERM , tmp);

            gtk_progress_bar_set_fraction(pgbVBAT, (gdouble)(ad_vbat)/4500);
            sprintf(tmp, "%.02f Volts", (gdouble)(ad_vbat)/1000);
            gtk_progress_bar_set_text(pgbVBAT , tmp);
          }

          gdk_threads_leave();
          pthread_mutex_unlock(&mutex_gui_lock);
        }
        break;

      case COMM_FNC_PWR:
        gdk_threads_enter();
        if(msg.data.pwr) {
          printf("Sistema energizado");

          // Avisa o LPC2109 que nao deve manter o sistema ligado caso haja nova queda de energia.
          pthread_mutex_lock  (&mutex_comm_lock);
          comm_put(&(struct comm_msg){ COMM_FNC_PWR, { 0x0 } });
          pthread_mutex_unlock(&mutex_comm_lock);
        } else {
          OnPowerDown = 1;
          printf("Sistema sem energia");

          MaqGravarConfig();

          gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblPowerDownMsg")),
              "30 segundos");
          gtk_widget_show_all(GTK_WIDGET(dlg));
          rp = gtk_dialog_run(dlg);
          printf("Resposta: %d\n", rp);
          if(!rp) // Clicado ok, desligar!
            gtk_main_quit();
          gtk_widget_hide_all(GTK_WIDGET(dlg));

          if(rp != 2) { // 2: energia reestabelecida, nao pedir para manter ligado
            // Para manter o sistema ligado, devemos informar o LPC2109
            pthread_mutex_lock  (&mutex_comm_lock);
            comm_put(&(struct comm_msg){ COMM_FNC_PWR, { 0x1 } });
            pthread_mutex_unlock(&mutex_comm_lock);
          }

          OnPowerDown = 0;
        }
        gdk_threads_leave();
        break;

      case COMM_FNC_ON: // Pressionado botao ON/OFF, termina o programa
        gdk_threads_enter();
        gtk_main_quit();
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
#endif
    /*** Loop para atualizacao da imagem da bateria ***/

    if(ciclos++ > 100) {
      ciclos = 0;

      // Tensao superior a 8 volts indica presenca de alimentacao externa
      if(ad_vin > 8000) {
        batt_level = curr_batt_level-1; // Cicla entre as imagens
        if(batt_level < 0) {
          batt_level = 3;
        }
      } else { // Sistema alimentado pela bateria, calcula seu nivel atual
        batt_level = 3999 - ad_vbat;
        if(batt_level < 0) {
          batt_level = 0;
        } else if(batt_level > 1000) {
          batt_level = 1000;
        }
        batt_level /= 250; // Nivel entre 0 (cheia) e 3 (vazia).
      }

      if(curr_batt_level != batt_level) { // Alterado o nivel, atualiza imagem
        curr_batt_level = batt_level;
#ifndef DEBUG_PC
        gtk_image_set_from_pixbuf(imgBatt, pbBatt[curr_batt_level]);
#endif
      }
    }

    /*** Fim do loop para atualizacao da imagem da bateria ***/

    // Loop de checagem de mensagens vindas da thread principal
    if(IPCMQ_Threads_Receber(&ipc_msg) >= 0) {
      switch(ipc_msg.mtype) {
      case IPCMQ_FNC_TEXT:
        strcpy(ipc_msg.data.text, "resposta");
        IPCMQ_Threads_Enviar(&ipc_msg);
        break;

      case IPCMQ_FNC_MODBUS:
        printf("Enviando msg modbus\n");
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
  uint32_t ret = 0;
#ifndef DEBUG_PC_NOETH
  int32_t opts;
#endif
  pthread_t tid;
  GSList *lst;
  GtkWidget *wnd;
  GtkComboBox *cmb;
  char *campos_log   [] = { "Data", "Usuário", "Evento", "" };

  /* init threads */
  g_thread_init (NULL);
  gdk_threads_init ();

  gdk_threads_enter();

  gtk_init( &argc, &argv );

  //Carrega a interface a partir do arquivo glade
  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "IHM.glade", NULL);
  wnd = GTK_WIDGET(gtk_builder_get_object(builder, "wndDesktop"));
  cmb = GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbLoginUser"));

  //Conecta Sinais aos Callbacks
  gtk_builder_connect_signals(builder, NULL);

  // Carrega os nomes dos widgets do gtkbuilder para poderem ser usados nos temas
  lst = gtk_builder_get_objects(builder);
  while(lst != NULL) {
    if(GTK_IS_WIDGET(lst->data))
      gtk_widget_set_name(GTK_WIDGET(lst->data), gtk_buildable_get_name(GTK_BUILDABLE(lst->data)));
    lst = lst->next;
 }

//  g_object_unref (G_OBJECT (builder));
  gtk_rc_parse("gtk.rc");

  // Configura TreeView da tela de Logs
  TV_Config(GTK_WIDGET(gtk_builder_get_object(builder, "tvwLog")), campos_log,
      GTK_TREE_MODEL(gtk_list_store_new((sizeof(campos_log)/sizeof(campos_log[0]))-1,
          G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING)));

  // Cria filas de mensagens para comunicacao entre a thread ihm_update e o main
  fd_rd = msgget(IPC_PRIVATE, IPC_CREAT);
  if(fd_rd < 0) {
    printf("erro criando fila fd_rd (%d): %s\n", errno, strerror(errno));
    ret = 1;
    goto fim_fila_rd;
  }

  fd_wr = msgget(IPC_PRIVATE, IPC_CREAT);
  if(fd_wr < 0) {
    printf("erro criando fila fd_wr (%d): %s\n", errno, strerror(errno));
    ret = 2;
    goto fim_fila_wr;
  }

#ifndef DEBUG_PC
  ps = SerialInit("/dev/ttymxc2");

  if(ps == NULL) {
    printf("Erro abrindo porta serial!\n");
    ret = 3;
    goto fim_serial;
  }

  SerialConfig(ps, 115200, 8, 1, SerialParidadeNenhum, 0);

  ps->txf = SerialTX;
  ps->rxf = SerialRX;

  comm_init(CommTX, CommRX);
  comm_put (&(struct comm_msg){ COMM_FNC_LED, { 0xA } });
  comm_put (&(struct comm_msg){ COMM_FNC_AIN, { 0x0 } });
#endif

// Inicializacao do ModBus
  mbdev.identification.Id = 0x02;
  mbdev.hl                = NULL;
  mbdev.hl_size           = 0;
  mbdev.mode              = MB_MODE_MASTER;
  mbdev.TX                = IHM_MB_TX;

#ifndef DEBUG_PC_NOETH
  tcp_socket = ihm_connect("192.168.0.235", 502);
  if(tcp_socket >= 0) {
    // Configura socket para o modo non-blocking e retorna se houver erro.
    opts = fcntl(tcp_socket,F_GETFL);
    if (opts < 0) {
      ret = 4;
      goto fim_opt;
    }
    if (fcntl(tcp_socket, F_SETFL, opts | O_NONBLOCK) < 0) {
      ret = 5;
      goto fim_opt;
    }
  } else {
    ret = 6;
    goto fim_tcp;
  }
#endif

  if(!MaqLerConfig()) {
    printf("Erro carregando configuracao\n");
    ret = 7;
    goto fim_config;
  }

  // Limpa a estrutura do banco, zerando ponteiros, etc...
  DB_Clear(&mainDB);

#ifndef DEBUG_PC
  // Carrega configuracoes do arquivo de configuracao e conecta ao banco
  if(!DB_LerConfig(&mainDB, DB_ARQ_CONFIG)) // Se ocorrer erro abrindo o arquivo, carrega defaults
  {
  mainDB.server  = "127.0.0.1";
  mainDB.user    = "root";
  mainDB.passwd  = "y1cGH3WK20";
  mainDB.nome_db = "cv";
  }
#else
  mainDB.server  = "interno.tecnequip.com.br";
  mainDB.user    = "root";
  mainDB.passwd  = "y1cGH3WK20";
  mainDB.nome_db = "cv";
#endif

  WorkAreaGoTo(NTB_ABA_LOGIN);
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

  gtk_main(); //Inicia o loop principal de eventos (GTK MainLoop)

  DB_Close(&mainDB);

// A partir deste ponto iniciam os desligamentos. Em caso de erro na inicializacao, o programa
// salta para o label correspondente a etapa que falhou para desfazer o que ja havia sido feito

fim_config: // Encerrando por erro de configuracao
  MaqGravarConfig();

#ifndef DEBUG_PC_NOETH
fim_opt: // Encerrando por erro na configuracao da conexao TCP
  close(tcp_socket);

fim_tcp: // Encerrando por falha ao tentar estabelecer a conexao TCP
#endif
#ifndef DEBUG_PC
  SerialClose(ps);

fim_serial: // Encerrando por erro ao abrir a porta serial
#endif
  msgctl(fd_wr, IPC_RMID, NULL);

fim_fila_wr: // Encerrando por falha ao criar fila de mensagens para escrita
  msgctl(fd_rd, IPC_RMID, NULL);

fim_fila_rd: // Encerrando por falha ao criar fila de mensagens para leitura
  pthread_mutex_destroy(&mutex_ipcmq_wr);
  pthread_mutex_destroy(&mutex_ipcmq_rd);

  return ret;
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
