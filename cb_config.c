// Para que a funcao atof funcione corretamente
#include <stdio.h>
#include <stdlib.h>

#include <net/modbus.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "defines.h"
#include "GtkUtils.h"

// Estrutura que representa o ModBus
extern struct MB_Device mbdev;

/*** Funcoes e variáveis de suporte ***/

int idUser=0; // Indica usuário que logou se for diferente de zero.
int CurrentWorkArea  = 0;  // Variavel que armazena a tela atual.
int PreviousWorkArea = 0; // Variavel que armazena a tela anterior.

// Função que salva um log no banco contendo usuário e data que gerou o evento.
extern void Log(char *evento, int tipo);

char * Crypto(char *str)
{
  return (char *)(crypt(str,"bc")+2);
}

int GetUserPerm(char *permissao)
{
  char sql[300];

  sprintf(sql, "select p.valor from permissoes as p, lista_permissoes as l, usuarios as u where p.ID_perm = l.ID and p.ID_user = u.ID and l.nome = '%s' and u.id = '%d'", permissao, idUser);
  DB_Execute(&mainDB, 0, sql);

// Se existe permissão para o usuário atual, retorna seu valor.
  if(DB_GetNextRow(&mainDB, 0)>0)
    return atoi(DB_GetData(&mainDB, 0, 0));
  else
    {
// Não encontrou a permissão para usuário atual, carregando default.
    sprintf(sql, "select valor from lista_permissoes where nome='%s'", permissao);
    DB_Execute(&mainDB, 0, sql);
    if(DB_GetNextRow(&mainDB, 0)>0)
      return atoi(DB_GetData(&mainDB, 0, 0)); // Retornando valor default
    }

// Não existe a permissão. Retorna zero.
  return 0;
}

void CarregaComboUsuarios()
{
  GtkWidget *obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbCadUserLogin"));

// O primeiro item é para inserção de novo usuário.
// Carregamento dos usuário cadastrados no MySQL no ComboBox.
  DB_Execute(&mainDB, 0, "select login from usuarios order by ID");
  CarregaCombo(GTK_COMBO_BOX(obj),0, "<Novo Usuário>");
}

void CarregaComboModelos()
{
  char sql[100];
  GtkWidget *obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbModNome"));

// O primeiro item é para inserção de novo modelo.
// Carregamento dos modelos cadastrados no MySQL no ComboBox.
  sprintf(sql, "select nome from modelos where estado='%d' order by ID", MOD_ESTADO_ATIVO);
  DB_Execute(&mainDB, 0, sql);
  CarregaCombo(GTK_COMBO_BOX(obj), 0, "<Novo Modelo>");
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
    "spbConfigAplanVelMaxManual",
    "spbConfigAplanVelMaxAuto",
    "spbConfigAplanRampa",
    ""
};

int GravarDadosConfig()
{
  unsigned int i;
  char **valor_ent;
  struct strMaqParam mp;
  valor_ent = (char **)(malloc(11*sizeof(char[10])));

  // Carrega o valor dos widgets conforme a lista fornecida
  LerValoresWidgets(lista_ent, valor_ent);
  for(i=0; lista_ent[i][0]; i++)
    printf("%d: %s = %s\n" , i, lista_ent[i], valor_ent[i]);

  mp.encoder.fator         = atof(valor_ent[ 1]);
  mp.encoder.precisao      = atol(valor_ent[ 2]);
  mp.encoder.perimetro     = atof(valor_ent[ 0]);
  MaqConfigEncoder(mp.encoder);

  mp.aplanadora.manual_vel = atol(valor_ent[ 3]);
  mp.aplanadora.auto_vel   = atol(valor_ent[ 4]);
  mp.aplanadora.rampa      = atof(valor_ent[ 5]);
  MaqConfigAplan(mp.aplanadora);

  GravaDadosBanco();

  MaqGravarConfig();

  free(valor_ent);

  return 0;
}

