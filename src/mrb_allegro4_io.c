#include "mrb_allegro4_io.h"
#include "mrb_allegro4_file.h"
#include "mrb_allegro4_sample.h"
#include "mrb_allegro4_bitmap.h"

struct RClass* module_allegro4_io = NULL;

static mrb_value mrb_allegro4_io_load_sample_packfile(mrb_state* mrb, mrb_value self) {
	mrb_value packfile;
	mrb_get_args(mrb, "o", &packfile);
	do {
		PACKFILE* file = mrb_allegro4_packfile_get_ptr(mrb, packfile);
		if (!file) break;
		SAMPLE* sample = load_wav_pf(file);
		if (!sample) break;
		return mrb_allegro4_sample_set(mrb, sample, 1);
	} while(0);
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_io_load_bitmap(mrb_state* mrb, mrb_value self) {
	char* filename = NULL;
	mrb_get_args(mrb, "z", &filename);
	BITMAP* bitmap = load_bitmap(filename, NULL);
	if (!bitmap) return mrb_nil_value();
	return mrb_allegro4_bitmap_set(mrb, bitmap, 1);
}

static mrb_value mrb_allegro4_io_load_bitmap_packfile(mrb_state* mrb, mrb_value self) {
	mrb_value packfile;
	mrb_get_args(mrb, "o", &packfile);
	do {
		PACKFILE* file = mrb_allegro4_packfile_get_ptr(mrb, packfile);
		if (!file) break;
		BITMAP* bitmap = load_bmp_pf(file, NULL);
		if (!bitmap) break;
		return mrb_allegro4_bitmap_set(mrb, bitmap, 1);
	} while(0);
	return mrb_nil_value();
}

// =================================================================================
// External!!!!
// =================================================================================
// ==========================================
// Vorbis
// ==========================================
#include <vorbis/vorbisfile.h>
#include "allegro/internal/aintern.h"

/* XXX requires testing */
#ifdef ALLEGRO_BIG_ENDIAN
	const int ENDIANNESS = 1;
#endif
#ifdef ALLEGRO_LITTLE_ENDIAN
	const int ENDIANNESS = 0;
#endif

static int logg_bufsize = 1024 * 64;

static size_t fread_packfile(void *buffer, size_t size, size_t count, void* packfile) {
	struct PACKFILE_VTABLE* vtable = ((PACKFILE*)packfile)->vtable;
	if (vtable == &normal_file_vtable) {
		FILE* file = (FILE*)(((PACKFILE*)packfile)->userdata);
		return fread(buffer, size, count, file);
	} else if (vtable == &memory_file_vtable){
		struct memory_file_struct* memfile = (struct memory_file_struct*)(((PACKFILE*)packfile)->userdata);
		if (size > (memfile->size - memfile->pos)) {
			return 0;
		}
		
		size_t read_count = 0;
		while ((read_count < count) && (read_count * size < (memfile->size - memfile->pos))) {
			read_count += 1;
		}
		
		memcpy(buffer, memfile->ptr_beg + memfile->pos, read_count * size);
		memfile->pos += read_count * size;
		
		return read_count;
	} else {
		return -1;
	}
}

static int fseek_packfile(void* packfile, ogg_int64_t offset, int whence) {
	struct PACKFILE_VTABLE* vtable = ((PACKFILE*)packfile)->vtable;
	if (vtable == &normal_file_vtable) {
		FILE* file = (FILE*)(((PACKFILE*)packfile)->userdata);
		return fseek(file, offset, whence);
	} else if (vtable == &memory_file_vtable){
		struct memory_file_struct* memfile = (struct memory_file_struct*)(((PACKFILE*)packfile)->userdata);
		int idxEnd = memfile->size - 1;
		switch (whence) {
		case SEEK_SET: {
			if (offset >= 0 && offset <= idxEnd) {
				memfile->pos = offset;
				return 0;
			}
		} break;
		case SEEK_CUR: {
			if ((offset + memfile->pos) >= 0 && (offset + memfile->pos) <= idxEnd) {
				memfile->pos += offset;
				return 0;
			}
		} break;
		case SEEK_END: {
			if ((offset + idxEnd) >= 0 && (offset + idxEnd) <= idxEnd) {
				memfile->pos = idxEnd + offset + 1;
				return 0;
			}
		} break;
		}
		return -1;
	} else {
		return -1;
	}
}

static long ftell_packfile(void* packfile) {
	struct PACKFILE_VTABLE* vtable = ((PACKFILE*)packfile)->vtable;
	if (vtable == &normal_file_vtable) {
		FILE* file = (FILE*)(((PACKFILE*)packfile)->userdata);
		return ftell(file);
	} else if (vtable == &memory_file_vtable){
		struct memory_file_struct* memfile = (struct memory_file_struct*)(((PACKFILE*)packfile)->userdata);
		return memfile->pos;
	} else {
		return -1;
	}
}

static ov_callbacks OV_CALLBACKS_PACKFILE = {
	(size_t (*)(void *, size_t, size_t, void *))  fread_packfile,
	(int (*)(void *, ogg_int64_t, int))		   		fseek_packfile,
	(int (*)(void *))							 						NULL,
	(long (*)(void *))													ftell_packfile
};

static mrb_value mrb_allegro4_io_load_sample_packfile_ogg(mrb_state* mrb, mrb_value self) {
	mrb_value packfile;
	mrb_get_args(mrb, "o", &packfile);
	do {
		PACKFILE* file = mrb_allegro4_packfile_get_ptr(mrb, packfile);
		if (!file) break;
		// =================================================
		// Vorbis Decode
		// =================================================
		OggVorbis_File ovf;
		vorbis_info* vi;
		SAMPLE* samp;
		int numRead;
		long offset = 0;
		int bitstream;
		char *buf = malloc(logg_bufsize);
		if (ov_open_callbacks(file, &ovf, 0, 0, OV_CALLBACKS_PACKFILE) != 0) {
			free(buf);
			break;
		}
		vi = ov_info(&ovf, -1);
		samp = (SAMPLE*)_al_malloc(sizeof(SAMPLE));
		if (!samp) {
			ov_clear(&ovf);
			free(buf);
			break;
		}
		samp->bits = 16;
		samp->stereo = vi->channels > 1 ? 1 : 0;
		samp->freq = vi->rate;
		samp->priority = 128;
		samp->len = ov_pcm_total(&ovf, -1);
		samp->loop_start = 0;
		samp->loop_end = samp->len;
		samp->data = _al_malloc(sizeof(unsigned short) * samp->len * 2);

		while ((numRead = ov_read(&ovf, buf, logg_bufsize,
				ENDIANNESS, 2, 0, &bitstream)) != 0) {
			memcpy((unsigned char*)samp->data + offset, buf, numRead);
			offset += numRead;
		}

		ov_clear(&ovf);
		free(buf);
		return mrb_allegro4_sample_set(mrb, samp, 1);
	} while(0);
	return mrb_nil_value();
}

// ==========================================
// ModPlug
// ==========================================
#include <libmodplug/modplug.h>

static mrb_value mrb_allegro4_io_load_sample_packfile_mod(mrb_state* mrb, mrb_value self) {
	mrb_value packfile;
	mrb_int bits = 16, frequency = 44100, resamplingMode = 0;
	mrb_get_args(mrb, "oiii", &packfile, &bits, &frequency, &resamplingMode);
	do {
		PACKFILE* file = mrb_allegro4_packfile_get_ptr(mrb, packfile);
		if (!file) break;
		// =================================================
		// Modplug Decode
		// =================================================
		unsigned long length = 0;
		pack_fseek(file, 0);
		while (pack_getc(file) != EOF) length += 1;
		pack_fseek(file, 0);
		unsigned char* buffer = (unsigned char*)malloc(length);
		if (!buffer) break;
		pack_fread(buffer, length, file);
		
		// Modplug settings
		ModPlug_Settings settings;
		ModPlug_GetSettings(&settings);
		settings.mBits = bits;
		settings.mFrequency = frequency;
		settings.mResamplingMode = resamplingMode;
		settings.mLoopCount = 0;
		ModPlug_SetSettings(&settings);
		
		ModPlugFile* modplug = ModPlug_Load(buffer, length);
		if (!modplug) {
			free(buffer);
			break;
		}
		SAMPLE* samp = (SAMPLE*)_al_malloc(sizeof(SAMPLE));
		if (!samp) {
			free(buffer);
			ModPlug_Unload(modplug);
			break;
		}
		samp->bits = settings.mBits;
		samp->stereo = settings.mChannels - 1;
		samp->freq = settings.mFrequency;
		samp->priority = 128;
	
		unsigned int offset = 0;
		unsigned int numReads = 0;
		
#define MOD_BUF_SIZE 4096
		
		unsigned char buf[MOD_BUF_SIZE];
		
		ModPlug_Seek(modplug, 0);
		while ((numReads = ModPlug_Read(modplug, buf, MOD_BUF_SIZE)) != 0) {
			offset += numReads;
		}
		
		unsigned int datalen = offset;
		
		samp->len = samp->freq * (ModPlug_GetLength(modplug) / 1000.0);
		samp->data = _al_malloc(datalen);
		
		if (!samp->data) {
			free(buffer);
			ModPlug_Unload(modplug);
			break;
		} 
		
		samp->loop_start = 0;
		samp->loop_end = samp->len;
		
		offset = 0;
		
		ModPlug_Seek(modplug, 0);
		while ((numReads = ModPlug_Read(modplug, buf, MOD_BUF_SIZE)) != 0) {
			memcpy((unsigned char*)samp->data + offset, buf, numReads);
			offset += numReads;
		}
		
		if (samp->bits == 16) {
			unsigned int i = 0;
			unsigned short* data = (unsigned short*)samp->data;
			unsigned int ilen = datalen / sizeof(unsigned short);
			for (;i < ilen; i++) {
				data[i] ^= 0x8000;
			}
		}
		
		free(buffer);
		ModPlug_Unload(modplug);
		return mrb_allegro4_sample_set(mrb, samp, 1);
	} while(0);
	return mrb_nil_value();
}

void mrb_mruby_allegro4_io_init(mrb_state* mrb) {
	module_allegro4_io = mrb_define_module_under(mrb, module_allegro4, "IO");
	
	mrb_define_module_function(mrb, module_allegro4_io,	"load_wav_packfile",			mrb_allegro4_io_load_sample_packfile,				MRB_ARGS_REQ(1));
	
	mrb_define_module_function(mrb, module_allegro4_io,	"load_bitmap", 						mrb_allegro4_io_load_bitmap, 								MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_io,	"load_bmp_packfile",			mrb_allegro4_io_load_bitmap_packfile,				MRB_ARGS_REQ(1));
	
	mrb_define_module_function(mrb, module_allegro4_io, 	"load_ogg_packfile",			mrb_allegro4_io_load_sample_packfile_ogg,		MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_io, 	"load_mod_packfile",			mrb_allegro4_io_load_sample_packfile_mod,		MRB_ARGS_REQ(4));
}
















































