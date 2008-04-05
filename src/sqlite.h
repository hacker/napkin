#ifndef __SQLITE_H
#define __SQLITE_H

#include <cassert>
#include <stdexcept>
#include <string>
#include <sqlite3.h>

namespace sqlite {
    using std::string;

    class exception : public std::runtime_error {
	public:
	    int rcode;
	    explicit exception(const string& w,int rc=-1)
		: std::runtime_error(w), rcode(rc) { }
	    ~exception() throw() { }
    };

    class db_t {
	public:
	    sqlite3 *_D;

	    db_t()
		: _D(0) { }
	    db_t(const char *f)
		: _D(0) { open(f); }
	    ~db_t() { close(); }

	    operator const sqlite3*(void) const { return _D; }
	    operator sqlite3*(void) { return _D; }

	    void close() {
		if(_D) {
		    sqlite3_close(_D);
		    _D = 0;
		}
	    }
	    void open(const char *f) {
		close();
		int r = sqlite3_open(f,&_D);
		if(r!=SQLITE_OK) {
		    string msg = sqlite3_errmsg(_D); sqlite3_close(_D);
		    throw exception("Failed to open SQLite database: "+msg,r);
		}
	    }

	    void exec(const char *sql) {
		assert(_D);
		char *errm;
		int r = sqlite3_exec(_D,sql,NULL,NULL,&errm);
		if(r!=SQLITE_OK)
		    throw exception(string("Failed to sqlite3_exec():")+errm,r);
	    }
	    void get_table(const char *sql,char ***resp,int *nr,int *nc) {
		assert(_D);
		char *errm;
		int r = sqlite3_get_table(_D,sql,resp,nr,nc,&errm);
		if(r!=SQLITE_OK)
		    throw exception(string("Failed to sqlite3_get_table():")+errm,r);
	    }
    };

    template<typename T>
    class mem_t {
	public:
	    T _M;

	    mem_t(T M) :_M(M) { }
	    ~mem_t() { if(_M) sqlite3_free(_M); }

	    operator const T&(void) const { return _M; }
	    operator T&(void) { return _M; }

	    mem_t operator=(T M) {
		if(_M) sqlite3_free(_M);
		_M = M;
	    }
    };

    class table_t {
	public:
	    char **_T;

	    table_t() : _T(0) { }
	    table_t(char **T) : _T(T) { }
	    ~table_t() { if(_T) sqlite3_free_table(_T); }

	    operator char**&(void) { return _T; }

	    operator char ***(void) {
		if(_T) sqlite3_free_table(_T);
		return &_T; }

	    const char *get(int r,int c,int nc) {
		assert(_T);
		return _T[r*nc+c];
	    }
    };

}

#endif /* __SQLITE_H */
