// Para que a funcao atof funcione corretamente
#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "defines.h"
#include "GtkUtils.h"

static const int FLAG_PROG_TAMANHO = 0x0200;
static const int FLAG_PROG_GRAVAR  = 0x0800;

static const int FLAG_CMD_MESA_SUBIR    = 0x0002;
static const int FLAG_CMD_MESA_DESCER   = 0x2000;
static const int FLAG_CMD_APLAN_SUBIR   = 0x4000;
static const int FLAG_CMD_APLAN_DESCER  = 0x8000;
static const int FLAG_CMD_DESBOB_LIGAR  = 0x0100;

static const int FLAG_PROG_PRENSA_SINGELO = 0x0001;
static const int FLAG_PROG_PRENSA_LIGAR   = 0x0002;
static const int FLAG_PROG_REPET_PROXIMO  = 0x0004;
static const int FLAG_PROG_REPET_ANTERIOR = 0x0008;
static const int FLAG_PROG_PASSO_PROXIMO  = 0x0010;
static const int FLAG_PROG_PASSO_ANTERIOR = 0x0020;
static const int FLAG_PROG_LER_ESTADO     = 0x0040;
static const int FLAG_PROG_LER_DADOS      = 0x0080;
static const int FLAG_PROG_FINALIZAR      = 0x0100;

static const int REG_PROG_INDICE     = 23;
static const int REG_PROG_AVANCO     = 24;
static const int REG_PROG_PORTAS     = 25;
static const int REG_PROG_REPETICOES = 18;
static const int REG_PROG_FLAGS      = 31;

static const int PROG_MASK_PRENSA    = 0x0001;
static const int PROG_MASK_FACA      = 0x0002;

static const int MAX_PROG_PASSOS   = 10;

void ProgCarregarDados(int forcar)
{
  char *arrayFieldNames[][4] = {
      { "lblExecProgPasso", "lblExecProgAvanco", "lblExecProgRepeticao", "lblExecProgPortas" },
  };

  static int ultProgPassoAtual = -1, ultProgRepetAtual = -1;
  int ProgPassoAtual, ProgRepetAtual;

  // Carrega os indices do passo atual
  MaqGravarRegistrador(REG_PROG_FLAGS, MaqLerRegistrador(REG_PROG_FLAGS, 0) | FLAG_PROG_LER_ESTADO);
  while(MaqLerRegistrador(REG_PROG_FLAGS, 0) & FLAG_PROG_LER_ESTADO);

  ProgPassoAtual = MaqLerRegistrador(REG_PROG_INDICE    , 0);
  ProgRepetAtual = MaqLerRegistrador(REG_PROG_REPETICOES, 0);

  if(forcar || ultProgPassoAtual != ProgPassoAtual || ultProgRepetAtual != ProgRepetAtual) {
    char tmp[100];
    char **FieldNames = arrayFieldNames[0];

    // Carrega os dados do passo atual
    MaqGravarRegistrador(REG_PROG_FLAGS, MaqLerRegistrador(REG_PROG_FLAGS, 0) | FLAG_PROG_LER_DADOS);
    while(MaqLerRegistrador(REG_PROG_FLAGS, 0) & FLAG_PROG_LER_DADOS);

    int ProgQtdPassos = MaqLerRegistrador(REG_PROG_INDICE    , 0);
    int ProgQtdRepet  = MaqLerRegistrador(REG_PROG_REPETICOES, 0);
    int ProgAvanco    = MaqLerRegistrador(REG_PROG_AVANCO    , 0);
    int ProgPortas    = MaqLerRegistrador(REG_PROG_PORTAS    , 0);

    sprintf(tmp, "%d de %d", ProgPassoAtual + 1, ProgQtdPassos);
    GravarValorWidget(FieldNames[0], tmp);

    sprintf(tmp, "%d", ProgAvanco / 100);
    GravarValorWidget(FieldNames[1], tmp);

    sprintf(tmp, "%d de %d", ProgRepetAtual + 1, ProgQtdRepet);
    GravarValorWidget(FieldNames[2], tmp);

    sprintf(tmp, "%c %c %c", (ProgPortas & 0x01) ? 'X' : '-',
        (ProgPortas & 0x02) ? 'X' : '-', (ProgPortas & 0x04) ? 'X' : '-');
    GravarValorWidget(FieldNames[3], tmp);

    ultProgPassoAtual = ProgPassoAtual;
    ultProgRepetAtual = ProgRepetAtual;
  }
}

