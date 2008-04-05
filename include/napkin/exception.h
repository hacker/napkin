#ifndef __NAPKIN_EXCEPTION_H
#define __NAPKIN_EXCEPTION_H

#include <stdexcept>
#include <string>

#define	NAPKIN_E_SUBCLASS(derived,base) \
    class derived : public base { \
	public: \
		explicit derived(const string& w) \
		: base(w) { } \
    }

namespace napkin {
    using std::string;

    class exception : public std::runtime_error {
	public:
	    explicit exception(const string& w)
		: std::runtime_error(w) { }
	    ~exception() throw() { }
    };

    NAPKIN_E_SUBCLASS(exception_sleeptracker,exception);
    NAPKIN_E_SUBCLASS(exception_st_port,exception_sleeptracker);
    NAPKIN_E_SUBCLASS(exception_st_data,exception_sleeptracker);
    NAPKIN_E_SUBCLASS(exception_st_data_envelope,exception_st_data);
    NAPKIN_E_SUBCLASS(exception_st_data_integrity,exception_st_data_envelope);

    NAPKIN_E_SUBCLASS(exception_db,exception);
    NAPKIN_E_SUBCLASS(exception_db_already,exception_db);
}

#undef NAPKIN_E_SUBCLASS

#endif /* __NAPKIN_EXCEPTION_H */
