/******************************************************************************\
 * Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
 * Leap Motion proprietary and confidential. Not for distribution.              *
 * Use subject to the terms of the Leap Motion SDK Agreement available at       *
 * https://developer.leapmotion.com/sdk_agreement, or another agreement         *
 * between Leap Motion and you, your company or other organization.             *
 \******************************************************************************/

#include <iostream>
#include <string.h>
#include <unistd.h>
#include "Leap.h"
#include "WorldObject.h"
#include "Menu.h"
#include "MenuOption.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <glut.h>
#endif

using namespace Leap;

float win_width = 700;
float win_height = 700;

/*#####################################################################################################################
 Leap Code */

HandList hands;
Hand hand;
Hand pastHand;
Arm arm;
InteractionBox box;

std::vector<WorldObject> tapObjects;
std::vector<WorldObject> menuItems;
std::vector<WorldObject> objectMenu;
int selected_obj = -1;
Menu optionMenu(0, 0, 0, 0);


float xColor = 1;
float yColor = 1;
float zColor = 1;

float cRed = 1, cGreen = .5, cBlue = 1;

bool tapped = false;
Vector tapPos;
float tapRotate = 0;

bool menu_open = false;
bool obj_menu_open = false;

float pitch;
float yaw;
float roll;

int menu_selected = -1;
int obj_menu_selected = -1;

void makeMenu(int num_items, int r, double x, double y, bool m) {
    double new_x;
    double new_y;
    double angle = 0;
    double dAngle = 2*M_PI/num_items;
    
    for (int i = 0; i < num_items; i++) {
        new_x = r*cos(angle) + x;
        new_y = r*sin(angle) + y;
        angle += dAngle;
        WorldObject option(2, new_x, new_y, -10, 10);
        option.r = 1;
        option.g = 0;
        option.b = 0;
        if (m) {
            objectMenu.push_back(option);
        }
        else {
            menuItems.push_back(option);
        }
        
    }
}

class SampleListener : public Listener {
public:
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
    virtual void onDeviceChange(const Controller&);
    virtual void onServiceConnect(const Controller&);
    virtual void onServiceDisconnect(const Controller&);
    
private:
};

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

void SampleListener::onInit(const Controller& controller) {
    std::cout << "Initialized" << std::endl;
}

void SampleListener::onConnect(const Controller& controller) {
    std::cout << "Connected" << std::endl;
    controller.enableGesture(Gesture::TYPE_CIRCLE);
    controller.enableGesture(Gesture::TYPE_KEY_TAP);
    controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
    controller.enableGesture(Gesture::TYPE_SWIPE);
    
}

void SampleListener::onDisconnect(const Controller& controller) {
    // Note: not dispatched when running in a debugger.
    std::cout << "Disconnected" << std::endl;
}

