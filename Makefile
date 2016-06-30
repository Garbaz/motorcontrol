default: debug

debug:
	gcc -std=gnu11 -lwiringPi -o motorcontrol motorcontrol.c

release:
	gcc -std=gnu11 -O3 -lwiringPi -o motorcontrol motorcontrol.c
