#ifndef CHUNKS2_H
#define CHUNKS2_H


#define CHUNK_RENDER_DISTANCE 1 // how many chunks are loaded into each direction from center. value of 2 is 25 chunks total (5x5). value of 1 is 9 chunks total (3x3) etc. NOTE: atleast with only 1 tree type rendering the total fps drop from values 1 to 2 is about -10%

// chunk offsets. These must be used to store negative values. So now the (0.0) chunk is in the middle of the chunks array
#define CHUNK_OFFSET_X WORLD_WIDTH / 2
#define CHUNK_OFFSET_Y WORLD_HEIGHT / 2

int total_entity_count = 0;

typedef struct Chunk {
    BiomeID biome_id;
    Vector2 pos_in_world;
    Vector2 pos_in_grid;
    Vector2 size;
    // Entity* entities[MAX_CHUNK_ENTITIES];
    int entity_count;
    Entity entities[MAX_CHUNK_ENTITIES];

    bool has_been_loaded;
} Chunk;


void initialize_chunks(DimensionData* dimension) {
    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            dimension->chunks[x][y] = NULL;
        }
    }
    printf("Initialized chunks for DimensionID = %d\n", dimension->id);
}

Chunk* get_chunk(DimensionData* dimension, Vector2 pos) {
    // pos is position in the chunks array
    int x = (int)pos.x + CHUNK_OFFSET_X;
    int y = (int)pos.y + CHUNK_OFFSET_Y;
    return dimension->chunks[x][y];
}

Vector2 get_chunk_world_position(int x, int y) {
    // Subtract the chunk offset and multiply by chunk size to get world coordinates
    float world_x = (x - CHUNK_OFFSET_X) * CHUNK_SIZE;
    float world_y = (y - CHUNK_OFFSET_Y) * CHUNK_SIZE;

    // Return the world position as a Vector2
    return v2(world_x, world_y);
}


Entity* entity_create_to_chunk(Chunk* chunk) {
    Entity* entity_found = 0;

    int asd_x = chunk->pos_in_grid.x + CHUNK_OFFSET_X;
    int asd_y = chunk->pos_in_grid.y + CHUNK_OFFSET_Y;

    for (int i = 0; i < MAX_CHUNK_ENTITIES; i++){
        Entity* existing_entity = &chunk->entities[i];

        // if (existing_entity == NULL){
        //     // printf("EXISTING ENTITY = NULL! Allocating memory...\n");
        //     existing_entity = alloc(get_heap_allocator(), sizeof(Entity));
        // }

        if (!existing_entity->is_valid){
            chunk->entity_count++;
            total_entity_count++;
            chunk->has_been_loaded = true;
            entity_found = existing_entity;
            break;
        }
    }

    assert(entity_found, "No more free entities!");
    entity_found->is_valid = true;

    return entity_found;
}

void create_tree_to_chunk(Chunk* chunk, int amount, Range2f range){
    Vector2 positions[amount];

    for (int i = 0; i < amount; i++){
        float x = get_random_float32_in_range(range.min.x, range.max.x);
        float y = get_random_float32_in_range(range.min.y, range.max.y);
        positions[i] = v2(x,y);
    }

    for (int i = 0; i < amount; i++){
        Entity* en = entity_create_to_chunk(chunk);
        // setup tree
        setup_pine_tree(en);
        // en->pos = v2(positions[i].x, positions[i].y);
        en->pos = v2(positions[i].x + chunk->pos_in_world.x, positions[i].y + chunk->pos_in_world.y);
        en->pos = round_v2_to_tile(en->pos);
    }


    // free the list of positions
	memset(positions, 0, sizeof(positions));
    // dealloc(get_heap_allocator(), (Vector2*)&positions);

    int res_x = chunk->pos_in_grid.x + CHUNK_OFFSET_X;
    int res_y = chunk->pos_in_grid.y + CHUNK_OFFSET_Y;

    printf("Created %d trees to chunk[%d][%d]\n", amount, res_x, res_y);
}


