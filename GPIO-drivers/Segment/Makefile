obj-m += seg_driver.o
RESULT1 = seg_example
SRC1 = $(RESULT1).c

RESULT2 = seg_example2
SRC2 = $(RESULT2).c

all:
	make -C /home/siyoung/working/kernel M=$(PWD) modules
	aarch64-linux-gnu-gcc -o $(RESULT1) $(SRC1)
	aarch64-linux-gnu-gcc -o $(RESULT2) $(SRC2)
clean:
	make -C $(HOME)/working/kernel M=$(PWD) clean
	rm -f $(RESULT1) $(RESULT2)
