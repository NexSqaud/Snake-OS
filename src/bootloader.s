.code16

.equ VIDEO_MODE, ((0x11B) | (1 << 14))

.section .data
	.extern CORE_BEGIN
	
	.extern _core_size_sectors
	.extern _bootloader_last_opcode
	
.section .text
	.globl _start;
_start:
	movw $0x7000, %ax
	movw %ax, %ss
	movw $0xFFFF, %ax
	movw %ax, %sp
	
	movw $0x00, %ax
	movw %ax, %es
	movw $CORE_BEGIN, %ax
	movw %ax, %bx
	
	xor %ax, %ax
	
	movb $0x02, %ah
	
	movb $_core_size_sectors, %al
	movb $0, %ch
	movb $2, %cl
	movb $0, %dh
	
	# mov %bx, %si
	int $0x13
	
	movw $CORE_BEGIN, %ax
	jmp %ax
	
	jmp _bootloader_last_opcode
	