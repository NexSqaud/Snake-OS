@echo off

IF NOT EXIST "debug.img" (
	build.bat
)

qemu-system-i386 -s -S -name "[DEBUG] SnakeOS (32bit)" -fda debug.img -serial file:"debug.log"