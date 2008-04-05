#ifndef __N_DB_H
#define __N_DB_H

#include <string>
#include <list>
#include <napkin/types.h>
#include "sqlite.h"

namespace napkin {
    using std::string;
    using std::list;

    class db_t : public sqlite::db_t {
	public:
	    string datadir;

	    db_t();

	    void store(const hypnodata_t& hd);
	    void remove(const hypnodata_t& hd);

	    void load(list<hypnodata_ptr_t>& rv,
		    const string& sql);
    };

}

#endif /* __N_DB_H */