void LerDadosConfig()
{
  char tmp[100];
  unsigned int i;
  struct strMaqParam mp;
  char **valor_ent;

  // Diminui em 1 o número de campos devido ao elemento vazio no final.
  i = sizeof(lista_ent)/sizeof(lista_ent[0])-1;
  valor_ent = (char **)(malloc(i * sizeof(char *)));

  mp.encoder    = MaqLerEncoder();
  mp.aplanadora = MaqLerAplan  ();

  sprintf(tmp, "%f", mp.encoder.fator);
  valor_ent[1] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[1], tmp);

  sprintf(tmp, "%d", mp.encoder.precisao);
  valor_ent[2] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[2], tmp);

  sprintf(tmp, "%d", mp.encoder.perimetro);
  valor_ent[0] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[0], tmp);

  sprintf(tmp, "%d", mp.aplanadora.manual_vel);
  valor_ent[3] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[3], tmp);

  sprintf(tmp, "%d", mp.aplanadora.auto_vel);
  valor_ent[4] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[4], tmp);

  sprintf(tmp, "%f", mp.aplanadora.rampa);
  valor_ent[5] = (char *)malloc(sizeof(tmp)+1);
  strcpy(valor_ent[5], tmp);

  GravarValoresWidgets(lista_ent, valor_ent);

  while(i--) {
    if(valor_ent[i] != NULL)
      free(valor_ent[i]);
  }
  free(valor_ent);

  if(mainDB.status & DB_FLAGS_CONNECTED) // Somente carrega se há conexão com o DB
    {
    // Carrega dados da aba de usuários
    CarregaComboUsuarios();

    // Carrega dados da aba de modelos
    CarregaComboModelos();
    }

// Carrega dados da aba de Banco de Dados
  CarregaDadosBanco();
}

void cbLoginUserSelected(GtkComboBox *combobox, gpointer user_data)
{
  char *lst_campos[] =
    {
      "entNome"    , "nome",
      "entSenha"   , "senha",
      "entLembrete", "lembrete",
      ""           , ""
    };

  char *lst_botoes[] = { "btnExcluir", "btnPerm", "" };

  if(gtk_combo_box_get_active(combobox)>=0)
    CarregaCampos(combobox, lst_campos, lst_botoes, "usuarios", "login");
}

void cbAplicarUsuario(GtkButton *button, gpointer user_data)
{
  char sql[400], *nome, *senha, *lembrete, *login;
  GtkWidget *dialog, *obj, *wnd, *entry;

  obj = GTK_WIDGET(gtk_builder_get_object(builder, "entNome"));
  nome = (char *)(gtk_entry_get_text(GTK_ENTRY(obj)));

  obj = GTK_WIDGET(gtk_builder_get_object(builder, "entSenha"));
  senha = Crypto((char *)(gtk_entry_get_text(GTK_ENTRY(obj))));

  obj = GTK_WIDGET(gtk_builder_get_object(builder, "entLembrete"));
  lembrete = (char *)(gtk_entry_get_text(GTK_ENTRY(obj)));

  obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbCadUserLogin"));
  if(gtk_combo_box_get_active(GTK_COMBO_BOX(obj)) != 0) // Não é o primeiro item, alterar usuário.
    {
    dialog = gtk_message_dialog_new (NULL,
              GTK_DIALOG_DESTROY_WITH_PARENT,
              GTK_MESSAGE_QUESTION,
              GTK_BUTTONS_YES_NO,
              "Aplicar as alterações ao usuário '%s'?",
              LerComboAtivo(GTK_COMBO_BOX(obj)));

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
      {
      sprintf(sql, "update usuarios set nome='%s', senha='%s', lembrete='%s' where login='%s'",
        nome, senha, lembrete, LerComboAtivo(GTK_COMBO_BOX(obj)));
      DB_Execute(&mainDB, 0, sql);

      sprintf(sql, "Alterado o usuário %s", LerComboAtivo(GTK_COMBO_BOX(obj)));
      Log(sql, LOG_TIPO_SISTEMA);

      MessageBox("Usuário alterado com sucesso!");
      }
    }
  else
    {
    wnd = GTK_WIDGET(gtk_builder_get_object(builder, "wndConfig"));
    dialog = gtk_dialog_new_with_buttons ("Digite o login do usuário:",
          GTK_WINDOW(wnd),
          GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL | GTK_DIALOG_NO_SEPARATOR,
          GTK_STOCK_OK,
          GTK_RESPONSE_OK,
          GTK_STOCK_CANCEL,
          GTK_RESPONSE_CANCEL,
          NULL);

    // TODO: Ler do banco de dados o tamanho do campo de login.
    entry = gtk_entry_new_with_max_length(10);
    gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), entry);
    AbrirJanelaModal(dialog);

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
      {
      login = (char *)(gtk_entry_get_text(GTK_ENTRY(entry)));
      sprintf(sql, "select ID from usuarios where login='%s'", login);
      DB_Execute(&mainDB, 0, sql);
      if(DB_GetNextRow(&mainDB, 0)>0)
        {
        wnd = gtk_message_dialog_new (NULL,
                  GTK_DIALOG_DESTROY_WITH_PARENT,
                  GTK_MESSAGE_ERROR,
                  GTK_BUTTONS_OK,
                  "O usuário '%s' já existe!",
                  login);

        gtk_dialog_run(GTK_DIALOG(wnd));
        gtk_widget_destroy (wnd);
        }
      else // O usuário não existe. Realizando inserção.
        {
        sprintf(sql, "insert into usuarios (nome, senha, lembrete, login) "
               "values ('%s', '%s', '%s', '%s')",
               nome, senha, lembrete, login);
        DB_Execute(&mainDB, 0, sql);

        CarregaItemCombo(GTK_COMBO_BOX(obj), login);
        gtk_combo_box_set_active(GTK_COMBO_BOX(obj), 0);
        cbLoginUserSelected(GTK_COMBO_BOX(obj), NULL);

        sprintf(sql, "Adicionado o usuário %s", login);
        Log(sql, LOG_TIPO_SISTEMA);

        MessageBox("Usuário adicionado com sucesso!");
        }
      }
    }

  gtk_widget_destroy (dialog);
}