void AbrirProgParam(void);

void ProgPrensa_Carregar(unsigned int numProg)
{
  int i;
  char sql[300];

  sprintf(sql, "select ProgPrensaAvanco, ProgPrensaPortas, Repeticoes, LigarPrensa from ProgPrensaPassos where ProgPrensaID='%d' order by ID", numProg);
  DB_Execute(&mainDB, 0, sql);

  // Grava um tamanho incorreto pois ainda precisamos contar o numero de itens!
  MaqGravarRegistrador(REG_PROG_INDICE, MAX_PROG_PASSOS);
  MaqConfigFlags(MaqLerFlags() | FLAG_PROG_TAMANHO);
  while(MaqLerFlags() & FLAG_PROG_TAMANHO);

  for(i = 0; DB_GetNextRow(&mainDB, 0) > 0; i++) {
    // Espera terminar escrita anterior
    while(MaqLerFlags() & FLAG_PROG_GRAVAR);

    // Grava a nova posicao
    MaqGravarRegistrador(REG_PROG_INDICE, i);
    MaqGravarRegistrador(REG_PROG_AVANCO    ,  atoi(DB_GetData(&mainDB, 0, 0)));
    MaqGravarRegistrador(REG_PROG_PORTAS    ,  atoi(DB_GetData(&mainDB, 0, 1)));
    MaqGravarRegistrador(REG_PROG_REPETICOES,  atoi(DB_GetData(&mainDB, 0, 2)));
    MaqGravarRegistrador(REG_PROG_FLAGS     , (atoi(DB_GetData(&mainDB, 0, 3)) > 0) ? FLAG_PROG_PRENSA_LIGAR : 0);
    MaqConfigFlags(MaqLerFlags() | FLAG_PROG_GRAVAR);
  }

  MaqGravarRegistrador(REG_PROG_INDICE, i);
  MaqConfigFlags(MaqLerFlags() | FLAG_PROG_TAMANHO);
  while(MaqLerFlags() & FLAG_PROG_TAMANHO);

  sprintf(sql, "select ModoSingelo, NumCiclos from ProgPrensa where ID=%d", numProg);
  DB_Execute(&mainDB, 0, sql);
  DB_GetNextRow(&mainDB, 0);

  MaqGravarRegistrador(REG_PROG_FLAGS, (atoi(DB_GetData(&mainDB, 0, 0)) > 0) ? FLAG_PROG_PRENSA_SINGELO : 0);
  MaqConfigProdQtd(atoi(DB_GetData(&mainDB, 0, 1)));
}

int ProgPrensa_Init(void)
{
  GtkWidget *obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbPrensaProg"));

// Carregamento dos programas cadastrados no MySQL no ComboBox.
  DB_Execute(&mainDB, 0, "select Nome from ProgPrensa order by ID");
  CarregaCombo(&mainDB, GTK_COMBO_BOX(obj),0, NULL);

  return 1;
}

// Logica da tela principal: Callback de botoes e demais funcoes

void cbProgPrensa_MesaSubir(GtkButton *button, gpointer user_data)
{
  atividade++;
  SetMaqStatus(MAQ_STATUS_MANUAL);
  MaqConfigFlags(MaqLerFlags() | FLAG_CMD_MESA_SUBIR);
}

void cbProgPrensa_MesaDescer(GtkButton *button, gpointer user_data)
{
  atividade++;
  SetMaqStatus(MAQ_STATUS_MANUAL);
  MaqConfigFlags(MaqLerFlags() | FLAG_CMD_MESA_DESCER);
}

void cbProgPrensa_MesaParar(GtkButton *button, gpointer user_data)
{
  MaqConfigFlags(MaqLerFlags() & (~(FLAG_CMD_MESA_DESCER | FLAG_CMD_MESA_SUBIR)));
}

void cbProgPrensa_AplanSubir(GtkButton *button, gpointer user_data)
{
  atividade++;
  SetMaqStatus(MAQ_STATUS_MANUAL);
  MaqConfigFlags(MaqLerFlags() | FLAG_CMD_APLAN_SUBIR);
}

