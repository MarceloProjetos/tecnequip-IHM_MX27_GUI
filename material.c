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

// Retorna o material com o id da tarefa fornecido. Retorna nulo se nao for encontrado
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

// Retorna o material com o id fornecido. Retorna nulo se nao for encontrado
struct strMaterial * GetMaterialByID(int id)
{
  struct strMaterial *Item = lstMaterial;

  while(Item != NULL) {
	  // Se encontrou, retorna o elemento encontrado
	  if(Item->id == id) {
		  return Item;
	  }

	  Item = Item->Next;
  }

  return NULL;
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

// Funcao para copiar uma estrutura de material para outra. Se a flag de Full Copy estiver ativada,
// mesmo variaveis que sao exclusivas de um material serao copiadas (por exemplo: id)
struct strMaterial *material_copiar(struct strMaterial *dst, struct strMaterial *src, int isFullCopy)
{
	// Se nao possui origem, retorna nulo!
	if(src == NULL) return NULL;

	// Se nao possui destino, aloca um novo material!
	if(dst == NULL) {
		dst = AllocNewMaterial();
	}

	strcpy(dst->produto    , src->produto  );
	strcpy(dst->descricao  , src->descricao);
	strcpy(dst->local      , src->local    );
	dst       ->espessura  = src->espessura;
	dst       ->largura    = src->largura;
	dst       ->tipo       = src->tipo;
	dst       ->quantidade = src->quantidade;
	dst       ->tamanho    = src->tamanho;
	dst       ->idTarefa   = src->idTarefa;
	dst       ->peso       = src->peso;

	if(isFullCopy) {
		strcpy(dst->codigo     , src->codigo   );
		dst       ->id  = src->id;
		dst       ->inUse  = src->inUse;
	}

