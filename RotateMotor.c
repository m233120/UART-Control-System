

//512 steps per 1 revolution
//8 micro rotations per step, 4096 microrotations per step
//Can have precision up to 0.0879 degrees.

#include <math.h>
#define delay 2

void Port_Init(void);
void rotate(int degrees, int direct);
void reset(void);
double toSteps(int degrees);
int optimizeDeg(int degrees);
extern void LEDs(int n);
extern void port_config(void);
extern void delay_x_ms(int s);

static int currentDeg = 0;
int main(void)
{
    Port_Init();
    rotate(-440, 1);
		rotate(500, 0);
		rotate(180, 1);
    delay_x_ms(1000);
    reset();
}

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

double toSteps(int degrees)
{
	
  currentDeg = currentDeg + degrees;  
  double newSteps = (degrees * 512.0) / 360; //Converts the degrees to "steps" of the motor

  return newSteps;
	
}

void rotate(int degrees, int direct)
{
    volatile unsigned int* FIO0SET_ptr = (unsigned int *) 0x2009C018;
    volatile unsigned int* FIO0CLR_ptr = (unsigned int *) 0x2009C01C; 
    int opDeg = optimizeDeg(degrees);

    if(direct == 1) //User told the motor to move directly to an angle relative to true "north"
    {
        opDeg = opDeg - currentDeg;
    }

    double steps = toSteps(opDeg);
    int fullSteps = (int) steps;
    double partialSteps = steps - fullSteps;

    if(fullSteps > 0)//Rotate clockwise
    {	
        for(int i = 0; i < fullSteps + 1; i++) //Goes through a "step" which contains 8 microrotations.
        {
            if(i == fullSteps && (1.0 / 8) > partialSteps) //Keeps track of final partial step for highest accuracy
                break;
            *(FIO0SET_ptr) = 0x00000007; //Sets bit 0.0, 0.1, 0.2
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;

            if(i == fullSteps && (2.0 / 8) > partialSteps)
                break;
            *(FIO0SET_ptr) = 0x00000003; //Sets bit 0.0, 0.1
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;

            if(i == fullSteps && (3.0 / 8) > partialSteps)
                break;
            *(FIO0SET_ptr) = 0x0000000B; //Sets bit 0.0, 0.1, 0.3
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;

            if(i == fullSteps && (4.0 / 8) > partialSteps)
                break;
            *(FIO0SET_ptr) = 0x00000009; //Sets bit 0.0, 0.3
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;

            if(i == fullSteps && (5.0 / 8) > partialSteps)
                break;
            *(FIO0SET_ptr) = 0x0000000D; //Sets bit 0.0, 0.2, 0.3
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;

            if(i == fullSteps && (6.0 / 8) > partialSteps)
                break;
            *(FIO0SET_ptr) = 0x0000000C; //Sets bit 0.2, 0.3
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;
            
            if(i == fullSteps && (7.0 / 8) > partialSteps)
                break;
            *(FIO0SET_ptr) = 0x0000000E; //Sets bit 0.1, 0.2, 0.3
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;

            if(i == fullSteps)
                break;
            *(FIO0SET_ptr) = 0x00000006; //Sets bit 0.1, 0.2
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;
        }
    }
    else //Rotate counterclockwise
    {
        for(int i = 0; i > fullSteps - 1; i--)
        {

            partialSteps = fabs(partialSteps);

            if(i == fullSteps && (1.0 / 8) > partialSteps)
                break;
            *(FIO0SET_ptr) = 0x00000006; //Sets bit 0.0, 0.1, 0.2
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;

            if(i == fullSteps && (2.0 / 8) > partialSteps)
                break;
            *(FIO0SET_ptr) = 0x0000000E; //Sets bit 0.0, 0.1
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;

            if(i == fullSteps && (3.0 / 8) > partialSteps)
                break;
            *(FIO0SET_ptr) = 0x0000000C; //Sets bit 0.0, 0.1, 0.3
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;

            if(i == fullSteps && (4.0 / 8) > partialSteps)
                break;
            *(FIO0SET_ptr) = 0x0000000D; //Sets bit 0.0, 0.3
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;

            if(i == fullSteps && (5.0 / 8) > partialSteps)
                break;
            *(FIO0SET_ptr) = 0x00000009; //Sets bit 0.0, 0.2, 0.3
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;

            if(i == fullSteps && (6.0 / 8) > partialSteps)
                break;
            *(FIO0SET_ptr) = 0x0000000B; //Sets bit 0.2, 0.3
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;
            
            if(i == fullSteps && (7.0 / 8) > partialSteps)
                break;
            *(FIO0SET_ptr) = 0x00000003; //Sets bit 0.1, 0.2, 0.3
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;

            if(i == fullSteps)
                break;
            *(FIO0SET_ptr) = 0x00000007; //Sets bit 0.1, 0.2
            delay_x_ms(delay);
            *(FIO0CLR_ptr) = 0x0000000F;
        }
    }
		
		delay_x_ms(250);
}

void reset(void)
{
    rotate(0, 1);
    currentDeg = 0;
}

void Port_Init(void)
{
    volatile unsigned int* FIO0DIR_ptr = (unsigned int *) 0x2009C000;
    volatile unsigned int* PINSEL0_ptr = (unsigned int *) 0x4002C000; 

    // Port 0 (bits 0-3 as outputs)
    *(PINSEL0_ptr) = 0x00000000;
    *(FIO0DIR_ptr) = 0x0000000F;
}