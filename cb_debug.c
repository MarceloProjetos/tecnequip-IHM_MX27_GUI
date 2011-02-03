#include <gtk/gtk.h>
#include <net/modbus.h>

#include "defines.h"

// Objeto que contem toda a interface GTK
extern GtkBuilder *builder;

// Estrutura que representa o ModBus
extern struct MB_Device mbdev;

// Carrega aba referente aos controles da funcao de codigo escolhida
void cbFunctionCodeChanged(GtkComboBox *widget, gpointer user_data)
{
  GtkNotebook *ntb = GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbFunctionCode"));
  gtk_notebook_set_current_page(ntb, gtk_combo_box_get_active(widget));
}

// Retorna para a tela anterior, saindo da tela de debug do modbus
void cbDebugModBusVoltar(GtkButton *button, gpointer user_data)
{
  WorkAreaGoPrevious();
}

void cbDebugModBus(GtkButton *button, gpointer user_data)
{
  WorkAreaGoTo(NTB_ABA_MODBUS);
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
        offset += sprintf(reply_string+offset, "%d: %02x\n", i, rp.reply.read_coils.data[i]);
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
        offset += sprintf(reply_string+offset, "%d: %02x\n", i, rp.reply.read_discrete_inputs.data[i]);
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
        offset += sprintf(reply_string+offset, "%d: %02x\n", i, rp.reply.read_holding_registers.data[i]);
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
        offset += sprintf(reply_string+offset, "%d: %02x\n", i, rp.reply.read_input_registers.data[i]);
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
        offset += sprintf(reply_string+offset, "%d: %02x\n", i, rp.reply.rw_multiple_registers.data[i]);
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
      gtk_label_set_text(GTK_LABEL(wdg), (const gchar *)rp.reply.read_device_identification.data);

      offset += sprintf(reply_string+offset, "Retorno:\nFunction Code: %d\n", rp.FunctionCode);
      offset += sprintf(reply_string+offset, "string: %s\n", rp.reply.read_device_identification.data);
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
