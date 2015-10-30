// Para que a funcao atof funcione corretamente
#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "defines.h"
#include "GtkUtils.h"

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))
#define CTOI(x) (min(max(0, x - '0'), 9))

// Funcao e dados de usuario para a tela de aplicar material
static int (*userAplicarMaterial)(struct strMaterial, int, int, void *);
static void *userData;

// Lista de materiais disponiveis para uso
struct strMaterial *lstMaterial = NULL;

// Aloca memoria para nova tarefa
struct strMaterial * AllocNewMaterial(void)
{
  struct strMaterial *currItem;
  static struct strMaterial *lastItem;

  currItem = (struct strMaterial *)malloc(sizeof(struct strMaterial));
  memset(currItem, 0, sizeof(struct strMaterial));

  if(lstMaterial == NULL) {
	  lstMaterial = currItem;
  } else {
    lastItem->Next = currItem;
  }

  lastItem = currItem;

  return currItem;
}

// Limpa as tarefas atuais
void ClearMaterials(void)
{
  static struct strMaterial *currItem;

  while(lstMaterial != NULL) {
	  currItem = lstMaterial;
	  lstMaterial = lstMaterial->Next;
	  free(currItem);
  }
}

// Retorna o material indicado pelo indice
struct strMaterial * GetMaterial(unsigned int idx)
{
  struct strMaterial *Item = lstMaterial;

  while(idx-- && Item != NULL)
	  Item = Item->Next;

  return Item;
}

// Retorna o material marcado como em uso. Retorna nulo se nenhum estiver marcado
struct strMaterial * GetMaterialInUse(void)
{
  struct strMaterial *Item = lstMaterial;

  while(Item != NULL && !(Item->inUse)) {
	  Item = Item->Next;
  }

  return Item;
}

// Retorna o material marcado como em uso. Retorna nulo se nenhum estiver marcado
struct strMaterial * GetMaterialByTask(int idTask)
{
  struct strMaterial *Item = lstMaterial, *itemFound = NULL;

  while(Item != NULL) {
	  // Se a tarefa coincide, marca porem continua para achar o ultimo registro para esta tarefa
	  if(Item->idTarefa == idTask) {
		  itemFound = Item;
	  }

	  Item = Item->Next;
  }

  return itemFound;
}

/*** Funcoes internas de gerenciamento / controle de materiais ***/
int material_calcModulo10(char *val, int count)
{
	int i;
	int digito = 0;
	long long sum = 0;

	for(i = count - 1; i >= 0; i--) {
		// Calcula o fator
		int fator = CTOI(val[i]) * (1 + ((count - i) % 2));

		// Soma os digitos de fator
		do {
			sum += fator % 10;
			fator /= 10;
		} while(fator > 0);
	}

	// Soma calculada. Agora calculamos o digito em si
	digito = 10 - (sum % 10);

	return digito == 10 ? 0 : digito;
}

int material_calcModulo11(char *val, int count)
{
	int i;
	int digito = 0;
	long long sum = 0;

	for(i = count - 1; i >= 0; i--) {
		// Soma o valor. O fator de multiplicacao deve variar entre 2 e 9, voltando a 2 ao ultrapassar.
		sum += CTOI(val[i]) * (((count - i - 1) % 8) + 2);
	}

	// Soma calculada. Agora calculamos o digito em si
	digito = sum % 11;
	if(digito > 1) {
		digito = 11 - digito;
	} else {
		digito = 0;
	}

	return digito;
}

