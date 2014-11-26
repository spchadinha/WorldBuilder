//
//  Menu.h
//  RobotSolution
//
//  Created by Alden Hart on 11/13/14.
//  Copyright (c) 2014 Davidson College. All rights reserved.
//

#ifndef __RobotSolution__Menu__
#define __RobotSolution__Menu__

#include <stdio.h>
#include <cstdlib>
#include "MenuOption.h"

class Menu {
public:
    // Constructor/destructor
    Menu(double x_in, double y_in, double z_in, double size);
    ~Menu();
    
    // Member functions
    //void initialize();
    void showMenu();
    void hideMenu(bool done);
    void getInteractions(Leap::Vector p);
    
    // Fields
    bool touched;
    
private:
    double x;
    double y;
    double z;
    double radius;
    int num_items;
    std::vector<MenuItem> menu_items;
    
};

#endif /* defined(__RobotSolution__Menu__) */
