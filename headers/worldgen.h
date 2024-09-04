#ifndef WORLDGEN_H
#define WORLDGEN_H

#include "types.h"

typedef struct Biome {
    
    string name;
    Vector2 size;
    Range2f pos;

    BiomeData biome_data;
   
    bool enabled;
} Biome;

float biome_id_to_float(BiomeID id){
    // this func returns the float value based on the biomeID. should return always the same value for the biomes
    // NOTE: ordering the biomes in BiomeID matters.

    float start_value = 0;
    int biome_count = BIOME_MAX;
    float increase = 1 / BIOME_MAX;

    // NOTE: using BIOME_MAX here prolly overshoots the thing. do BIOME_MAX - 1
    for (BiomeID i = 1; i < BIOME_MAX; i++){
        if (i == id){
            return start_value;
        }
        start_value += increase;
    }

    // switch (id){
    //     case BIOME_nil: return 0.0f; break;
    //     case BIOME_forest: return 0.0f; break;
    //     case BIOME_pine_forest: return 0.0f; break;
    //     case BIOME_magical_forest: return 0.0f; break;
    //     case BIOME_desert: return 0.0f; break;
    //     case BIOME_cave: return 0.0f; break;
    //     default: log_error("Failed to get float from BiomeID. Missing case @ 'biome_id_to_float'"); break;
    // }
}

void spawn_biome(BiomeData* biome) {
	if (biome->spawn_pine_trees) {create_pine_trees((int)biome->spawn_pine_tree_weight, biome->size.x); }
	if (biome->spawn_spruce_trees) {create_spruce_trees((int)biome->spawn_spruce_tree_weight, biome->size.x); }
	if (biome->spawn_magical_trees) {create_magical_trees((int)biome->spawn_magical_tree_weight, biome->size.x); }
	if (biome->spawn_rocks) {create_rocks((int)biome->spawn_rocks_weight, biome->size.x); }
	if (biome->spawn_berries) {create_bushes((int)biome->spawn_berries_weight, biome->size.x); }
	if (biome->spawn_twigs) {create_twigs((int)biome->spawn_twigs_weight, biome->size.x); }
	if (biome->spawn_mushrooms) {create_mushrooms((int)biome->spawn_mushrooms_weight, biome->size.x); }
	if (biome->enable_parallax) {create_parallax_trees((int)biome->parallax_weight, biome->size.x); }

	
	if (biome->spawn_ores) {
		if (biome->spawn_ore_iron) {create_ores((int)biome->spawn_ore_iron_weight, biome->size.x, ORE_iron); }
		if (biome->spawn_ore_gold) {create_ores((int)biome->spawn_ore_gold_weight, biome->size.x, ORE_gold); }
		if (biome->spawn_ore_copper) {create_ores((int)biome->spawn_ore_copper_weight, biome->size.x, ORE_copper); }
	}



}

void worldgen(Matrix3 perlin){
    for (int i = 0; i < PERLIN_PIXELS; i++){
        float pixel = perlin[i];
        if (pixel > 0 && pixel <= 0.3){
            // biome == forest
        }
        else if (pixel > 0.3)
    }
}


