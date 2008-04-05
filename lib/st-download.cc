#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdexcept>
#include <napkin/exception.h>
#include <napkin/st/download.h>
#include <napkin/st/decode.h>

namespace napkin {
    namespace sleeptracker {
	using std::runtime_error;

	int	download_initiate(const char *port) {
	    int fd = open(port?port:"/dev/sleeptracker",
		    O_RDWR|O_NOCTTY|O_NONBLOCK);
	    if(fd<0)
		throw exception_st_port("failed to open() sleeptracker port");

	    if(tcflush(fd,TCIOFLUSH)) {
		close(fd);
		throw exception_st_port("failed to tcflush()");
	    }
	    struct termios ts;
	    ts.c_cflag = CS8|CREAD;
	    cfsetispeed(&ts,B2400); cfsetospeed(&ts,B2400);
	    ts.c_iflag = IGNPAR;
	    ts.c_oflag = ts.c_lflag = 0;
	    ts.c_cc[VMIN]=1; ts.c_cc[VTIME]=0;
	    if(tcsetattr(fd,TCSANOW,&ts)) {
		close(fd);
		throw exception_st_port("failed to tcsetattr()");
	    }

	    if(write(fd,"V",1)!=1) {
		close(fd);
		throw exception_st_port("failed to write() to sleeptracker");
	    }
	    return fd;
	}
	size_t download_finish(int fd,void *buffer,size_t buffer_size) {
	    size_t rv = read(fd,buffer,buffer_size);
	    close(fd);

	    if(rv==(size_t)-1)
		throw exception_st_port("failed to read() from sleeptracker");
	    return rv;
	}

	size_t download(
		void *buffer,size_t buffer_size,
		const char *port) {
	    int fd = download_initiate(port);
	    /* this is not the best way to wait for data, but
	     * after all it's a sleeptracker! */
	    sleep(1);
	    return download_finish(fd,buffer,buffer_size);
	}

	hypnodata_ptr_t download(const char *port) {
	    char buffer[2048];
	    size_t rb = download(buffer,sizeof(buffer),port);
	    hypnodata_ptr_t rv( new hypnodata_t );
	    decode(*rv,buffer,rb);
	    return rv;
	}

    }
}
