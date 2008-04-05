#ifndef __NAPKIN_ST_DOWNLOAD_H
#define __NAPKIN_ST_DOWNLOAD_H

#include <napkin/types.h>

namespace napkin {
    namespace sleeptracker {

	int download_initiate(const char *port=0);
	size_t download_finish(int fd,void *buffer,size_t buffer_size);

	size_t download(
		void *buffer,size_t buffer_size,
		const char *port=0);
	hypnodata_ptr_t download(const char *port=0);

    }
}

#endif /* __NAPKIN_ST_DOWNLOAD_H */
