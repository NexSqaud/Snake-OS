.code16

.include "src/boot/gdt.s"

.set _temp_core_location, 0x500

.set _video_width, 800
.set _video_height, 600
.set _video_bpp, 32

.extern _core_begin
.extern _core_size_int
.extern _core_size_sectors

.extern set_video_mode

.section .data
	.globl video_buffer
	video_buffer: .int 0x0
	

.section .text
	.globl _start

_start:
	movw $0x00, %ax
	movw %ax, %es
	movw $_temp_core_location, %ax
	movw %ax, %bx
	
	xor %ax, %ax
	
	movb $0x02, %ah
	
	movb $_core_size_sectors, %al
	movb $0, %ch
	movb $2, %cl
	movb $0, %dh
	
	int $0x13
	
	movw $0x2401, %ax
	int $0x15
	
	movw $_video_width, %ax
	movw $_video_height, %bx
	movb $_video_bpp, %cl
	call set_video_mode
	
	movl %eax, video_buffer(,1)
	
	cli
	
	lgdt gdt_ptr
	movl %cr0, %eax
	or $1, %eax
	movl %eax, %cr0
	
	jmp $0x8,$second_part

.code32
second_part:
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %gs
	movw %ax, %fs
	movw %ax, %ss
	
	movl $0x7FFFF, %eax
	movl %eax, %esp
	movl %eax, %ebp
	
	movl $_core_size_int, %ecx
	movl $_core_begin, %edi
	movl $_temp_core_location, %esi
	
	rep movsd
	
	push video_buffer
	jmp _core_begin
