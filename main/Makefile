#gcc random-test-client.c -o random-test-client -I./ -L./ -lmodbus
target := main

object := $(patsubst %.c,%.o,$(wildcard *.c))  
      
#CROSS_COMPILE := aarch64-linux-gnu- 
#CC := $(CROSS_COMPILE)gcc   
CC := gcc
#CFLAGS += -I/home/gqb/crossdir/linux-devkit/arm-none-linux-gnueabi/usr/include  
#LDFLAGS += -L/home/gqb/crossdir/linux-devkit/arm-none-linux-gnueabi/usr/lib

CFLAGS += -I./include

LDFLAGS += -L./ 

LDFLAGS += -lmodbus -lwiringPi -lm -lpthread -lrt -lcrypt -lcurl
   
%.o : %c  
	$(CC) $< -o $@ $(CFLAGS) $(LDFLAGS)  
   
all : $(target)  
$(target): $(object)  
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)  
clean:  
	rm -f $(target) $(object)  
	
