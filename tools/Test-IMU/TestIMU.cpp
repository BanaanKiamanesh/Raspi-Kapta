#include "../../include/BNO055.h"
#include <iostream>
#include <wiringPi.h>
#include <wiringPiI2C.h>

int main()
{

    BNO055 imu;
    imu.update();

    while (true)
        imu.print_state();

    return 0;
}