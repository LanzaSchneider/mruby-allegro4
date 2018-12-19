#include "mrb_allegro4_sample.h"
#include <mruby/data.h>
#include <mruby/class.h>

typedef struct mrb_allegro4_sample_data_t {
	SAMPLE* sample;
	int need_dispose;
} mrb_allegro4_sample_data_t;

static void mrb_allegro4_sample_data_free(mrb_state *mrb, void *p) {
	mrb_allegro4_sample_data_t* data = (mrb_allegro4_sample_data_t*)p;
	if (NULL != data) {
		if (data->sample && data->need_dispose) {
			destroy_sample(data->sample);
		}
		mrb_free(mrb, data);
	}
}

static struct mrb_data_type const mrb_allegro4_sample_data_type = {
	"allegro4/sample", mrb_allegro4_sample_data_free
};

SAMPLE* mrb_allegro4_sample_get_ptr(mrb_state *mrb, mrb_value sample) {
	mrb_allegro4_sample_data_t *data;
	if (mrb_nil_p(sample)) return NULL;
	data = (mrb_allegro4_sample_data_t*)mrb_data_get_ptr(mrb, sample, &mrb_allegro4_sample_data_type);
	return data->sample;
}

mrb_value mrb_allegro4_sample_set(mrb_state* mrb, SAMPLE* sample, mrb_bool need_dispose) {
	mrb_allegro4_sample_data_t *data = (mrb_allegro4_sample_data_t*)mrb_malloc(mrb, sizeof(mrb_allegro4_sample_data_t));
	if (NULL == data) return mrb_nil_value();
	data->sample = sample;
	data->need_dispose = need_dispose;
	return mrb_obj_value(Data_Wrap_Struct(mrb, class_allegro4_sample, &mrb_allegro4_sample_data_type, data));
}

// =======================================================================
// Sample Object
// =======================================================================

struct RClass* class_allegro4_sample = NULL;

static mrb_value mrb_allegro4_sample_initialize(mrb_state* mrb, mrb_value self) {
	mrb_allegro4_sample_data_t* data = (mrb_allegro4_sample_data_t*)DATA_PTR(self);
	if (data == NULL) {
		data = (mrb_allegro4_sample_data_t*)mrb_malloc(mrb, sizeof(mrb_allegro4_sample_data_t));
		if (NULL == data) {
			mrb_raise(mrb, E_RUNTIME_ERROR, "no memory.");
			return self;
		}
		mrb_int bits, freq, len;
		mrb_bool stereo;
		mrb_get_args(mrb, "ibii", &bits, &stereo, &freq, &len);
		data->sample = create_sample(bits, stereo, freq, len);
		data->need_dispose = 1;
		if (NULL == data->sample) {
			mrb_raise(mrb, E_RUNTIME_ERROR, "no memory.");
			return self;
		}
		DATA_PTR(self) = data;
		DATA_TYPE(self) = &mrb_allegro4_sample_data_type;
	}
	return self;
}

static mrb_value mrb_allegro4_sample_play(mrb_state* mrb, mrb_value self) {
	SAMPLE* sample = mrb_allegro4_sample_get_ptr(mrb, self);
	mrb_int vol, pan, freq;
	mrb_bool loop;
	mrb_get_args(mrb, "iiib", &vol, &pan, &freq, &loop);
	return mrb_fixnum_value(play_sample(sample, vol, pan, freq, loop));
}

static mrb_value mrb_allegro4_sample_stop(mrb_state* mrb, mrb_value self) {
	SAMPLE* sample = mrb_allegro4_sample_get_ptr(mrb, self);
	stop_sample(sample);
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_sample_adjust(mrb_state* mrb, mrb_value self) {
	SAMPLE* sample = mrb_allegro4_sample_get_ptr(mrb, self);
	mrb_int vol, pan, freq;
	mrb_bool loop;
	mrb_get_args(mrb, "iiib", &vol, &pan, &freq, &loop);
	adjust_sample(sample, vol, pan, freq, loop);
	return mrb_nil_value();
}

// =======================================================================
// Parameters
// =======================================================================

static mrb_value mrb_allegro4_sample_bits(mrb_state* mrb, mrb_value self) {
	SAMPLE* sample = mrb_allegro4_sample_get_ptr(mrb, self);
	return mrb_fixnum_value(sample->bits);
}

static mrb_value mrb_allegro4_sample_stereo(mrb_state* mrb, mrb_value self) {
	SAMPLE* sample = mrb_allegro4_sample_get_ptr(mrb, self);
	return mrb_bool_value(sample->stereo);
}

static mrb_value mrb_allegro4_sample_freq(mrb_state* mrb, mrb_value self) {
	SAMPLE* sample = mrb_allegro4_sample_get_ptr(mrb, self);
	return mrb_fixnum_value(sample->freq);
}

static mrb_value mrb_allegro4_sample_length(mrb_state* mrb, mrb_value self) {
	SAMPLE* sample = mrb_allegro4_sample_get_ptr(mrb, self);
	return mrb_fixnum_value(sample->len);
}

void mrb_mruby_allegro4_sample_init(mrb_state* mrb) {
	class_allegro4_sample = mrb_define_class_under(mrb, module_allegro4, "Sample", mrb->object_class);
	
	MRB_SET_INSTANCE_TT(class_allegro4_sample,  MRB_TT_DATA);
	
	mrb_define_method(mrb, class_allegro4_sample, "initialize",			mrb_allegro4_sample_initialize,					MRB_ARGS_REQ(4));
	mrb_define_method(mrb, class_allegro4_sample, "play",				mrb_allegro4_sample_play,						MRB_ARGS_REQ(4));
	mrb_define_method(mrb, class_allegro4_sample, "stop",				mrb_allegro4_sample_stop,						MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_sample, "adjust",				mrb_allegro4_sample_adjust,						MRB_ARGS_REQ(4));
	
	mrb_define_method(mrb, class_allegro4_sample, "bits",				mrb_allegro4_sample_bits,						MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_sample, "stereo",				mrb_allegro4_sample_stereo,						MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_sample, "freq",				mrb_allegro4_sample_freq,						MRB_ARGS_NONE());
	mrb_define_method(mrb, class_allegro4_sample, "length",				mrb_allegro4_sample_length,						MRB_ARGS_NONE());
}

















































