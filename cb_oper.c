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
  static int ciclos_inicial, iniciando = 1;
  char tmp[30];
  int erro_posic;
  GtkWidget *wdg;
  static GtkLabel *lblSaldo = NULL, *lblProd = NULL, *lblCiclos = NULL, *lblErroPos = NULL;

  if(iniciando) {
    iniciando = 0;
    ciclos_inicial = maq_param.prensa.ciclos;
    lblProd    = GTK_LABEL(gtk_builder_get_object(builder, "lblExecProd" ));
    lblSaldo   = GTK_LABEL(gtk_builder_get_object(builder, "lblExecCiclos"));
    lblCiclos  = GTK_LABEL(gtk_builder_get_object(builder, "lblExecCiclosTotal"));
    lblErroPos = GTK_LABEL(gtk_builder_get_object(builder, "lblExecErroPos"));
  }

  printf("MaqPosAtual = %d\n", MaqLerPosAtual());

  erro_posic = MaqLerAplanErroPosic();

  sprintf(tmp, "%d", maq_param.prensa.ciclos - ciclos_inicial);
  gtk_label_set_text(lblSaldo, tmp);

  sprintf(tmp, "%d", maq_param.prensa.ciclos);
  gtk_label_set_text(lblCiclos, tmp);

  sprintf(tmp, "%.02f",
      (float)((maq_param.prensa.ciclos - ciclos_inicial) * maq_param.aplanadora.passo) / 1000);
  gtk_label_set_text(lblProd, tmp);

  sprintf(tmp, "%.01f", (float)(erro_posic)/10);
  gtk_label_set_text(lblErroPos, tmp);

  if((MaqLerEstado() & MAQ_MODO_MASK) == MAQ_MODO_MANUAL) {
    iniciando = 1;
    MaqConfigModo(MAQ_MODO_MANUAL);

    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecMsg")), "Parado");

    wdg = GTK_WIDGET(gtk_builder_get_object(builder, "btnExecIniciarParar"));
    gtk_button_set_label(GTK_BUTTON(wdg), "Iniciar");
    gtk_widget_set_sensitive(wdg, TRUE);

    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnExecVoltar")), TRUE);

    return FALSE;
  }

  return TRUE;
}

void AbrirOper()
{
  if(!GetUserPerm(PERM_ACESSO_OPER))
    {
    WorkAreaGoTo(NTB_ABA_HOME);
    MessageBox("Sem permissão para operar a máquina!");
    return;
    }

  // Configura visibilidade de avisos da prensa
  gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "lblExecAvisoLub"   )), 0);
  gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "lblExecAvisoFerram")), 0);

  // Se em reversão, avisa o operador
  if(MaqLerFlags() & MAQ_MODO_PRS_SENTIDO) {
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecMsg")), "Parado - Em Reversão");
  } else {
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecMsg")), "Parado");
  }
}

void cbExecTarefa(GtkButton *button, gpointer user_data)
{
  char *modo_botao[] = { "Iniciar", "Parar" }, msg[40];
  GtkWidget *wdg = GTK_WIDGET(gtk_builder_get_object(builder, "btnExecIniciarParar"));

  if(!strcmp(gtk_button_get_label(GTK_BUTTON(wdg)), modo_botao[0])) {
    if(!(MaqLerEstado() & MAQ_STATUS_PRONTA))
      return;

    Log("Iniciando producao", LOG_TIPO_TAREFA);

    MaqConfigModo(MAQ_MODO_AUTO);
    g_timeout_add(1000, tmrExec, NULL);

    sprintf(msg, "Produzindo com passo de %d mm", maq_param.aplanadora.passo);
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecMsg")), msg);

    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnExecVoltar")), FALSE);

    gtk_button_set_label(GTK_BUTTON(wdg), modo_botao[1]);
  } else {
    Log("Encerrando producao", LOG_TIPO_TAREFA);

    MaqConfigModo(MAQ_MODO_MANUAL);
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecMsg")), "Terminando...");
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    gtk_button_set_label(GTK_BUTTON(wdg), modo_botao[0]);

    gtk_widget_set_sensitive(wdg, FALSE);
  }
}