int material_getDV(char *strCodigo, enum enumTipoEstoque tipo)
{
	int strSize, dv;
	char *codigo;

	// Se string for nula, retorna zero
	if(strCodigo == NULL) return 0;

	// Calcula o tamanho do codigo. Se for string vazia, retorna erro
	strSize = strlen(strCodigo);
	if(strSize < 1) return 0;

	// Soma 2 ao tamanho pois iremos adicionar os digitos de tipo de estoque
	strSize += 2;

	// Aloca memoria para a string que sera gerada para calculo do digito verificador:
	// Tipo de Estoque (01 para materia prima) + codigo fornecido (sem o digito verificador)
	codigo = (char *)malloc(strSize + 1);

	// O usuario fornecera apenas o codigo do material e portanto devemos adicionar o digito que representa o tipo de material.
	sprintf(codigo, "%c%c%s", '0' + (tipo / 10), '0' + (tipo % 10), strCodigo);

	// Calcula o digito conforme o codigo fornecido e retorna
	dv = (material_calcModulo10(codigo, strSize) * 10) + material_calcModulo11(codigo, strSize);

	// Desaloca a string criada
	free(codigo);

	// Retorna o digito calculado
	return dv;
}

int material_checaDV(char *strCodigo, int dv, enum enumTipoEstoque tipo)
{
	int strSize;

	// Se string for nula, retorna zero
	if(strCodigo == NULL) return 0;

	// Calcula o tamanho do codigo. Se for string vazia, retorna erro
	strSize = strlen(strCodigo);
	if(strSize < 1) return 0;

	// Calcula o digito conforme o codigo fornecido. Se os digitos verificadores forem iguais, indica que o codigo eh valido
	return dv == material_getDV(strCodigo, tipo);
}

void material_select(struct strMaterial *material)
{
	char sql[200];

	// Primeiro desmarca todos os materiais em uso
	DB_Execute(&mainDB, 2, "update material set inUse=0");

	// Se foi fornecido um material valido, ele sera o novo material selecionado
	if(material != NULL) {
		sprintf(sql, "update material set inUse=1 where id=%d", material->id);
		DB_Execute(&mainDB, 2, sql);
	}
}

void material_registraConsumo(struct strMaterial *materialConsumido, struct strMaterial *materialProduzido, unsigned int qtd, unsigned int tamPeca, unsigned int tamPerda)
{
	// Se material for nulo, quantidade ou tamanho forem zero, nao ha como registrar o consumo
	if(materialConsumido == NULL || materialProduzido == NULL || qtd == 0 || (tamPeca == 0 && tamPerda == 0)) return;

	// Peso em gramas por metro. Como a medida eh em milimetros e o peso final deve ser em  KG / metro, precisamos dividir por 1000 por duas vezes ou 1.000.000
	const float pesoGramasPorCm3 = 7.856;

	float pesoKgProduzido = (pesoGramasPorCm3 * qtd * tamPeca  * materialConsumido->espessura * materialConsumido->largura) / 1000000.0;
	float pesoKgPerda     = (pesoGramasPorCm3 * qtd * tamPerda * materialConsumido->espessura * materialConsumido->largura) / 1000000.0;
	float pesoKgConsumido = pesoKgProduzido + pesoKgPerda;

	// TODO: Peso nao pode ficar negativo! Por isso consideramos como peso consumido apenas o que restava de bobina.
	// Mas de onde veio esse excesso de material?? Precisamos tratar isso futuramente...
	if(materialConsumido->peso < pesoKgConsumido) {
		pesoKgConsumido = materialConsumido->peso;
		// Recalcula a perda para "fechar a conta". Melhor solucao ate o momento...
		pesoKgPerda = pesoKgConsumido - pesoKgProduzido;
	}

	// Atualiza as propriedades do material consumido
	materialConsumido->peso       -= pesoKgConsumido;

	// Atualiza as propriedades do material produzido
	materialProduzido->quantidade += qtd;
	materialProduzido->tamanho     = tamPeca;
	materialProduzido->peso       += pesoKgConsumido;

	GravarMaterial(*materialConsumido);
	GravarMaterial(*materialProduzido);

	// Se o material acabou: Desmarca o material, obrigando a utilizacao de um novo material
	if(materialConsumido->peso <= 0.0) {
		material_select(NULL);
	}

	// Agora enviamos a mensagem informando da producao. Devemos trabalhar nos dados para adequar ao esperado pelo sistema
	// entao utilizamos novas variaveis para isso.
	struct strMaterial matProduto = *materialProduzido, matConsumo = *materialConsumido, matPerda = *materialConsumido;

	// Primeiro adequamos o material produzido.
	// Devemos enviar apenas o que foi produzido nesse momento e nao o total
	matProduto.quantidade = qtd;
	matProduto.peso       = pesoKgProduzido;

	// Agora adequamos o material consumido.
	matConsumo.peso       = pesoKgConsumido;

	// Por ultimo, as perdas.
	matPerda  .peso       = pesoKgPerda;

	monitor_enviaMsgMatProducao(&matConsumo, &matProduto, &matPerda);
}

