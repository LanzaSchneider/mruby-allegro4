MRuby::Gem::Specification.new('mruby-allegro4') do |spec|
	spec.license = 'None'
	spec.authors = 'Lanza Artist Schneider'
	spec.summary = 'Allegro 4 library binding'
	
	spec.cc.defines << 'ALLEGRO_STATICLINK'
	spec.linker.libraries << 'alleg'
	spec.cc.defines << 'MODPLUG_STATIC'
	spec.linker.libraries << 'modplug'
	
	spec.linker.libraries << %w(vorbisfile vorbis ogg)
	spec.linker.libraries << %w(kernel32 user32 gdi32 comdlg32 ole32)
	spec.linker.libraries << %w(dinput ddraw dxguid dsound winmm winspool shell32 oleaut32 uuid advapi32)
end
