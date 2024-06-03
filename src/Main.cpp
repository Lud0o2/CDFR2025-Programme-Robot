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
    
    if(!lidarSetup("/dev/ttyUSB0",256000)){LOG_ERROR("cannot find the lidar");}
    
    position_t position = {x,y,teta,dist,0};
    sleep(4);
    for (int i = 0; i < 4; i++){
        getlidarData(lidarData,count);
        //convertAngularToAxial(lidarData, count, &position);
        position_facile(lidarData,count, &x,&y ,x_prec, y_prec);
        //printf("\nx = %f / y = %f / x_prec = %f / y_prec = %f \n",x,y, x_prec, y_prec);
        x_prec = x;
        y_prec = y;
        
        delay(500);
    }

    lidarStop();
    
    }