# Embedded-System-Projects
- Following code/projects were created as part of an academic course at the University of Illinois Chicago 
- Using Tiva TM4C123GH6PM Micro-Controller, CCS Studio, and other necessary peripherals
- Only contains necessary source code
# Interupt Process Motion Detection 
Overview: Interrupt Processing Motion Alarm project uses the Tiva C microcontroller to detect motion through a PIR sensor using interrupt-driven I/O instead of polling. When motion is detected, the sensor triggers a GPIO interrupt, which runs an Interrupt Service Routine (ISR) to update motion state and schedule a callback event. This approach improves system responsiveness and energy efficiency, since the CPU can enter a low-power mode (wfi) and wake only when motion is detected.
# Time Captured Distance Measurement
Overview: The Time Capture (Distance Measurement) project uses the Timer Input Capture function of the Tiva C microcontroller to measure the time it takes for an ultrasonic pulse to bounce off an object and return. The time is used to compute the distance to an object. The lab is concerned with configuring hardware timers in Input Edge-Time Mode, handling the capture events in an Interrupt Service Routine (ISR), and displaying the distance in millimeters and feet/inches on a 7-segment display, with mode selection through pushbuttons.






