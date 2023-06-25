#include "strings.hpp"

#include "DSC"

int ui_measure_string(const char* text)
{
	int result=0;
	for(;*text;++text)		
		result+=DSC::Resources::Fonts::default_8x16.get_glyph_width(*text);
	return result;
}

#define define_string_resource(name, value) \
	const char* Strings::str_ ## name = value; \
	const int Strings::len_ ## name = ui_measure_string(value); \
	const int Strings::pcx_ ## name = (256-len_ ## name)/2; /* pos center x */

define_string_resource(wipe_data, "Wipe data");
define_string_resource(settings, "Settings");
define_string_resource(location_ask_reindex, "Do you want to reindex the location?");
define_string_resource(location_ask_remove, "Do you want to remove the location?");
define_string_resource(please_wait, "Please wait");
define_string_resource(browse_flipnotes, "Browse flipnotes");
define_string_resource(indexing_files, "Indexing files");
define_string_resource(found_n_flipnotes, "Found %i flipnotes");
define_string_resource(saving, "Saving");
define_string_resource(done, "Done");
define_string_resource(no_flipnote_locations, "No flipnote locations. Add one!");
define_string_resource(n_flipnotes, "%i flipnotes");
define_string_resource(pick_a_directory, "Pick a directory");
define_string_resource(select, "Select");
define_string_resource(play, "Play");
define_string_resource(err_null_argument, "Internal error (ERR_NULL_ARGUMENT)");
define_string_resource(err_fopen, "Could not open the file (ERR_FOPEN)");
define_string_resource(err_size_exceeded, "File too large (ERR_SIZE_EXCEEDED)");
define_string_resource(err_read_count, "File reading error (ERR_READ_COUNT)");
define_string_resource(error, "Error");
define_string_resource(sound_size_too_large, "Sound size too large");
define_string_resource(sound_buffer_not_allocated, "Sound buffer not allocated");
define_string_resource(sfx_buffer_not_allocated, "Sfx buffer not allocated");