#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <cstdlib>
#include <filesystem>
#include <pigpio.h>
#include <thread>
#include <fstream>

#include "fonction.h"
#include "lidarAnalize.h"
#include "lidar.h"
#include "asser.hpp"
#include "arduino.hpp"
#include "utils.h"
#include "arduinoSubFonction.h"
#include "logger.hpp"

#include "actionContainer.hpp"

//#define DISABLE_LIDAR
#define SIZEDATALIDAR 10000

typedef enum {
    INIT,
    INITIALIZE,
    SETHOME,
    WAITSTART,
    RUN,
    RETURNHOME,
    FIN,
    STOP
} main_State_t;

bool ctrl_c_pressed;
void ctrlc(int)
{
    ctrl_c_pressed = true;
}
bool ctrl_z_pressed = false;
void ctrlz(int signal) {
    ctrl_z_pressed = true;
}


bool isWifiConnected() {
    std::ifstream file("/proc/net/wireless");
    std::string line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line.find("wlan0") != std::string::npos) {
                // Si la ligne contient "wlan0", cela indique que l'interface Wi-Fi est présente
                return true;
            }
        }
        file.close();
    } else {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier /proc/net/wireless." << std::endl;
    }

    return false;
}

void executePythonScript(const std::string& command) {
    std::system(command.c_str());
}

