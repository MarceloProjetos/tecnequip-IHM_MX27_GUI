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

struct {
  unsigned int xmin, ymin, xmax, ymax;
  void (*fnc)();
  char *img;
} lst_coord[] = {
    {   0,   0,  50,  50, cbManAplanAvancar , "images/ihm-ent-perfil-avancar.png" },
    { 100,   0, 200, 100, cbManAplanRecuar  , "images/ihm-ent-perfil-recuar.png" },
    { 200,   0, 300, 100, cbManAplanAbrir   , "images/seta.png" },
    { 300,   0, 400, 100, cbManAplanFechar  , "images/seta.png" },
    {   0, 100, 100, 200, cbManAplanSubir   , "images/seta.png" },
    { 100, 100, 200, 200, cbManAplanDescer  , "images/seta.png" },
    { 200, 100, 300, 200, cbManAplanExpandir, "images/seta.png" },
    { 300, 100, 400, 200, cbManAplanRetrair , "images/seta.png" },
    { 400,  50, 500, 150, cbManAplanParar   , "images/seta.png" },
    {  50,  70, 315, 310, cbManPrsLigar     , "images/maq-aplan-corpo.png" },
    { 0, 0, 0, 0, NULL },
};

gboolean cbMaquinaButtonPress(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  unsigned int i;

  if(event->type == GDK_BUTTON_PRESS && MaqPronta()) {
    for(i=0; lst_coord[i].fnc != NULL; i++) {
      if(event->x >= lst_coord[i].xmin &&
         event->y >= lst_coord[i].ymin &&
         event->x <= lst_coord[i].xmax &&
         event->y <= lst_coord[i].ymax) {
        (lst_coord[i].fnc)();
        break;
      }
    }
  } else if (event->type == GDK_BUTTON_RELEASE) {
    MaqAplanManual(MAQ_APLAN_PARAR);
  }

  return TRUE;
}

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
  g_object_unref(pbtmp);

// Atualiza as coordenadas atuais
  last_x = x + width ;
  last_y = y + height;
}

void LoadCoordListIntoPixbuf(GdkPixbuf *pb)
{
  unsigned int i;

  for(i=0; lst_coord[i].fnc != NULL; i++) {
    LoadIntoPixbuf(pb, lst_coord[i].img, lst_coord[i].xmin, lst_coord[i].ymin, 1   , 1, LOADPB_REFPOS_UP | LOADPB_REFPOS_LEFT);
  }
}

