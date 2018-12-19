#include "mrb_allegro4_file.h"
#include <stdio.h>
#include <string.h>
#include <mruby/data.h>
#include <mruby/class.h>
#include <mruby/string.h>

/* Normal File*/
static int normal_file_fclose(void* userdata) {
	FILE* file = (FILE*)userdata;
	return fclose(file);
}

static int normal_file_getc(void* userdata) {
	FILE* file = (FILE*)userdata;
	return fgetc(file);
}

static int normal_file_ungetc(int c, void* userdata) {
	FILE* file = (FILE*)userdata;
	return ungetc(c, file);
}

static long normal_file_fread(void* p, long n, void* userdata) {
	FILE* file = (FILE*)userdata;
	return fread(p, 1, n, file);
}

static int normal_file_putc(int c, void* userdata) {
	FILE* file = (FILE*)userdata;
	return fputc(c, file);
}

static long normal_file_fwrite(AL_CONST void* p, long n, void* userdata) {
	FILE* file = (FILE*)userdata;
	return fwrite(p, 1, n, file);
}

static int normal_file_fseek(void* userdata, int offset) {
	FILE* file = (FILE*)userdata;
	return fseek(file, offset, SEEK_SET);
}

static int normal_file_feof(void* userdata) {
	FILE* file = (FILE*)userdata;
	return feof(file);
}

static int normal_file_ferror(void* userdata) {
	FILE* file = (FILE*)userdata;
	return ferror(file);
}

struct PACKFILE_VTABLE const normal_file_vtable = {
	normal_file_fclose, normal_file_getc, normal_file_ungetc, normal_file_fread, normal_file_putc, normal_file_fwrite, normal_file_fseek, normal_file_feof, normal_file_ferror
};

/* Mem File*/
static int memory_file_fclose(void* userdata) {
	struct memory_file_struct* file = (struct memory_file_struct*)userdata;
	if (file->auto_free) {
		mrb_free(file->mrb, file->ptr_beg);
	}
	mrb_free(file->mrb, file);
	return 0;
}

static int memory_file_getc(void* userdata) {
	struct memory_file_struct* file = (struct memory_file_struct*)userdata;
	if (file->pos >= file->size) {
		return -1;
	}
	return file->ptr_beg[file->pos++];
}

static int memory_file_ungetc(int c, void* userdata) {
	struct memory_file_struct* file = (struct memory_file_struct*)userdata;
	if (file->pos >= file->size) {
		return -1;
	}
	file->ptr_beg[file->pos++] = c;
	return c;
}

static long memory_file_fread(void* p, long n, void* userdata) {
	struct memory_file_struct* file = (struct memory_file_struct*)userdata;
	long result = 0;
	if (file->size - file->pos < n) {
		result = file->size - file->pos;
	} else {
		result = n;
	}
	memcpy(p, file->ptr_beg + file->pos, result);
	file->pos += result;
	return result;
}

static int memory_file_putc(int c, void* userdata) {
	struct memory_file_struct* file = (struct memory_file_struct*)userdata;
	if (file->pos >= file->size) {
		return -1;
	}
	file->ptr_beg[file->pos++] = c;
	return c;
}

static long memory_file_fwrite(AL_CONST void* p, long n, void* userdata) {
	struct memory_file_struct* file = (struct memory_file_struct*)userdata;
	long result = 0;
	if (file->size - file->pos < n) {
		result = file->size - file->pos;
	} else {
		result = n;
	}
	memcpy(file->ptr_beg + file->pos, p, result);
	file->pos += result;
	return result;
}

static int memory_file_fseek(void* userdata, int offset) {
	struct memory_file_struct* file = (struct memory_file_struct*)userdata;
	if (offset >= file->size) {
		offset = file->size;
	} else if (offset < 0) {
		offset = 0;
	}
	file->pos = offset;
	return 0;
}

static int memory_file_feof(void* userdata) {
	struct memory_file_struct* file = (struct memory_file_struct*)userdata;
	return file->pos >= file->size;
}

