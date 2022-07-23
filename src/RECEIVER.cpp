#include "../include/RECEIVER.h"
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

Receiver::Receiver() // Funtion to Primarily Init the operation and power registers
{
    I2C_FD = wiringPiI2CSetup(MAIN_ADDRESS);
    if (I2C_FD == -1)
    {
        std::cout << "Device Not Found!";
        return;
    }
}

void Receiver::readChannel()
{
    int bytes2Read = 16;

    int i;
    int bytesRead = bytes2Read;
    int rr = 500;

    bytesRead = 0;
    i = read(I2C_FD, I2CDataBuf, bytes2Read);

    if (i > 0)
        bytesRead += i;

    while (--rr && bytesRead < bytes2Read)
    {
        i = read(I2C_FD, &(I2CDataBuf[bytesRead]), bytes2Read - bytesRead);
        if (i > 0)
            bytesRead += i;
    }
}

void Receiver::updateData()
{
    readChannel();
    int tmpReceiverData[4] = {0, 0, 0, 0};

    // turning Ascii to Numbers
    for (int idx = 0; idx < 16; idx++)
        I2CDataBuf[idx] -= 48;

    int counter = 3;
    for (int i = 0; i < 16; i++)
    {
        int idx = (int)i / 4;
        tmpReceiverData[idx] += I2CDataBuf[i] * pow(10, counter);

        if (counter == 0)
            counter = 3;
        else
            counter--;
    }

    for (int i = 0; i < 4; i++)
        receiverData[i] = (1 - alpha) * receiverData[i] + tmpReceiverData[i] * alpha;
}

int Receiver::channel(int chnnl)
{
    chnnl--;

    if (chnnl > 3)
        chnnl = 3;
    else if (chnnl < 0)
        chnnl = 0;

    float data = receiverData[chnnl];

    if (data > MAX_PWM || data < MIN_PWM)
    {
        switch (chnnl)
        {
        case 2:
            data = 1000;
            break;

        default:
            data = 1500;
            break;
        }
    }

    return data;
}