void cbExcluirUsuario(GtkButton *button, gpointer user_data)
{
  int id;
  char sql[100], *ativo;
  GtkWidget *dialog, *obj;

  obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbCadUserLogin"));
  if(gtk_combo_box_get_active(GTK_COMBO_BOX(obj)) != 0) // Não é o primeiro item, exclui usuário.
    {
    ativo = LerComboAtivo(GTK_COMBO_BOX(obj));
    sprintf(sql, "select ID from usuarios where login='%s'", ativo);
    DB_Execute(&mainDB, 0 ,sql);
    id = atoi(DB_GetData(&mainDB, 0, 0));
    if(id == idUser) // Tentando remover usuário atual!
      {
      sprintf(sql, "O usuário '%s' é o usuário atual, não pode ser excluído!", ativo);
      MessageBox(sql);
      return; // Não pode excluir o usuário atual. Retorna!
      }

    dialog = gtk_message_dialog_new (NULL,
              GTK_DIALOG_DESTROY_WITH_PARENT,
              GTK_MESSAGE_QUESTION,
              GTK_BUTTONS_YES_NO,
              "Tem certeza que deseja excluir o usuário '%s'?",
              ativo);

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
      {
      sprintf(sql, "Removendo usuário %s", ativo);
      Log(sql, LOG_TIPO_SISTEMA);

      sprintf(sql, "update log set ID_Usuario='1' where ID_Usuario='%d'", id);
      DB_Execute(&mainDB, 0, sql);
      sprintf(sql, "delete from permissoes where ID_user='%d'", id);
      DB_Execute(&mainDB, 0, sql);
      sprintf(sql, "delete from usuarios where login='%s'", ativo);
      DB_Execute(&mainDB, 0, sql);
      ExcluiItemCombo(GTK_COMBO_BOX(obj), gtk_combo_box_get_active(GTK_COMBO_BOX(obj)));
      gtk_combo_box_set_active(GTK_COMBO_BOX(obj), 0);

      MessageBox("Usuário removido com sucesso!");
      }

    gtk_widget_destroy (dialog);
    }
}

void SaindoUserPerms(gpointer user_data)
{
  guint signal_id;
  gulong handler_id;
  GtkWidget *wdg;
  GtkContainer *container = GTK_CONTAINER(gtk_builder_get_object(builder, "tblPerms"));
  GList *start, *lst = gtk_container_get_children(container);

  start = lst;
  while(lst) {
    gtk_container_remove(container, GTK_WIDGET(lst->data));
    lst = lst->next;
  }

  signal_id = g_signal_lookup("clicked", GTK_TYPE_BUTTON);

  wdg = GTK_WIDGET(gtk_builder_get_object(builder, "btnUserPermsOK"));
  handler_id = g_signal_handler_find((gpointer)wdg, G_SIGNAL_MATCH_ID, signal_id, 0, NULL, NULL, NULL);
  g_signal_handler_disconnect((gpointer)wdg, handler_id);

  wdg = GTK_WIDGET(gtk_builder_get_object(builder, "btnUserPermsCancel"));
  handler_id = g_signal_handler_find((gpointer)wdg, G_SIGNAL_MATCH_ID, signal_id, 0, NULL, NULL, NULL);
  g_signal_handler_disconnect((gpointer)wdg, handler_id);

  gtk_grab_remove(GTK_WIDGET(gtk_builder_get_object(builder, "wndUserPerms")));
  free((int *)(user_data));
  g_list_free(start);
}

