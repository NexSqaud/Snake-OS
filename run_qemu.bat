@echo off

IF NOT EXIST "release.img" (
	build.bat release
)

qemu-system-i386 -name "SnakeOS (16bit)" -fda release.img