void ConfigBotoesMaterial(GtkWidget *wdg, struct strMaterial *material)
{
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnMaterialSelect"    )), material != NULL && material->tipo == enumTipoEstoque_MateriaPrima);
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnMovimentarMaterial")), material != NULL);
}

void CarregaComboLocais(GtkComboBox *cmb)
{
  DB_Execute(&mainDB, 2, "select codigo from local");
  CarregaCombo(&mainDB, cmb, 2, NULL);
}

void CarregaComboTipos(GtkComboBox *cmb)
{
	static int first = TRUE;

	if(first) {
		// Essa lista eh fixa. Itens nunca mudam! Carrega apenas na primeira vez
		first = FALSE;

		// Limpa os itens atuais
		gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(cmb)));

		// Carrega os tipos existentes
		CarregaItemCombo(cmb, "0 - Mercadoria Para Revenda");
		CarregaItemCombo(cmb, "1 - Matéria Prima"          );
		CarregaItemCombo(cmb, "2 - Embalagem"              );
		CarregaItemCombo(cmb, "3 - Produto em Processo"    );
		CarregaItemCombo(cmb, "4 - Produto Acabado"        );
		CarregaItemCombo(cmb, "5 - Subproduto"             );
		CarregaItemCombo(cmb, "6 - Produto Intermediário"  );
		CarregaItemCombo(cmb, "7 - Material Uso e Consumo" );
		CarregaItemCombo(cmb, "8 - Ativo Imobilizado"      );
		CarregaItemCombo(cmb, "9 - Serviços"               );
		CarregaItemCombo(cmb, "10 - Outros Insumos"        );
		CarregaItemCombo(cmb, "99 - Outros"                );
	}

	// Configura o tipo Materia Prima como padrao
	gtk_combo_box_set_active(cmb, 1);
}

void InsertMaterial(void)
{
  struct strMaterial *currItem;

  // Aloca memoria para nova tarefa
  currItem = AllocNewMaterial();

  // Salva dados do material
  currItem->id         = atoi(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "mID"       )));
  strcpy(currItem->codigo   , DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "mCod"      )));
  currItem->tipo       = atoi(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "tipo"      )));
  currItem->largura    = atoi(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "largura"   )));
  currItem->espessura  = atof(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "espessura" )));
  currItem->inUse      = atoi(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "inUse"     )));
  strcpy(currItem->local    , DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "lCod"      )));
  currItem->peso       = atof(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "peso"      )));
  currItem->tamanho    = atoi(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "tamanho"   )));
  currItem->quantidade = atoi(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "quantidade")));
  currItem->idTarefa   = atoi(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "idTarefa"  )));
}

