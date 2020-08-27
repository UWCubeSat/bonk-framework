#ifndef SERIAL_H
#define SERIAL_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

class FakeSerial {
public:
	FakeSerial(): buf_sz(0), buf_n(1) { }

	bool available() const {
		return buf_n < buf_sz;
	}

	char read() {
		return available() ? buf[buf_n++] : -1;
	}

	size_t write(const uint8_t *buf, size_t size) {
		// I sure hope it's null terminated!
		printf("%s", buf);
		return size;
	}

	size_t write(const char *buf) {
		printf("%s", buf);
		return strlen(buf);
	}

	size_t println() {
		puts("");
		return 1;
	}

	// add data to buffer
	void FAKE_replaceBuffer(const char buf_arg[]) {
		buf = buf_arg;
		buf_sz = strlen(buf_arg);
		buf_n = 0;
	} 
private:
	const char *buf;
	size_t buf_sz;
	size_t buf_n; // current index into buf
};

FakeSerial Serial;

#endif
