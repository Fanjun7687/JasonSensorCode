rm -rf main
#gcc main.c -o main  -I./include -L./ -lmodbus
gcc main.c -o main  -I./include -L./ -lmodbus -lwiringPi -lm -lpthread -lrt -lcrypt -lcurl
