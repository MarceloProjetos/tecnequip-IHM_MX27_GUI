// Para que a funcao atof funcione corretamente
#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "defines.h"
#include "GtkUtils.h"

// Flags de operacoes da maquina
#define MAQ_MODO_CALC_FATOR 0x8000
#define MAQ_MODO_CALC_REL   0x1000
#define MAQ_MODO_CALC_MIN   0x2000
#define MAQ_MODO_POSIC      0x4000

// Flags de estado da maquina
#define MAQ_STATUS_OPERANDO 0x0200

// Funcao que verifica que ja terminou a inicializacao
int CheckInitOK(void)
{
  if(!(MaqLerEstado() & MAQ_STATUS_INITOK)) {
    MessageBox("Máquina não inicializada!");
    return 0;
  }

  return 1;
}

// Executa o procedimento para calculo do fator de correcao do encoder do perfil
void MaqCalcFatorPerfil()
{
  uint16_t modo = MaqLerFlags();
  modo |= MAQ_MODO_CALC_FATOR;
  MaqConfigFlags(modo);
}

// Executa o procedimento para calculo da relacao entre os encoders (perfil e mesa)
void MaqCalcRelEnc()
{
  uint16_t modo = MaqLerFlags();
  modo |= MAQ_MODO_CALC_REL;
  MaqConfigFlags(modo);
}

// Executa o procedimento para calculo da menor peca para o modo Dinamico
void MaqCalcTamMin()
{
  uint16_t modo = MaqLerFlags();
  modo |= MAQ_MODO_CALC_MIN;
  MaqConfigFlags(modo);
}

// Retorna se a maquina esta realizando uma operacao
uint16_t MaqOperando()
{
  return MaqLerEstado() & MAQ_STATUS_OPERANDO ? TRUE : FALSE;
}

uint16_t MaqLerNovoFator(void)
{
  uint16_t fator;

  fator = MaqLerRegistrador(MAQ_REG_COLN_NOVO_VALOR, maq_param.encoder.fator);
  printf("Novo fator: %.03f\n", (float)(fator)/1000);

  return fator;
}

uint16_t MaqLerNovoRelEnc(void)
{
  uint16_t fator;

  fator = MaqLerRegistrador(MAQ_REG_COLN_NOVO_VALOR, 1000);
  printf("Novo fator: %.03f\n", (float)(fator)/1000);

  return fator;
}

uint16_t MaqLerNovoTamMin(void)
{
  uint16_t tammin;

  tammin = MaqLerRegistrador(MAQ_REG_COLN_NOVO_VALOR, maq_param.custom.coln.tam_min);
  printf("Novo Tamanho Minimo: %d\n", tammin);

  return tammin;
}

// Funcoes para trocar aba da tela de configuracoes da coluna n
void cbColN_MudarAbaConfig(GtkButton *button, gpointer user_data)
{
  // Descobre a aba selecionada pelo nome do botão
  const gchar *nome = gtk_buildable_get_name(GTK_BUILDABLE(button));
  unsigned int aba  = atoi(&nome[strlen(nome)-1]);

  gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbColN")), aba);
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

void MaqMesaPosic(uint16_t pos)
{
  uint16_t modo;

  MaqGravarRegistrador(MAQ_REG_COLN_MESA_POS, pos);

  modo = MaqLerFlags();
  modo |= MAQ_MODO_POSIC;
  MaqConfigFlags(modo);
}

void cbManualMesaPosic(GtkButton *button, gpointer user_data)
{
  MaqMesaPosic(0);
  AguardarFimOperacao(NULL);
}

void retCalcFatorPerfil()
{
  char tmp[10];

  sprintf(tmp, "%.03f", (float)(MaqLerNovoFator())/1000);
  GravarValorWidget("entEncoderFator", tmp);
}

void cbCalcFatorMesa(GtkButton *button, gpointer user_data)
{
  if(CheckInitOK()) {
    MaqCalcFatorPerfil();
    AguardarFimOperacao(retCalcFatorPerfil);
  }
}

void retCalcRelEnc()
{
  char tmp[10];

  sprintf(tmp, "%.03f", (float)(MaqLerNovoRelEnc())/1000);
  printf("%s\n", tmp);
//  GravarValorWidget("lblMaqRelEncoders", tmp);
}

void cbCalcRelEnc(GtkButton *button, gpointer user_data)
{
  if(CheckInitOK()) {
    MaqCalcRelEnc();
    AguardarFimOperacao(retCalcRelEnc);
  }
}

void retCalcTamMin()
{
  char tmp[10];

  sprintf(tmp,"%d", MaqLerNovoTamMin()/10);
  GravarValorWidget("entConfigTamMin", tmp);
}

void cbCalcTamMin(GtkButton *button, gpointer user_data)
{
  if(CheckInitOK()) {
    MaqCalcTamMin();
    AguardarFimOperacao(retCalcTamMin);
  }
}
