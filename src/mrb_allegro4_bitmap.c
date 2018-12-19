#include "mrb_allegro4_bitmap.h"
#include <mruby/data.h>
#include <mruby/class.h>

typedef struct mrb_allegro4_bitmap_data_t {
	BITMAP* bitmap;
	int need_dispose;
} mrb_allegro4_bitmap_data_t;

static void mrb_allegro4_bitmap_data_free(mrb_state *mrb, void *p) {
	mrb_allegro4_bitmap_data_t* data = (mrb_allegro4_bitmap_data_t*)p;
	if (NULL != data) {
		if (data->bitmap && data->need_dispose) {
			destroy_bitmap(data->bitmap);
		}
		mrb_free(mrb, data);
	}
}

static struct mrb_data_type const mrb_allegro4_bitmap_data_type = {
	"allegro4/bitmap", mrb_allegro4_bitmap_data_free
};

BITMAP* mrb_allegro4_bitmap_get_ptr(mrb_state *mrb, mrb_value bitmap) {
	mrb_allegro4_bitmap_data_t *data;
	if (mrb_nil_p(bitmap)) return NULL;
	data = (mrb_allegro4_bitmap_data_t*)mrb_data_get_ptr(mrb, bitmap, &mrb_allegro4_bitmap_data_type);
	return data->bitmap;
}

mrb_value mrb_allegro4_bitmap_set(mrb_state* mrb, BITMAP* bitmap, mrb_bool need_dispose) {
	mrb_allegro4_bitmap_data_t *data = (mrb_allegro4_bitmap_data_t*)mrb_malloc(mrb, sizeof(mrb_allegro4_bitmap_data_t));
	if (NULL == data) return mrb_nil_value();
	data->bitmap = bitmap;
	data->need_dispose = need_dispose;
	return mrb_obj_value(Data_Wrap_Struct(mrb, class_allegro4_bitmap, &mrb_allegro4_bitmap_data_type, data));
}

// =======================================================================
// Bitmap Object
// =======================================================================

struct RClass* class_allegro4_bitmap = NULL;

static mrb_value mrb_allegro4_bitmap_create_video(mrb_state* mrb, mrb_value self) {
	mrb_int width, height;
	mrb_get_args(mrb, "ii", &width, &height);
	if (width * height <= 0) {
		mrb_raise(mrb, E_RUNTIME_ERROR, "bitmap width and height must > 0.");
		return mrb_nil_value();
	}
	BITMAP* bitmap = create_video_bitmap(width, height);
	if (!bitmap) return mrb_nil_value();
	return mrb_allegro4_bitmap_set(mrb, bitmap, 1);
}

static mrb_value mrb_allegro4_bitmap_create_system(mrb_state* mrb, mrb_value self) {
	mrb_int width, height;
	mrb_get_args(mrb, "ii", &width, &height);
	if (width * height <= 0) {
		mrb_raise(mrb, E_RUNTIME_ERROR, "bitmap width and height must > 0.");
		return mrb_nil_value();
	}
	BITMAP* bitmap = create_system_bitmap(width, height);
	if (!bitmap) return mrb_nil_value();
	return mrb_allegro4_bitmap_set(mrb, bitmap, 1);
}

static mrb_value mrb_allegro4_bitmap_destroy(mrb_state* mrb, mrb_value self) {
	mrb_allegro4_bitmap_data_t *data = (mrb_allegro4_bitmap_data_t*)mrb_data_get_ptr(mrb, self, &mrb_allegro4_bitmap_data_type);
	if (data->bitmap && data->need_dispose) {
		destroy_bitmap(data->bitmap);
		data->bitmap = NULL;
		data->need_dispose = 0;
	}
	return self;
}

