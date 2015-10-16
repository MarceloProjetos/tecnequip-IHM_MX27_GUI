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

int material_checaDigitoVerificador(char *strCodigo)
{
	int strSize;
	char *codigo;
	int digito_real, digito_calculado;

	// Se string for nula, retorna zero
	if(strCodigo == NULL) return 0;

	// Calcula o tamanho do codigo. Se for menor que 3 significa que eh invalido pois apenas o digito verificador exige 2 digitos
	strSize = strlen(strCodigo);
	if(strSize < 3) return 0;

	// Aloca memoria para a string que sera gerada para calculo do digito verificador:
	// Tipo de Estoque (01 para materia prima) + codigo fornecido (sem o digito verificador)
	codigo = (char *)malloc(strSize + 1);

	// Os dois ultimos numeros sao o digito verificador
	digito_real = atoi(&strCodigo[strSize - 2]);

	// O usuario fornecera apenas o codigo do material e portanto devemos adicionar o digito que representa o tipo de material.
	codigo[0] = '0'; // Codigo da Materia Prima: 01
	codigo[1] = '1'; // Codigo da Materia Prima: 01
	strncpy(&codigo[2], strCodigo, strSize - 2); // Agrega o codigo
	codigo[strSize] = 0; // Finaliza a string

	// Calcula o digito conforme o codigo fornecido
	digito_calculado = (material_calcModulo10(codigo, strSize) * 10) + material_calcModulo11(codigo, strSize);

	// Desaloca a string criada
	free(codigo);

	// Se os digitos verificadores forem iguais, indica que o codigo eh valido
	return digito_real == digito_calculado;
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

void material_registraConsumo(struct strMaterial *material, unsigned int qtd, unsigned int tam)
{
	// Se material for nulo, quantidade ou tamanho forem zero, nao ha como registrar o consumo
	if(material == NULL || qtd == 0 || tam == 0) return;

	// Peso em gramas por metro. Como a medida eh em milimetros e o peso final deve ser em  KG / metro, precisamos dividir por 1000 por duas vezes ou 1.000.000
	const int pesoGramasPorCm3 = 150;

	unsigned int peso, pesoKgConsumido = (qtd * tam * material->espessura * material->largura * pesoGramasPorCm3) / 1000000UL;

	char sql[500];

	sprintf(sql, "select peso from material where id=%d", material->id);
	DB_Execute(&mainDB, 1, sql);
	if(DB_GetNextRow(&mainDB, 1)>0) {
		// Calcula o novo peso
		peso = atoi(DB_GetData(&mainDB, 1, 0)) - pesoKgConsumido;
		if(peso < 0) peso = 0;

		// Atualiza o banco
		sprintf(sql, "update material set peso=%d where id=%d", peso, material->id);
		DB_Execute(&mainDB, 1, sql);

		// Se o material acabou: Desmarca o material, obrigando a utilizacao de um novo material
		if(peso == 0) {
			material_select(NULL);
		}
	}
}

#define TRF_TEXT_SIZE 10
/*
void ConfigBotoesMaterial(GtkWidget *wdg, gboolean modo)
{
  struct strTask *Task = NULL;
  char txt_id[10];

  TV_GetSelected(wdg, 0, txt_id);
  if(txt_id != NULL)
    Task = GetTask(atoi(txt_id)-1);

  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnExecTarefa"   )), modo);
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnEditarTarefa" )), modo && Task && (Task->Origem == TRF_ORIGEM_MANUAL));
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnRemoverTarefa")), modo && Task && (Task->Origem == TRF_ORIGEM_MANUAL));
}
*/
void CarregaComboLocais(GtkComboBox *cmb)
{
  DB_Execute(&mainDB, 2, "select codigo from local");
  CarregaCombo(&mainDB, cmb, 2, NULL);
}

void InsertMaterial(void)
{
  struct strMaterial *currItem;

  // Aloca memoria para nova tarefa
  currItem = AllocNewMaterial();

  // Salva dados do material
  currItem->id        = atoi(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "mID"      )));
  strcpy(currItem->codigo  , DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "mCod"     )));
  currItem->largura   = atoi(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "largura"  )));
  currItem->espessura = atof(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "espessura")));
  currItem->inUse     = atoi(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "inUse"    )));
  strcpy(currItem->local   , DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "lCod"     )));
  currItem->peso      = atoi(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "peso"     )));
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
  DB_Execute(&mainDB, 1, "select m.id as mID, m.codigo as mCod, m.inUse, m.largura, m.espessura, m.peso, l.codigo as lCod from material as m, local as l where l.id = m.idLocal and m.peso > 0 order by mID");
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

    // Insere uma nova tarefa a partir do registro atual
    InsertMaterial();

    TV_Adicionar(tvw, valores);

    free(valores[0]); // Indice
    free(valores[2]); // Em Uso
    }

//  ConfigBotoesMaterial(GTK_WIDGET(tvw), FALSE);
}

