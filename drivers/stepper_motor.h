#ifndef PIN_DEFINITION_H
#define PIN_DEFINITION_H

#define AM335_GPIO(bank,line)  (32 * bank + line)

#define STEPPER_OUT_0      AM335_GPIO(1, 12)       // PIN 44
#define STEPPER_OUT_1      AM335_GPIO(0, 26)       // PIN 26
#define STEPPER_OUT_2      AM335_GPIO(1, 14)       // PIN 46
#define STEPPER_OUT_3      AM335_GPIO(2, 1)        // PIN 65

#endif /* PIN_DEFINITION_H */
