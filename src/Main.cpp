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
    
    if(!lidarSetup("/dev/ttyUSB0",256000)){LOG_ERROR("cannot find the lidar");}
    /*
    if (gpioInitialise() < 0) {LOG_ERROR("cannot initialize lidar gpio speed");return 1;}
    gpioSetPWMfrequency(18, 40000);
    gpioSetMode(18, PI_OUTPUT);
    gpioSetPWMrange(18, 100);
    gpioPWM(18, 50);//lidar speed
    */
    position_t position = {x,y,teta,dist,0};
    position_t position_ennemie;
    sleep(4);
    for (int i = 0; i < 100; i++){
        position = {x,y,teta,dist,0};
        position_ennemie = {x,y,teta,dist,0};
        getlidarData(lidarData,count);
        convertAngularToAxial(lidarData, count, &position,0);
        init_position_balise(lidarData,count, &position, &position_ennemie);

        printf("\n : x = %f / y = %f / teta = %f",position.x,position.y,position.teta);
        printf(" /  x_ennemie = %f / y_ennemie = %f \n",position_ennemie.x,position_ennemie.y);
        
        delay(1000);
    }

    lidarStop();
    
    }