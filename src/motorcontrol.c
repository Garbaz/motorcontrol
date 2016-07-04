/*
Controls a motor using the L293D motor driver.

PINS (On Raspberry Pi B+):
 +------------+------+--------------+
 | DRIVER PIN | GPIO | PHYSICAL PIN |
 +------------+------+--------------+
 |  ENABLE 1  |  0   |      11      |
 |  ENABLE 2  |  1   |      12      |
 |  INPUT  1  |  2   |      13      |
 |  INPUT  2  |  3   |      15      |
 |  INPUT  3  |  4   |      16      |
 |  INPUT  4  |  5   |      18      |
 +------------+------+--------------+
*/

#include <wiringPi.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <garbaz/netlib.h> // https://github.com/garbaz/netlib
#include "defaults.h"
#include "mcc.h"

#define uint unsigned int
#define ulong unsigned long
#define uchar unsigned char

#define MOTOR_A 0
#define MOTOR_B 1

#define BUFFER_SIZE  64
#define BACKLOG      1

#define DEBUG        1

#if DEBUG
#define DPRNT(...) printf(__VA_ARGS__)
#else
#define DPRNT(...) ""
#endif

#define ERRPRNT(...) fprintf(stderr,__VA_ARGS__)

void rmchar(char * str, char c);

char recv_buffer[BUFFER_SIZE];
char* port_buffer;
int targetfd,hostfd, bytes_recvd;
struct sockaddr_storage sender;
socklen_t senderlen;

void interruptHandler(int sig)
{
	for(uchar i = 0; i < 6; i++)
	{
		digitalWrite(i,LOW);
	}
	printf("DONE!\n");
	exit(0);
}

/*
void setMotor(uint id, uchar state, uchar reverse)
{
	if(!id) // Motor A
	{
		digitalWrite(0,state);
		digitalWrite(2,!reverse);
		digitalWrite(3,reverse);
	}
	else // Motor B
	{
		digitalWrite(1,state);
		digitalWrite(4,!reverse);
		digitalWrite(5,reverse);
	}
}
*/

int motorCommand(char* cmd_str)
{
	char* cmd = strtok(cmd_str," "); // Split cmd from characters
	
	int argc = -1;
	int index = -1;

	const unsigned int cmd_size = sizeof(commands)/sizeof(struct mcc);
	for(unsigned int i = 0; i < cmd_size; i++)
	{	
		if(strcmp(commands[i].str,cmd) == 0)
		{
			argc = commands[i].argc;
			index = i;
			break;
		}
	}
	if(index == -1)
	{
		return -1; // No legit command found
	}

	char* args[argc];
	for(unsigned int i = 0; i < argc; i++)
	{
		args[i] = strtok(NULL," ");
		if(args[i] == NULL) return -2; // Not enough arguments
	}
	
	return (*(commands[index].func))(args,(unsigned int)index);
}


int main(int argc, char** argv)
{	
	if(argc > 1)
	{
		if(strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--help") == 0)
		{
			printf("Usage:\n");
			printf("%s [PORT]\n\n", argv[0]);
			return 0;
		}
		else
		{
			port_buffer = argv[1];
		}
	}
	else
	{
		port_buffer = DEFAULT_PORT;
	}

	senderlen = sizeof(struct sockaddr);
	
	DPRNT("Setting up host on port %s...\n",port_buffer);
	if((hostfd = tcreate_host(port_buffer)) < 0)
	{
		ERRPRNT("ERROR (%d): %s\n",hostfd,TCREATE_HOST_ERR__STR(hostfd));
		return -1;
	}
	DPRNT("done.\n");
	
	signal(SIGINT, interruptHandler);
	
	wiringPiSetup();
	for(uchar i = 0; i < 6; i++)
	{
		pinMode(i,OUTPUT);
	}
	
	while(1)
	{
		for(uchar i = 0; i < 6; i++)
		{
			digitalWrite(i,LOW);
		}
		DPRNT("Waiting for incoming connection...\n");
		if((targetfd = tlisten_accept(hostfd,BACKLOG)) < 0)
		{
			ERRPRNT("ERROR (%d): %s\n",targetfd,TLISTEN_ACCEPT_ERR__STR(targetfd));
		}
		DPRNT("done.\n");

		while(1)
		{
			bytes_recvd = BUFFER_SIZE;
			memset(recv_buffer,0,BUFFER_SIZE);
			if(trecv(targetfd, recv_buffer, &bytes_recvd) < 0)
			{
				ERRPRNT("ERROR (%d): %s\n", bytes_recvd, TRECV_ERR__STR(bytes_recvd));
				break;
			}
			DPRNT("Recieved %d bytes.\n",bytes_recvd);
			
			rmchar(recv_buffer,'\n');
			rmchar(recv_buffer,'\r');
			
			if(strcmp(recv_buffer,"quit") == 0)
			{
				tdisconnect(targetfd);
				break;
			}
			int motorcmdret = 0;
			if((motorcmdret = motorCommand(recv_buffer)) != 0)
			{
				switch(motorcmdret)
				{
					case -1:
						ERRPRNT("ERROR (%d): Unknown command \"%s\"\n",motorcmdret,recv_buffer);
						break;
					case -2:
						ERRPRNT("ERROR (%d): Not enough arguments\n",motorcmdret);
						break;
					default:
						ERRPRNT("ERROR (%d): Couldn't execute command\n",motorcmdret);
				}
			}
		}
	}
}

void rmchar(char * str, char c)
{
	const unsigned int STR_LEN = strlen(str);
	char tmp[STR_LEN+1];
	unsigned int j = 0;

	for(unsigned int i = 0; i < STR_LEN; i++)
	{
		if(str[i] != c)
		{
			tmp[j] = str[i];
			j++;
		}
	}
	tmp[j] = '\0';
	memcpy(str,tmp,j+1);
}
