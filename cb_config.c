// Para que a funcao atof funcione corretamente
#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "defines.h"
#include "GtkUtils.h"

// Estrutura que representa o ModBus
//extern struct MB_Device mbdev;

/*** Funcoes e variáveis de suporte ***/

int idUser=0; // Indica usuário que logou se for diferente de zero.
char *UserPerm = NULL; // Permissoes do usuario conectado
int CurrentWorkArea  = 0;  // Variavel que armazena a tela atual.

// Função que salva um log no banco contendo usuário e data que gerou o evento.
extern void Log(char *evento, int tipo);

char * Crypto(char *str)
{
  return (char *)(crypt(str,"bc")+2);
}

int GetUserPerm(unsigned int perm)
{
  unsigned int len = strlen(UserPerm);

  if(perm >= len) // Permissão inexistente
    return PERM_NONE; // Não existe a permissão. Retorna sem permissão.

  switch(UserPerm[perm]) {
  case 'w':
  case 'W':
    return PERM_WRITE;

  case 'r':
  case 'R':
    return PERM_READ;

  default:
    return PERM_NONE;
  }
}

void CarregaComboClientes()
{
  GtkWidget *obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbClientes"));

// Carregamento dos clientes cadastrados no MySQL no ComboBox.
  DB_Execute(&mainDB, 0, "select nome from clientes order by ID");
  CarregaCombo(&mainDB, GTK_COMBO_BOX(obj),0, NULL);
}

void CarregaComboModelos()
{
  char sql[100];
  GtkWidget *obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbModNome"));

// O primeiro item é para inserção de novo modelo.
// Carregamento dos modelos cadastrados no MySQL no ComboBox.
  sprintf(sql, "select nome from modelos where estado='%d' order by ID", MOD_ESTADO_ATIVO);
  DB_Execute(&mainDB, 0, sql);
  CarregaCombo(&mainDB, GTK_COMBO_BOX(obj), 0, "<Novo Modelo>");
}

void CarregaDadosBanco()
{
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoIP"   )), mainDB.server );
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoUser" )), mainDB.user   );
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoSenha")), mainDB.passwd );
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoNome" )), mainDB.nome_db);
}

void GravaDadosBanco()
{
  char *server, *user, *passwd, *nome_db;

  server  = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoIP"   )));
  user    = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoUser" )));
  passwd  = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoSenha")));
  nome_db = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoNome" )));

// Se houve alteração nos dados de conexão, exige reinício do programa
  if(strcmp(server , mainDB.server ) ||
     strcmp(user   , mainDB.user   ) ||
     strcmp(passwd , mainDB.passwd ) ||
     strcmp(nome_db, mainDB.nome_db))
    {
    DB_Close(&mainDB);

    mainDB.server  = server ;
    mainDB.user    = user   ;
    mainDB.passwd  = passwd ;
    mainDB.nome_db = nome_db;

    if(!DB_Init(&mainDB))
      MessageBox("Configurações do banco inválidas! Saindo...");
    else
      {
      MessageBox("Configurações do banco alteradas com sucesso! Saindo...");
      DB_GravarConfig(&mainDB, DB_ARQ_CONFIG);
      }

    gtk_main_quit();
    }
}

char *lista_ent[] = {
    "entEncoderPerim",
    "entEncoderFator",
    "entEncoderResol",
    "spbConfigPerfVelMaxAuto",
    "spbConfigPerfAcelAuto",
    "spbConfigPerfDesacelAuto",
    "spbConfigPerfVelMaxManual",
    "spbConfigPerfAcelManual",
    "spbConfigPerfDesacelManual",
    "entCorteFaca",
    "spbConfigPerfReducao",
    "spbConfigPerfDiamRolo",
    "entConfigDiagonalDistancia",
    "spbConfigDiagonalQtdFuros",
    "spbConfigColNPerfVelMaxAutoDinam",
    "entConfigColNMesaCurso",
    "spbConfigColNMesaVelMaxAuto",
    "spbConfigColNMesaVelMaxManual",
    "spbConfigColNOffset",
    "entConfigColNTamMin",
    "entConfigAplanPasso",
    "rdbConfigPrsSentidoAntiHor",
    "rdbConfigPrsSentidoHor",
    "lblConfigPrsCiclos",
    "entConfigPrsCiclosLub",
    "entConfigPrsCiclosFerram",
    ""
};