static int memory_file_ferror(void* userdata) {
	struct memory_file_struct* file = (struct memory_file_struct*)userdata;
	return 0;
}

struct PACKFILE_VTABLE const memory_file_vtable = {
	memory_file_fclose, memory_file_getc, memory_file_ungetc, memory_file_fread, memory_file_putc, memory_file_fwrite, memory_file_fseek, memory_file_feof, memory_file_ferror
};

/* PackFile For Mruby */
typedef struct mrb_allegro4_packfile_data_t {
	PACKFILE* packfile;
} mrb_allegro4_packfile_data_t;

static void mrb_allegro4_packfile_data_free(mrb_state *mrb, void *p) {
	mrb_allegro4_packfile_data_t* data = (mrb_allegro4_packfile_data_t*)p;
	if (NULL != data) {
		if(data->packfile) pack_fclose(data->packfile);
		mrb_free(mrb, data);
	}
}

static struct mrb_data_type const mrb_allegro4_packfile_data_type = {
	"allegro4/packfile", mrb_allegro4_packfile_data_free
};

PACKFILE* mrb_allegro4_packfile_get_ptr(mrb_state *mrb, mrb_value packfile) {
	mrb_allegro4_packfile_data_t *data;
	if (mrb_nil_p(packfile)) return NULL;
	data = (mrb_allegro4_packfile_data_t*)mrb_data_get_ptr(mrb, packfile, &mrb_allegro4_packfile_data_type);
	return data->packfile;
}

mrb_value mrb_allegro4_packfile_set(mrb_state* mrb, PACKFILE* packfile) {
	mrb_allegro4_packfile_data_t *data = (mrb_allegro4_packfile_data_t*)mrb_malloc(mrb, sizeof(mrb_allegro4_packfile_data_t));
	if (NULL == data) return mrb_nil_value();
	data->packfile = packfile;
	return mrb_obj_value(Data_Wrap_Struct(mrb, class_allegro4_normal_file, &mrb_allegro4_packfile_data_type, data));
}

/* File Object */
struct RClass* module_allegro4_file = NULL;
struct RClass* class_allegro4_normal_file = NULL;
struct RClass* class_allegro4_memory_file = NULL;

static mrb_value mrb_allegro4_normal_file_initialize(mrb_state* mrb, mrb_value self) {
	mrb_allegro4_packfile_data_t* data = (mrb_allegro4_packfile_data_t*)DATA_PTR(self);
	if (data == NULL) {
		data = (mrb_allegro4_packfile_data_t*)mrb_malloc(mrb, sizeof(mrb_allegro4_packfile_data_t));
		if (NULL == data) {
			mrb_raise(mrb, E_RUNTIME_ERROR, "no memory.");
			return self;
		}
		char* filename;
		char* mode;
		mrb_get_args(mrb, "zz", &filename, &mode);
		FILE* stdio_file = fopen(filename, mode);
		if (!stdio_file) {
			mrb_raise(mrb, E_RUNTIME_ERROR, "stdio fopen error.");
			return self;
		}
		data->packfile = pack_fopen_vtable(&normal_file_vtable, (void*)stdio_file);
		if (NULL == data->packfile) {
			mrb_raise(mrb, E_RUNTIME_ERROR, "no memory.");
			return self;
		}
		DATA_PTR(self) = data;
		DATA_TYPE(self) = &mrb_allegro4_packfile_data_type;
	}
	return self;
}

static mrb_value mrb_allegro4_normal_file_fclose(mrb_state* mrb, mrb_value self) {
	PACKFILE* packfile = (PACKFILE*)mrb_allegro4_packfile_get_ptr(mrb, self);
	return mrb_fixnum_value(pack_fclose(packfile));
}

static mrb_value mrb_allegro4_normal_file_getc(mrb_state* mrb, mrb_value self) {
	PACKFILE* packfile = (PACKFILE*)mrb_allegro4_packfile_get_ptr(mrb, self);
	return mrb_fixnum_value(pack_getc(packfile));
}