void SampleListener::onExit(const Controller& controller) {
    std::cout << "Exited" << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {
    // Get the most recent frame and report some basic information
    const Frame frame = controller.frame();
    const Frame pastFrame = controller.frame(4);
    box = frame.interactionBox();
    
    HandList pastHands = pastFrame.hands();
    for (HandList::const_iterator h = pastHands.begin(); h != pastHands.end(); ++h){
        pastHand = *h;
    }
    hands = frame.hands();
    for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
        // Get the first hand
        hand = *hl;
        std::string handType = hand.isLeft() ? "Left hand" : "Right hand";

        const Vector normal = hand.palmNormal();
        const Vector direction = hand.direction();
        
        // Get the Arm bone
        arm = hand.arm();
        
        // Get fingers
        const FingerList fingers = hand.fingers();
        for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
            const Finger finger = *fl;
            
            // Get finger bones
            for (int b = 0; b < 4; ++b) {
                Bone::Type boneType = static_cast<Bone::Type>(b);
                Bone bone = finger.bone(boneType);
            }
        }
    }
    
    // Get tools
    const ToolList tools = frame.tools();
    for (ToolList::const_iterator tl = tools.begin(); tl != tools.end(); ++tl) {
        const Tool tool = *tl;
    }
    
    // Get gestures
    const GestureList gestures = frame.gestures();
    for (int g = 0; g < gestures.count(); ++g) {
        Gesture gesture = gestures[g];
        
        switch (gesture.type()) {
            case Gesture::TYPE_CIRCLE:
            {
                CircleGesture circle = gesture;
                std::string clockwiseness;
                if (tapObjects.size()>0) {
                    
                
                    if (circle.pointable().direction().angleTo(circle.normal()) <= PI/2) {
                        clockwiseness = "clockwise";
                        if (obj_menu_selected == 0) {
                            tapObjects[selected_obj].len += .05;
                        }
                        else {
                            tapObjects[selected_obj].r += .01;
                            tapObjects[selected_obj].g += .01;
                            tapObjects[selected_obj].b += .01;
                        }
                    
                    } else {
                        clockwiseness = "counterclockwise";
                        if (obj_menu_selected == 0) {
                            tapObjects[selected_obj].len -= .05;
                        }
                        else {
                            tapObjects[selected_obj].r -= .01;
                            tapObjects[selected_obj].g -= .01;
                            tapObjects[selected_obj].b -= .01;
                        }
                    }
                }
                // Calculate angle swept since last frame
                float sweptAngle = 0;
                if (circle.state() != Gesture::STATE_START) {
                    CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
                    sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
                }
                break;
            }
            case Gesture::TYPE_SWIPE:
            {
                SwipeGesture swipe = gesture;

                Vector swipeDirection = swipe.direction();
                // Swipe up to show menu
                if (swipeDirection.y > 0.7 && !menu_open) {
                    menu_open = true;
                    makeMenu(5, 180, 0, 0, false);
                }
                // Swipe down to hide menu
                if (swipeDirection.y < -0.7 && abs(swipeDirection.x) < 0.2) {
                    menu_open = false;
                    obj_menu_open = false;
                    if (selected_obj >=0) {
                        tapObjects[selected_obj].interactionScore = 0;
                    }
                    selected_obj = -1;
                    menuItems.clear();
                    objectMenu.clear();
                }
                // Swipe left to clear screen
                if (swipeDirection.x < -0.7) {
                    tapObjects.clear();
                }
                
                xColor = drand48();
                yColor = drand48();
                zColor = drand48();
                break;
            }
            case Gesture::TYPE_KEY_TAP:
            {
                KeyTapGesture tap = gesture;
                //                std::cout << std::string(2, ' ')
                //                << "Key Tap id: " << gesture.id()
                //                << ", state: " << stateNames[gesture.state()]
                //                << ", position: " << tap.position()
                //                << ", direction: " << tap.direction()<< std::endl;
                std::cout << "get tapped\n";
                tapped = true;
                tapPos = box.normalizePoint(tap.position());
//                int obj = floor(4*drand48());
                int obj = menu_selected;
                std::cout << obj << std::endl;
                WorldObject s(obj, 400*(tapPos.x-.5), 400*(tapPos.y-.5), 400*(tapPos.z-.5), 13);
                s.setColor(drand48(), drand48(), drand48());
                tapObjects.push_back(s);
                
                break;
            }
            case Gesture::TYPE_SCREEN_TAP:
            {
                ScreenTapGesture screentap = gesture;
                //                std::cout << std::string(2, ' ')
                //                << "Screen Tap id: " << gesture.id()
                //                << ", state: " << stateNames[gesture.state()]
                //                << ", position: " << screentap.position()
                //                << ", direction: " << screentap.direction()<< std::endl;
                break;
            }
            default:
                std::cout << std::string(2, ' ')  << "Unknown gesture type." << std::endl;
                break;
        }
    }
    
    if (!frame.hands().isEmpty() || !gestures.isEmpty()) {
        std::cout << std::endl;
    }
}

void SampleListener::onFocusGained(const Controller& controller) {
    //std::cout << "Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Controller& controller) {
    //std::cout << "Focus Lost" << std::endl;
}