//  create entities
    void create_pine_trees(int amount, int range) {
        // Creates trees
        // Wont allow multiple trees to spawn in the same tile

        Vector2 tree_positions[amount];

        for (int i = 0; i < amount; i++){
            float x = get_random_float32_in_range(-range, range);
            float y = get_random_float32_in_range(-range, range);
            tree_positions[i] = v2(x,y);
        }

        for (int i = 0; i < amount; i++){
            Entity* en = entity_create();
            setup_pine_tree(en);
            en->pos = v2(tree_positions[i].x, tree_positions[i].y);
            en->pos = round_v2_to_tile(en->pos);
            // printf("Created a tree at '%.0f     %.0f'\n", tree_positions[i].x, tree_positions[i].y);
        }


        // free the list of positions
        memset(tree_positions, 0, sizeof(tree_positions));
    }

    void create_spruce_trees(int amount, int range) {
        // Creates trees
        // Wont allow multiple trees to spawn in the same tile

        Vector2 tree_positions[amount];

        for (int i = 0; i < amount; i++){
            float x = get_random_float32_in_range(-range, range);
            float y = get_random_float32_in_range(-range, range);
            tree_positions[i] = v2(x,y);
        }

        for (int i = 0; i < amount; i++){
            Entity* en = entity_create();
            setup_spruce_tree(en);
            en->pos = v2(tree_positions[i].x, tree_positions[i].y);
            en->pos = round_v2_to_tile(en->pos);
            // printf("Created a tree at '%.0f     %.0f'\n", tree_positions[i].x, tree_positions[i].y);
        }


        // free the list of positions
        memset(tree_positions, 0, sizeof(tree_positions));
    }

    void create_magical_trees(int amount, int range) {
        // Creates trees
        // Wont allow multiple trees to spawn in the same tile

        Vector2 tree_positions[amount];

        for (int i = 0; i < amount; i++){
            float x = get_random_float32_in_range(-range, range);
            float y = get_random_float32_in_range(-range, range);
            tree_positions[i] = v2(x,y);
        }

        for (int i = 0; i < amount; i++){
            Entity* en = entity_create();
            setup_magical_tree(en);
            en->pos = v2(tree_positions[i].x, tree_positions[i].y);
            en->pos = round_v2_to_tile(en->pos);
            en->enable_shadow = false;
        }


        // free the list of positions
        memset(tree_positions, 0, sizeof(tree_positions));
    }

    void create_rocks(int amount, int range) {
        // Create rock entities
        // for (int i = 0; i < amount; i++) {

        int last_entity_index = world->dimension->entity_count;

        for (int i = last_entity_index; i < last_entity_index + amount; i++) {
            Entity* en = entity_create();
            setup_rock(en);
            en->pos = v2(get_random_float32_in_range(-range, range), get_random_float32_in_range(-range, range));
            en->pos = round_v2_to_tile(en->pos);
        }	
    }

    void create_bushes(int amount, int range) {
        // Create bush entities
        for (int i = 0; i < amount; i++) {
            Entity* en = entity_create();
            setup_bush(en);
            en->pos = v2(get_random_float32_in_range(-range, range), get_random_float32_in_range(-range, range));
            en->pos = round_v2_to_tile(en->pos);
        }
    }

    void create_twigs(int amount, int range) {
        // Create twig entities
        for (int i = 0; i < amount; i++) {
            Entity* en = entity_create();
            setup_twig(en);
            en->pos = v2(get_random_float32_in_range(-range, range), get_random_float32_in_range(-range, range));
            en->pos = round_v2_to_tile(en->pos);
        }
    }

    void create_mushrooms(int amount, int range) {
        // Create mushroom entities
        for (int i = 0; i < amount; i++) {
            Entity* en = entity_create();
            setup_mushroom(en);
            en->pos = v2(get_random_float32_in_range(-range, range), get_random_float32_in_range(-range, range));
            en->pos = round_v2_to_tile(en->pos);
        }
    }

    void create_ores(int amount, int range, OreID id) {
        // create ore entities
        for (int i = 0; i < amount; i++) {
            Entity* en = entity_create();
            setup_ore(en, id);
            en->pos = v2(get_random_float32_in_range(-range, range), get_random_float32_in_range(-range, range));
            en->pos = round_v2_to_tile(en->pos);
        }
    }

    void clear_empty_slots_in_entities(Entity* entities, int count){
        // Verification loop to ensure no empty slots
        int shiftIndex = 0;
        for (int i = 0; i < count; i++) {
            Entity* en = &entities[i];
            if (en->arch == ARCH_nil) {
                shiftIndex++;
            } else if (shiftIndex > 0) {
                // Shift the entity left to fill the gap
                entities[i - shiftIndex] = entities[i];
            }
        }
    }

    void create_parallax_trees(int amount, int range) {
        // Creates trees
        // Wont allow multiple trees to spawn in the same tile

        Vector2 parallax_positions[amount];

        for (int i = 0; i < amount; i++){
            float x = get_random_float32_in_range(-range, range);
            float y = get_random_float32_in_range(-range, range);
            parallax_positions[i] = v2(x,y);
        }

        for (int i = 0; i < amount; i++){
            Entity* en = entity_create();
            setup_parallax(en);
            en->pos = v2(parallax_positions[i].x, parallax_positions[i].y);
            en->pos = round_v2_to_tile(en->pos);
            // printf("Created a tree at '%.0f     %.0f'\n", tree_positions[i].x, tree_positions[i].y);
        }


        // free the list of positions
        memset(parallax_positions, 0, sizeof(parallax_positions));
    }
// 



#endif