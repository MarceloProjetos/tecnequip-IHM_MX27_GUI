#ifndef GTKUTILS_H
#define GTKUTILS_H

#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include <DB.h>

// Objeto que contem toda a interface GTK
extern GtkBuilder *builder;

extern struct strDB mainDB;

extern void  MessageBox          (gchar *);
extern void  EstadoBotoes        (GtkWidget *, guint);
extern void  CarregaCombo        (struct strDB *sDB, GtkComboBox *, guint, char *);
extern void  CarregaItemCombo    (GtkComboBox *cmb, char *txt);
extern char *LerComboAtivo       (GtkComboBox *cmb);
extern void  ExcluiItemCombo     (GtkComboBox *cmb, guint pos);
extern int   AchaIndiceCombo     (GtkComboBox *cmb, char *valor);
extern char *LerValorWidget      (char *nome);
extern int   LerValoresWidgets   (char **lst_wdg, char **lst_val);
extern void  GravarValorWidget   (char *nome, char *valor);
extern void  GravarValoresWidgets(char **lst_wdg, char **lst_val);
extern int   BuscaStringLista    (char *lista[], char *string, gboolean modo_UTF);
extern int   CarregaCampos       (struct strDB *sDB, GtkComboBox *cmb, char **campos, char **botoes, char *tabela, char *campo_where);

extern void  Asc2Utf             (unsigned char *origem, unsigned char *destino);

extern void TV_Limpar     (GtkWidget *tvw);
extern void TV_Adicionar  (GtkWidget *tvw, char *lst_valores[]);
extern void TV_GetSelected(GtkWidget *tvw, int pos, char *dado);
extern void TV_Config     (GtkWidget *tvw, char *lst_campos[], GtkTreeModel *model);

// Função para abrir uma janela em modo modal
extern void AbrirJanelaModal(GtkWidget *wdg);

#endif /* UTILS_H */
