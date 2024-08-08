// ----- ::Settings || ::Tweaks || ::Global --------------------------------------------------------|

bool IS_DEBUG = false;

float entity_selection_radius = 5.0f;
const int player_pickup_radius = 15.0;
int player_health = 3;
const int rock_health = 1;
const int tree_health = 1;
const int bush_health = 1;
const int tile_width = 8;

// int icon_row_index = 1;

const Vector4 item_shadow_color = {0, 0, 0, 0.1};

// rendering layers
const s32 layer_ui = 20;
const s32 layer_world = 10;

// Global app stuff
float64 delta_t;
Gfx_Font* font;
u32 font_height = 48;
float screen_width = 240.0;
float screen_height = 135.0;
int selected_slot = 0;

// ----- engine changes (by: randy) ----------------------------------------------------------------|

#define m4_identity m4_make_scale(v3(1, 1, 1))

// "resizable" array helpers (by: randy)
// void* array_add() {}


// inline float v2_length(Vector2 a) {
//     return sqrt(a.x * a.x + a.y * a.y);
// }



// Yoinked Range.c stuff
typedef struct Range1f {
  float min;
  float max;
} Range1f;
// ...

typedef struct Range2f {
  Vector2 min;
  Vector2 max;
} Range2f;
inline Range2f range2f_make(Vector2 min, Vector2 max) { return (Range2f) { min, max }; }

Range2f range2f_shift(Range2f r, Vector2 shift) {
  r.min = v2_add(r.min, shift);
  r.max = v2_add(r.max, shift);
  return r;
}

Range2f range2f_make_bottom_center(Vector2 size) {
  Range2f range = {0};
  range.max = size;
  range = range2f_shift(range, v2(size.x * -0.5, 0.0));
  return range;
}

Vector2 range2f_size(Range2f range) {
  Vector2 size = {0};
  size = v2_sub(range.min, range.max);
  size.x = fabsf(size.x);
  size.y = fabsf(size.y);
  return size;
}

bool range2f_contains(Range2f range, Vector2 v) {
  return v.x >= range.min.x && v.x <= range.max.x && v.y >= range.min.y && v.y <= range.max.y;
}




// randy: is this something that's usually standard in math libraries or am I tripping?
inline float v2_dist(Vector2 a, Vector2 b) {
    return v2_length(v2_sub(a, b));
}


Vector2 range2f_get_center(Range2f range) {
	return (Vector2) {(range.max.x - range.min.x) * 0.5 + range.min.x, (range.max.y - range.min.y) * 0.5 + range.min.y};
}

// ----- the scuff zone (by: randy) --------------------------------------------------------------------|

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




// ----- funky -----------------------------------------------------------------------------------------|

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

// ::SPRITE ---------------------------|
typedef struct Sprite {
	Gfx_Image* image;
} Sprite;

typedef enum SpriteID {
	SPRITE_nil,

	// Entities
	SPRITE_player,
	SPRITE_tree0,
	SPRITE_tree1,
	SPRITE_tree2,
	SPRITE_tree3,
	SPRITE_rock0,
	SPRITE_rock1,
	SPRITE_rock2,
	SPRITE_rock3,
	SPRITE_bush0,
	SPRITE_tall_grass0,
	SPRITE_tall_grass1,
	SPRITE_mushroom0,

	// Items
	SPRITE_item_rock,
	SPRITE_item_pine_wood,
	SPRITE_item_sprout,
	SPRITE_item_berry,
	SPRITE_item_twig,

	// tools
	SPRITE_tool_pickaxe,

	// fossils
	SPRITE_item_fossil0,
	SPRITE_item_fossil1,
	SPRITE_item_fossil2,



	// buildings
	SPRITE_building_furnace,
	SPRITE_building_workbench,
	SPRITE_building_chest,

	SPRITE_MAX,
} SpriteID;

Sprite sprites[SPRITE_MAX];

Sprite* get_sprite(SpriteID id) {
	if (id >= 0 && id < SPRITE_MAX) {
		Sprite* sprite = &sprites[id];
		if (sprite->image) {
			return sprite;
		} else {
			return &sprites[0];
		}
	}
	return &sprites[0];
}

Vector2 get_sprite_size(Sprite* sprite) {
	return (Vector2) {sprite->image->width, sprite->image->height};
}

// ::ITEM ID -------------------------------|
typedef enum ItemID {
	ITEM_nil,
	ITEM_rock,
	ITEM_pine_wood,
	ITEM_sprout,
	ITEM_berry,
	ITEM_mushroom0,
	ITEM_twig,
	ITEM_furnace,
	ITEM_workbench,
	ITEM_chest,
	ITEM_fossil0,
	ITEM_fossil1,
	ITEM_fossil2,
	ITEM_fossil3,
	ITEM_MAX,
} ItemID;

typedef struct InventoryItemData {
	int amount;
} InventoryItemData;



// ::ENTITY --------------------------------|
typedef enum EntityArchetype {
	ARCH_nil = 0,
	ARCH_rock = 1,
	ARCH_tree = 2,
	ARCH_bush = 3,
	ARCH_twig = 4,
	ARCH_player = 5,

	ARCH_item = 6,
	ARCH_tool = 7,
	ARCH_building = 8,

	ARCH_mushroom = 9,
/*
	// items
	// ARCH_item_rock = 6,
	// ARCH_item_pine_wood = 7,
	// ARCH_item_sprout = 8,
	// ARCH_item_berry = 9,
	// ARCH_item_twig = 10,

	// buildings
	// ARCH_furnace = 11,
	// ARCH_item_furnace = 12,
	// ARCH_workbench = 13,
	// ARCH_item_workbench = 14,
	// ARCH_chest = 15,
	// ARCH_item_chest = 16,

	// ARCH_item_fossil0 = 17,
	// ARCH_item_fossil1 = 18,
	// ARCH_item_fossil2 = 19,
*/
	ARCH_MAX,
} EntityArchetype;

typedef struct Entity {
	bool is_valid;
	EntityArchetype arch;
	ItemID item_id;
	Vector2 pos;
	bool render_sprite;
	SpriteID sprite_id;
	int health;
	bool destroyable;
	bool is_item;
} Entity;

#define MAX_ENTITY_COUNT 1024


typedef struct ItemData {
	string pretty_name;
	// recipe
	EntityArchetype for_structure;
	ItemID output;
	ItemID input[8];
} ItemData;
ItemData item_data[ITEM_MAX];
ItemData get_item_data(ItemID id) {
	return item_data[id];
}