void cbPrsZerarCiclos(GtkButton *button, gpointer user_data)
{
  maq_param.prensa.ciclos = 0;
  MaqConfigPrsCiclos(0);

  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblConfigPrsCiclos")), "0");
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

void cbManAplanAbrir()
{
  MaqAplanManual(MAQ_APLAN_ABRIR);
}

void cbManAplanFechar()
{
  MaqAplanManual(MAQ_APLAN_FECHAR);
}

void cbManAplanSubir()
{
  MaqAplanManual(MAQ_APLAN_EXT_SUBIR);
}

void cbManAplanDescer()
{
  MaqAplanManual(MAQ_APLAN_EXT_DESCER);
}

void cbManAplanExpandir()
{
  MaqAplanManual(MAQ_APLAN_EXT_EXPANDIR);
}

void cbManAplanRetrair()
{
  MaqAplanManual(MAQ_APLAN_EXT_RETRAIR);
}

void cbManAplanParar()
{
  MaqAplanManual(MAQ_APLAN_PARAR);
}

void cbManPrsLigar()
{
  if(MaqLerEstado() & MAQ_STATUS_PRS_LIGADA)
    MaqPrsManual(MAQ_PRS_DESLIGAR);
  else
    MaqPrsManual(MAQ_PRS_LIGAR);
}

struct strCoordIHM {
  unsigned int xpos, ypos, xsize, ysize;
  void (*fnc)();
  char *img;
} *CurrentLstCoord, lst_coord[2][2][2][10] = {
    // Posição 000 - Tampa fechada, extensão abaixada, prolongamento recuado.
    [0][0][0] = {
    { 415,  85, 70, 55, cbManAplanAvancar , "images/cmd-aplan-perfil-avanca.png"},
    { 135,  85, 70, 55, cbManAplanRecuar  , "images/cmd-aplan-perfil-recua.png" },
    { 235,  70, 70, 55, cbManAplanAbrir   , "images/cmd-aplan-tampa-abrir.png"  },
    { 110, 165, 70, 55, cbManAplanSubir   , "images/cmd-aplan-ext-subir.png"    },
    {  45, 245, 70, 55, cbManAplanExpandir, "images/cmd-aplan-ext-expandir.png" },
    { 579,   0, 80, 80, cbManPrsLigar     , "images/cmd-prensa-ligar.png"       },
    { 0, 0, 0, 0, NULL } },

    // Posição 001 - Tampa fechada, extensão abaixada, prolongamento avançado.
    [0][0][1] = {
    { 415,  85, 70, 55, cbManAplanAvancar , "images/cmd-aplan-perfil-avanca.png"},
    { 135,  85, 70, 55, cbManAplanRecuar  , "images/cmd-aplan-perfil-recua.png" },
    { 235,  70, 70, 55, cbManAplanAbrir   , "images/cmd-aplan-tampa-abrir.png"  },
    { 110, 165, 70, 55, cbManAplanSubir   , "images/cmd-aplan-ext-subir.png"    },
    {  45, 245, 70, 55, cbManAplanRetrair , "images/cmd-aplan-ext-expandir.png" },
//    { 579, -30, 80, 80, cbManPrsLigar     , "images/cmd-prensa-ligar.png"       },
    { 0, 0, 0, 0, NULL } },

    // Posição 010 - Tampa fechada, extensão levantada, prolongamento recuado.
    [0][1][0] = {
    { 415,  85, 70, 55, cbManAplanAvancar , "images/cmd-aplan-perfil-avanca.png"},
    { 135,  85, 70, 55, cbManAplanRecuar  , "images/cmd-aplan-perfil-recua.png" },
    { 235,  70, 70, 55, cbManAplanAbrir   , "images/cmd-aplan-tampa-abrir.png"  },
    { 110, 215, 70, 55, cbManAplanDescer  , "images/cmd-aplan-ext-descer.png"   },
    {   0, 175, 70, 55, cbManAplanExpandir, "images/cmd-aplan-ext-expandir.png" },
//    { 579, -30, 80, 80, cbManPrsLigar     , "images/cmd-prensa-ligar.png"       },
    { 0, 0, 0, 0, NULL } },

    // Posição 011 - Tampa fechada, extensão levantada, prolongamento avançado.
    [0][1][1] = {
    { 415,  85, 70, 55, cbManAplanAvancar , "images/cmd-aplan-perfil-avanca.png"},
    { 135,  85, 70, 55, cbManAplanRecuar  , "images/cmd-aplan-perfil-recua.png" },
    { 235,  70, 70, 55, cbManAplanAbrir   , "images/cmd-aplan-tampa-abrir.png"  },
    { 110, 215, 70, 55, cbManAplanDescer  , "images/cmd-aplan-ext-descer.png"   },
    {   0, 175, 70, 55, cbManAplanRetrair , "images/cmd-aplan-ext-expandir.png" },
//    { 579, -30, 80, 80, cbManPrsLigar     , "images/cmd-prensa-ligar.png"       },
    { 0, 0, 0, 0, NULL } },

    // Posição 100 - Tampa aberta, extensão abaixada, prolongamento recuado.
    [1][0][0] = {
    { 415,  85, 70, 55, cbManAplanAvancar , "images/cmd-aplan-perfil-avanca.png"},
    { 135,  85, 70, 55, cbManAplanRecuar  , "images/cmd-aplan-perfil-recua.png" },
    { 235,  40, 70, 55, cbManAplanAbrir   , "images/cmd-aplan-tampa-abrir.png"  },
    { 315,  40, 70, 55, cbManAplanFechar  , "images/cmd-aplan-tampa-abrir.png"  },
    { 110, 165, 70, 55, cbManAplanSubir   , "images/cmd-aplan-ext-subir.png"    },
    {  45, 245, 70, 55, cbManAplanExpandir, "images/cmd-aplan-ext-expandir.png" },
//    { 579, -30, 80, 80, cbManPrsLigar     , "images/cmd-prensa-ligar.png"       },
    { 0, 0, 0, 0, NULL } },

    // Posição 101 - Tampa aberta, extensão abaixada, prolongamento avançado.
    [1][0][1] = {
    { 415,  85, 70, 55, cbManAplanAvancar , "images/cmd-aplan-perfil-avanca.png"},
    { 135,  85, 70, 55, cbManAplanRecuar  , "images/cmd-aplan-perfil-recua.png" },
    { 235,  40, 70, 55, cbManAplanAbrir   , "images/cmd-aplan-tampa-abrir.png"  },
    { 315,  40, 70, 55, cbManAplanFechar  , "images/cmd-aplan-tampa-abrir.png"  },
    { 110, 165, 70, 55, cbManAplanSubir   , "images/cmd-aplan-ext-subir.png"    },
    {  45, 245, 70, 55, cbManAplanRetrair , "images/cmd-aplan-ext-expandir.png" },
//    { 579, -30, 80, 80, cbManPrsLigar     , "images/cmd-prensa-ligar.png"       },
    { 0, 0, 0, 0, NULL } },

    // Posição 110 - Tampa aberta, extensão levantada, prolongamento recuado.
    [1][1][0] = {
    { 415,  85, 70, 55, cbManAplanAvancar , "images/cmd-aplan-perfil-avanca.png"},
    { 135,  85, 70, 55, cbManAplanRecuar  , "images/cmd-aplan-perfil-recua.png" },
    { 235,  40, 70, 55, cbManAplanAbrir   , "images/cmd-aplan-tampa-abrir.png"  },
    { 315,  40, 70, 55, cbManAplanFechar  , "images/cmd-aplan-tampa-abrir.png"  },
    { 110, 215, 70, 55, cbManAplanDescer  , "images/cmd-aplan-ext-descer.png"   },
    {   0, 175, 70, 55, cbManAplanExpandir, "images/cmd-aplan-ext-expandir.png" },
//    { 579, -30, 80, 80, cbManPrsLigar     , "images/cmd-prensa-ligar.png"       },
    { 0, 0, 0, 0, NULL } },

    // Posição 111 - Tampa aberta, extensão levantada, prolongamento avançado.
    [1][1][1] = {
    { 415,  85, 70, 55, cbManAplanAvancar , "images/cmd-aplan-perfil-avanca.png"},
    { 135,  85, 70, 55, cbManAplanRecuar  , "images/cmd-aplan-perfil-recua.png" },
    { 235,  40, 70, 55, cbManAplanAbrir   , "images/cmd-aplan-tampa-abrir.png"  },
    { 315,  40, 70, 55, cbManAplanFechar  , "images/cmd-aplan-tampa-abrir.png"  },
    { 110, 215, 70, 55, cbManAplanDescer  , "images/cmd-aplan-ext-descer.png"   },
    {   0, 175, 70, 55, cbManAplanRetrair , "images/cmd-aplan-ext-expandir.png" },
    //{ 579, -30, 80, 80, cbManPrsLigar     , "images/cmd-prensa-ligar.png"       },
    { 0, 0, 0, 0, NULL } },
};

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

  tela->offset = 0;
  tela->coord  = lst_coord;

  sprintf(arq, "images/maq.%d.png", n++);
  tela->pb = gdk_pixbuf_new_from_file(arq, NULL);

/*  GdkPixbuf *pb, *copy_pb;
  unsigned int i = 0;

  tela->offset = offset;
  tela->coord  = lst_coord;

  tela->pb = gdk_pixbuf_new_from_file_at_scale(lst_img[0].arq,
      lst_img[0].posx + 2*tela->offset, lst_img[0].posy + 2*tela->offset,
      FALSE, NULL);

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
  gdk_pixbuf_save(pb, arq, "png", NULL, NULL);*/
}