static mrb_value mrb_allegro4_bitmap_initialize(mrb_state* mrb, mrb_value self) {
	mrb_allegro4_bitmap_data_t* data = (mrb_allegro4_bitmap_data_t*)DATA_PTR(self);
	if (data == NULL) {
		data = (mrb_allegro4_bitmap_data_t*)mrb_malloc(mrb, sizeof(mrb_allegro4_bitmap_data_t));
		if (NULL == data) {
			mrb_raise(mrb, E_RUNTIME_ERROR, "no memory.");
			return self;
		}
		mrb_int width = 0, height = 0;
		mrb_int depth = 8;
		switch(mrb->c->ci->argc){
		case 2:
			mrb_get_args(mrb, "ii", &width, &height);
			if (width * height <= 0) {
				mrb_raise(mrb, E_RUNTIME_ERROR, "bitmap width and height must > 0.");
				return self;
			}
			data->bitmap = create_bitmap(width, height);
			data->need_dispose = 1;
		break;
		case 3:
			mrb_get_args(mrb, "iii", &width, &height, &depth);
			if (width * height <= 0) {
				mrb_raise(mrb, E_RUNTIME_ERROR, "bitmap width and height must > 0.");
				return self;
			}
			data->bitmap = create_bitmap_ex(depth, width, height);
			data->need_dispose = 1;
		break;
		default:
			mrb_raise(mrb, E_ARGUMENT_ERROR, "request (width, height[, depth]).");
		}
		if (NULL == data->bitmap) {
			mrb_raise(mrb, E_RUNTIME_ERROR, "no memory.");
			return self;
		}
		DATA_PTR(self) = data;
		DATA_TYPE(self) = &mrb_allegro4_bitmap_data_type;
	}
	return self;
}

static mrb_value mrb_allegro4_bitmap_color_depth(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	return mrb_fixnum_value(bitmap_color_depth(bitmap));
}

static mrb_value mrb_allegro4_bitmap_mask_color(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	return mrb_fixnum_value(bitmap_mask_color(bitmap));
}

static mrb_value mrb_allegro4_bitmap_is_same_bitmap(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_value other;
	mrb_get_args(mrb, "o", &other);
	BITMAP* other_bitmap = mrb_allegro4_bitmap_get_ptr(mrb, other);
	return mrb_bool_value(is_same_bitmap(bitmap, other_bitmap));
}

static mrb_value mrb_allegro4_bitmap_is_planar_bitmap(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	return mrb_bool_value(is_planar_bitmap(bitmap));
}

static mrb_value mrb_allegro4_bitmap_is_memory_bitmap(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	return mrb_bool_value(is_memory_bitmap(bitmap));
}

static mrb_value mrb_allegro4_bitmap_is_screen_bitmap(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	return mrb_bool_value(is_screen_bitmap(bitmap));
}

static mrb_value mrb_allegro4_bitmap_is_linear_bitmap(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	return mrb_bool_value(is_linear_bitmap(bitmap));
}

static mrb_value mrb_allegro4_bitmap_is_video_bitmap(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	return mrb_bool_value(is_video_bitmap(bitmap));
}

static mrb_value mrb_allegro4_bitmap_is_system_bitmap(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	return mrb_bool_value(is_system_bitmap(bitmap));
}

static mrb_value mrb_allegro4_bitmap_is_sub_bitmap(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	return mrb_bool_value(is_sub_bitmap(bitmap));
}

// =======================================================================
// Bitmap Drawing
// =======================================================================

static mrb_value mrb_allegro4_bitmap_clear(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	clear_bitmap(bitmap);
	return self;
}

static mrb_value mrb_allegro4_bitmap_clear_to_color(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_int color;
	mrb_get_args(mrb, "i", &color);
	clear_to_color(bitmap, color);
	return self;
}

static mrb_value mrb_allegro4_bitmap_putpixel(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_int x, y, color;
	mrb_get_args(mrb, "iii", &x, &y, &color);
	putpixel(bitmap, x, y, color);
	return self;
}