	return dst;
}

double material_CalculaPeso(struct strMaterial *material, unsigned int tamanho)
{
	// Se material for nulo ou tamanho forem zero, nao ha como calcular o peso
	if(material == NULL || tamanho == 0) return 0.0f;

	// Peso em gramas por metro. Como a medida eh em milimetros e o peso final deve ser em  KG / metro, precisamos dividir por 1000 por duas vezes ou 1.000.000
	const double pesoGramasPorCm3 = 7.856;

	return (pesoGramasPorCm3 * tamanho  * material->espessura * material->largura) / 1000000.0;
}

void material_registraConsumo(struct strMaterial *materialConsumido, struct strMaterial *materialProduzido, unsigned int qtd, unsigned int tamPeca, unsigned int tamPerda)
{
	// Se material for nulo, quantidade ou tamanho forem zero, nao ha como registrar o consumo
	if(materialConsumido == NULL || materialProduzido == NULL || qtd == 0 || (tamPeca == 0 && tamPerda == 0)) return;

	double pesoKgProduzido = material_CalculaPeso(materialConsumido, qtd * tamPeca);
	double pesoKgPerda     = material_CalculaPeso(materialConsumido, qtd * tamPerda);
	double pesoKgConsumido = pesoKgProduzido + pesoKgPerda;

	// TODO: Peso nao pode ficar negativo! Por isso consideramos como peso consumido apenas o que restava de bobina.
	// Mas de onde veio esse excesso de material?? Precisamos tratar isso futuramente...
	if(materialConsumido->peso < pesoKgConsumido) {
// Para acompanhar o inicio do monitoramento e afinar, vamos permitir trabalhar com peso negativo
//		pesoKgConsumido = materialConsumido->peso;
		// Recalcula a perda para "fechar a conta". Melhor solucao ate o momento...
//		pesoKgPerda = pesoKgConsumido - pesoKgProduzido;
	}

	// Atualiza as propriedades do material consumido
	materialConsumido->peso       -= pesoKgConsumido;

	// Atualiza as propriedades do material produzido
	materialProduzido->quantidade += qtd;
	materialProduzido->tamanho     = tamPeca;
	materialProduzido->peso       += pesoKgConsumido;

	GravarMaterial(materialConsumido);
	GravarMaterial(materialProduzido);

	// Se o material acabou: Desmarca o material, obrigando a utilizacao de um novo material
	if(materialConsumido->peso <= 0.0) {
// Para acompanhar o inicio do monitoramento e afinar, vamos permitir trabalhar com peso negativo
//		material_select(NULL);
	}

	// Agora enviamos a mensagem informando da producao. Devemos trabalhar nos dados para adequar ao esperado pelo sistema
	// entao utilizamos novas variaveis para isso.
	struct strMaterial matProduto = *materialProduzido, matConsumo = *materialConsumido, matPerda = *materialConsumido;

	// Primeiro adequamos o material produzido.
	// Devemos enviar apenas o que foi produzido nesse momento e nao o total
	matProduto.quantidade = qtd;
	matProduto.peso       = pesoKgProduzido;

	// Agora adequamos o material consumido.
	matConsumo.quantidade = matProduto.quantidade;
	matConsumo.tamanho    = matProduto.tamanho;
	matConsumo.peso       = pesoKgConsumido;

	// Por ultimo, as perdas.
	matPerda  .quantidade = matProduto.quantidade;
	matPerda  .tamanho    = matProduto.tamanho;
	matPerda  .peso       = pesoKgPerda;

	monitor_enviaMsgMatProducao(&matConsumo, &matProduto, &matPerda);
}

// Registra perda de material. Se for informado o tamanho, desconsidera o peso.
void material_registraPerda(struct strMaterial *materialConsumido, unsigned int qtd, unsigned int tamPerda, double pesoPerda)
{
	// Material nulo ou valores zerados! Retorna...
	if(materialConsumido == NULL || ((qtd == 0 || tamPerda == 0) && pesoPerda == 0)) return;

	struct strMaterial *materialPerda = material_copiar(NULL, materialConsumido, TRUE);

	materialPerda->id         = 0;
	materialPerda->quantidade = 0;
	materialPerda->tamanho    = 0;
	materialPerda->inUse      = FALSE;

	if(pesoPerda == 0.0) {
		materialPerda->quantidade = qtd;
		materialPerda->tamanho    = tamPerda;
		materialPerda->peso       = material_CalculaPeso(materialPerda, qtd * tamPerda);
	} else {
		materialPerda->peso = pesoPerda;
	}

	// Ajusta os valores para a mensagem
	if(materialConsumido->tipo != enumTipoEstoque_MateriaPrima) {
		materialConsumido->quantidade -= materialPerda->quantidade;
	}
	materialConsumido->peso -= materialPerda->peso;

	GravarMaterial(materialConsumido);

	// Ajusta os valores para a mensagem
	if(materialConsumido->tipo != enumTipoEstoque_MateriaPrima) {
		materialConsumido->quantidade = qtd;
		materialConsumido->tamanho    = tamPerda;
	}
	materialConsumido->peso       = materialPerda->peso;

	monitor_enviaMsgMatProducao(materialConsumido, NULL, materialPerda);
}

void material_ajustarEstoque(struct strMaterial *material, double peso)
{
	struct strMaterial *materialOriginal;
	// Material nulo ou peso menor que zero! Retorna...
	if(material == NULL || peso < 0.0) return;

	// Faz copia do material original para enviar na mensagem
	materialOriginal = material_copiar(NULL, material, TRUE);

	// Altera o peso e grava
	material->peso = peso;
	GravarMaterial(material);

	// Envia mensagem para o sistema de monitoramento
	monitor_enviaMsgAjusteEstoque(materialOriginal, material);
}

/**
 * Funcao que realiza o ajuste de inventario. Existem 3 operacoes:
 *
 * - Entrada de peca (Entrada por ajuste de inventario): quando por algum motivo houver sobrado uma peca, como no caso de uma peca finalizada manualmente
 * - Saida de peca (Saida por ajuste de inventario): quando for descartada uma ou mais pecas, como o caso de haver erro na furacao, por exemplo...
 * - Peca produzida com tamanho errado: nesse caso havera tanto entrada como saida de peca. Ocorre quando a maquina errar a medida e produzir uma peca com tamanho errado
 * 		mas o registro houver sido feito para o tamanho correto, como numa falha de leitura do encoder. Para isso o operador devera informar uma nova etiqueta para
 * 		registrar as pecas de tamanho diferente ja que elas sao diferentes.
**/
void material_ajustarInventario(struct strMaterial *materialOrigem, struct strMaterial *materialDestino, unsigned int qtd)
{
	// Se ambos forem nulo, nada a fazer...
	if(materialOrigem == NULL && materialDestino == NULL) return;

	if(materialDestino == NULL) { // Primeira situacao: Sem destino, apenas saida!!
		struct strMaterial material;

		materialOrigem->quantidade -= qtd;
		GravarMaterial(materialOrigem);

		material_copiar(&material, materialOrigem, TRUE);
		material.quantidade = qtd;
		material.peso       = material_CalculaPeso(materialOrigem, qtd * materialOrigem->tamanho);

		monitor_enviaMsgAjusteInventario(&material, NULL);
	} else if(materialOrigem == NULL) { // Segunda situacao: Sem origem, apenas entrada!!
		struct strMaterial material;

		materialDestino->quantidade += qtd;
		GravarMaterial(materialDestino);

		material_copiar(&material, materialDestino, TRUE);
		material.quantidade = qtd;
		material.peso       = material_CalculaPeso(materialDestino, qtd * materialDestino->tamanho);

		monitor_enviaMsgAjusteInventario(NULL, &material);
	} else { // Terceira situacao: nenhum nulo e portanto significa divisao de lote. Normalmente tamanho errado de alguma peca!
		struct strMaterial material;

		materialOrigem ->quantidade -= qtd;
		materialDestino->quantidade  = qtd;

		materialDestino->peso = material_CalculaPeso(materialDestino, materialDestino->quantidade * materialDestino->tamanho);
		materialOrigem ->peso = material_CalculaPeso(materialOrigem , materialOrigem ->quantidade * materialOrigem ->tamanho);

		// Grava os materiais atualizados
		GravarMaterial(materialOrigem );
		GravarMaterial(materialDestino);

		// Agora ajusta os valores e envia a mensagem de ajuste do inventario
		material_copiar(&material, materialOrigem, TRUE);
		material.quantidade = qtd;
		material.peso       = material_CalculaPeso(materialOrigem, qtd * materialOrigem->tamanho);

		monitor_enviaMsgAjusteInventario(&material, materialDestino);
	}
}

void ConfigBotoesMaterial(GtkWidget *wdg, struct strMaterial *material)
{
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnMaterialSelect"    )), material != NULL && material->tipo == enumTipoEstoque_MateriaPrima);
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnRegistrarMaterial" )), material != NULL);
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "btnMovimentarMaterial")), material != NULL);
}

