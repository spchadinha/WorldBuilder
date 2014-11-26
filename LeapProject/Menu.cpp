//
//  Menu.cpp
//  RobotSolution
//
//  Created by Alden Hart on 11/13/14.
//  Copyright (c) 2014 Davidson College. All rights reserved.
//
#include "Leap.h"
#include "Menu.h"
#include "MenuOption.h"
#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <glut.h>
#endif

using namespace Leap;

Menu::Menu(double x_in, double y_in, double z_in, double size) {
    touched = false;
    radius = size;
    x = x_in;
    y = y_in;
    z = z_in;
    num_items = 5;
}

Menu::~Menu() {
    
}

//void Menu::initialize() {
//    glPushMatrix();
//    glTranslatef(x, y, z);
//    glColor3f(0, 1, 0);
//    glutSolidSphere(radius, 20, 20);
//    glPopMatrix();
//}

void Menu::showMenu() {
    double new_x; double new_y;
    double angle = 0;
    double dAngle = 2*M_PI/num_items;
    
    for (int i = 0; i < num_items; i++) {
        new_x = 0;//cos(angle);
        new_y = 0;//sin(angle);
        angle += dAngle;
        MenuItem option(i, new_x, new_y, -10, 10);
        menu_items.push_back(option);
        //        glPushMatrix();
        //        glTranslatef(new_x, new_y, -5);
        //        glColor3f(0, 0, 1);
        //        glutSolidSphere(10, 30, 30);
        //        glPopMatrix();
    }
}

void Menu::hideMenu(bool done) {
    
}

void Menu::getInteractions(Leap::Vector p) {
    for (std::vector<MenuItem>::iterator iter = menu_items.begin(); iter != menu_items.end(); ++iter) {
        // Iterate through the menu items and check if they're being hovered over
        if ((*iter).checkInteraction(p)) {
            (*iter).setR(1);
            (*iter).setG(0);
            (*iter).setB(0);
        }
    }
}