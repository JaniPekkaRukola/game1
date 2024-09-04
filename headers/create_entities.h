#ifndef CREATE_ENTITIES_H
#define CREATE_ENTITIES_H

// void create_pine_trees(int amount, int range) {
// 	// Creates trees
// 	// Wont allow multiple trees to spawn in the same tile

// 	Vector2 tree_positions[amount];

// 	for (int i = 0; i < amount; i++){
// 		float x = get_random_float32_in_range(-range, range);
// 		float y = get_random_float32_in_range(-range, range);
// 		tree_positions[i] = v2(x,y);
// 	}

// 	for (int i = 0; i < amount; i++){
// 		Entity* en = entity_create();
// 		setup_pine_tree(en);
// 		en->pos = v2(tree_positions[i].x, tree_positions[i].y);
// 		en->pos = round_v2_to_tile(en->pos);
// 		// printf("Created a tree at '%.0f     %.0f'\n", tree_positions[i].x, tree_positions[i].y);
// 	}


// 	// free the list of positions
// 	memset(tree_positions, 0, sizeof(tree_positions));
// }

// void create_spruce_trees(int amount, int range) {
// 	// Creates trees
// 	// Wont allow multiple trees to spawn in the same tile

// 	Vector2 tree_positions[amount];

// 	for (int i = 0; i < amount; i++){
// 		float x = get_random_float32_in_range(-range, range);
// 		float y = get_random_float32_in_range(-range, range);
// 		tree_positions[i] = v2(x,y);
// 	}

// 	for (int i = 0; i < amount; i++){
// 		Entity* en = entity_create();
// 		setup_spruce_tree(en);
// 		en->pos = v2(tree_positions[i].x, tree_positions[i].y);
// 		en->pos = round_v2_to_tile(en->pos);
// 		// printf("Created a tree at '%.0f     %.0f'\n", tree_positions[i].x, tree_positions[i].y);
// 	}


// 	// free the list of positions
// 	memset(tree_positions, 0, sizeof(tree_positions));
// }

// void create_magical_trees(int amount, int range) {
// 	// Creates trees
// 	// Wont allow multiple trees to spawn in the same tile

// 	Vector2 tree_positions[amount];

// 	for (int i = 0; i < amount; i++){
// 		float x = get_random_float32_in_range(-range, range);
// 		float y = get_random_float32_in_range(-range, range);
// 		tree_positions[i] = v2(x,y);
// 	}

// 	for (int i = 0; i < amount; i++){
// 		Entity* en = entity_create();
// 		setup_magical_tree(en);
// 		en->pos = v2(tree_positions[i].x, tree_positions[i].y);
// 		en->pos = round_v2_to_tile(en->pos);
// 		en->enable_shadow = false;
// 	}


// 	// free the list of positions
// 	memset(tree_positions, 0, sizeof(tree_positions));
// }

// void create_rocks(int amount, int range) {
// 	// Create rock entities
// 	// for (int i = 0; i < amount; i++) {

// 	int last_entity_index = world->dimension->entity_count;

// 	for (int i = last_entity_index; i < last_entity_index + amount; i++) {
// 		Entity* en = entity_create();
// 		setup_rock(en);
// 		en->pos = v2(get_random_float32_in_range(-range, range), get_random_float32_in_range(-range, range));
// 		en->pos = round_v2_to_tile(en->pos);
// 	}	
// }

// void create_bushes(int amount, int range) {
// 	// Create bush entities
// 	for (int i = 0; i < amount; i++) {
// 		Entity* en = entity_create();
// 		setup_bush(en);
// 		en->pos = v2(get_random_float32_in_range(-range, range), get_random_float32_in_range(-range, range));
// 		en->pos = round_v2_to_tile(en->pos);
// 	}
// }

// void create_twigs(int amount, int range) {
// 	// Create twig entities
// 	for (int i = 0; i < amount; i++) {
// 		Entity* en = entity_create();
// 		setup_twig(en);
// 		en->pos = v2(get_random_float32_in_range(-range, range), get_random_float32_in_range(-range, range));
// 		en->pos = round_v2_to_tile(en->pos);
// 	}
// }

// void create_mushrooms(int amount, int range) {
// 	// Create mushroom entities
// 	for (int i = 0; i < amount; i++) {
// 		Entity* en = entity_create();
// 		setup_mushroom(en);
// 		en->pos = v2(get_random_float32_in_range(-range, range), get_random_float32_in_range(-range, range));
// 		en->pos = round_v2_to_tile(en->pos);
// 	}
// }

// void create_ores(int amount, int range, OreID id) {
// 	// create ore entities
// 	for (int i = 0; i < amount; i++) {
// 		Entity* en = entity_create();
// 		setup_ore(en, id);
// 		en->pos = v2(get_random_float32_in_range(-range, range), get_random_float32_in_range(-range, range));
// 		en->pos = round_v2_to_tile(en->pos);
// 	}
// }

// void clear_empty_slots_in_entities(Entity* entities, int count){
// 	// Verification loop to ensure no empty slots
//     int shiftIndex = 0;
//     for (int i = 0; i < count; i++) {
// 		Entity* en = &entities[i];
//         if (en->arch == ARCH_nil) {
//             shiftIndex++;
//         } else if (shiftIndex > 0) {
//             // Shift the entity left to fill the gap
//             entities[i - shiftIndex] = entities[i];
//         }
//     }
// }

// void create_parallax_trees(int amount, int range) {
// 	// Creates trees
// 	// Wont allow multiple trees to spawn in the same tile

// 	Vector2 parallax_positions[amount];

// 	for (int i = 0; i < amount; i++){
// 		float x = get_random_float32_in_range(-range, range);
// 		float y = get_random_float32_in_range(-range, range);
// 		parallax_positions[i] = v2(x,y);
// 	}

// 	for (int i = 0; i < amount; i++){
// 		Entity* en = entity_create();
// 		setup_parallax(en);
// 		en->pos = v2(parallax_positions[i].x, parallax_positions[i].y);
// 		en->pos = round_v2_to_tile(en->pos);
// 		// printf("Created a tree at '%.0f     %.0f'\n", tree_positions[i].x, tree_positions[i].y);
// 	}


// 	// free the list of positions
// 	memset(parallax_positions, 0, sizeof(parallax_positions));
// }

#endif