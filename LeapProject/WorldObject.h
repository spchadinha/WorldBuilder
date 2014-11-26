//
//  WorldObject.h
//  LeapProject
//
//  Created by Spencer Chadinha on 11/12/14.
//  Copyright (c) 2014 Davidson. All rights reserved.
//

#ifndef __LeapProject__WorldObject__
#define __LeapProject__WorldObject__

#include <stdio.h>
#include <iostream>
#include <string.h>
#include "Leap.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <glut.h>
#endif

enum shape {CONE, CUBE, SPHERE, TORUS};

class WorldObject {
public:
    WorldObject(int, double, double, double, double);
    ~WorldObject();
    
    bool checkInteraction(Leap::Vector);
    void draw();
    void setColor(float, float, float);
    bool compColor();
    bool isWhite();
    
    double len;
    
    int type;
    
    double x;
    double y;
    double z;
    
    double xL;
    double xU;
    double yL;
    double yU;
    double zL;
    double zU;
    
    float origR;
    float origG;
    float origB;
    float r;
    float g;
    float b;
    
    bool pinched;
    
    int interactionScore;
};









#endif /* defined(__LeapProject__WorldObject__) */
