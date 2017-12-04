#ifndef _CYCBUF_H_
#define _CYCBUF_H_

typedef struct cycbuf
{
	size_t head;
	size_t tail;
	size_t size;
	void   *vaddr;
}cycbuf_t;


int cycbuf_create(size_t size, void **hnd);


int cycbuf_write(cycbuf_t *pcb, size_t len, void *data);


void cycbuf_query(cycbuf_t *pcb, void **pdata, size_t *len);



int cycbuf_update(cycbuf_t *pcb, size_t size);




#endif
