#include <stdio.h> // *printf, stderr
#include <string.h> // strlen
#include <stdlib.h> // calloc, free

#include "fscodec.h" // fsPrint, endswith
#include "rd_file.h" // rd_fopen, rd_fclose, rd_fopen, rd_fgetc

int main(int argc, char * * argv) {
	char * path = (argc > 1) ? argv[1] : "output.fs";
	while (endswith(path, "/")) {
		path[strlen(path) - 1] = 0;
	}

	char * filePath = "/test/resources/bundle.txt";
	RD_FILE * file = rd_fopen(filePath, "o");
	printf("file %s: %p\ncontents:\n", filePath, file);
	int c = 0;
	while ((c = rd_fgetc(file)) != EOF) {
		printf("%c", c);
	}
	printf("\n");
	rd_fclose(file);

	return 0;
}
