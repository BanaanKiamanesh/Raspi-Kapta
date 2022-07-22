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
#define MIN_WORKING_PWM 2250
#define MAX_WORKING_PWM 4050

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

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        int speed = std::stoi(argv[1]);

        if (speed > MAX_WORKING_PWM)
            speed = MAX_WORKING_PWM;

        else if (speed < MIN_WORKING_PWM)
            speed = MIN_WORKING_PWM;

        else
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

            for (int idx = 0; idx < 4; idx++)
            {
                pca.setPWM(pins[idx], speed);
                delay(25);
            }

            delay(250);
            std::cout << "\nAll Chennels Set to " << speed << std::endl;
        }
    }

    else
    {
        std::cerr << "Usage : " << argv[0] << " <speed>\n";
        return 1;
    }
    return 0;
}
