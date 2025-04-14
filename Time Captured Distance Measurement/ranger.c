#include "ranger.h"

/*
 * Hardware configure: The ultrasonic ranger (and its signal pin) shall be connected to:
 *      Grove baseboard: Port J6, Pin 24
 *      Tiva C: GPIO PD1
 *      Timer pin: WT2CCP1
 */

/*
 * Global/static variables and declarations
 */
typedef struct {
    Event *callback_event;
    enum {
        IDLE,                   // ranger is idle
        EXPECT_START_EVENT_1,   // expecting the rising edge of the start pulse
        EXPECT_START_EVENT_2,   // expecting the falling edge of the start pulse
        EXPECT_DATA_EVENT_1,    // expecting the rising edge of the data pulse
        EXPECT_DATA_EVENT_2     // expecting the falling edge of the data pulse
    } state;
    bool new_data_ready;
    uint32_t rising_time;
    uint32_t falling_time;
    uint32_t pulse_width;
} RangerState;

volatile RangerState ranger;

void RangerISR();

/*
 * Initialize the ranger. It should be connected to Grove jumper J13 (SIG connected to
 * pin 40). The GPIO pin is PF2, and the time pin is T1CCP0. The timer is TIMER1 and the
 * sub-Time is A.
 */
void RangerInit()
{
    // Initial ranger state in memory
    ranger.callback_event = NULL;
    ranger.new_data_ready = false;

    // Enable GPIO Port D and Wide Timer 2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER2);

    // Configure PD2 as timer pin and connect it to WT2CCP1
    // Do NOT enable the time as this time
    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PD1_WT2CCP1);

    // Count both edges as event
    TimerControlLevel(WTIMER2_BASE, TIMER_B, true);
    TimerControlEvent(WTIMER2_BASE, TIMER_B, TIMER_EVENT_BOTH_EDGES);

    // Register an ISR to deal with the timeout event
    TimerIntRegister(WTIMER2_BASE, TIMER_B, RangerISR);
    TimerIntEnable(WTIMER2_BASE, TIMER_CAPB_EVENT);
}

/*
 * Send the start pulse to the ultrasonic ranger. The pin being used will be switched
 * to timer input pin at the end.
 */
void RangerTriggerReading()
{
    // Clean data ready flag and set the ranger state
    ranger.new_data_ready = false;
    ranger.state = EXPECT_START_EVENT_1;

    // It is safe to disable a peripheral during configuration
    TimerDisable(WTIMER2_BASE, TIMER_B);

    // Configure WT2CCP1 for PWM to generate a pulse of 5 microseconds, with
    // 2 microseconds of leading low time
    TimerConfigure(WTIMER2_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PWM);
    TimerLoadSet(WTIMER2_BASE, TIMER_B, 1000);
    TimerMatchSet(WTIMER2_BASE, TIMER_B, 500);
    TimerIntClear(WTIMER2_BASE, TIMER_CAPB_EVENT);

    // Enable the timer, and start PWM waveform
    TimerEnable(WTIMER2_BASE, TIMER_B);
}

/*
 * If any new data is ready (for polling)
 */
bool RangerDataReady()
{
    //printf("DataReady check: %d\n", ranger.new_data_ready);
    return ranger.new_data_ready;
}

void RangerISR()
{
    //printf("ISR fired, state = %d\n", ranger.state);
    switch (ranger.state)
    {
    case EXPECT_START_EVENT_1:
        ranger.state = EXPECT_START_EVENT_2;
        break;

    case EXPECT_START_EVENT_2:
       // Disable the timer before reconfigaration
        TimerDisable(WTIMER2_BASE, TIMER_B);

        // Reconfigure WT2CCP1 for time capture (rising edge)
        TimerConfigure(WTIMER2_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_CAP_TIME_UP);
        TimerControlEvent(WTIMER2_BASE, TIMER_B, TIMER_EVENT_POS_EDGE);  // Rising edge first
        TimerLoadSet(WTIMER2_BASE, TIMER_B, 0xFFFFFFFF);  // Set large load value so timer doesnt overflow

        // Clear and enable interrupt again
        TimerIntClear(WTIMER2_BASE, TIMER_CAPB_EVENT);
        TimerEnable(WTIMER2_BASE, TIMER_B);


        ranger.state = EXPECT_DATA_EVENT_1;
        break;

    case EXPECT_DATA_EVENT_1:
        // Capture the rising edge timestamp
        ranger.rising_time = TimerValueGet(WTIMER2_BASE, TIMER_B);


        // Reconfigure to capture the falling edge next
        TimerControlEvent(WTIMER2_BASE, TIMER_B, TIMER_EVENT_NEG_EDGE);

        //move to next
        ranger.state = EXPECT_DATA_EVENT_2;
        break;

    case EXPECT_DATA_EVENT_2:

        //printf("Echo falling edge detected Data event 2 is triggered. \n");
        // Capture the falling edge timestamp
        ranger.falling_time = TimerValueGet(WTIMER2_BASE, TIMER_B);

        // Compute pulse width in clock cycles
        if (ranger.falling_time > ranger.rising_time) {
            ranger.pulse_width = ranger.falling_time - ranger.rising_time;
        } else {
            ranger.pulse_width = (0xFFFFFFFF - ranger.rising_time) + ranger.falling_time;
        }
        //printf("%lu clock cycles. \n", ranger.pulse_width);

        // Mark data as ready
        ranger.new_data_ready = true;

        //printf("DataReady check: %d\n", ranger.new_data_ready);

        ranger.state = IDLE;
        break;
    }

    TimerIntClear(WTIMER2_BASE, TIMER_CAPB_EVENT);
}


uint32_t RangerGetData()
{
    //printf("GetData called. Pulse = %lu\n", ranger.pulse_width);
    ranger.new_data_ready = false;   // Clear the data ready flag
    return ranger.pulse_width;       // Return the pulse width in clock cycles
}
