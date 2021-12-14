#ifndef _HMC5883L_H_
#define _HMC5883L_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CLASS_DIR       "/sys/class/"
#define HMC5883L_DIR    CLASS_DIR "hmc5883l/"
#define HMC5883L_X_AXIS_FILE     HMC5883L_DIR "x_axis"
#define HMC5883L_Y_AXIS_FILE     HMC5883L_DIR "y_axis"
#define HMC5883L_Z_AXIS_FILE     HMC5883L_DIR "z_axis"

#define BUFF_SIZE 20

typedef struct s_hmc5883l {
    int X_fd;
    int Y_fd;
    int Z_fd;
    int X_axis;
    int Y_axis;
    int Z_axis;
}              t_hmc5883l;

int init_hms5883l(t_hmc5883l *hmc5883l);
void close_hms5883l(void);
int read_X_axis(void);
int read_Y_axis(void);
int read_Z_axis(void);

#endif /* _HMC5883L_H_ */