int GravarDadosConfig()
{
  MaqConfig *m;
  unsigned int i, idx = 0;
  char **valor_ent, cmdline[1000];
  struct strMaqParam mp;
  valor_ent = (char **)(malloc((sizeof(lista_ent)/sizeof(lista_ent[0]))*sizeof(char *)));

  // Carrega o valor dos widgets conforme a lista fornecida
  LerValoresWidgets(lista_ent, valor_ent);
  for(i=0; lista_ent[i][0]; i++)
    printf("%d: %s = %s\n" , i, lista_ent[i], valor_ent[i] );

  mp.encoder.perimetro     = atof(valor_ent[idx++]);
  mp.encoder.fator         = atof(valor_ent[idx++]);
  mp.encoder.precisao      = atol(valor_ent[idx++]);

  mp.perfil.auto_vel       = atol(valor_ent[idx++]);
  mp.perfil.auto_acel      = atof(valor_ent[idx++]);
  mp.perfil.auto_desacel   = atof(valor_ent[idx++]);
  mp.perfil.manual_vel     = atol(valor_ent[idx++]);
  mp.perfil.manual_acel    = atof(valor_ent[idx++]);
  mp.perfil.manual_desacel = atof(valor_ent[idx++]);

  mp.corte.tam_faca        = atol(valor_ent[idx++]);

  mp.perfil.fator          = atof(valor_ent[idx++]);
  mp.perfil.diam_rolo      = atol(valor_ent[idx++]);

  // Parametros personalizados da Diagonal / Travessa
  mp.custom.diagonal.dist_prensa_corte = atol(valor_ent[idx++]);
  mp.custom.diagonal.qtd_furos_interm  = atol(valor_ent[idx++]);

  // Parametros personalizados da Coluna N
  mp.custom.coln.dinam_vel  = atol(valor_ent[idx++]);
  mp.custom.coln.curso      = atol(valor_ent[idx++]);
  mp.custom.coln.auto_vel   = atol(valor_ent[idx++]);
  mp.custom.coln.manual_vel = atol(valor_ent[idx++]);
  mp.custom.coln.offset     = atof(valor_ent[idx++]);
  mp.custom.coln.tam_min    = atol(valor_ent[idx++]);

  // Parametros personalizados da Prensa
  mp.custom.prensa.passo         = atol(valor_ent[idx++]);
  mp.custom.prensa.sentido       = atol(valor_ent[idx++]);
  idx++; // Este objeto representa o radio button. Precisamos dele apenas para gravar o controle, nao para leitura do valor
  mp.custom.prensa.ciclos        = atol(valor_ent[idx++]);
  mp.custom.prensa.ciclos_lub    = atol(valor_ent[idx++]);
  mp.custom.prensa.ciclos_ferram = atol(valor_ent[idx++]);

  MaqConfigPerfil (mp.perfil );
  MaqConfigEncoder(mp.encoder);
  MaqConfigCorte  (mp.corte  );
  MaqConfigCustom (mp.custom );

  GravaDadosBanco();

  MaqGravarConfig();

  free(valor_ent);

  m = MaqConfig_GetMachine(gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbMaquina"))));
  if(m && m != MaqConfigCurrent) {
    // Alterando o Hostname
    sethostname(m->ID, strlen(m->ID));
    sprintf(cmdline, "./UpdateHostName.sh %s", m->ID);
    system(cmdline);

    MessageBox("Máquina Alterada! Saindo...");
    gtk_main_quit();
  }

  return 0;
}

void LerDadosConfig()
{
  char tmp[100];
  unsigned int i, idx = 0;
  struct strMaqParam mp;
  char **valor_ent;

  // Diminui em 1 o número de campos devido ao elemento vazio no final.
  i = sizeof(lista_ent)/sizeof(lista_ent[0])-1;
  valor_ent = (char **)(malloc(i * sizeof(char *)));

  mp.perfil  = MaqLerPerfil ();
  mp.encoder = MaqLerEncoder();
  mp.corte   = MaqLerCorte  ();
  mp.custom  = MaqLerCustom ();

  sprintf(tmp, "%d", mp.encoder.perimetro);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%.4f", mp.encoder.fator);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.encoder.precisao);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.perfil.auto_vel);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%f", mp.perfil.auto_acel);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%f", mp.perfil.auto_desacel);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.perfil.manual_vel);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%f", mp.perfil.manual_acel);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%f", mp.perfil.manual_desacel);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.corte.tam_faca);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%f", mp.perfil.fator);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.perfil.diam_rolo);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.custom.diagonal.dist_prensa_corte);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.custom.diagonal.qtd_furos_interm);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.custom.coln.dinam_vel);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.custom.coln.curso);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.custom.coln.auto_vel);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.custom.coln.manual_vel);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%.1f", mp.custom.coln.offset);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.custom.coln.tam_min);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.custom.prensa.passo);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.custom.prensa.sentido);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", !mp.custom.prensa.sentido);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.custom.prensa.ciclos);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.custom.prensa.ciclos_lub);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  sprintf(tmp, "%d", mp.custom.prensa.ciclos_ferram);
  valor_ent[idx] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[idx++], tmp);

  GravarValoresWidgets(lista_ent, valor_ent);

  while(i--) {
    if(valor_ent[i] != NULL)
      free(valor_ent[i]);
  }
  free(valor_ent);

  if(mainDB.status & DB_FLAGS_CONNECTED) // Somente carrega se há conexão com o DB
    {
    // Carrega clientes cadastrados
    CarregaComboClientes();

    // Carrega dados da aba de modelos
    CarregaComboModelos();
    }

// Carrega dados da aba de Banco de Dados
  CarregaDadosBanco();

  // Seleciona a maquina configurada
  gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbMaquina")), MaqConfig_GetActive());
}

#define VK_TYPE_WIDGET 0
#define VK_TYPE_PCHAR  1

typedef struct strVKData VKData;

struct strVKData {
  int type;
  union {
    struct {
      int        size;
      char      *buffer;
      char      *msg;
      void      (*finish)(VKData *vkd, int ok);
      void      *user_data;
    } pchar;
    GtkWidget   *widget;
  } obj;
};

void SairVirtualKeyboard()
{
  GtkButton *button;
  guint signal_id;
  gulong handler_id;

  // Recebe o ID do sinal do callback
  signal_id = g_signal_lookup("clicked", GTK_TYPE_BUTTON);

  // Remove do botao Cancelar
  button = GTK_BUTTON(gtk_builder_get_object(builder, "btnVirtualKeyboardCancel"));
  handler_id = g_signal_handler_find(button, G_SIGNAL_MATCH_ID, signal_id, 0, NULL, NULL, NULL);
  g_signal_handler_disconnect(button, handler_id);

  // Remove do botao OK
  button = GTK_BUTTON(gtk_builder_get_object(builder, "btnVirtualKeyboardOK"));
  handler_id = g_signal_handler_find(button, G_SIGNAL_MATCH_ID, signal_id, 0, NULL, NULL, NULL);
  g_signal_handler_disconnect(button, handler_id);

  WorkAreaGoPrevious();
}

void cbVirtualKeyboardKeyPress(GtkButton *button, gpointer user_data)
{
  int pos;
  GtkEditable *editable;
  GtkTextBuffer *tb;
  GtkTextIter cursor;
  gboolean editable_is_visible;
  gchar *str = (gchar *)gtk_button_get_label(button);

  editable = GTK_EDITABLE(gtk_builder_get_object(builder, "entVirtualKeyboard"));
  editable_is_visible = gtk_widget_get_visible(GTK_WIDGET(editable));

  // Verifica o label para identificar se foi clicado espaco, backspace ou enter.
  if        (!strcmp(str, "Enter")) {
    if(editable_is_visible) return; // ENTER desativado para entry
    str = "\n";
  } else if (!strcmp(str, "Espaço")) {
    str = " ";
  } else if (!strcmp(str, "gtk-clear")) {
    str = NULL;
  }

  if(editable_is_visible) {
    if(str != NULL) {
      pos = gtk_editable_get_position(editable);
      gtk_editable_insert_text(editable, str, -1, &pos);
      gtk_editable_set_position(editable, pos);
    } else if (gtk_editable_get_selection_bounds(editable, &pos, &pos)) {
      gtk_editable_delete_selection(editable);
    } else {
      pos = gtk_editable_get_position(editable);
      if(pos) {
        gtk_editable_delete_text(editable, pos-1, pos);
      }
    }
  } else {
    tb  = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(builder, "txvVirtualKeyboard")));

    if(str != NULL) {
      gtk_text_buffer_insert_at_cursor(tb, str, strlen(str));
    } else {
      if (gtk_text_buffer_get_has_selection (tb)) {
        gtk_text_buffer_delete_selection (tb, TRUE, TRUE);
      } else {
        gtk_text_buffer_get_iter_at_mark (tb, &cursor, gtk_text_buffer_get_insert (tb));
        gtk_text_buffer_backspace (tb, &cursor, TRUE, TRUE);
      }
    }
  }
}

void cbVirtualKeyboardCapsLock(GtkToggleButton *button, gpointer user_data)
{
  char tmp[10], *label;
  unsigned int i;
  GtkButton *wdg;
  gboolean toggled = gtk_toggle_button_get_active(button);

  for(i=1;;i++) { // Loop eterno, finaliza quando acabarem os botoes
    sprintf(tmp, "btnVK%02d", i);
    wdg = GTK_BUTTON(gtk_builder_get_object(builder, tmp));
    if(wdg == NULL) // Acabaram os botoes
      break; // Sai do loop

    // Se não for uma letra, altera para o que deve ser
    label = (char *)gtk_button_get_label(wdg);
    if(!strcmp(label, "Ç")) {
      strcpy(tmp, "ç");
    } else if(!strcmp(label, "ç")) {
      strcpy(tmp, "Ç");
/*    } else if(!strcmp(label, ".")) {
      strcpy(tmp, ",");
    } else if(!strcmp(label, ",")) {
      strcpy(tmp, ".");*/
    } else { // Letra, inverte case.
      sprintf(tmp, "%c", (toggled ? toupper : tolower)(*label));
    }

    gtk_button_set_label(wdg, tmp);
  }
}

