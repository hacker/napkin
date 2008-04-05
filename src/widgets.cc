#include <napkin/util.h>
#include "widgets.h"

namespace napkin {
    namespace gtk {

	hypnoinfo_t::hypnoinfo_t()
	    : w_upper(4,3,false/*homogeneous*/),
	    lc_tobed("To bed:",0.5,0.5),
	    lc_timeline("Sleep timeline:",0.5,0.5),
	    lc_alarm("Alarm:",0.5,0.5), lc_window("Window:",0.5,0.5),
	    l_data_a("",0.9,0.5)
	{
	    add(l_date);
	    add(l_hseparator);
	     w_upper.set_col_spacings(5);
	     w_upper.attach(lc_tobed,0,1,0,1, Gtk::SHRINK);
	     w_upper.attach(lc_timeline,1,2,0,1, Gtk::SHRINK);
	     w_upper.attach(lc_alarm,2,3,0,1, Gtk::SHRINK);
	     w_upper.attach(lf_tobed,0,1,1,4, Gtk::SHRINK);
	     w_upper.attach(st_timeline,1,2,1,4,
		     Gtk::FILL|Gtk::EXPAND,Gtk::FILL|Gtk::EXPAND,0,0);
	     w_upper.attach(lf_alarm,2,3,1,2, Gtk::SHRINK);
	     w_upper.attach(lc_window,2,3,2,3, Gtk::SHRINK);
	     w_upper.attach(lf_window,2,3,3,4, Gtk::SHRINK);
	    add(w_upper);
	    add(lc_almost_awakes);
	    add(lf_almost_awakes);
	    add(l_data_a);
	    show_all();
	}

	void hypnoinfo_t::update_data(const hypnodata_ptr_t& hd) {
	    l_date.set_use_markup(true);
	    l_date.set_markup("<b>"+hd->str_date()+"</b>");
	    lf_tobed.set_use_markup(true);
	    lf_tobed.set_markup("<b>"+hd->str_to_bed()+"</b>");
	    lf_alarm.set_use_markup(true);
	    lf_alarm.set_markup("<b>"+hd->str_alarm()+"</b>");
	    char tmp[64];
	    snprintf(tmp,sizeof(tmp),"<b>%d mins</b>",hd->window);
	    lf_window.set_use_markup(true);
	    lf_window.set_markup(tmp);
	    snprintf(tmp,sizeof(tmp),"<b>%d</b> almost awake moments:",(int)hd->almost_awakes.size());
	    lc_almost_awakes.set_use_markup(true);
	    lc_almost_awakes.set_markup(tmp);
	    string awlist;
	    for(vector<time_t>::const_iterator i=hd->almost_awakes.begin();i!=hd->almost_awakes.end();++i) {
		if(!awlist.empty())
		    awlist += ", ";
		awlist += strftime("<b>%H:%M:%S</b>",*i);
	    }
	    lf_almost_awakes.set_use_markup(true);
	    lf_almost_awakes.set_line_wrap(true);
	    lf_almost_awakes.set_line_wrap_mode(Pango::WRAP_WORD);
	    lf_almost_awakes.set_markup("<tt>"+awlist+"</tt>");
	    l_data_a.set_use_markup(true);
	    l_data_a.set_markup("Data A is <b>"+hd->str_data_a()+"</b>");
	    st_timeline.set_data(hd);
	}

    }
}
