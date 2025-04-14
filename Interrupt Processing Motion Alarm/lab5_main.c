#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "launchpad.h"
#include "buzzer.h"
#include "motion.h"

/********************************************************************************
 * Global declarations
 *******************************************************************************/

// The data structure for the system state
typedef struct
{
    // Activated state: if the system is activated or not
    bool activated;

    // Alert state: If the alert is on or off
    bool alerted;

    // Sound state: If the buzzer is turned on to make a sound or not
    // Note: When the system is activated and alerted, the buzzer is turned
    // on and off periodically
    bool buzzer_on;
} SysState;

// The system state
static SysState sys = {
                        false /* not activated */,
                        false /* not alerted */,
                        false /* buzzer not turned on */
};

// The events
Event buzzer_play_event;
Event check_push_button_event;
Event check_motion_sensor;

/********************************************************************************
 If the system is activated and alerted, make the buzzer play
 * on/off sound. The buzzer will be repeatedly turned on and off.
 *
 * You may adjust the on and off times to change the sound pattern and intensity.
 *
 * Note: This is a callback function.
 *******************************************************************************/

void BuzzerPlay(Event *event)                   // the scheduled time
{
    uint32_t delay = 500;                       // default checking interval

    if (sys.alerted)
    {
        // Turn buzzer on and off
        if (sys.buzzer_on)
        {
            // The buzzer is on: turn it off
            BuzzerOff();
            sys.buzzer_on = false;
            delay = 2988;                       // off for 2988 ms
        }
        else
        {
            // The buzzer is off: turn it on
            BuzzerOn();
            sys.buzzer_on = true;
            delay = 12;                         // on for 12 ms
        }
    }
    else
    {
        // Ensure the buzzer is turned off
        if (sys.buzzer_on)
        {
            BuzzerOff();
            sys.buzzer_on = false;
        }
    }

    // Schedule callback to buzzer play
    EventSchedule(event, event->time + delay);
}

/********************************************************************************
 * Check the motion sensor input. If motion is detected, put the system
 * in the alerted state. Otherwise, put it in the non-alerted state.
 *
 * This is a callback function. In the next lab, you will re-write this function
 * to make it interrupt based.
 *******************************************************************************/
void CheckMotionSensor(Event *event)
{
    int sensorValue = MotionInputRead(); // Read the motion sensor state
    if (sys.activated)
    {
        if (sensorValue != 0)
        {
            sys.alerted = true;  //Motion detected
            LedTurnOnOff(true /* red */, false /* blue */, false /* green */);
            printf("motion detected\n");
        }
        else
        {
            sys.alerted = false;   //Motion not detected
            LedTurnOnOff(false /* red */, false /* blue */, true /* green */);
            printf("motion not detected\n");
        }
    }
    else
    {
        // Ensure buzzer is off if the system is deactivated
        sys.alerted = false;
        BuzzerOff();
    }
}

/********************************************************************************
 * Pushing SW1 activates the system and puts it in the alerted state.
 * Pushing SW2 deactivates the system.
 *
 * If the system is activated, the motion sensor input decides if the
 * system is put in the alerted state or not.
 *
 * This function is called upon interrupts. It is NOT a callback function.
 *******************************************************************************/
void CheckPushButton(Event *event)
{
    int code = PushButtonRead();        // Read the pushbutton
    switch (code)
    {
    case 1:                     // SW1 is pushed
        if (!sys.activated)
        {
            // Activate the system
            sys.activated = true;
            LedTurnOnOff(false /* red */, false /* blue */, true /* green */);
        }
        break;

    case 2:                     // SW2 is pushed
        if (sys.activated)
        {
            // Deactivate the system and turn off LED
            sys.activated = false;
            LedTurnOnOff(false /* red */, false /* blue */, false /* green */);
        }
        break;
    }
}

/********************************************************************************
 * The main function: Print out a message, schedule the first callback event(s),
 * and then run the callback scheduler.
 *******************************************************************************/
int main(void)
{
    // Initialize the Tiva C LaunchPad and peripherals
    LaunchPadInit();
    BuzzerInit();
    MotionInit();

    uprintf("%s\n\r", "Lab 5: Motion Detection Alarm");

    // Initialize the event objects
    EventInit(&check_motion_sensor, CheckMotionSensor);
    EventInit(&buzzer_play_event, BuzzerPlay);
    EventInit(&check_push_button_event, CheckPushButton);


    // Register ISR event for push button and motion sensor
    PushButtonEventRegister(&check_push_button_event);
    EventRegisterMotionSensor(&check_motion_sensor);

    // Schedule time events
    EventSchedule(&buzzer_play_event, 100);

    // Run the callback scheduler
    while (true)
    {
        // Wait for interrupt events (include callback events)
        asm("   wfi");

        // Run callback events
        EventExecute();
    }
}
