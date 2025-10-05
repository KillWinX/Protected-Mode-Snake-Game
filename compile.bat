nasm -fbin asm/bl.asm -o bin/boot.bin
nasm -f elf asm/k_e.asm -o bin/k_e.o
nasm -f bin asm/z.asm -o bin/z.bin
i686-elf-gcc -ffreestanding -m32 -g -c main.c -o bin/kernel.o 
i686-elf-ld -o bin/kernel.bin -Ttext 0x1000 bin/k_e.o bin/kernel.o --oformat binary
cd bin 
type boot.bin kernel.bin z.bin > MBR
qemu MBR
pause
cd..