// ::BUILDINGS -----------------------------|
// NOTE: randy: a 'resource' is a thing that we set up during startup, and is constant.
typedef enum BuildingID {
	BUILDING_nil,
	BUILDING_furnace,	
	BUILDING_workbench,
	BUILDING_chest,
	BUILDING_MAX,
} BuildingID;

typedef struct BuildingData {
	EntityArchetype to_build;
	SpriteID icon;
	// BuildingID id;
	// display name
	// cost
	// health
	// etc
} BuildingData;

BuildingData buildings[BUILDING_MAX];

BuildingData get_building_data(BuildingID id) {
	return buildings[id];
}



string get_archetype_pretty_name(EntityArchetype arch) {
	switch (arch) {
		// case ARCH_furnace: return STR("Furnace");
		case ARCH_building: return STR("building");
		// :arch
		default: return STR("nil");
	}
}



// 
	// ::ITEMDATA ------------------------------|
	// typedef struct ItemData {
	// 	int amount;
	// 	string name;
	// 	string tooltip;
	// } ItemData;


	// ItemID items[ITEM_MAX];
	// ItemID* get_item(ItemID id) {
	// 	if (id >= 0 && id < ITEM_MAX) {
	// 		return &items[id];
	// 	}
	// 	return &items[0];
	// }



SpriteID get_sprite_id_from_item(ItemID item) {
	switch (item) {
		case ITEM_pine_wood: return SPRITE_item_pine_wood; break;
		case ITEM_rock: return SPRITE_item_rock; break;
		case ITEM_twig: return SPRITE_item_twig; break;
		case ITEM_sprout: return SPRITE_item_sprout; break;
		case ITEM_berry: return SPRITE_item_berry; break;
		case ITEM_mushroom0: return SPRITE_mushroom0; break;
		case ITEM_furnace: return SPRITE_building_furnace; break;
		case ITEM_workbench: return SPRITE_building_workbench; break;
		case ITEM_chest: return SPRITE_building_chest; break;
		case ITEM_fossil0: return SPRITE_item_fossil0; break;
		case ITEM_fossil1: return SPRITE_item_fossil1; break;
		case ITEM_fossil2: return SPRITE_item_fossil2; break;
		default: return 0;
	}
}


// ::TOOL DATA || ::TOOL ID -----------------------------|
typedef struct ToolData {
	string name;
	string tooltip;
	int durability;
	int miningLevel;
} ToolData;

typedef enum ToolID {
	TOOL_nil,
	TOOL_pickaxe,
	TOOL_axe,

	TOOL_MAX,
}ToolID;



// ::UX ------------------------------------|
typedef enum UXState {
	UX_nil,
	UX_inventory,
	UX_building,
	UX_place_mode,
	// UX_map,
	// UX_settings,
	// UX_menu,
} UXState;


// ::WORLD ---------------------------------|
typedef struct World {
	Entity entities[MAX_ENTITY_COUNT];
	// ItemData inventory_items[ARCH_MAX];
	InventoryItemData inventory_items[ITEM_MAX];
	UXState ux_state;
	float inventory_alpha;
	float inventory_alpha_target;
	float building_menu_alpha;
	float building_menu_alpha_target;
	BuildingID placing_building;
} World;

World* world = 0;

typedef struct WorldFrame {
	Entity* selected_entity;
	Matrix4 world_projection;
	Matrix4 world_view;
	bool hover_consumed;
} WorldFrame;
WorldFrame world_frame;


// :ENTITY create & destroy ----------------|
// should prolly move these into ::ENTITY above
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


// SpriteID get_sprite_id_from_archetype(EntityArchetype arch) {
// 	switch (arch) {
// 		case ITEM_pine_wood: return SPRITE_item_pine_wood; break;
// 		case ITEM_rock: return SPRITE_item_rock; break;
// 		case ITEM_sprout: return SPRITE_item_sprout; break;
// 		case ITEM_berry: return SPRITE_item_berry; break;
// 		// case ARCH_twig: return SPRITE_item_twig; break;
// 		case ITEM_fossil0: return SPRITE_item_fossil0; break;
// 		case ITEM_fossil1: return SPRITE_item_fossil1; break;
// 		case ITEM_fossil2: return SPRITE_item_fossil2; break;
// 		case ARCH_tool: return SPRITE_tool_pickaxe; break;

// 		// buildings as items
// 		case ARCH_building: return SPRITE_nil; break;
// 		// case BUILDING_furnace: return SPRITE_building_furnace; break;
// 		// case BUILDING_workbench: return SPRITE_building_workbench; break;
// 		// case BUILDING_chest: return SPRITE_building_chest; break;
// 		default: return 0;
// 	}
// }


string get_archetype_name(EntityArchetype arch) {
	switch (arch) {
		case ITEM_pine_wood: return STR("Pine Wood");
		case ITEM_rock: return STR("Rock");
		case ITEM_sprout: return STR("Sprout");
		case ITEM_berry: return STR("Berry");
		case ITEM_mushroom0: return STR("Mushroom");
		// case ARCH_twig: return STR("Twig");
		case ITEM_fossil0: return STR("Ammonite Fossil");
		case ITEM_fossil1: return STR("Bone Fossil");
		case ITEM_fossil2: return STR("Fang Fossil");
		case ARCH_tool: return STR("Pickaxe");

		// building names
		// case BUILDING_furnace: return STR("Furnace");
		// case BUILDING_workbench: return STR("Workbench");
		// case BUILDING_chest: return STR("Chest");
		default: return STR("nill");
	}
}



// ----- ::SETUP entity --------------------------------------------------------------------------------|
// @reminder gotta remember to modify the range variale in "get_random_int_in_range" based on the amount of different variations per sprite
void setup_player(Entity* en) {
	en->arch = ARCH_player;
	en->sprite_id = SPRITE_player;
	en->health = player_health;
}

void setup_rock(Entity* en) {
	en->arch = ARCH_rock;
	int random = get_random_int_in_range(0,3);
	if (random == 0){en->sprite_id = SPRITE_rock0;}
	if (random == 1){en->sprite_id = SPRITE_rock1;}
	if (random == 2){en->sprite_id = SPRITE_rock2;}
	if (random == 3){en->sprite_id = SPRITE_rock3;}
	en->health = rock_health;
	en->destroyable = true;
}

void setup_tree(Entity* en) {
	en->arch = ARCH_tree;
	int random = get_random_int_in_range(0,3);
	if (random == 0){en->sprite_id = SPRITE_tree0;}
	if (random == 1){en->sprite_id = SPRITE_tree1;}
	if (random == 2){en->sprite_id = SPRITE_tree2;}
	if (random == 3){en->sprite_id = SPRITE_tree3;}
	en->health = tree_health;
	en->destroyable = true;
}

