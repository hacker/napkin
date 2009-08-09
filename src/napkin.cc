#include <fcntl.h>
#include <iostream>
using std::cerr;
using std::endl;
#include <fstream>
using std::ofstream;
#include <cstdlib>
using std::min;
#include <stdexcept>
using std::runtime_error;
#include <list>
using std::list;
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/stock.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/aboutdialog.h>
#include <napkin/exception.h>
#include <napkin/util.h>
#include <napkin/st/decode.h>
#include <napkin/st/download.h>

#include "db.h"
#include "sleep_timeline.h"
#include "dialogs.h"
#include "sleep_history.h"

#include "config.h"

class napkin_ui : public Gtk::Window {
    public:
	Gtk::VBox w_outer_box;
	Gtk::Statusbar w_status_bar;
	napkin::gtk::sleep_history_t w_history;
	Glib::RefPtr<Gtk::UIManager> uiman;
	Glib::RefPtr<Gtk::ActionGroup> agroup;
	napkin::db_t db;
	Glib::RefPtr<Gtk::Action> a_remove;

	napkin_ui()
	    : w_history(db)
	{
	    static const char *ui_info =
		"<ui>"
		 "<menubar name='menu_bar'>"
		  "<menu action='menu_sleep'>"
#ifndef NDEBUG
		   "<menu action='menu_sleep_add'>"
#endif
		    "<menuitem action='sleep_add_from_sleeptracker'/>"
#ifndef NDEBUG
		    "<menuitem action='sleep_add_from_datafile'/>"
		   "</menu>"
#endif
		   "<menuitem action='sleep_remove'/>"
		   "<menuitem action='exit'/>"
		  "</menu>"
		  "<menu action='menu_help'>"
		   "<menuitem action='help_about'/>"
		  "</menu>"
		 "</menubar>"
		 "<toolbar action='tool_bar'>"
		  "<toolitem action='sleep_add_from_sleeptracker'/>"
		  "<toolitem action='sleep_remove'/>"
		  "<separator expand='true'/>"
#ifndef NDEBUG
		  "<toolitem action='debug'/>"
		  "<separator/>"
#endif
		  "<toolitem action='exit'/>"
		 "</toolbar>"
		"</ui>";
	    agroup = Gtk::ActionGroup::create();
	    agroup->add(Gtk::Action::create("menu_sleep","Sleep"));
	    agroup->add(Gtk::Action::create("menu_sleep_add","Add"));
	    agroup->add(Gtk::Action::create("sleep_add_from_sleeptracker",Gtk::Stock::CONNECT,
			"from sleeptracker","import sleeptracker data from watch"),
		    Gtk::AccelKey("<Ctrl>d"),
		    sigc::mem_fun(*this,&napkin_ui::on_sleep_add_from_sleeptracker));
#ifndef NDEBUG
	    agroup->add(Gtk::Action::create("sleep_add_from_datafile",Gtk::Stock::CONVERT,
			"from data file","import sleeptracker data stored in a file"),
		    sigc::mem_fun(*this,&napkin_ui::on_sleep_add_from_datafile));
#endif
	    agroup->add(a_remove=Gtk::Action::create("sleep_remove",Gtk::Stock::REMOVE,
			"Remove","remove highlighted sleep event from the database"),
		    Gtk::AccelKey("delete"),
		    sigc::mem_fun(*this,&napkin_ui::on_remove));
	    agroup->add(Gtk::Action::create("exit",Gtk::Stock::QUIT,"Exit","Exit "PACKAGE_NAME),
		    Gtk::AccelKey("<control>w"),
		    sigc::mem_fun(*this,&napkin_ui::on_quit));
	    agroup->add(Gtk::Action::create("menu_help","Help"));
	    agroup->add(Gtk::Action::create("help_about",Gtk::Stock::ABOUT,
			"About","About this program"),
		    sigc::mem_fun(*this,&napkin_ui::on_help_about));
#ifndef NDEBUG
	    agroup->add(Gtk::Action::create("debug",Gtk::Stock::INFO,"Debug","debug action"),
		    sigc::mem_fun(*this,&napkin_ui::on_debug));
#endif
	    uiman = Gtk::UIManager::create();
	    uiman->insert_action_group(agroup);
	    add_accel_group(uiman->get_accel_group());
	    uiman->add_ui_from_string(ui_info);
	    Gtk::Widget * mb = uiman->get_widget("/menu_bar");
	    if(mb)
		w_outer_box.pack_start(*mb,Gtk::PACK_SHRINK);
	    Gtk::Widget * tb = uiman->get_widget("/tool_bar");
	    if(tb) {
		static_cast<Gtk::Toolbar*>(tb)->set_toolbar_style(Gtk::TOOLBAR_ICONS);
		w_outer_box.pack_start(*tb,Gtk::PACK_SHRINK);
	    }
	    w_outer_box.pack_start(w_history,true/*expand*/,true/*fill*/);
	    w_outer_box.pack_end(w_status_bar,false/*expand*/,false/*fill*/);
	    add(w_outer_box);
	    set_title(PACKAGE_STRING);
	    set_default_size(800,600);
	    show_all();
	    w_status_bar.push(" "PACKAGE_STRING);

	    refresh_data();

	    w_history.signal_cursor_changed().connect(
		    sigc::mem_fun(*this,&napkin_ui::on_history_cursor_changed));
	    on_history_cursor_changed();
	    w_history.signal_double_click().connect(
		    sigc::mem_fun(*this,&napkin_ui::on_history_double_click));
	}

