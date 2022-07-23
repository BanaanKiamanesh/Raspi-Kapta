#include "../../include/BNO055.h"
#include <fstream>
#include <iostream>
#include <wiringPi.h>

using namespace std;

int main()
{
    BNO055 imu;
    imu.update();

    cout << "\nPut the device in a stable and balanced place\nand don't touch it!\n";

    vector eul;
    long double x = 0, y = 0, z = 0;
    int numSamples = 10000;

    for (int i = 0; i < numSamples; i++)
    {
        eul = imu.read_euler();

        x += eul.x;
        y += eul.y;
        z += eul.z;

        delay(4);
    }

    x /= numSamples;
    z /= numSamples;
    y /= numSamples;

    cout << "\nX offSet = " << x << "  - Y offSet = " << y << "  - Z offSet = " << z << endl;

    cout << "\nCreating the File..." << endl;
    ofstream out("../../data/IMUOffSets.dat");

    cout << "\nSaving..." << endl;

    out << x << "\n"
        << y << "\n"
        << z << "\n";

    cout << "\nEnd!" << endl;
    return 0;
}
