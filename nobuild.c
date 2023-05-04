#define NOBUILD_IMPLEMENTATION
#include <nobuild.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <sys/stat.h>

#define C_COMMON_FLAGS "-m32", "-Wall", "-Wextra", "-Isrc/core", "-nostdlib", "-lgcc", "-nostdinc", "-ffreestanding", "-fno-builtin", "-march=i386", "-no-pie", "-fno-pic", "--std=c11"
#define C_RELEASE_FLAGS "-O3", "-Werror", "-pedantic"
#define C_DEBUG_FLAGS "-ggdb"

#define ASM_COMMON_FLAGS "-march=i386", "--32"

#define LD_FLAGS "-nostdlib", "-no-pie", "-melf_i386", "--print-map" 

#define CC "i686-elf-gcc"
#define ASM "as"
#define LD "i686-elf-ld"

#define CSTR_ARRAY_APPEND(array, element) array = cstr_array_append(array, element);

typedef enum
{
	Release,
	Debug
} BuildType;

BuildType currentBuildType = Debug;
const char* buildFolder = "build/debug";

void compileAsm(const char* filename, const char* outputFile)
{
	INFO("Compiling assembly file: %s -> %s", filename, outputFile);

	const char* outputPath = PATH(buildFolder, outputFile);

	if (currentBuildType == Release)
	{
		CMD(ASM, ASM_COMMON_FLAGS, "-c", filename, "-o", outputPath);
	}
	else if (currentBuildType == Debug)
	{
		CMD(ASM, ASM_COMMON_FLAGS, filename, "-o", outputPath);
	}
	else
	{
		PANIC("Unknown build type %d", currentBuildType);
	}

	free(outputPath);
}

void compileC(const char* filename, const char* outputFile)
{
	INFO("Compiling C file: %s -> %s", filename, outputFile);

	const char* outputPath = PATH(buildFolder, outputFile);

	if (currentBuildType == Release)
	{
		CMD(CC, C_COMMON_FLAGS, C_RELEASE_FLAGS, "-c", filename, "-o", outputPath);
	}
	else if (currentBuildType == Debug)
	{
		CMD(CC, C_COMMON_FLAGS, C_DEBUG_FLAGS, "-c", filename, "-o", outputPath);
	}
	else
	{
		PANIC("Unknown build type %d", currentBuildType);
	}

	free(outputPath);
}

void compileBootloader(void)
{
	compileAsm("src/boot/bootloader.s", "bootloader.s.o");
	compileAsm("src/boot/vbe.s", "vbe.s.o");
}

size_t coreFilesCount = 2;
char** coreFiles;

void compileCore(void)
{
	coreFiles = (char**)malloc(coreFilesCount * sizeof(char*));
	FOREACH_FILE_IN_DIR(file, "src/core", {
		if (ENDS_WITH(file, ".c"))
		{
			const char* path = PATH("src/core", file);
			const char* outputFile = CONCAT(file, ".o");

			coreFiles[coreFilesCount - 2] = outputFile;
			coreFilesCount++;
			coreFiles = realloc(coreFiles, coreFilesCount * sizeof(char*));

			compileC(path, outputFile);

			free(path);
		}
		});

	FOREACH_FILE_IN_DIR(file, "src/core/games", {
		if (ENDS_WITH(file, ".c"))
		{
			const char* path = PATH("src/core/games", file);
			const char* outputFile = CONCAT(file, ".o");

			coreFiles[coreFilesCount - 2] = outputFile;
			coreFilesCount++;
			coreFiles = realloc(coreFiles, coreFilesCount * sizeof(char*));

			compileC(path, outputFile);

			free(path);
		}
		});
}

void linkCore(void)
{
	const char* outputFile = PATH(buildFolder, "snake.bin");

	Cstr_Array line = cstr_array_make(LD, LD_FLAGS, "-Tlinker.ld", NULL);

	const char* linkPath = getenv("LIBGCC_PATH");
	const char* linkParameter = CONCAT("-L", linkPath);


	coreFiles[coreFilesCount - 2] = "bootloader.s.o";
	coreFiles[coreFilesCount - 1] = "vbe.s.o";

	for (size_t i = 0; i < coreFilesCount; i++)
	{
		if (coreFiles[i] == NULL) break;
		CSTR_ARRAY_APPEND(line, PATH(buildFolder, coreFiles[i]));
	}

	CSTR_ARRAY_APPEND(line, "-o");
	CSTR_ARRAY_APPEND(line, outputFile);
	CSTR_ARRAY_APPEND(line, linkParameter);
	CSTR_ARRAY_APPEND(line, "-lgcc");

	Cmd cmd = {
		.line = line
	};

	INFO("Linking core");
	INFO("Run: %s", cmd_show(cmd));

	cmd_run_sync(cmd);

	free(outputFile);
	free(linkParameter);
}

void makeImg(void)
{
	INFO("Copying linked core to image");

	const char* buildTypeString = "debug";
	if (currentBuildType == Release) buildTypeString = "release";

	const char* outputFile = CONCAT(buildTypeString, ".img");

	FILE* image = fopen(outputFile, "wb");
	if (!image)
	{
		PANIC("Can't create image file");
	}

	{
		const char* corePath = PATH(buildFolder, "snake.bin");
		FILE* coreFile = fopen(corePath, "rb");

		struct stat st;
		fstat(fileno(coreFile), &st);

		uint8_t* buffer = malloc(st.st_size);
		fread(buffer, sizeof(uint8_t), st.st_size, coreFile);
		fwrite(buffer, sizeof(uint8_t), st.st_size, image);

		fclose(coreFile);
		free(corePath);
	}

	INFO("Successfuly written image to %s", outputFile);

	fclose(image);
	free(outputFile);
}

int main(int argc, char** argv)
{
	if (!getenv("LIBGCC_PATH"))
	{
		PANIC("LIBGCC_PATH not found!");
	}

	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "debug"))
		{
			currentBuildType = Debug;
		}
		else if (!strcmp(argv[i], "release"))
		{
			currentBuildType = Release;
		}
	}

	if (currentBuildType == Release) buildFolder = "build/release";
	if (currentBuildType == Debug) buildFolder = "build/debug";

	if (!PATH_EXISTS(buildFolder))
	{
		MKDIRS(buildFolder);
	}

	INFO("Build folder: %s", buildFolder);

	compileBootloader();
	compileCore();

	linkCore();

	makeImg();
}