#ifndef CHUNKS_H
#define CHUNKS_H

// #define CHUNK_SIZE 32
#define MAX_CHUNKS 100
#define MAX_LOADED_CHUNKS 9
#define CHUNK_RENDER_DISTANCE 1
#define MAX_CHUNK_ENTITIES 128

typedef struct Chunk {
    // Vector2 size; // this might be useless since "#define CHUNK_SIZE"
    BiomeID* tiles;
    Vector2 pos;
    Entity* entities[MAX_CHUNK_ENTITIES];
    int entity_count;
    bool has_been_loaded; // true if chunk has been loaded before
    BiomeID biome_id;
} Chunk;


// Generate entities in a chunk based on its biome
void generate_chunk_entities(Chunk* chunk) {
    int amount_of_rocks = 10;  // Example number of rocks
    float spawn_range = 20.0f;  // Example range

    // Loop to generate entities based on the chunk's biome (e.g., trees in forest, rocks in rocky biome)
    for (int i = 0; i < amount_of_rocks && chunk->entity_count < MAX_CHUNK_ENTITIES; i++) {
        Entity* en = entity_create();
        setup_rock(en);
        en->pos = v2(get_random_float32_in_range(-spawn_range, spawn_range),
                     get_random_float32_in_range(-spawn_range, spawn_range));
        en->pos = round_v2_to_tile(en->pos);

        chunk->entities[chunk->entity_count++] = en;
    }
}


Chunk* create_chunk(int chunk_x, int chunk_y, WorldData* world_data) {
    Chunk* chunk = alloc(get_heap_allocator(), sizeof(Chunk));
    assert(chunk != NULL);

    chunk->entity_count = 0;
    chunk->has_been_loaded = false;  // First time loading this chunk
    chunk->pos = v2(chunk_x, chunk_y);

    // test
    chunk->biome_id = BIOME_forest;

    // Allocate memory for the tiles
    // chunk->tiles = alloc(get_heap_allocator(), CHUNK_SIZE * CHUNK_SIZE * sizeof(BiomeID));
    // assert(chunk->tiles != NULL);

    // // Generate or load biome tiles for this chunk
    // for (int y = 0; y < CHUNK_SIZE; y++) {
    //     for (int x = 0; x < CHUNK_SIZE; x++) {
    //         int world_x = chunk_x * CHUNK_SIZE + x;
    //         int world_y = chunk_y * CHUNK_SIZE + y;

    //         // Ensure that world_x and world_y are within bounds
    //         if (world_x >= 0 && world_x < world_data->width && world_y >= 0 && world_y < world_data->height) {
    //             // Retrieve the biome at the global world position (using the world_data tiles)
    //             chunk->tiles[y * CHUNK_SIZE + x] = world_data->tiles[world_y * world_data->width + world_x];
    //         } else {
    //             // If out of bounds, default to a biome, e.g., BIOME_nil or some other fallback
    //             chunk->tiles[y * CHUNK_SIZE + x] = BIOME_nil; // Fallback biome if out of bounds
    //         }
    //     }
    // }

    // Populate the chunk with entities (if this is the first time it's loaded)
    if (!chunk->has_been_loaded) {
        // generate_chunk_entities(chunk);
        chunk->has_been_loaded = true;
    }

    // printf("Created a chunk\n");
    world->dimension->chunk_count++;
    return chunk;
}


void save_chunk(Chunk* chunk) {
    // Save chunk data (e.g., entities and tile state)
    // This could involve writing to a file or an in-memory structure
}

void free_chunk(Chunk* chunk) {
    // Free the chunk's memory
    // free(chunk->tiles);
    // dealloc(get_heap_allocator(), chunk->tiles);
    for (int i = 0; i < chunk->entity_count; i++) {
        // free(chunk->entities[i]);
        dealloc(get_heap_allocator(), chunk->entities[i]);
    }
    // free(chunk);
    dealloc(get_heap_allocator(), chunk);
    world->dimension->chunk_count--;
    // printf("Deleted chunk\n");
}

