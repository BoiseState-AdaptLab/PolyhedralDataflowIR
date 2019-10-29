#include "exception.h"

jmp_buf *bufs;
size_t buf_count = 0;

exception_t last_exception;


void exception_init() {
	bufs = (jmp_buf*) malloc(0);
	last_exception.code = 0;
	last_exception.msg = (char*) malloc(0);
}

jmp_buf *exception_newbuf() {
	buf_count++;
	bufs = (jmp_buf*) realloc((void*) bufs, sizeof(jmp_buf) * buf_count);
	return &bufs[buf_count-1];
}

void exception_endbuf() {
	buf_count--;
	bufs = (jmp_buf*) realloc((void*) bufs, sizeof(jmp_buf) * buf_count);
}

void throw(const char *msg, int code) {
	last_exception.code = code;
	if (!buf_count) {
		fprintf(stderr, "Uncaught exception of code %d and message: %s\n", code, msg);
		exception_end();
		raise(SIGSEGV);
	}
	free((void*) last_exception.msg);
	last_exception.msg = (char*) malloc( sizeof(char)*(strlen(msg)+1) );
	strcpy(last_exception.msg, msg);
	longjmp(bufs[buf_count-1], code);
}

const exception_t exception_get_last(){
	return last_exception;
}

void exception_end() {
	buf_count = 0;
	last_exception.code = 0;
	free((void*) last_exception.msg);
	free((void*) bufs);
}

