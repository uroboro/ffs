#include <stdio.h> // *printf, stderr

#include "fscodec.h" // fsPrint, endswith
#include "fscod.h" // fsFilesAtPath, fsSerialize

int main(int argc, char * * argv) {
	char * path = (argc > 1) ? argv[1] : ".";
	char * output = (argc > 2) ? argv[2] : "output.fs";

	while (endswith(path, "/")) {
		path[strlen(path) - 1] = 0;
	}
	fs_file * file = fsFilesAtPath(path);
	fsPrint(file);
	int r = fsSerialize(output, file);
	printf("serialize result:%d\n", r);
	fsFree(file);

	return 0;
}
