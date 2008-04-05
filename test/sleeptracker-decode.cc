#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <iterator>
using namespace std;
#include <napkin/st/decode.h>

string str_f_time(const char *fmt,time_t t) {
    struct tm tt;
    localtime_r(&t,&tt);
    char rv[1024];
    strftime(rv,sizeof(rv),fmt,&tt);
    return rv;
}

ostream& operator<<(ostream& o,const napkin::hypnodata_t& hd) {
    o
	<< "Window is " << hd.window << endl
	<< "'To bed' time is " << str_f_time("%Y-%m-%d %H:%M",hd.to_bed) << endl
	<< "Alarm time is " << str_f_time("%Y-%m-%d %H:%M",hd.alarm) << endl
	<< "Data A is " << hd.data_a/60 << ":" << hd.data_a%60 << endl;
    for(vector<time_t>::const_iterator i=hd.almost_awakes.begin();i!=hd.almost_awakes.end();++i)
	o << " almost awake at " << str_f_time("%Y-%m-%d %H:%M:%S",*i) << endl;
    return o;
}

int main(int/*argc*/,char **argv) {
    try {
	int fd = open(argv[1],O_RDONLY);
	if(fd<0)
	    throw runtime_error("failed to open() data");
	unsigned char buffer[1024];
	int rb = read(fd,buffer,sizeof(buffer));
	if(!(rb>0))
	    throw runtime_error("failed to read() data");
	close(fd);

	napkin::hypnodata_t hd;
	cout << napkin::sleeptracker::decode(hd,buffer,rb);
    }catch(exception& e) {
	cerr << "oops: " << e.what() << endl;
    }
}