void GravarMaterial(struct strMaterial material)
{
	char sql[800];

	// Carrega o ID do local selecionado
	sprintf(sql, "select id from local where codigo='%s'", material.local);
	DB_Execute(&mainDB, 2, sql);
	DB_GetNextRow(&mainDB, 2);

	if(material.id != 0) {
		sprintf(sql, "update material set codigo='%s', espessura='%f', largura='%d', idLocal='%d', peso='%f', tipo='%d', quantidade='%d', "
			  "tamanho='%d', idTarefa='%d' where id='%d'",
			  material.codigo, material.espessura, material.largura, atoi(DB_GetData(&mainDB, 2, 0)), material.peso,
			  material.tipo, material.quantidade, material.tamanho, material.idTarefa, material.id);

		DB_Execute(&mainDB, 0, sql);
	} else {
		sprintf(sql, "insert into material (codigo, espessura, largura, idLocal, peso, tipo, quantidade, tamanho, idTarefa)"
				" values ('%s', '%f', '%d', '%d', '%f', '%d', '%d', '%d', '%d')",
				material.codigo, material.espessura, material.largura, atoi(DB_GetData(&mainDB, 2, 0)),
				material.peso, material.tipo, material.quantidade, material.tamanho, material.idTarefa);

		DB_Execute(&mainDB, 0, sql);
	}
}

void CarregaListaMateriais(GtkWidget *tvw)
{
  int i, ItemIndex = 1;
  const int tam = 7;
  char *valores[tam+1];

  valores[tam] = NULL;

  // Limpa a lista anterior
  ClearMaterials();

  // Exclui os itens atuais
  TV_Limpar(tvw);

  // Carrega os materiais do MySQL
  DB_Execute(&mainDB, 1, "select m.id as mID, m.codigo as mCod, m.inUse, m.largura, m.espessura, m.peso, l.codigo as lCod, m.tamanho, m.quantidade, m.tipo, m.idTarefa"
		  " from material as m, local as l where l.id = m.idLocal and (m.peso > 0 or tipo = 3) order by mID");
  while(DB_GetNextRow(&mainDB, 1)>0)
    {
    valores[0] = (char*)malloc(10);
    sprintf(valores[0], "%d", ItemIndex++);
    for(i = 1; i<tam; i++) {
      valores[i] = DB_GetData(&mainDB, 1, i);
      if(valores[i] == NULL)
        valores[i] = "";

      // Condicao especial: Para o campo "Em Uso" devemos tratar e mudar de 1 / 0 para Sim / Nao
      if(i == 2) {
    	  char *tmp = (char *)malloc(4); // Aloca espaco para Sim ou Nao
    	  if(valores[i][0] == '0') {
    		  strcpy(tmp, "Não");
    	  } else {
    		  strcpy(tmp, "Sim");
    	  }

    	  valores[i] = tmp;
      }
    }

    // Insere um novo material a partir do registro atual
    InsertMaterial();

    TV_Adicionar(tvw, valores);

    free(valores[0]); // Indice
    free(valores[2]); // Em Uso
    }

  ConfigBotoesMaterial(GTK_WIDGET(tvw), NULL);
}

gboolean ChecarMaterial(struct strMaterial material, int dv, int isFullCheck)
{
  int materialExiste = FALSE;
  char tmp[100], sql[500];
  char *msg = NULL;

  // Limites
  const float peso_max      = 5500.0;
  const int   largura_min   = 10 , largura_max   = 500;
  const float espessura_min = 1.0, espessura_max = 5.0;

  // Verifica se o material ja existe
  sprintf(sql, "select id from material where codigo='%s' and tipo=%d and (idTarefa = 0 or idTarefa != %d)", material.codigo, material.tipo, material.idTarefa);
  DB_Execute(&mainDB, 1, sql);
  if(DB_GetNextRow(&mainDB, 1) > 0) {
	  materialExiste = TRUE;
  }

  // Carrega o ID do local selecionado
  sprintf(sql, "select id from local where codigo='%s'", material.local);
  DB_Execute(&mainDB, 1, sql);
  DB_GetNextRow(&mainDB, 1);

  if(material_checaDV(material.codigo, dv, material.tipo) == 0) { // Codigo invalido
    msg = "Codigo de material inválido!";
  } else if(materialExiste) { // Material ja cadastrado!
    msg = "Este material já foi cadastrado!";
  } else if(isFullCheck) {
	  if(material.peso < 1.0) { // Deve ser múltiplo do passo e maior que zero.
		msg = "O peso deve ser maior que zero!";
	  } else if(material.peso > peso_max) { // Verifica se peso supera o limite
			sprintf(tmp, "O peso deve ser menor ou igual a %d", (int)peso_max);
			msg = tmp;
	  } else if(DB_GetData(&mainDB, 1, 0) == NULL) {
		  // Apenas precaucao. Isso nunca deveria acontecer!
		  msg = "Local selecionado inexistente!";
	  } else if(material.tipo == enumTipoEstoque_ProdutoEmProcesso) {
		  if(material.quantidade < 1) { // Largura deve ser superior a minima
			msg = "A quantidade deve ser maior que zero!";
		  } else if(material.tamanho < 1) {
			msg = "O comprimento deve ser maior que zero!";
		  }
	  } else {
		  if(material.largura < largura_min || material.largura > largura_max) { // Largura deve ser superior a minima
			sprintf(tmp, "A largura deve estar entre %d e %d", largura_min, largura_max);
			msg = tmp;
		  } else if(material.espessura < espessura_min || material.espessura > espessura_max) {
			sprintf(tmp, "A espessura deve estar entre %0.2f e %0.2f", espessura_min, espessura_max);
			msg = tmp;
		  }
	  }
  }

  if(msg==NULL)
    return TRUE;

  ShowMessageBox(msg, FALSE);

  return FALSE;
}

