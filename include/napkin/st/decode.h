#ifndef __NAPKIN_ST_DECODE_H
#define __NAPKIN_ST_DECODE_H

#include <napkin/types.h>

namespace napkin {
    namespace sleeptracker {

	hypnodata_t& decode(hypnodata_t& rv,const void *data,size_t data_length);
	hypnodata_ptr_t decode(const void *data,size_t data_length);

    }
}

#endif /* __NAPKIN_ST_DECODE_H */