void cbProgPrensa_AplanDescer(GtkButton *button, gpointer user_data)
{
  atividade++;
  SetMaqStatus(MAQ_STATUS_MANUAL);
  MaqConfigFlags(MaqLerFlags() | FLAG_CMD_APLAN_DESCER);
}

void cbProgPrensa_AplanParar(GtkButton *button, gpointer user_data)
{
  MaqConfigFlags(MaqLerFlags() & (~(FLAG_CMD_APLAN_DESCER | FLAG_CMD_APLAN_SUBIR)));
}

void cbProgPrensa_Parar(GtkButton *button, gpointer user_data)
{
  atividade++;
  printf("Saindo do modo automatico\n");
  MaqConfigModo(MAQ_MODO_MANUAL);

  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecProgMsg")), "Terminando...");
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
}

void cbProgPrensa_Finalizar(GtkButton *button, gpointer user_data)
{
  atividade++;

  MaqGravarRegistrador(REG_PROG_FLAGS, MaqLerRegistrador(REG_PROG_FLAGS, 0) | FLAG_PROG_FINALIZAR);

  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecProgMsg")), "Terminando...");
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
}

void cbProgPrensa_Produzir(GtkButton *button, gpointer user_data)
{
  if(!GetUserPerm(PERM_ACESSO_OPER))
    {
    MessageBox("Sem permissão para operar a máquina!");
    return;
    }

  AbrirProgParam();
}

void cbDesbob(GtkToggleButton *togglebutton, gpointer user_data)
{
  if(gtk_toggle_button_get_active(togglebutton)) {
      MaqConfigFlags(MaqLerFlags() |   FLAG_CMD_DESBOB_LIGAR );
    } else {
      MaqConfigFlags(MaqLerFlags() & (~FLAG_CMD_DESBOB_LIGAR));
    }
}

void cbExecProgPassoProx(GtkButton *button, gpointer user_data)
{
  uint16_t flags = MaqLerRegistrador(REG_PROG_FLAGS, 0);
  MaqGravarRegistrador(REG_PROG_FLAGS, flags | FLAG_PROG_PASSO_PROXIMO);
}

void cbExecProgPassoAnterior(GtkButton *button, gpointer user_data)
{
  uint16_t flags = MaqLerRegistrador(REG_PROG_FLAGS, 0);
  MaqGravarRegistrador(REG_PROG_FLAGS, flags | FLAG_PROG_PASSO_ANTERIOR);
}

void cbExecProgRepetProx(GtkButton *button, gpointer user_data)
{
  uint16_t flags = MaqLerRegistrador(REG_PROG_FLAGS, 0);
  MaqGravarRegistrador(REG_PROG_FLAGS, flags | FLAG_PROG_REPET_PROXIMO);
}

void cbExecProgRepetAnterior(GtkButton *button, gpointer user_data)
{
  uint16_t flags = MaqLerRegistrador(REG_PROG_FLAGS, 0);
  MaqGravarRegistrador(REG_PROG_FLAGS, flags | FLAG_PROG_REPET_ANTERIOR);
}

/*** Logica para montar a janela de parametros do programa ***/

// Estrutura para implementar a lista ligada com os itens
typedef struct strObjList {
  int id;
  GtkWidget *obj;

  struct strObjList *next;
} tObjList;

// Estrutura passada como parametro para as funcoes de callback
// Contem os objetos e respectivos IDs para atualizacao do banco de dados
typedef struct {
  int id;
  GtkWidget *qtd;

  tObjList *lista_avanco;
  tObjList *lista_repeticoes;
} tProgParam;

