#include "main.h"

int init_device(t_devices *device)
{
    int rc = 0;
    device->repeat = 1;
    rc = init_hms5883l(&device->hmc5883l);
    
    if(rc != EXIT_SUCCESS)
        goto INIT_FAIL_1;
    
    rc = init_stepper_motor(&device->stepper);
    
    if(rc != EXIT_SUCCESS)
        goto INIT_FAIL_2;
        
    rc = init_hc_sr04(&device->hc_sr04);

    if(rc != EXIT_SUCCESS)
        goto INIT_FAIL_3;

    return EXIT_SUCCESS;

INIT_FAIL_3: close_hms5883l();
INIT_FAIL_2: close_stepper_motor();
INIT_FAIL_1: return -EXIT_FAILURE;
}

void close_device(t_devices *device)
{
    close_hms5883l();
    close_stepper_motor();
    close_hc_sr04();
}

/*
*   @brief Get current space position
*/
void take_current_position(void)
{
    int X_axis, Y_axis, Z_axis;
    X_axis = read_X_axis();
    Y_axis = read_Y_axis();
    Z_axis = read_Z_axis();
    printf("| X axis | Y axis | Z axis |\n");
    printf("| %6d | %6d | %6d |\n", X_axis, Y_axis, Z_axis);
}

/*
*   @brief Scan distance in sector
*
*   @param mOffset  start engle (can be less zero) [mili Degrees]
*   @param mAngle   sector angle [mili Degrees]
*   @param mDegre   step engle [mili Degrees]
*/
void scan_sector(int mOffset, int mAngle, int mDegree)
{
    int distance, X_axis, Y_axis, Z_axis;
    int BackAngle = -1 * (mOffset + mAngle);
    int count = 0;
    int steps = mAngle / mDegree;

    stepper_rotate_angle(mOffset);

    printf("/*********************************************************\\\n");
    printf("| Angle [mdeg] | X axis | Y axis | Z axis | Distance [mm] |\n");

    while(stepper_is_busy());

    for(; count < steps; ++count) {
        distance = read_distance();
        X_axis = read_X_axis();
        Y_axis = read_Y_axis();
        Z_axis = read_Z_axis();
        printf("| %8d.%3d | %6d | %6d | %6d | %13d |\n", 
            mAngle / 100, mAngle % 100, X_axis, Y_axis, Z_axis, distance);

        stepper_rotate_angle(mDegree);
        while(stepper_is_busy());

        mAngle += mDegree;
    }
    
    printf("\\*********************************************************/\n");

    stepper_rotate_angle(BackAngle);
    while(stepper_is_busy());
}

/*
*   @brief Scan distance by circule
*
*   @param mDegrees start engle [mili Degrees]
*/
void scan_sircul(int mDegrees)
{
    int distance, X_axis, Y_axis, Z_axis;
    int mAngle = 0;
    int count = 0;
    int steps = 360 * 1000 / mDegrees;
    printf("/*********************************************************\\\n");
    printf("| Angle [mdeg] | X axis | Y axis | Z axis | Distance [mm] |\n");

    for(; count < steps; ++count) {
        distance = read_distance();
        X_axis = read_X_axis();
        Y_axis = read_Y_axis();
        Z_axis = read_Z_axis();
        printf("| %8d.%3d | %6d | %6d | %6d | %13d |\n", 
            mAngle / 100, mAngle % 100, X_axis, Y_axis, Z_axis, distance);

        stepper_rotate_angle(mDegrees);
        while(stepper_is_busy());

        mAngle += mDegrees;
    }

    printf("\\*********************************************************/\n");

    stepper_rotate_angle(-1 * (mDegrees * count));
    while(stepper_is_busy());
}
