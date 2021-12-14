#ifndef _STEPPER_H_
#define _STEPPER_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CLASS_DIR       "/sys/class/"
#define STEPPER_DIR     CLASS_DIR "stepper/"
#define STEPPER_SPEED_FILE  STEPPER_DIR "speed"
#define STEPPER_STEPS_FILE  STEPPER_DIR "steps"
#define STEPPER_BUSY_FILE   STEPPER_DIR "busy"

#define BUFF_SIZE 20

typedef struct s_stepper
{
    int motor_speed_fd;
    int motor_steps_fd;
    int motor_busy_fd;
}              t_stepper;

int init_stepper_motor(t_stepper *stepper);
void close_stepper_motor(void);

int stepper_is_busy(void);
int stepper_read_speed(void);
int stepper_set_speed(int speed);
int stepper_rotate_steps(int steps);
int stepper_rotate_angle(int mDeg);

#endif /* _STEPPER_H_ */