// Timer de producao
gboolean tmrExecProg(gpointer data)
{
  static int qtd = 0, iniciando = 1, progID = 0;
  char tmp[30], sql[300];
  int qtdprod;
  static GtkLabel *lblSaldo = NULL;

  if(iniciando) {
    iniciando = 0;
    ProgCarregarDados(TRUE);
    progID = *(int *)(data);

    sprintf(sql, "select NumCiclos, Nome from ProgPrensa where ID=%d", progID);
    DB_Execute(&mainDB, 0, sql);
    DB_GetNextRow(&mainDB, 0);

    qtd = atoi(DB_GetData(&mainDB, 0, 0));

    lblSaldo = GTK_LABEL(gtk_builder_get_object(builder, "lblPrensaExecSaldo"));

    if(qtd > 0) {
      sprintf(tmp, "%d", qtd);
    } else {
      strcpy(tmp, "Sem Limite");
      gtk_label_set_text(lblSaldo, tmp);
    }

    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblPrensaExecTotal")), tmp);

    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecProgMsg")), DB_GetData(&mainDB, 0, 1));
  }

  qtdprod = MaqLerProdQtd(); // Retorna quantidade de pecas restantes.

  if((MaqLerEstado() & MAQ_MODO_MASK) == MAQ_MODO_MANUAL) {
    iniciando = 1;
    MaqConfigModo(MAQ_MODO_MANUAL);

    // Configura o estado da máquina
    SetMaqStatus(MAQ_STATUS_MANUAL);

    if(progID > 0 && qtdprod >= 0) {
      // Executa a instrução SQL necessária para atualizar a quantidade restante.
      sprintf(sql, "update ProgPrensa set NumCiclos=%d where ID=%d", qtdprod, progID);
      DB_Execute(&mainDB, 0, sql);
    }

    qtd    = 0;
    progID = 0;

    WorkAreaGoTo(MaqConfigCurrent->AbaHome);
    return FALSE;
  } else {
    if(qtd > 0) {
      sprintf(tmp, "%d", qtdprod);
      gtk_label_set_text(lblSaldo, tmp);
    }

    ProgCarregarDados(FALSE);
  }

  return TRUE;
}

void IniciarProducao(int id)
{
  ProgPrensa_Carregar(id);

  WorkAreaGoTo(NTB_ABA_PRENSA_PROD);

  printf("Entrando no modo automatico\n");
  MaqConfigModo(MAQ_MODO_AUTO);

  tmrExecProg((gpointer)&id);
  g_timeout_add(1000, tmrExecProg, NULL);

  // Configura o estado da máquina
  SetMaqStatus(MAQ_STATUS_PRODUZINDO);

  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnPrensaExecParar"  )), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnExecProgFinalizar")), TRUE);
}

void SaindoProgParam(gpointer user_data)
{
  guint signal_id;
  gulong handler_id;
  GtkWidget *wdg;
  GtkContainer *container = GTK_CONTAINER(gtk_builder_get_object(builder, "tblProgParam"));
  GList *start, *lst = gtk_container_get_children(container);

  start = lst;
  while(lst) {
    gtk_container_remove(container, GTK_WIDGET(lst->data));
    lst = lst->next;
  }

  g_list_free(start);

  signal_id = g_signal_lookup("clicked", GTK_TYPE_BUTTON);

  wdg = GTK_WIDGET(gtk_builder_get_object(builder, "btnProgParamProduzir"));
  handler_id = g_signal_handler_find((gpointer)wdg, G_SIGNAL_MATCH_ID, signal_id, 0, NULL, NULL, NULL);
  g_signal_handler_disconnect((gpointer)wdg, handler_id);

  wdg = GTK_WIDGET(gtk_builder_get_object(builder, "btnProgParamCancelar"));
  handler_id = g_signal_handler_find((gpointer)wdg, G_SIGNAL_MATCH_ID, signal_id, 0, NULL, NULL, NULL);
  g_signal_handler_disconnect((gpointer)wdg, handler_id);

  tProgParam *ProgParam = (tProgParam *)(user_data);

  while(ProgParam->lista_avanco != NULL) {
    tObjList *next = ProgParam->lista_avanco->next;
    free(ProgParam->lista_avanco);
    ProgParam->lista_avanco = next;
  }

  while(ProgParam->lista_repeticoes != NULL) {
    tObjList *next = ProgParam->lista_repeticoes->next;
    free(ProgParam->lista_repeticoes);
    ProgParam->lista_repeticoes = next;
  }

  free(ProgParam);
}

void cbProgParamCancelar(GtkButton *button, gpointer user_data)
{
  WorkAreaGoPrevious();
  SaindoProgParam(user_data);
}

