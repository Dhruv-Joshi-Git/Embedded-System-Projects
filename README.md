# Embedded-System-Projects
- Following code was created as part of an academic course at University of Illinois Chicago 
- Using Tiva TM4C123GH6PM Micro-Controller, CCS Studio, and other necessary peripherals
- Only contains necessary source code
# Interupt Process Motion Detection 
Overview: Interrupt Processing Motion Alarm project uses the Tiva C microcontroller to detect motion through a PIR sensor using interrupt-driven I/O instead of polling. When motion is detected, the sensor triggers a GPIO interrupt, which runs an Interrupt Service Routine (ISR) to update motion state and schedule a callback event. This approach improves system responsiveness and energy efficiency, since the CPU can enter a low-power mode (wfi) and wake only when motion is detected.





