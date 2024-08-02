// ----- settings --------------------------------------------------------------------------------------

float entity_selection_radius = 5.0f;
const int tile_width = 8;

const int player_pickup_radius = 15.0;
const int player_health = 3;
const int rock_health = 1;
const int tree_health = 1;
const int bush_health = 1;

const Vector4 item_shadow_color = {0, 0, 0, 0.1};


// ----- engine changes (by: randy) ----------------------------------------------------------------

#define m4_identity m4_make_scale(v3(1, 1, 1))

// "resizable" array helpers (by: randy)
// void* array_add() {}


// inline float v2_length(Vector2 a) {
//     return sqrt(a.x * a.x + a.y * a.y);
// }


// randy: is this something that's usually standard in math libraries or am I tripping?
inline float v2_dist(Vector2 a, Vector2 b) {
    return v2_length(v2_sub(a, b));
}


Vector2 range2f_get_center(Range2f range) {
	return (Vector2) {(range.max.x - range.min.x) * 0.5 + range.min.x, (range.max.y - range.min.y) * 0.5 + range.min.y};
}




// ----- the scuff zone (by: randy) --------------------------------------------------------------------

Draw_Quad ndc_quad_to_screen_quad(Draw_Quad ndc_quad) {

	// NOTE: we're assuming these are the screen space matricies (by: randy)
	Matrix4 proj = draw_frame.projection;
	Matrix4 view = draw_frame.view;
	
	Matrix4 ndc_to_screen_space = m4_identity;
	ndc_to_screen_space = m4_mul(ndc_to_screen_space, m4_inverse(proj));
	ndc_to_screen_space = m4_mul(ndc_to_screen_space, view);

	ndc_quad.bottom_left = m4_transform		(ndc_to_screen_space, v4(v2_expand(ndc_quad.bottom_left), 0, 1)).xy;
	ndc_quad.bottom_right = m4_transform	(ndc_to_screen_space, v4(v2_expand(ndc_quad.bottom_right), 0, 1)).xy;
	ndc_quad.top_left = m4_transform		(ndc_to_screen_space, v4(v2_expand(ndc_quad.top_left), 0, 1)).xy;
	ndc_quad.top_right = m4_transform		(ndc_to_screen_space, v4(v2_expand(ndc_quad.top_right), 0, 1)).xy;

	return ndc_quad;
}




// ----- funky -----------------------------------------------------------------------------------------

// 0 -> 1
float sin_breathe(float time, float rate) {
	return (sin(time * rate) + 1.0) / 2.0;
}

bool almost_equals(float a, float b, float epsilon) {
	return fabs(a - b) <= epsilon;
}

bool animate_f32_to_target(float* value, float target, float delta_t, float rate) {
	*value += (target - *value) * (1.0 - pow(2.0f, -rate * delta_t));
	if (almost_equals(*value, target, 0.001f)) {
		*value = target;
		return true; // reached
	}
	return false;
}

void animate_v2_to_target(Vector2* value, Vector2 target, float delta_t, float rate) {
	animate_f32_to_target(&(value->x), target.x, delta_t, rate);
	animate_f32_to_target(&(value->y), target.y, delta_t, rate);
}

Range2f quad_to_range(Draw_Quad quad) {
	return (Range2f) {quad.bottom_left, quad.top_right};
}



// ^^^ Generic utils



int world_pos_to_tile_pos(float world_pos) {
	return roundf(world_pos / (float)tile_width);
}

float tile_pos_to_world_pos(int tile_pos) {
	return ((float)tile_pos * (float)tile_width);
}

Vector2 round_v2_to_tile(Vector2 world_pos) {
	world_pos.x = tile_pos_to_world_pos(world_pos_to_tile_pos(world_pos.x));
	world_pos.y = tile_pos_to_world_pos(world_pos_to_tile_pos(world_pos.y));
	return world_pos;
}



typedef struct Sprite {
	Gfx_Image* image;
	// Vector2 size;
} Sprite;
typedef enum SpriteID {
	SPRITE_nil,
	SPRITE_player,
	SPRITE_tree0,
	SPRITE_tree1,
	SPRITE_rock0,
	SPRITE_rock1,
	SPRITE_bush0,

	// Items
	SPRITE_item_rock,
	SPRITE_item_pine_wood,
	SPRITE_item_sprout,
	SPRITE_item_berry,

	SPRITE_MAX,
} SpriteID;
Sprite sprites[SPRITE_MAX];

Sprite* get_sprite(SpriteID id) {
	if (id >= 0 && id < SPRITE_MAX) {
		return &sprites[id];
	}
	return &sprites[0];
}

