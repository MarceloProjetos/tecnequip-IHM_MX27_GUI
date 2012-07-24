// Para que a funcao atof funcione corretamente
#include <stdio.h>
#include <stdlib.h>

#include <net/modbus.h>

#include <gtk/gtk.h>

#include "defines.h"
#include "maq.h"
#include "GtkUtils.h"

// Estrutura que representa o ModBus
extern struct MB_Device mbdev;

/*** Funcoes e variáveis de suporte ***/

extern int idUser; // Indica usuário que logou se for diferente de zero.
extern int CurrentWorkArea;  // Variavel que armazena a tela atual.
extern int PreviousWorkArea; // Variavel que armazena a tela anterior.

// Função que salva um log no banco contendo usuário e data que gerou o evento.
extern void Log(char *evento, int tipo);

/*** Fim das funcoes e variáveis de suporte ***/

// Timer de producao
gboolean tmrExec(gpointer data)
{
  static int iniciando = 1;
  char tmp[30], txtQtd[10];
  int erro_posic;
  GtkWidget *wdg;
  unsigned int qtd;
  static GtkLabel *lblErroPos = NULL;

  if(iniciando) {
    iniciando = 0;
    lblErroPos = GTK_LABEL(gtk_builder_get_object(builder, "lblExecErroPos"));
  }

  qtd = MaqLerQtd();
  sprintf(txtQtd, "%d", qtd);

  printf("MaqPosAtual = %d\n", MaqLerPosAtual());

  erro_posic = MaqLerAplanErroPosic();

  sprintf(tmp, "%.01f", (float)(erro_posic)/10);
  gtk_label_set_text(lblErroPos, tmp);

  if((MaqLerEstado() & MAQ_MODO_MASK) == MAQ_MODO_MANUAL) {
    MaqConfigModo(MAQ_MODO_MANUAL);

    Log("Encerrando producao", LOG_TIPO_TAREFA);

    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecMsg")), "Parado");

    wdg = GTK_WIDGET(gtk_builder_get_object(builder, "btnExecIniciarParar"));
    gtk_button_set_label(GTK_BUTTON(wdg), "Iniciar");
    gtk_widget_set_sensitive(wdg, TRUE);

    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnExecVoltar")), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "cmbExecModelo")), TRUE);

    if(qtd) {
      sprintf(txtQtd, "%d", qtd);
    } else {
      txtQtd[0] = 0;
    }

    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "entExecQtd")), txtQtd);

    return FALSE;
  }

  sprintf(txtQtd, "%d", *(unsigned int *)(data) - qtd);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "entExecQtd")), txtQtd);

  return TRUE;
}

void AbrirOper()
{
  char sql[100];
  GtkWidget *obj = GTK_WIDGET(gtk_builder_get_object(builder, "cmbExecModelo"));

  if(!GetUserPerm(PERM_ACESSO_OPER))
    {
    WorkAreaGoTo(NTB_ABA_HOME);
    MessageBox("Sem permissão para operar a máquina!");
    return;
    }

// Carregamento dos modelos cadastrados no MySQL no ComboBox.
  sprintf(sql, "select nome from modelos where estado='%d' order by ID", MOD_ESTADO_ATIVO);
  DB_Execute(&mainDB, 0, sql);
  CarregaCombo(GTK_COMBO_BOX(obj), 0, NULL);
}

unsigned int CarregarPrograma(char *modelo)
{
  char sql[100];
  unsigned int id_modelo, i;

  sprintf(sql, "select ID from modelos where nome='%s'", modelo);
  DB_Execute(&mainDB, 0, sql);
  if(DB_GetNextRow(&mainDB, 0)>0) {
    id_modelo = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "nome")));
  } else {
    printf("ERRO!!! Modelo inexistente\n");
    return 0;
  }

  sprintf(sql, "select * from modelos_programas where ID_modelo=%d", id_modelo);
  DB_Execute(&mainDB, 0, sql);
  for(i=0; DB_GetNextRow(&mainDB, 0)>0 && i<MAQ_PASSOS_MAX; i++) {
    maq_param.aplanadora.passos[i].passo      = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "passo")));
    maq_param.aplanadora.passos[i].repeticoes = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "repeticoes")));
    maq_param.aplanadora.passos[i].portas     = atoi(DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, "portas")));
#ifdef DEBUG_PC
    printf("Passo %d:\n", i+1);
    printf("\tpasso.....: %d\n", maq_param.aplanadora.passos[i].passo);
    printf("\trepeticoes: %d\n", maq_param.aplanadora.passos[i].repeticoes);
    printf("\tportas....: %c%c%c%c%c\n",
        maq_param.aplanadora.passos[i].portas & 16 ? 'X' : '.',
        maq_param.aplanadora.passos[i].portas &  8 ? 'X' : '.',
        maq_param.aplanadora.passos[i].portas &  4 ? 'X' : '.',
        maq_param.aplanadora.passos[i].portas &  2 ? 'X' : '.',
        maq_param.aplanadora.passos[i].portas &  1 ? 'X' : '.');
