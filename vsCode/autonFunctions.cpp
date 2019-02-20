vex::brain Brain;
vex::controller Controller1 = vex::controller();

// Drive
vex::motor leftFront = vex::motor(vex::PORT1,vex::gearSetting::ratio18_1,false);
vex::motor leftBack = vex::motor(vex::PORT2,vex::gearSetting::ratio18_1,false);
vex::motor rightFront = vex::motor(vex::PORT9,vex::gearSetting::ratio18_1,true); // True = reversed
vex::motor rightBack = vex::motor(vex::PORT10,vex::gearSetting::ratio18_1,true);
// Intake
vex::motor intakeLeft = vex::motor(vex::PORT3,vex::gearSetting::ratio18_1,false);
vex::motor intakeRight = vex::motor(vex::PORT4,vex::gearSetting::ratio18_1,true);
// Puncher
vex::motor puncherLeft = vex::motor(vex::PORT5,vex::gearSetting::ratio36_1,true);
vex::motor puncherRight = vex::motor(vex::PORT6,vex::gearSetting::ratio36_1,false);

#include <cmath>

// Motivational quote
void littleCeasars(){
    Controller1.Screen.print("lEtS gEt tHiS bReAd");
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                           User Control Functions                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

// Drive
void drive(){
    leftFront.spin(vex::directionType::fwd, Controller1.Axis3.value(), vex::velocityUnits::pct);
    leftBack.spin(vex::directionType::fwd, Controller1.Axis3.value(), vex::velocityUnits::pct);
    rightFront.spin(vex::directionType::fwd, Controller1.Axis2.value(), vex::velocityUnits::pct);
    rightBack.spin(vex::directionType::fwd, Controller1.Axis2.value(), vex::velocityUnits::pct);
}
void driveLock(){
    if(Controller1.ButtonX.pressing()){
        leftFront.stop(vex::brakeType::hold);
        leftBack.stop(vex::brakeType::hold);
        rightFront.stop(vex::brakeType::hold);
        rightBack.stop(vex::brakeType::hold);
    }
    else if(Controller1.ButtonY.pressing()){
        leftFront.stop(vex::brakeType::coast);
        leftBack.stop(vex::brakeType::coast);
        rightFront.stop(vex::brakeType::coast);
        rightBack.stop(vex::brakeType::coast);
    }
}

// Intake
void intake(){
    if(Controller1.ButtonR2.pressing()){ // Intake ball
        intakeLeft.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct);
        intakeRight.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct);
    }
    else if (Controller1.ButtonL1.pressing()){ // Reverse intake
        intakeLeft.spin(vex::directionType::fwd, -100, vex::velocityUnits::pct);
        intakeRight.spin(vex::directionType::fwd, -100, vex::velocityUnits::pct);
    }
    else if (Controller1.ButtonL2.pressing()){ // Run reversed bottom roller only
        intakeRight.spin(vex::directionType::fwd, -100, vex::velocityUnits::pct);
    }
    else {
        intakeLeft.stop(vex::brakeType::coast);
        intakeRight.stop(vex::brakeType::coast);
    }
}

