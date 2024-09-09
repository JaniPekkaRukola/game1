#ifndef SAVEGAME_HANDLER_H
#define SAVEGAME_HANDLER_H

// #include <io.h>
// #include <direct.h>
// #include <string.h>
// #include <stdio.h>

bool world_save_to_disk() {
	return os_write_entire_file_s(STR("world"), (string){sizeof(World), (u8*)world});
}

bool world_attempt_load_from_disk() {
	string result = {0};
	bool succ = os_read_entire_file_s(STR("world"), &result, temp_allocator);
	if (!succ) {
		log_error("Failed to load world.");
		return false;
	}

	// NOTE, for errors I used to do stuff like this assert:
	// assert(result.count == sizeof(World), "world size has changed!");
	//
	// But since shipping to users, I've noticed that it's always better to gracefully fail somehow.
	// That's why this function returns a bool. We handle that at the callsite.
	// Maybe we want to just start up a new world, throw a user friendly error, or whatever as a fallback. Not just crash the game lol.

	if (result.count != sizeof(World)) {
		log_error("world size different to one on disk.");
		return false;
	}

	memcpy(world, result.data, result.count);
	return true;
}


/*
    #define BASE_NAME "savegame_"
    void save_game_with_incremented_index(string base_filename) {
        int index = 1;
        bool file_exists = true;
        char indexed_filename_buffer[256];
        string indexed_filename;

        // Keep incrementing index until we find an available slot
        while (file_exists) {
            // Generate the indexed filename (e.g., "savegame_1", "savegame_2", etc.)
            snprintf(indexed_filename_buffer, sizeof(indexed_filename_buffer), "%.*s_%d.sav", base_filename.count, base_filename.data, index);
            indexed_filename = make_string(indexed_filename_buffer);

            // Check if the file already exists
            file_exists = os_is_file_s(indexed_filename);
            
            if (file_exists) {
                index++; // If the file exists, increment the index
            }
        }

        // Now that we have a unique filename, create and write to the file
        File savegame_file = os_file_open_s(indexed_filename, O_CREATE | O_WRITE);

        if (savegame_file != OS_INVALID_FILE) {
            // Write some data (replace this with actual savegame data)
            string save_data = make_string("Save game data goes here...\n");
            os_file_write_string(savegame_file, save_data);

            // Close the file after writing
            os_file_close(savegame_file);
            
            os_write_string_to_stdout(make_string("Savegame created successfully with filename: "));
            os_write_string_to_stdout(indexed_filename);
            os_write_string_to_stdout(make_string("\n"));
        } else {
            os_write_string_to_stdout(make_string("Error creating savegame file\n"));
        }
    }
*/

#endif

