#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <gtk/gtk.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <net/modbus.h>
#include "serial.h"
#include <comm.h>

//#define DEBUG_PC

extern SERIAL_TX_FNC(SerialTX);
extern SERIAL_RX_FNC(SerialRX);

struct MB_Device mbdev;

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
  int32_t tcp_socket;

  printf("MB Send: ");
  for(i=0; i<size; i++)
    printf("%02x ", data[i]);
  printf("\n");

// Envia a mensagem pela ethernet
  tcp_socket = ihm_connect("192.168.0.232", 502);
  if(tcp_socket >= 0) {
    send(tcp_socket, data, size, 0);
    while(!(size=recv(tcp_socket, data, MB_BUFFER_SIZE, 0)) && tent--) {
      usleep(10000);
    }
    close(tcp_socket);

    printf("Retorno de MB Send: ");
    for(i=0; i<size; i++)
      printf("%02x ", data[i]);
    printf("\n");
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
  comm_put(&(struct comm_msg){ COMM_FNC_AIN, { 0x0 } });
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
 * Callbacks do GTK
 ***************************************************************************/

// Seleciona a aba correspondente ao botao clicado
void cbFunctionKey(GtkButton *button, gpointer user_data)
{
  const gchar *nome = gtk_widget_get_name(GTK_WIDGET(button));
  GtkWidget *ntb = GTK_WIDGET(gtk_builder_get_object(builder, "ntbWorkArea"));
  gtk_notebook_set_current_page(GTK_NOTEBOOK(ntb), nome[strlen(nome)-1]-'0' - 1);
}

// Volta para a aba Home se cancelar desligamento
void cbQuitCancel(GtkButton *button, gpointer user_data)
{
  GtkWidget *ntb = GTK_WIDGET(gtk_builder_get_object(builder, "ntbWorkArea"));
  gtk_notebook_set_current_page(GTK_NOTEBOOK(ntb), 0);
}

// Carrega aba referente aos controles da funcao de codigo escolhida
void cbFunctionCodeChanged(GtkComboBox *widget, gpointer user_data)
{
  GtkNotebook *ntb = GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbFunctionCode"));
  gtk_notebook_set_current_page(ntb, gtk_combo_box_get_active(widget));
}

// Envia o comando selecionado pelo ModBus
void cbModBusSend(GtkButton *button, gpointer user_data)
{
  guint fc;
  uint8_t out[] = { 0, 0 };
  uint32_t i, offset=0;
  char wdgName[100], reply_string[2000] = "";
  GtkComboBox     *cbxFunctionCode ;
  GtkSpinButton   *spb1, *spb2, *spb3;
  GtkToggleButton *tgb;
  GtkTreeIter      iter;
  GtkWidget       *mdg, *wdg;

  union MB_FCD_Data data;
  struct MB_Reply rp;

  cbxFunctionCode   = GTK_COMBO_BOX  (gtk_builder_get_object(builder, "cbxFunctionCode"  ));

  gtk_combo_box_get_active_iter(cbxFunctionCode, &iter);
  gtk_tree_model_get(gtk_combo_box_get_model(cbxFunctionCode), &iter, 1, &fc, -1);

  printf("Enviando mensagem pelo ModBus:\n");
  switch(fc) {
  case MB_FC_READ_COILS:
    spb1 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbReadCoilsStart"));
    spb2 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbReadCoilsQuant"));

    data.read_coils.start = (uint32_t)gtk_spin_button_get_value(spb1);
    data.read_coils.quant = (uint32_t)gtk_spin_button_get_value(spb2);

    printf("ReadCoils Start: %02x\n", data.read_coils.start);
    printf("ReadCoils Quant: %02x\n", data.read_coils.quant);

    rp = MB_Send(&mbdev, fc, &data);

    if(!rp.ExceptionCode) {
      offset += sprintf(reply_string+offset, "Retorno:\nFunction Code: %d\n", rp.FunctionCode);
      offset += sprintf(reply_string+offset, "size: %d\n", rp.reply.read_coils.size);
      for(i=0; i<rp.reply.read_coils.size; i++)
        offset += sprintf(reply_string+offset, "%d: %02x\n", i, (&rp.reply.read_coils.val)[i]);
    }

    break;

  case MB_FC_READ_DISCRETE_INPUTS:
    spb1 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbReadDiscreteInputsStart"));
    spb2 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbReadDiscreteInputsQuant"));

    data.read_discrete_inputs.start = (uint32_t)gtk_spin_button_get_value(spb1);
    data.read_discrete_inputs.quant = (uint32_t)gtk_spin_button_get_value(spb2);

    printf("ReadDiscreteInputs Start: %02x\n", data.read_discrete_inputs.start);
    printf("ReadDiscreteInputs Quant: %02x\n", data.read_discrete_inputs.quant);

    rp = MB_Send(&mbdev, fc, &data);

    if(!rp.ExceptionCode) {
      offset += sprintf(reply_string+offset, "Retorno:\nFunction Code: %d\n", rp.FunctionCode);
      offset += sprintf(reply_string+offset, "size: %d\n", rp.reply.read_discrete_inputs.size);
      for(i=0; i<rp.reply.read_discrete_inputs.size; i++)
        offset += sprintf(reply_string+offset, "%d: %02x\n", i, (&rp.reply.read_discrete_inputs.val)[i]);
    }

    break;

  case MB_FC_READ_HOLDING_REGISTERS:
    spb1 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbReadHoldingRegistersStart"));
    spb2 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbReadHoldingRegistersQuant"));

    data.read_holding_registers.start = (uint32_t)gtk_spin_button_get_value(spb1);
    data.read_holding_registers.quant = (uint32_t)gtk_spin_button_get_value(spb2);

    printf("ReadHoldingRegisters Start: %02x\n", data.read_holding_registers.start);
    printf("ReadHoldingRegisters Quant: %02x\n", data.read_holding_registers.quant);

    rp = MB_Send(&mbdev, fc, &data);

    if(!rp.ExceptionCode) {
      offset += sprintf(reply_string+offset, "Retorno:\nFunction Code: %d\n", rp.FunctionCode);
      offset += sprintf(reply_string+offset, "size: %d\n", rp.reply.read_holding_registers.size);
      for(i=0; i<rp.reply.read_holding_registers.size; i++)
        offset += sprintf(reply_string+offset, "%d: %02x\n", i, (&rp.reply.read_holding_registers.val)[i]);
    }

    break;

  case MB_FC_READ_INPUT_REGISTERS:
    spb1 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbReadInputRegistersStart"));
    spb2 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbReadInputRegistersQuant"));

    data.read_input_registers.start = (uint32_t)gtk_spin_button_get_value(spb1);
    data.read_input_registers.quant = (uint32_t)gtk_spin_button_get_value(spb2);

    printf("ReadInputRegisters Start: %02x\n", data.read_input_registers.start);
    printf("ReadInputRegisters Quant: %02x\n", data.read_input_registers.quant);

    rp = MB_Send(&mbdev, fc, &data);

    if(!rp.ExceptionCode) {
      offset += sprintf(reply_string+offset, "Retorno:\nFunction Code: %d\n", rp.FunctionCode);
      offset += sprintf(reply_string+offset, "size: %d\n", rp.reply.read_input_registers.size);
      for(i=0; i<rp.reply.read_input_registers.size; i++)
        offset += sprintf(reply_string+offset, "%d: %02x\n", i, (&rp.reply.read_input_registers.val)[i]);
    }

    break;

  case MB_FC_WRITE_SINGLE_COIL:
    spb1 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbWriteSingleCoilOutput"));
    wdg  = GTK_WIDGET     (gtk_builder_get_object(builder, "rdbWriteSingleCoilOn"    ));

    data.write_single_coil.output = (uint16_t)gtk_spin_button_get_value(spb1);
    data.write_single_coil.val    = (uint8_t )gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wdg));

    printf("WriteSingleCoil Output: %02x. Val: %s\n",
        data.write_single_coil.output, data.write_single_coil.val ? "ON" : "OFF");

    rp = MB_Send(&mbdev, fc, &data);
    if(!rp.ExceptionCode) {
      offset += sprintf(reply_string+offset, "Retorno:\nFunction Code: %d\n", rp.FunctionCode);
      offset += sprintf(reply_string+offset, "output: %d\n", rp.reply.write_single_coil.output);
      offset += sprintf(reply_string+offset, "val   : %s\n", rp.reply.write_single_coil.val ? "ON" : "OFF");
    }

    break;

  case MB_FC_WRITE_SINGLE_REGISTER:
    spb1 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbWriteSingleRegisterAddress"));
    spb2 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbWriteSingleRegisterVal"    ));

    data.write_single_register.address = (uint16_t)gtk_spin_button_get_value(spb1);
    data.write_single_register.val     = (uint16_t)gtk_spin_button_get_value(spb2);

    printf("WriteSingleRegister Address: %04x. Val: %04x\n",
        data.write_single_register.address, data.write_single_register.val);

    rp = MB_Send(&mbdev, fc, &data);
    if(!rp.ExceptionCode) {
      offset += sprintf(reply_string+offset, "Retorno:\nFunction Code: %d\n", rp.FunctionCode);
      offset += sprintf(reply_string+offset, "address: %04x\n", rp.reply.write_single_register.address);
      offset += sprintf(reply_string+offset, "val    : %04x\n", rp.reply.write_single_register.val);
    }

    break;

  case MB_FC_WRITE_MULTIPLE_COILS:
    for(i=0; i<16; i++) {
      sprintf(wdgName, "tgbWriteMultipleCoils%02d", i+1);
      tgb = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, wdgName));
      out[i/8] |= ((uint32_t)gtk_toggle_button_get_active(tgb)) << (i%8);
    }

    data.write_multiple_coils.start = 0;
    data.write_multiple_coils.quant = 16;
    data.write_multiple_coils.size  = 2;
    data.write_multiple_coils.val   = out;
    printf("WriteMultipleCoils Start: %04x\n", data.write_multiple_coils.start);

    rp = MB_Send(&mbdev, fc, &data);
    if(!rp.ExceptionCode) {
      offset += sprintf(reply_string+offset, "Retorno:\nFunction Code: %d\n", rp.FunctionCode);
      offset += sprintf(reply_string+offset, "size: %d\n", rp.reply.write_multiple_coils.size);
      for(i=0; i<rp.reply.write_multiple_coils.size; i++)
        offset += sprintf(reply_string+offset, "%d: %04x\n", i, rp.reply.write_multiple_coils.val[i]);
    }

    break;

  case MB_FC_WRITE_MULTIPLE_REGISTERS:
    spb1 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbWriteMultipleRegistersStart"));
    spb2 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbWriteMultipleRegistersQuant"));

    data.write_multiple_registers.start = (uint32_t)gtk_spin_button_get_value(spb1);
    data.write_multiple_registers.quant = (uint32_t)gtk_spin_button_get_value(spb2);
    data.write_multiple_registers.size  = 2;
    data.write_multiple_registers.val   = out;
    out[0] = 0xaa;
    out[1] = 0x55;

    printf("WriteMultipleRegisters Start: %04x\n", data.write_multiple_registers.start);
    printf("WriteMultipleRegisters Quant: %02x\n", data.write_multiple_registers.quant);

    rp = MB_Send(&mbdev, fc, &data);

    if(!rp.ExceptionCode) {
      offset += sprintf(reply_string+offset, "Retorno:\nFunction Code: %d\n", rp.FunctionCode);
      offset += sprintf(reply_string+offset, "start: %d\n", rp.reply.write_multiple_registers.start);
      offset += sprintf(reply_string+offset, "quant: %d\n", rp.reply.write_multiple_registers.quant);
      offset += sprintf(reply_string+offset, "size: %d\n", rp.reply.write_multiple_registers.size);
      for(i=0; i<rp.reply.write_multiple_registers.size; i++)
        offset += sprintf(reply_string+offset, "%d: %02x\n", i, rp.reply.write_multiple_registers.val[i]);
    }

    break;

  case MB_FC_MASK_WRITE_REGISTER:
    spb1 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbMaskWriteRegisterAddress"));
    spb2 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbMaskWriteRegisterAND"    ));
    spb3 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbMaskWriteRegisterOR"     ));

    data.mask_write_register.address = (uint16_t)gtk_spin_button_get_value(spb1);
    data.mask_write_register.and     = (uint16_t)gtk_spin_button_get_value(spb2);
    data.mask_write_register.or      = (uint16_t)gtk_spin_button_get_value(spb3);

    printf("MaskWriteRegister Address: %04x. AND: %04x. OR = %04x\n",
        data.mask_write_register.address, data.mask_write_register.and, data.mask_write_register.or);

    rp = MB_Send(&mbdev, fc, &data);
    if(!rp.ExceptionCode) {
      offset += sprintf(reply_string+offset, "Retorno:\nFunction Code: %d\n", rp.FunctionCode);
      offset += sprintf(reply_string+offset, "address: %04x\n", rp.reply.mask_write_register.address);
      offset += sprintf(reply_string+offset, "and    : %04x\n", rp.reply.mask_write_register.and);
      offset += sprintf(reply_string+offset, "or     : %04x\n", rp.reply.mask_write_register.or);
    }

    break;

  case MB_FC_RW_MULTIPLE_REGISTERS:
    spb1 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbRWMultipleRegistersStart"));
    spb2 = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spbRWMultipleRegistersQuant"));

    data.rw_multiple_registers.start_read  = (uint32_t)gtk_spin_button_get_value(spb1);
    data.rw_multiple_registers.quant_read  = (uint32_t)gtk_spin_button_get_value(spb2);
    data.rw_multiple_registers.start_write = data.rw_multiple_registers.start_read;
    data.rw_multiple_registers.quant_write = data.rw_multiple_registers.quant_read;
    data.rw_multiple_registers.size  = 2;
    data.rw_multiple_registers.val   = out;
    out[0] = 0xaa;
    out[1] = 0x55;

    printf("RWMultipleRegisters Start: %04x\n", data.rw_multiple_registers.start_read);
    printf("RWMultipleRegisters Quant: %02x\n", data.rw_multiple_registers.quant_read);

    rp = MB_Send(&mbdev, fc, &data);

    if(!rp.ExceptionCode) {
      offset += sprintf(reply_string+offset, "Retorno:\nFunction Code: %d\n", rp.FunctionCode);
      offset += sprintf(reply_string+offset, "size: %d\n", rp.reply.rw_multiple_registers.size);
      for(i=0; i<rp.reply.rw_multiple_registers.size; i++)
        offset += sprintf(reply_string+offset, "%d: %02x\n", i, (&rp.reply.rw_multiple_registers.val)[i]);
    }

    break;

  case MB_FC_READ_EXCEPTION_STATUS:
    rp = MB_Send(&mbdev, fc, &data);

    if(!rp.ExceptionCode) {
      wdg = GTK_WIDGET(gtk_builder_get_object(builder, "lblReadExceptionStatusVal"));
      sprintf(wdgName, "%02x", rp.reply.read_exception_status.status);
      gtk_label_set_text(GTK_LABEL(wdg), wdgName);

      offset += sprintf(reply_string+offset, "Retorno:\nFunction Code: %d\n", rp.FunctionCode);
      offset += sprintf(reply_string+offset, "status: %02x\n", rp.reply.read_exception_status.status);
    }

    break;

  case MB_FC_READ_DEVICE_IDENTIFICATION:
    wdg = GTK_WIDGET(gtk_builder_get_object(builder, "cbxReadDeviceIdentification"));

    data.read_device_identification.id_code   = 0x01;
    data.read_device_identification.object_id = gtk_combo_box_get_active(GTK_COMBO_BOX(wdg));

    printf("ReadDeviceIdentification IdCode    : %02x\n", data.read_device_identification.id_code);
    printf("ReadDeviceIdentification ObjectCode: %02x\n", data.read_device_identification.object_id);

    rp = MB_Send(&mbdev, fc, &data);

    if(!rp.ExceptionCode) {
      wdg = GTK_WIDGET(gtk_builder_get_object(builder, "lblReadDeviceIdentificationVal"));
      gtk_label_set_text(GTK_LABEL(wdg), (gchar *)&rp.reply.read_device_identification.val);

      offset += sprintf(reply_string+offset, "Retorno:\nFunction Code: %d\n", rp.FunctionCode);
      offset += sprintf(reply_string+offset, "string: %s\n", &rp.reply.read_device_identification.val);
    }

    break;

  default:
    rp.FunctionCode = fc;
    rp.ExceptionCode = MB_EXCEPTION_ILLEGAL_FUNCTION;
  }

  if(rp.ExceptionCode != MB_EXCEPTION_NONE)
    sprintf(reply_string, "Erro enviando Function Code %02x.\nException Code: %02x",
        fc, rp.ExceptionCode);

  mdg = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", reply_string);
  gtk_dialog_run(GTK_DIALOG(mdg));
  gtk_widget_destroy(mdg);
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
      case COMM_FNC_AIN:
        gdk_threads_enter();
        if(msg.data.ad.vin != ad_vin) {
          ad_vin = msg.data.ad.vin;
          gtk_progress_bar_set_fraction(pgbVIN , (gdouble)(msg.data.ad.vin )/0x3ff);
        }
        if(msg.data.ad.term != ad_term) {
          ad_term = msg.data.ad.term;
          gtk_progress_bar_set_fraction(pgbTERM, (gdouble)(msg.data.ad.term)/0x3ff);
        }
        if(msg.data.ad.vbat != ad_vbat) {
          ad_vbat = msg.data.ad.vbat;
          gtk_progress_bar_set_fraction(pgbVBAT, (gdouble)(msg.data.ad.vbat)/0x3ff);
        }
        gdk_threads_leave();
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
  g_timeout_add(500, tmrAD, NULL);

  pthread_create (&tid, NULL, ihm_update, NULL);

  //Inicia o loop principal de eventos (GTK MainLoop)
  gtk_main ();

  SerialClose(ps);

  gdk_threads_leave();

  return 0;
}
