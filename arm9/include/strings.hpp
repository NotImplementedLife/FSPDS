#pragma once

int ui_measure_string(const char* text);

#define declare_string_resource(name) \
namespace Strings {\
	extern const char* str_ ## name; \
	extern const int len_ ## name; \
	extern const int pcx_ ## name; /* pos center x */ \
}
	
declare_string_resource(wipe_data);
declare_string_resource(settings);
declare_string_resource(location_ask_reindex);
declare_string_resource(location_ask_remove);
declare_string_resource(please_wait);
declare_string_resource(browse_flipnotes);
declare_string_resource(indexing_files);
declare_string_resource(found_n_flipnotes);
declare_string_resource(saving);
declare_string_resource(done);
declare_string_resource(no_flipnote_locations);
declare_string_resource(n_flipnotes);
declare_string_resource(pick_a_directory);
declare_string_resource(select);
declare_string_resource(play);
declare_string_resource(err_null_argument);
declare_string_resource(err_fopen);
declare_string_resource(err_size_exceeded);
declare_string_resource(err_read_count);
declare_string_resource(error);
declare_string_resource(sound_size_too_large);
declare_string_resource(sound_buffer_not_allocated);
declare_string_resource(sfx_buffer_not_allocated);
