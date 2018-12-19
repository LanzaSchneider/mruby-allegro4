#ifndef MRB_ALLEGRO4_INPUT
#define MRB_ALLEGRO4_INPUT

#include "mrb_allegro4.h"
#include <allegro.h>

extern struct RClass* module_allegro4_input;
extern struct RClass* module_allegro4_keyboard;
extern struct RClass* module_allegro4_joystick;
extern struct RClass* module_allegro4_mouse;
void mrb_mruby_allegro4_input_init(mrb_state* mrb);

#endif