void cbProgParamProduzir(GtkButton *button, gpointer user_data)
{
  int val;
  char sql[200];

  tProgParam *ProgParam = (tProgParam *)(user_data);
  tObjList   *item      = ProgParam->lista_avanco;

  while(item != NULL) {
    val = atoi(gtk_entry_get_text(GTK_ENTRY(item->obj))) * 100;
    if(val > 0) {
      sprintf(sql,"update ProgPrensaPassos set ProgPrensaAvanco=%d where ID=%d", val, item->id);
      DB_Execute(&mainDB, 0, sql);
    }

    item = item->next;
  }

  item = ProgParam->lista_repeticoes;

  while(item != NULL) {
    val = atoi(gtk_entry_get_text(GTK_ENTRY(item->obj)));
    if(val > 0) {
      sprintf(sql,"update ProgPrensaPassos set Repeticoes=%d where ID=%d", val, item->id);
      DB_Execute(&mainDB, 0, sql);
    }

    item = item->next;
  }

  if(ProgParam->qtd != NULL) {
    val = atoi(gtk_entry_get_text(GTK_ENTRY(ProgParam->qtd)));
    if(val >= 0) {
      sprintf(sql,"update ProgPrensa set NumCiclos=%d where ID=%d",
          val, ProgParam->id);
      DB_Execute(&mainDB, 0, sql);
    }
  }

  IniciarProducao(ProgParam->id);
}

void cbProgPadraoIniciar(GtkButton *button, gpointer user_data)
{
  int qtd, avanco, passo, tamanho, usar_faca, modo_singelo = 0, porta_prensa = 0;

  avanco      = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entProgPadraoAvanco" )))) * 10;
  passo       = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entProgPadraoPasso"  )))) * 10;
  tamanho     = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entProgPadraoTamanho")))) * 10;
  qtd         = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entProgPadraoQtd"    ))));
  usar_faca   = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "rbtProgPadraoComFaca")));

  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "rbtProgPadraoSingelo")))) {
    modo_singelo = 1;
  }

  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "rbtProgPadraoComPrensa")))) {
    porta_prensa = PROG_MASK_PRENSA;
  }

  if       (avanco  <= 0) {
    MessageBox("O avanço deve ser maior que zero!");
  } else if(usar_faca && passo   <= 0) {
    MessageBox("O passo deve ser maior que zero!");
  } else if(usar_faca && tamanho <= 0) {
    MessageBox("O tamanho deve ser maior que zero!");
  } else if(usar_faca && tamanho <  passo) {
    MessageBox("O tamanho deve ser maior que o passo!");
  } else if(usar_faca && (avanco % passo) != 0) {
    MessageBox("O avanço deve ser múltiplo do passo!");
  } else {
    char sql[300];
    int passos = tamanho / avanco, correcao_passo = 0;
    int passo_final = tamanho - (passos * avanco);

    // Primeiro devemos excluir o programa anterior
    DB_Execute(&mainDB, 0, "delete from ProgPrensaPassos where ProgPrensaID=1");

    if(usar_faca) {
      if(passo_final == 0) {
        passos--;
        passo_final = avanco;
      } else if((passo_final % passo) != 0) {
        passos--;
        correcao_passo = (passo_final % passo) / 2;
      }

      if(passos > 0) {
        sprintf(sql, "insert into ProgPrensaPassos (ProgPrensaID, ProgPrensaAvanco, ProgPrensaPortas, Repeticoes) values (1, %d, %d, %d)",
            avanco, porta_prensa, passos);
        DB_Execute(&mainDB, 0, sql);
      }

      if(passo_final == avanco || !(passo_final % passo)) {
        sprintf(sql, "insert into ProgPrensaPassos (ProgPrensaID, ProgPrensaAvanco, ProgPrensaPortas) values (1, %d, %d)",
            passo_final, porta_prensa | PROG_MASK_FACA);
        DB_Execute(&mainDB, 0, sql);
      } else {
        sprintf(sql, "insert into ProgPrensaPassos (ProgPrensaID, ProgPrensaAvanco, ProgPrensaPortas) values (1, %d, %d)",
            passo_final - correcao_passo, PROG_MASK_FACA);
        DB_Execute(&mainDB, 0, sql);

        sprintf(sql, "insert into ProgPrensaPassos (ProgPrensaID, ProgPrensaAvanco, ProgPrensaPortas) values (1, %d, %d)",
            avanco + correcao_passo, porta_prensa);
        DB_Execute(&mainDB, 0, sql);
      }
    } else {
      sprintf(sql, "insert into ProgPrensaPassos (ProgPrensaID, ProgPrensaAvanco, ProgPrensaPortas) values (1, %d, %d)",
          avanco, porta_prensa);
      DB_Execute(&mainDB, 0, sql);
    }

    sprintf(sql, "update ProgPrensa set NumCiclos=%d, ModoSingelo=%d where ID=1", qtd, modo_singelo);
    DB_Execute(&mainDB, 0, sql);

    IniciarProducao(1);
  }
}

