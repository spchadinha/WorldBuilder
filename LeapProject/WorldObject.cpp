//
//  WorldObject.cpp
//  LeapProject
//
//  Created by Spencer Chadinha on 11/12/14.
//  Copyright (c) 2014 Davidson. All rights reserved.
//

#include <iostream>
#include <string.h>
#include "Leap.h"
#include "WorldObject.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <glut.h>
#endif

using namespace Leap;

WorldObject::WorldObject(int shape, double xLoc, double yLoc, double zLoc, double length) {
    len = length;
    
    type = shape;
    
    x = xLoc;
    y = yLoc;
    z = zLoc;
    
    double halfLen = length/2;
    
    xL = x - halfLen;
    xU = x + halfLen;
    yL = y - halfLen;
    yU = y + halfLen;
    zL = z - halfLen;
    zU = z + halfLen;
    
    pinched = false;
    
    interactionScore = 0;
}

WorldObject::~WorldObject() {
    
}

bool WorldObject::checkInteraction(Leap::Vector p){
    double hl = len/2;
    
    if (400*(p.x-.5) >= x-hl && 400*(p.x-.5) <= x+hl && 400*(p.y-.5) >= y-hl && 400*(p.y-.5) <= y+hl) {
        return true;
    }
    else {
        return false;
    }
}

void WorldObject::draw() {
    
    if (type == 0) {
        glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidCone(len, len, 50, 50);
        glPopMatrix();
    }
    else if (type == 1) {
        glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidCube(len);
        glPopMatrix();
    }
    else if (type == 2){
        glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidSphere(len, 50, 50);
        glPopMatrix();
    }
    else {
        glPushMatrix();
        glTranslatef(x, y, z);
        glutWireTorus(len/4, len, 10, 30);
        glPopMatrix();
    }
}


void WorldObject::setColor(float red, float green, float blue) {
    origR = red;
    origG = green;
    origB = blue;
    r = red;
    g = green;
    b = blue;
}

bool WorldObject::compColor() {
    if (r == origR && g == origG && b == origB) {
        return true;
    }
    else {
        return false;
    }
}

bool WorldObject::isWhite() {
    if (r >= 1 && g >= 1 && b >= 1) {
        return true;
    }
    else {
        return false;
    }
    
}