void cbUserPermsCancel(GtkButton *button, gpointer user_data)
{
  gtk_widget_hide_all(GTK_WIDGET(gtk_builder_get_object(builder, "wndUserPerms")));
  SaindoUserPerms(user_data);
}

void cbUserPermsAtual(GtkButton *button, gpointer user_data)
{
  int i=0, val_perm, inserir = 0;
  char sql[100];
  GList *lst = gtk_container_get_children(GTK_CONTAINER(gtk_builder_get_object(builder, "tblPerms")));

  sprintf(sql,"delete from permissoes where ID_User=%d", *(int *)(user_data));
  DB_Execute(&mainDB, 0, sql);

  while(lst)
    {
    if(GTK_IS_ENTRY(lst->data))
      {
      inserir = 1;
      val_perm = atoi(gtk_entry_get_text(GTK_ENTRY(lst->data)));
      }
    else if(GTK_IS_CHECK_BUTTON(lst->data))
      {
      inserir = 1;
      val_perm = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lst->data));
      }

    if(inserir)
      {
      sprintf(sql,"insert into permissoes values (%d, %d, %d)",
        *(int *)(user_data),
        *((int *)(user_data)+i+1),
        val_perm);
      DB_Execute(&mainDB, 0, sql);

      i++;
      inserir = 0;
      }

    lst = lst->next;
    }

  gtk_widget_hide_all(GTK_WIDGET(gtk_builder_get_object(builder, "wndUserPerms")));
  SaindoUserPerms(user_data);
}

void cbAbrirUserPerms(GtkButton *button, gpointer user_data)
{
  char sql[100], tmp[30];
  int i=0, tam, id_user, val_perm;
  GtkWidget *wnd, *tbl, *wdg, *combo;
// Ponteiro para uma lista de IDs para a futura inserção / alteração dos dados no banco.
  int *ptrIDs;

  wnd = GTK_WIDGET(gtk_builder_get_object(builder, "wndUserPerms"));
  tbl = GTK_WIDGET(gtk_builder_get_object(builder, "tblPerms"));

  combo = GTK_WIDGET(gtk_builder_get_object(builder, "cmbCadUserLogin"));
  sprintf(sql, "select ID from usuarios where login='%s'", LerComboAtivo(GTK_COMBO_BOX(combo)));

  DB_Execute(&mainDB, 0, sql);
  DB_GetNextRow(&mainDB, 0);

  id_user = atoi((char *)DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "ID")));

  DB_Execute(&mainDB, 0, "select ID, descricao, valor, tipo from lista_permissoes order by ID");
  DB_GetNextRow(&mainDB, 0);

  tam = DB_GetCount(&mainDB, 0);

  if(tam>0)
    {
    // Aloca o ponteiro para os IDs.
    // Deve ser desalocado pela função de callback do sinal destroy da janela.
    ptrIDs = (int *)(malloc(sizeof(int)*(tam+1)));
    gtk_table_resize(GTK_TABLE(tbl), tam, 2);

// Conexão dos sinais de callback
    wdg = GTK_WIDGET(gtk_builder_get_object(builder, "btnUserPermsOK"));
    g_signal_connect ((gpointer) wdg, "clicked",
      G_CALLBACK (cbUserPermsAtual ), (gpointer)(ptrIDs));

    wdg = GTK_WIDGET(gtk_builder_get_object(builder, "btnUserPermsCancel"));
    g_signal_connect ((gpointer) wdg, "clicked",
      G_CALLBACK (cbUserPermsCancel), (gpointer)(ptrIDs));

// A primeira posicao contém o ID do usuário.
    *ptrIDs = id_user;

    do
      {
      *(ptrIDs+tam-i) = atoi((char *)DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "ID")));

      Asc2Utf((unsigned char *)DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "descricao")), (unsigned char *)tmp);
      wdg = gtk_label_new(tmp);
      gtk_table_attach_defaults(GTK_TABLE(tbl), wdg, 0, 1, i, i+1);

      sprintf(sql, "select valor from permissoes where ID_user=%d and ID_perm=%d", id_user, *(ptrIDs+tam-i));
      DB_Execute(&mainDB, 1, sql);
      if(DB_GetNextRow(&mainDB, 1)>0) // Carrega o valor da permissão para este usuário
        strcpy(tmp, DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "valor")));
      else // A permissão do usuário não existe. Carrega o valor default da permissão
        strcpy(tmp, DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "valor")));

      switch(atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "tipo"))))
        {
        case PERM_TIPO_BOOL:
          val_perm = atoi(tmp);
          wdg      = gtk_check_button_new();
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wdg), val_perm);

          break;

        default:
        case PERM_TIPO_INT:
          wdg = gtk_entry_new_with_max_length(5); // Tamanho de um inteiro
          gtk_entry_set_text(GTK_ENTRY(wdg), tmp);

          break;
        }

      gtk_table_attach_defaults(GTK_TABLE(tbl), wdg, 1, 2, i, i+1);

      i++;
      } while(DB_GetNextRow(&mainDB, 0)>0);

    AbrirJanelaModal(wnd);
    gtk_grab_add(wnd);
    }
}

