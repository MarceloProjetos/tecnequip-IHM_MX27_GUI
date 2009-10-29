/*===============================================================================
Programa: Corte Voador
Módulo: Acesso a porta serial
Linguagem: C
Data: 2007/05/03 09:48:00
Versão: 1.0.0
Autor(es): Marcelo Utikawa da Fonseca
=================================================================================
Copyright (c) 2006/2007
Tecnequip Tecnologia em Equipamentos Ltda. (www.tecnequip.com.br)
Este arquivo é de uso exclusivo e não pode ser distribuído total ou parcialmente
=================================================================================
Código independente, pode ser utilizado em outros programas.
=================================================================================
Changelog:

1.0.0:
	Versão inicial.

1.0.1:
	Alterada função de transmissão para calcular tamanho da string se recebido
		zero como tamanho dos dados a serem transmitidos.
	Alteradas funções para receber uma estrutura de porta serial, permitindo
		que existam várias portas abertas ao mesmo tempo.

=================================================================================*/

#include "serial.h"

SERIAL_TX_FNC(SerialTX)
{
	int bytes;

	if(!size)
		size = strlen(data);

	bytes = write(ps->hnd, data, size);

	if(bloquear)
		tcdrain(ps->hnd);

	return bytes;
}

SERIAL_RX_FNC(SerialRX)
{
	return read(ps->hnd, data, size);
}

struct PortaSerial * SerialInit(char *device)
{
	struct PortaSerial *ps = (struct PortaSerial *)(malloc(sizeof(struct PortaSerial)));

	if(ps != NULL)
		{
		ps->txf = SerialTX;

		ps->hnd = open(device, O_RDWR | O_NOCTTY);
		if(ps->hnd < 0)
			{
			free(ps);
			return NULL;
			}

		tcgetattr(ps->hnd, &ps->original);
		SerialConfig(ps,115200,8,1,SerialParidadeNenhum,0);
		}

	return ps;
}

void SerialClose(struct PortaSerial *ps)
{
	tcsetattr(ps->hnd, TCSANOW, &ps->original);
	close(ps->hnd);
	free(ps);
}

unsigned int SerialFlagVel(unsigned int Vel)
{
	switch(Vel)
	{
		case    300: return    B300;
		case    600: return    B600;
		case   1200: return   B1200;
		case   2400: return   B2400;
		case   4800: return   B4800;
		case   9600: return   B9600;
		case  19200: return  B19200;
		case  38400: return  B38400;
		case  57600: return  B57600;
		default    : // Caso o valor for invalido, seleciona B115200.
		case 115200: return B115200;
	}

	return 0; // Nunca pode chegar ate aqui!
}

unsigned int SerialVel(unsigned int Vel)
{
	switch(Vel)
	{
		case    B300: return    300;
		case    B600: return    600;
		case   B1200: return   1200;
		case   B2400: return   2400;
		case   B4800: return   4800;
		case   B9600: return   9600;
		case  B19200: return  19200;
		case  B38400: return  38400;
		case  B57600: return  57600;
		default     : // Caso o valor for invalido, seleciona 115200.
		case B115200: return 115200;
	}

	return 0; // Nunca pode chegar aqui!
}

void SerialConfig(struct PortaSerial *ps, unsigned long Vel, unsigned long Dados, unsigned long Stop, unsigned long Paridade, unsigned long CtrlFluxoHard)
{
// Calcula a flag de velocidade de transmissao.
	Vel = SerialFlagVel(Vel);
// Calcula a flag de numero de bits de dados.
	switch(Dados)
	{
		case  5: Dados = CS5; break;
		case  6: Dados = CS6; break;
		case  7: Dados = CS7; break;
		default: // Caso a configuraçao for invalida, seleciona 8.
		case  8: Dados = CS8; break;
	}

//Calcula a flag de paridade.
	switch(Paridade)
	{
		default:
		case  SerialParidadeNenhum: Paridade = 0              ; break;
		case  SerialParidadePar   : Paridade = PARENB         ; break;
		case  SerialParidadeImpar : Paridade = PARENB | PARODD; break;
	}

// Calcula a flag de stop bit.
	if(Stop==2)
		Stop = CSTOPB;
	else
		Stop = 0; // Sem stop bit adicional.

// Calcula a flag de controle de fluxo por hardware.
	if(CtrlFluxoHard)
		CtrlFluxoHard = CRTSCTS;

// Configura a porta com o modo solicitado.
	bzero((void *)(&ps->terminal),sizeof(ps->terminal));
	ps->terminal.c_cflag = Vel | Dados | Stop | Paridade | CtrlFluxoHard | CLOCAL | CREAD;
	ps->terminal.c_iflag = IGNPAR;
	ps->terminal.c_oflag = 0;
	ps->terminal.c_lflag = 0;
	ps->terminal.c_cc[VMIN ] = 0;
	ps->terminal.c_cc[VTIME] = 0;
	tcflush(ps->hnd, TCIFLUSH);
	tcsetattr(ps->hnd, TCSANOW, &ps->terminal);
}

void SerialLerConfig(struct PortaSerial *ps, struct CfgSerial *Config)
{
	struct termios tmp;

// Carrega os valores atuais.
	tcgetattr(ps->hnd, &tmp);

// Le a velocidade de transmissao.
	Config->Vel = SerialVel(cfgetispeed(&tmp));

// Calcula o numero de bits de dados.
	if((tmp.c_cflag&CS8)==CS8)
		Config->Dados=8;
	else if((tmp.c_cflag&CS7)==CS7)
		Config->Dados=7;
	else if((tmp.c_cflag&CS6)==CS6)
		Config->Dados=6;
	else
		Config->Dados=5;

//Calcula a paridade.
	if((tmp.c_cflag&(PARODD|PARENB))==(PARODD|PARENB))
		Config->Paridade = SerialParidadeImpar;
	else if((tmp.c_cflag&PARENB)==PARENB)
		Config->Paridade = SerialParidadePar;
	else
		Config->Paridade = SerialParidadeNenhum;

// Calcula o numero de stop bits.
	if((tmp.c_cflag&CSTOPB)==CSTOPB)
		Config->Stop = 2;
	else
		Config->Stop = 1; // Sem stop bit adicional.

// Calcula a flag de controle de fluxo por hardware.
	if((tmp.c_cflag&CRTSCTS)==CRTSCTS)
		Config->CtrlFluxoHard = 1;
	else
		Config->CtrlFluxoHard = 0;
}

int Transmitir(struct PortaSerial *ps, char *txt, int tam, int bloquear)
{
	return (*ps->txf)(ps, txt, tam, bloquear);
}

int Receber(struct PortaSerial *ps, char *txt, int tam)
{
	return (*ps->rxf)(ps, txt, tam);
}