void cbVirtualKeyboardOK(GtkButton *button, gpointer user_data)
{
  char *data;
  GtkTextBuffer *tb;
  GtkTextIter start, end;
  VKData *vkdata = (VKData *)user_data;
  GtkWidget *widget = vkdata->obj.widget;

  if(vkdata->type == VK_TYPE_WIDGET) {
    if(GTK_IS_ENTRY(widget)) {
      gtk_entry_set_text(GTK_ENTRY(widget),gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entVirtualKeyboard"))));
    } else if(GTK_IS_TEXT_VIEW(widget)) {
      tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(builder, "txvVirtualKeyboard")));
      gtk_text_buffer_get_start_iter(tb, &start);
      gtk_text_buffer_get_end_iter(tb, &end);
      data = (char *)(gtk_text_buffer_get_text(tb, &start, &end, FALSE));

      gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget)), data, -1);
    }
  } else {
    strncpy(vkdata->obj.pchar.buffer, gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entVirtualKeyboard"))), vkdata->obj.pchar.size);
    vkdata->obj.pchar.buffer[vkdata->obj.pchar.size] = 0;
    if(vkdata->obj.pchar.finish != NULL) {
      (*vkdata->obj.pchar.finish)(vkdata, TRUE);
    }
  }

  free(vkdata);

  SairVirtualKeyboard();
}

void cbVirtualKeyboardCancelar(GtkButton *button, gpointer user_data)
{
  VKData *vkdata = (VKData *)user_data;

  if(vkdata->type == VK_TYPE_PCHAR && vkdata->obj.pchar.finish != NULL) {
    (*vkdata->obj.pchar.finish)(vkdata, FALSE);
  }

  free(vkdata);

  SairVirtualKeyboard();
}

void AbrirVirtualKeyboard(VKData *vkdata)
{
  VKData *vkd;
  const char *msg = NULL;
  char *data, nome_label[100];
  GtkWidget *obj, *widget = vkdata->obj.widget;
  GtkEntry *entry;
  GtkTextBuffer *tb;
  GtkLabel *lbl = NULL, *lblVK;
  GtkTextView *textview;
  GtkTextIter start, end;
  GtkScrolledWindow *scrollwnd;

  // Lê o texto atual.
  if(vkdata->type == VK_TYPE_WIDGET) {
    if(GTK_IS_ENTRY(widget)) {
      data = (char *)gtk_entry_get_text(GTK_ENTRY(widget));
    } else if(GTK_IS_TEXT_VIEW(widget)) {
      tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
      gtk_text_buffer_get_start_iter(tb, &start);
      gtk_text_buffer_get_end_iter(tb, &end);
      data = (char *)(gtk_text_buffer_get_text(tb, &start, &end, FALSE));
    } else {
      return; // Objeto nao suportado
    }

    // Tenta carregar o label relacionado ao campo de texto
    sprintf(nome_label, "lbl%s", (char *)(gtk_buildable_get_name(GTK_BUILDABLE(widget))+3));
    lbl   = GTK_LABEL(gtk_builder_get_object(builder, nome_label));
    if(lbl && GTK_IS_LABEL(lbl)) {
      msg = gtk_label_get_text(lbl);
    }
  } else if(vkdata->type == VK_TYPE_PCHAR) {
    data = vkdata->obj.pchar.buffer;
    msg  = vkdata->obj.pchar.msg;
  } else { // Tipo nao suportado!
    return;
  }

  vkd  = (VKData *)malloc(sizeof(VKData));
  *vkd = *vkdata;

  lblVK = GTK_LABEL(gtk_builder_get_object(builder, "lblVirtualKeyboard"));

  if(msg != NULL) {
    gtk_label_set_text(lblVK, msg);
    gtk_widget_set_visible(GTK_WIDGET(lblVK), TRUE );
  } else {
    gtk_widget_set_visible(GTK_WIDGET(lblVK), FALSE);
  }

// Carrega o ponteiro para o botão de confirmar.
  obj = GTK_WIDGET(gtk_builder_get_object(builder, "btnVirtualKeyboardOK"));

// Conexão dos sinais de callback
  g_signal_connect ((gpointer) obj, "clicked",  G_CALLBACK(cbVirtualKeyboardOK),
              (gpointer)(vkd));

  // Carrega o ponteiro para o botão de cancelar.
  obj = GTK_WIDGET(gtk_builder_get_object(builder, "btnVirtualKeyboardCancel"));

  // Conexão dos sinais de callback
  g_signal_connect ((gpointer) obj, "clicked",  G_CALLBACK(cbVirtualKeyboardCancelar),
                (gpointer)(vkd));

  // Carrega ponteiro para os campos de edição
  entry     = GTK_ENTRY          (gtk_builder_get_object(builder, "entVirtualKeyboard"));
  textview  = GTK_TEXT_VIEW      (gtk_builder_get_object(builder, "txvVirtualKeyboard"));
  scrollwnd = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "scwVirtualKeyboard"));

  // Carrega o texto e ativa o objeto correspondente
  if(vkdata->type == VK_TYPE_PCHAR || GTK_IS_ENTRY(widget)) {
    gtk_entry_set_text(entry, data);
    if(vkdata->type == VK_TYPE_PCHAR) {
      gtk_entry_set_visibility(entry, TRUE);
    } else {
      gtk_entry_set_visibility(entry, gtk_entry_get_visibility(GTK_ENTRY(widget)));
    }
    gtk_widget_set_visible(GTK_WIDGET(entry    ), TRUE );
    gtk_widget_set_visible(GTK_WIDGET(textview ), FALSE);
    gtk_widget_set_visible(GTK_WIDGET(scrollwnd), FALSE);

    GtkEditable *editable = GTK_EDITABLE(gtk_builder_get_object(builder, "entVirtualKeyboard"));
    gtk_editable_set_position(editable, -1);
//    gtk_widget_grab_focus (GTK_WIDGET(entry    ));
  } else {
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(textview), data, -1);
    gtk_widget_set_visible(GTK_WIDGET(entry    ), FALSE);
    gtk_widget_set_visible(GTK_WIDGET(textview ), TRUE );
    gtk_widget_set_visible(GTK_WIDGET(scrollwnd), TRUE );
//    gtk_widget_grab_focus (GTK_WIDGET(textview ));
  }

// Exibe a janela.
  WorkAreaGoTo(NTB_ABA_VIRTUAL_KB);
}

