#include <gtkmm/main.h>
#include <napkin/util.h>

#include "sleep_timeline.h"
#include "sleep_history.h"

namespace napkin {
    namespace gtk {

	sleep_history_t::basic_textrenderer::basic_textrenderer() {
	    property_family().set_value("monospace");
	    property_single_paragraph_mode().set_value(true);
	}
	void sleep_history_t::basic_textrenderer::render_vfunc(
		const Glib::RefPtr<Gdk::Drawable>& window, Gtk::Widget& widget,
		const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area,
		const Gdk::Rectangle& expose_area, Gtk::CellRendererState flags) {
	    hypnodata_t *hd = (hypnodata_t*)property_user_data().get_value();
	    property_text().set_value(hd?get_text(*hd).c_str():"");
	    Gtk::CellRendererText::render_vfunc(window,widget,
		    background_area,cell_area,expose_area,
		    flags);
	}

	const string sleep_history_t::date_render_t::get_text(const hypnodata_t& hd) const {
	    return hd.str_date(); }
	const string sleep_history_t::tobed_render_t::get_text(const hypnodata_t& hd) const {
	    return hd.str_to_bed(); }
	const string sleep_history_t::alarm_render_t::get_text(const hypnodata_t& hd) const {
	    return hd.str_alarm(); }
	sleep_history_t::window_render_t::window_render_t() {
	    property_xalign().set_value(1); }
	const string sleep_history_t::window_render_t::get_text(
		const hypnodata_t& hd) const {
	    char tmp[16];
	    snprintf(tmp,sizeof(tmp),"%2d",hd.window);
	    return tmp;
	}
	sleep_history_t::nawakes_render_t::nawakes_render_t() {
	    property_xalign().set_value(1); }
	const string sleep_history_t::nawakes_render_t::get_text(
		const hypnodata_t& hd) const {
	    char tmp[16];
	    snprintf(tmp,sizeof(tmp),"%2d",(int)hd.almost_awakes.size());
	    return tmp;
	}
	const string sleep_history_t::data_a_render_t::get_text(const hypnodata_t& hd) const {
	    return hd.str_data_a(); }

	sleep_history_t::sleep_timeline_render_t::sleep_timeline_render_t(
		const sleep_history_t& sh) : sleep_history(sh)
	{
	    property_xpad().set_value(2); property_ypad().set_value(2);
	}
	void sleep_history_t::sleep_timeline_render_t::render_vfunc(
		const Glib::RefPtr<Gdk::Drawable>& window, Gtk::Widget&/*widget*/,
		const Gdk::Rectangle&/*background_area*/, const Gdk::Rectangle& cell_area,
		const Gdk::Rectangle&/*expose_area*/, Gtk::CellRendererState/*flags*/) {
	    hypnodata_t *hd = (hypnodata_t*)property_user_data().get_value();
	    if(!hd) return;
	    int xpad = property_xpad(), ypad = property_ypad();
	    int x0 = cell_area.get_x()+xpad, y0 = cell_area.get_y()+ypad,
		dx = cell_area.get_width()-2*xpad, dy = cell_area.get_height()-2*ypad;
	    time_t a = hd->aligned_start();
	    gtk::render_sleep_timeline(
		    *hd,
		    window,
		    x0,y0, dx,dy,
		    a+sleep_history.min_tobed,
		    a+sleep_history.max_alarm );
	}

	sleep_history_t::sleep_history_t(db_t& d)
	    : store( Gtk::ListStore::create(cols) ),
	    r_sleep_timeline(*this),
	    min_tobed(24*60*60*2), max_alarm(0),
	    db(d)
	{
	    w_tree.set_model(store);
	    add(w_tree);
	    append_c("Date",r_date);
	    append_c("To bed",r_to_bed);
	    (c_timeline=append_c("Sleep timeline",r_sleep_timeline))->set_expand(true);
	    append_c("Alarm",r_alarm);
	    append_c("Window",r_window);
	    append_c(" N",r_nawakes);
	    append_c("Data A",r_data_a);

	    w_tree.signal_query_tooltip().connect(
		    sigc::mem_fun(*this,&sleep_history_t::on_query_tooltip));
	    w_tree.set_has_tooltip(true);
	    w_tree.signal_button_press_event().connect(
		    sigc::mem_fun(*this,&sleep_history_t::on_button_press),false);
	}

	Gtk::TreeView::Column *sleep_history_t::append_c(const string& title,Gtk::CellRenderer& renderer) {
	    Gtk::TreeView::Column *rv = w_tree.get_column(w_tree.append_column(title,renderer)-1);
	    rv->add_attribute(renderer.property_user_data(),cols.c_hypnodata_ptr);
	    rv->set_resizable(true);
	    return rv;
	}

	bool sleep_history_t::on_button_press(GdkEventButton* geb) {
	    if(geb->type!=GDK_2BUTTON_PRESS) return false;
	    double_click_signal();
	    return true;
	}

	bool sleep_history_t::on_query_tooltip(
		int x,int y,bool keyboard_tooltip,
		const Glib::RefPtr<Gtk::Tooltip>& tooltip) {
	    if(keyboard_tooltip) return false;
	    int tx,ty;
	    w_tree.convert_widget_to_tree_coords(x,y,tx,ty);
	    Gtk::TreeModel::Path p;
	    Gtk::TreeViewColumn *c;
	    int cx, cy;
	    if(!w_tree.get_path_at_pos(tx,ty,p,c,cx,cy)) return false;
	    if(c != c_timeline) return false;
	    hypnodata_ptr_t hd = store->get_iter(p)->get_value(cols.c_hypnodata);
	    string mup = "Almost awake moments are:\n\n<tt>";
	    for(vector<time_t>::const_iterator i=hd->almost_awakes.begin();i!=hd->almost_awakes.end();++i)
		mup += strftime(" %H:%M:%S\n",*i);
	    mup += "</tt>";
	    tooltip->set_markup( mup );
	    return true;
	}

	void sleep_history_t::set_data(list<napkin::hypnodata_ptr_t> data) {
	    store->clear();
	    min_tobed = 24*60*60*2; max_alarm = 0;
	    for(std::list<napkin::hypnodata_ptr_t>::const_iterator
		    i=data.begin(); i!=data.end(); ++i) {
		Gtk::TreeModel::Row r = *(store->append());
		r[cols.c_hypnodata] = *i;
		r[cols.c_hypnodata_ptr] = i->get();
		time_t a = (*i)->aligned_start();
		time_t soff = (*i)->to_bed-a;
		if(soff < min_tobed) min_tobed = soff;
		time_t eoff = (*i)->alarm-a;
		if(eoff > max_alarm) max_alarm = eoff;
	    }
	    while(Gtk::Main::events_pending()) Gtk::Main::iteration() ;
	    w_tree.columns_autosize();
	}

	const hypnodata_ptr_t sleep_history_t::get_current() {
	    Gtk::TreeModel::Path p;
	    Gtk::TreeViewColumn *c;
	    w_tree.get_cursor(p,c);
	    if( (!p.gobj()) || p.empty())
		return hypnodata_ptr_t(); /* XXX: or throw? */
	    Gtk::ListStore::iterator i = store->get_iter(p);
	    return i->get_value(cols.c_hypnodata);
	}

    }
}
