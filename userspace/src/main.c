#include "main.h"

void print_menu(struct s_devices *device);
int process_request(struct s_devices *device);

int main(void)
{
    int loop = 1;
    t_devices device; 
    init_device(&device);

    while(loop) {
        print_menu(&device);
        loop = process_request(&device);
    }

    close_device(&device);
    
    return EXIT_SUCCESS;
}

void read_repeats(struct s_devices *device)
{
    int repeat = 1;
    char choice;
    printf("Redeat? [N/y]: ");
    scanf("%c", &choice);

    if('y' == choice || 'Y' == choice) {
        printf("Enter number of repeats: ");
        scanf("%d", &repeat);
        if (repeat < 0)
            repeat = 0;
    }
    
    device->repeat = repeat;
}

void print_menu(struct s_devices *device) 
{
    printf("===============Radar===============\n");
    printf("1) Read current position\n");
    printf("2) Read distance\n");
    printf("3) Turn the motor by step\n");
    printf("4) Turn the motor at an angle\n");
    printf("5) Show motor speed\n");
    printf("6) Change motor speed\n");
    printf("7) Circular scan\n");
    printf("8) Sector scan\n");
    printf("q) Exit\n");
}

int process_request(struct s_devices *device)
{
    char choice;
    int param[3];   
    
    choice = getc(stdin);

    switch(choice) {
    case '1':
        take_current_position();
        break;
    case '2':
        printf("Distance: %d [mm]\n", read_distance());
        break;
    case '3':
        printf("Enter number of steps: ");
        scanf("%d", param);
        stepper_rotate_steps(*param);
        printf("Set to new position");
        break;
    case '4':
        printf("Enter the angle(in mDeg): ");
        scanf("%d", param);
        stepper_rotate_angle(*param);
        printf("Set to new position");
        break;
    case '5':
        printf("Current speed mode: %d\n", stepper_read_speed());
        break;
    case '6':
        printf("Enter new speed mode: ");
        scanf("%d", param);
        stepper_set_speed(*param);
        break;
    case '7':
        read_repeats(device);
        if(!device->repeat)
            break;

        printf("Enter angle step(in mDeg): ");
        scanf("%d", param);
        while(device->repeat) {
            scan_sircul(*param);
            device->repeat--;
        }
        break;
    case '8':
        read_repeats(device);
        if(!device->repeat)
            break;

        printf("Enter angle start, end, step(in mDeg): ");
        scanf("%d %d %d", &param[0], &param[1], &param[2]);
        while(device->repeat) {
            scan_sector(param[0], param[1], param[2]);
            device->repeat--;
        }
        break;
    case 'q':
        return 0;
    default:
        printf("No such option\n");
        break;
    }

    return 1;
}
