@echo off

gdb -ix "gdb/gdb_real_mode.txt" -ex "set tdesc filename gdb/target.xml" -ex "target remote :1234" -ex "b *0x7C00" -ex "c"