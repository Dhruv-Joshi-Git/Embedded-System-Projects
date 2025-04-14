#include "motion.h"
#include "launchpad.h"
#include <driverlib/rom_map.h>
#include <driverlib/rom.h>
#include <driverlib/gpio.h>

// Motion sensor input state
typedef struct
{
    Event *callback_event;
    bool new_input;
    uint32_t pin_value;
} MotionSensorState;

MotionSensorState motion_sensor;

void MotionInit()
{
    motion_sensor.callback_event = NULL;
    motion_sensor.pin_value = 0;
    motion_sensor.new_input = 0;

    // Enable Port C peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    // Configure PC4 as input pin
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);

    GPIOIntRegister(GPIO_PORTC_BASE, MotionSensorISR); // Register the ISR
    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_BOTH_EDGES); // Set interrupt on both edges
    IntPrioritySet(INT_GPIOC, 0); // Set interrupt level to 0 (highest priority)
    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_PIN_4); // Enable interrupt for PC4
}

void EventRegisterMotionSensor(Event *callback_event)
{
    assert(EventInitialized(callback_event));
    motion_sensor.callback_event = callback_event;
}

void MotionSensorISR()
{
    int pin_value = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);

    if (pin_value != 0)
    {
        motion_sensor.pin_value = 1; // Cond. for motion detected
    }
    else
    {
        motion_sensor.pin_value = 0; // Cond. for no motion detected
    }
    motion_sensor.new_input = true;

    if (motion_sensor.callback_event != NULL)
    {
        EventSchedule(motion_sensor.callback_event, EventGetCurrentTime());
    }

    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);

}
