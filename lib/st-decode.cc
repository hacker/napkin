#include <stdexcept>
#include <numeric>
#include <napkin/exception.h>
#include <napkin/st/decode.h>

namespace napkin {
    namespace sleeptracker {
	using std::invalid_argument;
	using std::runtime_error;

	struct st_time_t {
	    uint8_t hour;
	    uint8_t min;
	};
	struct st_date_t {
	    uint8_t month;
	    uint8_t day;
	};
	struct st_fulltime_t {
	    uint8_t hour;
	    uint8_t min;
	    uint8_t sec;
	};
	struct st_data_header_t {
	    char magic;
	    st_date_t today;
	    uint8_t unknown;
	    uint8_t window;
	    st_time_t to_bed;
	    st_time_t alarm;
	    uint8_t nawakes;
	};
	struct st_data_footer_t {
	    uint16_t data_a;
	    uint8_t checksum;
	    uint8_t eof_mark;
	};

	static void back_a_day(struct tm& t) {
	    time_t ts = mktime(&t);
	    if(ts==(time_t)-1)
		throw exception_st_data("failed to make up time to step back a day");
	    ts -= 60*60*24;
	    if(!localtime_r(&ts,&t))
		throw exception_st_data("failed to localtime_r() while stepping back a day");
	}

	hypnodata_t& decode(hypnodata_t& rv,const void *data,size_t data_length) {
	    if(data_length < (sizeof(st_data_header_t)+sizeof(st_data_footer_t)))
		throw exception_st_data_envelope("not enough sleeptracker data to decode");
	    st_data_header_t *h = (st_data_header_t*)data;
	    if(h->magic != 'V')
		throw exception_st_data_envelope("invalid magic in the data");
	    st_data_footer_t *f = (st_data_footer_t*)(static_cast<const char *>(data)+data_length-sizeof(st_data_footer_t));
	    if( (std::accumulate((uint8_t*)&h->today,(uint8_t*)&f->checksum,0)&0xFF) != f->checksum )
		throw exception_st_data_integrity("checksum mismatch");
	    st_fulltime_t *aawake = (st_fulltime_t*)&h[1];
	    if((void*)&aawake[h->nawakes] != (void*)f)
		throw exception_st_data_envelope("unbelievably screwed up data");
	    rv.clear();
	    time_t now = time(0);
	    struct tm t;
	    if(!localtime_r(&now,&t))
		throw exception_st_data("failed to localtime_r()");
	    t.tm_mon = h->today.month-1;
	    t.tm_mday = h->today.day;
	    time_t mkt = mktime(&t);
	    if(mkt == (time_t)-1)
		throw exception_st_data("failed to mktime() for a timestamp");
	    if(mkt > now) {
		--t.tm_year;
	    }
	    struct tm ta;
	    memmove(&ta,&t,sizeof(ta));
	    ta.tm_sec = 0;
	    ta.tm_hour = h->alarm.hour; ta.tm_min = h->alarm.min;
	    rv.alarm = mktime(&ta);
	    if(rv.alarm == (time_t)-1)
		throw exception_st_data("failed to mktime() for alarm");
	    struct tm tb;
	    memmove(&tb,&ta,sizeof(tb));
	    tb.tm_hour = h->to_bed.hour; tb.tm_min = h->to_bed.min;
	    rv.to_bed = mktime(&tb);
	    if(rv.to_bed == (time_t)-1)
		throw exception_st_data("failed to mktime() for 'to bed'");
	    if(rv.to_bed > rv.alarm) {
		back_a_day(tb);
		rv.to_bed -= 24*60*60;
	    }
	    struct tm taaw;
	    memmove(&taaw,&tb,sizeof(taaw));
	    for(int rest=h->nawakes;rest;--rest,++aawake) {
		if(
			taaw.tm_mday!=ta.tm_mday
			&& (
			    aawake->hour < tb.tm_hour
			    || (
				aawake->hour==tb.tm_hour
				&& aawake->min < tb.tm_min )
			   ) )
		    memmove(&taaw,&ta,sizeof(taaw));
		taaw.tm_hour = aawake->hour;
		taaw.tm_min = aawake->min;
		taaw.tm_sec = aawake->sec;
		rv.almost_awakes.push_back( mktime(&taaw) );
		if(rv.almost_awakes.back() == (time_t)-1)
		    throw exception_st_data("failed to mktime() for almost awake moment");
	    }
	    rv.window = h->window;
	    rv.data_a = f->data_a;
	    return rv;
	}

	hypnodata_ptr_t decode(const void *data,size_t data_length) {
	    hypnodata_ptr_t rv( new hypnodata_t );
	    decode(*rv,data,data_length);
	    return rv;
	}

    }
}
