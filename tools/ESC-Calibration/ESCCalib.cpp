#include "../../include/PCA9685.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;

// declare MAX and MIN Available PWM Length for the ESCs
#define MIN_PWM 2048
#define MAX_PWM 4095

char filename[] = "../../data/motorPins.dat";

// Motor Pins properties
int pins[4];

void readMotorPins()
{ // Func to read Motor Pins from File
    ifstream indata;
    int num;

    indata.open(filename);
    if (!indata)
    { // file couldn't be opened
        cerr << "Error: file could not be opened, or doesn't exist." << endl;
        exit(1);
    }

    indata >> num;

    for (int idx = 0; idx < 4; idx++)
    {
        pins[idx] = num;
        indata >> num;
    }
    indata.close();
}

int main()
{
    readMotorPins();

    PCA9685 pca;

    // Check if Motor Driver is Connected
    int dev_status = pca.getFD();
    if (dev_status == -1)
    {
        std::cout << "\n\nDevice Not Found!\n\n";
        return 1;
    }

    pca.setPWMFreq(500); // Set Motor Driver PWM Frequency to 500 Hz

    std::cout << "\n========================================================";
    std::cout << "\nMotor Caliberation Sequence Starting!... \n";

    for (int i = 0; i < 4; i++) // Set Throttle to the Highest
    {
        pca.setPWM(pins[i], MAX_PWM);
        delay(25);
    }
    std::cout << "Throttle Set to the Highest!\n";

    // Ask the User to Connect the Motors
    char motor_check = 0;
    while (true)
    {
        std::cout << "\nConnect the Motors and type Y or y!\n";
        std::cin >> motor_check;
        if (motor_check == 'y' || motor_check == 'Y')
            break;
    }
    delay(3000); // Wait a bit

    // Set to the Min Throttle
    for (int i = 0; i < 4; i++)
    {
        pca.setPWM(pins[i], MIN_PWM);
        delay(25);
    }

    std::cout << "\n\nESCs Caliberated!\n";
    std::cout << "========================================================\n\n";
    delay(1500); // Calibration Sequence Ended

    // Test Rotation
    for (int i = 0; i < 4; i++)
        pca.setPWM(pins[i], 4000);

    delay(50);

    for (int i = 0; i < 4; i++)
        pca.setPWM(pins[i], MIN_PWM);

    return 0;
}
