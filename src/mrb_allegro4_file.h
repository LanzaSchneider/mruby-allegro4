#ifndef MRB_ALLEGRO4_FILE
#define MRB_ALLEGRO4_FILE

#include "mrb_allegro4.h"
#include <allegro.h>

extern struct RClass* module_allegro4_file;
extern struct RClass* class_allegro4_normal_file;
extern struct RClass* class_allegro4_memory_file;

extern struct PACKFILE_VTABLE const normal_file_vtable;
extern struct PACKFILE_VTABLE const memory_file_vtable;

struct memory_file_struct{
	unsigned char* ptr_beg;
	long size;
	long pos;
	char auto_free;
	mrb_state* mrb;
};

PACKFILE* mrb_allegro4_packfile_get_ptr(mrb_state* mrb, mrb_value packfile);
mrb_value mrb_allegro4_packfile_set(mrb_state* mrb, PACKFILE* packfile);
void mrb_mruby_allegro4_file_init(mrb_state* mrb);

#endif
