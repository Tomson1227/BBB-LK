#ifndef PIN_DEFINITION_H
#define PIN_DEFINITION_H

#define AM335_GPIO(bank,line)  (32 * bank + line)

#define HCSR04_INPUT    AM335_GPIO(1, 15)       // PIN 47
#define HCSR04_OUTPUT   AM335_GPIO(0, 27)       // PIN 27

#endif /* PIN_DEFINITION_H */