gboolean ChecarModelo()
{
  char *msg = NULL;
  GtkWidget *dlg;
  char nome_wdg[20];
  unsigned int i, p;

  for(i=0; i<MAQ_PASSOS_MAX; i++) {
    maq_param.aplanadora.passos[i].passo      = 0;
    maq_param.aplanadora.passos[i].repeticoes = 0;
    maq_param.aplanadora.passos[i].portas     = 0;
  }

  for(i=0; i<MAQ_PASSOS_MAX; i++) {
    sprintf(nome_wdg, "entModRep%d", i+1);
    maq_param.aplanadora.passos[i].repeticoes = strtoul(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, nome_wdg))), NULL, 0);
    if(!maq_param.aplanadora.passos[i].repeticoes) { // repeticoes sendo zero indica que o passo não é usado
      if(!i) { // Primeiro passo e repeticoes zerado. programa invalido!
        msg = "Quantidade deve ser preenchido no primeiro passo!";
      }
      break;
    }

    sprintf(nome_wdg, "entModPasso%d", i+1);
    maq_param.aplanadora.passos[i].passo = strtoul(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, nome_wdg))), NULL, 0);

    for(p = 0; p < 5; p++) {
      sprintf(nome_wdg, "ckbModPasso%dP%d", i+1, p+1);
      maq_param.aplanadora.passos[i].portas |= (unsigned int)(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, nome_wdg))))<<p;
    }
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

void cbConfigModeloSelecionado(GtkComboBox *combobox, gpointer user_data)
{
  char nome_wdg[20], valor[20];
  unsigned int passos, i, porta, ativo;

  if(gtk_combo_box_get_active(combobox)<0)
    return; // Sem item ativo.

  passos = CarregarPrograma(LerComboAtivo(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbModNome"))));

  // Se não carregou nenhum passo foi escolhida a opção de novo modelo. Desativando o botão Remover. Caso contário, ativa
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnModRemover")), passos ? TRUE : FALSE);

  for(i=0; i<MAQ_PASSOS_MAX; i++) {
    sprintf(nome_wdg, "entModPasso%d", i+1);
    if(i<passos) {
      sprintf(valor, "%d", maq_param.aplanadora.passos[i].passo);
    } else {
      valor[0] = 0;
    }
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, nome_wdg)), valor);

    sprintf(nome_wdg, "entModRep%d", i+1);
    if(i<passos) {
      sprintf(valor, "%d", maq_param.aplanadora.passos[i].repeticoes);
    } else {
      valor[0] = 0;
    }
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, nome_wdg)), valor);

    for(porta = 0; porta < 5; porta++) {
      sprintf(nome_wdg, "ckbModPasso%dP%d", i+1, porta+1);
      ativo = (maq_param.aplanadora.passos[i].portas>>porta)&1 ? TRUE : FALSE;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, nome_wdg)), i<passos ? ativo : FALSE);
    }
  }
}