#define CHUNK_OFFSET (MAX_CHUNKS / 2)  // Offset to handle negative chunk indices
void load_chunks_around_player(World* world) {
    Vector2 player_pos = get_player_pos();

    // Determine which chunks are within the player's view distance
    int start_chunk_x = player_pos.x / CHUNK_SIZE - CHUNK_RENDER_DISTANCE;
    int end_chunk_x = player_pos.x / CHUNK_SIZE + CHUNK_RENDER_DISTANCE;
    int start_chunk_y = player_pos.y / CHUNK_SIZE - CHUNK_RENDER_DISTANCE;
    int end_chunk_y = player_pos.y / CHUNK_SIZE + CHUNK_RENDER_DISTANCE;

    // printf("%d  %d  %d  %d  \n", start_chunk_x, end_chunk_x, start_chunk_y, end_chunk_y);

    // Iterate over the chunks within the render distance
    for (int chunk_x = start_chunk_x; chunk_x <= end_chunk_x; chunk_x++) {
        for (int chunk_y = start_chunk_y; chunk_y <= end_chunk_y; chunk_y++) {
            // Apply CHUNK_OFFSET to handle negative indices
            int chunk_x_offset = chunk_x + CHUNK_OFFSET;
            int chunk_y_offset = chunk_y + CHUNK_OFFSET;

            // printf("Chunk offsets xy = %d  %d\n", chunk_x_offset, chunk_y_offset);

            // Check bounds to avoid out-of-bounds errors
            if (chunk_x_offset >= 0 && chunk_x_offset < MAX_CHUNKS && chunk_y_offset >= 0 && chunk_y_offset < MAX_CHUNKS) {
                if (world->dimension->chunks[chunk_x_offset] == NULL) {
                    // Allocate the row if it's NULL
                    world->dimension->chunks[chunk_x_offset] = alloc(get_heap_allocator(), MAX_CHUNKS * sizeof(Chunk*));
                    memset(world->dimension->chunks[chunk_x_offset], 0, MAX_CHUNKS * sizeof(Chunk*));
                }

                if (world->dimension->chunks[chunk_x_offset][chunk_y_offset] == NULL) {
                    // Allocate the chunk if it is NULL
                    world->dimension->chunks[chunk_x_offset][chunk_y_offset] = create_chunk(chunk_x, chunk_y, &world->dimension->map);
                }
            }
        }
    }
}



// void load_chunks_around_player(World* world) {
//     Vector2 player_pos = get_player_pos();

//     // Determine which chunks are within the player's view distance
//     int start_chunk_x = player_pos.x / CHUNK_SIZE - CHUNK_RENDER_DISTANCE;
//     int end_chunk_x = player_pos.x / CHUNK_SIZE + CHUNK_RENDER_DISTANCE;
//     int start_chunk_y = player_pos.y / CHUNK_SIZE - CHUNK_RENDER_DISTANCE;
//     int end_chunk_y = player_pos.y / CHUNK_SIZE + CHUNK_RENDER_DISTANCE;

//     // Clamp the chunk bounds to avoid out-of-bounds errors
//     if (start_chunk_x < 0) start_chunk_x = 0;
//     if (end_chunk_x >= MAX_CHUNKS) end_chunk_x = MAX_CHUNKS - 1;
//     if (start_chunk_y < 0) start_chunk_y = 0;
//     if (end_chunk_y >= MAX_CHUNKS) end_chunk_y = MAX_CHUNKS - 1;

//     // Iterate over the chunks within the render distance
//     for (int chunk_x = start_chunk_x; chunk_x <= end_chunk_x; chunk_x++) {
//         for (int chunk_y = start_chunk_y; chunk_y <= end_chunk_y; chunk_y++) {
//             if (world->dimension->chunks == NULL) {
//                 printf("Error: Chunks array is NULL!\n");
//             }
//             else if (world->dimension->chunks[chunk_x] == NULL) {
//                 // Allocate the row if it's NULL
//                 world->dimension->chunks[chunk_x] = alloc(get_heap_allocator(), MAX_CHUNKS * sizeof(Chunk*));
//                 memset(world->dimension->chunks[chunk_x], 0, MAX_CHUNKS * sizeof(Chunk*));
//             }

