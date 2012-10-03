#include "io.h"

int            fd_devmem    = 0;
unsigned long  alvo_real    = 0;
void          *alvo_virtual = 0;

int io_ctrl(unsigned long alvo)
{
// Abre o dispositivo de memoria caso nao estiver aberto.
	if (!fd_devmem)
		{
		if((fd_devmem = open("/dev/mem", O_RDWR | O_SYNC)) == -1) return 0;
#ifdef DEBUG_IO
		printf("*** Aberto /dev/mem ***\n");
#endif
		}

// Mapeia a memoria para acesso a pagina relativa ao alvo, caso nao estiver mapeada ainda.
	if (alvo_real != (alvo & (~MAP_MASK)))
		{
#ifdef DEBUG_IO
		printf("*** Mudando da pagina 0x%lX para 0x%lX ***\n", alvo_real,alvo & (~MAP_MASK));
#endif

// A memoria ja esta mapeada em outro endereco. Devemos desmapear primeiro.
		if (alvo_real)
			if(munmap(alvo_virtual, MAP_SIZE) == -1) return 0;

		alvo_real = alvo & (~MAP_MASK);

		alvo_virtual = (void *)mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_devmem, alvo_real);
		if(alvo_virtual == (void *) -1) return 0;
		}

	return 1;
}

unsigned long io_read(unsigned long alvo)
{
	if(!io_ctrl(alvo)) return 0;

#ifdef DEBUG_IO
		printf("*** Lendo o endereco 0x%08lx ***\n", alvo);
#endif

	return *((unsigned long *)(alvo_virtual + (alvo & MAP_MASK)));
}

void io_write(unsigned long alvo, unsigned long wval)
{
	if(!io_ctrl(alvo)) return;

#ifdef DEBUG_IO
		printf("*** Escrevendo 0x%08lx no endereco 0x%08lx ***\n", wval, alvo);
#endif

	*((unsigned long *)(alvo_virtual + (alvo & MAP_MASK))) = wval;
}

void io_close(void)
{
	munmap(alvo_virtual, MAP_SIZE);
	close(fd_devmem);
}

