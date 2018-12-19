#ifndef MRB_ALLEGRO4_BITMAP
#define MRB_ALLEGRO4_BITMAP

#include "mrb_allegro4.h"
#include <allegro.h>

extern struct RClass* class_allegro4_bitmap;
BITMAP* mrb_allegro4_bitmap_get_ptr(mrb_state *mrb, mrb_value bitmap);
mrb_value mrb_allegro4_bitmap_set(mrb_state* mrb, BITMAP* bitmap, mrb_bool need_dispose);
void mrb_mruby_allegro4_bitmap_init(mrb_state* mrb);

#endif