//             if (world->dimension->chunks[chunk_x][chunk_y] == NULL) {
//                 // Allocate the chunk if it is NULL
//                 world->dimension->chunks[chunk_x][chunk_y] = create_chunk(chunk_x, chunk_y, &world->dimension->map);
//             }

//             // printf("chunk[%d][%d] pos in world = %.0f, %.0f\n", chunk_x, chunk_y, world->dimension->chunks[chunk_x][chunk_y]->pos);
//             // printf("Chunk xy = %d, %d\n", chunk_x * CHUNK_SIZE, chunk_y * CHUNK_SIZE);
//             // printf("Chunk xy = %d, %d\n", chunk_x, chunk_y);

//             // Matrix4 xform = m4_identity;
//             // xform = m4_translate(xform, v3(chunk_x * 10, chunk_y * 10, 0));
//             // xform = m4_translate(xform, v3(0, +250, 0));
//             // draw_rect_with_border(xform, v2(10, 10), 2, COLOR_RED, COLOR_BLACK);
//         }
//     }

//     // old
//         // // Get the total number of chunks in the world (calculated from world dimensions)
//         // int chunk_grid_width = world->dimension->map.width / CHUNK_SIZE;
//         // int chunk_grid_height = world->dimension->map.height / CHUNK_SIZE;

//         // // Iterate over the chunks within the render distance
//         // for (int chunk_x = start_chunk_x; chunk_x <= end_chunk_x; chunk_x++) {
//         //     for (int chunk_y = start_chunk_y; chunk_y <= end_chunk_y; chunk_y++) {
//         //         // Check if chunk indices are within bounds of the world's chunk grid
//         //         if (chunk_x >= 0 && chunk_x < chunk_grid_width && chunk_y >= 0 && chunk_y < chunk_grid_height) {
//         //             if (world->dimension->chunks == NULL) {
//         //                 printf("Error: Chunks array is NULL!\n");
//         //             } else if (world->dimension->chunks[chunk_x] == NULL) {
//         //                 // Allocate the row if it's NULL
//         //                 // world->dimension->chunks[chunk_x] = malloc(chunk_grid_height * sizeof(Chunk*));
//         //                 world->dimension->chunks[chunk_x] = alloc(get_heap_allocator(), chunk_grid_height * sizeof(Chunk*));
//         //                 memset(world->dimension->chunks[chunk_x], 0, chunk_grid_height * sizeof(Chunk*));
//         //             }

//         //             if (world->dimension->chunks[chunk_x][chunk_y] == NULL) {
//         //                 // Allocate the chunk if it is NULL
//         //                 world->dimension->chunks[chunk_x][chunk_y] = create_chunk(chunk_x, chunk_y, &world->dimension->map);
//         //             }
//         //         } else {
//         //             printf("Error: Chunk indices out of bounds! chunk_x = %d, chunk_y = %d\n", chunk_x, chunk_y);
//         //         }
//         //     }
//         // }
//     // 
// }

