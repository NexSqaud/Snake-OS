OUTPUT_FORMAT("binary");
ENTRY(_start);
SECTIONS
{
	_core_begin = 0x500;
	
	. = 0x0;
	.bootloader 0x7C00 : AT(0x0)
	{
		*bootloader.s.o(.text);
	}
	
	_bootloader_last_opcode = (. + 0x7C00 - 0x2); 
	
	.bootloader_data : 
	{
		*bootloader.s.o(.data);
	}
	
	
	.sig : AT(0x1FE)
	{
		SHORT(0xAA55);
	}

    . = 0x0200;
    .text _core_begin : AT(0x0200)
    {
        _text = .;
		*snake.c.o(.text.main);
        *(.text);
        _text_end = .;
    }
	
    .data :
    {
        _data = .;
        *(.bss);
        *(.bss*);
        *(.data);
        *(.rodata*);
        *(COMMON)
        _data_end = .;
    }
	
	_core_size = _data_end - _text;
	_core_size_sectors = (_core_size / 512) + 1;
	
	. = ALIGN(0x200);
	. -= 1;
	.align :
	{
		BYTE(0x0);
	}
	
    /DISCARD/ :
    {
        *(.note*);
        *(.iplt*);
        *(.igot*);
        *(.rel*);
        *(.comment);
    }
}