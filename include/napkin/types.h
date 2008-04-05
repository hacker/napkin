#ifndef __NAPKIN_TYPES_H
#define __NAPKIN_TYPES_H

#include <time.h>
#include <string>
#include <vector>
#include <tr1/memory>

namespace napkin {
    using std::vector;
    using std::tr1::shared_ptr;
    using std::string;

    class hypnodata_t {
	public:
	    time_t to_bed;
	    time_t alarm;
	    int window;
	    vector<time_t> almost_awakes;
	    int data_a;

	    void clear();

	    void set_to_bed(const string& w3c);
	    void set_alarm(const string& w3c);
	    void set_window(const string& str);
	    void set_data_a(const string& str);
	    void set_almost_awakes(const string& str);

	    const string w3c_to_bed() const;
	    const string w3c_alarm() const;
	    const string w3c_almostawakes() const;

	    const string str_to_bed() const;
	    const string str_alarm() const;
	    const string str_date() const;
	    const string str_data_a() const;

	    time_t aligned_start() const;
    };

    typedef shared_ptr<hypnodata_t> hypnodata_ptr_t;

}

#endif /* __NAPKIN_TYPES_H */