Vector2 get_sprite_size(Sprite* sprite) {
	return (Vector2) {sprite->image->width, sprite->image->height};
}


// typedef struct Item {
// 	// ...
// } Item;
// typedef enum ItemID {
// 	ITEM_nil,
// 	ITEM_rock,
// 	ITEM_pine_wood,
// 	ITEM_MAX,
// } ItemID;
// Item items[ITEM_MAX];
// Item* get_item(ItemID id) {
// 	if (id >= 0 && id < ITEM_MAX) {
// 		return &items[id];
// 	}
// 	return &items[0];
// }


typedef enum EntityArchetype {
	arch_nil = 0,
	arch_rock = 1,
	arch_tree = 2,
	arch_bush = 3,
	arch_player = 4,

	arch_item_rock = 5,
	arch_item_pine_wood = 6,
	arch_item_sprout = 7,
	arch_item_berry = 8,
	ARCH_MAX,

} EntityArchetype;

// dont like this - jani
SpriteID get_sprite_id_from_archetype(EntityArchetype arch) {
	switch (arch) {
		case arch_item_pine_wood: return SPRITE_item_pine_wood; break;
		case arch_item_rock: return SPRITE_item_rock; break;
		case arch_item_sprout: return SPRITE_item_sprout; break;
		case arch_item_berry: return SPRITE_item_berry; break;
		default: return 0;
	}
}

// donnt like this - jani
string get_archetype_name(EntityArchetype arch) {
	switch (arch) {
		case arch_item_pine_wood: return STR("Pine Wood");
		case arch_item_rock: return STR("Rock");
		case arch_item_sprout: return STR("Sprout");
		case arch_item_berry: return STR("Berry");
		default: return STR("nill");
	}
}



typedef struct Entity {
	bool is_valid;
	EntityArchetype arch;
	Vector2 pos;
	bool render_sprite;
	SpriteID sprite_id;
	int health;
	bool destroyable;
	bool is_item;
} Entity;

#define MAX_ENTITY_COUNT 1024

typedef struct ItemData {
	// EntityArchetype type;
	int amount;
} ItemData;

typedef enum UXState {
	UX_nil,
	UX_inventory,
	// UX_building,
} UXState;


// WORLD
typedef struct World {
	Entity entities[MAX_ENTITY_COUNT];
	ItemData inventory_items[ARCH_MAX];
	UXState ux_state;
	float inventory_alpha;
	float inventory_alpha_target;
} World;
World* world = 0;

typedef struct WorldFrame {
	Entity* selected_entity;
} WorldFrame;
WorldFrame world_frame;

Entity* entity_create() {
	Entity* entity_found = 0;
	for (int i = 0; i < MAX_ENTITY_COUNT; i++) {
		Entity* existing_entity = &world->entities[i];
		if (!existing_entity->is_valid) {
			entity_found = existing_entity;
			break;
		}
	}
	assert(entity_found, "No more free entities!");
	entity_found->is_valid = true;
	return entity_found;
}

void entity_destroy(Entity* entity) {
	memset(entity, 0, sizeof(Entity));
}

// ----- :Entity setups --------------------------------------------------------------------------------
void setup_player(Entity* en) {
	en->arch = arch_player;
	en->sprite_id = SPRITE_player;
	en->health = player_health;
}

void setup_rock(Entity* en) {
	en->arch = arch_rock;
	en->sprite_id = SPRITE_rock0;
	en->health = rock_health;
	en->destroyable = true;
}

void setup_tree(Entity* en) {
	en->arch = arch_tree;
	en->sprite_id = SPRITE_tree0;
	// en->sprite_id = SPRITE_tree1;
	en->health = tree_health;
	en->destroyable = true;
}

void setup_bush(Entity* en) {
	en->arch = arch_bush;
	en->sprite_id = SPRITE_bush0;
	en->health = bush_health;
	en->destroyable = true;
}

// Item setups
void setup_item_rock(Entity* en) {
	en->arch = arch_item_rock;
	en->sprite_id = SPRITE_item_rock;
	en->is_item = true;
}

void setup_item_pine_wood(Entity* en) {
	en->arch = arch_item_pine_wood;
	en->sprite_id = SPRITE_item_pine_wood;
	en->is_item = true;
}

void setup_item_sprout(Entity* en) {
	en->arch = arch_item_sprout;
	en->sprite_id = SPRITE_item_sprout;
	en->is_item = true;
}

void setup_item_berry(Entity* en) {
	en->arch = arch_item_berry;
	en->sprite_id = SPRITE_item_berry;
	en->is_item = true;
}