void CarregaComboLocais(GtkComboBox *cmb)
{
  DB_Execute(&mainDB, 2, "select codigo from local");
  CarregaCombo(&mainDB, cmb, 2, NULL);
}

void CarregaComboTipos(GtkComboBox *cmb, enum enumTipoEstoque tipo)
{
	// Tipo de estoque
	int idx;
	switch(tipo) {
		default:
		case enumTipoEstoque_MateriaPrima     : idx = 0; break;
		case enumTipoEstoque_ProdutoEmProcesso: idx = 1; break;
		case enumTipoEstoque_Subproduto       : idx = 2; break;
	}

	// Configura o item selecionado para o tipo passado como parametro
	gtk_combo_box_set_active(cmb, idx);
}

struct strMaterial *InsertMaterial(void)
{
  struct strMaterial *currItem;

  // Aloca memoria para nova tarefa
  currItem = AllocNewMaterial();

  // Salva dados do material
  currItem->id         = atoi         (DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "mID"       )));
  strcpy(currItem->codigo   ,          DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "mCod"      )));
  strcpy(currItem->produto  ,          DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "modProduto")));
  strcpy(currItem->descricao,          DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "modNome"   )));
  currItem->tipo       = atoi         (DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "tipo"      )));
  currItem->largura    = atoi         (DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "largura"   )));
  currItem->espessura  = StringToFloat(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "espessura" )));
  currItem->inUse      = atoi         (DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "inUse"     )));
  strcpy(currItem->local    ,          DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "lCod"      )));
  currItem->peso       = StringToFloat(DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "peso"      )));
  currItem->tamanho    = atoi         (DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "tamanho"   )));
  currItem->quantidade = atoi         (DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "quantidade")));
  currItem->idTarefa   = atoi         (DB_GetData(&mainDB, 1, DB_GetFieldNumber(&mainDB, 1, "idTarefa"  )));

  return currItem;
}

