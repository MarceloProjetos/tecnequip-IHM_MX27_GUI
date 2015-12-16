#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "defines.h"
#include "GtkUtils.h"

#include <jsonParser.h>
#include <jsonQueue.h>

// Strings de conexao com o servidor e as filas / topicos
#define URI_BROKER         "tcp://127.0.0.1:61616"
#define URI_QUEUE_ESTADO   "IHM-STATUS"
#define URI_QUEUE_MATERIAL "IHM-MATERIAL-MOVIMENTACAO"

// Nomes dos objetos da mensagem
#define MSG_NAME_MODE                   "modo"
#define MSG_NAME_DATETIME               "datahora"
#define MSG_NAME_USER                   "operador"
#define MSG_NAME_PARAM                  "parametros"
#define MSG_NAME_STATUS_TORQUE_MAX      "torque_maximo"
#define MSG_NAME_STATUS_CURRENT_MAX     "corrente_maxima"
#define MSG_NAME_STATUS_TEMPERATURE_MAX "temperatura_maxima"
#define MSG_NAME_STATUS_TORQUE_AVG      "torque_medio"
#define MSG_NAME_STATUS_CURRENT_AVG     "corrente_media"
#define MSG_NAME_STATUS_TEMPERATURE_AVG "temperatura_media"
#define MSG_NAME_STATUS_SEQUENCE        "sequencia"
#define MSG_NAME_STATUS_UP_TIME         "tempo"
#define MSG_NAME_STATUS_BUILD           "versao"
#define MSG_NAME_PARAM_MAQ_NAME         "ihm"

#define MSG_NAME_VALOR_MEDIDA           "medida"
#define MSG_NAME_VALOR_UNIDADE          "unidade"
#define MSG_NAME_VALOR_VALOR            "valor"

#define MSG_NAME_MATCAD_MATERIAL        "materiais"
#define MSG_NAME_MATCAD_CODIGO          "codigo"
#define MSG_NAME_MATCAD_MOVIMENTACAO    "movimentacao"
#define MSG_NAME_MATCAD_DESCRICAO       "descricao"
#define MSG_NAME_MATCAD_TRACKING        "lote"
#define MSG_NAME_MATCAD_TRACKING_TYPE   "tipo"
#define MSG_NAME_MATCAD_TRACKING_NUMBER "numero"
#define MSG_NAME_MATCAD_INUSE           "emUso"
#define MSG_NAME_MATCAD_MEDIDA          "medidas"
#define MSG_NAME_MATCAD_LARGURA         "largura"
#define MSG_NAME_MATCAD_ESPESSURA       "espessura"
#define MSG_NAME_MATCAD_LOCAL           "local"
#define MSG_NAME_MATCAD_PESO            "peso"
#define MSG_NAME_MATCAD_TAMANHO         "comprimento"
#define MSG_NAME_MATCAD_QUANTIDADE      "quantidade"

// Fila STATUS do sistema de monitoramento
static struct jsonQueue *jqMonitorStatus = NULL;

// Fila do sistema de controle de materiais
static struct jsonQueue *jqMaterial = NULL;

// Estrutura, Enumeracoes e Uniao que juntas armazenam uma variavel de qualquer tipo
enum enumValueType {
	enumValueType_Long = 0,
	enumValueType_Float,
	enumValueType_String
};

enum enumValueUnit {
	enumValueUnit_Unit = 0,
	enumValueUnit_Kg,
	enumValueUnit_Mm,
	enumValueUnit_Celsius,
	enumValueUnit_Amper,
	enumValueUnit_Percent
};

union uniValor {
	long    lVal;
	double  fVal;
	char   *sVal;
};

struct strValor {
	char                *nome;
	enum  enumValueType  type;
	enum  enumValueUnit  unit;
	union uniValor       valor;
};

