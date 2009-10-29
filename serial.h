#ifndef SERIAL_H
#define SERIAL_H

#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <termios.h>

#ifndef NULL
#define NULL ((void *)(0))
#endif

#define SerialParidadeNenhum 0
#define SerialParidadePar    1
#define SerialParidadeImpar  2

// Definições para criação da função de transmissão
#define SERIAL_TX_FNC_PTR(fnc)  unsigned int (*fnc)(struct PortaSerial *  , char *    , int     , int)
#define SERIAL_TX_FNC(fnc)      unsigned int   fnc (struct PortaSerial *ps, char *data, int size, int bloquear)

// Definições para criação da função de recepção
#define SERIAL_RX_FNC_PTR(fnc)  unsigned int (*fnc)(struct PortaSerial *  , char *    , int     )
#define SERIAL_RX_FNC(fnc)      unsigned int   fnc (struct PortaSerial *ps, char *data, int size)

struct PortaSerial
{
	int hnd; // Handle para acesso do arquivo.

	SERIAL_TX_FNC_PTR(txf); // Ponteiro para função de transmissão
	SERIAL_RX_FNC_PTR(rxf); // Ponteiro para função de recepção

	struct termios terminal; // Estrutura que recebera as configuracoes da serial.
	struct termios original; // Estrutura que retem as configuraçoes originais da porta.
};

struct CfgSerial
{
	unsigned int Vel, Dados, Stop, Paridade, CtrlFluxoHard;
};

// Funçao que recebe a velocidade e converte para as flags B9600, B115200, etc.
extern unsigned int SerialFlagVel(unsigned int);
// Funçao que recebe a flag de velocidade e converte para 9600, 115200, etc.
extern unsigned int SerialVel(unsigned int);
// Funçao que inicializa a porta serial. Recebe o nome do dispositivo referente a porta em questao.
extern struct PortaSerial * SerialInit(char *);
// Funçao que finaliza a porta serial.
extern void SerialClose(struct PortaSerial *);
// Funçao que le a configuraçao da porta serial.
extern void SerialLerConfig(struct PortaSerial *, struct CfgSerial *);
// Funçao que configura a serial.
extern void SerialConfig(struct PortaSerial *, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);
// Funçao que transmite dados para a serial. Retorna o numero de bytes enviados.
extern int  Transmitir (struct PortaSerial *, char *, int, int);
// Funçao que recebe dados da serial. Retorna o numero de bytes recebidos.
extern int  Receber    (struct PortaSerial *, char *, int);

#endif
