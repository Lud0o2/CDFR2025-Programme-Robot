#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include "math.h"
#include "logger.hpp"

#define MAP(value, fromLow, fromHigh, toLow, toHigh) ((toLow) + (((value) - (fromLow)) * ((toHigh) - (toLow)) / ((fromHigh) - (fromLow))))
typedef struct {
    int i ; //indice début élément décord
    int nb; //nombre d'élement 
    float moy_dist ; //somme distance
    float moy_angle; //somme angle
    float cm; //taille object


} element_decord;


void convertAngularToAxial(lidarAnalize_t* data, int count, position_t *position);

bool collideFordward(lidarAnalize_t* data, int count);

bool collideBackward(lidarAnalize_t* data, int count); 

int collide(lidarAnalize_t* data, int count ,int distanceStop);

void printLidarAxial(lidarAnalize_t* data, int count);

void printAngular(lidarAnalize_t* data, int count);

void pixelArtPrint(lidarAnalize_t* data, int count,int sizeX,int sizeY,int scale,position_t position);

double distance_2_pts(double d1,double deg1, double d2, double deg2);

void supprimerElement(element_decord**& array, int& rows, int index);

void position_facile(lidarAnalize_t* data,int count, double *X, double* Y, double X_prec, double Y_prec);




