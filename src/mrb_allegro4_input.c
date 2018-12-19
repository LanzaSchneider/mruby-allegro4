#include "mrb_allegro4_input.h"
#include "mrb_allegro4_bitmap.h"
#include <stdlib.h>
#include <mruby/class.h>
#include <mruby/string.h>
#include <allegro.h>

// Input Kerboard
struct RClass* module_allegro4_keyboard = NULL;

static mrb_value mrb_allegro4_keyboard_install(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(install_keyboard());
}

static mrb_value mrb_allegro4_keyboard_remove(mrb_state* mrb, mrb_value self) {
	remove_keyboard();
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_keyboard_poll(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(poll_keyboard());
}

static mrb_value mrb_allegro4_keyboard_needs_poll(mrb_state* mrb, mrb_value self) {
	return mrb_bool_value(keyboard_needs_poll());
}

static mrb_value mrb_allegro4_keyboard_index(mrb_state* mrb, mrb_value self) {
	mrb_int index;
	mrb_get_args(mrb, "i", &index);
	if (index < 0 || index >= KEY_MAX) return mrb_nil_value();
	return mrb_bool_value(key[index]);
}

static mrb_value mrb_allegro4_keyboard_keypressed(mrb_state* mrb, mrb_value self) {
	return mrb_bool_value(keypressed());
}

static mrb_value mrb_allegro4_keyboard_scancode_to_ascii(mrb_state* mrb, mrb_value self) {
	mrb_int scancode;
	mrb_get_args(mrb, "i", &scancode);
	return mrb_fixnum_value(scancode_to_ascii(scancode));
}

static mrb_value mrb_allegro4_keyboard_scancode_to_name(mrb_state* mrb, mrb_value self) {
	mrb_int scancode;
	mrb_get_args(mrb, "i", &scancode);
	return mrb_str_new_cstr(mrb, scancode_to_name(scancode));
}

static mrb_value mrb_allegro4_keyboard_simulate_keypress(mrb_state* mrb, mrb_value self) {
	mrb_int key;
	mrb_get_args(mrb, "i", &key);
	simulate_keypress(key);
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_keyboard_simulate_ukeypress(mrb_state* mrb, mrb_value self) {
	mrb_int key, scancode;
	mrb_get_args(mrb, "ii", &key, &scancode);
	simulate_ukeypress(key, scancode);
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_keyboard_set_keyboard_rate(mrb_state* mrb, mrb_value self) {
	mrb_int delay, repeat;
	mrb_get_args(mrb, "ii", &delay, &repeat);
	set_keyboard_rate(delay, repeat);
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_keyboard_clear_keybuf(mrb_state* mrb, mrb_value self) {
	clear_keybuf();
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_keyboard_readkey(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(readkey());
}

static mrb_value mrb_allegro4_keyboard_ureadkey(mrb_state* mrb, mrb_value self) {
	mrb_int scancode;
	mrb_get_args(mrb, "i", &scancode);
	return mrb_fixnum_value(ureadkey(&scancode));
}

// Input Joystick
struct RClass* module_allegro4_joystick = NULL;

static mrb_value mrb_allegro4_joystick_install(mrb_state* mrb, mrb_value self) {
	mrb_int type;
	mrb_get_args(mrb, "i", &type);
	return mrb_fixnum_value(install_joystick(type));
}

static mrb_value mrb_allegro4_joystick_remove(mrb_state* mrb, mrb_value self) {
	remove_joystick();
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_joystick_poll(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(poll_joystick());
}

static mrb_value mrb_allegro4_joystick_num_joysticks(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(num_joysticks);
}

static mrb_value mrb_allegro4_joystick_index(mrb_state* mrb, mrb_value self) {
	mrb_int index;
	mrb_get_args(mrb, "i", &index);
	if (index < 0 || index >= num_joysticks) return mrb_nil_value();
	mrb_value str = mrb_str_buf_new(mrb, sizeof(JOYSTICK_INFO));
	mrb_str_resize(mrb, str, sizeof(JOYSTICK_INFO));
	memcpy(RSTRING_PTR(str), joy + index, sizeof(JOYSTICK_INFO));
	return str;
}

static mrb_value mrb_allegro4_joystick_calibrate_joystick_name(mrb_state* mrb, mrb_value self) {
	mrb_int index;
	mrb_get_args(mrb, "i", &index);
	if (index < 0 || index >= num_joysticks) return mrb_nil_value();
	return mrb_str_new_cstr(mrb, calibrate_joystick_name(index));
}

static mrb_value mrb_allegro4_joystick_calibrate_joystick(mrb_state* mrb, mrb_value self) {
	mrb_int index;
	mrb_get_args(mrb, "i", &index);
	if (index < 0 || index >= num_joysticks) return mrb_nil_value();
	return mrb_fixnum_value(calibrate_joystick(index));
}

// Input Mouse
struct RClass* module_allegro4_mouse = NULL;

static mrb_value mrb_allegro4_mouse_install(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(install_mouse());
}

static mrb_value mrb_allegro4_mouse_exit(mrb_state* mrb, mrb_value self) {
	remove_mouse();
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_mouse_poll(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(poll_mouse());
}

static mrb_value mrb_allegro4_mouse_needs_poll(mrb_state* mrb, mrb_value self) {
	return mrb_bool_value(mouse_needs_poll());
}

static mrb_value mrb_allegro4_mouse_hardware_cursor_set(mrb_state* mrb, mrb_value self) {
	mrb_bool enabled = 0;
	mrb_get_args(mrb, "b", &enabled);
	if (enabled) {
		enable_hardware_cursor();
	} else {
		disable_hardware_cursor();
	}
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_mouse_cursor_set(mrb_state* mrb, mrb_value self) {
	mrb_int cursor;
	mrb_get_args(mrb, "i", &cursor);
	select_mouse_cursor(cursor);
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_mouse_cursor_bitmap_set(mrb_state* mrb, mrb_value self) {
	mrb_int cursor;
	mrb_value bitmap;
	mrb_get_args(mrb, "io", &cursor, &bitmap);
	set_mouse_cursor_bitmap(cursor, mrb_allegro4_bitmap_get_ptr(mrb, bitmap));
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_mouse_x(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(mouse_x);
}

static mrb_value mrb_allegro4_mouse_y(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(mouse_y);
}

static mrb_value mrb_allegro4_mouse_z(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(mouse_z);
}

static mrb_value mrb_allegro4_mouse_w(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(mouse_w);
}

static mrb_value mrb_allegro4_mouse_b(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(mouse_b);
}

static mrb_value mrb_allegro4_mouse_pos(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(mouse_pos);
}

static mrb_value mrb_allegro4_mouse_on_screen(mrb_state* mrb, mrb_value self) {
	return mrb_bool_value(mouse_on_screen());
}

// Allegro Input Mruby

struct RClass* module_allegro4_input = NULL;

void mrb_mruby_allegro4_input_init(mrb_state* mrb) {
	module_allegro4_input = mrb_define_module_under(mrb, module_allegro4, "Input");
	// Keyboard
	module_allegro4_keyboard = mrb_define_module_under(mrb, module_allegro4_input, "Keyboard");
	
	mrb_define_module_function(mrb, module_allegro4_keyboard,	"install",							mrb_allegro4_keyboard_install,							MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_keyboard,	"remove",								mrb_allegro4_keyboard_remove,								MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_keyboard,	"poll",									mrb_allegro4_keyboard_poll,									MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_keyboard,	"needs_poll?",					mrb_allegro4_keyboard_needs_poll,						MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_keyboard,	"[]",										mrb_allegro4_keyboard_index,								MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_keyboard,	"keypressed?",					mrb_allegro4_keyboard_keypressed,						MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_keyboard,	"scancode_to_ascii",		mrb_allegro4_keyboard_scancode_to_ascii,		MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_keyboard,	"scancode_to_name",			mrb_allegro4_keyboard_scancode_to_name,			MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_keyboard,	"simulate_keypress",		mrb_allegro4_keyboard_simulate_keypress,		MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_keyboard,	"simulate_ukeypress",		mrb_allegro4_keyboard_simulate_ukeypress,		MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, module_allegro4_keyboard,	"set_keyboard_rate",		mrb_allegro4_keyboard_set_keyboard_rate,		MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, module_allegro4_keyboard,	"clear_keybuf",					mrb_allegro4_keyboard_clear_keybuf,					MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_keyboard,	"readkey",							mrb_allegro4_keyboard_readkey,							MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_keyboard,	"ureadkey",							mrb_allegro4_keyboard_ureadkey,							MRB_ARGS_REQ(1));
	
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_A", 						mrb_fixnum_value(KEY_A));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_B", 						mrb_fixnum_value(KEY_B));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_C", 						mrb_fixnum_value(KEY_C));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_D", 						mrb_fixnum_value(KEY_D));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_E", 						mrb_fixnum_value(KEY_E));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_F", 						mrb_fixnum_value(KEY_F));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_G", 						mrb_fixnum_value(KEY_G));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_H", 						mrb_fixnum_value(KEY_H));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_I", 						mrb_fixnum_value(KEY_I));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_J", 						mrb_fixnum_value(KEY_J));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_K", 						mrb_fixnum_value(KEY_K));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_L", 						mrb_fixnum_value(KEY_L));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_M", 						mrb_fixnum_value(KEY_M));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_N", 						mrb_fixnum_value(KEY_N));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_O", 						mrb_fixnum_value(KEY_O));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_P", 						mrb_fixnum_value(KEY_P));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_Q", 						mrb_fixnum_value(KEY_Q));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_R", 						mrb_fixnum_value(KEY_R));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_S", 						mrb_fixnum_value(KEY_S));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_T", 						mrb_fixnum_value(KEY_T));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_U", 						mrb_fixnum_value(KEY_U));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_V", 						mrb_fixnum_value(KEY_V));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_W", 						mrb_fixnum_value(KEY_W));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_X", 						mrb_fixnum_value(KEY_X));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_Y", 						mrb_fixnum_value(KEY_Y));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_Z", 						mrb_fixnum_value(KEY_Z));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_0", 						mrb_fixnum_value(KEY_0));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_1", 						mrb_fixnum_value(KEY_1));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_2", 						mrb_fixnum_value(KEY_2));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_3", 						mrb_fixnum_value(KEY_3));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_4", 						mrb_fixnum_value(KEY_4));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_5", 						mrb_fixnum_value(KEY_5));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_6", 						mrb_fixnum_value(KEY_6));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_7", 						mrb_fixnum_value(KEY_7));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_8", 						mrb_fixnum_value(KEY_8));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_9", 						mrb_fixnum_value(KEY_9));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_0_PAD", 				mrb_fixnum_value(KEY_0_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_1_PAD", 				mrb_fixnum_value(KEY_1_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_2_PAD", 				mrb_fixnum_value(KEY_2_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_3_PAD", 				mrb_fixnum_value(KEY_3_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_4_PAD", 				mrb_fixnum_value(KEY_4_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_5_PAD", 				mrb_fixnum_value(KEY_5_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_6_PAD", 				mrb_fixnum_value(KEY_6_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_7_PAD", 				mrb_fixnum_value(KEY_7_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_8_PAD", 				mrb_fixnum_value(KEY_8_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_9_PAD", 				mrb_fixnum_value(KEY_9_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_F1", 					mrb_fixnum_value(KEY_F1));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_F2", 					mrb_fixnum_value(KEY_F2));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_F3", 					mrb_fixnum_value(KEY_F3));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_F4", 					mrb_fixnum_value(KEY_F4));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_F5", 					mrb_fixnum_value(KEY_F5));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_F6", 					mrb_fixnum_value(KEY_F6));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_F7", 					mrb_fixnum_value(KEY_F7));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_F8", 					mrb_fixnum_value(KEY_F8));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_F9", 					mrb_fixnum_value(KEY_F9));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_F10", 					mrb_fixnum_value(KEY_F10));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_F11", 					mrb_fixnum_value(KEY_F11));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_F12", 					mrb_fixnum_value(KEY_F12));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_ESC", 					mrb_fixnum_value(KEY_ESC));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_TILDE", 				mrb_fixnum_value(KEY_TILDE));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_MINUS", 				mrb_fixnum_value(KEY_MINUS));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_EQUALS", 			mrb_fixnum_value(KEY_EQUALS));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_BACKSPACE", 		mrb_fixnum_value(KEY_BACKSPACE));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_TAB", 					mrb_fixnum_value(KEY_TAB));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_OPENBRACE", 		mrb_fixnum_value(KEY_OPENBRACE));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_CLOSEBRACE", 	mrb_fixnum_value(KEY_CLOSEBRACE));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_ENTER", 				mrb_fixnum_value(KEY_ENTER));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_COLON", 				mrb_fixnum_value(KEY_COLON));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_QUOTE", 				mrb_fixnum_value(KEY_QUOTE));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_BACKSLASH", 		mrb_fixnum_value(KEY_BACKSLASH));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_BACKSLASH2", 	mrb_fixnum_value(KEY_BACKSLASH2));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_COMMA", 				mrb_fixnum_value(KEY_COMMA));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_STOP", 				mrb_fixnum_value(KEY_STOP));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_SLASH", 				mrb_fixnum_value(KEY_SLASH));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_SPACE", 				mrb_fixnum_value(KEY_SPACE));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_INSERT", 			mrb_fixnum_value(KEY_INSERT));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_DEL", 					mrb_fixnum_value(KEY_DEL));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_HOME", 				mrb_fixnum_value(KEY_HOME));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_END", 					mrb_fixnum_value(KEY_END));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_PGUP", 				mrb_fixnum_value(KEY_PGUP));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_PGDN", 				mrb_fixnum_value(KEY_PGDN));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_LEFT", 				mrb_fixnum_value(KEY_LEFT));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_RIGHT", 				mrb_fixnum_value(KEY_RIGHT));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_UP", 					mrb_fixnum_value(KEY_UP));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_DOWN", 				mrb_fixnum_value(KEY_DOWN));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_SLASH_PAD", 		mrb_fixnum_value(KEY_SLASH_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_ASTERISK", 		mrb_fixnum_value(KEY_ASTERISK));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_MINUS_PAD", 		mrb_fixnum_value(KEY_MINUS_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_PLUS_PAD", 		mrb_fixnum_value(KEY_PLUS_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_DEL_PAD", 			mrb_fixnum_value(KEY_DEL_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_ENTER_PAD", 		mrb_fixnum_value(KEY_ENTER_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_PRTSCR", 			mrb_fixnum_value(KEY_PRTSCR));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_PAUSE", 				mrb_fixnum_value(KEY_PAUSE));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_ABNT_C1", 			mrb_fixnum_value(KEY_ABNT_C1));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_YEN", 					mrb_fixnum_value(KEY_YEN));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_KANA", 				mrb_fixnum_value(KEY_KANA));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_CONVERT", 			mrb_fixnum_value(KEY_CONVERT));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_NOCONVERT", 		mrb_fixnum_value(KEY_NOCONVERT));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_AT", 					mrb_fixnum_value(KEY_AT));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_CIRCUMFLEX", 	mrb_fixnum_value(KEY_CIRCUMFLEX));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_COLON2", 			mrb_fixnum_value(KEY_COLON2));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_KANJI", 				mrb_fixnum_value(KEY_KANJI));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_EQUALS_PAD", 	mrb_fixnum_value(KEY_EQUALS_PAD));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_BACKQUOTE", 		mrb_fixnum_value(KEY_BACKQUOTE));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_SEMICOLON", 		mrb_fixnum_value(KEY_SEMICOLON));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_COMMAND", 			mrb_fixnum_value(KEY_COMMAND));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_UNKNOWN1", 		mrb_fixnum_value(KEY_UNKNOWN1));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_UNKNOWN2", 		mrb_fixnum_value(KEY_UNKNOWN2));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_UNKNOWN3", 		mrb_fixnum_value(KEY_UNKNOWN3));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_UNKNOWN4", 		mrb_fixnum_value(KEY_UNKNOWN4));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_UNKNOWN5", 		mrb_fixnum_value(KEY_UNKNOWN5));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_UNKNOWN6", 		mrb_fixnum_value(KEY_UNKNOWN6));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_UNKNOWN7", 		mrb_fixnum_value(KEY_UNKNOWN7));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_UNKNOWN8", 		mrb_fixnum_value(KEY_UNKNOWN8));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_MODIFIERS", 		mrb_fixnum_value(KEY_MODIFIERS));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_LSHIFT", 			mrb_fixnum_value(KEY_LSHIFT));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_RSHIFT", 			mrb_fixnum_value(KEY_RSHIFT));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_LCONTROL", 		mrb_fixnum_value(KEY_LCONTROL));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_RCONTROL", 		mrb_fixnum_value(KEY_RCONTROL));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_ALT", 					mrb_fixnum_value(KEY_ALT));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_ALTGR", 				mrb_fixnum_value(KEY_ALTGR));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_LWIN", 				mrb_fixnum_value(KEY_LWIN));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_RWIN", 				mrb_fixnum_value(KEY_RWIN));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_MENU", 				mrb_fixnum_value(KEY_MENU));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_SCRLOCK", 			mrb_fixnum_value(KEY_SCRLOCK));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_NUMLOCK", 			mrb_fixnum_value(KEY_NUMLOCK));
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_CAPSLOCK", 		mrb_fixnum_value(KEY_CAPSLOCK));
	
	mrb_define_const(mrb, module_allegro4_keyboard, "KEY_MAX", 		mrb_fixnum_value(KEY_MAX));
	
	// Joystick
	
	module_allegro4_joystick = mrb_define_module_under(mrb, module_allegro4_input, "Joystick");
	
	mrb_define_module_function(mrb, module_allegro4_joystick,		"install",									mrb_allegro4_joystick_install,									MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_joystick,		"remove",										mrb_allegro4_joystick_remove,										MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_joystick,		"poll",											mrb_allegro4_joystick_poll,											MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_joystick,		"num_joysticks",						mrb_allegro4_joystick_num_joysticks,						MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_joystick,		"[]",												mrb_allegro4_joystick_index,										MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_joystick,		"calibrate_joystick_name",	mrb_allegro4_joystick_calibrate_joystick_name,	MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_joystick,		"calibrate_joystick",				mrb_allegro4_joystick_calibrate_joystick,				MRB_ARGS_REQ(1));

	mrb_define_const(mrb, module_allegro4_joystick, "JOY_TYPE_AUTODETECT",		mrb_fixnum_value(JOY_TYPE_AUTODETECT));
	
	// Mouse
	
	module_allegro4_mouse = mrb_define_module_under(mrb, module_allegro4_input, "Mouse");
	
	mrb_define_module_function(mrb, module_allegro4_mouse, 	"install", 					mrb_allegro4_mouse_install, 									MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_mouse, 	"remove", 						mrb_allegro4_mouse_exit, 										MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_mouse, 	"poll", 							mrb_allegro4_mouse_poll, 										MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_mouse, 	"needs_poll?", 			mrb_allegro4_mouse_needs_poll, 							MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_mouse, 	"hardware_cursor=", 	mrb_allegro4_mouse_hardware_cursor_set, 			MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_mouse, 	"cursor=", 					mrb_allegro4_mouse_cursor_set, 							MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_mouse, 	"cursor_bitmap=", 		mrb_allegro4_mouse_cursor_bitmap_set, 				MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, module_allegro4_mouse, 	"x", 								mrb_allegro4_mouse_x, 												MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_mouse, 	"y", 								mrb_allegro4_mouse_y, 												MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_mouse, 	"z", 								mrb_allegro4_mouse_z, 												MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_mouse, 	"w", 								mrb_allegro4_mouse_w, 												MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_mouse, 	"b", 								mrb_allegro4_mouse_b, 												MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_mouse, 	"pos", 							mrb_allegro4_mouse_pos, 											MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_mouse, 	"on_screen?", 				mrb_allegro4_mouse_on_screen, 								MRB_ARGS_NONE());
	
	mrb_define_const(mrb, module_allegro4_mouse, "MOUSE_CURSOR_NONE",         		mrb_fixnum_value(MOUSE_CURSOR_NONE));
	mrb_define_const(mrb, module_allegro4_mouse, "MOUSE_CURSOR_ALLEGRO",         	mrb_fixnum_value(MOUSE_CURSOR_ALLEGRO));
	mrb_define_const(mrb, module_allegro4_mouse, "MOUSE_CURSOR_ARROW",         		mrb_fixnum_value(MOUSE_CURSOR_ARROW));
	mrb_define_const(mrb, module_allegro4_mouse, "MOUSE_CURSOR_BUSY",        		mrb_fixnum_value(MOUSE_CURSOR_BUSY));
	mrb_define_const(mrb, module_allegro4_mouse, "MOUSE_CURSOR_QUESTION",        mrb_fixnum_value(MOUSE_CURSOR_QUESTION));
	mrb_define_const(mrb, module_allegro4_mouse, "MOUSE_CURSOR_EDIT",         		mrb_fixnum_value(MOUSE_CURSOR_EDIT));
}