// void setup_tree1(Entity* en) {
// 	en->arch = arch_tree;
// 	en->sprite_id = SPRITE_tree1;
// 	// en->sprite_id = SPRITE_tree1;
// }


// ----- coordinate stuff ------------------------------------------------------------------------------

Vector2 get_mouse_pos_in_ndc() {
	float mouse_x = input_frame.mouse_x;
	float mouse_y = input_frame.mouse_y;
	Matrix4 proj = draw_frame.projection;
	Matrix4 view = draw_frame.view;
	float window_w = window.width;
	float window_h = window.height;

	// Normalize the mouse coordinates
	float ndc_x = (mouse_x / (window_w * 0.5f)) - 1.0f;
	float ndc_y = (mouse_y / (window_h * 0.5f)) - 1.0f;

	return (Vector2) {ndc_x, ndc_y};
}


Vector2 screen_to_world() {
	float mouse_x = input_frame.mouse_x;
	float mouse_y = input_frame.mouse_y;
	Matrix4 proj = draw_frame.projection;
	Matrix4 view = draw_frame.view;
	float window_w = window.width;
	float window_h = window.height;

	// Normalize the mouse coordinates
	float ndc_x = (mouse_x / (window_w * 0.5f)) - 1.0f;
	float ndc_y = (mouse_y / (window_h * 0.5f)) - 1.0f;

	// Transform to world coordinates
	Vector4 world_pos = v4(ndc_x, ndc_y, 0, 1);
	world_pos = m4_transform(m4_inverse(proj), world_pos);
	world_pos = m4_transform(view, world_pos);
	// log("%f, %f", world_pos.x, world_pos.y);

	// Return as 2D vector
	return (Vector2){ world_pos.x, world_pos.y};
}



