#include <cstdlib>
#include <vector>
#include <gtkmm/widget.h>

#include "sleep_timeline.h"

namespace napkin {
    namespace gtk {
	using std::vector;
	using std::min;

	void render_sleep_timeline(
		const hypnodata_t& hd,
		const Glib::RefPtr<Gdk::Drawable>& w,
		int x0,int y0,int dx,int dy,
		time_t _t0,time_t _t1) {
	    static Gdk::Color c_tobed("darkgreen"), c_alarm("red"),
		c_almostawake("maroon"), c_midnight("blue"), c_hour("#606060"),
		c_timeline("#404040"), c_window("red"),
		c_background("#ffffc0"), c_border("gray");
	    static bool beenthere=false;
	    if(!beenthere) {
		Glib::RefPtr<Gdk::Colormap> cm(Gtk::Widget::get_default_colormap());
		cm->alloc_color(c_tobed); cm->alloc_color(c_alarm);
		cm->alloc_color(c_almostawake);
		cm->alloc_color(c_midnight); cm->alloc_color(c_hour);
		cm->alloc_color(c_timeline); cm->alloc_color(c_window);
		cm->alloc_color(c_background); cm->alloc_color(c_border);
		beenthere = true;
	    }
	    Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(w);

	    gc->set_foreground(c_background);
	    w->draw_rectangle(gc,true, x0,y0, dx,dy+1 );
	    gc->set_foreground(c_border);
	    w->draw_rectangle(gc,false, x0,y0, dx,dy+1 );
	    x0+=3; dx-=6;

	    time_t t0, t1;
	    if(_t0 && _t1 && _t0!=_t1 && _t0<=hd.to_bed && hd.alarm<=_t1)
		t0 = _t0, t1 = _t1;
	    else
		t0 = hd.to_bed, t1 = hd.alarm;
	    time_t dt = t1-t0;

	    time_t tb = hd.to_bed; time_t ta = hd.alarm;
	    int xb = x0+dx*(tb-t0)/dt,
		xa = x0+dx*(ta-t0)/dt;
	    int ym = y0+dy/2;
	    gc->set_line_attributes(1,Gdk::LINE_SOLID,Gdk::CAP_BUTT,Gdk::JOIN_MITER);
	    gc->set_foreground(c_timeline);
	    w->draw_line(gc, xb,ym, xa,ym );
	    time_t ws = ta-hd.window*60;
	    int xws = x0+dx*(ws-t0)/dt;
	    gc->set_foreground(c_window);
	    w->draw_rectangle(gc, true, xws,ym-1, xa-xws,3 );
	    gc->set_foreground(c_almostawake);
	    int tl2 = min(dy/2 - 3, 7);
	    int yt0 = ym-tl2, yt1 = ym+tl2+1;
	    for(vector<time_t>::const_iterator i=hd.almost_awakes.begin();i!=hd.almost_awakes.end();++i) {
		int x = x0+dx*(*i-t0)/dt;
		w->draw_line(gc, x,yt0,  x,yt1 );
	    }
	    tl2 = min(dy/5, 5);
	    yt0 = ym-tl2; yt1 = ym+tl2+1;
	    gc->set_foreground(c_hour);
	    time_t midnight = hd.aligned_start()+24*60*60;
	    for(time_t h = tb-tb%3600 + 3600; h<ta ; h+=3600) {
		if(h==midnight) gc->set_foreground(c_midnight);
		int x = x0+dx*(h-t0)/dt;
		w->draw_line(gc, x,yt0, x,yt1 );
		if(h==midnight) gc->set_foreground(c_hour);
	    }
	    gc->set_line_attributes(2,Gdk::LINE_SOLID,Gdk::CAP_BUTT,Gdk::JOIN_MITER);
	    gc->set_foreground(c_tobed);
	    w->draw_line(gc, xb,yt0, xb,yt1 );
	    gc->set_foreground(c_alarm);
	    w->draw_line(gc, xa,yt0, xa,yt1 );
	}


	bool sleep_timeline_t::on_expose_event(GdkEventExpose*) {
	    if(!hd) return true;
	    Glib::RefPtr<Gdk::Window> w = get_window();
	    int x0,y0,dx,dy,wd;
	    w->get_geometry(x0,y0,dx,dy,wd);
	    render_sleep_timeline(
		    *hd,
		    w,
		    0,0, dx-2,dy-2 );
	    return true;
	}

	void sleep_timeline_t::set_data(const hypnodata_ptr_t& _hd) {
	    hd = _hd;
	    queue_draw();
	}

    }
}
