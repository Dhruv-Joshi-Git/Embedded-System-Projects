#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "launchpad.h"
#include "seg7.h"
#include "ranger.h"

/*
 * Global variables
 */

// System state
typedef struct {
    enum {Millimeter, Inch} display_mode;       // display inch or millimeter
} SysState;

// The events
Event trigger_ranger_reading_event;

SysState sys = {Millimeter};

/*******************************************
  Trigger and read the ranger
 ******************************************/

// Trigger the ranger reading every 0.5 seconds
void TriggerRangerReading(Event *event)
{
    int btn = PushButtonRead();

    if (btn == 1) {
        sys.display_mode = Millimeter;
    }
    else if (btn == 2) {
        sys.display_mode = Inch;
    }

    //printf("Checking for new ranger data...\n");
    // Check if new ranger data is ready
    if (!RangerDataReady()) {
        RangerTriggerReading();
    } else {
        uint32_t pulse_width = RangerGetData();
        printf("Pulse width: %lu clock cycles\n", pulse_width);

        // Convert to mm
        float distance_m = (pulse_width / 50000000.0f) * (340.0f / 2.0f);
        uint32_t distance_mm = (uint32_t)(distance_m * 1000.0f);

        // Setup 7-seg display struct
        Seg7Display disp;

        if (sys.display_mode == Millimeter) {
            if (distance_mm > 9999) distance_mm = 9999;

            disp.digit[0] = distance_mm % 10;
            disp.digit[1] = (distance_mm / 10) % 10;
            disp.digit[2] = (distance_mm / 100) % 10;
            disp.digit[3] = (distance_mm / 1000) % 10;
            disp.colon_on = false;
        } else {
            uint32_t total_inches = (uint32_t)(distance_mm / 25.4f);
            uint32_t feet = total_inches / 12;
            uint32_t inches = total_inches % 12;

            disp.digit[0] = inches % 10;
            disp.digit[1] = inches / 10;
            disp.digit[2] = feet % 10;
            disp.digit[3] = feet / 10;
            disp.colon_on = true;
        }

        Seg7Update(&disp);
    }



    EventSchedule(event, event->time + 500);
}


/*******************************************
 Main function
 ******************************************/
void main(void)
{
    // Initialize the LaunchPad and peripherals
    LaunchPadInit();
    RangerInit();
    Seg7Init();

    //printf("testing.\n");
    // Initialize the events
    EventInit(&trigger_ranger_reading_event, TriggerRangerReading);

    // Schedule time event
    EventSchedule(&trigger_ranger_reading_event, 100);

    // Loop forever
    while (true)
    {
        // Wait for interrupt
        asm("   wfi");

        // Execute scheduled callbacks
        EventExecute();
    }
}