void LimparDadosMaterial()
{
  char *campos[] =
    {
	"lblCadMaterialNumero",
	"entCadMaterialCodigo",
	"entCadMaterialCodigoDV",
	"entCadMaterialEspessura",
	"entCadMaterialLargura",
	"entCadMaterialPeso",
    "entCadMaterialQuantidade",
    "entCadMaterialComprimento",
    "entCadMaterialPesoProdutoProcesso",
    "lblCadMaterialTarefa",
    ""
    };
  char *valor[] =
    {
    "- - -",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "- - -",
    };

  GravarValoresWidgets(campos, valor);
}

void IniciarDadosMaterial()
{
	LimparDadosMaterial();

	CarregaComboLocais(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbCadMaterialLocal")));
	CarregaComboTipos (GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbCadMaterialTipo" )));
}

int AplicarMaterial()
{
  char *valores[15];
  struct strMaterial material;
  gint resp = GTK_RESPONSE_YES;

  char *campos[] =
    {
    "lblCadMaterialNumero",
    "entCadMaterialCodigo",
    "entCadMaterialEspessura",
    "entCadMaterialLargura",
    "entCadMaterialPeso",
    "cmbCadMaterialLocal",
    "cmbCadMaterialTipo",
    "entCadMaterialCodigoDV",
    "entCadMaterialQuantidade",
    "entCadMaterialComprimento",
    "entCadMaterialPesoProdutoProcesso",
    "lblCadMaterialTarefa",
    ""
    };

  GtkWidget *dialog;

  if(!LerValoresWidgets(campos, valores))
    return FALSE; // Ocorreu algum erro lendo os campos. Retorna.

  // Carrega os dados na estrutura de material
  material.id         = atoi(valores[ 0]);
  strcpy(material.codigo   , valores[ 1]);
  material.tipo       = atoi(valores[ 6]);
  material.largura    = atoi(valores[ 3]);
  material.espessura  = atof(valores[ 2]);
  material.inUse      = 0;
  strcpy(material.local    , valores[ 5]);
  material.tamanho    = atoi(valores[ 9]);
  material.quantidade = atoi(valores[ 8]);
  material.idTarefa   = atoi(valores[11]);

  switch(material.tipo) {
	  default:
	  case enumTipoEstoque_MateriaPrima     : material.peso = (float)atoi(valores[ 4]); break;
	  case enumTipoEstoque_ProdutoEmProcesso: material.peso = (float)atoi(valores[10]); break;
  }

  if(userAplicarMaterial == NULL) {
	  // Checa se os dados são válidos.
	  if(!ChecarMaterial(material, atoi(valores[7]), TRUE))
		return FALSE;

	  if(material.id != 0) {
		dialog = gtk_message_dialog_new (NULL,
				  GTK_DIALOG_DESTROY_WITH_PARENT,
				  GTK_MESSAGE_QUESTION,
				  GTK_BUTTONS_YES_NO,
				  "Aplicar as alterações a este material?"
				  );

		resp = gtk_dialog_run(GTK_DIALOG(dialog));

		gtk_widget_destroy (dialog);
	  }

	  if(resp == GTK_RESPONSE_YES) {
		  GravarMaterial(material);
		  monitor_enviaMsgMatCadastro(&material);
	  }
  } else {
	  return (*userAplicarMaterial)(material, atoi(valores[7]), FALSE, userData);
  }

  return TRUE;
}

// Funcao para abrir a tela de cadastro de material conforme o modo solicitado
void AbrirCadastroMaterial(struct strMaterial *material, int canEdit, int showDetails, int (*fnc)(struct strMaterial, int, int, void *), void *data)
{
	IniciarDadosMaterial();

	if(material != NULL) {
		int i;
		const int size = 10;
		char *valor[size];
		char *campos[] = {
			"lblCadMaterialNumero",
			"entCadMaterialCodigo",
			"entCadMaterialEspessura",
			"entCadMaterialLargura",
			"entCadMaterialPeso",
			"entCadMaterialQuantidade",
			"entCadMaterialComprimento",
			"entCadMaterialPesoProdutoProcesso",
			"lblCadMaterialTarefa",
			""
		};

		/*** Carrega os campos de material ***/

		// Aloca 100 caracteres em cada posicao. Desalocamos apos carregar os campos na tela.
		for(i = 0; i < size; i++) {
			valor[i] = (char *)(malloc(100));
		}

		// Numero do material (Indice na lista)
		strcpy(valor[0], "---");

		// Codigo do material
		strcpy(valor[1], material->codigo);

		// Espessura em mm
		sprintf(valor[2], "%.2f", material->espessura);

		// Largura em mm
		sprintf(valor[3], "%d", material->largura);

		// Peso em kg (Aba de materia prima)
		sprintf(valor[4], "%d", (int)material->peso);

		// Quantidade
		sprintf(valor[5], "%d", material->quantidade);

		// Tamanho em mm
		sprintf(valor[6], "%d", material->tamanho);

		// Peso em kg (Aba de produto em processo)
		sprintf(valor[7], "%d", (int)material->peso);

		// Id da Tarefa
		sprintf(valor[8], "%d", material->idTarefa);

		/*** Valores carregados. Grava nos objetos da tela! ***/
		GravarValoresWidgets(campos, valor);

		// Desaloca a memoria do array de valores
		for(i = 0; i < size; i++) {
			free(valor[i]);
		}

		// Tipo de estoque
		int index = material->tipo;

		// Converte o tipo para o indice no combo
		if(index == enumTipoEstoque_Outros) {
			index = enumTipoEstoque_OutrosInsumos + 1;
		}

		// Configura o combo para o tipo solicitado
		gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbCadMaterialTipo")), index);
	}

	// Configura os campos para edicao ou nao
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "cmbCadMaterialTipo"               )), canEdit);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "entCadMaterialEspessura"          )), canEdit);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "entCadMaterialLargura"            )), canEdit);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "entCadMaterialPeso"               )), canEdit);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "entCadMaterialQuantidade"         )), canEdit);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "entCadMaterialComprimento"        )), canEdit);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "entCadMaterialPesoProdutoProcesso")), canEdit);

	gtk_widget_set_visible  (GTK_WIDGET(gtk_builder_get_object(builder, "ntbCadMaterial"                   )), showDetails);

	// Carrega o ponteiro de funcao e data nas variaveis globais para serem chamados ao fechar a tela
	userAplicarMaterial = fnc;
	userData = data;

	// Muda a tela atual para o cadastro de materiais
	WorkAreaGoTo(NTB_ABA_MATERIAL_ADD);
}