int main(int argc, char *argv[]) {
    LOG_INIT();

    
    if(!lidarSetup("/dev/ttyAMA0",256000)){
        LOG_ERROR("cannot find the lidar");
        return -1;
    }

#ifndef DISABLE_LIDAR
    if (gpioInitialise() < 0) {
        LOG_ERROR("cannot initialize lidar gpio speed");
        return 1;
    }
    gpioSetPWMfrequency(18, 25000);
    gpioSetMode(18, PI_OUTPUT);
    gpioSetPWMrange(18, 100);
    gpioPWM(18, 50);//lidar speed
#endif



    signal(SIGINT, ctrlc);
    signal(SIGTERM, ctrlc);
    //signal(SIGTSTP, ctrlz);
    
    tableState tableStatus;
    Asser *robotI2C = new Asser(I2C_ASSER_ADDR);
    //LOG_SETROBOT(robotI2C);
    lidarAnalize_t lidarData[SIZEDATALIDAR];    
    Arduino *arduino = new Arduino(100);
    main_State_t currentState = INIT;
    main_State_t nextState = INIT;
    bool initStat = true;
    actionContainer* actionSystem = new actionContainer(robotI2C, arduino, &tableStatus);
    int countStart = 0;
    int countSetHome = 0;

    // arduino->enableStepper(1);
    // arduino->servoPosition(1,180);
    // arduino->servoPosition(2,0);
    // arduino->moveStepper(2200,1);
    // while(!ctrl_c_pressed);
    // ctrl_c_pressed = false;
    // while(!catchPlant(arduino));
    // while(!ctrl_c_pressed);
    // ctrl_c_pressed = false;
    // while(!releasePlant(arduino));
    // while(!ctrl_c_pressed);

    // std::string colorTest = tableStatus.colorTeam == YELLOW ? "YELLOW" : "BLUE";
    // std::filesystem::path exe_pathTest = std::filesystem::canonical(std::filesystem::path(argv[0])).parent_path();
    // std::filesystem::path python_script_pathTest = exe_pathTest / "../startPAMI.py";
    // std::string commandTest = "python3 " + python_script_pathTest.string() + " " +  colorTest;
    // std::thread python_threadTest(executePythonScript,commandTest);


    while (1) {

        LOG_SCOPE("Main");
        sleep(0.01);
        
       
        int count = SIZEDATALIDAR;
        if(currentState != FIN){
            if(getlidarData(lidarData,count)){
                int x,x_ennemie=0,x_bal = 0, y,y_ennemie=0,y_bal = 0,distance,teta;
                double teta_ennemie=0, teta_bal = 0;
                robotI2C->getCoords(x,y,teta);
                position_t position = {x,y,double(teta),0}, pos_balise = {x_bal,y_bal,double(teta_bal),0}, pos_ennemie = {x_ennemie,y_ennemie,teta_ennemie,0};
                convertAngularToAxial(lidarData,count,&position,0);
                //init_position_balise(lidarData, count, &pos_balise, &pos_ennemie);
                //LOG_INFO("diff x = ",position.x, " / diff y = ", pos_balise.x  , " / ennemie = ", pos_ennemie.x, "/ ",pos_ennemie.y);
                //ennemieInAction(&tableStatus, &pos_ennemie );
                convertAngularToAxial(lidarData,count,&position,300);
                if(ctrl_z_pressed){
                    ctrl_z_pressed = false;
                    pixelArtPrint(lidarData,count,50,50,100,position);
                }                
                //printAngular(lidarData,count);
                robotI2C->getBrakingDistance(distance);
                tableStatus.robot.collide = collide(lidarData,count,distance);
                //LOG_DEBUG("collide : ", robot->collide);
                //printf("distance : %d \t collide : %d\n",distance,robot->collide);
            }
        }
       

       
       


        switch (currentState) {
            //****************************************************************
            case INIT:{
                if(initStat){ LOG_STATE("INIT");
                    int bStateCapteur2 = 0;
                    arduino->readCapteur(2,bStateCapteur2);
                    if(bStateCapteur2 == 1){
                        robotI2C->setCoords(-710,1170,90);
                    }
                    else{
                        robotI2C->setCoords(-710,-1170,-90);
                    }
                }
                int bStateCapteur3 = 0;
                int bStateCapteur1 = 0;
                arduino->readCapteur(3,bStateCapteur3);
                arduino->readCapteur(1,bStateCapteur1);
                blinkLed(arduino,2,500);
                blinkLed(arduino,1,500);
                if(bStateCapteur3 == 1 && bStateCapteur1 == 1){
                    countSetHome ++;
                }
                else{
                    countSetHome = 0;
                }
                if(countSetHome == 5){
                    nextState = INITIALIZE;
                    arduino->ledOff(2);
                    arduino->ledOff(1);
                }
                break;
            }
            //****************************************************************
            case INITIALIZE:{
                if(initStat) LOG_STATE("INITIALIZE");
                if(initStat){
                    arduino->servoPosition(4,100);
                    robotI2C->enableMotor(true);
                    robotI2C->brakeMotor(false);
                    arduino->enableStepper(1);
                    arduino->servoPosition(1,180);
                    arduino->servoPosition(2,CLAMPSLEEP);
                    arduino->moveStepper(ELEVATORUP,1);
                    robotI2C->setLinearMaxSpeed(MAX_SPEED);
                }
                int bStateCapteur2 = 0;
                arduino->readCapteur(2,bStateCapteur2);
                if(bStateCapteur2 == 1){
                    tableStatus.robot.colorTeam = YELLOW;
                    nextState = SETHOME;
                    robotI2C->setCoords(-710,1170,90);
                    LOG_INFO("teams : YELLOW");
                }
                else if(bStateCapteur2 == 0){
                    tableStatus.robot.colorTeam = BLUE;
                    nextState = SETHOME;
                    robotI2C->setCoords(-710,-1170,-90);
                    LOG_INFO("teams : BLUE");
                }
                //IF bStateCapteur2 != 1 && != 2 alors problem
                break;
            }
            //****************************************************************
            case SETHOME:{
                if(initStat) LOG_STATE("SETHOME");
                if(tableStatus.robot.colorTeam == YELLOW){
                    if(initPositon(&tableStatus,robotI2C,-800,1325,90)){
                        nextState = WAITSTART;
                    }
                }
                else{
                    if(initPositon(&tableStatus,robotI2C,-800,-1325,-90)){
                        nextState = WAITSTART;
                    }
                }
                
                break;
            }            
            case WAITSTART:{
                if(initStat) LOG_STATE("WAITSTART");
                int bStateCapteur1 = 0;
                arduino->readCapteur(1,bStateCapteur1);
                if(tableStatus.robot.colorTeam == YELLOW){
                    blinkLed(arduino,1,500);
                }
                else{
                    blinkLed(arduino,2,500);
                }
                
               if(bStateCapteur1 == 0){
                    countStart ++;
                }
                else{
                    countStart = 0;
                }
                if(countStart == 5){
                    nextState = RUN;
                    arduino->ledOff(1);
                    arduino->ledOff(2);
                    tableStatus.startTime = millis();
                    actionSystem->initAction( robotI2C, arduino, &(tableStatus));
                    //LAUNCH PYTHON
                    // std::string color = tableStatus.colorTeam == YELLOW ? "YELLOW" : "BLUE";
                    // std::filesystem::path exe_path = std::filesystem::canonical(std::filesystem::path(argv[0])).parent_path();
                    // std::filesystem::path python_script_path = exe_path / "../startPAMI.py";
                    // std::string command = "python3 " + python_script_path.string() + " " +  color;
                    // python_thread = new std::thread(executePythonScript,command);
                    //
                }
                break;
            }
            //****************************************************************
            case RUN:{
                if(initStat) LOG_STATE("RUN");
                bool finish;
                if (allJardiniereFull(&tableStatus)) {for(int i = 0; i<6;i++){ tableStatus.planteStockFull[i].etat = false;}}
                if(tableStatus.robot.colorTeam == YELLOW){
                    finish = actionSystem->actionContainerRun();
                    //finish =  FSMMatch(mainRobot,robotI2C, arduino);
                }
                else{
                    finish = actionSystem->actionContainerRun();
                    //finish =  TestPinceFSM(mainRobot,robotI2C, arduino);
                    //finish =  FSMMatch(mainRobot,robotI2C, arduino);
                }
                if(tableStatus.startTime+90000 < millis()){
                    LOG_GREEN_INFO("END BY TIMER");
                    nextState = FIN;
                }
                if(finish){
                    nextState = FIN;
                }
                break;
            }
            //****************************************************************
            case RETURNHOME:{
                if(initStat) LOG_STATE("RETURNHOME");
                bool finish =  returnToHome(robotI2C);
                if(tableStatus.startTime+900000 < millis() || finish){
                    nextState = FIN;
                }
                break;
            }
            //****************************************************************
            case FIN:
                if(initStat){
                    LOG_STATE("FIN");
                    arduino->servoPosition(4,180);
                    arduino->servoPosition(1,180);
                    arduino->servoPosition(2,CLAMPSTOP);
                    //lidarStop();
                    //gpioPWM(18, 0);
                    arduino->disableStepper(1);
                    robotI2C->enableMotor(false);
                    robotI2C->brakeMotor(true);
                    nextState = STOP;
                    LOG_INFO("Nombre points = ",tableStatus.points);
                } 
                break;
            //****************************************************************
            case STOP:
                if(initStat) LOG_STATE("STOP");
                break;
            //****************************************************************
            default:
                LOG_STATE("default");
                nextState = STOP;
                break;
        }

        initStat = false;
        if(currentState != nextState){
            initStat = true;
        }
        currentState = nextState;


        //ctrl_c_pressed |= turnSolarPannel(robotI2C,arduino);

        if (ctrl_c_pressed){ 
            break;
        }
    }

    gpioPWM(18, 0);
    arduino->servoPosition(4,180);
    arduino->ledOff(2);
    arduino->ledOff(1);
    arduino->servoPosition(1,180);
    arduino->servoPosition(2,CLAMPSTOP);
    arduino->moveStepper(0,1);
    robotI2C->enableMotor(false);
    robotI2C->brakeMotor(false);
    robotI2C->stop();
    lidarStop();
    sleep(2);
    arduino->disableStepper(1);
    LOG_DEBUG("PROCESS KILL");

    return 0;
}