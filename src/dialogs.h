#ifndef __N_DIALOGS_H
#define __N_DIALOGS_H

#include <gtkmm/dialog.h>
#include <gtkmm/box.h>
#include "widgets.h"

namespace napkin {
    namespace gtk {

	class hypnoinfo_dialog_t : public Gtk::Dialog {
	    public:
		hypnoinfo_t w_hinfo;

		hypnoinfo_dialog_t(Gtk::Window& w);

		inline void update_data(const hypnodata_ptr_t& hd) {
		    w_hinfo.update_data(hd); }
	};


    }
}

#endif /* __N_DIALOGS_H */