#endif
  }

  return i;
}

void cbExecTarefa(GtkButton *button, gpointer user_data)
{
  unsigned int passos, qtd;
  char *modo_botao[] = { "Iniciar", "Parar" }, msg[40];
  GtkWidget *wdg = GTK_WIDGET(gtk_builder_get_object(builder, "btnExecIniciarParar"));

  if(!strcmp(gtk_button_get_label(GTK_BUTTON(wdg)), modo_botao[0])) {
    if(!(MaqLerEstado() & MAQ_STATUS_PRONTA))
      return;

    passos = CarregarPrograma(LerComboAtivo(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbExecModelo"))));
    qtd    = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entExecQtd"))));

    if(!passos || !qtd)
      return;

    MaqConfigQtd (qtd);
    MaqSyncPassos(passos);

    Log("Iniciando producao", LOG_TIPO_TAREFA);

    MaqConfigModo(MAQ_MODO_AUTO);
    g_timeout_add(1000, tmrExec, (gpointer)&qtd);

    sprintf(msg, "Produzindo com passo de %d mm", 0);//maq_param.aplanadora.passo);
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecMsg")), msg);

    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnExecVoltar")), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "cmbExecModelo")), FALSE);

    gtk_button_set_label(GTK_BUTTON(wdg), modo_botao[1]);
  } else {
    MaqConfigModo(MAQ_MODO_MANUAL);
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecMsg")), "Terminando...");
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    gtk_button_set_label(GTK_BUTTON(wdg), modo_botao[0]);

    gtk_widget_set_sensitive(wdg, FALSE);
  }
}

void cbExecLimparErro(GtkButton *button, gpointer user_data)
{
  gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "lblExecAvisoLub"   )), 0);
  gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "lblExecAvisoFerram")), 0);
}

void cbManAplanAvancar()
{
  MaqAplanManual(MAQ_APLAN_AVANCAR);
}

void cbManAplanRecuar()
{
  MaqAplanManual(MAQ_APLAN_RECUAR);
}

void cbManAplanParar()
{
  MaqAplanManual(MAQ_APLAN_PARAR);
}

struct strCoordIHM {
  unsigned int xpos, ypos, xsize, ysize;
  void (*fnc)();
  char *img;
} CurrentLstCoord[] = {
     { 415,  85, 70, 55, cbManAplanAvancar , "images/cmd-aplan-perfil-avanca.png"},
    { 135,  85, 70, 55, cbManAplanRecuar  , "images/cmd-aplan-perfil-recua.png" },
    { 0, 0, 0, 0, NULL } };

// defines que permitem selecionar o ponto de referencia para insercao da imagem
#define LOADPB_REFPOS_UP      0x00
#define LOADPB_REFPOS_DOWN    0x01
#define LOADPB_REFPOS_LEFT    0x00
#define LOADPB_REFPOS_RIGHT   0x02
#define LOADPB_REFPOS_DEFAULT (LOADPB_REFPOS_UP | LOADPB_REFPOS_LEFT)

void LoadIntoPixbuf(GdkPixbuf *pb, char *file, gint x, gint y, gdouble scale_x, gdouble scale_y, gint refpos)
{
  GdkPixbuf *pbtmp;
  gint width, height;
  static gint last_x = 0, last_y = 0;

  if(pb == NULL) {
    return;
  }

// Verifica se devemos usar a ultima coordenada ou a passada como parametro
  if(x<0)
    x = last_x;
  if(y<0)
    y = last_y;

// Carrega a nova imagem
  pbtmp  = gdk_pixbuf_new_from_file(file, NULL);

// Carrega as dimensoes da nova imagem
  width  = gdk_pixbuf_get_width (pbtmp)*scale_x;
  height = gdk_pixbuf_get_height(pbtmp)*scale_y;

// Recalcula as coordenadas de acordo com o ponto de referencia passado como parametro
  if(refpos & LOADPB_REFPOS_DOWN)
    y = gdk_pixbuf_get_height(pb) - height - y;
  if(refpos & LOADPB_REFPOS_RIGHT)
    x = gdk_pixbuf_get_width (pb) - width  - x;

// Agrega as duas imagens e remove a referencia a nova imagem
  gdk_pixbuf_composite(pbtmp, pb,
      x, y, width, height, x, y, scale_x, scale_y,
      GDK_INTERP_BILINEAR, 255);
  gdk_pixbuf_unref(pbtmp);
//  g_object_unref(pbtmp);

// Atualiza as coordenadas atuais
  last_x = x + width ;
  last_y = y + height;
}

struct strImgIHM {
  char *arq;
  int posx, posy;
  unsigned int scalex, scaley;
};

struct strTelaIHM {
  GdkPixbuf *pb;
  unsigned int offset;
  struct strCoordIHM *coord;
};