// Puncher
void punch(){
    if(Controller1.ButtonR1.pressing()){
        puncherLeft.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct);
        puncherRight.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct);
    }
    else {
        puncherLeft.stop(vex::brakeType::hold);
        puncherRight.stop(vex::brakeType::hold);
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                           Autonomous Functions                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                       Autonomous Drive Functions                          */
/*---------------------------------------------------------------------------*/
double sgn(double number){
    if (number > 0) number = 1;
    else number = -1;
    return number;
}

void setLeftFrontPower (int power){
    if(power==0) leftFront.stop(vex::brakeType::brake);
    else{
        leftFront.spin(vex::directionType::fwd,power,vex::velocityUnits::pct);
    }
}
void setLeftBackPower (int power){
    if(power==0) leftBack.stop(vex::brakeType::brake);
    else{
        leftBack.spin(vex::directionType::fwd,power,vex::velocityUnits::pct);
    }
}
void setRightFrontPower (int power){
    if(power==0) rightFront.stop(vex::brakeType::brake);
    else{
        rightFront.spin(vex::directionType::fwd,power,vex::velocityUnits::pct);
    }
}
void setRightBackPower (int power){
    if(power==0) rightBack.stop(vex::brakeType::brake);
    else{
        rightBack.spin(vex::directionType::fwd,power,vex::velocityUnits::pct);
    }
}

void setDrivePower (int left, int right){
    setLeftFrontPower(left);
    setLeftBackPower(left);
    setRightFrontPower(right);
    setRightBackPower(right);
}

void autoDrive(int distance, int power = 100){
    int direction = sgn(distance);
        
    leftFront.resetRotation();
    leftBack.resetRotation();
    rightFront.resetRotation();
    rightBack.resetRotation();
    
    while(std::abs(leftFront.rotation(vex::rotationUnits::deg))<std::abs(distance)){
        setDrivePower(power*direction, power*direction);
    }
    setDrivePower(0,0);
}

void autoTurn(int distance, int power = 60){
    int direction = sgn(distance);
        
    leftFront.resetRotation();
    leftBack.resetRotation();
    rightFront.resetRotation();
    rightBack.resetRotation();
    
    while(std::abs(leftFront.rotation(vex::rotationUnits::deg))<std::abs(distance)){
        setDrivePower(-power*direction, power*direction);
    }
    setDrivePower(0,0);
}

// Drive with ramping
bool DriveRampingEnabled;

class Ramping{
    public:
    int ChangePct=1; // The amout of Pct change per loop
    int ChangeMsec=1; // The amount of time in between loops

    int RequestedPct=0; // Used to request Pct value change
    int Pct=0; // Pct output
    int MinUpPct=0; // Used as moveing up StillSpeed
    int MinDownPct=0; // Used as moving down StillSpeed
    int MaxPct=100; // the max pct value

    Ramping(); // Object specifer

    Ramping(int CP,int CM,int MaxP=100,int MinDP=0,int MinUP=0){
        // ChangePct,ChangeMsec,MaxPct
        ChangePct=CP;
        ChangeMsec=CM;
        MaxPct=MaxP;
        MinUpPct=MinUP;
        MinDownPct=MinDP;
    }

    void TaskRun(){
        if(RequestedPct>Pct){ // Ramp up
            Pct=Pct+ChangePct;
        }
        else if(RequestedPct<Pct){ // Ramp down
            Pct=Pct-ChangePct;
        }
        // Limit Pct
        if(Pct>MaxPct)	Pct=MaxPct;
        if(Pct<-MaxPct)	Pct=-MaxPct;
        if(Pct>0 && Pct<MinUpPct) Pct=MinUpPct;
        if(Pct<0 && Pct>MinDownPct)    Pct=MinDownPct;
    }
}; // End of task

Ramping leftRamp(1,8); // First value = pct of pwr change after each interval passed
Ramping rightRamp(1,8); // Second value = number of Msec between each pct change

    int Drive_Ramping(){
        DriveRampingEnabled=true;
        while(DriveRampingEnabled){
            leftRamp.TaskRun();
            rightRamp.TaskRun();
            setDrivePower(leftRamp.Pct,rightRamp.Pct);
            vex::task::sleep(leftRamp.ChangeMsec);
        }
        return 1;
    }

void SetDRpower(int Lpower,int Rpower){ // DMR
    leftRamp.RequestedPct = Lpower;
    rightRamp.RequestedPct = Rpower;
}
void DI(int Lpower,int Rpower){
    leftRamp.RequestedPct=Lpower; // Tells ramping to run at inputed power
    rightRamp.RequestedPct=Rpower; 
    
    leftRamp.Pct=Lpower; // Instantly changes the ramping pct to desired power instead of ramping up
    rightRamp.Pct=Rpower;
    setDrivePower(leftRamp.Pct,rightRamp.Pct);
}

// Drive ramping with auto straightening
void driveRamp(double Distance,int Pct=75,int EndWait=200, int Correction=1){
        // Update ramping speed
        leftRamp.ChangeMsec = 8;
        rightRamp.ChangeMsec = 8;

        double Direction=sgn(Distance);
        int Lpower=0;
        int Rpower=0;
    
        // Clear encoder
        leftFront.resetRotation();
        leftBack.resetRotation();
        rightFront.resetRotation();
        rightBack.resetRotation();
    
        // Is it there yet?
        while(std::abs(leftFront.rotation(vex::rotationUnits::deg))<std::abs(Distance)){
            double LEncValue=leftFront.rotation(vex::rotationUnits::deg);
            double REncValue=rightBack.rotation(vex::rotationUnits::deg);
            // Straighten
            if(std::abs(LEncValue)>std::abs(REncValue)){
                Lpower=Pct-Correction;
                Rpower=Pct;
            }
            else if(std::abs(LEncValue)<std::abs(REncValue)){
                Lpower=Pct;
                Rpower=Pct-Correction;
            }
            else if(std::abs(LEncValue)==std::abs(REncValue)){
                Lpower=Pct;
                Rpower=Pct;
            }
            // Correct direction
            Lpower=Lpower*Direction;
            Rpower=Rpower*Direction;
            // Send to SetDRpower
            SetDRpower(Lpower,Rpower);
            vex::task::sleep(1);
        }
        SetDRpower(0,0);
        vex::task::sleep(EndWait);
    }

void rampTurn(double deg,int LPowerSend=40,int RPowerSend=40,int waitTime=200){ //-left,+right
        int Dir=sgn(deg);
        deg=std::abs(deg)/12.56;
        LPowerSend=LPowerSend*Dir;
        RPowerSend=RPowerSend*Dir;

        leftFront.resetRotation();
        leftBack.resetRotation();
        rightFront.resetRotation();
        rightBack.resetRotation();

        while((std::abs(leftFront.rotation(vex::rotationUnits::rev))) <std::abs(deg)){
            DI(LPowerSend,-RPowerSend);

            vex::task::sleep(1);
        }
        DI(0,0);
        vex::task::sleep(waitTime);
}

/*---------------------------------------------------------------------------*/
/*                       Autonomous Intake Functions                         */
/*---------------------------------------------------------------------------*/
void setIntakeLeftPower (int power){
    if(power==0) intakeLeft.stop(vex::brakeType::coast);
    else{
        intakeLeft.spin(vex::directionType::fwd,power,vex::velocityUnits::pct);
    }
}
void setIntakeRightPower (int power){
    if(power==0) intakeRight.stop(vex::brakeType::coast);
    else{
        intakeRight.spin(vex::directionType::fwd,power,vex::velocityUnits::pct);
    }
}
void setIntakePower (int left, int right){
    setIntakeLeftPower(left);
    setIntakeRightPower(right);
}

void autoIn(int rotations, int power = 100){
    int direction = sgn(rotations);
        
    intakeLeft.resetRotation();
    intakeRight.resetRotation();
    
    while(std::abs(intakeLeft.rotation(vex::rotationUnits::deg))<std::abs(rotations)){
        setIntakePower(power*direction, power*direction);
    }
    setIntakePower(0,0);
}
void autoIntakeHalf(int rotations, int power = 100){
    int direction = sgn(rotations);
    
    intakeRight.resetRotation();
    
    while(std::abs(intakeLeft.rotation(vex::rotationUnits::deg))<std::abs(rotations)){
        setIntakePower(power*direction, power*direction);
    }
    setIntakeRightPower(0);
}

// Auto intake
void autoIntake(bool ON, bool In){
    if(ON){
        if(In){
            setIntakePower(100,100);
        } 
        if(!In){
            setIntakePower(-100,-100);
        } 
    }
    if(!ON){
        intakeLeft.stop();
        intakeRight.stop();
    }
}

/*---------------------------------------------------------------------------*/
/*                      Autonomous Puncher Functions                         */
/*---------------------------------------------------------------------------*/
void setPuncherLeftPower (int power){
    if(power==0) puncherLeft.stop(vex::brakeType::hold);
    else{
        puncherLeft.spin(vex::directionType::fwd,power,vex::velocityUnits::pct);
    }
}
void setPuncherRightPower (int power){
    if(power==0) puncherRight.stop(vex::brakeType::hold);
    else{
        puncherRight.spin(vex::directionType::fwd,power,vex::velocityUnits::pct);
    }
}
void setPuncherPower (int left, int right){
    setPuncherLeftPower(left);
    setPuncherRightPower(right);
}

void autoShoot(int rotations = 360, int power = 100){
    int direction = sgn(rotations);
        
    puncherLeft.resetRotation();
    puncherRight.resetRotation();
    
    while(std::abs(puncherLeft.rotation(vex::rotationUnits::deg))<std::abs(rotations)){
        setPuncherPower(power*direction, power*direction);
    }
    setPuncherPower(0,0);
}
void pullBack(int rotations = 180, int power = 100){
    int direction = sgn(rotations);
        
    puncherLeft.resetRotation();
    puncherRight.resetRotation();
    
    while(std::abs(puncherLeft.rotation(vex::rotationUnits::deg))<std::abs(rotations)){
        setPuncherPower(power*direction, power*direction);
    }
    setPuncherPower(0,0);
}