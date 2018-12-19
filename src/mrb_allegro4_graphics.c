#include "mrb_allegro4_graphics.h"
#include "mrb_allegro4_bitmap.h"
#include <mruby/data.h>
#include <mruby/array.h>
#include <mruby/class.h>

struct RClass* module_allegro4_graphics = NULL;

static mrb_value mrb_allegro4_graphics_color_depth_set(mrb_state* mrb, mrb_value self) {
	mrb_int depth;
	mrb_get_args(mrb, "i", &depth);
	set_color_depth(depth);
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_graphics_color_depth(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(get_color_depth());
}

static mrb_value mrb_allegro4_graphics_refresh_rate_set(mrb_state* mrb, mrb_value self) {
	mrb_int rate;
	mrb_get_args(mrb, "i", &rate);
	request_refresh_rate(rate);
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_graphics_refresh_rate(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(get_refresh_rate());
}

static mrb_value mrb_allegro4_graphics_gfx_mode_list(mrb_state* mrb, mrb_value self) {
	mrb_int card;
	mrb_get_args(mrb, "i", &card);
	GFX_MODE_LIST* list = get_gfx_mode_list(card);
	if (!list) return mrb_nil_value();
	mrb_value result = mrb_ary_new_capa(mrb, list->num_modes);
	int i = 0;
	for (; i < list->num_modes; i++) {
		GFX_MODE mode = list->mode[i];
		mrb_value gfx_mode = mrb_ary_new_capa(mrb, 3);
		mrb_ary_push(mrb, gfx_mode, mrb_fixnum_value(mode.width));
		mrb_ary_push(mrb, gfx_mode, mrb_fixnum_value(mode.height));
		mrb_ary_push(mrb, gfx_mode, mrb_fixnum_value(mode.bpp));
		mrb_ary_push(mrb, result, gfx_mode);
	}
	destroy_gfx_mode_list(list);
	return result;
}

static mrb_value mrb_allegro4_graphics_gfx_mode_set(mrb_state* mrb, mrb_value self) {
	mrb_int card, w, h, v_w, v_h;
	mrb_get_args(mrb, "iiiii", &card, &w, &h, &v_w, &v_h);
	if (set_gfx_mode(card, w, h, v_w, v_h)) {
		mrb_raise(mrb, E_RUNTIME_ERROR, allegro_error);
	}
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_graphics_display_switch_mode_set(mrb_state* mrb, mrb_value self) {
	mrb_int mode;
	mrb_get_args(mrb, "i", &mode);
	return mrb_fixnum_value(set_display_switch_mode(mode));
}

static mrb_value mrb_allegro4_graphics_display_switch_mode(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(get_display_switch_mode());
}

static mrb_value mrb_allegro4_graphics_is_windowed_mode(mrb_state* mrb, mrb_value self) {
	return mrb_bool_value(is_windowed_mode());
}

static mrb_value mrb_allegro4_graphics_gfx_mode_type(mrb_state* mrb, mrb_value self) {
	mrb_int card;
	mrb_get_args(mrb, "i", &card);
	return mrb_fixnum_value(get_gfx_mode_type(card));
}

static mrb_value mrb_allegro4_graphics_gfx_mode_get(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(get_gfx_mode());
}

static mrb_value mrb_allegro4_graphics_enable_triple_buffer(mrb_state* mrb, mrb_value self) {
	return mrb_bool_value(enable_triple_buffer());
}

static mrb_value mrb_allegro4_graphics_scroll_screen(mrb_state* mrb, mrb_value self) {
	mrb_int x, y;
	mrb_get_args(mrb, "ii", &x, &y);
	return mrb_bool_value(scroll_screen(x, y));
}

static mrb_value mrb_allegro4_graphics_request_scroll(mrb_state* mrb, mrb_value self) {
	mrb_int x, y;
	mrb_get_args(mrb, "ii", &x, &y);
	return mrb_bool_value(request_scroll(x, y));
}

static mrb_value mrb_allegro4_graphics_poll_scroll(mrb_state* mrb, mrb_value self) {
	return mrb_bool_value(poll_scroll());
}

static mrb_value mrb_allegro4_graphics_vsync(mrb_state* mrb, mrb_value self) {
	vsync();
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_graphics_show_video_bitmap(mrb_state* mrb, mrb_value self) {
	mrb_value bitmap;
	mrb_get_args(mrb, "o", &bitmap);
	BITMAP* video_buffer = mrb_allegro4_bitmap_get_ptr(mrb, bitmap);
	if (!video_buffer) return mrb_nil_value();
	return mrb_fixnum_value(show_video_bitmap(video_buffer));
}

static mrb_value mrb_allegro4_graphics_capabilities(mrb_state* mrb, mrb_value self) {
	return mrb_fixnum_value(gfx_capabilities);
}

static mrb_value mrb_allegro4_graphics_screen_bitmap(mrb_state* mrb, mrb_value self) {
	return mrb_allegro4_bitmap_set(mrb, screen, 0);
}

void mrb_mruby_allegro4_graphics_init(mrb_state* mrb) {
	module_allegro4_graphics = mrb_define_module_under(mrb, module_allegro4, "Graphics");
	
	mrb_define_module_function(mrb, module_allegro4_graphics, 	"color_depth=", 					mrb_allegro4_graphics_color_depth_set, 					MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_graphics, 	"color_depth", 					mrb_allegro4_graphics_color_depth, 							MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_graphics,	"refresh_rate=",				mrb_allegro4_graphics_refresh_rate_set,					MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_graphics,	"refresh_rate",					mrb_allegro4_graphics_refresh_rate,							MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_graphics,	"gfx_mode_list",				mrb_allegro4_graphics_gfx_mode_list,						MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_graphics,	"gfx_mode_set",					mrb_allegro4_graphics_gfx_mode_set,							MRB_ARGS_REQ(5));
	mrb_define_module_function(mrb, module_allegro4_graphics,	"display_switch_mode=",	mrb_allegro4_graphics_display_switch_mode_set,	MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_graphics,	"display_switch_mode",	mrb_allegro4_graphics_display_switch_mode,			MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_graphics,	"is_windowed_mode?",		mrb_allegro4_graphics_is_windowed_mode,					MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_graphics,	"gfx_mode_type",				mrb_allegro4_graphics_gfx_mode_type,						MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_graphics,	"gfx_mode_get",					mrb_allegro4_graphics_gfx_mode_get,							MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_graphics,	"enable_triple_buffer",	mrb_allegro4_graphics_enable_triple_buffer,			MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_graphics,	"scroll_screen",				mrb_allegro4_graphics_scroll_screen,						MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, module_allegro4_graphics,	"request_scroll",				mrb_allegro4_graphics_request_scroll,						MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, module_allegro4_graphics,	"poll_scroll",					mrb_allegro4_graphics_poll_scroll,							MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_graphics,	"vsync",								mrb_allegro4_graphics_vsync,										MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_graphics,	"show_video_bitmap",		mrb_allegro4_graphics_show_video_bitmap,				MRB_ARGS_REQ(1));
	
	mrb_define_module_function(mrb, module_allegro4_graphics,	"capabilities",					mrb_allegro4_graphics_capabilities,							MRB_ARGS_NONE());
	
	mrb_define_module_function(mrb, module_allegro4_graphics,	"screen_bitmap",				mrb_allegro4_graphics_screen_bitmap,						MRB_ARGS_NONE());
	
	mrb_define_const(mrb, module_allegro4_graphics, "GFX_AUTODETECT",         							mrb_fixnum_value(GFX_AUTODETECT));
	mrb_define_const(mrb, module_allegro4_graphics, "GFX_AUTODETECT_FULLSCREEN",         		mrb_fixnum_value(GFX_AUTODETECT_FULLSCREEN));
	mrb_define_const(mrb, module_allegro4_graphics, "GFX_AUTODETECT_WINDOWED",         			mrb_fixnum_value(GFX_AUTODETECT_WINDOWED));
	mrb_define_const(mrb, module_allegro4_graphics, "GFX_SAFE",         										mrb_fixnum_value(GFX_SAFE));
	mrb_define_const(mrb, module_allegro4_graphics, "GFX_TEXT",         										mrb_fixnum_value(GFX_TEXT));
	
	mrb_define_const(mrb, module_allegro4_graphics, "GFX_HW_VRAM_BLIT",         						mrb_fixnum_value(GFX_HW_VRAM_BLIT));
	mrb_define_const(mrb, module_allegro4_graphics, "GFX_HW_MEM_BLIT",         							mrb_fixnum_value(GFX_HW_MEM_BLIT));
	
	/* Specific */
	mrb_define_const(mrb, module_allegro4_graphics, "GFX_GDI",         											mrb_fixnum_value(GFX_GDI));
	mrb_define_const(mrb, module_allegro4_graphics, "GFX_DIRECTX_WIN",         							mrb_fixnum_value(GFX_DIRECTX_WIN));
	mrb_define_const(mrb, module_allegro4_graphics, "GFX_DIRECTX_SAFE",         						mrb_fixnum_value(GFX_DIRECTX_SAFE));
	
	mrb_define_const(mrb, module_allegro4_graphics, "SWITCH_NONE",         									mrb_fixnum_value(SWITCH_NONE));
	mrb_define_const(mrb, module_allegro4_graphics, "SWITCH_PAUSE",         								mrb_fixnum_value(SWITCH_PAUSE));
	mrb_define_const(mrb, module_allegro4_graphics, "SWITCH_AMNESIA",         							mrb_fixnum_value(SWITCH_AMNESIA));
	mrb_define_const(mrb, module_allegro4_graphics, "SWITCH_BACKGROUND",         						mrb_fixnum_value(SWITCH_BACKGROUND));
	mrb_define_const(mrb, module_allegro4_graphics, "SWITCH_BACKAMNESIA",         					mrb_fixnum_value(SWITCH_BACKAMNESIA));
	
	mrb_define_const(mrb, module_allegro4_graphics, "GFX_TYPE_UNKNOWN",         						mrb_fixnum_value(GFX_TYPE_UNKNOWN));
	mrb_define_const(mrb, module_allegro4_graphics, "GFX_TYPE_WINDOWED",         						mrb_fixnum_value(GFX_TYPE_WINDOWED));
	mrb_define_const(mrb, module_allegro4_graphics, "GFX_TYPE_FULLSCREEN",         					mrb_fixnum_value(GFX_TYPE_FULLSCREEN));
	mrb_define_const(mrb, module_allegro4_graphics, "GFX_TYPE_DEFINITE",         						mrb_fixnum_value(GFX_TYPE_DEFINITE));
	mrb_define_const(mrb, module_allegro4_graphics, "GFX_TYPE_MAGIC",         							mrb_fixnum_value(GFX_TYPE_MAGIC));
}
