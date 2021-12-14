#include "stepper.h"

static t_stepper *pSteper;

int init_stepper_motor(t_stepper *pStepperInit)
{
    pSteper = pStepperInit;
    pSteper->motor_speed_fd = open(STEPPER_SPEED_FILE, O_RDONLY | O_NONBLOCK);
    pSteper->motor_steps_fd = open(STEPPER_STEPS_FILE, O_RDONLY | O_NONBLOCK);
    pSteper->motor_busy_fd =  open(STEPPER_BUSY_FILE, O_RDONLY | O_NONBLOCK);

    if (pSteper->motor_speed_fd < 0 || 
        pSteper->motor_steps_fd < 0 || 
        pSteper->motor_busy_fd < 0) {
        
        if (pSteper->motor_speed_fd >= 0)
            close(pSteper->motor_speed_fd);

        if (pSteper->motor_steps_fd >= 0)
            close(pSteper->motor_steps_fd);

        if (pSteper->motor_busy_fd >= 0)
            close(pSteper->motor_busy_fd);

        printf("ERROR: can not open stepper motor files\n");

        return EXIT_FAILURE;
    }

    if(stepper_read_speed() > 0) 
        return EXIT_FAILURE;

    if(stepper_is_busy()) 
        return EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}

void close_stepper_motor(void)
{
    close(pSteper->motor_speed_fd);
    close(pSteper->motor_steps_fd);
    close(pSteper->motor_busy_fd);
}

int stepper_read_speed(void)
{
    char buff[BUFF_SIZE];
    int speed, rc;

    rc = read(pSteper->motor_speed_fd, buff, BUFF_SIZE);
    speed = atoi(buff);

    if(rc <= 0) 
        return -EXIT_FAILURE;
    
    return speed;
}

/* speed limits [1 ; 18] */
int stepper_set_speed(int speed)
{
    char buff[BUFF_SIZE];
    int rc;

    if(speed > 18) {
        speed = 18;
        printf("speed limits [1 ; 18]\n");
    }
    else if (speed < 1) {
        speed = 1;
        printf("speed limits [1 ; 18]\n");
    }

    sprintf(buff, "%d", speed);
    rc = write(pSteper->motor_speed_fd, buff, strlen(buff));

    if(rc <= 0) 
        return -EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}

int stepper_is_busy(void)
{
    char buff[BUFF_SIZE];
    int busy, rc;

    rc = read(pSteper->motor_busy_fd, buff, BUFF_SIZE);
    busy = atoi(buff);

    if(rc <= 0) 
        return -EXIT_FAILURE;
    
    return busy;
}

int stepper_rotate_steps(int steps)
{
    char buff[BUFF_SIZE];
    int rc;

    sprintf(buff, "%d", steps);
    rc = write(pSteper->motor_steps_fd, buff, strlen(buff));

    if(rc <= 0)
        return -EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}

int stepper_rotate_angle(int mDeg)
{
    int steps;

    steps = mDeg * 64 / 5625;
    if(stepper_rotate_steps(steps))
        return -EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}
