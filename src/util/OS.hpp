#ifndef _OS_HPP_
#define _OS_HPP_

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>
using std::string;

namespace util {
class OS {
public:
static string run(const string& cmd) {
	char buffer[BUFFSIZE];
	string result = "";

	FILE* pipe = popen(cmd.c_str(), "r");
	if (pipe) {
		while (!feof(pipe)) {
			if (fgets(buffer, BUFFSIZE, pipe) != NULL) {
				result += string(buffer);
            }
		}
		pclose(pipe);
	}

	return result;
}

static int OS::ncpus() {
    int ncpus = 1;

#ifdef _SC_NPROCESSORS_ONLN
    ncpus = sysconf(_SC_NPROCESSORS_ONLN);
#endif

    return ncpus;
}

private:
    const int BUFFSIZE 1024;
};
}

#endif

