//
//  MenuOption.h
//  RobotSolution
//
//  Created by Alden Hart on 11/13/14.
//  Copyright (c) 2014 Davidson College. All rights reserved.
//

#ifndef __RobotSolution__MenuOption__
#define __RobotSolution__MenuOption__

#include <stdio.h>
#include <iostream>
#include <string.h>
#include "Leap.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <glut.h>
#endif

class MenuItem {
    
public:
    MenuItem();
    MenuItem(int type, double x_in, double y_in, double z_in, double size);
    ~MenuItem();
    
    void MakeObject();
    void appear();
    bool checkInteraction(Leap::Vector p);
    
    void setR(double red);
    void setG(double green);
    void setB(double blue);
    
    double getR();
    double getG();
    double getB();
    
private:
    
    enum types {CONE, CUBE, SPHERE, TORUS, QUIT};
    double x;
    double y;
    double z;
    double len;
    bool touched;
    double r;
    double g;
    double b;
};

#endif /* defined(__RobotSolution__MenuOption__) */