// Enumeracao com os tipos de movimentacao de material
enum enumMaterialTipoMovimentacao {
	enumMaterialTipoMovimentacao_ConsumoProprio     = 0,
	enumMaterialTipoMovimentacao_SaidaMovEntreDep   ,
	enumMaterialTipoMovimentacao_EntradaMovEntreDep ,
	enumMaterialTipoMovimentacao_Devolucao          ,
	enumMaterialTipoMovimentacao_EntradaAjuste      ,
	enumMaterialTipoMovimentacao_Expedicao          ,
	enumMaterialTipoMovimentacao_Perda              ,
	enumMaterialTipoMovimentacao_Producao           ,
	enumMaterialTipoMovimentacao_Recebimento        ,
	enumMaterialTipoMovimentacao_SaidaAjuste        ,
	enumMaterialTipoMovimentacao_AjusteInvOriginal  ,
	enumMaterialTipoMovimentacao_AjusteInvNovo      ,
};

// Estrutura com os dados de monitoramento da maquina
static struct {

	struct  {

		unsigned int  opmode;
		char user[MAX_USERNAME_SIZE + 1];

		struct {

			struct {
				long max;
				long sum;
				long count;
			} torque;

			struct {
				long max;
				long sum;
				long count;
			} current;

			struct {
				long max;
				long sum;
				long count;
			} temperature;

			long startTime;
			long sequence;

		} status;

		struct {
			  char ip_ihm[NI_MAXHOST];
		} param;
	} estado;

} monitor;

// Funcao que inicializa a estrutura de status do monitor
void monitor_Clear_Status(void)
{
	// Inicializa o torque
	monitor.estado.status.torque     .max   = 0;
	monitor.estado.status.torque     .sum   = 0;
	monitor.estado.status.torque     .count = 0;

	// Inicializa a corrente
	monitor.estado.status.current    .max   = 0;
	monitor.estado.status.current    .sum   = 0;
	monitor.estado.status.current    .count = 0;

	// Inicializa a temperatura
	monitor.estado.status.temperature.max   = 0;
	monitor.estado.status.temperature.sum   = 0;
	monitor.estado.status.temperature.count = 0;
}

// Funcao que inicializa a estrutura de status do monitor
void monitor_Set_Status(long torque, long current, long temperature)
{
	// Atualiza o torque
	if(monitor.estado.status.torque.count == 0 || monitor.estado.status.torque.max < torque) {
		monitor.estado.status.torque.max = torque;
	}

	monitor.estado.status.torque.count++;
	monitor.estado.status.torque.sum += torque;

	// Atualiza a corrente
	if(monitor.estado.status.current.count == 0 || monitor.estado.status.current.max < current) {
		monitor.estado.status.current.max = current;
	}

	monitor.estado.status.current.count++;
	monitor.estado.status.current.sum += current;

	// Atualiza a temperatura
	if(monitor.estado.status.temperature.count == 0 || monitor.estado.status.temperature.max < temperature) {
		monitor.estado.status.temperature.max = temperature;
	}

	monitor.estado.status.temperature.count++;
	monitor.estado.status.temperature.sum += temperature;
}

// Funcao que inicializa a estrutura de status do monitor
void monitor_Set_User(char *user)
{
	strcpy(monitor.estado.user, user);
}

void monitor_Set_OpMode(unsigned int opmode)
{
	// Envia o estado atual
	monitor_SendEstado();

	// Atualiza o modo
	monitor.estado.status.startTime = time(NULL);
	monitor.estado.opmode = opmode;

	// Envia a mensagem com o estado atualizado
	monitor_SendEstado();
}

// Funcao para inicializar a estrutura de monitoramento
void monitor_Init(void)
{
	monitor.estado.opmode  = MAQ_STATUS_DESLIGADA;
	monitor.estado.user[0] = 0;

	monitor_Clear_Status();

	monitor.estado.status.startTime = system_Shutdown;
	monitor.estado.status.sequence  = 0;

	MaqGetIpAddress("eth0", monitor.estado.param.ip_ihm);

	// Cria fila de Status
	jqMonitorStatus = jsonQueue_New();
	jsonQueue_SetBroker  (jqMonitorStatus, URI_BROKER      , TRUE );
	jsonQueue_SetProducer(jqMonitorStatus, URI_QUEUE_ESTADO, FALSE);
	jsonQueue_SetConsumer(jqMonitorStatus, ""              , TRUE );

	// Cria fila de Materiais
	jqMaterial = jsonQueue_New();
	jsonQueue_SetBroker  (jqMaterial, URI_BROKER        , TRUE );
	jsonQueue_SetProducer(jqMaterial, URI_QUEUE_MATERIAL, FALSE);
	jsonQueue_SetConsumer(jqMaterial, ""                , TRUE );

	// Inicia as filas
	jsonQueue_Start(jqMonitorStatus);
	jsonQueue_Start(jqMaterial);
}