	void on_help_about() {
	    Gtk::AboutDialog about;
	    about.set_authors(vector<string>(1,"Michael Krelin <hacker@klever.net>"));
	    about.set_copyright("© 2008 Klever Group");
	    extern const char *COPYING;
	    about.set_license(COPYING);
	    about.set_program_name(PACKAGE_NAME);
	    about.set_version(VERSION);
	    about.set_website("http://kin.klever.net/");
	    about.set_website_label("Klever Internet Nothings");
	    about.set_comments("The Sleeptracker PRO watch support program");
	    about.run();
	}

	void on_history_double_click() {
	    napkin::hypnodata_ptr_t hd = w_history.get_current();
	    if(!hd) return;
	    napkin::gtk::hypnoinfo_dialog_t hid(*this);
	    hid.update_data(hd);
	    hid.add_button(Gtk::Stock::OK,Gtk::RESPONSE_OK);
	    hid.run();
	}

	void refresh_data() {
	    load_data("ORDER BY s_alarm DESC");
	}

	void load_data(const string& sql) {
	    list<napkin::hypnodata_ptr_t> hds;
	    db.load(hds,sql);
	    w_history.set_data(hds);
	}

	void on_history_cursor_changed() {
	    a_remove->set_sensitive(w_history.get_current());
	}

	void on_remove() {
	    napkin::hypnodata_ptr_t hd = w_history.get_current();
	    if(!hd) return;
	    napkin::gtk::hypnoinfo_dialog_t hid(*this);
	    hid.update_data(hd);
	    hid.add_button("Remove from the database",Gtk::RESPONSE_OK);
	    hid.add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
	    if(hid.run() == Gtk::RESPONSE_OK) {
		db.remove(*hd); // TODO: handle error
		refresh_data();
	    }
	}

	void on_quit() {
	    hide();
	}

	void import_data(const napkin::hypnodata_ptr_t& hd) {
	    napkin::gtk::hypnoinfo_dialog_t hid(*this);
	    hid.update_data(hd);
	    hid.add_button("Add to the database",Gtk::RESPONSE_OK);
	    hid.add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
	    if(hid.run() == Gtk::RESPONSE_OK) {
		try {
		    db.store(*hd);
		    refresh_data();
		}catch(napkin::exception_db& nedb) {
		    Gtk::MessageDialog md(*this,
			    string("Failed to add data to the database... ")+nedb.what(),
			    false/*use_markup*/,Gtk::MESSAGE_ERROR,Gtk::BUTTONS_OK,
			    true/*modal*/);
		    md.run();
		}
	    }
	}

	class st_download_t : public Gtk::Dialog {
	    public:
		Gtk::Label hint, attempt, error;
		int nattempt;
		napkin::hypnodata_ptr_t rv;

		st_download_t(Gtk::Window& w)
		    : Gtk::Dialog("Importing data from watch",w,true/*modal*/,false/*use separator*/),
		    hint("\nImporting data from the sleeptracker...\n\n"
			    "Set your watch to the 'data' screen "
			    " and connect to the compuer, if you haven't yet.",0.5,0.5),
		    attempt("",1,0.5), error("",0,0.5),
		    nattempt(1), fd(-1)
		{
		    Gtk::VBox *vb = get_vbox();
		    vb->set_spacing(10);
		     hint.set_justify(Gtk::JUSTIFY_CENTER);
		    vb->pack_start(hint,Gtk::PACK_SHRINK,5);
		    vb->pack_start(attempt);
		    vb->pack_start(error);
		    add_button("Cancel",Gtk::RESPONSE_CANCEL);
		    vb->show_all();
		}
		~st_download_t() {
		    if(!(fd<0)) close(fd);
		}

