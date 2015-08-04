#include "defines.h"

static const int FLAG_FILA_TAMANHO = 0x0200;
static const int FLAG_FILA_LER     = 0x0400;
static const int FLAG_FILA_GRAVAR  = 0x2000;

static const int REG_FILA_INDICE  = 18;
static const int REG_FILA_POSICAO = 19;

#define MAX_FILA_PECAS 40
static const char ARQ_FILA_PECAS[] = "diagonal.cfg";

static unsigned int FilaPecas[MAX_FILA_PECAS+1];

static int FilaGravada = 0;

unsigned long CalcCheckSum(void *ptr, unsigned int tam);

void FilaPecas_Ler(void)
{
  int i;

  MaqConfigFlags(MaqLerFlags() | FLAG_FILA_LER);
  while(MaqLerFlags() & FLAG_FILA_LER);
  FilaPecas[0] = MaqLerRegistrador(REG_FILA_INDICE, 0);

  for(i=1; i <= FilaPecas[0]; i++) {
    // Le a nova posicao
    MaqGravarRegistrador(REG_FILA_INDICE , i-1);
    MaqConfigFlags(MaqLerFlags() | FLAG_FILA_LER);
    // Espera terminar leitura atual
    while(MaqLerFlags() & FLAG_FILA_LER);
    FilaPecas[i] = MaqLerRegistrador(REG_FILA_POSICAO, 0);
  }
}

void FilaPecas_Gravar(void)
{
  int i;

  MaqGravarRegistrador(REG_FILA_INDICE, FilaPecas[0]);
  MaqConfigFlags(MaqLerFlags() | FLAG_FILA_TAMANHO);
  while(MaqLerFlags() & FLAG_FILA_TAMANHO);

  for(i=1; i <= FilaPecas[0]; i++) {
    // Espera terminar escrita anterior
    while(MaqLerFlags() & FLAG_FILA_GRAVAR);

    // Grava a nova posicao
    MaqGravarRegistrador(REG_FILA_INDICE , i-1);
    MaqGravarRegistrador(REG_FILA_POSICAO, FilaPecas[i]);
    MaqConfigFlags(MaqLerFlags() | FLAG_FILA_GRAVAR);
  }
}

int Diagonal_Init(void)
{
  unsigned long val;
  long fd = open(ARQ_FILA_PECAS, O_RDONLY);

  FilaPecas[0] = 0;

  if(fd >= 0) {
    read(fd, &val, sizeof(val));
    if(val == MAQ_CFG_MAGIC) {
      read(fd, &FilaPecas, sizeof(FilaPecas));

      read(fd, &val, sizeof(val));
      if(val != CalcCheckSum((void *)(&FilaPecas), sizeof(FilaPecas))) {
        FilaPecas[0] = 0;
      }
    }

    close(fd);
  }

  FilaPecas_Gravar();
  FilaGravada = 1;

  return 1;
}

void Diagonal_Auto(int ativo)
{
  unsigned long val;
  long fd;

  if(!FilaGravada) return;

  FilaPecas_Ler();

  fd = open(ARQ_FILA_PECAS, O_WRONLY | O_CREAT, 0666);
  if(fd >= 0) {
    val = MAQ_CFG_MAGIC;
    write(fd, &val, sizeof(val));
    write(fd, &FilaPecas, sizeof(FilaPecas));
    val = CalcCheckSum((void *)(&FilaPecas), sizeof(FilaPecas));
    write(fd, &val, sizeof(val));

    close(fd);
  }
}

void cbDiagonal_LimparFila(GtkButton *button, gpointer user_data)
{
	FilaPecas[0] = 0;
	FilaPecas_Gravar();
}