void spawn_chunk_entities(Chunk* chunk){

    // chunks pos in worldspace
    Vector2 chunk_pos_in_world;
    chunk_pos_in_world.x = (chunk->pos_in_grid.x - CHUNK_OFFSET_X) / CHUNK_SIZE;
    chunk_pos_in_world.y = (chunk->pos_in_grid.y - CHUNK_OFFSET_Y) / CHUNK_SIZE;
    // chunk_pos_in_world = get_chunk_world_position(chunk->pos_in_grid.x, chunk->pos_in_grid.y);

    // chunk boundaries in worldspace
    Range2f chunk_boundaries;
    chunk_boundaries.min.x = chunk_pos_in_world.x;
    chunk_boundaries.min.y = chunk_pos_in_world.y;
    chunk_boundaries.max.x = chunk_pos_in_world.x + CHUNK_SIZE;
    chunk_boundaries.max.y = chunk_pos_in_world.y + CHUNK_SIZE;

    BiomeData biomedata = get_biome_data_from_id(chunk->biome_id);

    if (biomedata.spawn_pine_trees) create_tree_to_chunk(chunk, biomedata.pine_tree_weight, chunk_boundaries);

}

void load_chunk(DimensionData* dimension, Vector2 pos) {
    int x = (int)pos.x + CHUNK_OFFSET_X;
    int y = (int)pos.y + CHUNK_OFFSET_Y;

    // if (x >= 0 && x < WORLD_WIDTH && y >= 0 && y < WORLD_HEIGHT) {
    if (x < WORLD_WIDTH && y < WORLD_HEIGHT) {
        Chunk* chunk = dimension->chunks[x][y];
        if (chunk == NULL) {
            chunk = alloc(get_heap_allocator(), sizeof(Chunk));
            chunk->pos_in_grid = pos;
            chunk->entity_count = 0;
            chunk->biome_id = BIOME_forest;
            // dimension->chunks[x][y]->pos_in_grid = v2(x,y);
            chunk->pos_in_world = get_chunk_world_position(x, y);
            world->dimension->chunk_count++;
            printf("Loaded new chunk\n");
            world->dimension->chunks[x][y] = chunk;

            spawn_chunk_entities(dimension->chunks[x][y]);
        }
    }
}

void unload_chunk(DimensionData* dimension, Vector2 pos) {
    int x = (int)pos.x + CHUNK_OFFSET_X;
    int y = (int)pos.y + CHUNK_OFFSET_Y;

    // TODO: does the entities list also need to be freed???

    if (x >= 0 && x < WORLD_WIDTH && y >= 0 && y < WORLD_HEIGHT) {
        if (dimension->chunks[x][y] != NULL) {
            total_entity_count -= dimension->chunks[x][y]->entity_count;
            dealloc(get_heap_allocator(), dimension->chunks[x][y]);
            dimension->chunks[x][y] = NULL;
            world->dimension->chunk_count--;
            printf("UN-Loaded chunk\n");
        }
    }
}


void load_chunks_renderdistance() {

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

            // Ensure chunk coordinates are within world bounds
            if (chunk_x < WORLD_WIDTH && chunk_y < WORLD_HEIGHT) {
                Vector2 chunk_pos = v2(chunk_x, chunk_y);
                load_chunk(dimension, chunk_pos);  // Load chunk if not already loaded
                // printf("LOADED CHUNK AT (grid pos) = %d, %d\n", chunk_x, chunk_y);
            }
        }
    }
}

void unload_chunks_renderdistance() {

    DimensionData* dimension = world->dimension;

    Vector2 player_pos = get_player_pos();
    int render_distance = CHUNK_RENDER_DISTANCE;

    int chunk_size = dimension->chunk_size;

    // Convert player world position to chunk coordinates
    int player_chunk_x = (int)(player_pos.x / chunk_size) + CHUNK_OFFSET_X;
    int player_chunk_y = (int)(player_pos.y / chunk_size) + CHUNK_OFFSET_Y;

    // Iterate over all chunks in the world
    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            Chunk* chunk = dimension->chunks[x][y];

            if (chunk != NULL) {
                // Check the chunk's position relative to the player
                // int chunk_x = (int)chunk->pos_in_grid.x + CHUNK_OFFSET_X;
                // int chunk_y = (int)chunk->pos_in_grid.y + CHUNK_OFFSET_Y;
                // int chunk_x = (int)chunk->pos_in_grid.x;
                // int chunk_y = (int)chunk->pos_in_grid.y;
                int chunk_x = x;
                int chunk_y = y;

                int dx = abs(chunk_x - player_chunk_x);
                int dy = abs(chunk_y - player_chunk_y);

                // If the chunk is outside the render distance, unload it
                if (dx > render_distance || dy > render_distance) {
                    // unload_chunk(dimension, chunk->pos_in_grid);
                    unload_chunk(dimension, chunk->pos_in_grid);
                }
            }
        }
    }
}

