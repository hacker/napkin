#ifndef __N_SLEEP_TIMELINE_H
#define __N_SLEEP_TIMELINE_H

#include <time.h>
#include <gtkmm/drawingarea.h>
#include <napkin/types.h>

namespace napkin {
    namespace gtk {

	void render_sleep_timeline(
		const hypnodata_t& hd,
		const Glib::RefPtr<Gdk::Drawable>& w,
		int x0,int y0,int dx,int dy,
		time_t _t0=0,time_t _t1=0);

	class sleep_timeline_t : public Gtk::DrawingArea {
	    public:
		hypnodata_ptr_t hd;

		bool on_expose_event(GdkEventExpose*);
		void set_data(const hypnodata_ptr_t& _hd);
	};


    }
}

#endif /* __N_SLEEP_TIMELINE_H */
