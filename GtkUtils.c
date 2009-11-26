#include "GtkUtils.h"

// Função para abrir uma janela em modo modal
void AbrirJanelaModal(GtkWidget *wdg)
{
  gtk_window_set_modal(GTK_WINDOW(wdg), 1);
  gtk_widget_show_all(wdg);
}

void MessageBox(gchar *msg)
{
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new (NULL,
                          GTK_DIALOG_DESTROY_WITH_PARENT,
                          GTK_MESSAGE_INFO,
                          GTK_BUTTONS_CLOSE,
                          "%s",
						  msg);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

void ExcluiItemCombo(GtkComboBox *cmb, guint pos)
{
	char tree_path[10];
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(cmb);

	sprintf(tree_path, "%d", pos);
	gtk_tree_model_get_iter_from_string(model, &iter, tree_path);
	gtk_list_store_remove(GTK_LIST_STORE(model), &iter);

	gtk_combo_box_set_wrap_width(cmb,(gtk_tree_model_iter_n_children(model,NULL)/5) + 1);
}

void CarregaItemCombo(GtkComboBox *cmb, char *txt)
{
	GtkTreeIter iter;
	GtkListStore *store = GTK_LIST_STORE(gtk_combo_box_get_model(cmb));

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, txt, -1);

	gtk_combo_box_set_wrap_width(cmb,(gtk_tree_model_iter_n_children(GTK_TREE_MODEL(store),NULL)/5) + 1);
}

void CarregaCombo(GtkComboBox *cmb, guint nsel, char *adicional)
{
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(cmb)));

	if(adicional != NULL)
		CarregaItemCombo(cmb, adicional);

	while(DB_GetNextRow(&mainDB, nsel)>0)
		CarregaItemCombo(cmb, DB_GetData(&mainDB, nsel, 0));

	gtk_combo_box_set_active(cmb, 0);
}

char *LerComboAtivo(GtkComboBox *cmb)
{
	char tmp[10];
	GtkTreeIter iter;
	GValue valor = { 0 };
	GtkListStore *store = GTK_LIST_STORE(gtk_combo_box_get_model(cmb));

	if(GTK_IS_COMBO_BOX_ENTRY(cmb))
		return gtk_combo_box_get_active_text(cmb);

	printf("gtk_combo_box_get_active(cmb) = %d\n", gtk_combo_box_get_active(cmb));
	sprintf(tmp, "%d", gtk_combo_box_get_active(cmb));
	gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(store), &iter, tmp);
	gtk_tree_model_get_value(GTK_TREE_MODEL(store), &iter, 0, &valor);

	return (char *)(g_value_get_string(&valor));
}

gboolean CarregaCampos(GtkComboBox *cmb, char **campos, char **botoes, char *tabela, char *campo_where)
{
	GtkWidget *obj;
	gboolean estado = FALSE;
	char sql[100];
	int i;

	if(gtk_combo_box_get_active(cmb) != 0) // Não é o primeiro item, carregar campos.
		{
		sprintf(sql, "select * from %s where %s='%s'",
			tabela, campo_where, LerComboAtivo(cmb));

		DB_Execute(&mainDB, 0, sql);
		DB_GetNextRow(&mainDB, 0);

		estado = TRUE;
		}

	for(i=0; campos[i][0]!=0; i+=2)
		{
		obj = GTK_WIDGET(gtk_builder_get_object(builder, campos[i]));

		if(estado == TRUE)
			gtk_entry_set_text(GTK_ENTRY(obj), DB_GetData(&mainDB, 0, DB_GetFieldNumber(&mainDB, 0, campos[i+1])));
		else
			gtk_entry_set_text(GTK_ENTRY(obj), "");
		}

	for(i=0; botoes[i][0]!=0; i++)
		{
		obj = GTK_WIDGET(gtk_builder_get_object(builder, botoes[i]));
		gtk_widget_set_sensitive(obj, estado);
		}

	return estado;
}

// Carrega em Valor o nome do RadioButton atualmente selecionado
char * LerRadioAtual(GtkWidget *wdg, char *valor)
{
	GSList *lst = gtk_radio_button_get_group(GTK_RADIO_BUTTON(wdg));

	while(lst)
		{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lst->data)) == TRUE)
			break;

		lst = lst->next;
		}

	if(lst)
		valor = (char *)(gtk_button_get_label(GTK_BUTTON(lst->data)));

	return (char *)(gtk_button_get_label(GTK_BUTTON(lst->data)));
}

