#
#saiyn
#

TARGET = cycbuf

CC = gcc


CFLAGS = -g -O0 

LDFLAGS = -lpthread

OBJS = cycbuf.o test.o


all:$(TARGET)


$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

-include $(OBJS:.o=.d)

%.d:%.c
	$(CC) -MM $(CFLAGS) $< > $@

%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@


clean:
	rm $(TARGET) $(OBJS) *.d -rf
	

