#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<termios.h>

#define BCM2711_PERI_BASE 0xFE000000
#define BLOCK_SIZE (4*1024)
#define PAGE_SIZE  (4*1024)
#define GPIO_BASE  (BCM2711_PERI_BASE + 0x200000)

#define GPFSEL0 0x00
#define GPFSEL1 0x04
#define GPSET0  0x1c
#define GPCLR0  0x28
#define GPLEV0  0x34

static struct termios init_setting, new_setting;

void init_keyboard(){
    tcgetattr(STDIN_FILENO, &init_setting);
    new_setting = init_setting;
    new_setting.c_lflag &= ~ICANON;
    new_setting.c_lflag &= ~ECHO;
    new_setting.c_cc[VMIN] = 0;
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

int main(int argc, char **argv){
    int data;
    int memfd;
    char *addr_gpio;
    volatile unsigned int *gpio;
    int led_on = 0;
    int tmp;
    int prev;
    char ch;

    memfd = open("/dev/mem", O_RDWR | O_SYNC);
    if(memfd < 0){
        printf("mem open fail\n");
        return -1;
    }

    addr_gpio = (char*)mmap(NULL, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, memfd, GPIO_BASE);

    if(addr_gpio == MAP_FAILED){
        printf("mmap failed\n");
        return -1;
    }

    gpio = (volatile unsigned int*)addr_gpio;

    gpio[GPFSEL0/4] |= (1<<12);
    gpio[GPCLR0/4] |= (1<<4);

    init_keyboard();

    printf("Press button to toggle LED. Press 'q' to quit.\n");

    while(1){
        ch = get_key();

        if(ch == 'q') break;

        prev = tmp;
        tmp = gpio[GPLEV0/4] & (1<<17);

        if(tmp != prev && tmp){
            led_on = !led_on;
            if(led_on){
                printf("toggle op.. set LED on\n");
                gpio[GPSET0/4] |= (1<<4);
            }else{
                printf("toggle op.. set LED off\n");
                gpio[GPCLR0/4] |= (1<<4);
            }
        }
        usleep(10000);
    }

    close_keyboard();
    munmap(addr_gpio, BLOCK_SIZE);
    close(memfd);

    return 0;
}