void setup_bush(Entity* en) {
	en->arch = ARCH_bush;
	en->sprite_id = SPRITE_bush0;
	en->health = bush_health;
	en->destroyable = true;
}

void setup_twig(Entity* en) {
	en->arch = ARCH_twig;
	en->sprite_id = SPRITE_item_twig;
	en->health = 1;
	en->destroyable = true;
}

void setup_mushroom(Entity* en) {
	en->arch = ARCH_mushroom;
	en->sprite_id = SPRITE_mushroom0;
	en->health = 1;
	en->destroyable = true;
}


// ----- ::SETUP item --------------------------------------------------------------------------------|

void setup_item(Entity* en, ItemID item_id) {
	en->arch = ARCH_item;
	en->sprite_id = get_sprite_id_from_item(item_id);
	en->is_item = true;
	en->item_id = item_id;
}
/*
	// void setup_item_rock(Entity* en) {
	// 	en->arch = ARCH_item_rock;
	// 	en->sprite_id = SPRITE_item_rock;
	// 	en->is_item = true;
	// }

	// void setup_item_pine_wood(Entity* en) {
	// 	en->arch = ARCH_item_pine_wood;
	// 	en->sprite_id = SPRITE_item_pine_wood;
	// 	en->is_item = true;
	// }

	// void setup_item_sprout(Entity* en) {
	// 	en->arch = ARCH_item_sprout;
	// 	en->sprite_id = SPRITE_item_sprout;
	// 	en->is_item = true;
	// }

	// void setup_item_berry(Entity* en) {
	// 	en->arch = ARCH_item_berry;
	// 	en->sprite_id = SPRITE_item_berry;
	// 	en->is_item = true;
	// }

	// void setup_item_twig(Entity* en) {
	// 	en->arch = ARCH_twig;
	// 	en->sprite_id = SPRITE_item_twig;
	// 	en->is_item = true;
	// }

	// void setup_item_fossil0(Entity* en) {
	// 	en->arch = ARCH_item_fossil0;
	// 	en->sprite_id = SPRITE_item_fossil0;
	// 	en->is_item = true;
	// }

	// void setup_item_fossil1(Entity* en) {
	// 	en->arch = ARCH_item_fossil1;
	// 	en->sprite_id = SPRITE_item_fossil1;
	// 	en->is_item = true;
	// }

	// void setup_item_fossil2(Entity* en) {
	// 	en->arch = ARCH_item_fossil2;
	// 	en->sprite_id = SPRITE_item_fossil2;
	// 	en->is_item = true;
	// }
*/
// ----- ::SETUP building -----------------------------------------------------------------------------|

void setup_item_furnace(Entity* en) {
	en->arch = ITEM_furnace;
	en->sprite_id = SPRITE_building_furnace;
	en->is_item = true;
}
void setup_item_workbench(Entity* en) {
	en->arch = ITEM_workbench;
	en->sprite_id = SPRITE_building_workbench;
	en->is_item = true;
}

void setup_item_chest(Entity* en) {
	en->arch = ITEM_chest;
	en->sprite_id = SPRITE_building_chest;
	en->is_item = true;
}


// entity setup automation
// void entity_setup(Entity* en, EntityArchetype id) {
// 	switch (id) {
// 		case ARCH_furnace: setup_furnace(en); break;
// 		case ARCH_workbench: setup_workbench(en); break;
// 		case ARCH_chest: setup_chest(en); break;
// 		default: log_error("Missing entity_setup case entry"); break;
// 	}
// }

// Building setup automation
void setup_building(Entity* en, BuildingID id) {
	switch (id) {
		case BUILDING_furnace: {
			{
				en->arch = ARCH_building;
				en->sprite_id = SPRITE_building_furnace;
				en->is_item = false;
				en->destroyable = true;
				en->health = 3;
			}
		} break;

		case BUILDING_workbench: {
			{
				en->arch = ARCH_building;
				en->sprite_id = SPRITE_building_workbench;
				en->is_item = false;
				en->destroyable = true;
				en->health = 3;
			}
		} break;
		case BUILDING_chest: {
			{
				en->arch = ARCH_building;
				en->sprite_id = SPRITE_building_chest;
				en->is_item = false;
				en->destroyable = true;
				en->health = 3;
			}
		} break;

		// case BUILDING_furnace: setup_furnace(en); break;
		// case BUILDING_workbench: setup_workbench(en); break;
		// case BUILDING_chest: setup_chest(en); break;
		default: log_error("Missing building_setup case entry"); break;
	}
}



// ----- coordinate stuff ------------------------------------------------------------------------------|

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


