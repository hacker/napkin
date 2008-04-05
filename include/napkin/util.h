#ifndef __NAPKIN_UTIL_H
#define __NAPKIN_UTIL_H

#include <time.h>
#include <string>

namespace napkin {
    using std::string;

    string strftime(const char *fmt,time_t t);

}

#endif /* __NAPKIN_UTIL_H */
