#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <napkin/exception.h>
#include <napkin/util.h>
#include <napkin/types.h>

namespace napkin {

    void hypnodata_t::clear() {
	to_bed = alarm = 0;
	data_a = window = 0;
	almost_awakes.clear();
    }

    static time_t from_minute_w3c(const string& w3c) {
	struct tm t; memset(&t,0,sizeof(t)); t.tm_isdst=-1;
	if(sscanf(w3c.c_str(),"%04d-%02d-%02dT%02d:%02d",
		&t.tm_year,&t.tm_mon,&t.tm_mday,&t.tm_hour,&t.tm_min)!=5)
	    throw exception("failed to parse w3c time");
	--t.tm_mon;t.tm_year-=1900;
	time_t rv = mktime(&t);
	if(rv==(time_t)-1)
	    throw exception("failed to mktime()");
	return rv;
    }

    void hypnodata_t::set_to_bed(const string& w3c) {
	to_bed = from_minute_w3c(w3c); }
    void hypnodata_t::set_alarm(const string& w3c) {
	alarm = from_minute_w3c(w3c); }
    void hypnodata_t::set_window(const string& str) {
	window = strtol(str.c_str(),0,10); /* TODO: check for error */
    }
    void hypnodata_t::set_data_a(const string& str) {
	data_a = strtol(str.c_str(),0,10); /* TODO: check for error */
    }
    void hypnodata_t::set_almost_awakes(const string& str) {
	almost_awakes.clear();
	static const char *significants = "0123456789-T:Z";
	string::size_type p = str.find_first_of(significants);
	struct tm t; memset(&t,0,sizeof(t)); t.tm_isdst=-1;
	while(p!=string::npos) {
	    string::size_type ns = str.find_first_not_of(significants,p);
	    string w3c;
	    if(ns==string::npos) {
		w3c = str.substr(p);
		p = string::npos;
	    }else{
		w3c = str.substr(p,ns-p);
		p = str.find_first_of(significants,ns);
	    }
	    if(w3c.empty()) continue;
	    if(sscanf(w3c.c_str(),"%04d-%02d-%02dT%02d:%02d:%02d",
		    &t.tm_year,&t.tm_mon,&t.tm_mday,&t.tm_hour,&t.tm_min,&t.tm_sec)!=6)
		throw exception("failed to parse w3c time");
	    --t.tm_mon;t.tm_year-=1900;
	    time_t aa = mktime(&t);
	    if(aa==(time_t)-1)
		throw exception("failed to mktime()");
	    almost_awakes.push_back(aa);
	}
    }

    const string hypnodata_t::w3c_to_bed() const {
	return strftime("%Y-%m-%dT%H:%M",to_bed); }
    const string hypnodata_t::w3c_alarm() const {
	return strftime("%Y-%m-%dT%H:%M",alarm); }
    const string hypnodata_t::w3c_almostawakes() const {
	string rv;
	for(vector<time_t>::const_iterator i=almost_awakes.begin();i!=almost_awakes.end();++i) {
	    if(!rv.empty())
		rv += ',';
	    rv += strftime("%Y-%m-%dT%H:%M:%S",*i);
	}
	return rv;
    }

    const string hypnodata_t::str_to_bed() const {
	return strftime("%H:%M",to_bed); }
    const string hypnodata_t::str_alarm() const {
	return strftime("%H:%M",alarm); }
    const string hypnodata_t::str_date() const {
	return strftime("%Y-%m-%d, %a",alarm); }
    const string hypnodata_t::str_data_a() const {
	char tmp[16];
	snprintf(tmp,sizeof(tmp),"%d:%02d:%02d",
		data_a/3600, (data_a%3600)/60,
		data_a % 60 );
	return tmp; }

    time_t hypnodata_t::aligned_start() const {
	return alarm - (alarm % (24*60*60)) - 24*60*60; }

}