void CriarTelaIHM(struct strTelaIHM *tela, unsigned int offset, struct strImgIHM *lst_img, struct strCoordIHM *lst_coord)
{
  char arq[50];
  static unsigned int n = 0;

#ifdef DEBUG_CARREGAR_TELA
  tela->offset = 0;
  tela->coord  = lst_coord;

  sprintf(arq, "images/maq.%d.png", n++);
  tela->pb = gdk_pixbuf_new_from_file(arq, NULL);
#else
  GdkPixbuf *pb, *copy_pb;
  unsigned int i = 0;

  tela->offset = offset;
  tela->coord  = lst_coord;

  tela->pb = gdk_pixbuf_new_from_file_at_scale(lst_img[0].arq,
      lst_img[0].posx + 2*tela->offset, lst_img[0].posy + 2*tela->offset,
      FALSE, NULL);

// Carrega a nova imagem
  pb = gdk_pixbuf_new_from_file_at_scale(lst_img[0].arq,
      lst_img[0].posx, lst_img[0].posy,
      FALSE, NULL);

// Agrega as duas imagens e remove a referencia a nova imagem
  gdk_pixbuf_composite(pb, tela->pb, tela->offset, tela->offset,
      lst_img[0].posx, lst_img[0].posy, tela->offset, tela->offset, 1, 1,
      GDK_INTERP_BILINEAR, 255);
  gdk_pixbuf_unref(pb);

  while(lst_img[++i].arq != NULL) {
    LoadIntoPixbuf(tela->pb, lst_img[i].arq, lst_img[i].posx + tela->offset, lst_img[i].posy + tela->offset, lst_img[i].scalex, lst_img[i].scaley, LOADPB_REFPOS_DEFAULT);
  }

  for(i=0; tela->coord[i].img != NULL; i++) {
    LoadIntoPixbuf(tela->pb, tela->coord[i].img, tela->coord[i].xpos + tela->offset, tela->coord[i].ypos + tela->offset, 1, 1, LOADPB_REFPOS_UP | LOADPB_REFPOS_LEFT);
  }

  // Recorta pixbuf para o tamanho da tela e copia para um novo
  pb = gdk_pixbuf_new_subpixbuf(tela->pb, tela->offset, tela->offset, lst_img[0].posx, lst_img[0].posy);
//  copy_pb = gdk_pixbuf_copy(pb);

  // Remove referências dos pixbufs antigos e aponta a tela para o novo
//  gdk_pixbuf_unref(pb);
//  gdk_pixbuf_unref(tela->pb);
//  tela->pb = copy_pb;

  sprintf(arq, "maq.%d.png", n++);
  gdk_pixbuf_save(pb, arq, "png", NULL, NULL);
#endif
}

void DesenharTelaIHM(GtkWidget *widget, struct strTelaIHM *tela)
{
  gdk_draw_pixbuf(widget->window,
                  widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                  tela->pb,
                  tela->offset, tela->offset,
                  0, 0, widget->allocation.width, widget->allocation.height,
                  GDK_RGB_DITHER_NONE, 0, 0);
}

gboolean cbMaquinaButtonPress(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  unsigned int i;

  if(event->type == GDK_BUTTON_PRESS && CurrentLstCoord != NULL){// && MaqPronta()) {
    for(i=0; CurrentLstCoord[i].fnc != NULL; i++) {
      if(event->x >= CurrentLstCoord[i].xpos &&
         event->y >= CurrentLstCoord[i].ypos &&
         event->x <= CurrentLstCoord[i].xpos + CurrentLstCoord[i].xsize &&
         event->y <= CurrentLstCoord[i].ypos + CurrentLstCoord[i].ysize) {
        (CurrentLstCoord[i].fnc)();
        break;
      }
    }
  } else if (event->type == GDK_BUTTON_RELEASE) {
    MaqAplanManual(MAQ_APLAN_PARAR);
  }

  return TRUE;
}

gboolean cbDesenharMaquina(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  static int first = 1;
  unsigned int offset = 200;
  static struct strTelaIHM maq;

  if(widget == NULL) {
    widget = GTK_WIDGET(gtk_builder_get_object(builder, "dwaMaquina"));
  }

  if(first) {
    first = 0;

    // Imagem 000 - Tampa fechada, extensão abaixada, prolongamento recuado.
    CriarTelaIHM(&maq, offset,
        (struct strImgIHM[]){ { "images/bg01.png", widget->allocation.width, widget->allocation.height, 1, 1 },
                              { "images/maq-aplan-corpo.png"        , 170, 153, 1, 1 },
                              { "images/maq-aplan-prol-baixo.png"   , 148, 209, 1, 1 },
                              { "images/maq-aplan-ext-baixo.png"    , 156, 184, 1, 1 },
                              { "images/maq-aplan-tampa-fechada.png", 205, 136, 1, 1 },
                              { "images/maq-prs-corpo.png"          , 520, -42, 1, 1 },
                              { "images/maq-prs-martelo.png"        , 563, -42, 1, 1 },
                              { "images/maq-prs-cobertura.png"      , 554, -52, 1, 1 },
                              { NULL, 0, 0, 0, 0 } },
                              CurrentLstCoord);
  }

  if(idUser) {
    DesenharTelaIHM(widget, &maq);
  }

  return TRUE;
}