gboolean ChecarMaterial(char *codigo, int largura, float espessura, int peso, char *local)
{
  int bobinaExiste = FALSE;
  char tmp[100], sql[500];
  char *msg = NULL;

  // Limites
  const int   peso_max      = 5500;
  const int   largura_min   = 10 , largura_max   = 500;
  const float espessura_min = 1.0, espessura_max = 5.0;

  // Verifica se bobina ja existe
  sprintf(sql, "select id from material where codigo='%s'", codigo);
  DB_Execute(&mainDB, 1, sql);
  if(DB_GetNextRow(&mainDB, 1) > 0) {
	  bobinaExiste = TRUE;
  }

  // Carrega o ID do local selecionado
  sprintf(sql, "select id from local where codigo='%s'", local);
  DB_Execute(&mainDB, 1, sql);
  DB_GetNextRow(&mainDB, 1);

  if(material_checaDigitoVerificador(codigo) == 0) { // Codigo invalido
    msg = "Codigo de material inválido!";
  } else if(bobinaExiste) { // Bobina ja cadastrada!
    msg = "Esta bobina já foi cadastrada!";
  } else if(largura < largura_min || largura > largura_max) { // Largura deve ser superior a minima
    sprintf(tmp, "A largura deve estar entre %d e %d", largura_min, largura_max);
    msg = tmp;
  } else if(espessura < espessura_min || espessura > espessura_max) {
    sprintf(tmp, "A espessura deve estar entre %0.2f e %0.2f", espessura_min, espessura_max);
    msg = tmp;
  } else if(peso < 1) { // Deve ser múltiplo do passo e maior que zero.
    msg = "O peso deve ser maior que zero!";
  } else if(peso > peso_max) { // Verifica se peso supera o limite
	    sprintf(tmp, "O peso deve ser menor ou igual a %d", peso_max);
	    msg = tmp;
  } else if(DB_GetData(&mainDB, 1, 0) == NULL) {
	    msg = "Local selecionado inexistente!";
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
	"entCadMaterialEspessura",
	"entCadMaterialLargura",
	"entCadMaterialPeso",
    ""
    };
  char *valor[] =
    {
    "---",
    "",
    "",
    "",
    "",
    "",
    };

  GravarValoresWidgets(campos, valor);
}

void IniciarDadosMaterial()
{
	LimparDadosMaterial();

	CarregaComboLocais(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbCadMaterialLocal")));
}

int AplicarMaterial()
{
  char *valores[10], sql[800];

  char *campos[] =
    {
    "lblCadMaterialNumero",
    "entCadMaterialCodigo",
    "entCadMaterialEspessura",
    "entCadMaterialLargura",
    "entCadMaterialPeso",
    "cmbCadMaterialLocal",
    ""
    };

  GtkWidget *dialog;

  if(!LerValoresWidgets(campos, valores))
    return FALSE; // Ocorreu algum erro lendo os campos. Retorna.

// Checa se os dados são válidos.
  if(!ChecarMaterial(valores[1], atol(valores[3]), atof(valores[2]), atol(valores[4]), valores[5]))
    return FALSE;

// Carrega o ID do local selecionado
  sprintf(sql, "select id from local where codigo='%s'", valores[5]);
  printf("Executando comando SQL: %s\n", sql);
  DB_Execute(&mainDB, 2, sql);
  DB_GetNextRow(&mainDB, 2);

  if(atoi(valores[0]) != 0)
    {
	  printf("Editando material\n");
    dialog = gtk_message_dialog_new (NULL,
              GTK_DIALOG_DESTROY_WITH_PARENT,
              GTK_MESSAGE_QUESTION,
              GTK_BUTTONS_YES_NO,
              "Aplicar as alterações a este material?"
              );

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
      {
      sprintf(sql, "update material set codigo='%s', espessura='%f', largura='%d', idLocal='%d', peso='%d' where id='%d'",
              valores[1], atof(valores[2]), atoi(valores[3]), atoi(DB_GetData(&mainDB, 2, 0)), atoi(valores[4]), atoi(valores[0]));

      DB_Execute(&mainDB, 0, sql);
      }

    gtk_widget_destroy (dialog);
    }
  else
    {
  	  printf("Inserindo material\n");
    sprintf(sql, "insert into material (codigo, espessura, largura, idLocal, peso) values ('%s', '%f', '%d', '%d', '%d')",
          valores[1], atof(valores[2]), atoi(valores[3]), atoi(DB_GetData(&mainDB, 2, 0)), atoi(valores[4]));

    printf("Executando comando SQL: %s\n", sql);
    DB_Execute(&mainDB, 0, sql);
    }

  return TRUE;
}

/*** Callbacks ***/

// Funcao que confirma os dados de um material, tanto ao adicionar quanto ao editar
void cbAplicarMaterial(GtkButton *button, gpointer user_data)
{
  if(AplicarMaterial()) {
	  CarregaListaMateriais(GTK_WIDGET(gtk_builder_get_object(builder, "tvwMaterial")));
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
	IniciarDadosMaterial();
	WorkAreaGoTo(NTB_ABA_MATERIAL_ADD);
}
