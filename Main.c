#include "RotateMotor.h"

extern void port_config(void);
extern void delay_x_ms(int s);

int main(void)
{
    Port_Init();
    rotate(-5, 0, 2);
    rotate(125, 1, 2);
    delay_x_ms(1000);
    reset();
}