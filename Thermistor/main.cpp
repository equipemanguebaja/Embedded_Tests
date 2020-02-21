#include "mbed.h"
#define VCC 3.3
#define R 1000

AnalogIn x(PA_0);
float T, Vout;
Serial pc(PA_9, PA_10, 115200); // tx and rx

int main() {
    while(1) {
        
        Vout = (x.read())*VCC;
        
        //T = ((float) (1.0/0.032)*log((1842.8*(VCC - Vout)/(Vout*R))));
        T = 115.5*(exp(-0.02187*(Vout*R)/(VCC - Vout))) + 85.97*(exp(-0.00146*(Vout*R)/(VCC - Vout)));
        
        pc.printf("\r\nt = %.2f",T);
    }
}