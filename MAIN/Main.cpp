#include "../include/BNO055.h"
#include "../include/PCA9685.h"
#include "../include/PID.h"
#include "../include/RECEIVER.h"
#include <fstream>
#include <iostream>
#include <wiringPi.h>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;

#define MAX_PITCH 60
#define MAX_ROLL 60

#define MIN_MOTOR_PWM 2250
#define MAX_MOTOR_PWM 3300

/*
````` Global Vars `````
*/
float imuOffsets[3];
int motorPins[4];

float xKp, yKp, zKp;
float xKd, yKd, zKd;
float xKi, yKi, zKi;
float xTau, yTau, zTau;
float pidMin, pidMax;

/*
````` Functions `````
*/
void ReadIMUOffsets();
void ReadMotorPins();
void ReadControllerGains();
float map(float, float, float, float, float);

int main()
{
    /*
    ````` IMU Properties `````
    */

    BNO055 imu;
    imu.update();
    vector eul;
    float pitchAngle, rollAngle, yawAngle;

    ReadIMUOffsets();

    /*
    ````` Motor Driver Properties `````
    */

    PCA9685 pca;
    pca.setPWMFreq(500); // Set Motor Driver PWM Frequency to 500 Hz

    ReadMotorPins();

    int motorSpeeds[4] = {MIN_MOTOR_PWM, MIN_MOTOR_PWM, MIN_MOTOR_PWM, MIN_MOTOR_PWM};
    /*
    ````` Radio Receiver Properties `````
    */

    Receiver rx;
    rx.updateData();
    int pitchInput, rollInput, yawInput, throttle;

    delay(200);

    /*
    ````` Loop Properties `````
    */

    const double loop_freq = 50;
    const double loop_time_s = (1 / loop_freq);
    const double loop_time_us = loop_time_s * 1'000'000;
    unsigned long loop_timer;

    /*
    ````` Controller Properties `````
    */

    float pitchDesired, rollDesired, yawDesired;
    ReadControllerGains();

    PID rollPID;
    PID pitchPID;
    PID yawPID;

    rollPID.init(xKp, xKi, xKd, xTau, loop_freq);
    pitchPID.init(yKp, yKi, yKd, yTau, loop_freq);
    yawPID.init(zKp, zKi, zKd, zTau, loop_freq);

    rollPID.set_bounds(pidMin, pidMax);
    pitchPID.set_bounds(pidMin, pidMax);
    yawPID.set_bounds(pidMin, pidMax);

    float rollCorrection, pitchCorrection, yawCorrection;

    loop_timer = micros();

    while (true)
    {
        // Read IMU Feedback
        eul = imu.read_euler();
        pitchAngle = eul.x - imuOffsets[0];
        rollAngle = eul.y - imuOffsets[1];
        yawAngle = eul.z;

        if (yawAngle > 180)
            yawAngle -= 360;

        // Read Radio Receiver Data
        rx.updateData();
        pitchInput = rx.channel(1);
        rollInput = rx.channel(2);
        yawInput = rx.channel(4);
        throttle = rx.channel(3);

        // Map Receiver Data Into the Desired Domain
        pitchDesired = map(pitchInput, 1000, 2000, -MAX_PITCH, MAX_PITCH);
        rollDesired = map(rollInput, 1000, 2000, -MAX_ROLL, MAX_ROLL);
        yawDesired = map(yawInput, 1000, 2000, 0, 360);

        // Calculate PID Vals
        rollCorrection = rollPID.update(rollDesired, rollAngle);
        pitchCorrection = pitchPID.update(pitchDesired, pitchAngle);
        yawCorrection = yawPID.update(yawDesired, yawAngle);

        // Motor Speed Mixing Algorithm
        motorSpeeds[0] = throttle - rollCorrection - pitchCorrection + yawCorrection;
        motorSpeeds[1] = throttle + rollCorrection - pitchCorrection - yawCorrection;
        motorSpeeds[2] = throttle + rollCorrection + pitchCorrection + yawCorrection;
        motorSpeeds[3] = throttle - rollCorrection + pitchCorrection - yawCorrection;

        // Constrain Motor Speed and Send them
        for (int i = 0; i < 4; i++)
        {
            if (motorSpeeds[i] > MAX_MOTOR_PWM)
                motorSpeeds[i] = MAX_MOTOR_PWM;

            if (motorSpeeds[i] < MIN_MOTOR_PWM)
                motorSpeeds[i] = MIN_MOTOR_PWM;

            pca.setPWM(motorPins[i], motorSpeeds[i]);
        }

        // UnComment for Simple Testing...
        // cout << "Roll Correction:" << rollCorrection << "  - Pitch Correction:" << pitchCorrection << "   - yaw Correction:" << yawCorrection << endl;
        // cout << "roll:" << rollDesired << "  - Pitch:" << pitchDesired << "   - yaw:" << yawDesired << endl;
        // cout << "Pitch:" << pitchAngle << "-Roll:" << rollAngle << "-Yaw:" << yawAngle << "-PitchIn:" << pitchInput << "-RollIn:" << rollInput << "-YawIn:" << yawInput << "-Throt:" << throttle << endl;

        // Loop Time set stuff...
        while (micros() - loop_timer < loop_time_us)
            ;
        loop_timer = micros();
    }

    return 0;
}

