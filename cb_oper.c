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
  int ciclos;
  static GtkLabel *lblSaldo = NULL, *lblProd = NULL;

  if(iniciando) {
    iniciando = 0;
    ciclos_inicial = atoi(gtk_label_get_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecTotal" ))));
    lblProd  = GTK_LABEL(gtk_builder_get_object(builder, "lblExecProd" ));
    lblSaldo = GTK_LABEL(gtk_builder_get_object(builder, "lblExecSaldo"));
  }

  ciclos = MaqLerPrsCiclos();

  if((MaqLerEstado() & MAQ_MODO_MASK) == MAQ_MODO_MANUAL) {
    iniciando = 1;
    MaqConfigModo(MAQ_MODO_MANUAL);

    WorkAreaGoTo(NTB_ABA_HOME);
    return FALSE;
  } else {
    sprintf(tmp, "%d", qtd - qtdprod);
    gtk_label_set_text(lblProd, tmp);

    sprintf(tmp, "%d", qtdprod);
    gtk_label_set_text(lblSaldo, tmp);
  }

  return TRUE;
}

gboolean tmrAguardaFimOperacao(gpointer data)
{
  if(MaqOperando()) {
    return TRUE;
  } else {
    if(data != NULL)
      (*((void (*)(void))(data)))();

    gtk_widget_hide_all(GTK_WIDGET(gtk_builder_get_object(builder, "wndOperando")));

    return FALSE;
  }
}

void AguardarFimOperacao(void (*fnc)(void))
{
  gtk_widget_show_all(GTK_WIDGET(gtk_builder_get_object(builder, "wndOperando")));
  g_timeout_add(500, tmrAguardaFimOperacao, (gpointer)fnc);
}

void AbrirOper()
{
  if(!GetUserPerm(PERM_ACESSO_OPER))
    {
    WorkAreaGoTo(NTB_ABA_HOME);
    MessageBox("Sem permissão para operar a máquina!");
    return;
    }
}

void cbExecTarefa(GtkButton *button, gpointer user_data)
{
  GtkWidget *wdg;

  if(!(MaqLerEstado() & MAQ_STATUS_INITOK))
    return;

  Log("Iniciando producao", LOG_TIPO_TAREFA);

  MaqConfigModo(MAQ_MODO_AUTO);
  g_timeout_add(1000, tmrExec, NULL);

  wdg = GTK_WIDGET(gtk_builder_get_object(builder, "btnExecIniciarParar"));
  gtk_widget_set_sensitive(wdg, TRUE);
}

void cbExecParar(GtkButton *button, gpointer user_data)
{
  MaqConfigModo(MAQ_MODO_MANUAL);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblExecMsg")), "Terminando...");
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
}

void cbManAplanAvancar()
{
  printf("Aplanadora: Avançar\n");
}

void cbManAplanRecuar()
{
  printf("Aplanadora: Recuar\n");
}

void cbManAplanAbrir()
{
  printf("Aplanadora: Abrir\n");
}

void cbManAplanFechar()
{
  printf("Aplanadora: Fechar\n");
}

void cbManAplanSubir()
{
  printf("Aplanadora: Subir\n");
}

void cbManAplanDescer()
{
  printf("Aplanadora: Descer\n");
}

void cbManAplanExpandir()
{
  printf("Aplanadora: Expandir\n");
}

void cbManAplanRetrair()
{
  printf("Aplanadora: Retrair\n");
}

struct {
  unsigned int xmin, ymin, xmax, ymax;
  void (*fnc)();
  char *img;
} lst_coord[] = {
    {   0,   0, 100, 100, cbManAplanAvancar , "images/seta.png" },
    { 100,   0, 200, 100, cbManAplanRecuar  , "images/seta.png" },
    { 200,   0, 300, 100, cbManAplanAbrir   , "images/seta.png" },
    { 300,   0, 400, 100, cbManAplanFechar  , "images/seta.png" },
    {   0, 100, 100, 200, cbManAplanSubir   , "images/seta.png" },
    { 100, 100, 200, 200, cbManAplanDescer  , "images/seta.png" },
    { 200, 100, 300, 200, cbManAplanExpandir, "images/seta.png" },
    { 300, 100, 400, 200, cbManAplanRetrair , "images/seta.png" },
    { 0, 0, 0, 0, NULL },
};

