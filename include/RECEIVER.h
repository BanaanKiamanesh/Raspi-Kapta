#ifndef _RECEIVER_H
#define _RECEIVER_H

// Registers
#define MAIN_ADDRESS 0x08
#define MAX_PWM 2000
#define MIN_PWM 1000

class Receiver
{
private:
    int I2C_FD;
    unsigned char I2CDataBuf[16];
    float receiverData[4] = {1500, 1500, 1000, 1500};
    float alpha = 0.015;

public:
    Receiver();
    void readChannel();
    void updateData();
    int channel(int);
};

#endif