void cbProgPadraoContinuar(GtkButton *button, gpointer user_data)
{
  IniciarProducao(1);
}

void cbProgPadraoCancelar(GtkButton *button, gpointer user_data)
{
  WorkAreaGoTo(MaqConfigCurrent->AbaHome);
}

void AbrirProgramaPadrao(void)
{
  char tmp[100];
  int modo_singelo, ciclos, avanco, passo = 0, tamanho = 0, usar_faca = 0, usar_prensa = 0;

  DB_Execute(&mainDB, 0, "select NumCiclos, ModoSingelo from ProgPrensa where ID=1");
  DB_GetNextRow(&mainDB, 0);

  ciclos = atoi(DB_GetData(&mainDB, 0, 0));
  sprintf(tmp, "%d", ciclos);
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entProgPadraoQtd")), tmp);

  modo_singelo = atoi(DB_GetData(&mainDB, 0, 1));

  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entProgPadraoPasso" )), "40");
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entProgPadraoAvanco")), "0");

  DB_Execute(&mainDB, 0, "select ProgPrensaAvanco, ProgPrensaPortas, Repeticoes from ProgPrensaPassos where ProgPrensaID=1");
  while(DB_GetNextRow(&mainDB, 0) > 0) {
    avanco = atoi(DB_GetData(&mainDB, 0, 0)) / 10;
    tamanho += avanco * atoi(DB_GetData(&mainDB, 0, 2));

    if(atoi(DB_GetData(&mainDB, 0, 1)) & PROG_MASK_FACA) {
      usar_faca = 1;
    }

    if(atoi(DB_GetData(&mainDB, 0, 1)) & PROG_MASK_PRENSA) {
      usar_prensa = 1;
    }

    if(passo == 0) {
      // Se for o primeiro passo, carrega o avanco no campo correspondente...
      sprintf(tmp, "%d", avanco);
      gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entProgPadraoAvanco")), tmp);
    }

    passo++;
  }

  sprintf(tmp, "%d", tamanho);
  gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entProgPadraoTamanho")), tmp);

  char *radio;
  if(usar_faca) {
    radio = "rbtProgPadraoComFaca";
  } else {
    radio = "rbtProgPadraoSemFaca";
  }

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, radio)), TRUE);

  if(usar_prensa) {
    radio = "rbtProgPadraoComPrensa";
  } else {
    radio = "rbtProgPadraoSemPrensa";
  }

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, radio)), TRUE);

  if(modo_singelo) {
    radio = "rbtProgPadraoSingelo";
  } else {
    radio = "rbtProgPadraoContinuo";
  }

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, radio)), TRUE);

  WorkAreaGoTo(NTB_ABA_PROG_PADRAO);
}

