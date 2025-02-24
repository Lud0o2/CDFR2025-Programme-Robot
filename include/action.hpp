#pragma once

#include <functional>
#include <string>
#include "deplacement.h"
#include "arduinoSubFonction.h"
#include "fonction.h"
#include "tableState.hpp"
#include "constante.h"

class action;

typedef int (*FuncRunPtr)(action*, Asser*, Arduino*, tableState*);
typedef int (*FuncValidPtr)(tableState*);


class action
{
private: 


    typedef enum{
        FSMACTION_INIT,
        FSMACTION_MOVESTART,
        FSMACTION_ACTION,
        FSMACTION_MOVEEND
    }fsmAction_t;

private:
    Asser* robot;
    Arduino* arduino;
    tableState* table;

    std::function<int(action*, Asser*, Arduino*, tableState*)> runActionPtr;
    int validActionPtr;
    std::function<void(tableState*)> goodEndPtr;
    std::function<void(tableState*)> badEndPtr;

    position_t startPostion;
    asser_direction_side startDirection;
    asser_rotation_side startRotation;

    bool noEndPoint = true;
    position_t endPostion;
    asser_direction_side endDirection;
    asser_rotation_side endRotation;

    fsmAction_t currentState = FSMACTION_INIT;
    bool initStat = true;

    std::string actionName;
    bool actionEnable = true;

    unsigned long keyMoment;
    bool keyMomentSet = false;
    bool noTetaStart = false;

public:
    action(std::string name, Asser* irobot, Arduino* iarduino, tableState* itable);
    int runAction(void);
    void setRunAction(std::function<int(action*, Asser*, Arduino*, tableState*)> ptr);
    void setStartPoint(int x, int y, int teta, asser_direction_side Direction, asser_rotation_side rotation);
    void setStartPoint(int x, int y, asser_direction_side Direction, asser_rotation_side rotation);
    void setEndPoint(int x, int y, int teta, asser_direction_side Direction, asser_rotation_side rotation);
    int costAction(void);
    void goodEnd(std::function<void(tableState*)> ptr);
    void badEnd(std::function<void(tableState*)> ptr);
    void setCostAction(int num_action, int num_i_action, tableState *itable);
    void resetActionEnable(void);
    void setKeyMoment(unsigned long keyMom);
    bool actionNeedForce(void);
    std::string getName(void);
    ~action();

    friend std::ostream& operator<<(std::ostream& os, action& obj) {
        os << obj.getName();
        return os;
    }

private:
    int goToStart(void);
    int goToEnd(void);
};
