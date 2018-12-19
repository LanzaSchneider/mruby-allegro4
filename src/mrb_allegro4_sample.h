#ifndef MRB_ALLEGRO4_SAMPLE
#define MRB_ALLEGRO4_SAMPLE

#include "mrb_allegro4.h"
#include <allegro.h>

extern struct RClass* class_allegro4_sample;
SAMPLE* mrb_allegro4_sample_get_ptr(mrb_state *mrb, mrb_value sample);
mrb_value mrb_allegro4_sample_set(mrb_state* mrb, SAMPLE* sample, mrb_bool need_dispose);
void mrb_mruby_allegro4_sample_init(mrb_state* mrb);

#endif