// ----- SETUP -----------------------------------------------------------------------------------------
int entry(int argc, char **argv) {

	bool IS_DEBUG = false;
	
	// Window
	window.title = STR("Game");
	window.scaled_width = 1280; // We need to set the scaled size if we want to handle system scaling (DPI)
	window.scaled_height = 720; 
	// window.x = 200; // default value
	// window.y = 200; // default value
	window.x = window.width * 0.5;
	window.y = window.height * 0.5;

	window.clear_color = hex_to_rgba(0x43693aff);

	// Memory
	world = alloc(get_heap_allocator(), sizeof(World));
	memset(world, 0, sizeof(World));

	// :Load entity sprites
	sprites[0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/missing_texture.png"), get_heap_allocator())};
	sprites[SPRITE_player] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/player.png"), get_heap_allocator())};
	sprites[SPRITE_tree0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tree0.png"), get_heap_allocator())};
	sprites[SPRITE_tree1] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tree1.png"), get_heap_allocator())};
	sprites[SPRITE_rock0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/rock0.png"), get_heap_allocator())};
	sprites[SPRITE_bush0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/bush0.png"), get_heap_allocator())};

	// :Load item sprites
	sprites[SPRITE_item_rock] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_rock.png"), get_heap_allocator())};
	sprites[SPRITE_item_pine_wood] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_pine_wood.png"), get_heap_allocator())};
	sprites[SPRITE_item_sprout] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_sprout.png"), get_heap_allocator())};
	sprites[SPRITE_item_berry] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_berry.png"), get_heap_allocator())};

	// Font
	Gfx_Font *font = load_font_from_disk(STR("C:/windows/fonts/arial.ttf"), get_heap_allocator());
	assert(font, "Failed loading arial.ttf, %d", GetLastError());
	const u32 font_height = 48;


	// :Create entities

	// :init

	// test item adding
	{
		// world->inventory_items[arch_item_pine_wood].amount = 5;
		// world->inventory_items[arch_item_rock].amount = 5;
		world->inventory_items[arch_item_sprout].amount = 2;
	}

	
	// :Create player entity
	Entity* player_en = entity_create();
	setup_player(player_en);


	// Create rock entities
	for (int i = 0; i < 10; i++) {
		Entity* en = entity_create();
		setup_rock(en);
		en->pos = v2(get_random_float32_in_range(-200, 200), get_random_float32_in_range(-200, 200));
		en->pos = round_v2_to_tile(en->pos);
		// en->pos.y -= tile_width * 0.5;	// bring sprite down to bottom of Tile (now done in sprite renderer)
		
	}	

	// Create tree0 entities
	for (int i = 0; i < 120; i++) {
		Entity* en = entity_create();
		setup_tree(en);
		en->pos = v2(get_random_float32_in_range(-200, 200), get_random_float32_in_range(-200, 200));
		en->pos = round_v2_to_tile(en->pos);
		// en->pos.y -= tile_width * 0.5;	// bring sprite down to bottom of Tile (now done in sprite renderer)
	}

	// Create bush entities
	for (int i = 0; i < 15; i++) {
		Entity* en = entity_create();
		setup_bush(en);
		en->pos = v2(get_random_float32_in_range(-200, 200), get_random_float32_in_range(-200, 200));
		en->pos = round_v2_to_tile(en->pos);
		// en->pos.y -= tile_width * 0.5;	// bring sprite down to bottom of Tile (now done in sprite renderer)
	}


	// // Create tree1 entities
	// for (int i = 0; i < 100; i++) {s
	// 	Entity* en = entity_create();
	// 	setup_tree1(en);
	// 	en->pos = v2(get_random_float32_in_range(-200, 200), get_random_float32_in_range(-200, 200));
	// }


	// Player variables
	float player_speed = 50.0;			// pixels per second
	const float player_size_x = 10.0;	// player sprite size x (pixels)
	const float player_size_y = 10.0;	// player sprite size y (pixels)
	bool inventory_open = false;

	// World variables
	bool draw_grid = false;

	// Timing
	float64 seconds_counter = 0.0;
	s32 frame_count = 0;
	float64 last_time = os_get_current_time_in_seconds();


	float view_zoom = 0.1875; 			// view zoom ratio x (pixelart layer width / window width = 240 / 1280 = 0.1875)
	Vector2 camera_pos = v2(0, 0);


// ----- MAIN LOOP ----------------------------------------------------------------------------------------- 
	while (!window.should_close) {
		reset_temporary_storage();

		// reset world_frame
		world_frame = (WorldFrame){0};

		// :Timing
		float64 now = os_get_current_time_in_seconds();
		float64 delta_t = now - last_time;
		last_time = now;
		os_update(); 

		// make the viewport (or whatever) to window size, instead of -1.7, 1.7, -1, 1
		draw_frame.projection = m4_make_orthographic_projection(window.width * -0.5, window.width * 0.5, window.height * -0.5, window.height * 0.5, -1, 10);
		// draw_frame.enable_z_sorting = true;	//:testing

		
		{	// :camera
			Vector2 target_pos = player_en->pos;
			animate_v2_to_target(&camera_pos, target_pos, delta_t, 10.0f); // 4th value controls how smooth the camera transition is to the player

			draw_frame.view = m4_make_scale(v3(1.0, 1.0, 1.0)); // View zoom (zooms so pixel art is the correct size)
			draw_frame.view = m4_mul(draw_frame.view, m4_make_translation(v3(camera_pos.x, camera_pos.y, 0)));
			draw_frame.view = m4_mul(draw_frame.view, m4_make_scale(v3(view_zoom, view_zoom, 1.0)));
			
		}

		Vector2 mouse_pos_world = screen_to_world();
		int mouse_tile_x = world_pos_to_tile_pos(mouse_pos_world.x);
		int mouse_tile_y = world_pos_to_tile_pos(mouse_pos_world.y);
		
		
		// :Entity selection
		{	
			// log("%f, %f", input_frame.mouse_x, input_frame.mouse_y);
			// draw_text(font, sprint(temp, STR("%.0f, %.0f"), pos.x, pos.y), font_height, pos, v2(0.1, 0.1), COLOR_RED);

			float smallest_dist = INFINITY;


			for (int i = 0; i < MAX_ENTITY_COUNT; i++){
				Entity* en = &world->entities[i];
				if (en->is_valid && en->destroyable){
					Sprite* sprite = get_sprite(en->sprite_id);

					int entity_tile_x = world_pos_to_tile_pos(en->pos.x);
					int entity_tile_y = world_pos_to_tile_pos(en->pos.y);

					float dist = fabsf(v2_dist(en->pos, mouse_pos_world));

					if (dist < entity_selection_radius) {
						if (!world_frame.selected_entity || (dist < smallest_dist)) {
							world_frame.selected_entity = en;
							// smallest_dist = dist; // imo entity selection works better with this line commented
						}
					}
				}
			}
		}
				
		// :Render grid (:Grid)
		if (draw_grid)
		{	
			// NOTE: rendering tiles has a big fkin impact on fps 
			int player_tile_x = world_pos_to_tile_pos(player_en->pos.x);
			int player_tile_y = world_pos_to_tile_pos(player_en->pos.y);
			int tile_radius_x = 18;
			int tile_radius_y = 10;

			for (int x = player_tile_x - tile_radius_x; x < player_tile_x + tile_radius_x; x++){
				for (int y = player_tile_y - tile_radius_y; y < player_tile_y + tile_radius_y; y++){
					if ((x +(y % 2 == 0) ) % 2 == 0) {
						Vector4 col = v4(0.1, 0.1, 0.1, 0.1);
						float x_pos = x * tile_width;
						float y_pos = y * tile_width;
						if (x == mouse_tile_x && y == mouse_tile_y) {
							// Change tile under mouse pos to a different color
							// col.a = 0.5;
						}
						// Draw Tile Grid
						draw_rect(v2(x_pos + tile_width * -0.5, y_pos + tile_width * -0.5), v2(tile_width, tile_width), col);
					}
				}
			}
			// Draw different tile on mouse pos
			// draw_rect(v2(tile_pos_to_world_pos(mouse_tile_x) + tile_width * -0.5, tile_pos_to_world_pos(mouse_tile_y) + tile_width * -0.5), v2(tile_width, tile_width), v4(0.5, 0.5, 0.5, 0.5)); //hex_to_rgba(0x406438ff)
		}


		// :update entities
		{
			for (int i = 0; i < MAX_ENTITY_COUNT; i++) {
				Entity* en = &world->entities[i];
				if (en->is_valid) {

					// item pick up
					if (en->is_item) {
						// TODO PHYSICS

						if (fabsf(v2_dist(en->pos, player_en->pos)) < player_pickup_radius) {
							world->inventory_items[en->arch].amount += 1;
							entity_destroy(en);
						}
					}
				}
			}
		}

		// :player use/attack (clicky)
		{
			Entity* selected_en = world_frame.selected_entity;

			if (is_key_just_pressed(MOUSE_BUTTON_LEFT)) {
				if (selected_en) {
					consume_key_just_pressed(MOUSE_BUTTON_LEFT);
					selected_en->health -= 1;
					if (selected_en->health <= 0) {

						switch (selected_en->arch) {
							case arch_tree: {
								{
									Entity* en = entity_create();
									setup_item_pine_wood(en);
									en->pos = selected_en->pos;
								}
							} break;

							case arch_rock: {
								{
									Entity* en = entity_create();
									setup_item_rock(en);
									en->pos = selected_en->pos;
								}
							} break;

							case arch_bush: {
								{
									Entity* en = entity_create();
									setup_item_berry(en);
									en->pos = selected_en->pos;
								}
							} break;

							default: { } break;
						}

						entity_destroy(selected_en);
					}
				}
			}
		}

		// :Render entities
		for (int i = 0; i < MAX_ENTITY_COUNT; i++)  {
			Entity* en = &world->entities[i];
			if (en->is_valid) {
				switch (en->arch) {

					// case arch_player:

					default: 
					{
						Sprite* sprite = get_sprite(en->sprite_id);
						Matrix4 xform = m4_scalar(1.0);
						
						// ITEM
						if (en->is_item) {
							xform         = m4_translate(xform, v3(0, 2.0 * sin_breathe(os_get_current_time_in_seconds(), 5.0), 0)); // bob item up and down
							
							// shadow position
							Vector2 position = en->pos;
							position.x -= 3.5;
							position.y -= 3.5;
							
							// item shadow
							// draw_circle(position, v2(7.0, 7.0), v4(0.1, 0.1, 0.1, 0.5));
							draw_circle(position, v2(7.0, 7.0), item_shadow_color);

						}
						
						// SPRITE
						if (!en->is_item) {
							xform         = m4_translate(xform, v3(0, tile_width * -0.5, 0));			// bring sprite down to bottom of Tile if not item
						}

						xform         = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
						xform         = m4_translate(xform, v3(sprite->image->width * -0.5, 0.0, 0));

						Vector4 col = COLOR_WHITE;
						if (world_frame.selected_entity == en){
							col = v4(0.7, 0.7, 0.7, 1.0);
						}

						draw_image_xform(sprite->image, xform, get_sprite_size(sprite), col);

						if (IS_DEBUG){
							// draw_text(font, sprint(temp_allocator, STR("%.0f, %.0f"), en->pos.x, en->pos.y), 40, en->pos, v2(0.1, 0.1), COLOR_WHITE);
						}

						break;
					}
				}
			}
		}




		// inventory render test (randy's solution #2) (NOT centered inventory - status: WORKING)
		// :Render UI
		{
			float width = 240.0;
			float height = 135.0;

			draw_frame.view = m4_scalar(1.0);
			draw_frame.projection = m4_make_orthographic_projection(0.0, width, 0.0, height, -1, 10);

			// :Render Inventory

			// open inventory
			if (is_key_just_pressed(KEY_TAB)) {
				consume_key_just_pressed(KEY_TAB);
				world->ux_state = (world->ux_state == UX_inventory ? UX_nil : UX_inventory);
			}


			world->inventory_alpha_target = (world->ux_state == UX_inventory ? 1.0 : 0.0);
			animate_f32_to_target(&world->inventory_alpha, world->inventory_alpha_target, delta_t, 15.0);
			bool is_inventory_enabled = world->inventory_alpha_target == 1.0;
			// if (world->inventory_alpha != 0.0)
			if (world->inventory_alpha_target != 0.0) // temp line for instant opening of the inventory, since global draw frame alpha is not a thing (yet)
			{

				// TODO: some opacity thing here (by: randy)

				float pos_y = 95.0;

				// Colors
				Vector4 icon_background_col = v4(1.0, 1.0, 1.0, 0.2);
				Vector4 inventory_bg = v4(0.0, 0.0, 0.0, 0.4);
				Vector4 tooltip_bg = inventory_bg;
				
				float inventory_tile_size = 8.0;
				float padding = 2.0;
				const int icon_row_count = 8;

				// float icon_width = inventory_tile_size * padding;
				float icon_width = inventory_tile_size;
				float box_width = icon_row_count * icon_width;
				float x_start_pos = (width / 2.0) - (box_width / 2.0);

				// get how many different items in inventory
				int item_count = 0;
				for (int i = 0; i < ARCH_MAX; i++) {
					ItemData* item = &world->inventory_items[i];
					if (item->amount > 0){
						item_count += 1;
						// printf("ITEM COUNT = %d", item_count);
					}
				}

				// inventory background_box rendering
				{
					Matrix4 xform = m4_identity;
					xform = m4_translate(xform, v3(x_start_pos, pos_y, 0.0));
					draw_rect_xform(xform, v2(box_width, icon_width + padding), inventory_bg);
				}

				// inventory item rendering
				int slot_index = 0;
				for (int archetype_num = 0; archetype_num < ARCH_MAX; archetype_num++) {
					ItemData* item = &world->inventory_items[archetype_num];
					if (item->amount > 0){

						float slot_index_offset = slot_index * icon_width;

						Matrix4 xform = m4_scalar(1.0);
						float pos_x = (padding * 0.5) + x_start_pos + slot_index_offset + (padding * 0.5) * slot_index;
						xform = m4_translate(xform, v3(pos_x, pos_y + (padding * 0.5), 0));

						Sprite* sprite = get_sprite(get_sprite_id_from_archetype(archetype_num));
						
						// draw icon background
						// draw_rect_xform(xform, v2(inventory_tile_size, inventory_tile_size), icon_background_col);

						float is_selected_alpha = 0.0;

						Draw_Quad* quad = draw_rect_xform(xform, v2(8, 8), v4(1,1,1,0));
						Range2f icon_box = quad_to_range(*quad);
						if (is_inventory_enabled && range2f_contains(icon_box, get_mouse_pos_in_ndc())) {
								is_selected_alpha = true;
							}

						// save xfrom for later
						Matrix4 box_bottom_right_xform = xform;

						// center sprite
						xform = m4_translate(xform, v3(icon_width * 0.5, icon_width * 0.5, 0));


						// Item selected (HOVERING)
						if (is_selected_alpha){
							// Scale item
							// float scale_adjust = 0.5 * sin_breathe(os_get_current_time_in_seconds(), 5.0);
							float scale_adjust = 1.5;
							xform = m4_scale(xform, v3(scale_adjust, scale_adjust, 1));


						}

						// item bobbing
						// {
						// 	float scale_adjust = 0.1 * sin_breathe(os_get_current_time_in_seconds(), 5.0);
						// 	xform = m4_scale(xform, v3(1 + scale_adjust, 1 + scale_adjust, 1));

						// }

						// rotation
						// {
						// 	float rotation_adjust = 0.25 * PI32 * sin_breathe(os_get_current_time_in_seconds(), 1.0);
						// 	xform = m4_rotate_z(xform, rotation_adjust);

						// }


						xform = m4_translate(xform, v3(get_sprite_size(sprite).x * -0.5, get_sprite_size(sprite).y * -0.5, 0));


						// draw sprite
						draw_image_xform(sprite->image, xform, get_sprite_size(sprite), COLOR_WHITE);

						// draw_text(font, sprint(temp_allocator, STR("%d"), item->amount), 40, v2(pos_x, pos_y), v2(0.1, 0.1), COLOR_WHITE);
						draw_text_xform(font, sprint(temp_allocator, STR("%d"), item->amount), 40, box_bottom_right_xform, v2(0.1, 0.1), COLOR_WHITE);	// randy's solution

						// tooltip
						{
							if (is_selected_alpha){
								
								Draw_Quad screen_quad = ndc_quad_to_screen_quad(*quad);
								Range2f screen_range = quad_to_range(screen_quad);
								Vector2 icon_center = range2f_get_center(screen_range);

								// icon_center
								Vector2 tooltip_box_size = v2(40, 20);

								Matrix4 xform = m4_scalar(1.0);
								xform = m4_translate(xform, v3(tooltip_box_size.x * -0.5, -tooltip_box_size.y - icon_width * 0.5 - padding * 0.5, 0));
								xform = m4_translate(xform, v3(icon_center.x, icon_center.y, 0));

								// tooltip bg box
								draw_rect_xform(xform, tooltip_box_size, tooltip_bg);

								// string txt = STR("%s");
								string title = sprint(temp_allocator, STR("%s"), get_archetype_name(archetype_num));

								// draw text
								Gfx_Text_Metrics metrics = measure_text(font, title, font_height, v2(0.1, 0.1));
								Vector2 draw_pos = icon_center;
								draw_pos = v2_sub(draw_pos, metrics.visual_pos_min);
								draw_pos = v2_add(draw_pos, v2_mul(metrics.visual_size, v2(-0.5, -1.0))); // TOP CENTER

								draw_pos = v2_add(draw_pos, v2(0, icon_width * -0.5));
								draw_pos = v2_add(draw_pos, v2(0, -2.0)); // padding

								draw_text(font, title, font_height, draw_pos, v2(0.1, 0.1), COLOR_WHITE);

							}
						}

						slot_index += 1;
					}
				}
			}


			// :Render hotbar
			if (IS_DEBUG)
			{
				// window size (figure out a better way of getting these)
				float width = 240.0;
				float height = 135.0;

				float slot_size = 8.0;
				float slot_index = 1;
				int padding = 15.0;

				Vector2 hotbar_box_size = v2(80, 10);
				Vector2 hotbar_tile = v2(8, 8);

				float x_start_pos = (width * 0.5) - (hotbar_box_size.x * 0.5);

				draw_frame.view = m4_scalar(1.0);
				draw_frame.projection = m4_make_orthographic_projection(0.0, width, 0.0, height, -1, 10);

				// Colors
				Vector4 hotbar_border_color = v4(1.0, 1.0, 1.0, 0.2);
				Vector4 hotbar_bg_color = v4(0.0, 0.0, 0.0, 0.4);
				


				// Draw hotbar background
				// Matrix4 xform = m4_identity;
				// xform = m4_translate(xform, v3((width * 0.5) - (hotbar_box_size.x * 0.5), 2, 0.0));
				// draw_rect_xform(xform, v2(hotbar_box_size.x, hotbar_box_size.y), hotbar_bg_color);

				float pos_y = 2;
				Vector4 col = v4(1,1,1,1);
				for (int i = 0; i <= 9; i++) {

					// float pos_x = (width * 0.5) - (hotbar_box_size.x * 0.5);
					float pos_x = x_start_pos;
					pos_x += (slot_size * i);
					pos_x += padding;

					Matrix4 xform = m4_identity;
					xform = m4_translate(xform, v3(pos_x, pos_y, 0.0));
					draw_rect_xform(xform, v2(slot_size, slot_size), col);

					col = v4_add(col, v4(-0.1, -0.1, -0.1, 0));

				}

			}
		}

/*
		// // :inventory render test (randy's solution #1) (centered inventory - status: WORKING nicely)
		// {
		// 	if (inventory_open){
		// 	float width = 240.0;
		// 	float height = 135.0;

		// 	draw_frame.view = m4_scalar(1.0);
		// 	draw_frame.projection = m4_make_orthographic_projection(0.0, width, 0.0, height, -1, 10);


		// 	int item_count = 0;
		// 	for (int i = 0; i < ARCH_MAX; i++) {
		// 		ItemData* item = &world->inventory_items[i];
		// 		if (item->amount > 0){
		// 			item_count += 1;
		// 		}
		// 	}

		// 	float inventory_tile_size = 8.0;
		// 	float padding = 2.0;
		// 	float icon_width = inventory_tile_size * padding;

		// 	float box_width = item_count * icon_width;
		// 	float x_start_pos = (width - box_width) / 2.0;
		// 	// float x_start_pos = (width / 2.0) - (box_width / 2.0) + (icon_width * 0.5);

		// 	int slot_index = 0;
		// 	for (int i = 0; i < ARCH_MAX; i++) {
		// 		ItemData* item = &world->inventory_items[i];
		// 		if (item->amount > 0){

		// 			float slot_index_offset = slot_index * icon_width;

		// 			Matrix4 xform = m4_scalar(1.0);
		// 			xform = m4_translate(xform, v3(x_start_pos + slot_index_offset, pos_y, 0));
		// 			xform = m4_translate(xform, v3(4, 4, 0));
		// 			draw_rect_xform(xform, v2(inventory_tile_size, inventory_tile_size), COLOR_WHITE);

		// 			Sprite* sprite = get_sprite(get_sprite_id_from_archetype(i));

		// 			draw_image_xform(sprite->image, xform, get_sprite_size(sprite), COLOR_WHITE);

		// 			slot_index += 1;
		// 		}
		// 	}
		// 	}
		// }



		// inventory render test (my solution)
		{
			if (IS_DEBUG){
				int size_x = 100;
				int size_y = 40;
				float pos_x = camera_pos.x - (0.5*size_x);
				float pos_y = camera_pos.y + size_y * 0.5;

				// draw inventory
				draw_rect(v2(pos_x, pos_y) , v2(size_x, size_y), v4(0, 0, 0, 0.3));

				// draw item
				for (int i = 0; i < MAX_ENTITY_COUNT; i++)  {
					// Entity* en = &world->entities[i];
					ItemData* item = &world->inventory_items[i];
					// if (en->is_valid && en->is_item) {
					if (item->amount > 0) {
						Sprite* sprite = get_sprite(en->sprite_id);
						Matrix4 xform = m4_scalar(1.0);
						xform         = m4_translate(xform, v3(pos_x, pos_y, 0));
						xform         = m4_translate(xform, v3(sprite->image->width * -0.5, 0.0, 0));
						draw_image_xform(sprite->image, xform, get_sprite_size(sprite), COLOR_WHITE);
						// draw_text(font, sprint(temp_allocator, STR("%i"), i), 40, v2((i * 2) + pos_x, pos_y), v2(0.1, 0.1), v4(1,1,1,1));
						
						
					// ItemData item = world->inventory_items[i];
					// if (item.amount > 0){
					// 	draw_text(font, sprint(temp_allocator, STR("%d"), i), 40, v2((i * 2) + pos_x, pos_y), v2(0.1, 0.1), v4(1,1,1,1));
					}

				}

			}
		}
		

*/




		// :Input
		if (is_key_just_pressed(KEY_ESCAPE)){ // EXIT
			window.should_close = true;
		}
		if (is_key_just_pressed('Q')){
			if (IS_DEBUG){
				IS_DEBUG = false;
			}
			else{
				IS_DEBUG = true;
			}
		}


		// if (is_key_just_pressed(KEY_TAB)) {
		// 	if (!inventory_open) {
		// 		inventory_open = true;
		// 	}
		// 	else{
		// 		inventory_open = false;
		// 	}
		// }



		// Mousewheel ZOOM (debug for now)
		if (IS_DEBUG){
			for (u64 i = 0; i < input_frame.number_of_events; i++) {
				Input_Event e = input_frame.events[i];
				switch (e.kind) {
					case (INPUT_EVENT_SCROLL):
					{
						if (e.yscroll > 0){
							view_zoom -= 0.01;
						}
						else{
							view_zoom += 0.01;
						}
						break;
					}
					case (INPUT_EVENT_KEY):
					{
						break;
					}
					case (INPUT_EVENT_TEXT):
					{
						break;
					}
				}
			}
		}


		// :Sprint
		if (is_key_down(KEY_SHIFT)){
			player_speed = 100.0;
		}
		else{
			player_speed = 50.0;
		}

		// Player movement
		Vector2 input_axis = v2(0, 0);
		if (is_key_down('A')){
			input_axis.x -= 1.0;
		}
		if (is_key_down('D')){
			input_axis.x += 1.0;
		}
		if (is_key_down('S')){
			input_axis.y -= 1.0;
		}
		if (is_key_down('W')){
			input_axis.y += 1.0;
		}

		// bugfix (diagonal speed)
		input_axis = v2_normalize(input_axis);

		// player_pos = player_pos + (input_axis * 10.0);
		player_en->pos = v2_add(player_en->pos, v2_mulf(input_axis, player_speed * delta_t));

		gfx_update();

		// :Timing
		seconds_counter += delta_t;
		frame_count += 1;
		if (seconds_counter > 1.0){
			log("fps: %i", frame_count);
			seconds_counter = 0.0;
			frame_count = 0;
		}
	}

	return 0;
}