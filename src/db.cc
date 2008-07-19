#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cassert>
#include <napkin/exception.h>
#include "db.h"

#include "config.h"

namespace napkin {

    extern const char *sql_bootstrap;

    db_t::db_t() {
	const char *h = getenv("HOME");
	if(h) {
	    datadir = h;
	    datadir += "/."PACKAGE_NAME"/";
	}else{
#if defined(HAVE_GET_CURRENT_DIR_NAME)
	    char *cwd = get_current_dir_name();
	    if(!cwd)
		throw napkin::exception("failed to get_current_dir_name()");
	    datadir = cwd;
	    free(cwd);
#elif defined(HAVE_GETCWD)
	    {
		char cwd[
# if defined(MAXPATH)
		    MAXPATH
# elif defined(MAXPATHLEN)
		    MAXPATHLEN
# else /* maxpath */
		    512
# endif /* maxpath */
		];
		if(!getcwd(cwd,sizeof(cwd)))
		    throw napkin::exception("failed to getcwd()");
		datadir = cwd;
	    }
#else /* get cwd */
# error dunno how to get current workdir
#endif /* get cwd */
	    datadir += "/."PACKAGE_NAME"/";
	}
	if(access(datadir.c_str(),R_OK|W_OK)
		&& mkdir(datadir.c_str(),0700))
	    throw napkin::exception("no access to '"+datadir+"' directory");
	open((datadir+PACKAGE_NAME".db").c_str());
	assert(_D);
	char **resp; int nr,nc; char *errm;
	if(sqlite3_get_table(
		    _D,
		    "SELECT s_tobed FROM sleeps LIMIT 0",
		    &resp,&nr,&nc,&errm)!=SQLITE_OK) {
	    if(sqlite3_exec(_D,sql_bootstrap,NULL,NULL,&errm)!=SQLITE_OK)
		throw napkin::exception(string("failed to bootstrap sqlite database: ")+errm);
	}else
	    sqlite3_free_table(resp);
    }

    void db_t::store(const hypnodata_t& hd) {
	sqlite::mem_t<char*> S = sqlite3_mprintf(
		"INSERT INTO sleeps ("
		 "s_tobed,s_alarm,"
		 "s_window,s_data_a,"
		 "s_almost_awakes,"
		 "s_timezone"
		") VALUES ("
		 "%Q,%Q,%d,%d,%Q,%ld"
		")",
		hd.w3c_to_bed().c_str(),
		hd.w3c_alarm().c_str(),
		hd.window,hd.data_a,
		hd.w3c_almostawakes().c_str(),
		timezone );
	try {
	    exec(S);
	}catch(sqlite::exception& se) {
	    if(se.rcode==SQLITE_CONSTRAINT)
		throw exception_db_already("The record seems to be already in the database");
	    throw exception_db("Well, some error occured");
	}
    }

    void db_t::remove(const hypnodata_t& hd) {
	sqlite::mem_t<char*> S = sqlite3_mprintf(
		"DELETE FROM sleeps"
		" WHERE s_tobed=%Q AND s_alarm=%Q",
		hd.w3c_to_bed().c_str(),
		hd.w3c_alarm().c_str() );
	exec(S);
    }

    void db_t::load(list<hypnodata_ptr_t>& rv,
	    const string& sql) {
	sqlite::table_t T;
	int nr,nc;
	get_table( string(
		    "SELECT"
		     " s_tobed, s_alarm,"
		     " s_window, s_data_a,"
		     " s_almost_awakes"
		    " FROM sleeps"
		    " "+sql).c_str(),T,&nr,&nc );
	if(nr) {
	    assert(nc==5);
	    for(int r=1;r<=nr;++r) {
		hypnodata_ptr_t hd(new hypnodata_t());
		hd->set_to_bed(T.get(r,0,nc));
		hd->set_alarm(T.get(r,1,nc));
		hd->set_window(T.get(r,2,nc));
		hd->set_data_a(T.get(r,3,nc));
		hd->set_almost_awakes(T.get(r,4,nc));
		rv.push_back(hd);
	    }
	}
    }

}
