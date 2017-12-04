#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "cycbuf.h"


#define ALIGN(size,n) (((size) + (n) - 1) & ~((n) - 1))
#define PAGE_ALIGN(size) ALIGN(size, sysconf(_SC_PAGE_SIZE))


int cycbuf_create(size_t size, void **hnd)
{

	cycbuf_t *pcb = malloc(sizeof(*pcb));

	if(!pcb)
	{
		printf("no mem for cycbuf_t\n");
		return -1;
	}

	pcb->size = PAGE_ALIGN(size);
	pcb->tail = 0UL;
	pcb->head = 0UL;

	pcb->vaddr = mmap(NULL, pcb->size << 1, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

	if(pcb->vaddr == MAP_FAILED)
	{
		printf("mmap faild, %d - %s\n", errno, strerror(errno));
		goto mmap_error;
	}

	
	if(remap_file_pages(pcb->vaddr + pcb->size, pcb->size, 0, 0, 0) < 0)
	{
		printf("remap file pages failed, %d - %d\n", errno, strerror(errno));
		goto remap_error;			
	}
	
	*hnd = pcb;

	printf("cycbuf size:%d create done\n", pcb->size);
	
	return 0;


remap_error:
	munmap(pcb->vaddr, pcb->size << 1);
	
mmap_error:
	free(pcb);
		

	return -1;

}


int cycbuf_write(cycbuf_t *pcb, size_t len, void *data)
{
	size_t bytes_to_write;

	bytes_to_write = (pcb->size - pcb->head + pcb->tail - 1UL) % pcb->size;

	if(bytes_to_write < len)
	{
		return -1;
	}

	memcpy(pcb->vaddr + pcb->head, data, len);


	pcb->head = (pcb->head + len) % pcb->size;

	return 0;
}


void cycbuf_query(cycbuf_t *pcb, void **pdata, size_t *len)
{
	size_t bytes_to_read;
	
	bytes_to_read = (pcb->size - pcb->tail + pcb->head) % pcb->size;

	*pdata = pcb->vaddr + pcb->tail;

	*len = bytes_to_read;				
}



int cycbuf_update(cycbuf_t *pcb, size_t size)
{
	size_t bytes_to_read;

	bytes_to_read = (pcb->size - pcb->tail + pcb->head) % pcb->size;

	if(bytes_to_read < size)
	{
		return -1;
	}

	
	pcb->tail = (pcb->tail + size) % pcb->size;

	return 0;
}


void cycbuf_free(cycbuf_t *pcb)
{
	if(pcb)
	{
		munmap(pcb->vaddr, pcb->size << 1);

		free(pcb);
	}

}

















