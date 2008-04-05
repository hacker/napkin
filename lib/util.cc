#include <napkin/util.h>

namespace napkin {

    string strftime(const char *fmt,time_t t) {
	struct tm tt;
	localtime_r(&t,&tt);// TODO: check res
	char rv[1024];
	strftime(rv,sizeof(rv),fmt,&tt); // TODO: check res
	return rv;
    }

}
