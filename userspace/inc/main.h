#ifndef _MAIN_H_
#define _MAIN_H_

#include "hc-sr04.h"
#include "hmc5883l.h"
#include "stepper.h"

typedef enum e_mode {
    MODE_1 = 0,
    MODE_2,
    MODE_3,
    MODE_4,
    MODE_5,
    MODE_6,
    MODE_7
}            t_mode;

typedef struct s_devices {
    t_mode mode;
    t_hmc5883l  hmc5883l;
    t_stepper   stepper; 
    t_hc_sr04   hc_sr04; 
    int repeat;
}              t_devices;

int init_device(t_devices *device);
void close_device(t_devices *device);
void scan_sircul(int mDegrees);
void take_current_position(void);
void scan_sector(int mOffset, int mAngle, int mDegree);

#endif /* _MAIN_H_ */