/*** Callbacks ***/

void cbMaterialSelecionado(GtkTreeView *treeview, gpointer user_data)
{
	int idx;
	char val[20];

	// Identifica o material selecionado
	TV_GetSelected(GTK_WIDGET(gtk_builder_get_object(builder, "tvwMaterial")), 0, val);

	// E entao configura os botoes para esse material
	idx = atoi(val);
	ConfigBotoesMaterial(GTK_WIDGET(treeview), (idx > 0) ? GetMaterial(idx - 1) : NULL);
}

// Funcao que confirma os dados de um material, tanto ao adicionar quanto ao editar
void cbAplicarMaterial(GtkButton *button, gpointer user_data)
{
  if(AplicarMaterial()) {
	  CarregaListaMateriais(GTK_WIDGET(gtk_builder_get_object(builder, "tvwMaterial")));
	  WorkAreaGoPrevious();
  }
}

void cbCadMaterialVoltar(GtkButton *button, gpointer user_data)
{
	int voltar = TRUE;

	if(userAplicarMaterial != NULL) {
		struct strMaterial material;
		memset(&material, 0, sizeof(struct strMaterial));
		voltar = (*userAplicarMaterial)(material, 0, TRUE, userData);
	}

	if(voltar) {
		WorkAreaGoPrevious();
	}
}

