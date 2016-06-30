/*
Implementation of the standard commands for the motor control program.

To add a command define the appropriate function and add the definition to the commands list.
Command function ought to return 0 upon success and POSITIVE integers as error codes upon failure.
*/

#include <wiringPi.h>
#include <stdlib.h>
#include <string.h>

#define MOTOR_A 0
#define MOTOR_B 1

#define MOTOR_R MOTOR_A
#define MOTOR_L MOTOR_B

#define CMD_STR_MAX_LEN 10 // including \0 // Unused

#define isDecimal(c) (c >= '0' && c <= '9')
#define isLetter(c) ((c >= 'a' && c <= 'z' ) || (c >= 'A' && c <= 'Z'))

void setMotor(unsigned int id, unsigned char state, unsigned char reverse);
int stop(char** args, unsigned int index);
int setm(char** args, unsigned int index);
int seta(char** args, unsigned int index);
int setb(char** args, unsigned int index);
int fwd(char** args, unsigned int index);
int back(char** args, unsigned int index);
int left(char** args, unsigned int index);
int right(char** args, unsigned int index);

struct mcc
{
	char* str;
	int (*func)(char**,unsigned int);
	int argc;
};


const struct mcc commands[] = 
{
{"stop",&stop,0},
{"s",&stop,0},
{"setm",&setm,3},
{"seta",&seta,2},
{"setb",&setb,2},
{"fwd",&fwd,0},
{"back",&back,0},
{"left",&left,0},
{"right",&right,0}
};


void setMotor(unsigned int id, unsigned char state, unsigned char reverse)
{
    if(!id) // Motor A (ID = 0)
    {
        digitalWrite(0,state);
		digitalWrite(2,!reverse);
		digitalWrite(3,reverse);
	}
	else // Motor B (ID = 1)
	{
		digitalWrite(1,state);
		digitalWrite(4,!reverse);
		digitalWrite(5,reverse);
	}
}

int stop(char** args, unsigned int index) //Stops both motors
{
	setMotor(MOTOR_A,0,0);
	setMotor(MOTOR_B,0,0);
	return 0;
}

int setm(char** args, unsigned int index) //Sets motor state
{
	unsigned int motorId = 0;
	unsigned char state, reverse;
	if(strcmp(args[0],"A") == 0 || strcmp(args[1],"a") == 0)
	{
		motorId = MOTOR_A;
	}
	else if(strcmp(args[0],"B") == 0 || strcmp(args[0],"b") == 0)
	{
		motorId = MOTOR_B;
	}
	else if(strcmp(args[0],"L") == 0 || strcmp(args[0],"l") == 0)
	{
		motorId = MOTOR_L;
	}
	else if(strcmp(args[0],"R") == 0 || strcmp(args[0],"r") == 0)
	{
		motorId = MOTOR_R;
	}
	else
	{
		return 1;
	}
	
	if(strcmp(args[1],"0") == 0)
	{
		state = 0;
	}
	else if(strcmp(args[1],"1") == 0)
	{
		state = 1;
	}
	else
	{
		return 2;
	}
	
	if(strcmp(args[2],"0") == 0)
	{
		reverse = 0;
	}
	else if(strcmp(args[2],"1") == 0)
	{
		reverse = 1;
	}
	else
	{
		return 3;
	}
	
	setMotor(motorId,state,reverse);

	return 0;
}

int seta(char** args, unsigned int index)
{
	char* setm_args[] = {"A", args[0], args[1]};
	return setm(setm_args, index);
}

int setb(char** args, unsigned int index)
{
	char* setm_args[] = {"B", args[0], args[1]};
	return setm(setm_args, index);
}

int fwd(char** args, unsigned int index)
{
	setMotor(MOTOR_A,1,0);
	setMotor(MOTOR_B,1,0);
	return 0;
}

int back(char** args, unsigned int index)
{
	setMotor(MOTOR_A,1,1);
	setMotor(MOTOR_B,1,1);
	return 0;
}

int left(char** args, unsigned int index)
{
	setMotor(MOTOR_L,1,1);
	setMotor(MOTOR_R,1,0);
	return 0;
}

int right(char** args, unsigned int index)
{
	setMotor(MOTOR_L,1,0);
	setMotor(MOTOR_R,1,1);
	return 0;
}

