#include "mbed.h"

#define NORMAL_THRESHOLD    68
 
DigitalOut led(PC_13);
Serial pc(PA_9, PA_10, 115200);                 // tx and rx
DigitalIn fuel_sensor(PB_1, PullNone);
uint8_t flags;
Timer t;
Ticker ticker100Hz;
Timeout fuel_timeout;

uint8_t fuel_timer = 0;
uint8_t fuel_counter = 0;
bool acquire = false;

void ticker100HzISR();
 
int main() 
{
    t.start();
    ticker100Hz.attach(&ticker100HzISR, 0.01);
    
    while(1) 
    {   
            if(acquire)
            {
                flags &= ~(0x08);                             // clear fuel flag
                flags |= (fuel_counter > NORMAL_THRESHOLD) ? (0x01 << 3) : 0;

                if((flags & 0x08))
                {
                   pc.printf("\r\n1");
                }
                else
                {
                    pc.printf("\r\n0");
                }
                
                fuel_timer = 0;
                fuel_counter = 0;
                ticker100Hz.attach(&ticker100HzISR, 0.01);
                acquire = false;
            }
    }
}

void ticker100HzISR()
{
    if (fuel_timer < 100) {
        fuel_timer++;
        fuel_counter += fuel_sensor.read();
    } else {
        acquire = true;
        ticker100Hz.detach();
    }
}