void GravarMaterial(struct strMaterial *material)
{
	char sql[800], stringEspessura[100], stringPeso[100];

	// Se material for nulo, retorna sem fazer nada
	if(material == NULL) return;

	// Carrega o ID do local selecionado
	sprintf(sql, "select id from local where codigo='%s'", material->local);
	DB_Execute(&mainDB, 2, sql);
	DB_GetNextRow(&mainDB, 2);

	floatToString(stringEspessura, material->espessura);
	floatToString(stringPeso     , material->peso     );

	if(material->id != 0) {
		sprintf(sql, "update material set codigo='%s', espessura='%s', largura='%d', idLocal='%d', peso='%s', tipo='%d', quantidade='%d', "
			  "tamanho='%d', idTarefa='%d' where id='%d'",
			  material->codigo, stringEspessura, material->largura, atoi(DB_GetData(&mainDB, 2, 0)), stringPeso,
			  material->tipo, material->quantidade, material->tamanho, material->idTarefa, material->id);

		DB_Execute(&mainDB, 0, sql);
	} else {
		sprintf(sql, "insert into material (codigo, espessura, largura, idLocal, peso, tipo, quantidade, tamanho, idTarefa)"
				" values ('%s', '%s', '%d', '%d', '%s', '%d', '%d', '%d', '%d')",
				material->codigo, stringEspessura, material->largura, atoi(DB_GetData(&mainDB, 2, 0)),
				stringPeso, material->tipo, material->quantidade, material->tamanho, material->idTarefa);

		DB_Execute(&mainDB, 0, sql);

		// Agora busca o maior id, ou seja, o id do elemento que acabamos de adicionar para atualizar na estrutura do material
		DB_Execute(&mainDB, 0, "select max(id) from material");
		DB_GetNextRow(&mainDB, 0);
		material->id = atoi(DB_GetData(&mainDB, 0, 0));
	}
}

