#include "mbed.h"
 
DigitalOut led(PC_13);
Serial pc(PA_9, PA_10, 115200);                 // tx and rx
InterruptIn freq_sensor(PB_6, PullNone);
PwmOut signal(PA_7);                            // Debug purposes

float  hz = 0;
Timer t;
Ticker ticker5Hz;

uint8_t pulse_counter = 0;
uint64_t last_count = 0, current_period = 0;
bool acquire = false;

void frequencyCounterISR();
void ticker5HzISR();
 
int main() 
{
    t.start();
    ticker5Hz.attach(&ticker5HzISR, 0.2);
    signal.period_ms(32);                       // set signal frequency to 1/0.032Hz
    signal.write(0.5f);                         // dutycycle 50%
    
    while(1) 
    {   
            if(acquire)
            {
                freq_sensor.fall(NULL);                                        // disable interrupt
                
                if (current_period != 0) 
                {
                    hz = 1000*((float)(pulse_counter)/current_period);    //calculates frequency in Hz
                    pc.printf("\r\nhertz = %.2f",hz);    
                }else 
                    {
                    hz = 0;
                    }               
                    
                /* prepare to re-init rpm counter */
                pulse_counter = 0;
                current_period = 0;                                   // reset pulses related variables
                last_count = t.read_ms();
                freq_sensor.fall(&frequencyCounterISR);               // enable interrupt
                acquire = false;
            }
    }
}

void frequencyCounterISR()
{
    led = !led;
    pulse_counter++;
    current_period += t.read_ms() - last_count;
    last_count = t.read_ms();
}

void ticker5HzISR()
{
    acquire = true;
}
