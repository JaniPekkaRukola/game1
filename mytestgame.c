float entity_selection_radius = 10.0f;
const int tile_width = 8;

const int player_health = 3;
const int rock_health = 3;
const int tree_health = 3;


// inline float v2_length(Vector2 a) {
//     return sqrt(a.x * a.x + a.y * a.y);
// }
// randy: is this something that's usually standard in math libraries or am I tripping?
inline float v2_dist(Vector2 a, Vector2 b) {
    return v2_length(v2_sub(a, b));
}

#define m4_identity m4_make_scale(v3(1, 1, 1))

Vector2 range2f_get_center(Range2f r) {
	return (Vector2) { (r.max.x - r.min.x) * 0.5 + r.min.x, (r.max.y - r.min.y) * 0.5 + r.min.y };
}

// ^^^ engine changes


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

	// Items
	SPRITE_item_rock,
	SPRITE_item_pine_wood,

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
	return (Vector2) { sprite->image->width, sprite->image->height };
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
	arch_player = 3,

	arch_item_rock = 4,
	arch_item_pine_wood = 5,
	ARCH_MAX,

} EntityArchetype;

// :entity
typedef struct Entity {
	bool is_valid;
	EntityArchetype arch;
	Vector2 pos;
	bool render_sprite;
	SpriteID sprite_id;
	int health;
	bool destroyable;
} Entity;

#define MAX_ENTITY_COUNT 1024

typedef struct World {
	Entity entities[MAX_ENTITY_COUNT];
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


// Entity setups
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

// Item setups
void setup_item_rock(Entity* en) {
	en->arch = arch_item_rock;
	en->sprite_id = SPRITE_item_rock;
}

void setup_item_pine_wood(Entity* en) {
	en->arch = arch_item_pine_wood;
	en->sprite_id = SPRITE_item_pine_wood;
}

// void setup_tree1(Entity* en) {
// 	en->arch = arch_tree;
// 	en->sprite_id = SPRITE_tree1;
// 	// en->sprite_id = SPRITE_tree1;
// }


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
	window.x = 200;
	window.y = 200;
	window.clear_color = hex_to_rgba(0x43693aff);

	// Memory
	world = alloc(get_heap_allocator(), sizeof(World));
	memset(world, 0, sizeof(World));

	// Sprites
	sprites[SPRITE_player] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/player.png"), get_heap_allocator())};
	sprites[SPRITE_tree0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tree0.png"), get_heap_allocator())};
	sprites[SPRITE_tree1] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tree1.png"), get_heap_allocator())};
	sprites[SPRITE_rock0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/rock0.png"), get_heap_allocator())};

	// Items
	sprites[SPRITE_item_rock] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_rock.png"), get_heap_allocator())};
	sprites[SPRITE_item_pine_wood] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_pine_wood.png"), get_heap_allocator())};

	// Font
	Gfx_Font *font = load_font_from_disk(STR("C:/windows/fonts/arial.ttf"), get_heap_allocator());
	assert(font, "Failed loading arial.ttf, %d", GetLastError());
	const u32 font_height = 48;


	// :Create entities

	// Create player entity
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
	for (int i = 0; i < 100; i++) {
		Entity* en = entity_create();
		setup_tree(en);
		en->pos = v2(get_random_float32_in_range(-200, 200), get_random_float32_in_range(-200, 200));
		en->pos = round_v2_to_tile(en->pos);
		// en->pos.y -= tile_width * 0.5;	// bring sprite down to bottom of Tile (now done in sprite renderer)
	}	


	// // Create tree1 entities
	// for (int i = 0; i < 100; i++) {
	// 	Entity* en = entity_create();
	// 	setup_tree1(en);
	// 	en->pos = v2(get_random_float32_in_range(-200, 200), get_random_float32_in_range(-200, 200));
	// }


	// Player variables
	float player_speed = 50.0;			// pixels per second
	const float player_size_x = 10.0;	// player sprite size x (pixels)
	const float player_size_y = 10.0;	// player sprite size y (pixels)

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

		// : Timing
		float64 now = os_get_current_time_in_seconds();
		float64 delta_t = now - last_time;
		last_time = now;
		os_update(); 

		// make the viewport (or whatever) to window size, instead of -1.7, 1.7, -1, 1
		draw_frame.projection = m4_make_orthographic_projection(window.width * -0.5, window.width * 0.5, window.height * -0.5, window.height * 0.5, -1, 10);


		
		{	// : camera
			Vector2 target_pos = player_en->pos;
			animate_v2_to_target(&camera_pos, target_pos, delta_t, 10.0f); // 4th value controls how smooth the camera transition is to the player

			draw_frame.view = m4_make_scale(v3(1.0, 1.0, 1.0)); // View zoom (zooms so pixel art is the correct size)
			draw_frame.view = m4_mul(draw_frame.view, m4_make_translation(v3(camera_pos.x, camera_pos.y, 0)));
			draw_frame.view = m4_mul(draw_frame.view, m4_make_scale(v3(view_zoom, view_zoom, 1.0)));
				
		}

		Vector2 mouse_pos_world = screen_to_world();
		int mouse_tile_x = world_pos_to_tile_pos(mouse_pos_world.x);
		int mouse_tile_y = world_pos_to_tile_pos(mouse_pos_world.y);
		
		
		
		{	// :Entity selection

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
				

		{	// : Tile-Grid Render
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

							default: { } break;
						}

						entity_destroy(selected_en);
					}
				}
			}
		}

		// : Entity Render
		for (int i = 0; i < MAX_ENTITY_COUNT; i++)  {
			Entity* en = &world->entities[i];
			if (en->is_valid) {
				switch (en->arch) {

					// case arch_player:

					default: 
					{
						Sprite* sprite = get_sprite(en->sprite_id);
						Matrix4 xform = m4_scalar(1.0);
						xform         = m4_translate(xform, v3(0, (float)tile_width * -0.5, 0));	// bring sprite down to bottom of Tile
						xform         = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
						xform         = m4_translate(xform, v3(sprite->image->width * -0.5, 0.0, 0));

						Vector4 col = COLOR_WHITE;
						if (world_frame.selected_entity == en){
							col = v4(0.7, 0.7, 0.7, 1.0);
						}

						draw_image_xform(sprite->image, xform, get_sprite_size(sprite), col);

						if (IS_DEBUG){
							draw_text(font, sprint(temp_allocator, STR("%.0f, %.0f"), en->pos.x, en->pos.y), 40, en->pos, v2(0.1, 0.1), COLOR_WHITE);
						}

						break;
					}
				}
			}
		}


		// : Input
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


		if (is_key_just_pressed(MOUSE_FIRST)){
			printf("TEST");
		}



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


		// Sprint
		if (is_key_down(KEY_SHIFT)){
			player_speed = 100.0;
		}
		else{
			player_speed = 50.0;
		}

		// Key input
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

		// Timing
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