// Funcao para abrir a tela de materiais
void cbAbrirMaterial(GtkButton *button, gpointer user_data)
{
	CarregaListaMateriais(GTK_WIDGET(gtk_builder_get_object(builder, "tvwMaterial")));
	WorkAreaGoTo(NTB_ABA_MATERIAL);
}

// Funcao que seleciona o material a ser utilizado na producao
void cbMaterialSelect(GtkButton *button, gpointer user_data)
{
	char val[20];

	// Identifica o material selecionado
	TV_GetSelected(GTK_WIDGET(gtk_builder_get_object(builder, "tvwMaterial")), 0, val);

	// E entao seleciona esse material
	material_select(GetMaterial(atoi(val) - 1));

	// Por fim, atualiza a lista de materiais
	CarregaListaMateriais(GTK_WIDGET(gtk_builder_get_object(builder, "tvwMaterial")));
}

// Funcao para adicionar um novo material
void cbMaterialAdd(GtkButton *button, gpointer user_data)
{
	AbrirCadastroMaterial(NULL, TRUE, TRUE, NULL, NULL);
}

// Funcao para adicionar um novo material
void cbMaterialMovimentar(GtkButton *button, gpointer user_data)
{
	char val[20];
	struct strMaterial *material;

	// Identifica o material selecionado
	TV_GetSelected(GTK_WIDGET(gtk_builder_get_object(builder, "tvwMaterial")), 0, val);

	// E entao seleciona esse material
	material = GetMaterial(atoi(val) - 1);
	material_registraConsumo(material, GetMaterialByTask(204), 3, 2500, 10);
}

#define NTB_ABA_MATERIAL_MATERIAPRIMA    0
#define NTB_ABA_MATERIAL_PRODUTOPROCESSO 1

void cbCadMaterialTipoAlterado(GtkComboBox *combobox, gpointer user_data)
{
	int tipo, aba;
	char strval[100];

	if(gtk_combo_box_get_active(combobox)<0)
	return;

	tipo = atoi(LerComboAtivo(combobox));

	sprintf(strval, "%02d -", tipo);

	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "lblCadMaterialCodigoTipo")), strval);

	switch(tipo) {
	default:
	case enumTipoEstoque_MateriaPrima     : aba = NTB_ABA_MATERIAL_MATERIAPRIMA   ; break;
	case enumTipoEstoque_ProdutoEmProcesso: aba = NTB_ABA_MATERIAL_PRODUTOPROCESSO; break;
	}

	gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "ntbCadMaterial")), aba);
}