void cbAplicarModelo(GtkButton *button, gpointer user_data)
{
  char sql[400], *nome;
  unsigned int id_modelo, i, inserir = 0;
  GtkWidget *dialog, *obj, *wnd, *entry;

// Checa se os dados são válidos.
  if(!ChecarModelo())
    return;

  obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbModNome"));
  if(gtk_combo_box_get_active(GTK_COMBO_BOX(obj)) != 0) // Não é o primeiro item, alterar usuário.
    {
    nome = LerComboAtivo(GTK_COMBO_BOX(obj));
    dialog = gtk_message_dialog_new (NULL,
              GTK_DIALOG_DESTROY_WITH_PARENT,
              GTK_MESSAGE_QUESTION,
              GTK_BUTTONS_YES_NO,
              "Aplicar as alterações ao modelo '%s'?",
              nome);

    if(gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_YES)
      return;
    }
  else
    {
    wnd = GTK_WIDGET(gtk_builder_get_object(builder, "wndDesktop"));
    dialog = gtk_dialog_new_with_buttons ("Digite o nome do modelo:",
          GTK_WINDOW(wnd),
          GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL | GTK_DIALOG_NO_SEPARATOR,
          GTK_STOCK_OK,
          GTK_RESPONSE_OK,
          GTK_STOCK_CANCEL,
          GTK_RESPONSE_CANCEL,
          NULL);

    // TODO: Ler do banco de dados o tamanho do campo de nome.
    entry = gtk_entry_new_with_max_length(20);
    gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), entry);
    AbrirJanelaModal(dialog);

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
      {
      nome = (char *)(gtk_entry_get_text(GTK_ENTRY(entry)));
      sprintf(sql, "select ID from modelos where nome='%s'", nome);
      DB_Execute(&mainDB, 0, sql);
      if(DB_GetNextRow(&mainDB, 0)>0)
        {
        wnd = gtk_message_dialog_new (NULL,
                  GTK_DIALOG_DESTROY_WITH_PARENT,
                  GTK_MESSAGE_ERROR,
                  GTK_BUTTONS_OK,
                  "O modelo '%s' já existe!",
                  nome);

        gtk_dialog_run(GTK_DIALOG(wnd));
        gtk_widget_destroy (wnd);
        gtk_widget_destroy (dialog);
        return;
        }
      else // O modelo não existe. Realizando inserção.
        {
        inserir = 1;
        }
      }
    }

  if(inserir) {
    sprintf(sql, "insert into modelos (nome, estado) values ('%s', '%d')", nome, MOD_ESTADO_ATIVO);
    DB_Execute(&mainDB, 0, sql);
  }

  sprintf(sql, "select ID from modelos where nome='%s'", nome);
  DB_Execute(&mainDB, 0, sql);
  if(DB_GetNextRow(&mainDB, 0)>0) {
    id_modelo = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "ID")));
  } else {
    printf("ERRO!!! Modelo inexistente\n");
    return;
  }

  sprintf(sql, "delete from modelos_programas where ID_modelo=%d", id_modelo);
  DB_Execute(&mainDB, 0, sql);

  for(i=0; i<MAQ_PASSOS_MAX; i++) {
    if(!maq_param.aplanadora.passos[i].repeticoes)
      break; // Sai do loop se acabaram os passos

    sprintf(sql, "insert into modelos_programas (ID_modelo, passo, repeticoes, portas) values ('%d', '%d', '%d', '%d')",
        id_modelo, maq_param.aplanadora.passos[i].passo, maq_param.aplanadora.passos[i].repeticoes, maq_param.aplanadora.passos[i].portas);
    DB_Execute(&mainDB, 0, sql);
  }

  if(inserir) {
    CarregaItemCombo(GTK_COMBO_BOX(obj), nome);

    sprintf(sql, "Adicionado modelo %s", nome);
    Log(sql, LOG_TIPO_CONFIG);

    MessageBox("Modelo adicionado com sucesso!");
  } else {
    sprintf(sql, "Alterado modelo %s", nome);
    Log(sql, LOG_TIPO_CONFIG);

    MessageBox("Modelo alterado com sucesso!");
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(obj), 0);
  cbConfigModeloSelecionado(GTK_COMBO_BOX(obj), NULL);

  gtk_widget_destroy (dialog);
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

