
#ifndef MOTION_H_
#define MOTION_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>

// Initialize the motion detector
void MotionInit();

// Read input from motion sensor
int MotionInputRead();

// Registering event for motion sensor
void EventRegisterMotionSensor();

void MotionSensorISR();

#endif /* MOTION_H_ */
