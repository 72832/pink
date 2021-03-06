/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       meredithgreen                                             */
/*    Created:      Sun Apr 07 2019                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"
#include "72832Lib.h"

using namespace vex;

// A global instance of vex::competition
vex::competition Competition;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void pre_auton( void ) {

    //start the ritual
    littleCeasars();
    
    //wait for VEX to not be a dumb :P
    LF.setStopping(vex::brakeType::coast);
    LB.setStopping(vex::brakeType::coast);
    RF.setStopping(vex::brakeType::coast);
    RB.setStopping(vex::brakeType::coast);
    lift.setStopping(vex::brakeType::hold);
    puncherLeft.setStopping(vex::brakeType::brake);
    puncherRight.setStopping(vex::brakeType::brake);
    intake.setStopping(vex::brakeType::coast);


}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void autonomous( void ) {

  RAMP Ramping;

  RampInit(&Ramping);

  LF.resetRotation();
  LB.resetRotation();
  RF.resetRotation();
  RB.resetRotation();

  while(RampCompute(&Ramping, 800, LF.rotation(vex::rotationUnits::deg))!=0){
    LF.spin(vex::directionType::fwd, RampCompute(&Ramping, 800, LF.rotation(vex::rotationUnits::deg)),vex::velocityUnits::pct);
    LB.spin(vex::directionType::fwd, RampCompute(&Ramping, 800, LF.rotation(vex::rotationUnits::deg)),vex::velocityUnits::pct);
    RF.spin(vex::directionType::fwd, RampCompute(&Ramping, 800, LF.rotation(vex::rotationUnits::deg)),vex::velocityUnits::pct);
    RB.spin(vex::directionType::fwd, RampCompute(&Ramping, 800, LF.rotation(vex::rotationUnits::deg)),vex::velocityUnits::pct);
  }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void usercontrol( void ) {
  // User control code here, inside the loop
  while (1) {
    // This is the main execution loop for the user control program.
    // Each time through the loop your program should update motor + servo 
    // values based on feedback from the joysticks.

    tankStrafe();
    liftCont();
    puncherCont();
    intakeCont();
    driveReverseFunc();
 
    vex::task::sleep(20); //Sleep the task for a short amount of time to prevent wasted resources. 
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
    //Set up callbacks for autonomous and driver control periods.
    Competition.autonomous( autonomous );
    Competition.drivercontrol( usercontrol );
    
    //Run the pre-autonomous function. 
    pre_auton();
       
    //Prevent main from exiting with an infinite loop.                        
    while(1) {
      vex::task::sleep(100);//Sleep the task for a short amount of time to prevent wasted resources.
    }       
}