/*** Funcoes para montar as mensagens ***/

// Funcao que retorna os dados de identificacao de uma IHM na mensagem
struct jsonMessageElem * monitor_getMsgHeader(void)
{
	struct jsonMessageElem *jMsg, *rootMsg;

	/*** Agora montamos a mensagem ***/

	// Elemento ID da Maquina
	rootMsg = jsonMessage_ElemNewFull(MSG_NAME_PARAM_MAQ_NAME     , FALSE);
	jsonMessage_DataSetString(jsonMessage_ElemGetData(rootMsg), MaqConfigCurrent->ID);

	// Elemento DataHora
	char tmp[100];
	time_t now;

	now = time(NULL);
	strftime (tmp, 100, "%Y-%m-%dT%H:%M:%S%z", localtime(&now));
	jMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_DATETIME, FALSE));
	jsonMessage_DataSetString(jsonMessage_ElemGetData(jMsg), tmp);

	// Elemento User
	jMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_USER, FALSE));
	jsonMessage_DataSetString(jsonMessage_ElemGetData(jMsg), (monitor.estado.user == NULL) ? "" : monitor.estado.user);

	return rootMsg;
}

// Funcao que retorna os dados de identificacao de uma IHM na mensagem
struct jsonMessageElem * monitor_getMsgValue(struct strValor valor)
{
	struct jsonMessageElem *jMsg, *rootMsg;

	/*** Agora montamos a mensagem ***/

	// Elemento com o nome do valor
	rootMsg = jsonMessage_ElemNewFull(MSG_NAME_VALOR_MEDIDA, FALSE);
	jsonMessage_DataSetString(jsonMessage_ElemGetData(rootMsg), valor.nome);

	// Elemento com a unidade utilizada
	char *unit;
	jMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_VALOR_UNIDADE, FALSE));
	switch(valor.unit) {
	default:
	case enumValueUnit_Unit   : unit = "un"; break;
	case enumValueUnit_Percent: unit = "%" ; break;
	case enumValueUnit_Mm     : unit = "mm"; break;
	case enumValueUnit_Kg     : unit = "kg"; break;
	case enumValueUnit_Celsius: unit = "C" ; break;
	case enumValueUnit_Amper  : unit = "A" ; break;
	}
	jsonMessage_DataSetString(jsonMessage_ElemGetData(jMsg), unit);

	// Agora o elemento com o valor em si
	jMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_VALOR_VALOR, FALSE));
	switch(valor.type) {
		case enumValueType_Long  : jsonMessage_DataSetNumber(jsonMessage_ElemGetData(jMsg), valor.valor.lVal); break;
		case enumValueType_Float : jsonMessage_DataSetReal  (jsonMessage_ElemGetData(jMsg), valor.valor.fVal); break;
		case enumValueType_String: jsonMessage_DataSetString(jsonMessage_ElemGetData(jMsg), valor.valor.sVal); break;
	}

	return rootMsg;
}

// Funcao que retorna os dados de um material na mensagem
struct jsonMessageElem * monitor_getMsgMaterial(struct strMaterial *material, enum enumMaterialTipoMovimentacao tipoMov)
{
	char tmp[100];
	struct strValor valor;
	struct jsonMessageElem *jMsg, *rootMsg, *parentMsg;

	// Fila nao existe!! Retorna...
	if(material == NULL) return NULL;

	/*** Agora montamos a mensagem ***/

