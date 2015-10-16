#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "defines.h"
#include "GtkUtils.h"

#include <jsonParser.h>
#include <jsonQueue.h>

// Strings de conexao com o servidor e as filas / topicos
#define URI_BROKER       "tcp://127.0.0.1:61616"
#define URI_QUEUE_ESTADO "status"

// Nomes dos objetos da mensagem
#define MSG_NAME_MODE               "modo"
#define MSG_NAME_USER               "usuario"
#define MSG_NAME_STATUS             "status"
#define MSG_NAME_PARAM              "parametros"
#define MSG_NAME_STATUS_TORQUE      "torque"
#define MSG_NAME_STATUS_CURRENT     "corrente"
#define MSG_NAME_STATUS_TEMPERATURE "temperatura"
#define MSG_NAME_STATUS_UP_TIME     "uptime"
#define MSG_NAME_PARAM_IPIHM        "ip_ihm"
#define MSG_NAME_PARAM_IPCLP        "ip_clp"
#define MSG_NAME_PARAM_MAQ_NAME     "maquina"

// Fila STATUS do sistema de monitoramento
static struct jsonQueue *jqMonitorStatus = NULL;

// Estrutura com os dados de monitoramento da maquina
static struct {

	struct  {

		int   opmode;
		char *user;

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

// Funcao para inicializar a estrutura de monitoramento
void monitor_Init(void)
{
	monitor.estado.opmode = MAQ_STATUS_PARADA;
	monitor.estado.user   = NULL;

	monitor_Clear_Status();

	monitor.estado.status.startTime = time(NULL);

	// Cria fila de Status
	jqMonitorStatus = jsonQueue_New();
	jsonQueue_SetBroker  (jqMonitorStatus, URI_BROKER      , TRUE );
	jsonQueue_SetProducer(jqMonitorStatus, URI_QUEUE_ESTADO, FALSE);
	jsonQueue_SetConsumer(jqMonitorStatus, ""              , TRUE );

	MaqGetIpAddress("eth0", monitor.estado.param.ip_ihm);

	// Inicia a fila
	jsonQueue_Start(jqMonitorStatus);
}

/*** Funcoes para comunicação com o sistema de monitoramento ***/

// Funcao para montar a mensagem de estado e enviar para a fila
void monitor_SendEstado(void)
{
	long avg_torque = 0, avg_current = 0, avg_temperature = 0;
	struct jsonMessageElem *jMsg, *rootMsg, *parentMsg;

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

	/*** Agora montamos a mensagem ***/

	// Elemento Operation Mode
	rootMsg = jsonMessage_ElemNewFull(MSG_NAME_MODE, FALSE);
	jsonMessage_DataSetNumber(jsonMessage_ElemGetData(rootMsg), monitor.estado.opmode);

	// Elemento User
	parentMsg = jsonMessage_ElemAppend(rootMsg, jsonMessage_ElemNewFull(MSG_NAME_USER, FALSE));
	jsonMessage_DataSetString(jsonMessage_ElemGetData(parentMsg), monitor.estado.user);

	// Elemento Status
	parentMsg = jsonMessage_ElemAppend(parentMsg, jsonMessage_ElemNewFull(MSG_NAME_STATUS, FALSE));

	// Elemento Torque
	jMsg = jsonMessage_ElemNewFull(MSG_NAME_STATUS_TORQUE, TRUE);
	jsonMessage_DataSetNumber(jsonMessage_ElemGetData(jMsg), monitor.estado.status.torque.max);
	jsonMessage_DataSetNumber(jsonMessage_ElemAppendDataNew(jMsg), avg_torque);

	// Configura o elemento de torque como o dado de status
	jsonMessage_DataSetObject(jsonMessage_ElemGetData(parentMsg), jMsg);

	// Elemento Corrente
	jMsg = jsonMessage_ElemAppend(jMsg, jsonMessage_ElemNewFull(MSG_NAME_STATUS_CURRENT, TRUE));
	jsonMessage_DataSetNumber(jsonMessage_ElemGetData(jMsg), monitor.estado.status.current.max);
	jsonMessage_DataSetNumber(jsonMessage_ElemAppendDataNew(jMsg), avg_current);

	// Elemento Temperatura
	jMsg = jsonMessage_ElemAppend(jMsg, jsonMessage_ElemNewFull(MSG_NAME_STATUS_TEMPERATURE, TRUE));
	jsonMessage_DataSetNumber(jsonMessage_ElemGetData(jMsg), monitor.estado.status.temperature.max);
	jsonMessage_DataSetNumber(jsonMessage_ElemAppendDataNew(jMsg), avg_temperature);

	// Elemento Uptime
	jMsg = jsonMessage_ElemAppend(jMsg, jsonMessage_ElemNewFull(MSG_NAME_STATUS_UP_TIME, FALSE));
	jsonMessage_DataSetNumber(jsonMessage_ElemGetData(jMsg), time(NULL) - monitor.estado.status.startTime);

	// Elemento Parametros
	parentMsg = jsonMessage_ElemAppend(parentMsg, jsonMessage_ElemNewFull(MSG_NAME_PARAM, FALSE));

	if(MaqConfigCurrent != NULL) {
		// Elemento IP_IHM
		jMsg = jsonMessage_ElemNewFull(MSG_NAME_PARAM_IPIHM, FALSE);
		jsonMessage_DataSetString(jsonMessage_ElemGetData(jMsg), monitor.estado.param.ip_ihm);

		// Configura o elemento de IP_IHM como o dado de parametros
		jsonMessage_DataSetObject(jsonMessage_ElemGetData(parentMsg), jMsg);

		// Elemento IP_CLP
		jMsg = jsonMessage_ElemAppend(jMsg, jsonMessage_ElemNewFull(MSG_NAME_PARAM_IPCLP, FALSE));
		jsonMessage_DataSetString(jsonMessage_ElemGetData(jMsg), MaqConfigCurrent->ClpAddr);

		// Elemento Nome da Maquina
		jMsg = jsonMessage_ElemAppend(jMsg, jsonMessage_ElemNewFull(MSG_NAME_PARAM_MAQ_NAME, FALSE));
		jsonMessage_DataSetString(jsonMessage_ElemGetData(jMsg), MaqConfigCurrent->Name);
	}

	// Mensagem pronta! Agora a enviamos
	jsonQueue_PutMessage(jqMonitorStatus, rootMsg);
	jsonQueue_Work(jqMonitorStatus);
}
