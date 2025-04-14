        ; To include names declared in C
        .cdecls "buzzer.h"

        .text

        ; Field pointer to motion sensor variable in .c, which is a global variable declared
        .ref motion_sensor
_motion_sensor  .field  motion_sensor

        .global MotionInputRead
        .asmfunc

MotionInputRead
        PUSH    {lr}                    ; Save link register

        ; Load the address of motion_sensor into R0 register
        LDR     r0, _motion_sensor

        ; Load new_input value (stored at byte offset #8 inside the motion_sensor structure)
        LDRB    r1, [r0, #8]

        ; Compare input with 0 to check for motion
        CMP     r1, #0
        BEQ     no_motion               ; If zero, no motion detected

        ; Else motion detected, return 1
        MOV     r0, #1
        B       clear_input

no_motion:
        ; No motion detected, return 0
        MOV     r0, #0
        B       done

clear_input:
        ; Clear input flag (handling edge case)
        MOVS    r2, #0
        STRB    r2, [r0, #8]
        B       done

done:
        POP     {lr}                    ; Restore link register
        BX      lr                      ; Return from function

        .endasmfunc