	// Elemento Movimentacao
	char *tmpMov;
	rootMsg = jsonMessage_ElemNewFull(MSG_NAME_MATCAD_MOVIMENTACAO, FALSE);
	switch(tipoMov) {
	case enumMaterialTipoMovimentacao_ConsumoProprio    : tmpMov = "CO"; break;
	case enumMaterialTipoMovimentacao_SaidaMovEntreDep  : tmpMov = "D1"; break;
	case enumMaterialTipoMovimentacao_EntradaMovEntreDep: tmpMov = "D2"; break;
	case enumMaterialTipoMovimentacao_Devolucao         : tmpMov = "DE"; break;
	case enumMaterialTipoMovimentacao_EntradaAjuste     : tmpMov = "EI"; break;
	case enumMaterialTipoMovimentacao_Expedicao         : tmpMov = "EX"; break;
	case enumMaterialTipoMovimentacao_Perda             : tmpMov = "PE"; break;
	case enumMaterialTipoMovimentacao_Producao          : tmpMov = "PR"; break;
	case enumMaterialTipoMovimentacao_Recebimento       : tmpMov = "RE"; break;
	case enumMaterialTipoMovimentacao_SaidaAjuste       : tmpMov = "SI"; break;
	case enumMaterialTipoMovimentacao_AjusteInvOriginal : tmpMov = "A1"; break;
	case enumMaterialTipoMovimentacao_AjusteInvNovo     : tmpMov = "A2"; break;
	}
	jsonMessage_DataSetString(jsonMessage_ElemGetData(rootMsg), tmpMov);

	// Elemento Numero de Rastreamento
	parentMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_MATCAD_TRACKING, FALSE));

	sprintf(tmp, "%02d", material->tipo);
	jMsg    = jsonMessage_ElemNewFull(MSG_NAME_MATCAD_TRACKING_TYPE, FALSE);
	jsonMessage_DataSetString(jsonMessage_ElemGetData(jMsg), tmp);
	jsonMessage_DataSetObject(jsonMessage_ElemGetData(parentMsg), jMsg);

	jMsg = jsonMessage_ElemAppend(jMsg, jsonMessage_ElemNewFull(MSG_NAME_MATCAD_TRACKING_NUMBER, FALSE));
	jsonMessage_DataSetNumber(jsonMessage_ElemGetData(jMsg), atol(material->codigo));

	// Elemento Codigo
	jMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_MATCAD_CODIGO, FALSE));
	jsonMessage_DataSetString(jsonMessage_ElemGetData(jMsg), material->produto);

	// Elemento Descricao
	jMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_MATCAD_DESCRICAO, FALSE));
	jsonMessage_DataSetString(jsonMessage_ElemGetData(jMsg), material->descricao);

	// Elemento Em Uso
	jMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_MATCAD_INUSE, FALSE));
	jsonMessage_DataSetBoolean(jsonMessage_ElemGetData(jMsg), material->inUse);

	// Elemento Local
	jMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_MATCAD_LOCAL, FALSE));
	jsonMessage_DataSetString(jsonMessage_ElemGetData(jMsg), material->local);

	// Elemento pai do array com todas as medidas
	jMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_MATCAD_MEDIDA, TRUE));

	// Elemento Largura
	valor.nome       = MSG_NAME_MATCAD_LARGURA;
	valor.type       = enumValueType_Long;
	valor.unit       = enumValueUnit_Mm;
	valor.valor.lVal = material->largura;
	jsonMessage_DataSetObject(jsonMessage_ElemGetData(jMsg), monitor_getMsgValue(valor));

	// Elemento Espessura
	valor.nome       = MSG_NAME_MATCAD_ESPESSURA;
	valor.type       = enumValueType_Float;
	valor.unit       = enumValueUnit_Mm;
	valor.valor.fVal = material->espessura;
	jsonMessage_DataSetObject(jsonMessage_ElemAppendDataNew(jMsg), monitor_getMsgValue(valor));

	// Elemento Quantidade
	valor.nome       = MSG_NAME_MATCAD_QUANTIDADE;
	valor.type       = enumValueType_Long;
	valor.unit       = enumValueUnit_Unit;
	valor.valor.lVal = material->quantidade;
	jsonMessage_DataSetObject(jsonMessage_ElemAppendDataNew(jMsg), monitor_getMsgValue(valor));

	// Elemento Tamanho
	valor.nome       = MSG_NAME_MATCAD_TAMANHO;
	valor.type       = enumValueType_Long;
	valor.unit       = enumValueUnit_Mm;
	valor.valor.lVal = material->tamanho;
	jsonMessage_DataSetObject(jsonMessage_ElemAppendDataNew(jMsg), monitor_getMsgValue(valor));

	// Elemento Peso
	valor.nome       = MSG_NAME_MATCAD_PESO;
	valor.type       = enumValueType_Float;
	valor.unit       = enumValueUnit_Kg;
	valor.valor.fVal = material->peso;
	jsonMessage_DataSetObject(jsonMessage_ElemAppendDataNew(jMsg), monitor_getMsgValue(valor));

	return rootMsg;
}