void SampleListener::onDeviceChange(const Controller& controller) {
    //std::cout << "Device Changed" << std::endl;
    const DeviceList devices = controller.devices();
    
    for (int i = 0; i < devices.count(); ++i) {
        //        std::cout << "id: " << devices[i].toString() << std::endl;
        //        std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
    }
}

void SampleListener::onServiceConnect(const Controller& controller) {
    //std::cout << "Service Connected" << std::endl;
}

void SampleListener::onServiceDisconnect(const Controller& controller) {
    //std::cout << "Service Disconnected" << std::endl;
}


/*#####################################################################################################################
 OpenGL Code */

void init( void ){
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glShadeModel (GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    
    GLfloat light_ambient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_position[] = { 6.0f, 10.0f, 10.0f, 0.0f };
    
    
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    GLfloat mat_specular[] = {.15, .15, .15, .15};
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    GLfloat mat_ambient[] = {0.35, 0.35, 0.35, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT,  mat_ambient);
    
    GLfloat shininess[] = {.55};
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void idle( void ){
    
    //    pitch = 180/PI * hand.direction().pitch();
    //    yaw = 180/PI * hand.direction().yaw();
    //    roll = 180/PI * hand.direction().roll();
    //    //std::cout << "pitch: " << pitch << " yaw: " << yaw << " roll: " << roll << std::endl;
    
    Vector f = box.normalizePoint(hand.fingers()[1].tipPosition());
    Vector th = box.normalizePoint(hand.fingers()[0].tipPosition());
    
    
    //std::cout << f.x;
    
    for (int i = 0; i < tapObjects.size(); i++) {
        //std::cout << "tip x: " << 400*(f.x-.5) << " obj: " << tapObjects[i].x << std::endl;
//        if (tapObjects[i].checkInteraction(f) && !tapObjects[i].isWhite()) {
//            tapObjects[i].r += .01;
//            tapObjects[i].g += .01;
//            tapObjects[i].b += .01;
//            if (tapObjects[i].isWhite()) {
//                selected = i;
//                std::cout << "We have selected some object YAYYYY!" << std::endl;
//                //std::cout << selected;
//            }
//        }
        
        if (hand.pinchStrength()>.8 && tapObjects[i].checkInteraction(th)) {
            //std::cout << "obj coords: " << tapObjects[i].x << " " << tapObjects[i].y << " " << tapObjects[i].z << " thumb coords: " << 400*(th.x-.5) << " " << 400*(th.y-.5) << " " << 400*(th.z-.5) << std::endl;
            tapObjects[i].pinched = true;
        }
        
        if (tapObjects[i].pinched) {
            tapObjects[i].x = 400*(th.x-.5);
            tapObjects[i].y = 400*(th.y-.5);
            tapObjects[i].z = 400*(th.z-.5);
        }
        
        if (hand.pinchStrength() < .8) {
            tapObjects[i].pinched = false;
        }

        
        if (tapObjects[i].checkInteraction(f) && !obj_menu_open) {
            tapObjects[i].interactionScore++;
            if (tapObjects[i].interactionScore > 99 && !obj_menu_open && hand.pinchStrength()<.8 && selected_obj < 0) {
                obj_menu_open = true;
                makeMenu(3, 60, tapObjects[i].x, tapObjects[i].y, true);
                selected_obj = i;
            }
        }
        
        
        
        
    }
    
    //std::cout << "vector size: " << objectMenu.size() << " selected: " << obj_menu_selected << std::endl;
    if (menu_open) {
        for (int i = 0; i < menuItems.size(); i++) {
            if (menuItems[i].checkInteraction(f)) {
                menuItems[i].r = 1;
                menuItems[i].g = 1;
                menuItems[i].b = 1;
                menu_selected = i;
            }
            if (i != menu_selected) {
                menuItems[i].g = 0;
                menuItems[i].b = 0;
            }
        }
    }
    
    if (obj_menu_open) {
        for (int i = 0; i < objectMenu.size(); i++) {
            if (objectMenu[i].checkInteraction(f)) {
                objectMenu[i].r = 1;
                objectMenu[i].g = 1;
                objectMenu[i].b = 0;
                obj_menu_selected = i;
            }
            if (i != obj_menu_selected) {
                objectMenu[i].g = 0;
                objectMenu[i].b = 0;
            }
        }
    }
    
    ///////////////////////////
    // Menu closing operations
    ///////////////////////////
    // These checks must be the last things to happen in idle, otherwise the program will crash
    
    if (menu_selected == 4) {
        std::cout << "You chose to quit!" << std::endl;
        exit(0);
    }
    
//    if (obj_menu_selected == 2) {
//        std::cout << selected_obj << " " << tapObjects.size() << std::endl;
//        tapObjects.erase(tapObjects.begin()+selected_obj);
//        obj_menu_open = false;
//        objectMenu.clear();
//        selected_obj = -1;
//    }
    
    glutPostRedisplay();
}

void display( void ) 
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    //gluPerspective(100.f, win_width/win_height, .1f, 500.f);
    glOrtho(-200, 200, -200, 200, .1, 1000);
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_LINE, GL_FILL);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 15, 0, -1, 0, 0, 1, 0);
    
    Vector palmPos = box.normalizePoint(hand.palmPosition());
    double palmX = 400*(palmPos.x - .5);
    double palmY = 400*(palmPos.y - .5);
    double palmZ = 400*(palmPos.z - .5);
    
    //////////////////////////////////
    // Draw palm torus
    //////////////////////////////////
    glPushMatrix();
    //glColor3f(.4, .1, .2);
    GLfloat mat_diffuse[] = {0.4, 0.1, 0.2, 1.0};
    glMaterialfv(GL_FRONT, GL_DIFFUSE,  mat_diffuse);
    glTranslatef(palmX, palmY, palmZ);
    glutSolidSphere(10, 40, 40);
    
    glRotatef(90, 1, 0, 0); //rotate palm about x
    //glRotatef(yaw, 0, 1, 0); // rotate palm about y
    glRotatef(-roll, 0, 1, 0); // rotate palm about z
    //glColor3f(.6, .4, .5);
    GLfloat mat_diff[] = {0.6, 0.4, 0.5, 1.0};
    glMaterialfv(GL_FRONT, GL_DIFFUSE,  mat_diff);
    glutWireTorus(10, 16, 40, 40);
    glPopMatrix();
    
    //    Matrix handTransform = hand.basis();
    //    handTransform.origin - hand.palmPosition();
    //    handTransform = handTransform.rigidInverse();
    
    for (int i = 0; i < hand.fingers().count(); i++) {
        Vector fPos = box.normalizePoint(hand.fingers()[i].tipPosition());
        double tipX = 400*(fPos.x - .5);
        double tipY = 400*(fPos.y - .5);
        double tipZ = 400*(fPos.z - .5);
        
        Leap::Bone bone;
        Leap::Bone::Type boneType;
        double prevX = palmX;
        double prevY = palmY;
        double prevZ = palmZ;
        for(int b = 0; b < 4; b++) {
            boneType = static_cast<Leap::Bone::Type>(b);
            bone = hand.fingers()[i].bone(boneType);
            
            Vector normalizedBone = box.normalizePoint(bone.center());
            double tipX = 400*(normalizedBone.x - 0.5);
            double tipY = 400*(normalizedBone.y - 0.5);
            double tipZ = 400*(normalizedBone.z - 0.5);
            
            glPushMatrix();
            
            glBegin(GL_LINES);
            glLineWidth(50);
            glColor3b(0.8, 0.8, 0.2);
            glVertex3f(prevX, prevY, prevZ);
            glVertex3f(tipX, tipY, tipZ);
            glEnd();
            GLfloat bone_diff[] = {0.8, 0.8, 0.2, 1.0};
            glMaterialfv(GL_FRONT, GL_DIFFUSE,  bone_diff);
            glTranslatef(tipX, tipY, tipZ);
            glutSolidSphere(3, 20, 20);
            
            glPopMatrix();
            
            prevX = tipX;
            prevY = tipY;
            prevZ = tipZ;
            
        }
        
        glPushMatrix();
        glTranslatef(tipX, tipY, tipZ);
        //glColor3f(xColor, yColor, zColor);
        GLfloat mat_diffuse[] = {xColor, yColor, zColor, 1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE,  mat_diffuse);
        glutSolidSphere(4, 40, 40);
        glPopMatrix();
    }
    
