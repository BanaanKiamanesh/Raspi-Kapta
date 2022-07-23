// I2C Transmission Vars
#include <Wire.h>
#define MY_ADDR 0x08

char charBuf[16] = "1500150010001500";

// Receiver Read Vars
byte channelState[4] = {0, 0, 0, 0};
unsigned long timer[4] = {0, 0, 0, 0}, current_time;
int receiverInput[4] = {0, 0, 0, 0};

#define MAX_PWM 2000
#define MIN_PWM 1000

void setup()
{
    TWBR = 12;
    Wire.begin(MY_ADDR);
    Wire.onRequest(requestEvent); // register event

    InterruptInit();
}

void loop() {}

void requestEvent()
{
    itoa(receiverInput[0], charBuf, 10);
    itoa(receiverInput[1], charBuf + 4, 10);
    itoa(receiverInput[2], charBuf + 8, 10);
    itoa(receiverInput[3], charBuf + 12, 10);

    Wire.write(charBuf);
}

void InterruptInit()
{
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT0);
    PCMSK0 |= (1 << PCINT1);
    PCMSK0 |= (1 << PCINT2);
    PCMSK0 |= (1 << PCINT3);
}

ISR(PCINT0_vect)
{
    current_time = micros();

    // Channel 1
    if (PINB & B00000001)
    {
        if (channelState[0] == 0)
        {
            channelState[0] = 1;
            timer[0] = current_time;
        }
    }

    else if (channelState[0] == 1)
    {
        channelState[0] = 0;
        receiverInput[0] = current_time - timer[0];

        if (receiverInput[0] > MAX_PWM)
            receiverInput[0] = MAX_PWM;

        else if (receiverInput[0] < MIN_PWM)
            receiverInput[0] = MIN_PWM;
    }

    // Channel 2
    if (PINB & B00000010)
    {
        if (channelState[1] == 0)
        {
            channelState[1] = 1;
            timer[1] = current_time;
        }
    }

    else if (channelState[1] == 1)
    {
        channelState[1] = 0;
        receiverInput[1] = current_time - timer[1];

        if (receiverInput[1] > MAX_PWM)
            receiverInput[1] = MAX_PWM;

        else if (receiverInput[1] < MIN_PWM)
            receiverInput[1] = MIN_PWM;
    }

    // Channel 3
    if (PINB & B00000100)
    {
        if (channelState[2] == 0)
        {
            channelState[2] = 1;
            timer[2] = current_time;
        }
    }

    else if (channelState[2] == 1)
    {
        channelState[2] = 0;
        receiverInput[2] = current_time - timer[2];

        if (receiverInput[2] > MAX_PWM)
            receiverInput[2] = MAX_PWM;

        else if (receiverInput[2] < MIN_PWM)
            receiverInput[2] = MIN_PWM;
    }

    // Channel 4
    if (PINB & B00001000)
    {
        if (channelState[3] == 0)
        {
            channelState[3] = 1;
            timer[3] = current_time;
        }
    }

    else if (channelState[3] == 1)
    {
        channelState[3] = 0;
        receiverInput[3] = current_time - timer[3];

        if (receiverInput[3] > MAX_PWM)
            receiverInput[3] = MAX_PWM;

        else if (receiverInput[3] < MIN_PWM)
            receiverInput[3] = MIN_PWM;
    }
}