		void on_map() {
		    Gtk::Dialog::on_map();
		    initiate_attempt();
		}

		void initiate_attempt() {
		    Glib::signal_timeout().connect_seconds(
			    sigc::mem_fun(*this,&st_download_t::try_watch),
			    1);
		}
		void show_error(const napkin::exception& e) {
		    error.set_use_markup(true);
		    error.set_markup(string()+
			    "<span color='red'>"+
			    e.what()+"</span>");
		}
		void next_attempt() {
		    char tmp[128];
		    snprintf(tmp,sizeof(tmp),"Trying again, attempt #%d",++nattempt);
		    attempt.set_text(tmp);
		}

		int fd;
		char buffer[512];
		size_t rb;

		bool try_watch() {
		    try {
			fd = napkin::sleeptracker::download_initiate(getenv("SLEEPTRACKER_PORT"));
			Glib::signal_timeout().connect_seconds(
				sigc::mem_fun(*this,&st_download_t::try_data),
				1);
			return false;
		    }catch(napkin::exception_sleeptracker& nest) {
			show_error(nest);
		    }
		    next_attempt();
		    return true;
		}

		bool try_data() {
		    try {
			try {
			    rb = napkin::sleeptracker::download_finish(fd,buffer,sizeof(buffer));
			}catch(napkin::exception_st_port& nestp) {
			    fd = -1;
			    show_error(nestp);
			    next_attempt();
			    initiate_attempt();
			    return false;
			}
			rv = napkin::sleeptracker::decode(buffer,rb);
			response(Gtk::RESPONSE_OK);
		    }catch(napkin::exception_st_data_envelope& neste) {
			show_error(neste);
			next_attempt();
			initiate_attempt();
		    }catch(napkin::exception_sleeptracker& nest) {
			show_error(nest);
		    }
		    return false;
		}
	};

	void on_sleep_add_from_sleeptracker() {
	    st_download_t sd(*this);
	    if(sd.run()==Gtk::RESPONSE_OK && sd.rv ) {
		sd.hide();
#ifndef NDEBUG
		{
		    ofstream dfile(
			    (db.datadir+"/raw-"+napkin::strftime("%Y-%m-%d.st",time(0))).c_str(),
			    std::ios::binary|std::ios::out|std::ios::trunc);
		    if(dfile)
			dfile.write(sd.buffer,sd.rb);
		    dfile.close();
		}
#endif
		import_data(sd.rv);
	    }
	}

#ifndef NDEBUG
	void on_sleep_add_from_datafile() {
	    Gtk::FileChooserDialog d("Please select a file",
		    Gtk::FILE_CHOOSER_ACTION_OPEN);
	    d.set_transient_for(*this);
	    d.add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
	    d.add_button(Gtk::Stock::OPEN,Gtk::RESPONSE_OK);
	    Gtk::FileFilter stfiles;
	    stfiles.set_name("Sleeptracker files");
	    stfiles.add_pattern("*.st");
	    d.add_filter(stfiles);
	    Gtk::FileFilter allfiles;
	    allfiles.set_name("All files");
	    allfiles.add_pattern("*");
	    d.add_filter(allfiles);
	    if(d.run()==Gtk::RESPONSE_OK) {
		d.hide();

		int fd = open(d.get_filename().c_str(),O_RDONLY);
		if(fd<0)
		    throw napkin::exception("failed to open() data");
		unsigned char buffer[512];
		size_t rb = read(fd,buffer,sizeof(buffer));
		close(fd);
		if( (rb==(size_t)-1) || rb==sizeof(buffer))
		    throw napkin::exception("error reading datafile");
		napkin::hypnodata_ptr_t hd = napkin::sleeptracker::decode(buffer,rb);
		import_data(hd);
	    }
	}
#endif

#ifndef NDEBUG
	void on_debug() {
	}
#endif
};

int main(int argc,char**argv) {
    try {
	Gtk::Main m(argc,argv);
	napkin_ui hui;
	m.run(hui);
	return 0;
    }catch(std::exception& e) {
	cerr << "oops: " << e.what() << endl;
    }
}
