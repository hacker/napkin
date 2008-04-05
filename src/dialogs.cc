#include "dialogs.h"

namespace napkin {
    namespace gtk {

	hypnoinfo_dialog_t::hypnoinfo_dialog_t(Gtk::Window& w)
	    : Gtk::Dialog("Sleepy information",w,true/*modal*/,true/*use separator*/)
	{
	    Gtk::VBox *vb = get_vbox();
	    vb->set_spacing(2);
	    vb->add(w_hinfo);
	    vb->show_all();
	}


    }
}
