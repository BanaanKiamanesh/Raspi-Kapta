#ifndef _RECEIVER_H
#define _RECEIVER_H

// Registers
#define MAIN_ADDRESS 0x08

class Receiver
{
private:
    int I2C_FD;
    unsigned char I2CDataBuf[16];
    int receiverData[4];

public:
    Receiver();
    void readChannel();
    void updateData();
    int channel(int);
};

#endif