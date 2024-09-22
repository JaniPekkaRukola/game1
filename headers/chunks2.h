#ifndef CHUNKS2_H
#define CHUNKS2_H

#define MAX_CHUNK_ENTITIES 100

#define CHUNK_RENDER_DISTANCE 2


typedef struct Chunk {
    BiomeID biome_id;
    Vector2 pos_in_world;
    Vector2 pos_in_grid;
    Vector2 size;
    Entity* entities[MAX_CHUNK_ENTITIES];
    int entity_count;

    bool has_been_loaded;
} Chunk;


void initialize_chunks(DimensionData* dimension) {
    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            dimension->chunks[x][y] = NULL;
        }
    }
}

Chunk* get_chunk(DimensionData* dimension, Vector2 pos) {
    int x = (int)pos.x;
    int y = (int)pos.y;
    return dimension->chunks[x][y];
}


void load_chunk(DimensionData* dimension, Vector2 pos) {
    int x = (int)pos.x;
    int y = (int)pos.y;

    if (dimension->chunks[x][y] == NULL) {
        // Allocate memory for the new chunk
        dimension->chunks[x][y] = alloc(get_heap_allocator(), sizeof(Chunk));
        dimension->chunks[x][y]->pos_in_grid = pos;
        dimension->chunks[x][y]->entity_count = 0;

        // Initialize the chunk (e.g., generate terrain, entities, etc.)
        world->dimension->chunk_count++;
    }
}


void unload_chunk(DimensionData* dimension, Vector2 pos) {
    int x = (int)pos.x;
    int y = (int)pos.y;

    if (dimension->chunks[x][y] != NULL) {
        // Free memory for the chunk
        dealloc(get_heap_allocator(), dimension->chunks[x][y]);
        dimension->chunks[x][y] = NULL;
        world->dimension->chunk_count--;
    }
}


void load_chunks_around_player() {

    DimensionData* dimension = world->dimension;

    int render_distance = CHUNK_RENDER_DISTANCE;

    Vector2 player_pos = get_player_pos();

    int chunk_size = dimension->chunk_size;

    // Convert player world position to chunk coordinates
    int player_chunk_x = (int)(player_pos.x / chunk_size);
    int player_chunk_y = (int)(player_pos.y / chunk_size);

    // Iterate over the chunks within the render distance
    for (int dx = -render_distance; dx <= render_distance; dx++) {
        for (int dy = -render_distance; dy <= render_distance; dy++) {
            int chunk_x = player_chunk_x + dx;
            int chunk_y = player_chunk_y + dy;

            // Ensure chunk coordinates are within world bounds (optional, depending on world size)
            if (chunk_x >= 0 && chunk_x < WORLD_WIDTH && chunk_y >= 0 && chunk_y < WORLD_HEIGHT) {
                Vector2 chunk_pos = v2(chunk_x, chunk_y);
                load_chunk(dimension, chunk_pos);  // Load chunk if not already loaded
            }
        }
    }
}

void unload_chunks_outside_render_distance() {

    DimensionData* dimension = world->dimension;

    Vector2 player_pos = get_player_pos();
    int render_distance = CHUNK_RENDER_DISTANCE;

    int chunk_size = dimension->chunk_size;

    // Convert player world position to chunk coordinates
    int player_chunk_x = (int)(player_pos.x / chunk_size);
    int player_chunk_y = (int)(player_pos.y / chunk_size);

    // Iterate over all chunks in the world
    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            Chunk* chunk = dimension->chunks[x][y];

            if (chunk != NULL) {
                // Check the chunk's position relative to the player
                int chunk_x = (int)chunk->pos_in_grid.x;
                int chunk_y = (int)chunk->pos_in_grid.y;

                int dx = abs(chunk_x - player_chunk_x);
                int dy = abs(chunk_y - player_chunk_y);

                // If the chunk is outside the render distance, unload it
                if (dx > render_distance || dy > render_distance) {
                    unload_chunk(dimension, chunk->pos_in_grid);
                }
            }
        }
    }
}