int AchaIndiceCombo(GtkComboBox *cmb, char *valor)
{
	guint i=0;
	GtkTreeIter iter;
	GValue gv = { 0 };
	char tree_path[10] = "0";
	GtkTreeModel *model = GTK_TREE_MODEL(gtk_combo_box_get_model(cmb));

	while(gtk_tree_model_get_iter_from_string(model, &iter, tree_path))
		{
		gtk_tree_model_get_value(model, &iter, 0, &gv);

		if((char *)(g_value_get_string(&gv)) != NULL)
			if(!strcmp(valor, (char *)(g_value_get_string(&gv))))
				{
				g_value_unset(&gv);
				return i;
				}

		g_value_unset(&gv);

		sprintf(tree_path, "%d", ++i);
		}

	return -1;
}

void GravarValorWidget(GtkWidget *wdg, char *nome, char *valor)
{
	GtkWidget *obj = GTK_WIDGET(gtk_builder_get_object(builder, nome));
	if(obj==NULL)
		return;

	if     (!strncmp(nome, "ent", 3))
		gtk_entry_set_text(GTK_ENTRY(obj), valor);
//	else if(!strncmp(nome, "rbt", 3))
//		return LerRadioAtual(obj, tmp);
	else if(!strncmp(nome, "txv", 3))
		gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(obj)), valor, -1);
	else if(!strncmp(nome, "cmb", 3) || !strncmp(nome, "cbe", 3))
		gtk_combo_box_set_active(GTK_COMBO_BOX(obj), AchaIndiceCombo(GTK_COMBO_BOX(obj), valor));
}

void GravarValoresWidgets(GtkWidget *wdg, char **lst_wdg, char **lst_val)
{
	int i;

	for(i=0; lst_wdg[i][0]; i++)
		if(strcmp(lst_wdg[i], "0")) // Se for um campo válido, grava o valor.
		GravarValorWidget(wdg, lst_wdg[i], lst_val[i]);
}

char * LerValorWidget(GtkWidget *wdg, char *nome)
{
	char tmp[30];
	GtkTextIter start, end;
	GtkTextBuffer *tb;
	GtkWidget *obj = GTK_WIDGET(gtk_builder_get_object(builder, nome));
	if(obj==NULL)
		return NULL;

	if     (!strncmp(nome, "ent", 3) || !strncmp(nome, "spb", 3))
		return (char *)(gtk_entry_get_text(GTK_ENTRY(obj)));
	else if(!strncmp(nome, "txv", 3))
		{
		tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(obj));
		gtk_text_buffer_get_start_iter(tb, &start);
		gtk_text_buffer_get_end_iter(tb, &end);
		return (char *)(gtk_text_buffer_get_text(tb, &start, &end, FALSE));
		}
	else if(!strncmp(nome, "rbt", 3))
		return LerRadioAtual(obj, tmp);
	else if(!strncmp(nome, "cmb", 3) || !strncmp(nome, "cbe", 3))
		return (char *)(LerComboAtivo(GTK_COMBO_BOX(obj)));

	return NULL;
}

int LerValoresWidgets(GtkWidget *wdg, char **lst_wdg, char **lst_val)
{
	int i;

	for(i=0; lst_wdg[i][0]; i++)
		{
		lst_val[i] = LerValorWidget(wdg, lst_wdg[i]);
		if(lst_val[i] == NULL)
			return 0;
		}

	return 1;
}

void Asc2Utf(unsigned char *origem, unsigned char *destino)
{
	int i, j=0;
	for(i=0; origem[i]; i++)
		if(origem[i]<0x80)
			destino[j++] = origem[i];
		else
			{
			destino[j++] = 0xC0 | ((origem[i]&0xC0)>>6); // Usa os 2 últimos bits
			destino[j++] = 0x80 |  (origem[i]&0x3F);     // usa os outros 6 bits
			}

	destino[j] = 0;
}

void Utf2Asc(unsigned char *origem, unsigned char *destino)
{
	int i, j=0;
	for(i=0; origem[i]; i++)
		if(origem[i]&0x80)
			{
			destino[j++] = ((origem[i]&0x03)<<6) | (origem[i+1]&0x3F);
			i++; // utilizados 2 bytes. Incrementa contador.
			}
		else
			destino[j++] = origem[i];

	destino[j] = 0;
}

// Retorna o índice para a string fornecida na lista.
int BuscaStringLista(char *lista[], char *string, gboolean modo_UTF)
{
	int i;
	char *string_UTF;

	if(modo_UTF)
		{
		string_UTF = (char *)(malloc(strlen(string)*2)); // UTF usa até o dobro.
		Utf2Asc(string, string_UTF);
		}
	else
		string_UTF = string;

	for(i=0; lista[i][0]; i++)
		if(!strcmp(lista[i], string_UTF))
			break;

	if(modo_UTF)
		free(string_UTF);

	return i;
}
