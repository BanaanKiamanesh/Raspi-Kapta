#include "../../include/RECEIVER.h"
#include <iostream>
#include <wiringPi.h>
#include <wiringPiI2C.h>

using namespace std;

int main()
{
    Receiver rx;

    while (1)
    {
        rx.updateData();

        std::cout << "Pitch : " << rx.channel(1);
        std::cout << " - Roll : " << rx.channel(2);
        std::cout << " - Throttle : " << rx.channel(3);
        std::cout << " - Yaw : " << rx.channel(4) << "\n";

        delay(4);
    }

    return 0;
}