static mrb_value mrb_allegro4_bitmap_getpixel(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_int x, y;
	mrb_get_args(mrb, "ii", &x, &y);
	return mrb_fixnum_value(getpixel(bitmap, x, y));
}

static mrb_value mrb_allegro4_bitmap_vline(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_int x, y1, y2, color;
	mrb_get_args(mrb, "iiii", &x, &y1, &y2, &color);
	vline(bitmap, x, y1, y2, color);
	return self;
}

static mrb_value mrb_allegro4_bitmap_hline(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_int x1, y, x2, color;
	mrb_get_args(mrb, "iiii", &x1, &y, &x2, &color);
	hline(bitmap, x1, y, x2, color);
	return self;
}

static mrb_value mrb_allegro4_bitmap_line(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_int x1, y1, x2, y2, color;
	mrb_get_args(mrb, "iiiii", &x1, &y1, &x2, &y2, &color);
	line(bitmap, x1, y1, x2, y2, color);
	return self;
}

static mrb_value mrb_allegro4_bitmap_fastline(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_int x1, y1, x2, y2, color;
	mrb_get_args(mrb, "iiiii", &x1, &y1, &x2, &y2, &color);
	fastline(bitmap, x1, y1, x2, y2, color);
	return self;
}

static mrb_value mrb_allegro4_bitmap_fill_triangle(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_int x1, y1, x2, y2, x3, y3, color;
	mrb_get_args(mrb, "iiiiiii", &x1, &y1, &x2, &y2, &x3, &y3, color);
	triangle(bitmap, x1, y1, x2, y2, x3, y3, color);
	return self;
}

static mrb_value mrb_allegro4_bitmap_draw_rect(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_int x1, y1, x2, y2, color;
	mrb_get_args(mrb, "iiiii", &x1, &y1, &x2, &y2, &color);
	rect(bitmap, x1, y1, x2, y2, color);
	return self;
}

static mrb_value mrb_allegro4_bitmap_fill_rect(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_int x1, y1, x2, y2, color;
	mrb_get_args(mrb, "iiiii", &x1, &y1, &x2, &y2, &color);
	rectfill(bitmap, x1, y1, x2, y2, color);
	return self;
}

static mrb_value mrb_allegro4_bitmap_draw_circle(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_int x, y, radius, color;
	mrb_get_args(mrb, "iiii", &x, &y, &radius, &color);
	circle(bitmap,x, y, radius, color);
	return self;
}

static mrb_value mrb_allegro4_bitmap_fill_circle(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_int x, y, radius, color;
	mrb_get_args(mrb, "iiii", &x, &y, &radius, &color);
	circlefill(bitmap,x, y, radius, color);
	return self;
}

static mrb_value mrb_allegro4_bitmap_draw_ellipse(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_int x, y, rx, ry, radius, color;
	mrb_get_args(mrb, "iiiii", &x, &y, &rx, &ry, &color);
	ellipse(bitmap,x, y, rx, ry, color);
	return self;
}

static mrb_value mrb_allegro4_bitmap_fill_ellipse(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_int x, y, rx, ry, radius, color;
	mrb_get_args(mrb, "iiiii", &x, &y, &rx, &ry, &color);
	ellipsefill(bitmap,x, y, rx, ry, color);
	return self;
}

static mrb_value mrb_allegro4_bitmap_arc(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_int x, y, ang1, ang2, r, color;
	mrb_get_args(mrb, "iiiiii", &x, &y, &ang1, &ang2, &r, &color);
	arc(bitmap, x, y, ang1, ang2, r, color);
	return self;
}

// =======================================================================
// Blitting
// =======================================================================

static mrb_value mrb_allegro4_bitmap_blit(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_value other;
	mrb_int src_x, src_y, dest_x, dest_y, width, height;
	mrb_get_args(mrb, "oiiiiii", &other, &src_x, &src_y, &dest_x, &dest_y, &width, &height);
	BITMAP* other_bitmap = mrb_allegro4_bitmap_get_ptr(mrb, other);
	blit(bitmap, other_bitmap, src_x, src_y, dest_x, dest_y, width, height);
	return self;
}

