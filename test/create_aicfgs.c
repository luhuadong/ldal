#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ldal.h"

#define ANALOG_DEV_PREFIX      "aifile"  /* For example: aifile */
#define ANALOG_PORT_NUM        16

/* struct aicfgs member: is_corrected, slope, intercept */

static struct aicfgs aicfgs_table[ANALOG_PORT_NUM][ADC_MODE_NUM] = {
    {{true, 0.178327, -43.927979}, {true, 0.717927, -166.989258}},    /* AI0 */
    {{true, 0.180870, -46.151855}, {true, 0.728120, -175.913086}},    /* AI1 */
    {{true, 0.180549, -46.461182}, {true, 0.726903, -178.672852}},    /* AI2 */
    {{true, 0.180029, -45.967529}, {true, 0.725111, -177.216797}},    /* AI3 */
    {{true, 0.181192, -46.022949}, {true, 0.729129, -178.636719}},    /* AI4 */
    {{true, 0.178966, -44.980225}, {true, 0.719476, -172.386719}},    /* AI5 */
    {{true, 0.180223, -45.296143}, {true, 0.727220, -174.095703}},    /* AI6 */
    {{true, 0.178660, -44.695068}, {true, 0.720669, -171.807617}},    /* AI7 */
    {{true, 0.178508, -44.091309}, {true, 0.719424, -171.943359}},    /* AI8 */
    {{true, 0.179469, -43.431396}, {true, 0.722074, -167.954102}},    /* AI9 */
    {{true, 0.177788, -44.506348}, {true, 0.716230, -176.764648}},    /* AI10 */
    {{true, 0.179158, -44.849121}, {true, 0.723066, -171.366211}},    /* AI11 */
    {{true, 0.178657, -45.319092}, {true, 0.721058, -175.144531}},    /* AI12 */
    {{true, 0.180050, -44.832520}, {true, 0.724743, -169.734375}},    /* AI13 */
    {{true, 0.179673, -44.618896}, {true, 0.723537, -171.768555}},    /* AI14 */
    {{true, 0.178285, -45.640869}, {true, 0.718030, -173.620117}},    /* AI15 */
};

int create_aicfgs(void)
{
    int fd;
    char pathname[LDAL_NAME_MAX] = {0};
    struct aicfgs aicfgs[ADC_MODE_NUM];

    if (0 != access(ANALOG_CALIB_DIR, F_OK)) {
        mkdir(ANALOG_CALIB_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }

    for (int i=0; i<ANALOG_PORT_NUM; i++) {

        snprintf(pathname, LDAL_NAME_MAX, "%s%s%d", ANALOG_CALIB_DIR, ANALOG_DEV_PREFIX, i);
        
        fd = open(pathname, O_WRONLY | O_CREAT, 0644);
        if (fd == -1) {
            perror("Can't open file");
            continue;
        }
        if (-1 == write(fd, aicfgs_table[i], sizeof(aicfgs_table[i]))) {
            perror("Can't write file");
            close(fd);
            continue;
        }
        close(fd);
    }
    return 0;
}

void show_aicfgs(void)
{
    int fd;
    char pathname[LDAL_NAME_MAX] = {0};
    struct aicfgs aicfgs[ADC_MODE_NUM];

    for (int i=0; i<ANALOG_PORT_NUM; i++) {

        snprintf(pathname, LDAL_NAME_MAX, "%s%s%d", ANALOG_CALIB_DIR, ANALOG_DEV_PREFIX, i);
        
        fd = open(pathname, O_RDONLY);
        if (fd == -1) {
            perror("Can't open file");
            continue;
        }
        if (-1 == read(fd, &aicfgs, sizeof(aicfgs))) {
            perror("Can't read file");
            close(fd);
            continue;
        }
        close(fd);
        printf("%s\t> %d %f %f, %d %f %f\n", pathname, 
                aicfgs[ADC_MODE_CURRENT].is_corrected, aicfgs[ADC_MODE_CURRENT].slope, aicfgs[ADC_MODE_CURRENT].intercept, 
                aicfgs[ADC_MODE_VOLTAGE].is_corrected, aicfgs[ADC_MODE_VOLTAGE].slope, aicfgs[ADC_MODE_VOLTAGE].intercept);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s [-c | -s]\n", argv[0]);
        return 0;
    }

    printf("size of aicfgs : %lu\n", sizeof(struct aicfgs));

    if (0 == strcmp(argv[1], "-c") || 0 == strcmp(argv[1], "--create")) {
        /* Write to files */
        printf("Write aicfgs to files...\n");
        create_aicfgs();
        show_aicfgs();
    }
    else if (0 == strcmp(argv[1], "-s") || 0 == strcmp(argv[1], "--show")) {
        /* Read from files */
        printf("Read aicfgs from files...\n");
        show_aicfgs();
    }
    else {
        printf("Invalid options\n");
    }

    return 0;
}
