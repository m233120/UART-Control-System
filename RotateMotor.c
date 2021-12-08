
//512 steps per 1 revolution
//8 micro rotations per step, 4096 microrotations per step
//Can have precision up to 0.0879 degrees.

#include <math.h>
#define delay 2

void Port_Init(void);
void rotate(int degrees, int direct, int motorNum);
void reset(void);
double toSteps(int degrees);
int optimizeDeg(int degrees);
extern void port_config(void);
extern void delay_x_ms(int s);

static int currentDeg1 = 0;
static int currentDeg2 = 0;
// int main(void)
// {
//     Port_Init();
//     rotate(-5, 0, 2);
// 	//rotate(125, 1, 2);
//     delay_x_ms(1000);
//     //reset();
// }

int optimizeDeg(int degrees) //This optimizes the rotation by preventing needless over rotations
{
    if(degrees > 0)
    {
        while(degrees > 360)
        {
            degrees -= 360;
        }
    }
    else
    {
        while(degrees < -360)
        {
            degrees += 360;
        }
    }
    return degrees;
}

void rotate(int degrees, int direct, int motorNum)
{
    volatile unsigned int* FIO1SET_ptr = (unsigned int *) 0x2009C038;
    volatile unsigned int* FIO1CLR_ptr = (unsigned int *) 0x2009C03C; 
    int opDeg = optimizeDeg(degrees);

    if(direct == 1) //User told the motor to move directly to an angle relative to true "north"
    {
				if(motorNum == 1)
					opDeg = opDeg - currentDeg1;
				else
					opDeg = opDeg - currentDeg2;
    }
	
		if(motorNum == 1)
			currentDeg1 = currentDeg1 + opDeg;
		else
			currentDeg2 = currentDeg2 + opDeg;
		
		while(currentDeg1 < 0)
		{
			currentDeg1 += 360;
		}
		
		while(currentDeg2 < 0)
		{
			currentDeg2 += 360;
		}
		
    double steps = (opDeg * 512.0) / 360; //Converts the degrees to "steps" of the motor
    int fullSteps = (int) steps;
    double partialSteps = steps - fullSteps;

    unsigned int order[8];
    unsigned int clear;

    if(motorNum == 1) //Pins 1.28 - 1.31
    {
        order[0] = 0x70000000;
        order[1] = 0x30000000;
        order[2] = 0xB0000000;
        order[3] = 0x90000000;
        order[4] = 0xD0000000;
        order[5] = 0xC0000000;
        order[6] = 0xE0000000;
        order[7] = 0x60000000;
        clear = 0xF0000000;
    }
    else //Pins 1.24 - 1.27
    {
        order[0] = 0x07000000;
        order[1] = 0x03000000;
        order[2] = 0x0B000000;
        order[3] = 0x09000000;
        order[4] = 0x0D000000;
        order[5] = 0x0C000000;
        order[6] = 0x0E000000;
        order[7] = 0x06000000;
        clear = 0x0F000000;
    }

    if(fullSteps > 0)//Rotate clockwise
    {	
        for(int i = 0; i < fullSteps + 1; i++) //Goes through a "step" which contains 8 microrotations.
        {
            if(i == fullSteps && (1.0 / 8) > partialSteps) //Keeps track of final partial step for highest accuracy
                break;
            *(FIO1SET_ptr) = order[0]; //Sets bit 0.0, 0.1, 0.2
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;

            if(i == fullSteps && (2.0 / 8) > partialSteps)
                break;
            *(FIO1SET_ptr) = order[1]; //Sets bit 0.0, 0.1
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;

            if(i == fullSteps && (3.0 / 8) > partialSteps)
                break;
            *(FIO1SET_ptr) = order[2]; //Sets bit 0.0, 0.1, 0.3
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;

            if(i == fullSteps && (4.0 / 8) > partialSteps)
                break;
            *(FIO1SET_ptr) = order[3]; //Sets bit 0.0, 0.3
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;

            if(i == fullSteps && (5.0 / 8) > partialSteps)
                break;
            *(FIO1SET_ptr) = order[4]; //Sets bit 0.0, 0.2, 0.3
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;

            if(i == fullSteps && (6.0 / 8) > partialSteps)
                break;
            *(FIO1SET_ptr) = order[5]; //Sets bit 0.2, 0.3
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;

            if(i == fullSteps && (7.0 / 8) > partialSteps)
                break;
            *(FIO1SET_ptr) = order[6]; //Sets bit 0.1, 0.2, 0.3
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;

            if(i == fullSteps)
                break;
            *(FIO1SET_ptr) = order[7]; //Sets bit 0.1, 0.2
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;
        }
    }
    else //Rotate counterclockwise
    {
        for(int i = 0; i > fullSteps - 1; i--)
        {

            partialSteps = fabs(partialSteps);

            if(i == fullSteps && (1.0 / 8) > partialSteps)
                break;
            *(FIO1SET_ptr) = order[7]; //Sets bit 0.0, 0.1, 0.2
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;

            if(i == fullSteps && (2.0 / 8) > partialSteps)
                break;
            *(FIO1SET_ptr) = order[6]; //Sets bit 0.0, 0.1
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;

            if(i == fullSteps && (3.0 / 8) > partialSteps)
                break;
            *(FIO1SET_ptr) = order[5]; //Sets bit 0.0, 0.1, 0.3
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;

            if(i == fullSteps && (4.0 / 8) > partialSteps)
                break;
            *(FIO1SET_ptr) = order[4]; //Sets bit 0.0, 0.3
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;

            if(i == fullSteps && (5.0 / 8) > partialSteps)
                break;
            *(FIO1SET_ptr) = order[3]; //Sets bit 0.0, 0.2, 0.3
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;

            if(i == fullSteps && (6.0 / 8) > partialSteps)
                break;
            *(FIO1SET_ptr) = order[2]; //Sets bit 0.2, 0.3
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;

            if(i == fullSteps && (7.0 / 8) > partialSteps)
                break;
            *(FIO1SET_ptr) = order[1]; //Sets bit 0.1, 0.2, 0.3
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;

            if(i == fullSteps)
                break;
            *(FIO1SET_ptr) = order[0]; //Sets bit 0.1, 0.2
            delay_x_ms(delay);
            *(FIO1CLR_ptr) = clear;
        }
    }		
		delay_x_ms(250);
}

void reset(void)
{
    rotate(0, 1, 1);
    rotate(0, 1, 2);
    currentDeg1 = 0;
		currentDeg2 = 0;
}

void Port_Init(void)
{
    volatile unsigned int* FIO1DIR_ptr = (unsigned int *) 0x2009C020;
    *(FIO1DIR_ptr) = 0xFF000000;
}