/*** Funcoes para comunicação com o sistema de monitoramento ***/

// Funcao para montar a mensagem de estado e enviar para a fila
void monitor_SendEstado(void)
{
	char buf[1024];
	struct strValor valor;
	long avg_torque = 0, avg_current = 0, avg_temperature = 0;
	struct jsonMessageElem *jMsg, *rootMsg;

	// Fila nao existe!! Retorna...
	if(jqMonitorStatus == NULL) return;

	/*** Primeiro calculamos os dados ***/

	if(monitor.estado.status.torque.count > 0) {
		avg_torque = monitor.estado.status.torque.sum / monitor.estado.status.torque.count;
	}

	if(monitor.estado.status.current.count > 0) {
		avg_current = monitor.estado.status.current.sum / monitor.estado.status.current.count;
	}

	if(monitor.estado.status.temperature.count > 0) {
		avg_temperature = monitor.estado.status.temperature.sum / monitor.estado.status.temperature.count;
	}

	/*** Verificando horario inicial do estado. Nunca pode ser de um dia anterior ao atual! ***/
	time_t startOfDay, now = time(NULL);
	struct tm *tmNow = localtime(&now);
	if(tmNow != NULL) {
		// Zera a hora pra receber os segundos referentes ao inicio do dia
		tmNow->tm_hour = 0;
		tmNow->tm_min  = 0;
		tmNow->tm_sec  = 0;

		startOfDay = mktime(tmNow);

		if(monitor.estado.status.startTime < startOfDay) {
			monitor.estado.status.startTime = startOfDay;
		}
	}

	/*** Agora montamos a mensagem ***/

	// Recebe o cabecalho da mensagem: ID da maquina e usuario conectado
	rootMsg = monitor_getMsgHeader();

	// Elemento Operation Mode
	jMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_MODE, FALSE));
	jsonMessage_DataSetNumber(jsonMessage_ElemGetData(jMsg), monitor.estado.opmode);

	// Elemento Sequencia
	jMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_STATUS_SEQUENCE, FALSE));
	jsonMessage_DataSetNumber(jsonMessage_ElemGetData(jMsg), monitor.estado.status.sequence++);

	// Elemento Uptime
	long time_diff = now - monitor.estado.status.startTime;
//	if(time_diff < 0) time_diff = 0; // Nao permite enviar mensagem com tempo negativo

	jMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_STATUS_UP_TIME, FALSE));
	jsonMessage_DataSetNumber(jsonMessage_ElemGetData(jMsg), time_diff);
	monitor.estado.status.startTime = now;

	// Elemento Versao
	sprintf(buf, "%d de %s", BUILD_NUMBER, BUILD_DATE);
	jMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_STATUS_BUILD, FALSE));
	jsonMessage_DataSetString(jsonMessage_ElemGetData(jMsg), buf);

	// Elemento Parametros
	jMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_PARAM, TRUE));

	// Elemento Torque
	valor.nome       = MSG_NAME_STATUS_TORQUE_MAX;
	valor.type       = enumValueType_Long;
	valor.unit       = enumValueUnit_Percent;
	valor.valor.lVal = monitor.estado.status.torque.max;
	jsonMessage_DataSetObject(jsonMessage_ElemGetData(jMsg), monitor_getMsgValue(valor));

	valor.nome       = MSG_NAME_STATUS_TORQUE_AVG;
	valor.valor.lVal = avg_torque;
	jsonMessage_DataSetObject(jsonMessage_ElemAppendDataNew(jMsg), monitor_getMsgValue(valor));

	// Elemento Corrente
	valor.nome       = MSG_NAME_STATUS_CURRENT_MAX;
	valor.type       = enumValueType_Long;
	valor.unit       = enumValueUnit_Amper;
	valor.valor.lVal = monitor.estado.status.current.max;
	jsonMessage_DataSetObject(jsonMessage_ElemAppendDataNew(jMsg), monitor_getMsgValue(valor));

	valor.nome       = MSG_NAME_STATUS_CURRENT_AVG;
	valor.valor.lVal = avg_current;
	jsonMessage_DataSetObject(jsonMessage_ElemAppendDataNew(jMsg), monitor_getMsgValue(valor));

	// Elemento Corrente
	valor.nome       = MSG_NAME_STATUS_TEMPERATURE_MAX;
	valor.type       = enumValueType_Long;
	valor.unit       = enumValueUnit_Celsius;
	valor.valor.lVal = monitor.estado.status.temperature.max;
	jsonMessage_DataSetObject(jsonMessage_ElemAppendDataNew(jMsg), monitor_getMsgValue(valor));

	valor.nome       = MSG_NAME_STATUS_TEMPERATURE_AVG;
	valor.valor.lVal = avg_temperature;
	jsonMessage_DataSetObject(jsonMessage_ElemAppendDataNew(jMsg), monitor_getMsgValue(valor));

	// Mensagem pronta! Agora a enviamos
	jsonQueue_PutMessage(jqMonitorStatus, rootMsg);
	jsonQueue_Work(jqMonitorStatus);
}

