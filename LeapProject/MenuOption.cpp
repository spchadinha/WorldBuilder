//
//  MenuOption.cpp
//  RobotSolution
//
//  Created by Alden Hart on 11/13/14.
//  Copyright (c) 2014 Davidson College. All rights reserved.
//

#include "MenuOption.h"
#include "Leap.h"
#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <glut.h>
#endif

MenuItem::MenuItem() {
    
}

MenuItem::MenuItem(int type, double x_in, double y_in, double z_in, double size) {
    x = x_in;
    y = y_in;
    z = z_in;
    r = 0;
    g = 0.7;
    b = 1.0;
    len = size;
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(r, g, b);
    glutSolidSphere(len, 20, 20);
    glPopMatrix();
}

MenuItem::~MenuItem() {
}

bool MenuItem::checkInteraction(Leap::Vector p){
    if (400*(p.x-.5) >= (x-len/2) && 400*(p.x-.5) <= (x+len/2) && 400*(p.y-.5) >= (y-len/2) && 400*(p.y-.5) <= (y+len/2)) {
        return true;
    }
    else {
        return false;
    }
}

void MenuItem::setR(double red) {
    r = red;
}

void MenuItem::setG(double green) {
    g = green;
}

void MenuItem::setB(double blue) {
    b = blue;
}

double MenuItem::getR() {
    return r;
}

double MenuItem::getG() {
    return g;
}

double MenuItem::getB() {
    return b;
}