static mrb_value mrb_allegro4_bitmap_masked_blit(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_value other;
	mrb_int src_x, src_y, dest_x, dest_y, width, height;
	mrb_get_args(mrb, "oiiiiii", &other, &src_x, &src_y, &dest_x, &dest_y, &width, &height);
	BITMAP* other_bitmap = mrb_allegro4_bitmap_get_ptr(mrb, other);
	masked_blit(bitmap, other_bitmap, src_x, src_y, dest_x, dest_y, width, height);
	return self;
}

static mrb_value mrb_allegro4_bitmap_stretch_blit(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_value other;
	mrb_int src_x, src_y, src_w, src_h, dest_x, dest_y, width, height;
	mrb_get_args(mrb, "oiiiiiiii", &other, &src_x, &src_y, &src_w, &src_h, &dest_x, &dest_y, &width, &height);
	BITMAP* other_bitmap = mrb_allegro4_bitmap_get_ptr(mrb, other);
	stretch_blit(bitmap, other_bitmap, src_x, src_y, src_w, src_h, dest_x, dest_y, width, height);
	return self;
}

static mrb_value mrb_allegro4_bitmap_masked_stretch_blit(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_value other;
	mrb_int src_x, src_y, src_w, src_h, dest_x, dest_y, width, height;
	mrb_get_args(mrb, "oiiiiiiii", &other, &src_x, &src_y, &src_w, &src_h, &dest_x, &dest_y, &width, &height);
	BITMAP* other_bitmap = mrb_allegro4_bitmap_get_ptr(mrb, other);
	masked_stretch_blit(bitmap, other_bitmap, src_x, src_y, src_w, src_h, dest_x, dest_y, width, height);
	return self;
}

// =======================================================================
// Parameters
// =======================================================================
static mrb_value mrb_allegro4_bitmap_width(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	return mrb_fixnum_value(bitmap->w);
}

static mrb_value mrb_allegro4_bitmap_height(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	return mrb_fixnum_value(bitmap->h);
}

// =======================================================================
// Sprites
// =======================================================================
static mrb_value mrb_allegro4_bitmap_draw_sprite_ex(mrb_state* mrb, mrb_value self) {
	BITMAP* bitmap = mrb_allegro4_bitmap_get_ptr(mrb, self);
	mrb_value other;
	mrb_int x, y, mode, flip;
	mrb_get_args(mrb, "oiiii", &other, &x, &y, &mode, &flip);
	BITMAP* other_bitmap = mrb_allegro4_bitmap_get_ptr(mrb, other);
	draw_sprite_ex(bitmap, other_bitmap, x, y, mode, flip);
	return self;
}

// =======================================================================
// Blenders
// =======================================================================
static mrb_value mrb_allegro4_bitmap_set_trans_blender(mrb_state* mrb, mrb_value self) {
	mrb_int r, g, b, a;
	mrb_get_args(mrb, "iiii", &r, &g, &b, &a);
	set_trans_blender(r, g, b, a);
	return mrb_nil_value();
}