gboolean cbDesenharMaquina(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  unsigned int tampa, ext, prol;
  static int first = 1, flags = 0;
  static GdkPixbuf *maq[2][2][2]; // 3 movimentos, 2 estados para cada um
  GdkPixbuf *pb; // 3 movimentos, 2 estados para cada um

  if(widget == NULL) {
    widget = GTK_WIDGET(gtk_builder_get_object(builder, "dwaMaquina"));
  }

  if(first) {
    first = 0;

    // Imagem 000 - Tampa fechada, extensão abaixada, prolongamento recuado.
    pb = gdk_pixbuf_new_from_file_at_scale("images/bg01.png",
        widget->allocation.width, widget->allocation.height,
        FALSE, NULL);
    maq[0][0][0] = pb;

    LoadIntoPixbuf(pb, "images/maq-aplan-corpo.png"        , 150,   0, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-prol-baixo.png"   , 128, 127, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-ext-baixo.png"    , 136, 134, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-tampa-fechada.png", 185, 127, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);

    LoadCoordListIntoPixbuf(pb);

    // Imagem 001 - Tampa fechada, extensão abaixada, prolongamento avançado.
    pb = gdk_pixbuf_new_from_file_at_scale("images/bg01.png",
        widget->allocation.width, widget->allocation.height,
        FALSE, NULL);
    maq[0][0][1] = pb;

    LoadIntoPixbuf(pb, "images/maq-aplan-corpo.png"        , 150,   0, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-prol-baixo.png"   ,  88,  87, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-ext-baixo.png"    , 136, 134, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-tampa-fechada.png", 185, 127, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);

    LoadCoordListIntoPixbuf(pb);

    // Imagem 010 - Tampa fechada, extensão levantada, prolongamento recuado.
    pb = gdk_pixbuf_new_from_file_at_scale("images/bg01.png",
        widget->allocation.width, widget->allocation.height,
        FALSE, NULL);
    maq[0][1][0] = pb;

    LoadIntoPixbuf(pb, "images/maq-aplan-corpo.png"        , 150,   0, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-prol-cima.png"    ,  93, 200, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-ext-cima.png"     , 109, 197, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-tampa-fechada.png", 185, 127, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);

    LoadCoordListIntoPixbuf(pb);

    // Imagem 011 - Tampa fechada, extensão levantada, prolongamento avançado.
    pb = gdk_pixbuf_new_from_file_at_scale("images/bg01.png",
        widget->allocation.width, widget->allocation.height,
        FALSE, NULL);
    maq[0][1][1] = pb;

    LoadIntoPixbuf(pb, "images/maq-aplan-corpo.png"        , 150,   0, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-prol-cima.png"    ,  53, 200, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-ext-cima.png"     , 109, 197, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-tampa-fechada.png", 185, 127, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);

    LoadCoordListIntoPixbuf(pb);

    // Imagem 100 - Tampa aberta, extensão abaixada, prolongamento recuado.
    pb = gdk_pixbuf_new_from_file_at_scale("images/bg01.png",
        widget->allocation.width, widget->allocation.height,
        FALSE, NULL);
    maq[1][0][0] = pb;

    LoadIntoPixbuf(pb, "images/maq-aplan-corpo.png"        , 150,   0, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-prol-baixo.png"   , 128, 127, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-ext-baixo.png"    , 136, 134, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-tampa-aberta.png" , 185, 127, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);

    LoadCoordListIntoPixbuf(pb);

    // Imagem 101 - Tampa aberta, extensão abaixada, prolongamento avançado.
    pb = gdk_pixbuf_new_from_file_at_scale("images/bg01.png",
        widget->allocation.width, widget->allocation.height,
        FALSE, NULL);
    maq[1][0][1] = pb;

    LoadIntoPixbuf(pb, "images/maq-aplan-corpo.png"        , 150,   0, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-prol-baixo.png"   ,  88,  87, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-ext-baixo.png"    , 136, 134, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-tampa-aberta.png" , 185, 127, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);

    LoadCoordListIntoPixbuf(pb);

    // Imagem 110 - Tampa aberta, extensão levantada, prolongamento recuado.
    pb = gdk_pixbuf_new_from_file_at_scale("images/bg01.png",
        widget->allocation.width, widget->allocation.height,
        FALSE, NULL);
    maq[1][1][0] = pb;

    LoadIntoPixbuf(pb, "images/maq-aplan-corpo.png"        , 150,   0, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-prol-cima.png"    ,  93, 200, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-ext-cima.png"     , 109, 197, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-tampa-aberta.png" , 185, 127, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);

    LoadCoordListIntoPixbuf(pb);

    // Imagem 111 - Tampa aberta, extensão levantada, prolongamento avançado.
    pb = gdk_pixbuf_new_from_file_at_scale("images/bg01.png",
        widget->allocation.width, widget->allocation.height,
        FALSE, NULL);
    maq[1][1][1] = pb;

    LoadIntoPixbuf(pb, "images/maq-aplan-corpo.png"        , 150,   0, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-prol-cima.png"    ,  53, 200, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-ext-cima.png"     , 109, 197, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);
    LoadIntoPixbuf(pb, "images/maq-aplan-tampa-aberta.png" , 185, 127, 1, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);

    LoadCoordListIntoPixbuf(pb);
  }

  if(data != NULL)
    flags = *(unsigned int *)(data);

  tampa = (flags >> 2) & 1;
  ext   = (flags >> 1) & 1;
  prol  = (flags >> 0) & 1;

  gdk_draw_pixbuf(widget->window,
                  widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                  maq[tampa][ext][prol],
                  0, 0, 0, 0, -1, -1,
                  GDK_RGB_DITHER_NONE, 0, 0);

  return TRUE;
}