void AbrirConfig(unsigned int pos)
{
  if(mainDB.status & DB_FLAGS_CONNECTED) // Banco de dados conectado!
    if(!GetUserPerm(PERM_ACESSO_CONFIG))
      {
      WorkAreaGoTo(NTB_ABA_HOME);
      MessageBox("Sem permissão para acesso à configuração!");
      return;
      }

  WorkAreaGoTo(NTB_ABA_CONFIG);

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

  Log("Alterada configuração da máquina", LOG_TIPO_CONFIG);

  GravarDadosConfig();
  WorkAreaGoTo(NTB_ABA_HOME);
}

void cbConfigVoltar(GtkButton *button, gpointer user_data)
{
  WorkAreaGoTo(NTB_ABA_HOME);
}

void cbLoginOk(GtkButton *button, gpointer user_data)
{
  int pos = 3; // Posição da aba de configuração do banco, iniciando de zero.
  char sql[100], *lembrete = "";
  static int first_time = 1;

  if(!(mainDB.status & DB_FLAGS_CONNECTED)) { // Banco não conectado!
    if(!strcmp(Crypto((char *)(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entLoginSenha"))))), SENHA_MASTER)) { // Senha correta
      idUser=1; // Grava 1 para indicar que foi logado

// Exibe a janela de configuração na aba de configuração do banco de dados
      AbrirConfig(pos);

      return;
    } else {
      lembrete = LEMBRETE_SENHA_MASTER;
    }
  } else {
    sprintf(sql, "select senha, lembrete, ID from usuarios where login='%s'",
      LerComboAtivo(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbLoginUser"))));

    DB_Execute(&mainDB, 0, sql);
    if(DB_GetNextRow(&mainDB, 0)>0) {
      if(!strcmp(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "senha")),Crypto((char *)(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entLoginSenha"))))))) {
        // Carrega o ID do usuário que está logando.
        idUser = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "ID")));
        Log("Entrada no sistema", LOG_TIPO_SISTEMA);

        // Oculta a janela de login.
        WorkAreaGoTo(NTB_ABA_HOME);

        // Limpa a senha digitada
        gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entLoginSenha")), "");

        // Executa apenas se for o primeiro login.
        if(first_time) {
          first_time = 0;

          // Sincroniza os parâmetros
          MaqSync(MAQ_SYNC_TODOS);

          // Configura a máquina para modo manual.
          MaqConfigModo(MAQ_MODO_MANUAL);

          // Libera o uso da máquina.
          MaqLiberar(1);
        }

        return;
      } else { // Erro durante login. Gera log informando esta falha.
        lembrete = DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "lembrete"));

        // Carrega Id para associar o log a este usuário
        idUser = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "ID")));

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

  tmpDB.server  = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoIP"   )));
  tmpDB.user    = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoUser" )));
  tmpDB.passwd  = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoSenha")));
  tmpDB.nome_db = (char *)gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entBancoNome" )));

  if(DB_Init(&tmpDB)) // Se inicializar o banco, precisamos verificar se existem as tabelas.
    {
// DB_Execute() retorna -1 se ocorrer um erro ou zero no sucesso.
// Se algum retornar algo diferente de zero existe um erro com o banco!
    if(DB_Execute(&tmpDB, 0, "select * from usuarios") ||
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
  union MB_FCD_Data data;
  struct MB_Reply rp;

  data.write_single_coil.output = atoi(&nome[strlen(nome)-2]);
  data.write_single_coil.val    = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));

  rp = MB_Send(&mbdev, MB_FC_WRITE_SINGLE_COIL, &data);

  sprintf(nome_img, "imgManutSai%02d", data.write_single_coil.output-1);
  gtk_image_set_from_stock(GTK_IMAGE(gtk_builder_get_object(builder, nome_img)),
      data.write_single_coil.val ? "gtk-apply" : "gtk-media-record", GTK_ICON_SIZE_BUTTON);

  if(rp.ExceptionCode != MB_EXCEPTION_NONE)
    printf("Erro escrevendo saida. Exception Code: %02x\n", rp.ExceptionCode);
}

void cbNotebookWorkAreaChanged(GtkNotebook *ntb, GtkNotebookPage *page, guint arg1, gpointer user_data)
{
  PreviousWorkArea = CurrentWorkArea;
  CurrentWorkArea  = arg1;
}

void cbManutAbaAlterada(GtkNotebook *ntb, GtkNotebookPage *page, guint arg1, gpointer user_data)
{
  if(arg1 == 0) { // Primeira aba é a de entradas, devemos ocultar o botão de voltar
    gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "btnManutVoltar")), 0);
  } else {
    gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "btnManutVoltar")), 1);
  }
}

