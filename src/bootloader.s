.code16

.section .data
	.extern _core_begin
	
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
	movw $_core_begin, %ax
	movw %ax, %bx
	
	xor %ax, %ax
	
	movb $0x02, %ah
	
	movb $_core_size_sectors, %al
	movb $0, %ch
	movb $2, %cl
	movb $0, %dh
	
	int $0x13
	
	movw $_core_begin, %ax
	jmp %ax
	
	jmp _bootloader_last_opcode
	