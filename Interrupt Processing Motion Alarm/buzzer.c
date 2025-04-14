#include "buzzer.h"

/******************************************************************************
 * Initialize the buzzer
 * Pin usage: Grove base port J17, Tiva C PC5 (Port C, Pin 5)
 *****************************************************************************/
void BuzzerInit()
{
    // Enable Port C peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    // Configure PC5 as output pin
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_5);
}