void WorkAreaGoTo(int NewWorkArea)
{
  // Caso estiver na aba de MessageBox, nao permite mudanca.
  // Marca a anterior como a nova e, ao sair do message box, esta nova sera selecionada.
  if(CurrentWorkArea != NTB_ABA_MESSAGEBOX)
    gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbWorkArea")), NewWorkArea);
  else if(NewWorkArea != NTB_ABA_MESSAGEBOX)
    PreviousWorkArea = NewWorkArea;
}

void WorkAreaGoPrevious()
{
  WorkAreaGoTo(PreviousWorkArea);
}

int WorkAreaGet()
{
  return CurrentWorkArea;
}

void cbMessageBoxOk(GtkButton *button, gpointer user_data)
{
  MaqLimparErro();
  CurrentWorkArea = NTB_ABA_HOME;
  WorkAreaGoPrevious();
}

void cbGoPrevious(GtkButton *button, gpointer user_data)
{
  WorkAreaGoPrevious();
}

void cbGoHome(GtkButton *button, gpointer user_data)
{
  WorkAreaGoTo(NTB_ABA_HOME);
}

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
  GtkTextBuffer *tb;
  GtkTextIter cursor;
  gchar *str = (gchar *)gtk_button_get_label(button);

  // Verifica o label para identificar se foi clicado espaco, backspace ou enter.
  if        (!strcmp(str, "Enter")) {
    str = "\n";
  } else if (!strcmp(str, "Espaço")) {
    str = " ";
  } else if (!strcmp(str, "gtk-clear")) {
    str = NULL;
  }

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
    } else if(!strcmp(label, ".")) {
      strcpy(tmp, ",");
    } else if(!strcmp(label, ",")) {
      strcpy(tmp, ".");
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
  GtkWidget *widget = (GtkWidget *)user_data;

  tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(builder, "txvVirtualKeyboard")));
  gtk_text_buffer_get_start_iter(tb, &start);
  gtk_text_buffer_get_end_iter(tb, &end);
  data = (char *)(gtk_text_buffer_get_text(tb, &start, &end, FALSE));

  if(GTK_IS_ENTRY(widget)) {
    gtk_entry_set_text(GTK_ENTRY(widget), data);
  } else if(GTK_IS_TEXT_VIEW(widget)) {
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget)), data, -1);
  }

  SairVirtualKeyboard();
}

void cbVirtualKeyboardCancelar(GtkButton *button, gpointer user_data)
{
  SairVirtualKeyboard();
}

void AbrirVirtualKeyboard(GtkWidget *widget)
{
  char *data;
  GtkWidget *obj;
  GtkTextBuffer *tb;
  GtkTextIter start, end;

  // Lê o texto atual.
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

// Carrega o ponteiro para o botão de confirmar.
  obj = GTK_WIDGET(gtk_builder_get_object(builder, "btnVirtualKeyboardOK"));

// Conexão dos sinais de callback
  g_signal_connect ((gpointer) obj, "clicked",  G_CALLBACK(cbVirtualKeyboardOK),
              (gpointer)(widget));

  // Carrega o ponteiro para o botão de cancelar.
  obj = GTK_WIDGET(gtk_builder_get_object(builder, "btnVirtualKeyboardCancel"));

  // Conexão dos sinais de callback
  g_signal_connect ((gpointer) obj, "clicked",  G_CALLBACK(cbVirtualKeyboardCancelar),
                (gpointer)(widget));

// Carrega o texto
  gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(builder, "txvVirtualKeyboard"))), data, -1);

// Exibe a janela.
  WorkAreaGoTo(NTB_ABA_VIRTUAL_KB);
}

gboolean cbFocusIn(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
  static int ignorar = 0;

  if(!ignorar) {
    ignorar = 1;
    AbrirVirtualKeyboard(widget);
    return TRUE;
  }

  ignorar = 0;
  return FALSE;
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
