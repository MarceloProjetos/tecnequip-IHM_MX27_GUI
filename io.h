#ifndef INCLUDED_IO_H
#define INCLUDED_IO_H

#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

//#define DEBUG_IO   //habilita debug

int           io_ctrl (unsigned long alvo);
unsigned long io_read (unsigned long alvo);
void          io_write(unsigned long alvo, unsigned long wval);
void          io_close(void);

#endif

