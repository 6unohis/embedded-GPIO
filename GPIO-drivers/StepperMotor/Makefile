obj-m += step_driver.o
RESULT = step_example
SRC = $(RESULT).c

all:
	make -C /home/siyoung/working/kernel M=$(PWD) modules
	aarch64-linux-gnu-gcc -o $(RESULT) $(SRC)
clean:
	make -C $(HOME)/working/kernel M=$(PWD) clean
	rm -f $(RESULT)
