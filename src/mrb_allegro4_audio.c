#include "mrb_allegro4_audio.h"

struct RClass* module_allegro4_audio = NULL;

static mrb_value mrb_allegro4_audio_detect_digi_driver(mrb_state* mrb, mrb_value self) {
	mrb_int driver_id;
	mrb_get_args(mrb, "i", &driver_id);
	return mrb_fixnum_value(detect_digi_driver(driver_id));
}

static mrb_value mrb_allegro4_audio_detect_midi_driver(mrb_state* mrb, mrb_value self) {
	mrb_int driver_id;
	mrb_get_args(mrb, "i", &driver_id);
	return mrb_fixnum_value(detect_midi_driver(driver_id));
}

// static mrb_value mrb_allegro4_audio_get_hardware_volume(mrb_state* mrb, mrb_value self) {
	
// }

static mrb_value mrb_allegro4_audio_digi_volume(mrb_state* mrb, mrb_value self) {
	mrb_int digi_volume, midi_volume;
	get_volume(&digi_volume, &midi_volume);
	return mrb_fixnum_value(digi_volume);
}

static mrb_value mrb_allegro4_audio_digi_volume_set(mrb_state* mrb, mrb_value self) {
	mrb_int digi_volume, midi_volume;
	get_volume(&digi_volume, &midi_volume);
	mrb_get_args(mrb, "i", &digi_volume);
	set_volume(digi_volume, midi_volume);
	return mrb_fixnum_value(digi_volume);
}

static mrb_value mrb_allegro4_audio_midi_volume(mrb_state* mrb, mrb_value self) {
	mrb_int digi_volume, midi_volume;
	get_volume(&digi_volume, &midi_volume);
	return mrb_fixnum_value(midi_volume);
}

static mrb_value mrb_allegro4_audio_midi_volume_set(mrb_state* mrb, mrb_value self) {
	mrb_int digi_volume, midi_volume;
	get_volume(&digi_volume, &midi_volume);
	mrb_get_args(mrb, "i", &midi_volume);
	set_volume(digi_volume, midi_volume);
	return mrb_fixnum_value(midi_volume);
}

static mrb_value mrb_allegro4_audio_install(mrb_state* mrb, mrb_value self) {
	mrb_int digi, midi;
	mrb_get_args(mrb, "ii", &digi, &midi);
	return mrb_fixnum_value(install_sound(digi, midi, NULL));
}

static mrb_value mrb_allegro4_audio_remove(mrb_state* mrb, mrb_value self) {
	remove_sound();
	return mrb_nil_value();
}

static mrb_value mrb_allegro4_audio_reserve_voices(mrb_state* mrb, mrb_value self) {
	mrb_int digi_voices, midi_voices;
	mrb_get_args(mrb, "ii", &digi_voices, &midi_voices);
	reserve_voices(digi_voices, midi_voices);
	return mrb_nil_value();
}

// static mrb_value mrb_allegro4_audio_set_hardware_volume(mrb_state* mrb, mrb_value self) {
	
// }

// static mrb_value mrb_allegro4_audio_set_volume_per_voice(mrb_state* mrb, mrb_value self) {
	
// }

void mrb_mruby_allegro4_audio_init(mrb_state* mrb) {
	module_allegro4_audio = mrb_define_module_under(mrb, module_allegro4, "Audio");
	
	mrb_define_module_function(mrb, module_allegro4_audio, 	"install", 															mrb_allegro4_audio_install, 									MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, module_allegro4_audio, 	"remove", 																mrb_allegro4_audio_remove, 									MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_audio, 	"mrb_allegro4_audio_reserve_voices", 		mrb_allegro4_audio_reserve_voices, 					MRB_ARGS_REQ(2));
	
	mrb_define_module_function(mrb, module_allegro4_audio, 	"detect_digi_driver", 		mrb_allegro4_audio_detect_digi_driver, 			MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_audio, 	"detect_midi_driver", 		mrb_allegro4_audio_detect_midi_driver, 			MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_audio, 	"digi_volume=", 					mrb_allegro4_audio_digi_volume_set, 					MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_audio, 	"digi_volume", 					mrb_allegro4_audio_digi_volume, 							MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module_allegro4_audio, 	"midi_volume=", 					mrb_allegro4_audio_midi_volume_set, 					MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module_allegro4_audio, 	"midi_volume", 					mrb_allegro4_audio_midi_volume, 							MRB_ARGS_NONE());
	
	mrb_define_const(mrb, module_allegro4_audio, "DIGI_AUTODETECT",         		mrb_fixnum_value(DIGI_AUTODETECT));
	mrb_define_const(mrb, module_allegro4_audio, "DIGI_NONE",         					mrb_fixnum_value(DIGI_NONE));
	
	mrb_define_const(mrb, module_allegro4_audio, "MIDI_AUTODETECT",         		mrb_fixnum_value(MIDI_AUTODETECT));
	mrb_define_const(mrb, module_allegro4_audio, "MIDI_NONE",         					mrb_fixnum_value(MIDI_NONE));
}