void mrb_mruby_allegro4_bitmap_init(mrb_state* mrb){
	class_allegro4_bitmap = mrb_define_class_under(mrb, module_allegro4, "Bitmap", mrb->object_class);
	
	MRB_SET_INSTANCE_TT(class_allegro4_bitmap,  MRB_TT_DATA);
	
	mrb_define_class_method(mrb, class_allegro4_bitmap, "create_video", 	mrb_allegro4_bitmap_create_video,		MRB_ARGS_REQ(2));
	mrb_define_class_method(mrb, class_allegro4_bitmap, "create_system", 	mrb_allegro4_bitmap_create_system,	MRB_ARGS_REQ(2));
	
	mrb_define_method(mrb, class_allegro4_bitmap, "initialize",			mrb_allegro4_bitmap_initialize,					MRB_ARGS_OPT(3));
	mrb_define_method(mrb, class_allegro4_bitmap, "destroy",					mrb_allegro4_bitmap_destroy,						MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_bitmap, "color_depth",			mrb_allegro4_bitmap_color_depth,				MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_bitmap, "mask_color",			mrb_allegro4_bitmap_mask_color,					MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_bitmap, "==",							mrb_allegro4_bitmap_is_same_bitmap,			MRB_ARGS_REQ(1));
	mrb_define_method(mrb, class_allegro4_bitmap, "is_planar?",			mrb_allegro4_bitmap_is_planar_bitmap,		MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_bitmap, "is_memory?",			mrb_allegro4_bitmap_is_memory_bitmap,		MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_bitmap, "is_screen?",			mrb_allegro4_bitmap_is_screen_bitmap,		MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_bitmap, "is_linear?",			mrb_allegro4_bitmap_is_linear_bitmap,		MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_bitmap, "is_video?",				mrb_allegro4_bitmap_is_video_bitmap,		MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_bitmap, "is_system?",			mrb_allegro4_bitmap_is_system_bitmap,		MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_bitmap, "is_sub?",					mrb_allegro4_bitmap_is_sub_bitmap,			MRB_ARGS_NONE());
	
	mrb_define_method(mrb, class_allegro4_bitmap, "clear",						mrb_allegro4_bitmap_clear,							MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_bitmap, "clear_to_color",	mrb_allegro4_bitmap_clear_to_color,			MRB_ARGS_REQ(1));
	mrb_define_method(mrb, class_allegro4_bitmap, "[]=",							mrb_allegro4_bitmap_putpixel,						MRB_ARGS_REQ(3));
	mrb_define_method(mrb, class_allegro4_bitmap, "[]",							mrb_allegro4_bitmap_getpixel,						MRB_ARGS_REQ(2));
	mrb_define_method(mrb, class_allegro4_bitmap, "v_line",					mrb_allegro4_bitmap_vline,							MRB_ARGS_REQ(4));
	mrb_define_method(mrb, class_allegro4_bitmap, "h_line",					mrb_allegro4_bitmap_hline,							MRB_ARGS_REQ(4));
	mrb_define_method(mrb, class_allegro4_bitmap, "line",						mrb_allegro4_bitmap_line,								MRB_ARGS_REQ(5));
	mrb_define_method(mrb, class_allegro4_bitmap, "fast_line",				mrb_allegro4_bitmap_fastline,						MRB_ARGS_REQ(5));
	mrb_define_method(mrb, class_allegro4_bitmap, "fill_triangle",		mrb_allegro4_bitmap_fill_triangle,			MRB_ARGS_REQ(7));
	mrb_define_method(mrb, class_allegro4_bitmap, "draw_rect",				mrb_allegro4_bitmap_draw_rect,					MRB_ARGS_REQ(5));
	mrb_define_method(mrb, class_allegro4_bitmap, "fill_rect",				mrb_allegro4_bitmap_fill_rect,					MRB_ARGS_REQ(5));
	mrb_define_method(mrb, class_allegro4_bitmap, "draw_circle",			mrb_allegro4_bitmap_draw_circle,				MRB_ARGS_REQ(4));
	mrb_define_method(mrb, class_allegro4_bitmap, "fill_circle",			mrb_allegro4_bitmap_fill_circle,				MRB_ARGS_REQ(4));
	mrb_define_method(mrb, class_allegro4_bitmap, "draw_ellipse",		mrb_allegro4_bitmap_draw_ellipse,				MRB_ARGS_REQ(5));
	mrb_define_method(mrb, class_allegro4_bitmap, "fill_ellipse",		mrb_allegro4_bitmap_fill_ellipse,				MRB_ARGS_REQ(5));
	mrb_define_method(mrb, class_allegro4_bitmap, "arc",							mrb_allegro4_bitmap_arc,								MRB_ARGS_REQ(6));
	
	mrb_define_method(mrb, class_allegro4_bitmap, "blit",								mrb_allegro4_bitmap_blit,									MRB_ARGS_REQ(7));
	mrb_define_method(mrb, class_allegro4_bitmap, "masked_blit",					mrb_allegro4_bitmap_masked_blit,					MRB_ARGS_REQ(7));
	mrb_define_method(mrb, class_allegro4_bitmap, "stretch_blit",				mrb_allegro4_bitmap_stretch_blit,					MRB_ARGS_REQ(9));
	mrb_define_method(mrb, class_allegro4_bitmap, "masked_stretch_blit",	mrb_allegro4_bitmap_masked_stretch_blit,	MRB_ARGS_REQ(9));
	
	mrb_define_method(mrb, class_allegro4_bitmap, "width",						mrb_allegro4_bitmap_width,							MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_bitmap, "height",					mrb_allegro4_bitmap_height,							MRB_ARGS_NONE());
	
	mrb_define_method(mrb, class_allegro4_bitmap, "draw_sprite_ex",	mrb_allegro4_bitmap_draw_sprite_ex,			MRB_ARGS_REQ(5));
	
	mrb_define_class_method(mrb, class_allegro4_bitmap, "set_trans_blender",		 mrb_allegro4_bitmap_set_trans_blender,		MRB_ARGS_REQ(4));
	
	mrb_define_const(mrb, class_allegro4_bitmap, "DRAW_MODE_SOLID",         			mrb_fixnum_value(DRAW_MODE_SOLID));
	mrb_define_const(mrb, class_allegro4_bitmap, "DRAW_MODE_XOR",						mrb_fixnum_value(DRAW_MODE_XOR));
	mrb_define_const(mrb, class_allegro4_bitmap, "DRAW_MODE_COPY_PATTERN",		mrb_fixnum_value(DRAW_MODE_COPY_PATTERN));
	mrb_define_const(mrb, class_allegro4_bitmap, "DRAW_MODE_SOLID_PATTERN",	mrb_fixnum_value(DRAW_MODE_SOLID_PATTERN));
	mrb_define_const(mrb, class_allegro4_bitmap, "DRAW_MODE_MASKED_PATTERN",	mrb_fixnum_value(DRAW_MODE_MASKED_PATTERN));
	mrb_define_const(mrb, class_allegro4_bitmap, "DRAW_MODE_TRANS",					mrb_fixnum_value(DRAW_MODE_TRANS));
	
	mrb_define_const(mrb, class_allegro4_bitmap, "DRAW_SPRITE_NORMAL",         		mrb_fixnum_value(DRAW_SPRITE_NORMAL));
	mrb_define_const(mrb, class_allegro4_bitmap, "DRAW_SPRITE_LIT",         			mrb_fixnum_value(DRAW_SPRITE_LIT));
	mrb_define_const(mrb, class_allegro4_bitmap, "DRAW_SPRITE_TRANS",         		mrb_fixnum_value(DRAW_SPRITE_TRANS));
	
	mrb_define_const(mrb, class_allegro4_bitmap, "DRAW_SPRITE_NO_FLIP",         	mrb_fixnum_value(DRAW_SPRITE_NO_FLIP));
	mrb_define_const(mrb, class_allegro4_bitmap, "DRAW_SPRITE_H_FLIP",         		mrb_fixnum_value(DRAW_SPRITE_H_FLIP));
	mrb_define_const(mrb, class_allegro4_bitmap, "DRAW_SPRITE_V_FLIP",         		mrb_fixnum_value(DRAW_SPRITE_V_FLIP));
	mrb_define_const(mrb, class_allegro4_bitmap, "DRAW_SPRITE_VH_FLIP",         	mrb_fixnum_value(DRAW_SPRITE_VH_FLIP));
}