// NOTE: fuck all this shit. sorry i mean fuck all this fuckshit
/*
    void load_chunks_around_player(World* world) {

        Vector2 player_pos = get_player_pos();

        // Determine which chunks are within the player's view distance
        int start_chunk_x = player_pos.x / CHUNK_SIZE - CHUNK_RENDER_DISTANCE;
        int end_chunk_x = player_pos.x / CHUNK_SIZE + CHUNK_RENDER_DISTANCE;
        int start_chunk_y = player_pos.y / CHUNK_SIZE - CHUNK_RENDER_DISTANCE;
        int end_chunk_y = player_pos.y / CHUNK_SIZE + CHUNK_RENDER_DISTANCE;

        for (int chunk_x = start_chunk_x; chunk_x <= end_chunk_x; chunk_x++) {
            for (int chunk_y = start_chunk_y; chunk_y <= end_chunk_y; chunk_y++) {
                if (chunk_x >= 0 && chunk_x < CHUNK_SIZE && chunk_y >= 0 && chunk_y < CHUNK_SIZE) {
                    if (world->dimension->chunks == NULL) {
                        printf("Error: Chunks array is NULL!\n");
                    } else if (world->dimension->chunks[chunk_x] == NULL) {
                        printf("Error: Row %d is NULL!\n", chunk_x);
                    } else if (world->dimension->chunks[chunk_x][chunk_y] == NULL) {
                        // Allocate the chunk if it is NULL
                        world->dimension->chunks[chunk_x][chunk_y] = create_chunk(chunk_x, chunk_y, &world->dimension->map);
                    }
                } else {
                    printf("Error: Chunk indices out of bounds! chunk_x = %d, chunk_y = %d\n", chunk_x, chunk_y);
                }
            }
        }

        // Load or create chunks within this range
        // for (int chunk_x = start_chunk_x; chunk_x <= end_chunk_x; chunk_x++) {
        //     for (int chunk_y = start_chunk_y; chunk_y <= end_chunk_y; chunk_y++) {
        //         if (world->dimension->chunks[chunk_x][chunk_y] == NULL) {
        //             // Create a new chunk if it doesn't exist
        //             // world->dimension->chunks[chunk_x][chunk_y] = create_chunk(chunk_x, chunk_y, &map, CHUNK_SIZE, CHUNK_SIZE);
        //             world->dimension->chunks[chunk_x][chunk_y] = create_chunk(chunk_x, chunk_y, &world->dimension->map);
        //         }
        //     }
        // }
}*/

void unload_chunks_outside_view() {

    // int MAX_CHUNKS_X = world->dimension->map.width;
    // int MAX_CHUNKS_Y = world->dimension->map.height;
    int MAX_CHUNKS_X = 100;
    int MAX_CHUNKS_Y = 100;

    Vector2 player_pos = get_player_pos();

    // Iterate over all chunks and check if they are outside the view distance
    for (int chunk_x = 0; chunk_x < MAX_CHUNKS_X; chunk_x++) {
        for (int chunk_y = 0; chunk_y < MAX_CHUNKS_Y; chunk_y++) {

            if (world->dimension->chunks[chunk_x][chunk_y] != NULL) {
                int dist_x = fabsf(chunk_x * CHUNK_SIZE - player_pos.x);
                int dist_y = fabsf(chunk_y * CHUNK_SIZE - player_pos.y);

                if (dist_x > CHUNK_RENDER_DISTANCE * CHUNK_SIZE || dist_y > CHUNK_RENDER_DISTANCE * CHUNK_SIZE) {
                    // Save the chunk state (e.g., entity state) before unloading
                    save_chunk(world->dimension->chunks[chunk_x][chunk_y]);

                    // Free the chunk memory
                    free_chunk(world->dimension->chunks[chunk_x][chunk_y]);
                    world->dimension->chunks[chunk_x][chunk_y] = NULL;
                }
            }
        }
    }
}


void update_active_chunks(World* world) {
    int MAX_CHUNKS_X = world->dimension->map.width;
    int MAX_CHUNKS_Y = world->dimension->map.height;
    
    for (int chunk_x = 0; chunk_x < MAX_CHUNKS_X; chunk_x++) {
        for (int chunk_y = 0; chunk_y < MAX_CHUNKS_Y; chunk_y++) {
            Chunk* chunk = world->dimension->chunks[chunk_x][chunk_y];
            if (chunk != NULL) {
                // Update each entity in the chunk
                for (int i = 0; i < chunk->entity_count; i++) {
                    // update_entity(chunk->entities[i]);
                }
            }
        }
    }
}