// Envia a mensagem informando do cadastro de um material
void monitor_enviaMsgMatCadastro(struct strMaterial *material)
{
	struct jsonMessageElem *jMsg, *rootMsg;

	// Fila nao existe!! Retorna...
	if(jqMaterial == NULL || material == NULL) return;

	/*** Agora montamos a mensagem ***/

	// Recebe o cabecalho da mensagem: ID da maquina e usuario conectado
	rootMsg = monitor_getMsgHeader();

	// Cria o objeto com array de materiais
	jMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_MATCAD_MATERIAL, TRUE));

	// Agora anexa o material
	jsonMessage_DataSetObject(jsonMessage_ElemGetData(jMsg), monitor_getMsgMaterial(material, enumMaterialTipoMovimentacao_Recebimento));

	// Mensagem pronta! Agora a enviamos
	jsonQueue_PutMessage(jqMaterial, rootMsg);
	jsonQueue_Work(jqMaterial);
}

// Envia a mensagem informando do cadastro de um material
void monitor_enviaMsgMatProducao(struct strMaterial *material, struct strMaterial *materialProd, struct strMaterial *materialPerda)
{
	struct jsonMessageElem *rootMsg, *parentMsg;

	// Fila nao existe!! Retorna...
	if(jqMaterial == NULL || material == NULL) return;

	/*** Agora montamos a mensagem ***/

	// Recebe o cabecalho da mensagem: ID da maquina e usuario conectado
	rootMsg = monitor_getMsgHeader();

	// Cria o objeto com array de materiais
	parentMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_MATCAD_MATERIAL, TRUE));

	// Agora anexa o material consumido
	jsonMessage_DataSetObject(jsonMessage_ElemGetData(parentMsg), monitor_getMsgMaterial(material, enumMaterialTipoMovimentacao_ConsumoProprio));

	// E entao anexa o material produzido
	if(materialProd != NULL && materialProd->peso > 0.0f) {
		jsonMessage_DataSetObject(jsonMessage_ElemAppendDataNew(parentMsg), monitor_getMsgMaterial(materialProd, enumMaterialTipoMovimentacao_Producao));
	}

	// Por fim anexa o material perdido
	if(materialPerda != NULL && materialPerda->peso > 0.0f) {
		jsonMessage_DataSetObject(jsonMessage_ElemAppendDataNew(parentMsg), monitor_getMsgMaterial(materialPerda, enumMaterialTipoMovimentacao_Perda));
	}

	// Mensagem pronta! Agora a enviamos
	jsonQueue_PutMessage(jqMaterial, rootMsg);
	jsonQueue_Work(jqMaterial);
}