void DesenharTelaIHM(GtkWidget *widget, struct strTelaIHM *tela)
{
  CurrentLstCoord = tela->coord;

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
  unsigned int tampa, ext, prol, offset = 200;
  static int first = 1, flags = 0;
  static struct strTelaIHM maq[2][2][2]; // 3 movimentos, 2 estados para cada um

  if(widget == NULL) {
    widget = GTK_WIDGET(gtk_builder_get_object(builder, "dwaMaquina"));
  }

  if(first) {
    first = 0;
    CurrentLstCoord = NULL;

    // Imagem 000 - Tampa fechada, extensão abaixada, prolongamento recuado.
    CriarTelaIHM(&maq[0][0][0], offset,
        (struct strImgIHM[]){ { "images/bg01.png", widget->allocation.width, widget->allocation.height, 1, 1 },
                              { "images/maq-aplan-corpo.png"        , 170, 153, 1, 1 },
                              { "images/maq-aplan-prol-baixo.png"   , 148, 209, 1, 1 },
                              { "images/maq-aplan-ext-baixo.png"    , 156, 184, 1, 1 },
                              { "images/maq-aplan-tampa-fechada.png", 205, 136, 1, 1 },
                              { "images/maq-prs-corpo.png"          , 520, -42, 1, 1 },
                              { "images/maq-prs-martelo.png"        , 563, -42, 1, 1 },
                              { "images/maq-prs-cobertura.png"      , 554, -52, 1, 1 },
                              { NULL, 0, 0, 0, 0 } },
        lst_coord[0][0][0]
        );

    // Imagem 001 - Tampa fechada, extensão abaixada, prolongamento avançado.
    CriarTelaIHM(&maq[0][0][1], offset,
        (struct strImgIHM[]){ { "images/bg01.png", widget->allocation.width, widget->allocation.height, 1, 1 },
                              { "images/maq-aplan-corpo.png"        , 170, 153, 1, 1 },
                              { "images/maq-aplan-prol-baixo.png"   , 108, 249, 1, 1 },
                              { "images/maq-aplan-ext-baixo.png"    , 156, 184, 1, 1 },
                              { "images/maq-aplan-tampa-fechada.png", 205, 136, 1, 1 },
                              { "images/maq-prs-corpo.png"          , 520, -42, 1, 1 },
                              { "images/maq-prs-martelo.png"        , 563, -42, 1, 1 },
                              { "images/maq-prs-cobertura.png"      , 554, -52, 1, 1 },
                              { NULL, 0, 0, 0, 0 } },
        lst_coord[0][0][1]
        );

    // Imagem 010 - Tampa fechada, extensão levantada, prolongamento recuado.
    CriarTelaIHM(&maq[0][1][0], offset,
        (struct strImgIHM[]){ { "images/bg01.png", widget->allocation.width, widget->allocation.height, 1, 1 },
                              { "images/maq-aplan-corpo.png"        , 170, 153, 1, 1 },
                              { "images/maq-aplan-prol-cima.png"    , 113, 196, 1, 1 },
                              { "images/maq-aplan-ext-cima.png"     , 129, 186, 1, 1 },
                              { "images/maq-aplan-tampa-fechada.png", 205, 136, 1, 1 },
                              { "images/maq-prs-corpo.png"          , 520, -42, 1, 1 },
                              { "images/maq-prs-martelo.png"        , 563, -42, 1, 1 },
                              { "images/maq-prs-cobertura.png"      , 554, -52, 1, 1 },
                              { NULL, 0, 0, 0, 0 } },
        lst_coord[0][1][0]
        );

    // Imagem 011 - Tampa fechada, extensão levantada, prolongamento avançado.
    CriarTelaIHM(&maq[0][1][1], offset,
        (struct strImgIHM[]){ { "images/bg01.png", widget->allocation.width, widget->allocation.height, 1, 1 },
                              { "images/maq-aplan-corpo.png"        , 170, 153, 1, 1 },
                              { "images/maq-aplan-prol-cima.png"    ,  73, 196, 1, 1 },
                              { "images/maq-aplan-ext-cima.png"     , 129, 186, 1, 1 },
                              { "images/maq-aplan-tampa-fechada.png", 205, 136, 1, 1 },
                              { "images/maq-prs-corpo.png"          , 520, -42, 1, 1 },
                              { "images/maq-prs-martelo.png"        , 563, -42, 1, 1 },
                              { "images/maq-prs-cobertura.png"      , 554, -52, 1, 1 },
                              { NULL, 0, 0, 0, 0 } },
        lst_coord[0][1][1]
        );

    // Imagem 100 - Tampa aberta, extensão abaixada, prolongamento recuado.
    CriarTelaIHM(&maq[1][0][0], offset,
        (struct strImgIHM[]){ { "images/bg01.png", widget->allocation.width, widget->allocation.height, 1, 1 },
                              { "images/maq-aplan-corpo.png"        , 170, 153, 1, 1 },
                              { "images/maq-aplan-prol-baixo.png"   , 148, 209, 1, 1 },
                              { "images/maq-aplan-ext-baixo.png"    , 156, 184, 1, 1 },
                              { "images/maq-aplan-tampa-aberta.png" , 205, 106, 1, 1 },
                              { "images/maq-prs-corpo.png"          , 520, -42, 1, 1 },
                              { "images/maq-prs-martelo.png"        , 563, -42, 1, 1 },
                              { "images/maq-prs-cobertura.png"      , 554, -52, 1, 1 },
                              { NULL, 0, 0, 0, 0 } },
        lst_coord[1][0][0]
        );

    // Imagem 101 - Tampa aberta, extensão abaixada, prolongamento avançado.
    CriarTelaIHM(&maq[1][0][1], offset,
        (struct strImgIHM[]){ { "images/bg01.png", widget->allocation.width, widget->allocation.height, 1, 1 },
                              { "images/maq-aplan-corpo.png"        , 170, 153, 1, 1 },
                              { "images/maq-aplan-prol-baixo.png"   , 108, 249, 1, 1 },
                              { "images/maq-aplan-ext-baixo.png"    , 156, 184, 1, 1 },
                              { "images/maq-aplan-tampa-aberta.png" , 205, 106, 1, 1 },
                              { "images/maq-prs-corpo.png"          , 520, -42, 1, 1 },
                              { "images/maq-prs-martelo.png"        , 563, -42, 1, 1 },
                              { "images/maq-prs-cobertura.png"      , 554, -52, 1, 1 },
                              { NULL, 0, 0, 0, 0 } },
        lst_coord[1][0][1]
        );

    // Imagem 110 - Tampa aberta, extensão levantada, prolongamento recuado.
    CriarTelaIHM(&maq[1][1][0], offset,
        (struct strImgIHM[]){ { "images/bg01.png", widget->allocation.width, widget->allocation.height, 1, 1 },
                              { "images/maq-aplan-corpo.png"        , 170, 153, 1, 1 },
                              { "images/maq-aplan-prol-cima.png"    , 113, 196, 1, 1 },
                              { "images/maq-aplan-ext-cima.png"     , 129, 186, 1, 1 },
                              { "images/maq-aplan-tampa-aberta.png" , 205, 106, 1, 1 },
                              { "images/maq-prs-corpo.png"          , 520, -42, 1, 1 },
                              { "images/maq-prs-martelo.png"        , 563, -42, 1, 1 },
                              { "images/maq-prs-cobertura.png"      , 554, -52, 1, 1 },
                              { NULL, 0, 0, 0, 0 } },
        lst_coord[1][1][0]
        );

    // Imagem 111 - Tampa aberta, extensão levantada, prolongamento avançado.
    CriarTelaIHM(&maq[1][1][1], offset,
        (struct strImgIHM[]){ { "images/bg01.png", widget->allocation.width, widget->allocation.height, 1, 1 },
                              { "images/maq-aplan-corpo.png"        , 170, 153, 1, 1 },
                              { "images/maq-aplan-prol-cima.png"    ,  73, 196, 1, 1 },
                              { "images/maq-aplan-ext-cima.png"     , 129, 186, 1, 1 },
                              { "images/maq-aplan-tampa-aberta.png" , 205, 106, 1, 1 },
                              { "images/maq-prs-corpo.png"          , 520, -42, 1, 1 },
                              { "images/maq-prs-martelo.png"        , 563, -42, 1, 1 },
                              { "images/maq-prs-cobertura.png"      , 554, -52, 1, 1 },
                              { NULL, 0, 0, 0, 0 } },
        lst_coord[1][1][1]
        );
  }

  if(data != NULL)
    flags = *(unsigned int *)(data);

  if(idUser) {
    tampa = (flags >> 2) & 1;
    ext   = (flags >> 1) & 1;
    prol  = (flags >> 0) & 1;

    DesenharTelaIHM(widget, &maq[tampa][ext][prol]);
  }

  return TRUE;
}
