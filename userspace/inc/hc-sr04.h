#ifndef _HC_SR_04_H_
#define _HC_SR_04_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CLASS_DIR       "/sys/class/"
#define HC_SR04_DIR     CLASS_DIR "hc_sr04/"
#define HC_SR04_DISTANCE_FILE   HC_SR04_DIR "distance"

#define BUFF_SIZE 20

typedef struct s_hc_sr04
{
    int distance_fd;
    int distance;
}              t_hc_sr04;

int init_hc_sr04(t_hc_sr04 *hc_sr04);
void close_hc_sr04(void);
int read_distance(void);

#endif /* _HC_SR_04_H_ */