void ReadIMUOffsets()
{   // function to read IMU OFFSETS from the Data File
    char filename[] = "../data/IMUOffSets.dat";

    ifstream indata;

    indata.open(filename);
    if (!indata)
    { // file couldn't be opened
        cerr << "Error: 'IMUOffSets.dat' could not be opened, or doesn't exist." << endl;
        exit(1);
    }

    indata >> imuOffsets[0];
    indata >> imuOffsets[1];
    indata >> imuOffsets[2];

    indata.close();
}

void ReadMotorPins()
{    // function to read Motor Pins From Data File
    char filename[] = "../data/motorPins.dat";

    ifstream indata;

    indata.open(filename);
    if (!indata)
    { // file couldn't be opened
        cerr << "Error: 'motorPins.dat' could not be opened, or doesn't exist." << endl;
        exit(1);
    }

    indata >> motorPins[0];
    indata >> motorPins[1];
    indata >> motorPins[2];
    indata >> motorPins[3];

    indata.close();
}

float map(float x, float inMin, float inMax, float outMin, float outMax)
{
    return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

void ReadControllerGains()
{ // function to read Controller Gains From Data File
    ifstream indata;

    indata.open("../data/xPIDvals.dat");
    if (!indata)
    { // file couldn't be opened
        cerr << "Error: 'xPIDvals.dat' could not be opened, or doesn't exist." << endl;
        exit(1);
    }

    indata >> xKp;
    indata >> xKi;
    indata >> xKd;
    indata >> xTau;

    indata.close();

    /////////////////////////////////////////////////////

    indata.open("../data/xPIDvals.dat");
    if (!indata)
    { // file couldn't be opened
        cerr << "Error: 'yPIDvals.dat' could not be opened, or doesn't exist." << endl;
        exit(1);
    }

    indata >> yKp;
    indata >> yKi;
    indata >> yKd;
    indata >> yTau;

    indata.close();

    /////////////////////////////////////////////////////

    indata.open("../data/zPIDvals.dat");
    if (!indata)
    { // file couldn't be opened
        cerr << "Error: 'zPIDvals.dat' could not be opened, or doesn't exist." << endl;
        exit(1);
    }

    indata >> zKp;
    indata >> zKi;
    indata >> zKd;
    indata >> zTau;

    indata.close();

    /////////////////////////////////////////////////////

    indata.open("../data/PIDLims.dat");
    if (!indata)
    { // file couldn't be opened
        cerr << "Error: 'PIDLims.dat' could not be opened, or doesn't exist." << endl;
        exit(1);
    }

    indata >> pidMin;
    indata >> pidMax;

    indata.close();
}