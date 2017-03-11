#ifndef RD_FILE_INTERNAL_H
#define RD_FILE_INTERNAL_H

#include "rd_file.h"
#include "fs_file.h"

struct file_t {
	uint32_t fpos;
	fs_file * file;
};

#endif /* RD_FILE_INTERNAL_H */