void AbrirProgParam(void)
{
  char sql[200], tmp[60];
  int id, num_ciclos, pedir_ciclos, tam = 0, i = 0, idx = 0;
  GtkWidget *tbl, *obj;
  GtkComboBox *combo = GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbPrensaProg"));

  sprintf(sql, "select ID, NumCiclos, PedirNumCiclos from ProgPrensa where LINHA='%s' and MAQUINA='%s' and nome='%s'",
      MAQ_LINHA, MAQ_MAQUINA, LerComboAtivo(combo));
  DB_Execute(&mainDB, 0, sql);
  DB_GetNextRow(&mainDB, 0);

  id           = atoi(DB_GetData(&mainDB, 0, 0));
  num_ciclos   = atoi(DB_GetData(&mainDB, 0, 1));
  pedir_ciclos = atoi(DB_GetData(&mainDB, 0, 2));

  if(id == 1) { // Condicao especial: Programa padrao!
    AbrirProgramaPadrao();
  } else {
    if(pedir_ciclos) {
      tam++;
    }

    tbl = GTK_WIDGET(gtk_builder_get_object(builder, "tblProgParam"));

    sprintf(sql, "select ID, TextoAvanco, ProgPrensaAvanco from ProgPrensaPassos where ProgPrensaID=%d and PedirAvanco=1 order by ID", id);
    DB_Execute(&mainDB, 0, sql);
    if(DB_GetNextRow(&mainDB, 0) <= 0) {
      idx++;
    }

    tam += DB_GetCount(&mainDB, 0);

    sprintf(sql, "select ID, TextoRepeticoes, Repeticoes from ProgPrensaPassos where ProgPrensaID=%d and PedirRepeticoes=1 order by ID", id);
    DB_Execute(&mainDB, 1, sql);
    if(DB_GetNextRow(&mainDB, 1) <= 0) {
      idx++;
    }

    tam += DB_GetCount(&mainDB, 1);

    if(tam > 0) {
      // Ponteiro para uma lista de IDs para a futura inserção / alteração dos dados no banco.
      // A logica a seguir aloca o ponteiro para a estrutura, o qual deve ser desalocado pela
      // função de callback do sinal destroy da janela.
      tProgParam *ProgParam = (tProgParam *)malloc(sizeof(tProgParam));
      memset(ProgParam, 0, sizeof(tProgParam));

      ProgParam->id = id;

      gtk_table_resize(GTK_TABLE(tbl), tam, 2);

  // Conexão dos sinais de callback
      obj = GTK_WIDGET(gtk_builder_get_object(builder, "btnProgParamProduzir"));
      g_signal_connect ((gpointer) obj, "clicked",
        G_CALLBACK (cbProgParamProduzir ), (gpointer)(ProgParam));

      obj = GTK_WIDGET(gtk_builder_get_object(builder, "btnProgParamCancelar"));
      g_signal_connect ((gpointer) obj, "clicked",
        G_CALLBACK (cbProgParamCancelar), (gpointer)(ProgParam));

      if(pedir_ciclos) {
        obj = gtk_label_new("Quantidade:");
        gtk_buildable_set_name(GTK_BUILDABLE(obj), "lblProgParamQtd");
        gtk_table_attach_defaults(GTK_TABLE(tbl), obj, 0, 1, 0, 1);

        sprintf(tmp, "%d", num_ciclos);
        obj = gtk_entry_new_with_max_length(10); // Tamanho de um int
        gtk_buildable_set_name(GTK_BUILDABLE(obj), "entProgParamQtd");
        gtk_entry_set_text(GTK_ENTRY(obj), tmp);
        g_signal_connect ((gpointer) obj, "focus-in-event", G_CALLBACK(cbFocusIn), NULL);

        ProgParam->qtd = obj;

        gtk_table_attach_defaults(GTK_TABLE(tbl), obj, 1, 2, 0, 1);

        i++;
      }

      tObjList **ObjList = &(ProgParam->lista_avanco);

      for(; idx < 2; i++) {
        *ObjList = (tObjList *)(malloc(sizeof(tObjList)));
        (*ObjList)->id = atoi(DB_GetData(&mainDB, idx, 0));

        Asc2Utf((unsigned char *)DB_GetData(&mainDB, idx, 1), (unsigned char *)tmp);
        obj = gtk_label_new(tmp);

        sprintf(tmp, "lblProgParam%02d", i);
        gtk_buildable_set_name(GTK_BUILDABLE(obj), tmp);

        gtk_table_attach_defaults(GTK_TABLE(tbl), obj, 0, 1, i, i+1);

        sprintf(tmp, "%d", atoi(DB_GetData(&mainDB, idx, 2)) / (idx == 0 ? 100 : 1));

        obj = gtk_entry_new_with_max_length(10); // Tamanho de um int
        gtk_entry_set_text(GTK_ENTRY(obj), tmp);

        sprintf(tmp, "entProgParam%02d", i);
        gtk_buildable_set_name(GTK_BUILDABLE(obj), tmp);

        g_signal_connect ((gpointer) obj, "focus-in-event", G_CALLBACK(cbFocusIn), NULL);

        (*ObjList)->obj = obj;

        gtk_table_attach_defaults(GTK_TABLE(tbl), obj, 1, 2, i, i+1);

        (*ObjList)->next = NULL;
        ObjList = &(*ObjList)->next;

        if(DB_GetNextRow(&mainDB, idx) <= 0) {
          ObjList = &(ProgParam->lista_repeticoes);
          idx++;
        }
      }

      gtk_widget_show_all(tbl);

      WorkAreaGoTo(NTB_ABA_PROG_PARAM);
    } else {
      IniciarProducao(id);
    }
  }
}

/*** Fim da logica para montar a janela de parametros do programa ***/