// Envia a mensagem informando do ajuste de um material (correcao de pecas a menor, maior, tamanho errado...)
void monitor_enviaMsgAjusteInventario(struct strMaterial *materialSaida, struct strMaterial *materialEntrada)
{
	struct jsonMessageElem *rootMsg, *parentMsg;

	// Fila nao existe!! Retorna...
	if(jqMaterial == NULL || (materialSaida == NULL && materialEntrada == NULL)) return;

	/*** Agora montamos a mensagem ***/

	// Recebe o cabecalho da mensagem: ID da maquina e usuario conectado
	rootMsg = monitor_getMsgHeader();

	// Cria o objeto com array de materiais
	parentMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_MATCAD_MATERIAL, TRUE));

	if(materialSaida == NULL) {
		// Anexa apenas o material de entrada
		jsonMessage_DataSetObject(jsonMessage_ElemGetData(parentMsg), monitor_getMsgMaterial(materialEntrada, enumMaterialTipoMovimentacao_EntradaAjuste));
	} else {
		// Agora anexa o material de saida
		jsonMessage_DataSetObject(jsonMessage_ElemGetData(parentMsg), monitor_getMsgMaterial(materialSaida, enumMaterialTipoMovimentacao_SaidaAjuste));

		// E entao anexa o material de entrada (Se nao for nulo)
		if(materialEntrada != NULL) {
			jsonMessage_DataSetObject(jsonMessage_ElemAppendDataNew(parentMsg), monitor_getMsgMaterial(materialEntrada, enumMaterialTipoMovimentacao_EntradaAjuste));
		}
	}

	// Mensagem pronta! Agora a enviamos
	jsonQueue_PutMessage(jqMaterial, rootMsg);
	jsonQueue_Work(jqMaterial);
}

// Envia a mensagem informando do ajuste de estoque (quando houver diferenca de peso entre o calculado e real)
void monitor_enviaMsgAjusteEstoque(struct strMaterial *materialOriginal, struct strMaterial *material)
{
	struct jsonMessageElem *rootMsg, *parentMsg;

	// Fila nao existe ou material nulo!! Retorna...
	if(jqMaterial == NULL || materialOriginal == NULL || material == NULL) return;

	/*** Agora montamos a mensagem ***/

	// Recebe o cabecalho da mensagem: ID da maquina e usuario conectado
	rootMsg = monitor_getMsgHeader();

	// Cria o objeto com array de materiais
	parentMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_MATCAD_MATERIAL, TRUE));

	// Anexa o material original
	jsonMessage_DataSetObject(jsonMessage_ElemGetData      (parentMsg), monitor_getMsgMaterial(materialOriginal, enumMaterialTipoMovimentacao_AjusteInvOriginal));

	// Anexa o material ajustado
	jsonMessage_DataSetObject(jsonMessage_ElemAppendDataNew(parentMsg), monitor_getMsgMaterial(material        , enumMaterialTipoMovimentacao_AjusteInvNovo    ));

	// Mensagem pronta! Agora a enviamos
	jsonQueue_PutMessage(jqMaterial, rootMsg);
	jsonQueue_Work(jqMaterial);
}

// Envia a mensagem de movimentacao de materiais entre depositos (localizacao fisica)
void monitor_enviaMsgTransferencia(struct strMaterial *materialOrigem, struct strMaterial *materialDestino)
{
	struct jsonMessageElem *rootMsg, *parentMsg;

	// Fila nao existe ou material nulo!! Retorna...
	if(jqMaterial == NULL || materialOrigem == NULL || materialDestino == NULL) return;

	/*** Agora montamos a mensagem ***/

	// Recebe o cabecalho da mensagem: ID da maquina e usuario conectado
	rootMsg = monitor_getMsgHeader();

	// Cria o objeto com array de materiais
	parentMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_MATCAD_MATERIAL, TRUE));

	// Anexa o material de origem
	jsonMessage_DataSetObject(jsonMessage_ElemGetData      (parentMsg), monitor_getMsgMaterial(materialOrigem , enumMaterialTipoMovimentacao_SaidaMovEntreDep));

	// Anexa o material de destino
	jsonMessage_DataSetObject(jsonMessage_ElemAppendDataNew(parentMsg), monitor_getMsgMaterial(materialDestino, enumMaterialTipoMovimentacao_EntradaMovEntreDep));

	// Mensagem pronta! Agora a enviamos
	jsonQueue_PutMessage(jqMaterial, rootMsg);
	jsonQueue_Work(jqMaterial);
}
