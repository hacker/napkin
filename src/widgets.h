#ifndef __N_WIDGETS_H
#define __N_WIDGETS_H

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/separator.h>
#include <gtkmm/table.h>
#include "sleep_timeline.h"

namespace napkin {
    namespace gtk {

	class hypnoinfo_t : public Gtk::VBox {
	    public:
		Gtk::Label l_date;
		Gtk::HSeparator l_hseparator;
		Gtk::Table w_upper;
		Gtk::Label lc_tobed, lc_timeline, lc_alarm, lc_window;
		Gtk::Label lf_tobed, lf_alarm, lf_window;
		sleep_timeline_t st_timeline;
		Gtk::Label lc_almost_awakes;
		Gtk::Label lf_almost_awakes;
		Gtk::Label l_data_a;

		hypnoinfo_t();

		void update_data(const hypnodata_ptr_t& hd);
	};

    }
}

#endif /* __N_WIDGETS_H */