Vector2 get_mouse_pos_in_world_space() {
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

void set_screen_space() {
	draw_frame.view = m4_scalar(1.0);
	draw_frame.projection = m4_make_orthographic_projection(0.0, screen_width, 0.0, screen_height, -1, 10);
}

void set_world_space() {
	// make the viewport (or whatever) to window size, instead of -1.7, 1.7, -1, 1
	draw_frame.projection = world_frame.world_projection;
	draw_frame.view = world_frame.world_view;
}


// ----- ::FUNC Dump (name by randy) -------------------------------------------------------------------|

// inventory render (randy's solution #2) (NOT centered inventory - status: WORKING)
// :Render UI
void render_ui()
{
	set_screen_space();
	push_z_layer(layer_ui);
	

	// :Inventory UI || :Render Inventory UI

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

		float inventory_tile_size = 8.0;
		float padding = 2.0;
		const int max_icons_per_row = 9;
		const int max_rows = 4;

		// Colors
		Vector4 icon_background_col = v4(1.0, 1.0, 1.0, 0.2);
		Vector4 inventory_bg = v4(0.0, 0.0, 0.0, 0.5);
		Vector4 tooltip_bg = inventory_bg;

		// float icon_width = inventory_tile_size * padding;
		float icon_width = inventory_tile_size;
		float box_width = (max_icons_per_row * icon_width) + ((max_icons_per_row * padding) + padding);
		float x_start_pos = (screen_width / 2.0) - (box_width / 2.0);
		float y_start_pos = 115.0;
		float box_size_y = (icon_width + padding) * max_rows;

		int slot_index = 0;
		int icon_row_index = 0;


		// get how many different items in inventory
		int item_count = 0;
		for (int i = 0; i < ITEM_MAX; i++) {
			InventoryItemData* item = &world->inventory_items[i];
			if (item->amount > 0){
				item_count += 1;
			}
		}

		// inventory background_box rendering
		{
			Matrix4 xform = m4_identity;
			xform = m4_translate(xform, v3(x_start_pos, (y_start_pos - box_size_y) + icon_width + padding, 0.0));
			draw_rect_xform(xform, v2(box_width, box_size_y), inventory_bg);
		}

		// inventory item rendering
		for (int i = 0; i < ITEM_MAX; i++) {

			// when row is full, jump to next row
			if (slot_index >= max_icons_per_row){
					slot_index = 0;
					icon_row_index += 1;
			}

			// ItemData* item_data = get_item_data(i);
			InventoryItemData* item = &world->inventory_items[i];
			if (item->amount > 0){
				float slot_index_offset = slot_index * icon_width;

				Matrix4 xform = m4_scalar(1.0);
				float pos_x = (padding) + x_start_pos + slot_index_offset + (padding) * slot_index;
				float pos_y = (y_start_pos) - (icon_width * icon_row_index) - (padding * icon_row_index);
				xform = m4_translate(xform, v3(pos_x, pos_y, 0));

				// Sprite* sprite = get_sprite(get_sprite_id_from_archetype(i));
				Sprite* sprite = get_sprite(get_sprite_id_from_item(i));
				
				// draw icon background
				// draw_rect_xform(xform, v2(inventory_tile_size, inventory_tile_size), icon_background_col);

				float is_selected_alpha = 0.0;

				Draw_Quad* quad = draw_rect_xform(xform, v2(8, 8), v4(1,1,1,0));
				Range2f icon_box = quad_to_range(*quad);
				if (is_inventory_enabled && range2f_contains(icon_box, get_mouse_pos_in_ndc())) {
					is_selected_alpha = true;
				}

				// save xfrom for later when drawing item counts
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
						string title = sprint(temp_allocator, STR("%s"), get_archetype_name(i));

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
		// dealloc(get_heap_allocator(), item_arch_slots);
	}


	// :Building UI || :Render Building UI
	{
		// open building menu
		if (is_key_just_pressed('C')) {
			consume_key_just_pressed('C');
			world->ux_state = (world->ux_state == UX_building ? UX_nil : UX_building);
		}
		world->building_menu_alpha_target = (world->ux_state == UX_building ? 1.0 : 0.0);
		animate_f32_to_target(&world->building_menu_alpha, world->building_menu_alpha_target, delta_t, 15.0);
		bool is_building_enabled = world->building_menu_alpha_target == 1.0;

		// draw a row of icons for build menu
		if (world->building_menu_alpha_target == 1.0) { // when building menu is open
		
			int building_count = BUILDING_MAX - 1; // "-1" because of nil
			float icon_size = 12.0;
			float padding = 2.0;
			float total_box_width = building_count * icon_size + (padding * (building_count + 1));

			for (BuildingID id = 1; id < BUILDING_MAX; id++) {
				BuildingData* building = &buildings[id];

				Matrix4 xform = m4_identity;

				float x0 = (screen_width * 0.5) - (total_box_width * 0.5);
				x0 += (id-1) * icon_size;
				x0 += padding * (id);
				xform = m4_translate(xform, v3(x0, 0 + 10, 0));

				Sprite* icon = get_sprite(building->icon);

				// draw bg
				draw_rect_xform(xform, v2(icon_size, icon_size), v4(0.1, 0.1, 0.1, 0.5));

				// draw sprite
				Draw_Quad* quad = draw_image_xform(icon->image, xform, v2(icon_size, icon_size), COLOR_WHITE);

				Range2f box = quad_to_range(*quad);

				if (range2f_contains(box, get_mouse_pos_in_ndc())){

					world_frame.hover_consumed = true;

					if (is_key_just_pressed(MOUSE_BUTTON_LEFT)) {
						consume_key_just_pressed(MOUSE_BUTTON_LEFT);
						world->placing_building	= id;
						world->ux_state = UX_place_mode;
					}
				}

			}
		}
	}


	// :Placement mode || :Build mode
	// TODO: alpha animation for place mode
	if (world->ux_state == UX_place_mode)
	{
		set_world_space();

		{
			Vector2 mouse_pos_world = get_mouse_pos_in_world_space();
			BuildingData building = get_building_data(world->placing_building);
			Sprite* icon = get_sprite(building.icon);

			Vector2 pos = mouse_pos_world;
			pos = round_v2_to_tile(pos);

			Matrix4 xform = m4_identity;
			xform = m4_translate(xform, v3(pos.x, pos.y, 0));

			xform = m4_translate(xform, v3(0, tile_width * -0.5, 0));			// bring sprite down to bottom of Tile
			xform = m4_translate(xform, v3(get_sprite_size(icon).x * -0.5, 0.0, 0));

			// draw a ghost image on cursor
			Vector4 color = v4(1, 1, 1, 0.7);
			draw_image_xform(icon->image, xform, get_sprite_size(icon), color);

			if (is_key_just_pressed(MOUSE_BUTTON_LEFT)) {
				consume_key_just_pressed(MOUSE_BUTTON_LEFT);
				Entity* en = entity_create();
				// entity_setup(en, building.to_build);
				// setup_building(en, building.to_build);
				setup_building(en, world->placing_building);
				en->pos = pos;
				world->ux_state = 0;
			}

		}
		set_screen_space();
	}


	// :Render Hotbar || :Hotbar
	// if (IS_DEBUG)
	{
		float icon_width = 8.0;
		float slot_size = 8.0;
		float slot_index = 0;
		int padding = 4.0;
		int slot_count = 9;

		Vector2 hotbar_box_size = v2(slot_count * slot_size + (padding * (slot_count + 1)), 10);

		// Colors
		Vector4 hotbar_border_color = v4(0.8, 0.8, 0.8, 1);
		Vector4 hotbar_bg_color = v4(0.1, 0.1, 0.1, 1);
		Vector4 hotbar_selected_slot_color = v4(1, 0, 0, 1);



		// TEST 2 scuffed af

		int slotcount = 9;
		int pos_y = 2;

		for (int i = 0; i < ITEM_MAX; i++) {
			if (slot_index >= slotcount) {
				break;
			}
			InventoryItemData* item = &world->inventory_items[i];

			ItemData* selected_item = item;

			if (item->amount > 0){

				float pos_x = (screen_width * 0.5) - (hotbar_box_size.x * 0.5);
				pos_x += (slot_size * slot_index);
				pos_x += padding * (slot_index + 1);

				Matrix4 xform = m4_identity;
				xform = m4_translate(xform, v3(pos_x, pos_y, 0.0));

				Matrix4 xform_border = m4_scale(xform, v3(1.1, 1.1, 0));
				xform_border = m4_translate(xform_border, v3(-0.3, -0.3, 0));

				// draw hotbar border
				if (slot_index == selected_slot){
					draw_rect_xform(xform_border, v2(slot_size, slot_size), hotbar_selected_slot_color);

				}
				else{
					draw_rect_xform(xform_border, v2(slot_size, slot_size), hotbar_border_color);
				}
				// draw hotbar slot
				draw_rect_xform(xform, v2(slot_size, slot_size), hotbar_bg_color);


				Sprite* sprite = get_sprite(get_sprite_id_from_item(i));

				// center sprite
				xform = m4_translate(xform, v3(icon_width * 0.5, icon_width * 0.5, 0));

				xform = m4_translate(xform, v3(get_sprite_size(sprite).x * -0.5, get_sprite_size(sprite).y * -0.5, 0));

				// draw icon
				draw_image_xform(sprite->image, xform, get_sprite_size(sprite), COLOR_WHITE);

				slot_index++;
			}
		}

		// draw the rest of the boxes
		if (slot_index < slotcount) {
			for (int i = slot_index; i < slotcount; i++) {
				float pos_x = (screen_width * 0.5) - (hotbar_box_size.x * 0.5);
				pos_x += (slot_size * slot_index);
				pos_x += padding * (slot_index + 1);

				Matrix4 xform = m4_identity;
				xform = m4_translate(xform, v3(pos_x, pos_y, 0.0));

				Matrix4 xform_border = m4_scale(xform, v3(1.1, 1.1, 0));
				xform_border = m4_translate(xform_border, v3(-0.3, -0.3, 0));

				// draw hotbar border
				if (slot_index == selected_slot){
					draw_rect_xform(xform_border, v2(slot_size, slot_size), hotbar_selected_slot_color);

				}
				else{
					draw_rect_xform(xform_border, v2(slot_size, slot_size), hotbar_border_color);
				}
				// draw hotbar slot
				draw_rect_xform(xform, v2(slot_size, slot_size), hotbar_bg_color);

				slot_index++;
			}
		}



/*
		// test
		// get all items into a list
		// ItemData* itemList = NULL;
		// itemList = alloc(get_heap_allocator(), sizeof(ARCH_MAX));

		// for (int i = 0; i < ARCH_MAX; i++) {
		// 	ItemData* item = &world->inventory_items[i];
		// 	if (item->amount > 0){
		// 		itemList[i] = item;
		// 	}
		// 	// printf("ADDED %s to itemlist\n", world->inventory_items[i].name);
		// }
		// dealloc(get_heap_allocator(), itemList);

		// Draw hotbar
		float pos_y = 2;
		for (int i = 0; i < slot_count; i++) {

			float pos_x = (screen_width * 0.5) - (hotbar_box_size.x * 0.5);
			pos_x += (slot_size * i);
			pos_x += padding * (i+1);

			Matrix4 xform = m4_identity;
			xform = m4_translate(xform, v3(pos_x, pos_y, 0.0));

			Matrix4 xform_border = m4_scale(xform, v3(1.1, 1.1, 0));
			xform_border = m4_translate(xform_border, v3(-0.3, -0.3, 0));

			// draw hotbar border
			draw_rect_xform(xform_border, v2(slot_size, slot_size), hotbar_border_color);
			// draw hotbar slot
			draw_rect_xform(xform, v2(slot_size, slot_size), hotbar_bg_color);

			// draw icons
			// for (int archetype_num = 0; archetype_num < ARCH_MAX; archetype_num++) {
				// ItemData* item = &world->inventory_items[archetype_num];
				ItemData* item = &world->inventory_items[i];
				// Sprite* sprite = get_sprite(get_sprite_id_from_archetype(archetype_num));
				Sprite* sprite = get_sprite(get_sprite_id_from_archetype(i));


				// this is for interacting with the hotbar using the mouse cursor
				// Draw_Quad* quad = draw_rect_xform(xform, v2(8, 8), v4(1,1,1,0));
				// Range2f icon_box = quad_to_range(*quad);
				// if (is_inventory_enabled && range2f_contains(icon_box, get_mouse_pos_in_ndc())) {
				// 	is_selected_alpha = true;
				// }
				// Item selected (HOVERING)
				// if (is_selected_alpha){
				// 	// Scale item
				// 	// float scale_adjust = 0.5 * sin_breathe(os_get_current_time_in_seconds(), 5.0);
				// 	float scale_adjust = 1.5;
				// 	xform = m4_scale(xform, v3(scale_adjust, scale_adjust, 1));


				// }


				// save xfrom for later when drawing item counts
				// Matrix4 box_bottom_right_xform = xform;


				// center sprite
				xform = m4_translate(xform, v3(icon_width * 0.5, icon_width * 0.5, 0));

				xform = m4_translate(xform, v3(get_sprite_size(sprite).x * -0.5, get_sprite_size(sprite).y * -0.5, 0));


				// draw sprite
				if (item->amount > 0){
					draw_image_xform(sprite->image, xform, get_sprite_size(sprite), COLOR_WHITE);
				}


				// solutions:
				// either save all the xforms and draw icons later
				// or fix the whole inventory order shit



				// draw item count
				// draw_text_xform(font, sprint(temp_allocator, STR("%d"), item->amount), 40, box_bottom_right_xform, v2(0.1, 0.1), COLOR_WHITE);	// randy's solution
			// }

		}
*/
	}

	set_world_space();
	pop_z_layer();
}

// ----- ::Create entities -----------------------------------------------------------------------------|
void create_trees(int amount, int range) {
	// Create tree entities
	for (int i = 0; i < amount; i++) {
		Entity* en = entity_create();
		setup_tree(en);
		en->pos = v2(get_random_float32_in_range(-range, range), get_random_float32_in_range(-range, range));
		en->pos = round_v2_to_tile(en->pos);
	}
}

void create_rocks(int amount, int range) {
	// Create rock entities
	for (int i = 0; i < amount; i++) {
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


// Biome struct test --------------------------------|
typedef struct BiomeData {
	string name;
	Vector2 size;

	bool spawn_animals;
	bool spawn_water;
	float water_weight;

	Vector4 grass_color;
	Vector4 leaves_color;

	// trees
	bool spawn_pine_trees;
	float spawn_pine_tree_weight;
	bool spawn_spruce_trees;
	float spawn_spruce_tree_weight;
	bool spawn_birch_trees;
	float spawn_birch_tree_weight;
	bool spawn_palm_trees;
	float spawn_palm_tree_weight;

	bool spawn_rocks;
	float spawn_rocks_weight;
	bool spawn_mushrooms;
	float spawn_mushrooms_weight;
	bool spawn_twigs;
	float spawn_twigs_weight;
	bool spawn_berries;
	float spawn_berries_weight;

	// fossils
	bool spawn_fossils;
	float fossil0_drop_chance;
	float fossil1_drop_chance;
	float fossil2_drop_chance;
	int fossil_rarity_level;

} BiomeData;

// this is an example
void setup_biome_forest(BiomeData* biome) {
	// spawn weigth = spawn amount
	// drop chace = drop chance in %

	biome->name = STR("Forest");
	biome->size = v2(400, 400);
	biome->spawn_animals = false;
	biome->spawn_water = false;
	biome->grass_color = v4(0.32, 0.97, 0.62, 1);
	// biome->grass_color = v4(1, 0.97, 0.62, 1);
	biome->leaves_color	= v4(0, 1, 0, 1);

	// trees
	biome->spawn_pine_trees = true;
	biome->spawn_pine_tree_weight = 400;
	biome->spawn_spruce_trees = true;
	biome->spawn_spruce_tree_weight = 10;
	biome->spawn_birch_trees = false;
	biome->spawn_birch_tree_weight = 0;
	biome->spawn_palm_trees = false;
	biome->spawn_palm_tree_weight = 0;

	// entities
	biome->spawn_rocks = true;
	biome->spawn_rocks_weight = 75;
	biome->spawn_mushrooms = true;
	biome->spawn_mushrooms_weight = 10;
	biome->spawn_twigs = true;
	biome->spawn_twigs_weight = 10;
	biome->spawn_berries = true;
	biome->spawn_berries_weight = 5;

	// fossils
	biome->spawn_fossils = true;
	biome->fossil0_drop_chance = 5;
	biome->fossil1_drop_chance = 5;
	biome->fossil2_drop_chance = 5;
	// biome->fossil_rarity_level = 2;
}

void spawn_biome(BiomeData* biome) {
	if (biome->spawn_pine_trees) {create_trees((int)biome->spawn_pine_tree_weight, biome->size.x); }
	if (biome->spawn_rocks) {create_rocks((int)biome->spawn_rocks_weight, biome->size.x); }
	if (biome->spawn_berries) {create_bushes((int)biome->spawn_berries_weight, biome->size.x); }
	if (biome->spawn_twigs) {create_twigs((int)biome->spawn_twigs_weight, biome->size.x); }
	if (biome->spawn_mushrooms) {create_mushrooms((int)biome->spawn_mushrooms_weight, biome->size.x); }
	// window.clear_color = hex_to_rgba(biome->grass_color);
	// window.clear_color = biome->grass_color;
}


// ::LOOT ----------------------------------|
typedef struct LootItem {
	string *name;
	// EntityArchetype arch;
	ItemID id;
	float baseDropChance;
	struct LootItem *next;
} LootItem;

typedef struct LootTable {
	LootItem *head;
	int itemCount;
} LootTable;

LootTable* createLootTable() {
	// LootTable *table = (LootTable *)malloc(sizeof(LootTable));
	LootTable *table = alloc(get_heap_allocator(), sizeof(LootTable));

	table->head = NULL;
	table->itemCount = 0;
	return table;
}

// void addItemToLootTable(LootTable *table, const char *name, float baseDropChance) {
void addItemToLootTable(LootTable *table, string *name, EntityArchetype id, float baseDropChance) {
    // LootItem *newItem = (LootItem *)malloc(sizeof(LootItem));
	LootItem *newItem = alloc(get_heap_allocator(), sizeof(LootItem));

    newItem->name = name;
    newItem->baseDropChance = baseDropChance;
    newItem->next = table->head;
	newItem->id = id;
    table->head = newItem;
    table->itemCount++;

	printf("[LOOTTABLE]: ADDED '%s' TO LOOT TABLE, Table size = %d\n", name, table->itemCount);
}

// @PIN1: maybe take arch as input and decide what loot table to use based on it. so instead of taking "LootTable*" in, take "arch"
void generateLoot(LootTable* table, float luckModifier, Vector2 pos) {
	LootItem* current = table->head;
	int x_shift = 0;		// if multiple drops. this variable shifts the items abit on the x-axis.
	while(current != NULL) {
		float adjustedChance = current->baseDropChance * (1 + luckModifier);
		if (get_random_float32() < (adjustedChance / 100.0)) {
			// printf("Dropped: %s\n", current->name);

			switch (current->id) {
				case ITEM_rock: {
					{
						Entity* en = entity_create();
						// setup_item_rock(en);
						setup_item(en, ITEM_rock);
						pos.x += x_shift;
						en->pos = pos;
					}
				} break;
				case ITEM_fossil0: {
					{
						Entity* en = entity_create();
						// setup_item_fossil0(en);
						setup_item(en, ITEM_fossil0);
						pos.x += x_shift;
						en->pos = pos;
					}
				} break;
				case ITEM_fossil1: {
					{
						Entity* en = entity_create();
						// setup_item_fossil1(en);
						setup_item(en, ITEM_fossil1);
						pos.x += x_shift;
						en->pos = pos;
					}
				} break;
				case ITEM_fossil2: {
					{
						Entity* en = entity_create();
						// setup_item_fossil2(en);
						setup_item(en, ITEM_fossil2);
						pos.x += x_shift;
						en->pos = pos;
					}
				} break;

				default: {log_error("Can't spawn an item. switch defaulted");} break;
			}

			x_shift -= 5;
		}
		current = current->next;
	}
}


// free memory (use when loot table is no longer needed -> never use?)
// {
	// void freeLootTable(LootTable *table) {
	//     LootItem *current = table->head;
	//     while (current != NULL) {
	//         LootItem *toFree = current;
	//         current = current->next;
	//         free(toFree->name);
	//         free(toFree);
	//     }
	//     free(table);
	// }
// }




// ----- SETUP -----------------------------------------------------------------------------------------|
int entry(int argc, char **argv) 
{
	// Window
	window.title = STR("Game");
	window.scaled_width = 1280; // We need to set the scaled size if we want to handle system scaling (DPI)
	window.scaled_height = 720; 

	// window spawn position
	window.x = window.width * 0.5 + 350;		// window.x = 200; // default value // +350 so i can see console
	window.y = window.height * 0.5;		// window.y = 200; // default value

	// bg color
	window.clear_color = hex_to_rgba(0x43693aff);
	// window.clear_color = v4(1,1,1,1);

	// Memory
	world = alloc(get_heap_allocator(), sizeof(World));
	memset(world, 0, sizeof(World));

	// Font
	font = load_font_from_disk(STR("C:/windows/fonts/arial.ttf"), get_heap_allocator());
	assert(font, "Failed loading arial.ttf, %d", GetLastError());

	// :Load entity sprites
	sprites[0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/missing_texture.png"), get_heap_allocator())};
	sprites[SPRITE_player] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/player.png"), get_heap_allocator())};
	sprites[SPRITE_tree0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tree0.png"), get_heap_allocator())};
	sprites[SPRITE_tree1] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tree1.png"), get_heap_allocator())};
	sprites[SPRITE_tree2] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tree2.png"), get_heap_allocator())};
	sprites[SPRITE_tree3] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tree3.png"), get_heap_allocator())};
	sprites[SPRITE_rock0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/rock0.png"), get_heap_allocator())};
	sprites[SPRITE_rock1] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/rock1.png"), get_heap_allocator())};
	sprites[SPRITE_rock2] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/rock2.png"), get_heap_allocator())};
	sprites[SPRITE_rock3] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/rock3.png"), get_heap_allocator())};
	sprites[SPRITE_bush0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/bush0.png"), get_heap_allocator())};
	sprites[SPRITE_tall_grass0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tall_grass0.png"), get_heap_allocator())};
	sprites[SPRITE_tall_grass1] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tall_grass1.png"), get_heap_allocator())};
	sprites[SPRITE_mushroom0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/mushroom0.png"), get_heap_allocator())};

	// :Load item sprites
	sprites[SPRITE_item_rock] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_rock.png"), get_heap_allocator())};
	sprites[SPRITE_item_pine_wood] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_pine_wood.png"), get_heap_allocator())};
	sprites[SPRITE_item_sprout] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_sprout.png"), get_heap_allocator())};
	sprites[SPRITE_item_berry] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_berry.png"), get_heap_allocator())};
	sprites[SPRITE_item_twig] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_twig.png"), get_heap_allocator())};
	sprites[SPRITE_item_fossil0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_fossil0.png"), get_heap_allocator())};
	sprites[SPRITE_item_fossil1] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_fossil1.png"), get_heap_allocator())};
	sprites[SPRITE_item_fossil2] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_fossil2.png"), get_heap_allocator())};
	sprites[SPRITE_tool_pickaxe] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tool_pickaxe.png"), get_heap_allocator())};

	// :Load building sprites
	sprites[SPRITE_building_furnace] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/building_furnace.png"), get_heap_allocator())};
	sprites[SPRITE_building_workbench] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/building_workbench.png"), get_heap_allocator())};
	sprites[SPRITE_building_chest] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/building_chest.png"), get_heap_allocator())};

	// @ship debug this off (by: randy)
	{
		// crash when image fails to be setup properly (by: randy)
			// not working for me (- jani)
		// for (SpriteID i = 0; i < SPRITE_MAX; i++) {
		// 	Sprite* sprite = &sprites[i];
		// 	assert(sprite->image, "Sprite was not setup correctly");
		// }
	}

	// Building resource setup
	{
		// buildings[0] = ;
		buildings[BUILDING_furnace] = (BuildingData){.to_build=ARCH_building,. icon=SPRITE_building_furnace};
		buildings[BUILDING_workbench] = (BuildingData){.to_build=ARCH_building,. icon=SPRITE_building_workbench};
		buildings[BUILDING_chest] = (BuildingData){.to_build=ARCH_building,. icon=SPRITE_building_chest};
	}




	// :TESTS
	// if (IS_DEBUG)
	{
		// test adding items to inventory
		{
			// world->inventory_items[ARCH_item_pine_wood].amount = 5;
			world->inventory_items[ITEM_rock].amount = 5;
			world->inventory_items[ITEM_sprout].amount = 2;
			world->inventory_items[ARCH_tool_pickaxe].amount = 1;
		}

		
		// test adding furnace to world
		{
			
			// setup furnace
			// Entity* en = entity_create();
			// setup_furnace(en);
			// en->pos = v2(-25, 0);
			// en->pos = round_v2_to_tile(en->pos);
		}
	}







	// :INIT
	// :Create player entity
	Entity* player_en = entity_create();
	setup_player(player_en);

	// setup forest test
	BiomeData* forest = 0;
	forest = alloc(get_heap_allocator(), sizeof(BiomeData));
	memset(forest, 0, sizeof(BiomeData));
	setup_biome_forest(forest);
	spawn_biome(forest);

	// test adding stuff to loot table (can't be in a scope)
	LootTable *lootTable_rock = createLootTable();
	addItemToLootTable(lootTable_rock, &STR("Stone"), ITEM_rock, 100);
	addItemToLootTable(lootTable_rock, &STR("Ammonite Fossil"), ITEM_fossil0, forest->fossil0_drop_chance);
	addItemToLootTable(lootTable_rock, &STR("Bone Fossil"), ITEM_fossil1, forest->fossil1_drop_chance);
	addItemToLootTable(lootTable_rock, &STR("Fang Fossil"), ITEM_fossil2, forest->fossil2_drop_chance);
	// addItemToLootTable(lootTable_rock, &STR("asd"), ARCH_nil, 10.0); // this line makes it so fossils dont spawn. bug?


	{
		// item_data[ITEM_rock] = (ItemData){ .pretty_name=STR("Rock"),};
	}

	// create entities (amount, range (from 0,0))
	// create_rocks(25, 200);
	// create_trees(100, 200);
	// create_bushes(15, 200);
	// create_twigs(5, 200);


	// inventory test #2
	// Inventory player_inventory;
	// initInventory(&player_inventory);
	// inventoryAddItem(&player_inventory, 1, "TEST", 10);



	// Player variables
	float player_speed = 50.0;			// pixels per second
	const float player_size_x = 10.0;	// player sprite size x (pixels)
	const float player_size_y = 10.0;	// player sprite size y (pixels)
	bool inventory_open = false;
	

	// World variables
	bool draw_grid = true;

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
		delta_t = now - last_time;
		last_time = now;
		os_update(); 

		// :Frame :update
		draw_frame.enable_z_sorting = true;
		world_frame.world_projection = m4_make_orthographic_projection(window.width * -0.5, window.width * 0.5, window.height * -0.5, window.height * 0.5, -1, 10);

		// :camera
		{
			Vector2 target_pos = player_en->pos;
			animate_v2_to_target(&camera_pos, target_pos, delta_t, 10.0f); // 4th value controls how smooth the camera transition is to the player

			world_frame.world_view = m4_make_scale(v3(1.0, 1.0, 1.0)); // View zoom (zooms so pixel art is the correct size)
			world_frame.world_view = m4_mul(world_frame.world_view, m4_make_translation(v3(camera_pos.x, camera_pos.y, 0)));
			world_frame.world_view = m4_mul(world_frame.world_view, m4_make_scale(v3(view_zoom, view_zoom, 1.0)));
		}

		set_world_space();
		push_z_layer(layer_world);

		Vector2 mouse_pos_world = get_mouse_pos_in_world_space();
		int mouse_tile_x = world_pos_to_tile_pos(mouse_pos_world.x);
		int mouse_tile_y = world_pos_to_tile_pos(mouse_pos_world.y);

		// Render ui
		render_ui();

		// :Entity selection
		if (!world_frame.hover_consumed)
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


		// :update entities (:item pick up)
		{
			// bool is_pickup_text_visible = false;
			for (int i = 0; i < MAX_ENTITY_COUNT; i++) {
				Entity* en = &world->entities[i];
				if (en->is_valid) {

					// item pick up
					if (en->is_item) {
						// TODO PHYSICS

						if (fabsf(v2_dist(en->pos, player_en->pos)) < player_pickup_radius) {
							world->inventory_items[en->item_id].amount += 1;
							// inventoryAddItem(&player_inventory, 1, STR(en->arch), 1);
							entity_destroy(en);

							// is_pickup_text_visible = true;
						}

						// Vector4 col = v4(1, 1, 1, 1);

						// // Render pickup-text
						// if (is_pickup_text_visible)
						// {
						// 	printf("pickup text visible\n");
						// 	if (col.a == 0.0){
						// 		printf("disabling pickup text now\n");
						// 		is_pickup_text_visible = false;
						// 	}

						// 	printf("col.a = %f\n", col.a);
						// 	animate_f32_to_target(&col.a, 0.0, delta_t, 15.0);
						// 	draw_text(font, STR("test"), font_height, v2(10, 10), v2(10, 10), col);
						// }
					}
				}
			}
		}


		// :player use/attack || :spawn item
		{
			Entity* selected_en = world_frame.selected_entity;

			if (is_key_just_pressed(MOUSE_BUTTON_LEFT)) {
				if (selected_en) {
					consume_key_just_pressed(MOUSE_BUTTON_LEFT);
					selected_en->health -= 1;
					if (selected_en->health <= 0) {

						// @PIN1: instead of switch case, maybe just do "generateLoot(selected_en->arch, 0, selected_en->pos);"
						// and in the generateLoot func decide what loot table to use based on the passed arch 
						switch (selected_en->arch) {
							case ARCH_tree: {
								{
									Entity* en = entity_create();
									// setup_item_pine_wood(en);
									setup_item(en, ITEM_pine_wood);
									en->pos = selected_en->pos;
								}
							} break;

							case ARCH_rock: {
								{
									// Entity* en = entity_create();
									// setup_item_rock(en);
									// en->pos = selected_en->pos;
									generateLoot(lootTable_rock, 0, selected_en->pos);
								}
							} break;

							case ARCH_bush: {
								{
									Entity* en = entity_create();
									// setup_item_berry(en);
									setup_item(en, ITEM_berry);
									en->pos = selected_en->pos;
								}
							} break;

							case ARCH_mushroom: {
								{
									Entity* en = entity_create();
									setup_item(en, ITEM_mushroom0);
									en->pos = selected_en->pos;
								}
							} break;

							case ARCH_twig: {
								{
									Entity* en = entity_create();
									// setup_item_twig(en);
									setup_item(en, ITEM_twig);
									en->pos = selected_en->pos;
								}
							} break;

							case ARCH_building: {
								{
									// Entity* en = entity_create();
									// setup_building(en, );
									// setup_item(en, get_sprite_id_from_archetype(ARCH_building));
									// en->pos = selected_en->pos;
									log_error(":spawn item, building is WIP");
								}
							}

							// case ARCH_furnace: {
							// 	{
							// 		Entity* en = entity_create();
							// 		setup_item_furnace(en);
							// 		en->pos = selected_en->pos;
							// 	}
							// } break;

							// case ARCH_workbench: {
							// 	{
							// 		Entity* en = entity_create();
							// 		setup_item_workbench(en);
							// 		en->pos = selected_en->pos;
							// 	}
							// } break;

							// case ARCH_chest: {
							// 	{
							// 		Entity* en = entity_create();
							// 		setup_item_chest(en);
							// 		en->pos = selected_en->pos;
							// 	}
							// } break;

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


/*
		// // inventory render test (randy's solution #1) (centered inventory - status: WORKING nicely)
		// {
		// 	if (inventory_open){
		// 	float width = 240.0;
		// 	float height = 135.0;

		// 	draw_frame.view = m4_scalar(1.0);
		// 	draw_frame.projection = m4_make_orthographic_projection(0.0, width, 0.0, height, -1, 10);


		// 	int item_count = 0;
		// 	for (int i = 0; i < ARCH_MAX; i++) {
				// ItemData* item = &world->inventory_items[i];
				// InventoryItemData* item = &world->inventory_items[i];
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
		if (is_key_just_pressed('X')){ // EXIT
			window.should_close = true;
		}
		if (is_key_just_pressed('T')){
			if (IS_DEBUG){
				IS_DEBUG = false;
			}
			else{
				IS_DEBUG = true;
			}
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
					case (INPUT_EVENT_KEY):{break;}
					case (INPUT_EVENT_TEXT):{break;}
				}
			}
		}

		// if (is_key_just_pressed('G')) {displayInventory(&player_inventory);}
		if (is_key_just_pressed('G')) {generateLoot(lootTable_rock, 100, v2(0,0));}


		// numbers
		if (is_key_just_pressed('1')) {selected_slot = 1 - 1;}
		if (is_key_just_pressed('2')) {selected_slot = 2 - 1;}
		if (is_key_just_pressed('3')) {selected_slot = 3 - 1;}
		if (is_key_just_pressed('4')) {selected_slot = 4 - 1;}
		if (is_key_just_pressed('5')) {selected_slot = 5 - 1;}
		if (is_key_just_pressed('6')) {selected_slot = 6 - 1;}
		if (is_key_just_pressed('7')) {selected_slot = 7 - 1;}
		if (is_key_just_pressed('8')) {selected_slot = 8 - 1;}
		if (is_key_just_pressed('9')) {selected_slot = 9 - 1;}

		// :Sprint
		if (is_key_down(KEY_SHIFT)){ player_speed = 100.0;}
		else { player_speed = 50.0;}

		// Player movement
		Vector2 input_axis = v2(0, 0);
		if (is_key_down('W')){input_axis.y += 1.0;}
		if (is_key_down('A')){input_axis.x -= 1.0;}
		if (is_key_down('S')){input_axis.y -= 1.0;}
		if (is_key_down('D')){input_axis.x += 1.0;}

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