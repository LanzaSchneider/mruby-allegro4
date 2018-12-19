#include "mrb_allegro4_timer.h"

struct RClass* module_allegro4_timer = NULL;

static mrb_value mrb_allegro4_timer_install(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(install_timer());
}

static mrb_value mrb_allegro4_timer_remove(mrb_state* mrb, mrb_value self) {
	remove_timer();
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_timer_rest(mrb_state* mrb, mrb_value self) {
	mrb_int time;
	mrb_get_args(mrb, "i", &time);
	rest(time);
	return mrb_nil_value();
}

void mrb_mruby_allegro4_timer_init(mrb_state* mrb) {
	module_allegro4_timer = mrb_define_module_under(mrb, module_allegro4, "Timer");
	
	mrb_define_module_function(mrb, module_allegro4_timer, 	"install", 		mrb_allegro4_timer_install, 		MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_timer, 	"remove", 			mrb_allegro4_timer_remove, 		MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_timer, 	"rest", 				mrb_allegro4_timer_rest, 			MRB_ARGS_REQ(1));
}