void cbClienteSelecionado(GtkComboBox *combobox, gpointer user_data)
{
  gboolean estado;

  if(gtk_combo_box_get_active(combobox)) {
    estado = TRUE;
  } else {
    estado = FALSE;
  }

  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnClienteRemover")), estado);
}

void cbRemoverCliente(GtkButton *button, gpointer user_data)
{
  int id;
  char sql[100];
  GtkWidget *dialog;
  GtkComboBox *obj = GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbClientes"));

  if(!gtk_combo_box_get_active(obj)) // O primeiro item não pode ser excluído.
    return;

  sprintf(sql, "select ID from clientes where nome='%s'", LerComboAtivo(obj));
  DB_Execute(&mainDB, 0, sql);
  DB_GetNextRow(&mainDB, 0);
  id = atoi(DB_GetData(&mainDB, 0, 0));

  dialog = gtk_message_dialog_new (NULL,
           GTK_DIALOG_DESTROY_WITH_PARENT,
           GTK_MESSAGE_QUESTION,
           GTK_BUTTONS_YES_NO,
           "Tem certeza que deseja excluir o cliente '%s'?",
           LerComboAtivo(obj));

  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
    {
    // Substitui o cliente sendo excluído das tarefas pelo cliente "Nenhum".
    sprintf(sql, "update tarefas set ID_Cliente='1' where ID_Cliente='%d'", id);
    DB_Execute(&mainDB, 0, sql);

    sprintf(sql, "delete from clientes where nome='%s'", LerComboAtivo(obj));
    DB_Execute(&mainDB, 0, sql);

    ExcluiItemCombo(GTK_COMBO_BOX(obj), gtk_combo_box_get_active(GTK_COMBO_BOX(obj)));
    gtk_combo_box_set_active(GTK_COMBO_BOX(obj), 0);

    sprintf(sql, "Removendo cliente '%s'", LerComboAtivo(obj));
    Log(sql, LOG_TIPO_CONFIG);

    MessageBox("Cliente removido com sucesso!");
    }

  gtk_widget_destroy (dialog);
}

gboolean ChecarModelo(int passo, int tam_max)
{
  char *msg = NULL;
  GtkWidget *dlg;

  if(passo <= 0)
    msg = "O passo tem que ser maior que zero!";
  if(tam_max < passo)
    msg = "O Tamanho máximo deve ser maior que o passo!";

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

void cbConfigModeloSelecionado(GtkComboBox *combobox, gpointer user_data)
{
  GSList *lst;
  int pilotar;

  char *lst_campos[] =
    {
      "entModPasso" , "passo",
      "entModTamMax", "tam_max",
      "entModTamMin", "tam_min",
      ""            , ""
    };

  char *lst_botoes[] = { "btnModRemover", "" }, *opt_piloto[] = { "Não", "Sim" };

  if(gtk_combo_box_get_active(combobox)<0)
    return; // Sem item ativo.

  if(CarregaCampos(&mainDB, combobox, lst_campos, lst_botoes, "modelos", "nome"))
    pilotar = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "pilotar")));
  else
    pilotar = 0;

  lst = gtk_radio_button_get_group(GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "rbtModPilotarSim")));
  while(lst)
    {
    if(!strcmp(gtk_button_get_label(GTK_BUTTON(lst->data)), opt_piloto[pilotar]))
      {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lst->data), TRUE);
      break;
      }

    lst = lst->next;
    }
}

typedef struct strModData ModData;

struct strModData {
  char **valores;
  char **opt_piloto;
};

void FinalizaInserirModelo(VKData *vkdata, int result)
{
  char sql[400];
  GtkWidget   *wnd;
  GtkComboBox *obj;
  ModData *md = (ModData *)vkdata->obj.pchar.user_data;

  if(result == TRUE)
    {
    sprintf(sql, "select ID from modelos where nome='%s'", vkdata->obj.pchar.buffer);
    DB_Execute(&mainDB, 0, sql);
    if(DB_GetNextRow(&mainDB, 0)>0)
      {
      wnd = gtk_message_dialog_new (NULL,
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "O modelo '%s' já existe!",
                vkdata->obj.pchar.buffer);

      gtk_dialog_run(GTK_DIALOG(wnd));
      gtk_widget_destroy (wnd);
      }
    else // O modelo não existe. Realizando inserção.
      {
      sprintf(sql, "insert into modelos (nome, pilotar, passo, tam_max, tam_min, estado) "
             "values ('%s', '%d', '%s', '%s', '%s', '%d')",
             vkdata->obj.pchar.buffer, BuscaStringLista(md->opt_piloto, md->valores[1], FALSE), md->valores[2], md->valores[3], md->valores[4], MOD_ESTADO_ATIVO);
      DB_Execute(&mainDB, 0, sql);

      obj = GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbModNome"));
      CarregaItemCombo(obj, vkdata->obj.pchar.buffer);
      gtk_combo_box_set_active(obj, 0);
      cbConfigModeloSelecionado(obj, NULL);

      sprintf(sql, "Adicionado modelo %s", vkdata->obj.pchar.buffer);
      Log(sql, LOG_TIPO_CONFIG);

      MessageBox("Modelo adicionado com sucesso!");
      }
    } else {
      MessageBox("Operação cancelada!");
    }
}

void cbAplicarModelo(GtkButton *button, gpointer user_data)
{
  char sql[400];
  static char *valores[30] = { "", "", "", "" }, *opt_piloto[] = { "Não", "Sim", "" };
  char *campos[] =
    {
    "cmbModNome",
    "rbtModPilotarSim",
    "entModPasso",
    "entModTamMax",
    "entModTamMin",
    ""
    };
  static ModData md = { valores, opt_piloto };

  GtkWidget *dialog, *obj;

  if(!LerValoresWidgets(campos, valores))
    return; // Ocorreu algum erro lendo os campos. Retorna.

// Checa se os dados são válidos.
  if(!ChecarModelo(atol(valores[2]), atol(valores[3])))
    return;

  obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbModNome"));
  if(gtk_combo_box_get_active(GTK_COMBO_BOX(obj)) != 0) // Não é o primeiro item, alterar usuário.
    {
    dialog = gtk_message_dialog_new (NULL,
              GTK_DIALOG_DESTROY_WITH_PARENT,
              GTK_MESSAGE_QUESTION,
              GTK_BUTTONS_YES_NO,
              "Aplicar as alterações ao modelo '%s'?",
              valores[0]);

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
      {
      sprintf(sql, "update modelos set pilotar='%d', passo='%s', tam_max='%s', tam_min='%s' where nome='%s'",
        BuscaStringLista(opt_piloto, valores[1], FALSE), valores[2], valores[3], valores[4], valores[0]);
      DB_Execute(&mainDB, 0, sql);

      gtk_combo_box_set_active(GTK_COMBO_BOX(obj), 0);

      sprintf(sql, "Alterado modelo %s", valores[0]);
      Log(sql, LOG_TIPO_CONFIG);

      MessageBox("Modelo alterado com sucesso!");
      }

    gtk_widget_destroy (dialog);
    }
  else
    {
    VKData vkdata;
    vkdata.type                = VK_TYPE_PCHAR;
    vkdata.obj.pchar.buffer    = (char *)malloc(21);
    vkdata.obj.pchar.size      = 20;
    vkdata.obj.pchar.msg       = "Nome do Modelo";
    vkdata.obj.pchar.finish    = FinalizaInserirModelo;
    vkdata.obj.pchar.user_data = &md;
    vkdata.obj.pchar.buffer[0] = 0;

    AbrirVirtualKeyboard(&vkdata);
    }
}