static mrb_value mrb_allegro4_normal_file_ungetc(mrb_state* mrb, mrb_value self) {
	PACKFILE* packfile = (PACKFILE*)mrb_allegro4_packfile_get_ptr(mrb, self);
	mrb_int c;
	mrb_get_args(mrb, "i", &c);
	return mrb_fixnum_value(pack_ungetc(c, packfile));
}

static mrb_value mrb_allegro4_normal_file_fread(mrb_state* mrb, mrb_value self) {
	PACKFILE* packfile = (PACKFILE*)mrb_allegro4_packfile_get_ptr(mrb, self);
	mrb_int size;
	mrb_get_args(mrb, "i", &size);
	mrb_value str = mrb_str_buf_new(mrb, size);
	int bytes = pack_fread(RSTRING_PTR(str), size, packfile);
	if (bytes > 0) {
		mrb_str_resize(mrb, str, bytes);
	}
	return str;
	// return mrb_fixnum_value(pack_fread(packfile));
}

static mrb_value mrb_allegro4_normal_file_putc(mrb_state* mrb, mrb_value self) {
	PACKFILE* packfile = (PACKFILE*)mrb_allegro4_packfile_get_ptr(mrb, self);
	mrb_int c;
	mrb_get_args(mrb, "i", &c);
	return mrb_fixnum_value(pack_putc(c, packfile));
	// return mrb_fixnum_value(pack_putc(packfile));
}

static mrb_value mrb_allegro4_normal_file_fwrite(mrb_state* mrb, mrb_value self) {
	PACKFILE* packfile = (PACKFILE*)mrb_allegro4_packfile_get_ptr(mrb, self);
	mrb_value str;
	mrb_get_args(mrb, "S", &str);
	return mrb_fixnum_value(pack_fwrite(RSTRING_PTR(str), RSTRING_LEN(str), packfile));
	// return mrb_fixnum_value(pack_fwrite(packfile));
}

static mrb_value mrb_allegro4_normal_file_fseek(mrb_state* mrb, mrb_value self) {
	PACKFILE* packfile = (PACKFILE*)mrb_allegro4_packfile_get_ptr(mrb, self);
	mrb_int offset;
	mrb_get_args(mrb, "i", &offset);
	return mrb_fixnum_value(pack_fseek(packfile, offset));
}

static mrb_value mrb_allegro4_normal_file_feof(mrb_state* mrb, mrb_value self) {
	PACKFILE* packfile = (PACKFILE*)mrb_allegro4_packfile_get_ptr(mrb, self);
	return mrb_bool_value(pack_feof(packfile));
}

static mrb_value mrb_allegro4_normal_file_ferror(mrb_state* mrb, mrb_value self) {
	PACKFILE* packfile = (PACKFILE*)mrb_allegro4_packfile_get_ptr(mrb, self);
	return mrb_fixnum_value(pack_ferror(packfile));
}

#define GETS_CAPA 1024
static mrb_value mrb_allegro4_normal_file_gets(mrb_state* mrb, mrb_value self) {
	PACKFILE* packfile = (PACKFILE*)mrb_allegro4_packfile_get_ptr(mrb, self);
	mrb_value str = mrb_str_new_capa(mrb, GETS_CAPA);
	pack_fgets(RSTRING_PTR(str), GETS_CAPA, packfile);
	return str;
}

static mrb_value mrb_allegro4_normal_file_puts(mrb_state* mrb, mrb_value self) {
	PACKFILE* packfile = (PACKFILE*)mrb_allegro4_packfile_get_ptr(mrb, self);
	char* str = NULL;
	mrb_get_args(mrb, "z", &str);
	return mrb_fixnum_value(pack_fputs(str, packfile));
}

