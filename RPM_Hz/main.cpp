#include "mbed.h"
 
DigitalOut led(PC_13);
Serial pc(PA_9, PA_10, 115200);                 // tx and rx
InterruptIn freq_sensor(PB_6, PullNone);

float  hz = 0;
Timer t;
Ticker ticker5Hz;

uint8_t pulse_counter = 0;
uint64_t last_count = 0, current_period = 0;
float rpm_hz;
bool acquire = false;

void frequencyCounterISR();
void ticker5HzISR();
 
int main() 
{
    t.start();
    ticker5Hz.attach(&ticker5HzISR, 0.2);
    
    while(1) 
    {   
            if(acquire)
            {
                freq_sensor.fall(NULL);                                        // disable interrupt
                
                if (current_period != 0) 
                {
                    hz = 1000000*((float)(2*pulse_counter)/current_period);    //calculates frequency in Hz
                    pc.printf("\r\nRPM = %.2f",hz*60);    
                }else 
                    {
                    hz = 0;
                    pc.printf("\r\nRPM = %.2f",hz*60);
                    }               
                    
                /* prepare to re-init rpm counter */
                pulse_counter = 0;
                current_period = 0;                                   // reset pulses related variables
                last_count = t.read_us();
                freq_sensor.fall(&frequencyCounterISR);     // enable interrupt
                acquire = false;
            }
    }
}

void frequencyCounterISR()
{
    led = !led;
    pulse_counter++;
    current_period += t.read_us() - last_count;
    last_count = t.read_us();
}

void ticker5HzISR()
{
    acquire = true;
}