gboolean cbMaquinaButtonPress(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  unsigned int i;

  if(event->type == GDK_BUTTON_PRESS) {
    printf("Clicado em %d:%d\n", (int)event->x, (int)event->y);
    for(i=0; lst_coord[i].fnc != NULL; i++) {
      printf("Comparando com %d =  %d:%d / %d:%d\n", i, lst_coord[i].xmin, lst_coord[i].ymin, lst_coord[i].xmax, lst_coord[i].ymax);
      if(event->x >= lst_coord[i].xmin &&
         event->y >= lst_coord[i].ymin &&
         event->x <= lst_coord[i].xmax &&
         event->y <= lst_coord[i].ymax) {
        (lst_coord[i].fnc)();
        break;
      }
    }

//    WorkAreaGoTo(NTB_ABA_MANUT);
  }

  return TRUE;
}

void cbManualPerfAvanca(GtkButton *button, gpointer user_data)
{
  MaqPerfManual(PERF_AVANCA);
}

void cbManualPerfRecua(GtkButton *button, gpointer user_data)
{
  MaqPerfManual(PERF_RECUA);
}

void cbManualPerfParar(GtkButton *button, gpointer user_data)
{
  MaqPerfManual(PERF_PARAR);
}

void cbManualMesaAvanca(GtkButton *button, gpointer user_data)
{
  MaqMesaManual(MESA_AVANCA);
}

void cbManualMesaRecua(GtkButton *button, gpointer user_data)
{
  MaqMesaManual(MESA_RECUA);
}

void cbManualMesaParar(GtkButton *button, gpointer user_data)
{
  MaqMesaManual(MESA_PARAR);
}

void cbManualMesaCortar(GtkButton *button, gpointer user_data)
{
  MaqCortar();
  AguardarFimOperacao(NULL);
}

void cbManualMesaPosic(GtkButton *button, gpointer user_data)
{
  MaqMesaPosic(0);
  AguardarFimOperacao(NULL);
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

#define PERF_ALTURA_MESA 98

gboolean cbDesenharMaquina(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  unsigned int i;
  static GdkPixbuf *pb = NULL;
  if(pb == NULL) {
    pb = gdk_pixbuf_new_from_file_at_scale("images/bg01.png",
        widget->allocation.width, widget->allocation.height,
        FALSE, NULL);

    LoadIntoPixbuf(pb, "images/maq-desbob.png"      ,  0,                0, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_RIGHT);

    LoadIntoPixbuf(pb, "images/maq-perf-ini.png"    , 10,                0, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);
    LoadIntoPixbuf(pb, "images/maq-perf-mesa.png"   , -1,                0, 1.25, 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);
    LoadIntoPixbuf(pb, "images/maq-perf-fim.png"    , -1,                0, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);

    LoadIntoPixbuf(pb, "images/maq-perf-prensa.png" , 20, PERF_ALTURA_MESA, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);
    LoadIntoPixbuf(pb, "images/maq-perf-guia.png"   , -1, PERF_ALTURA_MESA, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);
    LoadIntoPixbuf(pb, "images/maq-perf-castelo.png", -1, PERF_ALTURA_MESA, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);
    LoadIntoPixbuf(pb, "images/maq-perf-castelo.png", -1, PERF_ALTURA_MESA, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);
    LoadIntoPixbuf(pb, "images/maq-perf-castelo.png", -1, PERF_ALTURA_MESA, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);
    LoadIntoPixbuf(pb, "images/maq-perf-guia.png"   , -1, PERF_ALTURA_MESA, 1   , 1, LOADPB_REFPOS_DOWN | LOADPB_REFPOS_LEFT);

    for(i=0; lst_coord[i].fnc != NULL; i++) {
      LoadIntoPixbuf(pb, lst_coord[i].img, lst_coord[i].xmin, lst_coord[i].ymin, 1   , 1, LOADPB_REFPOS_UP | LOADPB_REFPOS_LEFT);
    }
  }

  gdk_draw_pixbuf(widget->window,
                  widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                  pb,
                  0, 0, 0, 0, -1, -1,
                  GDK_RGB_DITHER_NONE, 0, 0);

  return TRUE;
}
