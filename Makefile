

TARGET = cycbuf

CC = gcc


CFLAGS = -g -O0 

LDFLAGS = -lpthread

OBJS = cycbuf.o test.o


all:$(TARGET)


$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)




clean:
	rm $(TARGET) $(OBJS) -rf
	