// Chunk* create_chunk(int x, int y){
    //     // NOTE: x and y are positions in the chunk grid. NOT in worldspace!!!

    //     for (int i = 0; i < MAX_CHUNKS; i++){
    //         for (int j = 0; j < MAX_CHUNKS; j++){
    //             Chunk* chunk = world->dimension->chunks[i][j];
    //             if (chunk->pos_in_grid.x == x && chunk->pos_in_grid.y == y){
    //                 printf("Chunk already exists\n");
    //             }
    //         }
    //     }


    //     // chunk->entities = alloc(get_heap_allocator(), sizeof(Entity) * MAX_CHUNK_ENTITIES);
    //     // chunk->biome_id = BIOME_nil;
    //     // chunk->entity_count = 0;
    //     // chunk->has_been_loaded = false;
    //     // chunk->size = v2(CHUNK_SIZE, CHUNK_SIZE);
    //     // chunk->pos_in_grid = v2(x,y);
    //     Chunk* asd = 0;
    //     return asd;
// }
// void setup_dimension_chunks(DimensionData* dimension) {

    //     // Allocate the chunk grid
    //     dimension->chunks = (Chunk***)alloc(get_heap_allocator(), sizeof(Chunk)*CHUNK_SIZE);

    //     for (int i = 0; i < CHUNK_SIZE; i++) {
    //     	dimension->chunks[i] = (Chunk**)alloc(get_heap_allocator(), sizeof(Chunk**) * CHUNK_SIZE);
    //     	for (int j = 0; j < CHUNK_SIZE; j++) {
    //     		dimension->chunks[i][j] = NULL;
    //     	}
    //     }
    //     assert(dimension->chunks, "Failed to allocate memory for chunks");



    //     // int width = world->dimension->map.width;
    //     // int height = world->dimension->map.height;
    //     int width = 100;
    //     int height = 100;

    //     for (int x = 0; x < width; x++){
    //         for (int y = 0; y < height; y++){
    //             Chunk* chunk = dimension->chunks[x][y];
    //             chunk = NULL;
    //         }
    //     }
// }

void spawn_chunk_entities(BiomeID biome_id){
    switch (biome_id){
        case BIOME_forest:{
            {
                
            }
        } break;

        default:{}break;
    }
}

void render_chunks(){
    // set_world_space();

    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            Chunk* chunk = world->dimension->chunks[x][y];

            if (chunk != NULL) {
                float chunk_pos_world_x = x * CHUNK_SIZE;
                float chunk_pos_world_y = y * CHUNK_SIZE;

                chunk->biome_id = BIOME_forest;

                Texture* texture = get_texture(get_biome_data_from_id(chunk->biome_id).ground_texture);
                Vector4 ground_color = get_biome_data_from_id(chunk->biome_id).grass_color;

                Matrix4 xform = m4_identity;
                xform = m4_translate(xform, v3(chunk_pos_world_x, chunk_pos_world_y, 0));

                draw_image_xform(texture->image, xform, v2(CHUNK_SIZE, CHUNK_SIZE), ground_color);

                spawn_chunk_entities(chunk->biome_id);

                // draw_rect_with_border(xform, v2(CHUNK_SIZE - 1, CHUNK_SIZE - 1), 2, COLOR_WHITE, COLOR_BLACK);

                // printf("chunk_pos_world = %.0f, %.0f\n", chunk_pos_world_x, chunk_pos_world_y);
                // printf("Chunk at (%d, %d) is loaded.\n", x, y);
            }
            else {
                // Chunk not loaded
                // printf("Chunk at (%d, %d) is NULL.\n", x, y);
            }
        }
    }

    // set_screen_space();
}


void do_chunk_magic(){

    load_chunks_around_player();

    unload_chunks_outside_render_distance();

    render_chunks();

    printf("Chunk count = %d\n", world->dimension->chunk_count);

}

#endif