        ; Assembly functions related to buzzer

        ; Include names declared in C
        .cdecls "buzzer.h"

        .text

        ; Buzzer signal pin usage: Grove base port J17, Tiva C PC5 (Port C, Pin 5)

        ; Declare a field to store GPIO_PORTC_BASE (32-bit value) in program memory
_GPIO_PORTC_BASE     .field  GPIO_PORTC_BASE

        ; C prototype: void buzzOn()
        ; Turn on the buzzer. It calls GPIOPinWrite() to write 1 to the signal pin.
        .global BuzzerOn
        .asmfunc

BuzzerOn
        PUSH    {LR}                    ; Save return address

        ; GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_PIN_5)
        LDR     r0, _GPIO_PORTC_BASE
        MOV     r1, #GPIO_PIN_5
        MOV     r2, #GPIO_PIN_5
        BL      GPIOPinWrite

        POP     {PC}                    ; Return
        .endasmfunc

        ; C prototype: void buzzOff()
        ; Turn off the buzzer. It calls GPIOPinWrite() to write 0 to the signal pin.
        .global BuzzerOff
        .asmfunc

BuzzerOff
        PUSH    {LR}                    ; Save return address

        ; GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0)
        LDR     r0, _GPIO_PORTC_BASE
        MOV     r1, #GPIO_PIN_5
        MOV     r2, #0
        BL      GPIOPinWrite

        POP     {PC}                    ; Return
        .endasmfunc
