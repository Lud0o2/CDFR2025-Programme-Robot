#include "lidar.h"
#include "lidarAnalize.h"
#include "logger.hpp"
#include <pigpio.h>
#include <unistd.h>
#define SIZEDATALIDAR 10000

//lidarAnalise_t : valid, angle, dist, x, y, onTable

lidarAnalize_t lidarData[SIZEDATALIDAR];  
int count = SIZEDATALIDAR;

int main() {
    double x = 0, y= 0, teta= 0, dist = 10000;
    double x_prec = 0,y_prec = 0;
    
    if(!lidarSetup("/dev/ttyUSB1",256000)){LOG_ERROR("cannot find the lidar");}
    /*
    if (gpioInitialise() < 0) {LOG_ERROR("cannot initialize lidar gpio speed");return 1;}
    gpioSetPWMfrequency(18, 40000);
    gpioSetMode(18, PI_OUTPUT);
    gpioSetPWMrange(18, 100);
    gpioPWM(18, 50);//lidar speed
    */
    position_t position = {x,y,teta,dist,0};
    sleep(4);
    for (int i = 0; i < 1; i++){
        getlidarData(lidarData,count);
        convertAngularToAxial(lidarData, count, &position);
        position_ennemie(lidarData,count, &position);
        //printf("\nx = %f / y = %f / x_prec = %f / y_prec = %f \n",x,y, x_prec, y_prec);

        
        delay(500);
    }

    lidarStop();
    
    }