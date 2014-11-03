extern "C" {
#include "lstfltk.h"
#include <pthread.h>
}

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/fl_ask.H>


static void update_input_text(Fl_Widget* o, const char *input) {
  if (input) {
    o->copy_label(input);
    o->redraw();
  }
}


static void rename_me(Fl_Widget *o) {
  const char *input = fl_input("Input:", o->label());
  update_input_text(o, input);
}


static void rename_me_pwd(Fl_Widget *o) {
  const char *input = fl_password("Input PWD:", o->label());
  update_input_text(o, input);
}


static void window_callback (Fl_Widget *w, void *) {
  int rep = fl_choice("Are you sure you want to quit?", "Cancel", "Quit", "Dunno");
  if (rep == 1) {
    ((Fl_Double_Window *)w)->hide();
    return;
  }
  if (rep == 2) fl_message("Well, maybe you should know before we quit.");
}


static void fltkDo (void) {
  char buffer[128] = "Test text";
  char buffer2[128] = "MyPassword";

  Fl_Double_Window *w = new Fl_Double_Window(200, 105);

  Fl_Return_Button *b = new Fl_Return_Button(20, 10, 160, 35, buffer); b->callback(rename_me);
  Fl_Button *b2 = new Fl_Return_Button(20, 50, 160, 35, buffer2); b2->callback(rename_me_pwd);

  w->end();

  w->callback(window_callback);
  w->resizable(b);
  w->show();

  Fl::run();

  w->redraw();
  delete w;

  printf("fltkDo: complete\n");
}



extern "C" {
static void *fltkWorkerThread (void *dummy) {
  fltkDo();
  return NULL;
}


static void runFLTKThread (void) {
  pthread_t trd;
  if (pthread_create(&trd, NULL, fltkWorkerThread, NULL) == 0) {
    pthread_detach(trd);
  }
}


LST_PRIMFN_NONSTATIC(lpFLTKAction) {
  /*fltkDo();*/
  runFLTKThread();
  return lstNilObj;
}
}
