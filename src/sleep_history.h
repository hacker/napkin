#ifndef __N_SLEEP_HISTORY_H
#define __N_SLEEP_HISTORY_H

#include <string>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <napkin/types.h>
#include "db.h"

namespace napkin {
    namespace gtk {
	using std::string;

	class sleep_history_t : public Gtk::ScrolledWindow {
	    public:
		class basic_textrenderer : public Gtk::CellRendererText {
		    public:
			basic_textrenderer();
			void render_vfunc(
				const Glib::RefPtr<Gdk::Drawable>& window, Gtk::Widget& widget,
				const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area,
				const Gdk::Rectangle& expose_area, Gtk::CellRendererState flags);
			virtual const string get_text(const hypnodata_t& hd) const = 0;
		};

		class date_render_t : public basic_textrenderer {
		    public:
			const string get_text(const hypnodata_t& hd) const;
		};
		class tobed_render_t : public basic_textrenderer {
		    public:
			const string get_text(const hypnodata_t& hd) const;
		};
		class alarm_render_t : public basic_textrenderer {
		    public:
			const string get_text(const hypnodata_t& hd) const;
		};
		class window_render_t : public basic_textrenderer {
		    public:
			window_render_t();
			const string get_text(const hypnodata_t& hd) const;
		};
		class nawakes_render_t : public basic_textrenderer {
		    public:
			nawakes_render_t();
			const string get_text(const hypnodata_t& hd) const;
		};
		class data_a_render_t : public basic_textrenderer {
		    public:
			const string get_text(const hypnodata_t& hd) const;
		};

		class sleep_timeline_render_t : public Gtk::CellRenderer {
		    public:
			const sleep_history_t& sleep_history;

			sleep_timeline_render_t(const sleep_history_t& sh);
			void render_vfunc(const Glib::RefPtr<Gdk::Drawable>& window, Gtk::Widget&/*widget*/,
				    const Gdk::Rectangle&/*background_area*/, const Gdk::Rectangle& cell_area,
				    const Gdk::Rectangle&/*expose_area*/, Gtk::CellRendererState/*flags*/);
		};

		class columns_t : public Gtk::TreeModel::ColumnRecord {
		    public:
			Gtk::TreeModelColumn<hypnodata_ptr_t> c_hypnodata;
			Gtk::TreeModelColumn<void*> c_hypnodata_ptr;

			columns_t() {
			    add(c_hypnodata); add(c_hypnodata_ptr);
			}
		};

		columns_t cols;
		Gtk::TreeView w_tree;
		Glib::RefPtr<Gtk::ListStore> store;
		date_render_t r_date;
		tobed_render_t r_to_bed;
		alarm_render_t r_alarm;
		window_render_t r_window;
		nawakes_render_t r_nawakes;
		data_a_render_t r_data_a;
		sleep_timeline_render_t r_sleep_timeline;
		Gtk::TreeView::Column *c_timeline;

		sigc::signal<void> double_click_signal;
		sigc::signal<void>& signal_double_click() { return double_click_signal; }

		time_t min_tobed, max_alarm;

		db_t& db;

		sleep_history_t(db_t& d);

		Gtk::TreeView::Column *append_c(const string& title,Gtk::CellRenderer& renderer);

		bool on_button_press(GdkEventButton* geb);
		bool on_query_tooltip(int x,int y,bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip);


		void set_data(list<napkin::hypnodata_ptr_t> data);

		Glib::SignalProxy0<void> signal_cursor_changed() {
		    return w_tree.signal_cursor_changed();
		}

		const hypnodata_ptr_t get_current();

	};
    }
}

#endif /* __N_SLEEP_HISTORY_H */