void cbRemoverModelo(GtkButton *button, gpointer user_data)
{
  char sql[100];
  GtkWidget *dialog, *obj;

  obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbModNome"));
  if(gtk_combo_box_get_active(GTK_COMBO_BOX(obj)) != 0) // Não é o primeiro item, exclui modelo.
    {
    dialog = gtk_message_dialog_new (NULL,
              GTK_DIALOG_DESTROY_WITH_PARENT,
              GTK_MESSAGE_QUESTION,
              GTK_BUTTONS_YES_NO,
              "Tem certeza que deseja excluir o modelo '%s'?",
              LerComboAtivo(GTK_COMBO_BOX(obj)));

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
      {
      sprintf(sql, "update modelos set estado='%d' where nome='%s'", MOD_ESTADO_REMOVIDO, LerComboAtivo(GTK_COMBO_BOX(obj)));
      DB_Execute(&mainDB, 0, sql);

      sprintf(sql, "Removendo modelo %s", LerComboAtivo(GTK_COMBO_BOX(obj)));
      Log(sql, LOG_TIPO_CONFIG);

      ExcluiItemCombo(GTK_COMBO_BOX(obj), gtk_combo_box_get_active(GTK_COMBO_BOX(obj)));
      gtk_combo_box_set_active(GTK_COMBO_BOX(obj), 0);

      MessageBox("Modelo removido com sucesso!");
      }

    gtk_widget_destroy (dialog);
    }
}

void cbConfigGeralMais(GtkButton *button, gpointer user_data)
{
  gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbConfigGeral")),
      MaqConfigCurrent->AbaConfigMais);
}

void cbConfigGeralVoltar(GtkButton *button, gpointer user_data)
{
  gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbConfigGeral")), 0);
}

void AbrirConfig(unsigned int pos)
{
  if(mainDB.status & DB_FLAGS_CONNECTED) // Banco de dados conectado!
    if(!GetUserPerm(PERM_ACESSO_CONFIG))
      {
      WorkAreaGoTo(MaqConfigCurrent->AbaHome);
      MessageBox("Sem permissão para acesso à configuração!");
      return;
      }

  gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "btnConfigMais")),
      MaqConfigCurrent->AbaConfigMais != 0);

  WorkAreaGoTo(NTB_ABA_CONFIG);
  cbConfigGeralVoltar(NULL, NULL);

// Se pos != 0, foi chamada a janela forçadamente e portanto esta ação não pode ser cancelada!
  if(pos) {
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnConfigCancel")), FALSE);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbConfig")), pos);
  }

  LerDadosConfig();
}

void cbConfigOk(GtkButton *button, gpointer user_data)
{
// A máquina foi configurada, devemos remover o erro de configuração.
//  MQ.MQ_Data.funcao = CV_MQFNC_LIMPA_ERRO;
//  MQ.MQ_Data.data.di[0] = CV_ERRO_CONFIG;
//  MQ_Transfer(&MQ);

  WorkAreaGoTo(MaqConfigCurrent->AbaHome);

  if(mainDB.status & DB_FLAGS_CONNECTED) // Banco de dados conectado!
    if(GetUserPerm(PERM_ACESSO_CONFIG) != PERM_WRITE) {
      WorkAreaGoTo(MaqConfigCurrent->AbaHome);
      MessageBox("Sem permissão para alterar configuração! Nada alterado.");
      return;
    }

  Log("Alterada configuracao da maquina", LOG_TIPO_CONFIG);

  GravarDadosConfig();
}

void cbConfigVoltar(GtkButton *button, gpointer user_data)
{
  WorkAreaGoTo(MaqConfigCurrent->AbaHome);
}

