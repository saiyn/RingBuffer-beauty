#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "cycbuf.h"

#define CYCBUF_SIZE (1024)

#define READ_BUF_SIZE (17)

unsigned int periods;

FILE *fd;

void *product_thread(void *arg)
{
	int ret;
	size_t n;
	char buf[READ_BUF_SIZE];


	while((n = fread(buf, 1,READ_BUF_SIZE, fd)) == READ_BUF_SIZE)
	{
		while((ret = cycbuf_write((cycbuf_t *)arg, n,buf)) < 0)
		{
			printf("\rcycbuf full, retry:%d");
			usleep(5000);
		}

	}

	printf("product thread will out, n:%d\n", n);
	

	exit(0);
}


void *consume_thread(void *arg)
{
	int ret;
	size_t len;
	void *data;


	for(;;)
	{
		cycbuf_query((cycbuf_t *)arg, &data, &len);

		if(len < periods)
		{
			goto sleep;	
		}
	
		
		if(write(STDOUT_FILENO, data, len) != len)
		{
			printf("write to stdout error\n");
		}
		else
		{
			cycbuf_update((cycbuf_t *)arg, len);
			continue;
		}

	sleep:

		usleep(5000);

	}


}

int main(int argc, char *argv[])
{
	int ret = 0;
	pthread_t tid[2] = {0};
	void *cycbuf_handle = NULL;

	if(argc < 3)
	{
		printf("Usage: %s <periods> <infile>\n", argv[0]);
		return -1;
	}

	periods = atoi(argv[1]);

	if(periods == 0)
	{

		printf("input periods %d too small, changed to default 16\n", periods);	

		periods = 16;
	}

	fd = fopen(argv[2], "rb");
	if(!fd)
	{
		printf("open %s fail\n", argv[2]);
		return -1;
	}



	ret = cycbuf_create(CYCBUF_SIZE, &cycbuf_handle);	
	if(ret < 0)
	{
		return -1;
	}



	ret = pthread_create(&tid[0], NULL, product_thread,cycbuf_handle);
	if(ret < 0)
	{
		printf("product thread create failed\n");
		return -1;
	}

	ret = pthread_create(&tid[1], NULL, consume_thread, cycbuf_handle);
	if(ret < 0)
	{
		printf("consume thread create failed\n");
		return -1;
	}

	for(;;)
	{
		printf("task:%d running\n", getpid());

		sleep(10);
	}


	return 0;
}
