// Copyright (c) Mark Polyakov 2020, released under GPLv3
// SD library mock

#ifndef SD_H
#define SD_H

#include <inttypes.h>
#include <string.h>

#define O_APPEND (1<<1)
#define O_WRITE  (1<<2)

class FatFile {
public:
	void close() { }
	FatFile open(const char *blah, int bleh) { return *this; } 
	bool write(uint8_t) { return true; }
	size_t write(const char *blah) { return strlen(blah); }
	size_t write(const uint8_t *blah, size_t size) { return size; }
	size_t print(const char *blah) { return strlen(blah); }
	size_t println() { return 1; }
	void sync() { }
	
	operator bool() const { return true; }
};

#endif // SD_H