//    WorldObject thing(4, 12, 7, -9, 20);
//    thing.r = cRed;
//    thing.g = .2;
//    thing.b = .4;
//    glPushMatrix();
//    //glTranslatef(12, 7, -1);
//    GLfloat mat_dif[] = {thing.r, thing.g, thing.b, 1.0};
//    glMaterialfv(GL_FRONT, GL_DIFFUSE,  mat_dif);
//    glColor3f(thing.r, thing.g, thing.b);
//    thing.draw();
//    glPopMatrix();
    
    
    
    //    if (tapped) {
    //        int t = floor(4*drand48());
    //        WorldObject sphere(t, 400*(tapPos.x-.5), 400*(tapPos.y-.5), 400*(tapPos.z-.5), 14);
    //        sphere.r = drand48();
    //        tapObjects.push_back(sphere);
    //    }
    //    //tapped = false;
    
    //18446744073709551615
    //std::cout << tapObjects.size() << std::endl;
    //usleep(1000);
    for (int i = 0; i < tapObjects.size(); i++) {
        //glRotatef(tapRotate, 0, 0, 1);
        GLfloat mat_diffuse[] = {tapObjects[i].r, tapObjects[i].g, tapObjects[i].b, 1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE,  mat_diffuse);
        glColor3f(tapObjects[i].r, tapObjects[i].g, tapObjects[i].b);
        tapObjects[i].draw();
    }
    
    if (menu_open) {
//        GLfloat mat_diffuse[] = {.9, .9, .9, 1.0};
//        glMaterialfv(GL_FRONT, GL_DIFFUSE,  mat_diffuse);
//        optionMenu.showMenu();
        for (int i = 0; i < menuItems.size(); i++) {
            GLfloat menu_dif[] = {menuItems[i].r, menuItems[i].g, menuItems[i].b, 1.0};
            glMaterialfv(GL_FRONT, GL_DIFFUSE,  menu_dif);
            menuItems[i].draw();
        }
    }
    
    if (obj_menu_open) {
        for (int i = 0; i < objectMenu.size(); i++) {
            GLfloat obj_menu_dif[] = {objectMenu[i].r, objectMenu[i].g, objectMenu[i].b, 1.0};
            glMaterialfv(GL_FRONT, GL_DIFFUSE,  obj_menu_dif);
            objectMenu[i].draw();
        }
    }
    
    
    
    glutSwapBuffers();
    
}

void reshape( int w, int h )
{
    win_width = w;
    win_height = h;
    glViewport( 0, 0, w, h );
    
    glutPostRedisplay();
}

void keyboard( unsigned char key, int x, int y ) {
    switch(key) {
        case 27: // Escape key
            exit(0); break;
    }
    glutPostRedisplay();
}

/*#####################################################################################################################
 Main */

int main(int argc, char** argv) {
    // Create a sample listener and controller
    SampleListener listener;
    Controller controller;
    
    srand48(time(0));
    
    // Have the sample listener receive events from the controller
    controller.addListener(listener);
    
    if (argc > 1 && strcmp(argv[1], "--bg") == 0)
        controller.setPolicyFlags(Leap::Controller::POLICY_BACKGROUND_FRAMES);
    
    
    
    
    glutInit( &argc, argv );
    
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize( win_width, win_height );
    
    glutCreateWindow( "Universe" );
    
    init();
    
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    //glutSpecialFunc(special);
    glutIdleFunc( idle );
    glutMainLoop();
    
    
    
    
    
    
    // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();
    
    // Remove the sample listener when done
    controller.removeListener(listener);
    
    return 0;
}

