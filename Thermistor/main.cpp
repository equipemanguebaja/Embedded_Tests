// Test code for MTE3018
#include "mbed.h"
#define VCC 3.3
#define R 1000

AnalogIn x(PA_0);
float T, Vout;
Serial pc(PA_9, PA_10, 115200); // tx and rx

int main() {
    while(1) {
        
        Vout = (x.read())*VCC;
        
        T = (log((1843.8/((R*VCC/Vout)-R))))/0.032;
        
        pc.printf("\r\n%.2f",T);
    }
}
