arm-none-eabi-gcc -mcpu=arm1176jzf-s -mfloat-abi=softfp -mfpu=neon -O2 -fpic -ffreestanding -c start.s -Wall
arm-none-eabi-gcc -mcpu=arm1176jzf-s -mfloat-abi=softfp -mfpu=neon -O2 -fpic -ffreestanding -std=gnu99 -nostdlib -c ves_main.c -Wall
arm-none-eabi-ld start.o ves_main.o -T link.ld -o ves.elf
arm-none-eabi-objcopy ves.elf -O binary kernel.img
cp kernel.img /media/tanner/*/
