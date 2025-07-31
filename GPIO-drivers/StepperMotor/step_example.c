#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include<termios.h>

static struct termios init_setting, new_setting;

void init_keyboard()
{
	tcgetattr(STDIN_FILENO, &init_setting);
	new_setting = init_setting;
	new_setting.c_lflag &= ~ICANON;
	new_setting.c_lflag &= ~ECHO;
	new_setting.c_cc[VMIN]  = 0;
	new_setting.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &new_setting);
}

void close_keyboard()
{
	tcsetattr(0, TCSANOW, &init_setting);
}

char get_key()
{
	char ch = -1;
	if(read(STDIN_FILENO, &ch, 1) != 1)
		ch = -1;
	return ch;
}

void print_menu()
{
	printf("\n=========menu=========\n");
    printf("Usage : ./step_example <R | L | S | Q>\n");
	printf("[R], [r] : Stepper moves on Right\n");
	printf("[L], [l] : Stepper moves on Left\n");
    printf("[S], [s] : Stepper stops\n");
    printf("[Q], [q] : Program quit\n");
	printf("=======================\n\n");
}

int main(int argc, char **argv){
    char key, dir;
    int ret;
    int step = open("/dev/step_driver", O_RDONLY);
    if(step == -1){
        printf("Opening was not possible\n");
        return -1;
    }

    init_keyboard();
    print_menu();

    while(1){
        key = get_key();
        if(key == 'Q' || key == 'q'){
            printf("Q pressed! program quit\n");
            dir = 'Q';
            ret = write(step, &dir, 1);
            break;
        }else if(key == 'L' || key == 'l'){
            printf("L pressed! Stepper moves on left\n");
            dir = 'L';
            ret = write(step, &dir, 1);
        }else if(key == 'R' || key == 'r'){
            printf("R pressed! Stepper moves on right\n");
            dir = 'R';
            ret = write(step, &dir, 1);
        }else if(key == 'S' || key == 's'){
            printf("S pressed! Stepper stops\n");
            dir = 'S';
            ret = write(step, &dir, 1);
        }
    }

    close_keyboard();
    close(step);
    return 0;
}