void CarregaListaMateriais(GtkWidget *tvw)
{
  int i, ItemIndex = 1, qtdLista = 0;
  const int tam = 7;
  char *valores[tam+1];
  struct strMaterial *material;
  enum enumTipoEstoque tipoLista;

  valores[tam] = NULL;

  // Carrega o tipo de material a exibir
  switch(gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbListaMaterialTipo")))) {
  default:
  case 0: tipoLista = enumTipoEstoque_MateriaPrima     ; break;
  case 1: tipoLista = enumTipoEstoque_ProdutoEmProcesso; break;
  case 2: tipoLista = enumTipoEstoque_Subproduto       ; break;
  }

  // Limpa a lista anterior
  ClearMaterials();

  // Exclui os itens atuais
  TV_Limpar(tvw);

  // Carrega os materiais do MySQL
  DB_Execute(&mainDB, 1, "select m.id as mID, m.codigo as mCod, m.inUse, m.espessura, m.largura, m.peso, l.codigo as lCod, m.tamanho, m.quantidade, m.tipo, m.idTarefa, md.codigo as modProduto,"
// Essa a consulta alterada, carregando inclusive bobinas com peso negativo
		  " md.nome as modNome from material as m, local as l, tarefas as t, modelos as md where l.id = m.idLocal and m.idTarefa = t.id and t.id_modelo = md.id and (m.peso != 0 or m.tipo != 1) order by l.id asc, mID desc");
// Essa a consulta original, filtrando pelo peso
//  	  	  " md.nome as modNome from material as m, local as l, tarefas as t, modelos as md where l.id = m.idLocal and m.idTarefa = t.id and t.id_modelo = md.id and (m.peso > 0 or tipo = 3) order by mID");
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
    material = InsertMaterial();

    // Adicionar na lista apenas se for o tipo solicitado
    if(material != NULL && material->tipo == tipoLista && qtdLista < 50) {
    	qtdLista++;
    	TV_Adicionar(tvw, valores);
    }

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
  const double peso_max      = 5500.0;
  const int    largura_min   = 10 , largura_max   = 500;
  const double espessura_min = 1.0, espessura_max = 5.0;

  // Verifica se o material ja existe
  sprintf(sql, "select id from material where codigo='%s' and tipo=%d and id != %d", material.codigo, material.tipo, material.id);
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
	  if(!(material.peso > 0.0)) { // Peso deve ser maior que zero.
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
	CarregaComboTipos (GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbCadMaterialTipo" )), enumTipoEstoque_MateriaPrima);
}

int AplicarMaterial()
{
  int   ret = TRUE;
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
	  case enumTipoEstoque_MateriaPrima     : material.peso = atof(valores[ 4]); break;
	  case enumTipoEstoque_ProdutoEmProcesso: material.peso = atof(valores[10]); break;
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
		  GravarMaterial(&material);
		  monitor_enviaMsgMatCadastro(&material);
	  }
  } else {
	  ret = (*userAplicarMaterial)(material, atoi(valores[7]), FALSE, userData);
	  CarregaListaMateriais(GTK_WIDGET(gtk_builder_get_object(builder, "tvwMaterial")));
  }

  return ret;
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

		// Numero do material (Id no banco)
		if(material->id != 0) {
			sprintf(valor[0], "%d", material->id);
		} else {
			strcpy(valor[0], "---");
		}

		// Codigo do material
		strcpy(valor[1], material->codigo);

		// Espessura em mm
		sprintf(valor[2], "%.2f", material->espessura);

		// Largura em mm
		sprintf(valor[3], "%d", material->largura);

		// Peso em kg (Aba de materia prima)
		sprintf(valor[4], "%f", material->peso);

		// Quantidade
		sprintf(valor[5], "%d", material->quantidade);

		// Tamanho em mm
		sprintf(valor[6], "%d", material->tamanho);

		// Peso em kg (Aba de produto em processo)
		sprintf(valor[7], "%f", material->peso);

		// Id da Tarefa
		sprintf(valor[8], "%d", material->idTarefa);

		/*** Valores carregados. Grava nos objetos da tela! ***/
		GravarValoresWidgets(campos, valor);

		// Desaloca a memoria do array de valores
		for(i = 0; i < size; i++) {
			free(valor[i]);
		}

		// Configura o combo para o tipo solicitado
		CarregaComboTipos(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbCadMaterialTipo")), material->tipo);

		// Configura o combo para o local selecionado
		GtkComboBox *cmb = GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbCadMaterialLocal"));
		gtk_combo_box_set_active(cmb, AchaIndiceCombo(cmb, material->local));
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

void cbListaMaterialTipoAlterado(GtkComboBox *combobox, gpointer user_data)
{
	CarregaListaMateriais(GTK_WIDGET(gtk_builder_get_object(builder, "tvwMaterial")));
}

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
	CarregaComboTipos(GTK_COMBO_BOX(gtk_builder_get_object(builder, "cmbListaMaterialTipo")), enumTipoEstoque_MateriaPrima);
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

int cbMaterialMovimentarFinalizar(struct strMaterial material, int dv, int isCancel, void *data)
{
	struct strMaterial *matOrigem, *matDestino;
	struct strMaterial *materialOriginal = (struct strMaterial *)data;

	// Se a operacao foi cancelada, retorna para a tela inicial
	if(isCancel) {
		WorkAreaGoTo(MaqConfigCurrent->AbaHome);
		return FALSE;
	}

	// Checa se os dados são válidos.
	if(!ChecarMaterial(material, dv, FALSE)) {
		return FALSE;
	}

	// Copiamos os materiais para gerar a mensagem de transferencia
	matOrigem  = material_copiar(NULL, materialOriginal, TRUE);
	matDestino = material_copiar(NULL, materialOriginal, TRUE);

	// Ajusta os valores para a mensagem
	matOrigem ->quantidade = material.quantidade;
	matDestino->quantidade = material.quantidade;

	matOrigem ->peso = material_CalculaPeso(matOrigem , matOrigem ->quantidade * matOrigem ->tamanho);
	matDestino->peso = material_CalculaPeso(matDestino, matDestino->quantidade * matDestino->tamanho);

	strcpy(matDestino->local, material.local);

	// Envia a mensagem informando da transferencia
	monitor_enviaMsgTransferencia(matOrigem, matDestino);

	if(materialOriginal->quantidade != material.quantidade) {
		// Transferencia parcial. Subtrair a quantidade transferida do material original e gravar tambem o material de destino
		materialOriginal->quantidade -= material.quantidade;
		materialOriginal->peso = material_CalculaPeso(materialOriginal, materialOriginal->quantidade * materialOriginal->tamanho);

		// Aqui atualizamos os dados de destino para poder criar a nova etiqueta
		matDestino->id = 0;
		strcpy(matDestino->codigo, material.codigo);
		GravarMaterial(matDestino);
	} else {
		// Local mudou! Como a transferencia eh total, precisamos alterar o local do material original
		strcpy(materialOriginal->local, material.local);
	}

	GravarMaterial(materialOriginal);

	// Material alterado! Lista precisa ser recarregada.
	return TRUE;
}

int cbMaterialMovimentarAplicar(struct strMaterial material, int dv, int isCancel, void *data)
{
	char *msg = NULL;
	struct strMaterial *materialOriginal;

	// Se a operacao foi cancelada, retorna para a tela anterior
	if(isCancel) {
		WorkAreaGoPrevious();
		return FALSE;
	}

	// Checa se os dados são válidos.
	if(!ChecarMaterial(material, dv, FALSE)) {
		return FALSE;
	}

	// Carrega o codigo original para identificar se o codigo foi alterado (o que nao eh permitido) e se o local foi alterado (o que eh esperado).
	materialOriginal = GetMaterialByID(material.id);

	if(strcmp(materialOriginal->codigo, material.codigo) || materialOriginal->tipo != material.tipo) {
		// Codigo mudou! Devemos gerar erro e retornar...
		msg = "Codigo e tipo nao podem ser alterados ao movimentar o material!";
	} else if(materialOriginal->quantidade < material.quantidade) {
		// Tentando transferir quantidade superior a existente...
		msg = "Tentando transferir quantidade superior a existente!";
	} else if(!strcmp(materialOriginal->local, material.local)) {
		// Local nao foi alterado...
		msg = "Escolha o novo local!";
	} else {
		if(materialOriginal->quantidade == material.quantidade) { // Transferencia Total
			return cbMaterialMovimentarFinalizar(material, dv, isCancel, materialOriginal);
		} else { // Transferencia Parcial
			  struct strMaterial *matDestino = material_copiar(NULL, materialOriginal, FALSE);
			  matDestino->idTarefa   = 0;
			  matDestino->quantidade = material.quantidade;
			  strcpy(matDestino->local, material.local);

			  AbrirCadastroMaterial(matDestino, FALSE, FALSE, cbMaterialMovimentarFinalizar, materialOriginal);
		}
	}

	if(msg != NULL) {
		MessageBox(msg);
	}

	// Nada a fazer... Retorna falso pois nao queremos que a tela de cadastro de materiais execute acao nenhuma.
	return FALSE;
}

// Funcao para transferir o material para outro local fisico
void cbMaterialMovimentar(GtkButton *button, gpointer user_data)
{
	char val[20];
	struct strMaterial *material;

	// Identifica o material selecionado
	TV_GetSelected(GTK_WIDGET(gtk_builder_get_object(builder, "tvwMaterial")), 0, val);

	// E entao seleciona esse material
	material = GetMaterial(atoi(val) - 1);

	AbrirCadastroMaterial(material, TRUE, TRUE, cbMaterialMovimentarAplicar, NULL);
}

// Funcao para registrar peca perdida / sobrando / tamanho errado em um material
void cbMaterialRegistrar(GtkButton *button, gpointer user_data)
{
	char val[20];
	struct strMaterial *material;

	// Identifica o material selecionado
	TV_GetSelected(GTK_WIDGET(gtk_builder_get_object(builder, "tvwMaterial")), 0, val);

	// E entao seleciona esse material
	material = GetMaterial(atoi(val) - 1);

	AbrirRegistrarPeca(material);
}
