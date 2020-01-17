#include "stm32f103c8t6.h"
#include "Kalman.h"
#include "mbed.h"
#include "LSM6DS3.h"
#define RAD_TO_DEG 180/3.14

Kalman kalmanX; // Create the Kalman instances
Kalman kalmanY;
Serial pc(PA_9, PA_10, 115200); // tx, rx
LSM6DS3 LSM6DS3(PB_7, PB_6);
Timer t;

unsigned long int t1;
uint16_t adc_value;
double kalAngleX, kalAngleY; // Calculated angle using a Kalman filter

int16_t accx, accy, accz;
int16_t grx, gry, grz;
double dt;

void gyro_getdata(double accx, double accy, double accz, double grx, double gry, double grz, double dt);

int main() {
    t.start();
    t1 = t.read_us();
    while(1)
    {
        LSM6DS3.begin(LSM6DS3.G_SCALE_245DPS, LSM6DS3.A_SCALE_2G, LSM6DS3.G_ODR_208, LSM6DS3.A_ODR_208);
        LSM6DS3.readAccel();
        LSM6DS3.readGyro();
        accx = LSM6DS3.ax;
        accy = LSM6DS3.ay;
        accz = LSM6DS3.az;
        grx = LSM6DS3.gx;
        gry = LSM6DS3.gy;
        grz = LSM6DS3.ax;
        dt = (double)(t.read_us() - t1) / 1000000; // Calculate delta time
        t1 = t.read_us();
        //gyro_getdata(accx,accy,accz,grx,gry,grz,dt);
        //pc.printf("%d\r\n",accx);
        //pc.printf("%d\r\n",accy);
        //pc.printf("%d\r\n",accz);
        //pc.printf("%d\r\n",grx);
        //pc.printf("%d\r\n",gry);
        //pc.printf("%d\r\n",grz);
    }}
    
void gyro_getdata(double accx, double accy, double accz, double grx, double gry, double grz, double dt){
    double pitch  = atan2(accy, accz) * RAD_TO_DEG;
    double roll = atan(-accx / sqrt(accy * accy + accz * accz)) * RAD_TO_DEG;
    kalmanX.setAngle(roll); // Set starting angle
    kalmanY.setAngle(pitch);
    static double gyroXangle = roll;
    static double gyroYangle = pitch;
    double gyroXrate = grx / 131.0; // Convert to deg/s
    double gyroYrate = gry / 131.0; // Convert to deg/s

  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((roll < -90 && kalAngleX > 90) || (roll > 90 && kalAngleX < -90)) {
    kalmanX.setAngle(roll);
    kalAngleX = roll;
    gyroXangle = roll;
  } else
    kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleX) > 90)
    gyroYrate = -gyroYrate; // Invert rate, so it fits the restriced accelerometer reading
  kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);

  gyroXangle += gyroXrate * dt; // Calculate gyro angle without any filter
  gyroYangle += gyroYrate * dt;
  gyroXangle += kalmanX.getRate() * dt; // Calculate gyro angle using the unbiased rate
  gyroYangle += kalmanY.getRate() * dt;

  // Reset the gyro angle when it has drifted too much
  if (gyroXangle < -180 || gyroXangle > 180)
    gyroXangle = kalAngleX;
  if (gyroYangle < -180 || gyroYangle > 180)
    gyroYangle = kalAngleY;
    
    pc.printf("%.2f",roll);
    pc.printf("\r\n");
    pc.printf("%.2f",pitch);
    pc.printf("\r\n");
    wait_ms(500);
}