void show_active_chunks() {
    Vector2 player_pos = get_player_pos();

    int start_chunk_x = player_pos.x / CHUNK_SIZE - CHUNK_RENDER_DISTANCE;
    int end_chunk_x = player_pos.x / CHUNK_SIZE + CHUNK_RENDER_DISTANCE;
    int start_chunk_y = player_pos.y / CHUNK_SIZE - CHUNK_RENDER_DISTANCE;
    int end_chunk_y = player_pos.y / CHUNK_SIZE + CHUNK_RENDER_DISTANCE;

    // Clamp the chunk bounds
    if (start_chunk_x < 0) start_chunk_x = 0;
    if (end_chunk_x >= MAX_CHUNKS) end_chunk_x = MAX_CHUNKS - 1;
    if (start_chunk_y < 0) start_chunk_y = 0;
    if (end_chunk_y >= MAX_CHUNKS) end_chunk_y = MAX_CHUNKS - 1;

    // Iterate over the visible chunks
    for (int chunk_x = start_chunk_x; chunk_x <= end_chunk_x; chunk_x++) {
        for (int chunk_y = start_chunk_y; chunk_y <= end_chunk_y; chunk_y++) {
            // Check if the chunk is active
            if (world->dimension->chunks[chunk_x] != NULL && world->dimension->chunks[chunk_x][chunk_y] != NULL) {
                // Calculate the chunk's world position

                // printf("Chunk pos = %.0f, %.0f\n", world->dimension->chunks[chunk_x][chunk_y]->pos.x, world->dimension->chunks[chunk_x][chunk_y]->pos.y);

                float chunk_world_x = chunk_x * CHUNK_SIZE;
                float chunk_world_y = chunk_y * CHUNK_SIZE;

                // Set up a rectangle's dimensions
                // Vector2 rect_pos = { chunk_world_x, chunk_world_y };
                // Vector2 rect_size = { CHUNK_SIZE, CHUNK_SIZE };
                Matrix4 xform = m4_identity;
                
                // Transform the rect into screen coordinates (adjust based on your camera system)
                // Matrix3x2 transform = calculate_screen_transform(rect_pos);
                xform = m4_translate(xform, v3(chunk_world_x, chunk_world_y, 0));

                // Render the red rectangle for the active chunk
                // draw_rect_xform(rect_pos, rect_size, transform, v4(255, 0, 0, 128)); // Red with some transparency
                // printf("chunk_world_x = %d\n", chunk_world_x);
                // set_world_space();
                draw_rect_xform(xform, v2(CHUNK_SIZE, CHUNK_SIZE), COLOR_RED);
            }
        }
    }
}

void do_chunk_magic() {
    load_chunks_around_player(world);

    // show_active_chunks();


    unload_chunks_outside_view();

    // update_active_chunks(world);


    // printf("Chunk count = %d\n", world->dimension->chunk_count);
}






