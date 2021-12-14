#include "hmc5883l.h"

static t_hmc5883l *phmc5883l;

int init_hms5883l(t_hmc5883l *phmc5883lInit)
{
    phmc5883l = phmc5883lInit;
    phmc5883l->X_fd = open(HMC5883L_X_AXIS_FILE, O_RDONLY | O_NONBLOCK);
    phmc5883l->Y_fd = open(HMC5883L_Y_AXIS_FILE, O_RDONLY | O_NONBLOCK);
    phmc5883l->Z_fd = open(HMC5883L_Z_AXIS_FILE, O_RDONLY | O_NONBLOCK);

    if(phmc5883l->X_fd < 0 || phmc5883l->Y_fd < 0 || phmc5883l->Z_fd < 0) {
        if(phmc5883l->X_fd >= 0)
            close(phmc5883l->X_fd);

        if(phmc5883l->Y_fd >= 0)
            close(phmc5883l->Y_fd);

        if(phmc5883l->Z_fd >= 0)
            close(phmc5883l->Z_fd);

        printf("ERROR: can not open hms5883l files\n");

        return EXIT_FAILURE;
    }

    if(read_X_axis() || read_Y_axis() || read_Z_axis()) {
        close_hms5883l();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void close_hms5883l(void)
{
    close(phmc5883l->X_fd);
    close(phmc5883l->Y_fd);
    close(phmc5883l->Z_fd);
}

int read_X_axis(void)
{
    char buff[BUFF_SIZE];
    int rc = 0; 
    rc = read(phmc5883l->X_fd, buff, BUFF_SIZE);
    phmc5883l->X_axis = atoi(buff);
    
    if(rc <= 0) 
        return EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}

int read_Y_axis(void)
{
    char buff[BUFF_SIZE];
    int rc = 0; 
    rc = read(phmc5883l->Y_fd, buff, BUFF_SIZE);
    phmc5883l->Y_axis = atoi(buff);
    
    if(rc <= 0) 
        return EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}

int read_Z_axis(void)
{
    char buff[BUFF_SIZE];
    int rc = 0; 
    rc = read(phmc5883l->Z_fd, buff, BUFF_SIZE);
    phmc5883l->Z_axis = atoi(buff);
    
    if(rc <= 0) 
        return EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}