static mrb_value mrb_allegro4_memory_file_initialize(mrb_state* mrb, mrb_value self) {
	mrb_allegro4_packfile_data_t* data = (mrb_allegro4_packfile_data_t*)DATA_PTR(self);
	if (data == NULL) {
		data = (mrb_allegro4_packfile_data_t*)mrb_malloc(mrb, sizeof(mrb_allegro4_packfile_data_t));
		if (NULL == data) {
			mrb_raise(mrb, E_RUNTIME_ERROR, "no memory.");
			return self;
		}
		mrb_value buffer;
		mrb_get_args(mrb, "S", &buffer);
		struct memory_file_struct* file = mrb_malloc(mrb, sizeof(struct memory_file_struct));
		if (!file) {
			mrb_raise(mrb, E_RUNTIME_ERROR, "memory fopen error.");
			mrb_free(mrb, data);
			return self;
		}
		file->mrb = mrb;
		file->pos = 0;
		file->auto_free = 1;
		file->size = RSTRING_LEN(buffer);
		file->ptr_beg = mrb_malloc(mrb, file->size);
		if (!file->ptr_beg) {
			mrb_raise(mrb, E_RUNTIME_ERROR, "no memory.");
			mrb_free(mrb, data);
			mrb_free(mrb, file);
			return self;
		}
		memcpy(file->ptr_beg, RSTRING_PTR(buffer), file->size);
		data->packfile = pack_fopen_vtable(&memory_file_vtable, (void*)file);
		if (NULL == data->packfile) {
			mrb_raise(mrb, E_RUNTIME_ERROR, "no memory.");
			mrb_free(mrb, data);
			mrb_free(mrb, file);
			return self;
		}
		DATA_PTR(self) = data;
		DATA_TYPE(self) = &mrb_allegro4_packfile_data_type;
	}
	return self;
}

void mrb_mruby_allegro4_file_init(mrb_state* mrb) {
	module_allegro4_file = mrb_define_module_under(mrb, module_allegro4, "PackFile");
	
	// File
	class_allegro4_normal_file = mrb_define_class_under(mrb, module_allegro4_file, "File", mrb->object_class);
	
	MRB_SET_INSTANCE_TT(class_allegro4_normal_file,  MRB_TT_DATA);
	
	mrb_define_method(mrb, class_allegro4_normal_file,		"initialize",		mrb_allegro4_normal_file_initialize,	MRB_ARGS_REQ(2));
	mrb_define_method(mrb, class_allegro4_normal_file,		"getc",					mrb_allegro4_normal_file_getc,				MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_normal_file,		"ungetc",				mrb_allegro4_normal_file_ungetc,			MRB_ARGS_REQ(1));
	mrb_define_method(mrb, class_allegro4_normal_file,		"read",					mrb_allegro4_normal_file_fread,				MRB_ARGS_REQ(1));
	mrb_define_method(mrb, class_allegro4_normal_file,		"putc",					mrb_allegro4_normal_file_putc,				MRB_ARGS_REQ(1));
	mrb_define_method(mrb, class_allegro4_normal_file,		"write",				mrb_allegro4_normal_file_fwrite,			MRB_ARGS_REQ(1));
	mrb_define_method(mrb, class_allegro4_normal_file,		"seek",					mrb_allegro4_normal_file_fseek,				MRB_ARGS_REQ(1));
	mrb_define_method(mrb, class_allegro4_normal_file,		"eof?",					mrb_allegro4_normal_file_feof,				MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_normal_file,		"error",				mrb_allegro4_normal_file_ferror,			MRB_ARGS_NONE());
	
	mrb_define_method(mrb, class_allegro4_normal_file,		"gets",					mrb_allegro4_normal_file_gets,				MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_normal_file,		"puts",					mrb_allegro4_normal_file_puts,				MRB_ARGS_REQ(1));
	
	// Mem File
	class_allegro4_memory_file = mrb_define_class_under(mrb, module_allegro4_file, "MemFile", class_allegro4_normal_file);
	
	MRB_SET_INSTANCE_TT(class_allegro4_memory_file,  MRB_TT_DATA);
	
	mrb_define_method(mrb, class_allegro4_memory_file, "initialize",		mrb_allegro4_memory_file_initialize,	MRB_ARGS_REQ(1));
}
