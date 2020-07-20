// Copyright (c) Mark Polyakov 2020, released under GPLv3
// SD library mock

#ifndef SD_H
#define SD_H

#include <inttypes.h>

#define O_APPEND (1<<1)
#define O_WRITE  (1<<2)

typedef struct File {
	void close() { }
	bool write(uint8_t) { return true; }
	operator bool() const { return true; }
} File;

struct SD {
	File open(const char *ch, uint8_t) {
		File f;
		return f;
	}
} SD;

#endif // SD_H