void render_chunk_ground(){
    // render chunk ground texture
    for (int x = 0; x < WORLD_WIDTH; x++){
        for (int y = 0; y < WORLD_HEIGHT; y++){
            Chunk* chunk = world->dimension->chunks[x][y];
            if (chunk != NULL){
                Texture* texture = get_texture(get_biome_data_from_id(chunk->biome_id).ground_texture);
                Vector4 ground_color = get_biome_data_from_id(chunk->biome_id).grass_color;

                Vector2 chunk_pos_world = get_chunk_world_position(x,y);

                Vector2 assurance;
                assurance.x = (x - CHUNK_OFFSET_X) * CHUNK_SIZE;
                assurance.y = (y - CHUNK_OFFSET_Y) * CHUNK_SIZE;

                if (chunk_pos_world.x != assurance.x && chunk_pos_world.y != assurance.y){
                    assert(1==0, "PERKELE\n");
                }

                Matrix4 xform = m4_identity;
                xform = m4_translate(xform, v3(chunk_pos_world.x, chunk_pos_world.y, 0));
                draw_image_xform(texture->image, xform, v2(CHUNK_SIZE, CHUNK_SIZE), ground_color);
            }
        }
    }
}


void render_chunk_entities(){
    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            Chunk* chunk = world->dimension->chunks[x][y];

            if (chunk != NULL) {

                // printf("Chunk pos in grid = %d, %d\n", x, y);

                // render chunk ground texture
                // float chunk_pos_world_x = (x - CHUNK_OFFSET_X) * CHUNK_SIZE;
                // float chunk_pos_world_y = (y - CHUNK_OFFSET_Y) * CHUNK_SIZE;
                // // float chunk_pos_world_x = x * CHUNK_SIZE;
                // // float chunk_pos_world_y = y * CHUNK_SIZE;

                // Texture* texture = get_texture(get_biome_data_from_id(chunk->biome_id).ground_texture);
                // Vector4 ground_color = get_biome_data_from_id(chunk->biome_id).grass_color;

                // Matrix4 xform = m4_identity;
                // xform = m4_translate(xform, v3(chunk_pos_world_x, chunk_pos_world_y, 0));

                // draw_image_xform(texture->image, xform, v2(CHUNK_SIZE, CHUNK_SIZE), ground_color);
                // if (x == 255 && y == 255){
                //     draw_rect_with_border(xform, v2(CHUNK_SIZE-1, CHUNK_SIZE-1), 2, COLOR_BLUE, COLOR_BLACK);
                // }
                // else{
                //     draw_rect_with_border(xform, v2(CHUNK_SIZE-1, CHUNK_SIZE-1), 2, COLOR_RED, COLOR_BLACK);
                // }


                // render chunk entities
                for (int i = 0; i < chunk->entity_count; i++){
                    Entity* en = &chunk->entities[i];
                    if (en != NULL){
                        switch (en->arch){
                            default:{
                                {
                                    Sprite* sprite = get_sprite(en->sprite_id);

                                    Matrix4 xform = m4_identity;
                                    xform = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
                                    draw_image_xform(sprite->image, xform, get_sprite_size(sprite), COLOR_WHITE);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void print_chunk_count(){
    set_screen_space();
    Matrix4 xform = m4_identity;
    xform = m4_translate(xform, v3(0, screen_height - 10, 0));
    draw_text_xform(font, sprint(get_heap_allocator(), STR("chunk count = %d"), world->dimension->chunk_count), font_height, xform, v2(0.1, 0.1), COLOR_WHITE);

    xform = m4_translate(xform, v3(0, -5, 0));
    draw_text_xform(font, sprint(get_heap_allocator(), STR("Chunks entity count = %d"), total_entity_count), font_height, xform, v2(0.1, 0.1), COLOR_WHITE);

    set_world_space();
}

void do_chunk_magic(){

    load_chunks_renderdistance();

    unload_chunks_renderdistance();

    render_chunk_ground();

    render_chunk_entities();

    // printf("Chunk count = %d\n", world->dimension->chunk_count);
    print_chunk_count();

}

#endif