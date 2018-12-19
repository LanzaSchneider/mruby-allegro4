#include "mrb_allegro4.h"
#include "mrb_allegro4_audio.h"
#include "mrb_allegro4_sample.h"
#include "mrb_allegro4_graphics.h"
#include "mrb_allegro4_bitmap.h"
#include "mrb_allegro4_input.h"
#include "mrb_allegro4_file.h"
#include "mrb_allegro4_io.h"
#include "mrb_allegro4_timer.h"
#include <mruby/proc.h>
#include <mruby/class.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <allegro.h>

struct RClass* module_allegro4 = NULL;

static mrb_value mrb_allegro4_install(mrb_state* mrb, mrb_value self) {
	int system_id = SYSTEM_AUTODETECT, err = 0;
	mrb_get_args(mrb, "|i", &system_id);
	return mrb_fixnum_value(install_allegro(SYSTEM_AUTODETECT, &err, NULL));
}

static mrb_value mrb_allegro4_init(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(allegro_init());
}

static mrb_value mrb_allegro4_exit(mrb_state* mrb, mrb_value self) {
	allegro_exit();
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_id(mrb_state* mrb, mrb_value self) {
	return mrb_str_new_cstr(mrb, allegro_id);
}

static mrb_value mrb_allegro4_message(mrb_state* mrb, mrb_value self) {
	char* message = NULL;
	mrb_get_args(mrb, "z", &message);
	allegro_message(message);
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_title_set(mrb_state* mrb, mrb_value self) {
	mrb_value title;
	mrb_get_args(mrb, "S", &title);
	set_window_title(RSTRING_PTR(title));
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_color_depth(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(desktop_color_depth());
}

static mrb_value mrb_allegro4_resolution(mrb_state* mrb, mrb_value self) {
	int width, height;
	if(get_desktop_resolution(&width, &height)) return mrb_nil_value();
	mrb_value a = mrb_ary_new_capa(mrb, 2);
	mrb_ary_push(mrb, a, mrb_fixnum_value(width));
	mrb_ary_push(mrb, a, mrb_fixnum_value(height));
	return a;
}

static mrb_value mrb_allegro4_cpu_vendor(mrb_state* mrb, mrb_value self) {
	return mrb_str_new_cstr(mrb, cpu_vendor);
}

static mrb_value mrb_allegro4_set_uformat(mrb_state* mrb, mrb_value self) {
	mrb_int uformat;
	mrb_get_args(mrb, "i", &uformat);
	set_uformat(uformat);
	return mrb_nil_value();
}

// ================================================
// Callback on exit
// ================================================

static int close_accepted = 0;

static void close_button_handler() {
	close_accepted = 1;
}

static mrb_value mrb_allegro4_close_callback(mrb_state* mrb, mrb_value self) {
	close_accepted = 0;
	return mrb_fixnum_value(set_close_button_callback(close_button_handler));
}

static mrb_value mrb_allegro4_close_accepted(mrb_state* mrb, mrb_value self) {
	return mrb_bool_value(close_accepted);
}

// ================================================
// Gem
// ================================================
void mrb_mruby_allegro4_gem_init(mrb_state* mrb) {
	module_allegro4 = mrb_define_module(mrb, "Allegro4");
	
	mrb_define_module_function(mrb, module_allegro4, 	"install", 				mrb_allegro4_install, 					MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4, 	"init", 						mrb_allegro4_init, 						MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4, 	"exit", 						mrb_allegro4_exit, 						MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4,		"id",							mrb_allegro4_id,							MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4,		"message",				mrb_allegro4_message,					MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4,		"title=",					mrb_allegro4_title_set,				MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4,		"color_depth",		mrb_allegro4_color_depth,			MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4,		"resolution",			mrb_allegro4_resolution,			MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4,		"cpu_vendor",			mrb_allegro4_cpu_vendor,			MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4,		"set_uformat",		mrb_allegro4_set_uformat,			MRB_ARGS_REQ(1));
	
	mrb_define_module_function(mrb, module_allegro4,		"close_callback",	mrb_allegro4_close_callback,	MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4,		"close_accepted?",mrb_allegro4_close_accepted,	MRB_ARGS_NONE());
	
	mrb_define_const(mrb, module_allegro4, "U_ASCII",						mrb_fixnum_value(U_ASCII));
	mrb_define_const(mrb, module_allegro4, "U_ASCII_CP",					mrb_fixnum_value(U_ASCII_CP));
	mrb_define_const(mrb, module_allegro4, "U_UNICODE",					mrb_fixnum_value(U_UNICODE));
	mrb_define_const(mrb, module_allegro4, "U_UTF8",							mrb_fixnum_value(U_UTF8));
	
	int ai = mrb_gc_arena_save(mrb);
	mrb_mruby_allegro4_audio_init(mrb);mrb_gc_arena_restore(mrb, ai);
	mrb_mruby_allegro4_sample_init(mrb);mrb_gc_arena_restore(mrb, ai);
	mrb_mruby_allegro4_graphics_init(mrb);mrb_gc_arena_restore(mrb, ai);
	mrb_mruby_allegro4_bitmap_init(mrb);mrb_gc_arena_restore(mrb, ai);
	mrb_mruby_allegro4_input_init(mrb);mrb_gc_arena_restore(mrb, ai);
	mrb_mruby_allegro4_file_init(mrb);mrb_gc_arena_restore(mrb, ai);
	mrb_mruby_allegro4_io_init(mrb);mrb_gc_arena_restore(mrb, ai);
	mrb_mruby_allegro4_timer_init(mrb);mrb_gc_arena_restore(mrb, ai);
}

void mrb_mruby_allegro4_gem_final(mrb_state *mrb){}