// stuff here (old)
/*
    typedef struct Chunk {
        Vector2 pos;
        Vector2 slot; // position in map
        Vector2 size; // ????
        BiomeID biome_id;
        // DimensionID dimension_id; // ???
        Entity* entities[MAX_CHUNK_ENTITIES];
        int entity_count;
        bool has_been_loaded; // true if chunk has been loaded before

    } Chunk;




    // Chunk* chunks[MAX_CHUNKS];
    // Chunk loaded_chunks[MAX_LOADED_CHUNKS];

    // void init_chunks(){
    //     int index = 0;
    //     for(int x = 0; x < map.width; x++){
    //         for (int y = 0; y < map.height; y++){
    //             chunks[index]->biome_id = BIOME_nil;
    //             chunks[index]->slot = v2(x, y);
    //             chunks[index]->size = v2(CHUNK_SIZE, CHUNK_SIZE);
    //             // chunks[index].entities = NULL;
    //         }
    //     }
    // }


    // void init_chunks_for_current_dim(){
    //     // NOTE: if i want to init chunks for a different dim, i have to make a pointer to the chunks for that dim somehow: // Chunk* chunks = &world->dimension->chunks;

    //     int index = 0;
    //     for(int x = 0; x < map.width; x++){
    //         for (int y = 0; y < map.height; y++){
    //             world->dimension->chunks[index] = alloc(get_heap_allocator(), sizeof(Chunk));
    //             world->dimension->chunks[index]->biome_id = BIOME_nil;
    //             world->dimension->chunks[index]->slot = v2(x, y);
    //             world->dimension->chunks[index]->size = v2(CHUNK_SIZE, CHUNK_SIZE);
    //             // chunks[index].entities = NULL;
    //             index++;
    //             // if (index >= 100) break; break;
    //             if (index >= 100) return;
    //         }
    //         // print("index = %d\t x = %d\n", index, x);
    //     }
    //     print("INITED %d CHUNKS\n", index);
    // }

    // Chunk* get_chunk(Vector2 pos){
    //     for (int i = 0; i < MAX_CHUNKS; i++){
    //         Chunk* chunk = chunks[i];
    //         Range2f chunk_range;
    //         chunk_range.min = chunk->pos;
    //         chunk_range.max = chunk->pos;
    //         chunk_range.max.x += chunk->size.x;
    //         chunk_range.max.y += chunk->size.y;

    //         if (range2f_contains(chunk_range, pos)){
    //             return chunk;
    //         }
    //     }
    //     assert(1==0, "Failed to get chunk @ 'get_chunk'")
    // }

    // Chunk* get_chunk_from_dim(DimensionID id, Vector2 pos){
    //     // this is disgusting. so scuffed

    //     bool for_else = false;
    //     Chunk* (*chunks)[100] = NULL;
    //     for (int j = 0; j < DIM_MAX; j++){
    //         if (dimensions[j].id == id){
    //             Chunk* (*chunks)[100] = &dimensions[j].chunks;
    //             for_else = true;
    //         }
    //     }

    //     if (for_else == false) return 0;
        
    //     for (int i = 0; i < MAX_CHUNKS; i++){
    //         Chunk* chunk = *chunks[i];
    //         Range2f chunk_range;
    //         chunk_range.min = chunk->pos;
    //         chunk_range.max = chunk->pos;
    //         chunk_range.max.x += chunk->size.x;
    //         chunk_range.max.y += chunk->size.y;

    //         if (range2f_contains(chunk_range, pos)){
    //             return chunk;
    //         }
    //     }

    //     assert(1==0, "Failed to get chunk @ 'get_chunk'")
    // }

    void load_chunk(Vector2 pos){
        //  ________________________________________________________________________________________________________________________________________
        // | LOGIC:
        // | - get world pos (as parameter?)
        // | - calculate the chunk pos in MAP??
        // | - add the chunk to loaded chunks list ( add the pointer?)
        // | - 
        // |________________________________________________________________________________________________________________________________________
    }

    void load_chunks(){
        Vector2 player_pos = get_player_pos();
        // load chunks around the player. call load_chunk func from here

    }

    void unload_chunks(){
        Vector2 player_pos = get_player_pos();
        //  ________________________________________________________________________________________________________________________________________
        // | LOGIC:
        // | - check chunks around player. if chunks are found that are not around the player, remove them from loaded chunks list
        // | - 
        // |________________________________________________________________________________________________________________________________________
    }

    void spawn_from_chunk(chunk identifier or sum here??!){
        // Chunk* chunk = world->dimension->chunks[i]
        // Chunk* chunk;
        // BiomeData biome_data = get_biome_data_from_id(chunk->biome_id);
        // biome_data.
    }

    // entity renderer based on chunks test
    #define ACTIVE_CHUNKS 9
    void render_entities_from_loaded_chunks(){
        for (int i = 0; i < ACTIVE_CHUNKS; i++){
            Chunk chunk = loaded_chunks[i];
            for (int j = 0; j < chunk.entity_count; j++){
                Entity* en = chunk.entities[i];

                if (en->is_valid){

                    switch (en->arch){

                        case ARCH_item:{
                            {
                                Sprite* sprite = get_sprite(en->sprite_id);
                                Matrix4 xform = m4_identity;
                                xform = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
                                draw_circle_xform(xform, v2(get_sprite_size(sprite).x, get_sprite_size(sprite).y * 0.7), v4(0,0,0,0.5));
                                draw_image_xform(sprite->image, xform, get_sprite_size(sprite), COLOR_WHITE);
                            }
                        }break;

                        default:{
                            {
                                Sprite* sprite = get_sprite(en->sprite_id);
                                Matrix4 xform = m4_identity;
                                xform = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
                                draw_image_xform(sprite->image, xform, get_sprite_size(sprite), COLOR_WHITE);
                            }
                        }break;
                    }


                }




            }
        }
    }
*/
#endif