void cbLoginOk(GtkButton *button, gpointer user_data)
{
  struct strDB *sDB = MSSQL_Connect();
  int pos = 4; // Posição da aba de configuração do banco, iniciando de zero.
  char sql[300], *lembrete = "";
  char senha[20], *tmpPerm;

#ifndef DISABLE_SQL_SERVER
  // Verifica se conectamos no banco SQL Server
  if(!(sDB && (sDB->status & DB_FLAGS_CONNECTED))) {
    sDB = &mainDB; // Não conectamos, tenta login local
    MessageBox("Erro ao conectar ao servidor, tentando conexão local");
  }
#else
  sDB = &mainDB; // Nao estamos usando o SQL Server, usar banco local
#endif

  if(!(sDB->status & DB_FLAGS_CONNECTED)) { // Banco não conectado!
    if(!strcmp(Crypto((char *)(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entLoginSenha"))))), SENHA_MASTER)) // Senha correta
      {
      idUser=1; // Grava 1 para indicar que foi logado

// Exibe a janela de configuração na aba de configuração do banco de dados
      AbrirConfig(pos);

      return;
      }
    else
      lembrete = LEMBRETE_SENHA_MASTER;
  } else {
	char *user = LerComboAtivo(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbLoginUser")));
    sprintf(sql, "select O.*, P.PERMISSAO as pPERMISSAO from OPERADOR as O, PERFIL as P where O.PERFIL=P.ID and O.USUARIO='%s'", user);

    DB_Execute(sDB, 0, sql);
    if(DB_GetNextRow(sDB, 0)>0) {
      // Carrega o ID do usuário que está logando.
      idUser = atoi(DB_GetData(sDB, 0, DB_GetFieldNumber(sDB, 0, "ID")));

      strcpy(senha, Crypto((char *)(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entLoginSenha"))))));
      if(!strcmp(DB_GetData(sDB, 0, DB_GetFieldNumber(sDB, 0, "SENHA")),senha)) {
        // Oculta a janela de login.
        WorkAreaGoTo(MaqConfigCurrent->AbaHome);

        // Limpa a senha digitada
        gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entLoginSenha")), "");

        // Carrega permissão
        tmpPerm = DB_GetData(sDB, 0, DB_GetFieldNumber(sDB, 0, "pPERMISSAO"));

        // Salva o nome do usuario logado
        monitor_Set_User(user);

        if(UserPerm != NULL)
          free(UserPerm); // Desaloca permissão anterior
        UserPerm = (char *)malloc(strlen(tmpPerm)+1);

        strcpy(UserPerm, tmpPerm);

#ifndef DISABLE_SQL_SERVER
        // Se conectou pelo SQL Server, sincronizar usuário e permissões com o MySQL
        if(sDB->DriverID != NULL && !strcmp(sDB->DriverID, "MSSQL")) {
          // Primeiro sincroniza os perfis
          if(MSSQL_Execute(1, "select * from PERFIL", MSSQL_DONT_SYNC) >= 0) {
            // Exclui registros antigos
            DB_Execute(&mainDB, 0, "delete from PERFIL");

            // Inclui novos registros
            while(DB_GetNextRow(sDB, 1) > 0) {
              sprintf(sql, "insert into PERFIL (ID, NOME, PERMISSAO) values ('%s', '%s', '%s')",
                  MSSQL_GetData(1, DB_GetFieldNumber(sDB, 1, "ID"       )),
                  MSSQL_GetData(1, DB_GetFieldNumber(sDB, 1, "NOME"     )),
                  MSSQL_GetData(1, DB_GetFieldNumber(sDB, 1, "PERMISSAO")));
              DB_Execute(&mainDB, 0, sql); // Insere perfil
            }
          }

          // Agora sincroniza o usuário que acabou de entrar
          sprintf(sql, "select ID from OPERADOR where ID='%d'", idUser);
          DB_Execute(&mainDB, 0, sql); // Verifica se usuário já existe
          if(DB_GetNextRow(&mainDB, 0) > 0) { // Usuario existente! Usar update...
            sprintf(sql, "update OPERADOR set NOME='%s', USUARIO='%s', SENHA='%s', LEMBRETE='%s', PERFIL='%s' where ID='%d'",
              MSSQL_GetData(0, DB_GetFieldNumber(sDB, 0, "NOME"    )),
              MSSQL_GetData(0, DB_GetFieldNumber(sDB, 0, "USUARIO" )),
              MSSQL_GetData(0, DB_GetFieldNumber(sDB, 0, "SENHA"   )),
              MSSQL_GetData(0, DB_GetFieldNumber(sDB, 0, "LEMBRETE")),
              MSSQL_GetData(0, DB_GetFieldNumber(sDB, 0, "PERFIL"  )), idUser);
          } else {
            sprintf(sql, "insert into OPERADOR (ID, NOME, USUARIO, SENHA, LEMBRETE, PERFIL) values ('%s', '%s', '%s', '%s', '%s', '%s')",
              MSSQL_GetData(0, DB_GetFieldNumber(sDB, 0, "ID"      )),
              MSSQL_GetData(0, DB_GetFieldNumber(sDB, 0, "NOME"    )),
              MSSQL_GetData(0, DB_GetFieldNumber(sDB, 0, "USUARIO" )),
              MSSQL_GetData(0, DB_GetFieldNumber(sDB, 0, "SENHA"   )),
              MSSQL_GetData(0, DB_GetFieldNumber(sDB, 0, "LEMBRETE")),
              MSSQL_GetData(0, DB_GetFieldNumber(sDB, 0, "PERFIL" )));
          }

          DB_Execute(&mainDB, 0, sql); // Insere/Atualiza registro com os dados atuais
        }
#endif

        Log("Entrada no sistema", LOG_TIPO_SISTEMA);

        return;
      } else { // Erro durante login. Gera log informando esta falha.
        lembrete = DB_GetData(sDB, 0, DB_GetFieldNumber(sDB, 0, "LEMBRETE"));

        // Insere o log de erro no login para o usuário selecionado
        Log("Erro durante login", LOG_TIPO_ERRO);

        // Volta para zero pois o usuário não logou.
        idUser = 0;
      }
    }
  }

  // Se lembrete for nulo, seleciona para texto em branco.
  if(lembrete == NULL)
    lembrete = "";

  // Gera a string de erro utilizando a variável sql.
  sprintf(sql, "Erro durante o login! Lembrete da senha: %s", lembrete);
  MessageBox(sql);
}

void cbBancoTestar(GtkButton *button, gpointer user_data)
{
  struct strDB tmpDB;

  tmpDB.DriverID = "MySQL";
  tmpDB.server   = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoIP"   )));
  tmpDB.user     = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoUser" )));
  tmpDB.passwd   = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoSenha")));
  tmpDB.nome_db  = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoNome" )));

  if(DB_Init(&tmpDB)) // Se inicializar o banco, precisamos verificar se existem as tabelas.
    {
// DB_Execute() retorna -1 se ocorrer um erro ou zero no sucesso.
// Se algum retornar algo diferente de zero existe um erro com o banco!
    if(DB_Execute(&tmpDB, 0, "select * from OPERADOR") ||
       DB_Execute(&tmpDB, 1, "select * from tarefas" ) ||
       DB_Execute(&tmpDB, 2, "select * from modelos" ) ||
       DB_Execute(&tmpDB, 3, "select * from clientes"))
        MessageBox("Conexão OK mas banco de dados inválido ou com erro!");
    else
        MessageBox("Testes executados com sucesso");
    }
  else
    MessageBox("Erro conectando ao banco de dados!");

  DB_Close(&tmpDB);
}

void cbManutAtualSaida(GtkToggleButton *togglebutton, gpointer user_data)
{
  const gchar *nome = gtk_buildable_get_name(GTK_BUILDABLE(togglebutton));
  char nome_img[30];
  union MODBUS_FCD_Data data;
  struct MODBUS_Reply rp;

  data.write_single_coil.output = atoi(&nome[strlen(nome)-2]);
  data.write_single_coil.val    = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));

  rp.ExceptionCode = 0;//MB_Send(&mbdev, MODBUS_FC_WRITE_SINGLE_COIL, &data);

  sprintf(nome_img, "imgManutSai%02d", data.write_single_coil.output-1);
  gtk_image_set_from_stock(GTK_IMAGE(gtk_builder_get_object(builder, nome_img)),
      data.write_single_coil.val ? "gtk-apply" : "gtk-media-record", GTK_ICON_SIZE_BUTTON);

  if(rp.ExceptionCode != MODBUS_EXCEPTION_NONE)
    printf("Erro escrevendo saida. Exception Code: %02x\n", rp.ExceptionCode);
}

extern unsigned int CurrentStatus;

typedef struct strWAList WAList;

struct strWAList {
  int WorkArea;
  WAList *next;
};

WAList *WA_PreviousList = NULL;

int LastWA(WAList *list)
{
  if(list != NULL) {
    while(list->next != NULL) {
      list = list->next;
    }

    return list->WorkArea;
  }

  return NTB_ABA_NONE; // Por default retorna a aba indicando que nao existe aba.
}

WAList * AddWA(WAList **list, int WA)
{
  WAList *newWA;

  if(list == NULL) {
    return NULL;
  }

  newWA = (WAList *)malloc(sizeof(WAList));
  newWA->WorkArea = WA;
  newWA->next     = NULL;

  if(*list == NULL) {
    *list = newWA;
  } else {
    WAList *current = *list;
    while(current->next != NULL) {
      current = current->next;
    }

    current->next = newWA;
  }

  return newWA;
}

int RemoveWA(WAList **list)
{
  int WA = MaqConfigCurrent->AbaHome; // Por default retorna a tela anterior como a aba HOME.

  if(list != NULL && *list != NULL) {
    WAList *current = *list, *previous = NULL;
    while(current->next != NULL) {
      previous = current;
      current = current->next;
    }

    WA = current->WorkArea;
    free(current);

    if(previous == NULL) {
      *list = NULL;
    } else {
      previous->next = NULL;
    }
  }

  return WA;
}

void cbNotebookWorkAreaChanged(GtkNotebook *ntb, GtkNotebookPage *page, guint arg1, gpointer user_data)
{
  int ExibirBarra = 0;

  if(CurrentWorkArea != NTB_ABA_NONE) {
    AddWA(&WA_PreviousList, CurrentWorkArea);
  }
  CurrentWorkArea  = arg1;

  if(CurrentWorkArea != NTB_ABA_ESPERA && CurrentWorkArea != NTB_ABA_MESSAGEBOX) {
    atividade++;
  }

  if(CurrentWorkArea == MaqConfigCurrent->AbaHome) {
    ExibirBarra = 1;
  }
  gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "hbxNotificationArea")), ExibirBarra);

  if(CurrentWorkArea == NTB_ABA_POWERDOWN && !OnPowerDown) {
    WorkAreaGoPrevious(); // Tela nao faz mais sentido agora, energia restabelecida!
  }
}

void WorkAreaGoTo(int NewWorkArea)
{
  // Se não houve mudança, retorna.
  // Caso estiver na aba de Espera, nao permite mudanca.
  // Caso estiver na aba de MessageBox, permite mudanca apenas para aba de PowerDown e Espera
  if(CurrentWorkArea == NewWorkArea) {
    return;
  } else if(CurrentWorkArea != NTB_ABA_ESPERA && (CurrentWorkArea != NTB_ABA_MESSAGEBOX ||
		  NewWorkArea == NTB_ABA_ESPERA || NewWorkArea == NTB_ABA_POWERDOWN)) {
    // Se estiver indo para a aba HOME, exclui lista de telas anteriores e desmarca tela atual.
    if(NewWorkArea == MaqConfigCurrent->AbaHome) {
      CurrentWorkArea = NTB_ABA_NONE;
      while(WA_PreviousList != NULL)
        RemoveWA(&WA_PreviousList);
    }

    gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbWorkArea")), NewWorkArea);

    if((NewWorkArea == MaqConfigCurrent->AbaHome || NewWorkArea == MaqConfigCurrent->AbaOperAuto) && CurrentStatus == MAQ_STATUS_INDETERMINADO) {
      if(MaqConfigCurrent->UseIndet) {
        WorkAreaGoTo(NTB_ABA_INDETERMINADO);
      } else {
        SetMaqStatus(MAQ_STATUS_PARADA);
      }
    }
  } else if(NewWorkArea != NTB_ABA_ESPERA && NewWorkArea != LastWA(WA_PreviousList)) {
	// Nao pode mudar de aba no momento. Adiciona a nova aba na lista de anteriores e, ao sair da aba atual, a nova sera selecionada.
    AddWA(&WA_PreviousList, NewWorkArea);
  }
}

void WorkAreaGoPrevious()
{
  CurrentWorkArea = NTB_ABA_NONE;
  WorkAreaGoTo(RemoveWA(&WA_PreviousList));
}

int WorkAreaGet()
{
  return CurrentWorkArea;
}

void cbMessageBoxOk(GtkButton *button, gpointer user_data)
{
	if(gtk_widget_get_visible(GTK_WIDGET(gtk_builder_get_object(builder, "imgMessageBoxErro")))) {
		MaqLimparErro();
	}

	WorkAreaGoPrevious();
}

void cbGoPrevious(GtkButton *button, gpointer user_data)
{
  WorkAreaGoPrevious();
}

void cbGoHome(GtkButton *button, gpointer user_data)
{
  WorkAreaGoTo(MaqConfigCurrent->AbaHome);
}

void cbGoManut(GtkButton *button, gpointer user_data)
{
  WorkAreaGoTo(MaqConfigCurrent->AbaManut);
}

void cbIconPress (GtkEntry *entry, GtkEntryIconPosition icon_pos, GdkEvent *event, gpointer user_data)
{
	VKData vkdata;

	vkdata.type = VK_TYPE_WIDGET;
	vkdata.obj.widget = GTK_WIDGET(entry);
	AbrirVirtualKeyboard(&vkdata);
	WorkAreaGoTo(NTB_ABA_VIRTUAL_KB);
}

// Funcoes que aumentam/reduzem horas e minutos.
void DataAtualizaHora(int32_t inc)
{
  int32_t val;
  char tmp[10];
  GtkEntry *entry = GTK_ENTRY(gtk_builder_get_object(builder, "entDataHora"));

  val = atoi(gtk_entry_get_text(entry)) + inc;
  if(val > 23)
    val = 23;
  else if(val < 0)
    val = 0;

  sprintf(tmp, "%d", val);
  gtk_entry_set_text(entry, tmp);
}

void cbDataAumentaHora(GtkButton *button, gpointer user_data)
{
  DataAtualizaHora(+1);
}

void cbDataReduzHora(GtkButton *button, gpointer user_data)
{
  DataAtualizaHora(-1);
}

void DataAtualizaMinuto(int32_t inc)
{
  int32_t val;
  char tmp[10];
  GtkEntry *entry = GTK_ENTRY(gtk_builder_get_object(builder, "entDataMinuto"));

  val = atoi(gtk_entry_get_text(entry)) + inc;
  if(val > 59)
    val = 59;
  else if(val < 0)
    val = 0;

  sprintf(tmp, "%d", val);
  gtk_entry_set_text(entry, tmp);
}

void cbDataAumentaMinuto(GtkButton *button, gpointer user_data)
{
  DataAtualizaMinuto(+1);
}

void cbDataReduzMinuto(GtkButton *button, gpointer user_data)
{
  DataAtualizaMinuto(-1);
}

// Funcao executada quando for clicado voltar na tela de data
void cbDataVoltar(GtkButton *button, gpointer user_data)
{
  guint signal_id;
  gulong handler_id;
  GtkWidget *obj = GTK_WIDGET(gtk_builder_get_object(builder, "btnDataAplicar"));

  // Exclui o callback do botao de Aplicar
  signal_id = g_signal_lookup("clicked", GTK_TYPE_BUTTON);
  handler_id = g_signal_handler_find(obj, G_SIGNAL_MATCH_ID, signal_id, 0, NULL, NULL, NULL);
  g_signal_handler_disconnect(obj, handler_id);

  WorkAreaGoPrevious();
}

// Função que abre a janela de data, carregando a data de entry e
// associando a função cb como callback do botão de aplicar data.
void AbrirData(GtkEntry *entry, GCallback cb)
{
  gint v1, v2, v3;
  char *data, tmp[30];
  GtkWidget *obj;

// Carrega o ponteiro para o botão de aplicar tarefa.
  obj = GTK_WIDGET(gtk_builder_get_object(builder, "btnDataAplicar"));

// Conexão dos sinais de callback
  g_signal_connect ((gpointer) obj, "clicked",  cb,
              (gpointer)(entry));

// Lê a data atualmente inserida no entry box.
  data = (char *)gtk_entry_get_text(entry);

// Carrega o ano
  strncpy(tmp, data, 4);
  tmp[4] = 0;
  v1 = atoi(tmp);

// Carrega o mês
  strncpy(tmp, data+5, 2);
  tmp[2] = 0;
  v2 = atoi(tmp);

// Carrega o dia
  strncpy(tmp, data+8, 2);
  tmp[2] = 0;
  v3 = atoi(tmp);

// Carrega a data no calendário.
  obj = GTK_WIDGET(gtk_builder_get_object(builder, "cldData"));
  gtk_calendar_select_month(GTK_CALENDAR(obj), v2-1, v1);
  gtk_calendar_select_day  (GTK_CALENDAR(obj), v3);

// Carrega a hora
  strncpy(tmp, data+11, 2);
  tmp[2] = 0;
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entDataHora"  )), tmp);

// Carrega os minutos
  strncpy(tmp, data+14, 2);
  tmp[2] = 0;
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entDataMinuto")), tmp);

// Exibe a janela.
  WorkAreaGoTo(NTB_ABA_DATA);
}

// Aplicar alteracao de Data/Hora do sistema
void cbAplicarDataHora(GtkButton *button, gpointer user_data)
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

  sprintf(tmp, "date %02d%02d%02d%02d%04d.00", m+1, d, hora, min, y);
  system(tmp);

  // Atualiza a hora da POP-7
  MaqSetDateTime(NULL);

  WorkAreaGoPrevious();
}

// Alterar Data/Hora do sistema
void cbMudarDataHora(GtkButton *button, gpointer user_data)
{
  time_t t;
  struct tm *now;
  char tmp[100];
  GtkEntry *entry;

  time(&t);
  now = localtime(&t);
  sprintf(tmp, "%d-%02d-%02d %02d:%02d:00", 1900 + now->tm_year,
      now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min);

  entry = GTK_ENTRY(gtk_entry_new());
  gtk_entry_set_text(entry, tmp);

  AbrirData(entry, G_CALLBACK (cbAplicarDataHora));

  gtk_widget_destroy(GTK_WIDGET(entry));
}

// Funcao executada quando for clicado Mudar Senha na tela de Login
void cbMudarSenha(GtkButton *button, gpointer user_data)
{
  char user[100];
  // Limpa os campos de texto
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entSenhaAtual"   )), "");
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entSenhaNova"    )), "");
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entSenhaConfirma")), "");
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entSenhaLembrete")), "");

  // Carrega campo exibindo o usuario sendo alterado
  sprintf(user, "Alterando senha para o usuário %s",
      LerComboAtivo(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbLoginUser"))));
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblSenhaUsuario")), user);

  WorkAreaGoTo(NTB_ABA_MUDAR_SENHA);
}

// Funcao executada quando for aplicada a alteracao de senha
void cbSenhaAlterar(GtkButton *button, gpointer user_data)
{
  int i;
  struct strDB *sDB;
  char senha[100], sql[100];
  char *user     = (char*)gtk_label_get_text(GTK_LABEL(gtk_builder_get_object(builder, "lblSenhaUsuario" )));
  char *lembrete = (char*)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entSenhaLembrete")));

  strcpy(senha, gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entSenhaNova"))));
  if(strcmp(senha, gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entSenhaConfirma"))))) {
    MessageBox("Nova senha e confirmação são diferentes!");
    return;
  }

  for(i=strlen(user)-1; i>=0; i--) {
    if(user[i] == ' ') {
      i++;
      break;
    }
  }

  if(i>0) {
#ifndef DISABLE_SQL_SERVER
    sDB = MSSQL_Connect();
#else
    sDB = &mainDB;
#endif

    if(sDB != NULL) {
      char *oldpwd;
      sprintf(sql, "select SENHA from OPERADOR where USUARIO='%s'", user+i);

#ifndef DISABLE_SQL_SERVER
      MSSQL_Execute(0, sql, MSSQL_DONT_SYNC);
      DB_GetNextRow(sDB, 0);
      oldpwd = MSSQL_GetData(0, 0);
#else
      DB_Execute(sDB, 0, sql);
      DB_GetNextRow(sDB, 0);
      oldpwd = DB_GetData(sDB, 0, 0);
#endif

      if(!strcmp(Crypto((char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entSenhaAtual")))), oldpwd)) {
        sprintf(sql, "update OPERADOR set SENHA='%s', LEMBRETE='%s' where USUARIO='%s'", Crypto(senha), lembrete, user+i);

#ifndef DISABLE_SQL_SERVER
        MSSQL_Execute(0, sql, MSSQL_DONT_SYNC);
#else
        DB_Execute(sDB, 0, sql);
#endif

        MessageBox("Senha alterada com sucesso!");
        sprintf(sql, "Alterada senha do usuario %s", user+i);
        Log(sql, LOG_TIPO_SISTEMA);
        WorkAreaGoPrevious();
      } else {
        MessageBox("Senha atual inválida!");
      }
    }
  }
}

// Funcao executada quando for cancelada a alteracao de senha
void cbSenhaCancelar(GtkButton *button, gpointer user_data)
{
  WorkAreaGoPrevious();
}

// Funcao executada quando for clicado botao "Calcular Fator de Correcao do Encoder"
void cbCalcFatorPerfil(GtkButton *button, gpointer user_data)
{
  // Limpa os campos das medidas
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entCalcFatorTamMedido" )), "");
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entCalcFatorTamCorreto")), "");

  // Vai para a tela de calculo do fator
  WorkAreaGoTo(NTB_ABA_CALC_FATOR);
}

// Funcao executada quando clicar para confirmar alteracao do fator do encoder
void cbCalcFatorConfirma(GtkButton *button, gpointer user_data)
{
  int   NovoFator;
  char  StringFator[20];
  int   TamMedido     = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entCalcFatorTamMedido" ))));
  int   TamCorreto    = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entCalcFatorTamCorreto"))));
  float FatorAnterior = atof(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entEncoderFator"       ))));;

  if(FatorAnterior <= 0.0f) {
    FatorAnterior = 1.0f;
  }

  if(TamCorreto && TamMedido) {
    NovoFator = (int)(((float)(TamMedido*100000)*FatorAnterior) / TamCorreto);

    // Arredonda e depois remove a casa adicional
    if((NovoFator%10)>5) {
      NovoFator += 10;
    }
    NovoFator /= 10;

    sprintf(StringFator, "%.04f", (float)(NovoFator)/10000);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entEncoderFator")), StringFator);
  } else {
    MessageBox("Parâmetros inválidos!\nAs medidas devem ser diferentes de zero!");
  }

  WorkAreaGoPrevious();
}

// Funcao executada quando clicar para cancelar alteracao do fator do encoder
void cbCalcFatorCancelar(GtkButton *button, gpointer user_data)
{
  WorkAreaGoPrevious();
}
