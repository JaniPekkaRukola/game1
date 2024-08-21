// includes all headers
#include "headers\masterheader.h"

// ----- ::Settings || ::Tweaks || ::Global --------------------------------------------------------|

bool IS_DEBUG = false;
bool print_fps = true;
// bool print_fps = false;

bool enable_tooltip = true;
bool render_hotbar = true;
bool render_player = true;
bool render_other_entities = true;
bool draw_grid = false;
bool render_ground_texture = true;

// PLAYER
float player_speed = 50.0;			// pixels per second
float player_running_speed = 100.0;
bool player_running = false;
float entity_selection_radius = 5.0f;
const int player_pickup_radius = 15.0;
float render_distance = 175;		// 170 is pretty good

// keybinds
char player_use_key = 'F';


// COLORS
const Vector4 item_shadow_color = {0, 0, 0, 0.2};
const Vector4 entity_shadow_color = {0, 0, 0, 0.15};

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
// Vector2 entity_positions[MAX_ENTITY_COUNT]; // not in use
bool dragging = false;


// ----- engine changes (by: randy) ----------------------------------------------------------------|
// maybe should move these into "functions.h"

#define m4_identity m4_make_scale(v3(1, 1, 1))

// "resizable" array helpers (by: randy)
// void* array_add() {}


// inline float v2_length(Vector2 a) {
//     return sqrt(a.x * a.x + a.y * a.y);
// }


int compare_strings(string str, const char* cstr) {
    u64 cstr_len = strlen(cstr);
    if (str.count != cstr_len) {
        return 0;
    }
    for (u64 i = 0; i < str.count; i++) {
        if (str.data[i] != (u8)cstr[i]) {
            return 0;
        }
    }
    return 1;
}



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

// ::AUDIO ---------------------------|
typedef struct Audio {
	Audio_Source source;
	string name;
	string path;
	bool ok;
	// AudioID id;
} Audio;

typedef enum AudioID{
	AUDIO_nil,

	// hits
	AUDIO_hit_metal1,
	AUDIO_hit_metal2,

	// stuff breaking
	AUDIO_rock_breaking1,


	// swings
	AUDIO_swing_slow,
	AUDIO_swing_fast,


	AUDIO_MAX,
} AudioID;

Audio audioFiles[AUDIO_MAX];


// not in use

	// string get_archetype_pretty_name(EntityArchetype arch) {
	// 	switch (arch) {
	// 		// case ARCH_furnace: return STR("Furnace");
	// 		case ARCH_building: return STR("building");
	// 		// :arch
	// 		default: return STR("nil");
	// 	}
	// }



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


	// void add_entity_to_renderlist();
	// void remove_entity_from_renderlist();




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


	// this wont really work if i separate tools and buildings into their own structures
	// this might be a part of the @pin2 prob
	// string get_archetype_name(ItemID id) { // NOT IN USE
		// 	switch (id) {
		// 		case ITEM_pine_wood: return STR("Pine Wood"); break;
		// 		case ITEM_rock: return STR("Rock"); break;
		// 		case ITEM_sprout: return STR("Sprout"); break;
		// 		case ITEM_berry: return STR("Berry"); break;
		// 		case ITEM_mushroom0: return STR("Mushroom"); break;
		// 		// case ARCH_twig: return STR("Twig"); break;
		// 		case ITEM_fossil0: return STR("Ammonite Fossil"); break;
		// 		case ITEM_fossil1: return STR("Bone Fossil"); break;
		// 		case ITEM_fossil2: return STR("Fang Fossil"); break;
		// 		case ARCH_tool: return STR("Tool (WIP)"); break;

		// 		case ARCH_ore: return STR("ORE"); break;

		// 		// building names
		// 		// case BUILDING_furnace: return STR("Furnace");
		// 		// case BUILDING_workbench: return STR("Workbench");
		// 		// case BUILDING_chest: return STR("Chest");
		// 		default: return STR("nil"); break;
		// 	}
	// }



	// is this also @pin2 problem?
	// ToolID get_tool_id_from_arch(ToolID id) {
	// 	switch (id) {
	// 		case TOOL_axe: return Tool_axe;
	// 	}
	// }




	// not in use
	// void setup_tool(Entity* en, ToolID tool_id) {
	// 	en->arch = ARCH_item;
	// 	en->sprite_id = get_sprite_id_from_tool(tool_id);
	// 	en->is_item = true;

	// 	// TODO: fix this
	// 	// en->item_id = tool_id;
	// 	en->tool_id = tool_id;

	// 	switch (tool_id){
	// 		case TOOL_pickaxe:{en->name = STR("Pickaxe");}break;
	// 		case TOOL_axe:{en->name = STR("Axe");}break;
	// 		case TOOL_shovel:{en->name = STR("Shovel");}break;
	// 	}
	// }



// 


void set_screen_space() {
	draw_frame.view = m4_scalar(1.0);
	draw_frame.projection = m4_make_orthographic_projection(0.0, screen_width, 0.0, screen_height, -1, 10);
}

void set_world_space() {
	// make the viewport (or whatever) to window size, instead of -1.7, 1.7, -1, 1
	draw_frame.projection = world_frame.world_projection;
	draw_frame.view = world_frame.world_view;
}



// ----- ::Create entities -----------------------------------------------------------------------------|
// maybe should move these into "create_entities.h" ???

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


// ----- ::FUNC Dump (name by randy) -------------------------------------------------------------------|

// inventory render (randy's solution #2) (NOT centered inventory - status: WORKING)
// :Render UI
void render_ui()
{
	// InventoryItemData* dragging_now = (InventoryItemData*)alloc(get_heap_allocator(), sizeof(InventoryItemData));

	set_screen_space();
	push_z_layer(layer_ui);

	// NOTE: should this (the inventory rendering) be put into a scope?

	// open inventory
	if (is_key_just_pressed(KEY_TAB)) {
		consume_key_just_pressed(KEY_TAB);
		world->ux_state = (world->ux_state == UX_inventory ? UX_nil : UX_inventory);
	}

	world->inventory_alpha_target = (world->ux_state == UX_inventory ? 1.0 : 0.0);
	animate_f32_to_target(&world->inventory_alpha, world->inventory_alpha_target, delta_t, 15.0);
	bool is_inventory_enabled = world->inventory_alpha_target == 1.0;

	// if (world->inventory_alpha != 0.0)
	// :Inventory UI || :Render Inventory UI
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
		// {
			Matrix4 xform_inventory_box = m4_identity;
			xform_inventory_box = m4_translate(xform_inventory_box, v3(x_start_pos, (y_start_pos - box_size_y) + icon_width + padding, 0.0));
			draw_rect_xform(xform_inventory_box, v2(box_width, box_size_y), inventory_bg);
		// }

		// inventory item rendering
		for (int i = 0; i < ITEM_MAX; i++) {

			Matrix4 xform = m4_identity;
			Matrix4 xform2 = m4_identity;
			Matrix4 box_bottom_right_xform = m4_identity;
			Sprite* sprite = NULL;
			Draw_Quad* quad = NULL;
			float is_selected_alpha = 0.0;
			Sprite* icon = NULL;

			// when row is full, jump to next row
			if (slot_index >= max_icons_per_row){
					slot_index = 0;
					icon_row_index += 1;
			}

			// ItemData* item_data = get_item_data(i);
			InventoryItemData* item = &world->inventory_items[i];
			if (item->amount > 0){
				float slot_index_offset = slot_index * icon_width;

				// Matrix4 xform = m4_scalar(1.0);
				xform = m4_identity;
				float pos_x = (padding) + x_start_pos + slot_index_offset + (padding) * slot_index;
				float pos_y = (y_start_pos) - (icon_width * icon_row_index) - (padding * icon_row_index);
				xform = m4_translate(xform, v3(pos_x, pos_y, 0));

				// get sprite
				Sprite* sprite = get_sprite(get_sprite_from_itemID(i));

				// update sprite if its a tool (temp solution)
				if (item->arch == ARCH_tool){
					sprite = get_sprite(item->sprite_id);
				}

				
				// draw icon background
				// draw_rect_xform(xform, v2(inventory_tile_size, inventory_tile_size), icon_background_col);

				// quad for checking if mouse is ontop item
				Draw_Quad* quad = draw_rect_xform(xform, v2(8, 8), v4(1,1,1,0));
				Range2f icon_box = quad_to_range(*quad);
				if (is_inventory_enabled && range2f_contains(icon_box, get_mouse_pos_in_ndc())) {
					is_selected_alpha = true;
				}

				// save xfrom for later when drawing item counts
				// Matrix4 box_bottom_right_xform = xform;
				box_bottom_right_xform = xform;

				// center sprite
				xform = m4_translate(xform, v3(icon_width * 0.5, icon_width * 0.5, 0));

				// ITEM DRAGGING LOGIC:
				// ------------------------------------------------------------------------------------------------|
				// check if cursor is on item
				// check if mouse1 clicked
				//		- true: 
				// 			- save item to another variable
				// 			- delete original item
				// 			- draw ghost item on cursor with item count
				// 	check if mouse1 is released
				// 		- true:
				// 			- check if inside inventory
				// 				- true
				// 					put item back into inventory using the saved item
				// 				- false
				// 					spawn item on the world (next to player?) (cooldown on picking it up again?)
				// 		- false:
				// 			- continue dragging
				// ------------------------------------------------------------------------------------------------|

				// Item selected (HOVERING)
				if (is_selected_alpha){
					// Scale item
					// float scale_adjust = 0.5 * sin_breathe(os_get_current_time_in_seconds(), 5.0);
					float scale_adjust = 1.5;
					Matrix4 xform = m4_scale(xform, v3(scale_adjust, scale_adjust, 1));

					// get the icon under cursor
					icon_drag = get_sprite(item->sprite_id);

					// start dragging item
					if (is_key_down(MOUSE_BUTTON_LEFT)){	// this runs only once when mousebutton is pressed
						consume_key_just_pressed(MOUSE_BUTTON_LEFT);
						dragging = true;

						// save the item to another variable (stfu)
						*dragging_now = *item;

						if (dragging_now != NULL){
							printf("Deleted item '%s' from inventory\n", item->name);
							printf("TEST ITEM AMOUNT %d\n", item->amount);
							delete_item_from_inventory(item->item_id, item->amount);
						}
					}
				}

			// printf("DEBUG TIME\n");
			// printf("Dragging_now:\nDRAGGING = %d\n", dragging);
			// printf("Name = %s\n", dragging_now->name);
			// printf("Amount = %d\n", dragging_now->amount);
			
			// printf("\n");

			// printf("ITEM:\nDRAGGING = %d\n", dragging);
			// printf("Name = %s\n", item->name);
			// printf("Amount = %d\n", item->amount);

			// printf("\n----------------------------------------------\n");
			
		
		
	
			// TODO: FIX
			/*
				this is the terminal debug output when dragging ONLY pine wood (10) and releasing into inventory quickly
				these prints come from under rendering ghost image below
				wtf

				Deleted item 'Pine wood' from inventory
				TEST ITEM AMOUNT 10
				DRAGGING NAME = Pine wood
				DRAGGING NOW AMOUNT = 10
				--------------
				DRAGGING NAME = Pine wood
				DRAGGING NOW AMOUNT = 10
				--------------
				DRAGGING NAME = Pine wood
				DRAGGING NOW AMOUNT = 10
				--------------
				DRAGGING NAME = Pine wood
				DRAGGING NOW AMOUNT = 10
				--------------
				Deleted item 'Pickaxe' from inventory
				TEST ITEM AMOUNT 1
				DRAGGING NAME = Pickaxe
				DRAGGING NOW AMOUNT = 1
				--------------
				DRAGGING NAME = Pickaxe
				DRAGGING NOW AMOUNT = 1
				--------------
				DRAGGING NAME = Pickaxe
				DRAGGING NOW AMOUNT = 1
				--------------
				Deleted item 'AXE' from inventory
				TEST ITEM AMOUNT 1
				DRAGGING NAME = AXE
				DRAGGING NOW AMOUNT = 1
			*/


				// :DRAGGING
				{
					if (dragging){
						Vector2 mouse_pos_screen = get_mouse_pos_in_ndc();
						mouse_pos_screen = v2(mouse_pos_screen.x * (0.5 * screen_width) + (0.5 * screen_width), mouse_pos_screen.y * (0.5 * screen_height) + (0.5 * screen_height));

						xform2 = m4_identity;
						xform2 = m4_translate(xform2, v3(mouse_pos_screen.x, mouse_pos_screen.y, 0));

						Matrix4 box_bottom_right_xform2 = xform2;

						box_bottom_right_xform2 = m4_translate(box_bottom_right_xform2, v3(get_sprite_size(icon_drag).x * -0.5, get_sprite_size(icon_drag).y * -0.5, 0));

						// center icon sprite
						xform2 = m4_translate(xform2, v3(get_sprite_size(icon_drag).x * -0.5, get_sprite_size(icon_drag).y * -0.5, 0));

						// draw ghost image on cursor
						Vector4 color = v4(1, 1, 1, 0.7);
						draw_image_xform(icon_drag->image, xform2, get_sprite_size(icon_drag), COLOR_WHITE);


						printf("DRAGGING NAME = %s\n", dragging_now->name);
						printf("DRAGGING NOW AMOUNT = %d\n", dragging_now->amount);
						printf("--------------\n");
						

						// draw item count
						if (dragging_now != NULL && dragging_now->arch != ARCH_tool){
							draw_text_xform(font, sprint(temp_allocator, STR("%d"), dragging_now->amount), 40, box_bottom_right_xform2, v2(0.1, 0.1), COLOR_WHITE);	// randy's solution
						}

						if (is_key_up(MOUSE_BUTTON_LEFT)){
							// quad for checking if mouse is ontop item
							Draw_Quad* quad_inventory_box = draw_rect_xform(xform_inventory_box, v2(box_width, box_size_y), v4(0,0,0,0));
							Range2f range_inventory_box = quad_to_range(*quad_inventory_box);

							// check if released into inventory
							if (range2f_contains(range_inventory_box, get_mouse_pos_in_ndc())) {
								printf("RELEASED INTO INVENTORY\n");
								add_item_to_inventory_quick(dragging_now);
								printf("Added item '%s' into inventory\n", dragging_now->name);
								// dragging_now = NULL;
								dragging = false;
							}
							else{
								printf("RELEASED INTO WORLD\n");
								// dragging_now = NULL;
								dragging = false;
							}
						}

					}

				}

				// end dragging item
				if (is_key_up(MOUSE_BUTTON_LEFT)){
					dragging = false;
					// dragging_now = NULL;
				}



				// prevent clicking entities under the inventory (WIP)
				// TODO: needs to calculate if cursor is in the inventory box
				// else{
				// 	if (is_key_just_pressed(MOUSE_BUTTON_LEFT)){
				// 		consume_key_just_pressed(MOUSE_BUTTON_LEFT);
				// 	}
				// }


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

				sprite = get_sprite(get_sprite_from_itemID(i));
				if (item->arch == ARCH_tool){
					sprite = get_sprite(item->sprite_id);
				}

				xform = m4_translate(xform, v3(get_sprite_size(sprite).x * -0.5, get_sprite_size(sprite).y * -0.5, 0));

				// draw sprite
				draw_image_xform(sprite->image, xform, get_sprite_size(sprite), COLOR_WHITE);

				// draw item count (except for tools)
				// draw_text(font, sprint(temp_allocator, STR("%d"), item->amount), 40, v2(pos_x, pos_y), v2(0.1, 0.1), COLOR_WHITE);
				if (item->arch != ARCH_tool){
					draw_text_xform(font, sprint(temp_allocator, STR("%d"), item->amount), 40, box_bottom_right_xform, v2(0.1, 0.1), COLOR_WHITE);	// randy's solution
				}

				// tooltip
				if (enable_tooltip)
				{
					if (is_selected_alpha){
						// NOTE: ugly code. fix later :)

						float slot_index_offset = slot_index * icon_width;
						Matrix4 xform = m4_identity;
						float pos_x = (padding) + x_start_pos + slot_index_offset + (padding) * slot_index;
						float pos_y = (y_start_pos) - (icon_width * icon_row_index) - (padding * icon_row_index);
						xform = m4_translate(xform, v3(pos_x, pos_y, 0));
						Draw_Quad* quad = draw_rect_xform(xform, v2(8, 8), v4(1,1,1,0));
						
						Draw_Quad screen_quad = ndc_quad_to_screen_quad(*quad);
						Range2f screen_range = quad_to_range(screen_quad);
						Vector2 icon_center = range2f_get_center(screen_range);

						// icon_center
						Vector2 tooltip_box_size = v2(40, 20);

						Matrix4 xform_tooltip = m4_identity;
						xform_tooltip = m4_translate(xform_tooltip, v3(tooltip_box_size.x * -0.5, -tooltip_box_size.y - icon_width * 0.5 - padding * 0.5, 0));
						xform_tooltip = m4_translate(xform_tooltip, v3(icon_center.x, icon_center.y, 0));

						// tooltip bg box
						draw_rect_xform(xform_tooltip, tooltip_box_size, tooltip_bg);

						// get the tooltip title (item name)
						string title = item->name;

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
				xform = m4_translate(xform, v3(x0, 0 + 15, 0));

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
	if (render_hotbar)
	{
		float icon_width = 8.0;
		float slot_size = 8.0;
		int slot_index = 0;
		int padding = 4.0;
		int slot_count = 9;

		Vector2 hotbar_box_size = v2(slot_count * slot_size + (padding * (slot_count + 1)), 10);

		// Colors
		Vector4 hotbar_border_color = v4(0.8, 0.8, 0.8, 1);
		Vector4 hotbar_bg_color = v4(0.1, 0.1, 0.1, 1);
		Vector4 hotbar_selected_slot_color = v4(1, 0, 0, 1);

		InventoryItemData* new_selected_item = NULL;



		// TEST 2 scuffed af

		int slotcount = 9;
		int pos_y = 2;

		for (int i = 0; i < ITEM_MAX; i++) {
			if (slot_index >= slotcount) {
				break;
			}

			InventoryItemData* item = &world->inventory_items[i];

			if (item->amount > 0){
				
				// if (item->sprite_id == 20){
				// 	printf("20--------------------------------------------\n");
				// 	printf("%s\n", item->name);
				// 	printf("arch = %d\n", item->arch);
				// }

				float pos_x = (screen_width * 0.5) - (hotbar_box_size.x * 0.5);
				pos_x += (slot_size * slot_index);
				pos_x += padding * (slot_index + 1);

				Matrix4 xform = m4_identity;
				xform = m4_translate(xform, v3(pos_x, pos_y, 0.0));

				// save xfrom for later when drawing item counts
				Matrix4 box_bottom_right_xform = xform;

				Matrix4 xform_border = m4_scale(xform, v3(1.1, 1.1, 0));
				xform_border = m4_translate(xform_border, v3(-0.3, -0.3, 0));

				// draw hotbar border
				if (slot_index == selected_slot){
					// draw selected border
					draw_rect_xform(xform_border, v2(slot_size, slot_size), hotbar_selected_slot_color);

					// update selected item
					new_selected_item = item;
				}
				else{
					// draw UNselected border
					draw_rect_xform(xform_border, v2(slot_size, slot_size), hotbar_border_color);
				}

				// draw hotbar slot
				draw_rect_xform(xform, v2(slot_size, slot_size), hotbar_bg_color);

				// get the sprite of the item
				Sprite* sprite = get_sprite(get_sprite_from_itemID(i));

				// get the sprite of the item (if its a tool) its WIP shutup
				if (item->arch == ARCH_tool){
					sprite = get_sprite(item->sprite_id);
				}

				// center sprite to slot
				xform = m4_translate(xform, v3(icon_width * 0.5, icon_width * 0.5, 0));

				// center sprite
				xform = m4_translate(xform, v3(get_sprite_size(sprite).x * -0.5, get_sprite_size(sprite).y * -0.5, 0));

				// draw icon
				draw_image_xform(sprite->image, xform, get_sprite_size(sprite), COLOR_WHITE);

				// draw item count (except for tools)
				if (item->arch != ARCH_tool){
					draw_text_xform(font, sprint(temp_allocator, STR("%d"), item->amount), 40, box_bottom_right_xform, v2(0.1, 0.1), COLOR_WHITE);	// randy's solution
				}

				slot_index++;
			}

			// Update the selected item
			if (new_selected_item) {
				selected_item = new_selected_item;
				selected_item->valid = true;
			} else if (selected_item) {
				selected_item->valid = false;
				selected_item = NULL;
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



	}

	set_world_space();
	pop_z_layer();
}

// :Chest UI || :Render Chest UI
void render_building_ui(UXState ux_state)
{

	// close building ui
	if (is_key_just_pressed(KEY_ESCAPE) || is_key_just_pressed(player_use_key)){
		consume_key_just_pressed(KEY_ESCAPE);
		consume_key_just_pressed(player_use_key);
		world->ux_state = UX_nil;
	}

	// if (is_key_just_pressed(MOUSE_BUTTON_RIGHT)){
	// 	consume_key_just_pressed(MOUSE_BUTTON_RIGHT);
	// }

	set_screen_space();
	push_z_layer(layer_ui);

	// RENDER CHEST UI
	if (ux_state == UX_chest){

		// printf("RENDERING CHEST UI\n");

		// chest size variables
		const int max_slots_row = 9;
		const int max_slots_col = 4;
		const int slot_size = 8;
		const int padding = 2;
		int slot_index = 0;
		int row_index = 0;

		// const Vector2 chest_ui_size = v2(200, 100);
		const Vector2 chest_ui_size = v2((max_slots_row * slot_size) + (max_slots_row * padding) + padding, (max_slots_col * slot_size) + (max_slots_col * padding) + padding);
		Vector2 chest_ui_pos = v2(screen_width * 0.5, screen_height * 0.5);
		chest_ui_pos = v2(chest_ui_pos.x - (chest_ui_size.x * 0.5), chest_ui_pos.y - (chest_ui_size.y * 0.5));
		float slot_border_width = 1;

		const float x_start_pos = chest_ui_pos.x;
		const float y_start_pos = chest_ui_pos.y;

		// Colors
		Vector4 chest_bg = v4(0.25, 0.25, 0.25, 0.7);
		Vector4 slot_border_color = v4(1, 1, 1, 0.7);
		// Vector4 slot_color = v4(1, 1, 1, 0.7);

		world->chest_alpha_target = (world->ux_state == UX_chest ? 1.0 : 0.0);
		animate_f32_to_target(&world->chest_alpha, world->chest_alpha_target, delta_t, 15.0);
		bool is_chest_enabled = world->chest_alpha_target == 1.0;

		if (world->chest_alpha_target != 0.0)
		{
			Matrix4 xform = m4_identity;
			xform = m4_translate(xform, v3(chest_ui_pos.x, chest_ui_pos.y, 0));
			// xform = m4_scale(xform, v3(chest_ui_size.x, chest_ui_size.y, 0));

			// draw background
			draw_rect_xform(xform, v2(chest_ui_size.x, chest_ui_size.y), chest_bg);

			// draw slots
			for (int i = 0; i < max_slots_row* max_slots_col; i++){

				Matrix4 xform_slot = m4_identity;
				float x_pos = x_start_pos + (slot_index * slot_size) + (slot_index * padding) + padding;
				float y_pos = y_start_pos + (row_index * slot_size) + (row_index * padding) + padding;

				// translate xform
				xform_slot = m4_translate(xform_slot, v3(x_pos, y_pos, 0));

				// draw slot border
				draw_rect_xform(xform_slot, v2(slot_size, slot_size), slot_border_color);

				// draw slot
				xform_slot = m4_translate(xform_slot, v3((0.5 * slot_border_width), (0.5 * slot_border_width), 0));
				draw_rect_xform(xform_slot, v2(slot_size - slot_border_width, slot_size - slot_border_width), chest_bg);

				slot_index++;
				if (slot_index >= max_slots_row){
					row_index++;
					slot_index = 0;
				}

			}

		}
	}

	// RENDER FURNACE UI
	else if (ux_state == UX_furnace){
		printf("RENDERING FURNACE UI\n");

		// furnace size variables
		const int max_slots_row = 9;
		const int max_slots_col = 4;
		const int slot_size = 8;
		const int padding = 2;
		const int slot_offset_from_center = 20;
		float slot_border_width = 1;
		int slot_index = 0;
		int row_index = 0;

		const Vector2 furnace_ui_size = v2((max_slots_row * slot_size) + (max_slots_row * padding) + padding, (max_slots_col * slot_size) + (max_slots_col * padding) + padding);
		Vector2 furnace_ui_pos = v2(screen_width * 0.5, screen_height * 0.5);
		furnace_ui_pos = v2(furnace_ui_pos.x - (furnace_ui_size.x * 0.5), furnace_ui_pos.y - (furnace_ui_size.y * 0.5));

		const float x_start_pos = furnace_ui_pos.x;
		const float y_start_pos = furnace_ui_pos.y;

		// Colors
		Vector4 furnace_bg = v4(0.15, 0.15, 0.15, 0.8);
		Vector4 slot_border_color = v4(1, 1, 1, 0.7);
		// Vector4 slot_color = v4(1, 1, 1, 0.7);

		world->furnace_alpha_target = (world->ux_state == UX_furnace ? 1.0 : 0.0);
		animate_f32_to_target(&world->furnace_alpha, world->furnace_alpha_target, delta_t, 15.0);
		bool is_furnace_enabled = world->furnace_alpha_target == 1.0;


		if (world->furnace_alpha_target != 0.0)
		{
			Matrix4 xform_input_slot = m4_identity;
			Matrix4 xform_output_slot = m4_identity;
			Matrix4 xform_fuel_slot = m4_identity;
			Matrix4 xform_bg = m4_identity;

			xform_input_slot = m4_translate(xform_input_slot, v3(furnace_ui_pos.x + (furnace_ui_size.x * 0.5)- (slot_size * 0.5) - slot_offset_from_center, 	furnace_ui_pos.y + (furnace_ui_size.y * 0.5) - (slot_size * 0.5) + (padding * 5), 0));
			xform_output_slot = m4_translate(xform_output_slot, v3(furnace_ui_pos.x + (furnace_ui_size.x * 0.5)- (slot_size * 0.5) + slot_offset_from_center, 	furnace_ui_pos.y + (furnace_ui_size.y * 0.5) - (slot_size * 0.5), 0));
			xform_fuel_slot = m4_translate(xform_fuel_slot, v3(furnace_ui_pos.x + (furnace_ui_size.x * 0.5)- (slot_size * 0.5) - slot_offset_from_center, 		furnace_ui_pos.y + (furnace_ui_size.y * 0.5) - (slot_size * 0.5) - (padding * 5), 0));
			xform_bg = m4_translate(xform_bg, v3(furnace_ui_pos.x, furnace_ui_pos.y, 0));


			// draw background
			draw_rect_xform(xform_bg, v2(furnace_ui_size.x, furnace_ui_size.y), furnace_bg);

			// draw slot border and then draw input slot
			draw_rect_xform(xform_input_slot, v2(slot_size, slot_size), slot_border_color);
			draw_rect_xform(m4_translate(xform_input_slot, v3((0.5 * slot_border_width), (0.5 * slot_border_width), 0)), v2(slot_size - slot_border_width, slot_size - slot_border_width), furnace_bg);

			// draw output slot
			draw_rect_xform(xform_output_slot, v2(slot_size, slot_size), slot_border_color);
			draw_rect_xform(m4_translate(xform_output_slot, v3((0.5 * slot_border_width), (0.5 * slot_border_width), 0)), v2(slot_size - slot_border_width, slot_size - slot_border_width), furnace_bg);


			// draw fuel slot
			draw_rect_xform(xform_fuel_slot, v2(slot_size, slot_size), slot_border_color);
			draw_rect_xform(m4_translate(xform_fuel_slot, v3((0.5 * slot_border_width), (0.5 * slot_border_width), 0)), v2(slot_size - slot_border_width, slot_size - slot_border_width), furnace_bg);


			// draw text above slots
			draw_text_xform(font, STR("INPUT"), 	font_height, m4_translate(xform_input_slot, 	v3(1, slot_size + padding, 0)), v2(0.05, 0.05), COLOR_WHITE);
			draw_text_xform(font, STR("FUEL"), 		font_height, m4_translate(xform_fuel_slot, 		v3(1, slot_size + padding, 0)), v2(0.05, 0.05), COLOR_WHITE);
			draw_text_xform(font, STR("OUTPUT"), 	font_height, m4_translate(xform_output_slot, 	v3(0, slot_size + padding, 0)), v2(0.05, 0.05), COLOR_WHITE);

			

		}




	}



	set_world_space();
	pop_z_layer();
}




// ----- ::SPAWN BIOME -----------------------------------------------------------------------------|

// #biome
void spawn_biome(BiomeData* biome) {
	if (biome->spawn_pine_trees) {create_pine_trees((int)biome->spawn_pine_tree_weight, biome->size.x); }
	if (biome->spawn_spruce_trees) {create_spruce_trees((int)biome->spawn_spruce_tree_weight, biome->size.x); }
	if (biome->spawn_rocks) {create_rocks((int)biome->spawn_rocks_weight, biome->size.x); }
	if (biome->spawn_berries) {create_bushes((int)biome->spawn_berries_weight, biome->size.x); }
	if (biome->spawn_twigs) {create_twigs((int)biome->spawn_twigs_weight, biome->size.x); }
	if (biome->spawn_mushrooms) {create_mushrooms((int)biome->spawn_mushrooms_weight, biome->size.x); }
	if (biome->has_portals) {
		for (int i = 0; i < biome->portal_count; i++){
			PortalData* portal = &biome->portals;
			if (portal->enabled){
				// continue 2
				// create_portal_to(biome->portals[i].destination, true);
			}
		}
	}
	if (biome->spawn_ores) {
		if (biome->spawn_ore_iron) {create_ores((int)biome->spawn_ore_iron_weight, biome->size.x, ORE_iron); }
		if (biome->spawn_ore_gold) {create_ores((int)biome->spawn_ore_gold_weight, biome->size.x, ORE_gold); }
		if (biome->spawn_ore_copper) {create_ores((int)biome->spawn_ore_copper_weight, biome->size.x, ORE_copper); }
	}
}

void unload_biome(World* world, BiomeID id){
	// world->entities
	int removed_entity_count = 0;
	for (int i = 0; i < world->dimension->entity_count; i++)  {
		// Entity* en = &world->entities[i];
		Entity* en = &world->dimension->entities[i];
		for (int i = 0; i < BIOME_MAX; i++){
			if (en->arch == ARCH_portal){
				en->portal_data.enabled = false;
			}
			if (en->biome_ids[i] == id){
				// dealloc(get_heap_allocator(), &en);
				// memset(&en, 0, sizeof(en)); // i dont know what im doing

				// entity_destroy(en);
				entity_clear(en);
				removed_entity_count++;
			}
		}
	}
	world->dimension->entity_count -= removed_entity_count;
}

// #Biome || :change biome
void change_biomes(World* world, BiomeID new_id){
	
	printf("Changing biome: %s -> %s\n", get_biome_data_from_id(world->dimension->biome_id).name, get_biome_data_from_id(new_id).name);
	
	// dealloc all entities
	// set new biome to world
	// spawn new entities based on biome

	// world->entities;

	if (new_id == BIOME_forest){
		int asdads = 1;
	}

	set_portal_valid(0);

	unload_biome(world, world->dimension->biome_id);

	// world->entities;

	world->dimension->biome_id = new_id;
	BiomeData biome = get_biome_data_from_id(new_id);
	spawn_biome(&biome);

	clear_empty_slots_in_entities(&world->dimension->entities, MAX_ENTITY_COUNT);
	// spawn portal?!?!?!?

	// set portal to valid
	set_portal_valid(1);

}

void change_dimensions(DimensionID new_dim){
	world->dimension_id = new_dim;
}


// :Audio player ----------------------------------|
void setup_audio_player(){
	Audio_Playback_Config config = {0};
	config.volume                = 0.5;
	config.playback_speed        = 1.0;
	config.enable_spacialization = true;
}


void sort_entities_by_prio_and_y(Entity* entities, int count) {
    // sorts entities:
    // primary sort: sort entities based on their rendering_prio value
    // secondary sort: if rendering_prio is the same, sort based on their y coordinates in descending order

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int prio1 = entities[j].rendering_prio;
            int prio2 = entities[j + 1].rendering_prio;
            int y1 = entities[j].pos.y;
            int y2 = entities[j + 1].pos.y;

            // Sort by rendering priority first (ascending order)
            // If the priorities are equal, sort by y value (descending order)
            if (prio1 > prio2 || (prio1 == prio2 && y1 < y2)) {
                Entity temp = entities[j];
                entities[j] = entities[j + 1];
                entities[j + 1] = temp;
            }
        }
    }

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


void sort_entity_indices_by_prio_and_y(int* indices, Entity* entities, int count) {
	// sorts entities:
	// primary sort: sort entities based on their rendering_prio value
	// secondary sort: if rendering_prio is the same, sort based on their y coordinates in descending order

	// printf("--> SORTER ENTITIES <--\n");

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int prio1 = entities[indices[j]].rendering_prio;
            int prio2 = entities[indices[j + 1]].rendering_prio;
            int y1 = entities[indices[j]].pos.y;
            int y2 = entities[indices[j + 1]].pos.y;

            // Sort by rendering priority first (ascending order)
            // If the priorities are equal, sort by y value (descending order)
            if (prio1 > prio2 || (prio1 == prio2 && y1 < y2)) {
                int temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
            }
        }
    }
}

/*
	// #renderlist || #render list || #render_list
	// void update_entity_render_order(RenderList* render_list, Entity* entities, int entity_count) {
	//     if (render_list->needs_sorting) {
	// 		// int entity_count = world->entity_count + 1;

	// 	    // // Create an array of indices
	// 		// int indices[entity_count];
	// 		// for (int i = 0; i < entity_count; i++) {
	// 		// 	indices[i] = i;
	// 		// }

	//         sort_entity_indices_by_prio_and_y(render_list->indices, entities, render_list->count);
	//         render_list->needs_sorting = false;
	//     }
	// }

	// // #renderlist || #render list || #render_list
	// void add_entity_to_renderlist(RenderList* render_list, int entity_index) {
	//     // Check if the entity is already in the render list to avoid duplicates
	//     for (int i = 0; i < render_list->count; i++) {
	//         if (render_list->indices[i] == entity_index) {
	//             return; // Entity is already in the render list
	//         }
	//     }

	//     // Add the entity index to the render list
	//     render_list->indices[render_list->count++] = entity_index;

	//     // Mark the list as needing sorting
	//     render_list->needs_sorting = true;
	// }

	// // #renderlist || #render list || #render_list
	// void remove_entity_from_renderlist(RenderList* render_list, int entity_index) {
	//     // Find the entity in the list and remove it (shifting elements if necessary)
	//     for (int i = 0; i < render_list->count; i++) {
	//         if (render_list->indices[i] == entity_index) {
	//             // Shift elements down to fill the gap
	//             for (int j = i; j < render_list->count - 1; j++) {
	//                 render_list->indices[j] = render_list->indices[j + 1];
	//             }
	//             render_list->count--;
	//             render_list->needs_sorting = true;
	//             break;
	//         }
	//     }
	// }
*/

// :Render entities
void render_entities(World* world) {

	// NOTE: its cheaper to use "world->entity_count" here instead of "MAX_ENTITY_COUNT". But if world->entity_count is used, the "first item spawned is invisible" bug happens
	// NOTE: bugfix for this is to do "world->entity_count + 1". Don't know about the side effects of this fix tho
	int entity_count = world->dimension->entity_count + 1;

    // Create an array of indices
    int indices[entity_count];
    for (int i = 0; i < entity_count; i++) {
        indices[i] = i;
    }

    // Sort the indices based on the y coordinates AND the rendering prios of the entities 
	// NOTE: sorting the entities drops the fps like 2500 - 4000
	// ??: fix idea: cache the sorted entities and sort them again when changes occur. NOT EVERY DAMN FRAME
	// sort_entity_indices_by_prio_and_y(indices, world->entities, entity_count);
	// update_entity_render_order(&render_list, world->entities, render_list.count);


	render_list.needs_sorting = true;

	if (world->dimension->biome_id == BIOME_cave){
		int asd = 1;
	}

	if (render_list.needs_sorting){
		// sort_entity_indices_by_prio_and_y(render_list.indices, world->entities, render_list.count);
		sort_entity_indices_by_prio_and_y(indices, &world->dimension->entities, entity_count);
		// sort_entities_by_prio_and_y(world->entities, entity_count);
		render_list.needs_sorting = false;
	}

   for (int i = 0; i < entity_count; i++)  {

		int index = indices[i];
		Entity* en = &world->dimension->entities[index];
		// Entity* en = &world->entities[i];

		if (en->is_valid) {

			float entity_dist_from_player = fabsf(v2_dist(en->pos, get_player_pos()));


			switch (en->arch) {

				// :Render player
				case ARCH_player: {
					if (render_player)
					{
						// render player
						Sprite* sprite = get_sprite(en->sprite_id);
						Matrix4 xform = m4_identity;

						xform = m4_translate(xform, v3(0, tile_width * -0.5, 0));
						xform = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
						xform = m4_translate(xform, v3(sprite->image->width * -0.5, 0.0, 0));

						// draw shadow
						if (en->enable_shadow){
							Vector2 shadow_pos = en->pos;
							shadow_pos.x = shadow_pos.x - (0.5 * get_sprite_size(sprite).x);
							shadow_pos.y = shadow_pos.y - (0.75 * get_sprite_size(sprite).y);
							draw_circle(shadow_pos, v2(get_sprite_size(sprite).x, 4.0), entity_shadow_color);
						}

						// draw sprite
						draw_image_xform(sprite->image, xform, get_sprite_size(sprite), COLOR_WHITE);

						// :Render held item
						if (selected_item != NULL && selected_item->valid){

							Sprite* sprite_held_item = get_sprite(selected_item->sprite_id);
							Matrix4 xform_held_item = m4_scalar(1.0);
							xform_held_item = m4_translate(xform_held_item, v3(en->pos.x, en->pos.y, 0));
							xform_held_item = m4_translate(xform_held_item, v3(0, -3, 0));

							draw_image_xform(sprite_held_item->image, xform_held_item, v2(5, 5), COLOR_WHITE);
						}

						// draw debug text
						if (IS_DEBUG){
							xform = m4_translate(xform, v3(-3.5, -3, 0));
							draw_text_xform(font, STR("DEBUG"), font_height, xform, v2(0.1, 0.1), COLOR_RED);
						}
					}

				} break;

				case ARCH_portal:{
					{
						// frustrum culling
						if (entity_dist_from_player <= render_distance){
							for (int i = 0; i < en->biome_count; i++) {
								BiomeID portal_biome_id = en->biome_ids[i];
								if (portal_biome_id == world->dimension->biome_id){
									// printf("Drawing sprite at biome = %s\t", get_biome_data_from_id(portal_biome_id).name);
									// printf("portal destination id: %d\n", en->portal_data.destination);
									Sprite* sprite = get_sprite(en->sprite_id);
									Matrix4 xform = m4_identity;

									xform = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));

									// center sprite
									xform = m4_translate(xform, v3(sprite->image->width * -0.5, sprite->image->height * -0.5, 0));

									// draw sprite
									draw_image_xform(sprite->image, xform, get_sprite_size(sprite), COLOR_WHITE);

								if (IS_DEBUG){
									Matrix4 xform_debug = m4_identity;
									xform_debug = m4_translate(xform_debug, v3(en->pos.x, en->pos.y, 0));
									draw_rect_xform(xform_debug, v2(1,1), COLOR_RED);
								}

								}
							}
						}
					}
				} break;

				default: 
				{
					if (render_other_entities){
					// if (en->arch != ARCH_player){

						if (en->arch == ARCH_portal){
							printf("FORCED CRASH @ 'render_entities'\n ");
							float i = 1/0;
						}

						// if (en->is_item = true && en->item_id == ITEM_pine_wood){
						// 	int asdasd = 1;
						// }

						// frustrum culling
						if (entity_dist_from_player <= render_distance){

							Sprite* sprite = get_sprite(en->sprite_id);
							Matrix4 xform = m4_identity;
							
							// ITEM
							if (en->is_item) {
								xform         = m4_translate(xform, v3(0, 2.0 * sin_breathe(os_get_current_time_in_seconds(), 5.0), 0)); // bob item up and down
								
								// shadow position
								Vector2 position = en->pos;
								position.x = position.x - (0.5 * get_sprite_size(sprite).x);
								position.y = position.y - (0.5 * get_sprite_size(sprite).y) - 1;
								
								// item shadow
								draw_circle(position, v2(get_sprite_size(sprite).x, get_sprite_size(sprite).y * 0.5), item_shadow_color);

							}
							else{
								xform         = m4_translate(xform, v3(0, tile_width * -0.5, 0));			// bring sprite down to bottom of Tile if not item

								if (en->enable_shadow){
									Vector2 shadow_pos = en->pos;
									shadow_pos.x = shadow_pos.x - (0.5 * get_sprite_size(sprite).x);
									// shadow_pos.y = shadow_pos.y - (0.75 * get_sprite_size(sprite).y);
									shadow_pos.y -= 6;
									draw_circle(shadow_pos, v2(get_sprite_size(sprite).x, 4.0), entity_shadow_color);
								}
							}
							
							if (en->building_id){
								// printf("BUILDING\n");
							}

							xform         = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
							xform         = m4_translate(xform, v3(sprite->image->width * -0.5, 0.0, 0));

							Vector4 col = COLOR_WHITE;
							if (world_frame.selected_entity == en){
								col = v4(0.7, 0.7, 0.7, 1.0);
							}

							// draw sprite
							draw_image_xform(sprite->image, xform, get_sprite_size(sprite), col);
							

							if (IS_DEBUG){
								// draw_text(font, sprint(temp_allocator, STR("%.0f, %.0f"), en->pos.x, en->pos.y), 40, en->pos, v2(0.1, 0.1), COLOR_WHITE);
								Matrix4 xform_debug = m4_identity;
								xform_debug = m4_translate(xform_debug, v3(en->pos.x, en->pos.y, 0));
								// xform_debug = m4_translate(xform_debug, v3(sprite->image->width * 0.5, sprite->image->height * 0.5, 0));

								// draw_rect_xform(xform, v2(1,1), COLOR_RED);
								draw_rect_xform(xform_debug, v2(1,1), COLOR_RED);
							}
						}
					// }
					}
				} break;
			}
		}
	}
}


void render_keybinding(Entity* en, char keybind) {
	Matrix4 xform = m4_identity;
	xform = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
	xform = m4_translate(xform, v3(0, 10, 0));

	draw_text_xform(font, sprint(temp_allocator, STR("%c"), keybind), font_height, xform, v2(0.1, 0.1), COLOR_WHITE);
}


// ----- SETUP -----------------------------------------------------------------------------------------|
// ::Entry
int entry(int argc, char **argv) 
{


	// Window
	window.title = STR("Game.");
	window.scaled_width = 1280; // We need to set the scaled size if we want to handle system scaling (DPI)
	window.scaled_height = 720; 

	// window spawn position
	window.x = window.width * 0.5 + 350;		// window.x = 200; // default value // +350 so i can see console
	window.y = window.height * 0.5;				// window.y = 200; // default value

	// bg color
	window.clear_color = hex_to_rgba(0x43693aff);
	// window.clear_color = v4(1,1,1,1);

	// window.enable_vsync = true;

	// Memory
	world = alloc(get_heap_allocator(), sizeof(World));
	memset(world, 0, sizeof(World));
	// world->entity_count = 0;

	// :Init Render List 	#renderlist || #render list || #render_list
	// render_list.indices = (int*)alloc(get_heap_allocator(), MAX_ENTITY_COUNT * sizeof(int));
	// render_list.count = 0;
	// render_list.needs_sorting = true;

	// Audio
	Audio_Player *audio_player = audio_player_get_one();

	// Font
	font = load_font_from_disk(STR("C:/windows/fonts/arial.ttf"), get_heap_allocator());
	assert(font, "Failed loading arial.ttf, %d", GetLastError());

	// :LOAD RESOURCES --------------------------------->

	// :Load entity sprites
	sprites[0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/missing_texture.png"), get_heap_allocator())};
	sprites[SPRITE_player] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/player.png"), get_heap_allocator())};
	sprites[SPRITE_tree_pine] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tree_pine.png"), get_heap_allocator())};
	sprites[SPRITE_tree_spruce] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tree_spruce.png"), get_heap_allocator())};
	sprites[SPRITE_rock0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/rock0.png"), get_heap_allocator())};
	sprites[SPRITE_rock1] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/rock1.png"), get_heap_allocator())};
	sprites[SPRITE_rock2] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/rock2.png"), get_heap_allocator())};
	sprites[SPRITE_rock3] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/rock3.png"), get_heap_allocator())};
	sprites[SPRITE_bush0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/bush0.png"), get_heap_allocator())};
	sprites[SPRITE_bush1] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/bush1.png"), get_heap_allocator())};
	sprites[SPRITE_tall_grass0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tall_grass0.png"), get_heap_allocator())};
	sprites[SPRITE_tall_grass1] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tall_grass1.png"), get_heap_allocator())};
	sprites[SPRITE_portal] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/portal0.png"), get_heap_allocator())};

	// :Load item/entity sprites (these sprites are the same for their entities and items (for now))
	sprites[SPRITE_mushroom0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/mushroom0.png"), get_heap_allocator())};
	sprites[SPRITE_ORE_iron] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/ore_iron.png"), get_heap_allocator())};
	sprites[SPRITE_ORE_gold] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/ore_gold.png"), get_heap_allocator())};
	sprites[SPRITE_ORE_copper] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/ore_copper.png"), get_heap_allocator())};

	// :Load item sprites
	sprites[SPRITE_item_rock] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_rock.png"), get_heap_allocator())};
	sprites[SPRITE_item_pine_wood] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_pine_wood.png"), get_heap_allocator())};
	sprites[SPRITE_item_sprout] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_sprout.png"), get_heap_allocator())};
	sprites[SPRITE_item_berry] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_berry.png"), get_heap_allocator())};
	sprites[SPRITE_item_twig] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_twig.png"), get_heap_allocator())};
	sprites[SPRITE_item_fossil0] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_fossil0.png"), get_heap_allocator())};
	sprites[SPRITE_item_fossil1] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_fossil1.png"), get_heap_allocator())};
	sprites[SPRITE_item_fossil2] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/item_fossil2.png"), get_heap_allocator())};

	// :Load tool sprites
	sprites[SPRITE_tool_pickaxe] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tool_pickaxe.png"), get_heap_allocator())};
	sprites[SPRITE_tool_axe] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tool_axe.png"), get_heap_allocator())};
	sprites[SPRITE_tool_shovel] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/tool_shovel.png"), get_heap_allocator())};

	// :Load building sprites
	sprites[SPRITE_building_furnace] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/building_furnace.png"), get_heap_allocator())};
	sprites[SPRITE_building_workbench] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/building_workbench.png"), get_heap_allocator())};
	sprites[SPRITE_building_chest] = (Sprite){ .image=load_image_from_disk(STR("res/sprites/building_chest.png"), get_heap_allocator())};



	// :Load textures
	textures[TEXTURE_grass] = (Texture){ .image=load_image_from_disk(STR("res/textures/grass.png"), get_heap_allocator())};
	textures[TEXTURE_cave_floor] = (Texture){ .image=load_image_from_disk(STR("res/textures/cave_floor.png"), get_heap_allocator())};



	// :Load audio
	Audio_Source hit_metal1, hit_metal2, rock_breaking1, swing_slow, swing_fast;
	audioFiles[AUDIO_hit_metal1] = (Audio){.name=STR("Hit metal 1"),.ok=audio_open_source_load(&hit_metal1, STR("res/sounds/hit_metal1.wav"), get_heap_allocator()),.path=STR("res/sounds/hit_metal1.wav"),.source=hit_metal1};
	audioFiles[AUDIO_hit_metal2] = (Audio){.name=STR("Hit metal 2"),.ok=audio_open_source_load(&hit_metal2, STR("res/sounds/hit_metal2.wav"), get_heap_allocator()),.path=STR("res/sounds/hit_metal2.wav"),.source=hit_metal2};
	audioFiles[AUDIO_rock_breaking1] = (Audio){.name=STR("Rock Breaking 1"),.ok=audio_open_source_load(&rock_breaking1, STR("res/sounds/rock_breaking1.wav"), get_heap_allocator()),.path=STR("res/sounds/rock_breaking1.wav"),.source=rock_breaking1};
	audioFiles[AUDIO_swing_slow] = (Audio){.name=STR("Swing slow"),.ok=audio_open_source_load(&swing_slow, STR("res/sounds/swing_slow.wav"), get_heap_allocator()),.path=STR("res/sounds/swing_slow.wav"),.source=swing_slow};
	audioFiles[AUDIO_swing_fast] = (Audio){.name=STR("Swing fast"),.ok=audio_open_source_load(&swing_fast, STR("res/sounds/swing_fast.wav"), get_heap_allocator()),.path=STR("res/sounds/swing_fast.wav"),.source=swing_fast};



	// assert all sprites			@ship debug this off (by: randy)
	{
		// crash when image fails to be setup properly (by: randy)
		for (SpriteID i = 0; i < SPRITE_MAX; i++) {
			Sprite* sprite = &sprites[i];
			assert(sprite->image, "Sprite was not setup correctly");
		}
	}
	
	// assert all texture files		@ship debug this off (by: jani)
	{
		for (TextureID i = 1; i < TEXTURE_MAX; i++) {
			Texture* texture = &textures[i];
			assert(texture->image, "Texture was not setup correctly");
		}
	}
	
	// assert all audio files		@ship debug this off (by: jani)
	{
		for (AudioID i = 1; i < AUDIO_MAX; i++) {
			Audio* audio = &audioFiles[i];
			assert(audio->ok, "Audio was not setup correctly: '%s'", audio->name);
			log_verbose("Audio file set up '%s'", audio->name);
		}
	}


	// setup dimensions
	DimensionData *dimension_overworld = alloc(get_heap_allocator(), sizeof(DimensionData));
	setup_dimension(dimension_overworld, STR("overworld"), DIM_overworld);
	DimensionData *dimension_cavern = alloc(get_heap_allocator(), sizeof(DimensionData));
	setup_dimension(dimension_cavern, STR("cavern"), DIM_cavern);

	// set current dimension
	world->dimension = dimension_overworld;



	// Building resource setup
	{
		buildings[BUILDING_furnace] = (BuildingData){.to_build=ARCH_building,. icon=SPRITE_building_furnace};
		buildings[BUILDING_workbench] = (BuildingData){.to_build=ARCH_building,. icon=SPRITE_building_workbench};
		buildings[BUILDING_chest] = (BuildingData){.to_build=ARCH_building,. icon=SPRITE_building_chest};
	}


	// :TESTS
	{
		{
			// test adding items to inventory
			add_item_to_inventory(ITEM_TOOL_pickaxe, STR("Pickaxe"), 1, ARCH_tool, SPRITE_tool_pickaxe, TOOL_pickaxe, true);
			add_item_to_inventory(ITEM_TOOL_axe, STR("Axe"), 1, ARCH_tool, SPRITE_tool_axe, TOOL_axe, true);
			add_item_to_inventory(ITEM_TOOL_shovel, STR("Shovel"), 1, ARCH_tool, SPRITE_tool_shovel, TOOL_shovel, true);
			// add_item_to_inventory(ITEM_rock, STR("Rock"), 5, ARCH_item, SPRITE_item_rock, TOOL_nil, true);
			// add_item_to_inventory(ITEM_pine_wood, STR("Pine wood"), 10, ARCH_item, SPRITE_item_pine_wood, TOOL_nil, true);

		}
		
		// test adding buildings to world
		{
			// FURNACE:
			{
				Entity* en = entity_create();
				setup_building(en, BUILDING_furnace);
				en->pos = v2(-25, 0);
				en->pos = round_v2_to_tile(en->pos);
			}


			// CHEST:
			{
				Entity* en = entity_create();
				setup_building(en, BUILDING_chest);
				en->pos = v2(-25, -10);
				en->pos = round_v2_to_tile(en->pos);
			}
		}
	}




	// ::INIT

	dragging_now = (InventoryItemData*)alloc(get_heap_allocator(), sizeof(InventoryItemData));

	setup_audio_player();

	// :Create player entity
	Entity* player_en = entity_create();
	setup_player(player_en, v2(0, 0));

	// setup all biomes && #Biome
	setup_all_biomes();

	// set current biome
	world->dimension->biome_id = BIOME_forest;

	BiomeData temp_data = get_biome_data_from_id(world->dimension->biome_id);
	spawn_biome(&temp_data);
	memset(&temp_data, 0, sizeof(temp_data)); // i dont know what im doing

	// test adding stuff to loot table (can't be in a scope) 
	// FIX: @pin2 im defining item names in multiple different places eg.A: here
	LootTable *lootTable_rock = createLootTable();
	addItemToLootTable(lootTable_rock, &STR("Stone"), ITEM_rock, 100);
	addItemToLootTable(lootTable_rock, &STR("Ammonite Fossil"), ITEM_fossil0, get_biome_data_from_id(world->dimension->biome_id).fossil0_drop_chance);
	addItemToLootTable(lootTable_rock, &STR("Bone Fossil"), ITEM_fossil1, get_biome_data_from_id(world->dimension->biome_id).fossil1_drop_chance);
	addItemToLootTable(lootTable_rock, &STR("Fang Fossil"), ITEM_fossil2, get_biome_data_from_id(world->dimension->biome_id).fossil2_drop_chance);
	// addItemToLootTable(lootTable_rock, &STR("asd"), ARCH_nil, 10.0); // this line makes it so fossils dont spawn. bug?


	// Timing
	float64 seconds_counter = 0.0;
	s32 frame_count = 0;
	float64 last_time = os_get_current_time_in_seconds();


	float view_zoom = 0.1875; 			// view zoom ratio x (pixelart layer width / window width = 240 / 1280 = 0.1875)
	Vector2 camera_pos = v2(0, 0);



	// view_zoom += 0.2;		// zoom out a bit


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

		// find player entity
		for (int i = 0; i < MAX_ENTITY_COUNT; i++) {
			Entity* en = &world->dimension->entities[i];
			if (en->is_valid && en->arch == ARCH_player) {
				world_frame.player = en;
			}
		}

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


		// :Entity selection by MOUSE
		if (!world_frame.hover_consumed)
		{	
			// log("%f, %f", input_frame.mouse_x, input_frame.mouse_y);
			// draw_text(font, sprint(temp, STR("%.0f, %.0f"), pos.x, pos.y), font_height, pos, v2(0.1, 0.1), COLOR_RED);

			// float smallest_dist = INFINITY; // compiler gives a warning
			float smallest_dist = 9999999;

			// for (int i = 0; i < MAX_ENTITY_COUNT; i++){
			for (int i = 0; i < world->dimension->entity_count; i++){
				Entity* en = &world->dimension->entities[i];

				if (IS_DEBUG){
					// world_frame.selected_entity = en;
					if (en->is_valid){
						float dist = fabsf(v2_dist(en->pos, mouse_pos_world));
						if (dist < entity_selection_radius){
							if (!world_frame.selected_entity || (dist < smallest_dist)){
								printf("EN = %s\n", en->name);
								if (IS_DEBUG){
									int asd = 1;
								}
							}
						}
					}
				}

				// portal
				if (en->arch == ARCH_portal){
					// if (en->is_valid){
					// 	float dist = fabsf(v2_dist(en->pos, mouse_pos_world));
					// 	if (dist < entity_selection_radius){
					// 		if (!world_frame.selected_entity || (dist < smallest_dist)){
					// 			// printf("PORTAL SELECTED\n");
					// 			world_frame.selected_entity = en;
					// 		}
					// 	}
					// }
				}

				else if (en->is_valid && en->destroyable){
					// Sprite* sprite = get_sprite(en->sprite_id);

					// int entity_tile_x = world_pos_to_tile_pos(en->pos.x);
					// int entity_tile_y = world_pos_to_tile_pos(en->pos.y);

					float dist = fabsf(v2_dist(en->pos, mouse_pos_world));

					// :select entity
					if (dist < entity_selection_radius) {
						if (!world_frame.selected_entity || (dist < smallest_dist)) {
							world_frame.selected_entity = en;
							// smallest_dist = dist; // imo entity selection works better with this line commented
						}
					}
				}
			}
		}


		// :Entity selection by player position
		{
			float smallest_dist = 9999999;
			float entity_selection_radius = 10.0f;

			for (int i = 0; i < world->dimension->entity_count; i++){
				Entity* en = &world->dimension->entities[i];
				if (en->arch == ARCH_portal){ // #portal
					if (en->is_valid){
						float dist = fabsf(v2_dist(en->pos, get_player_pos()));
						if (dist < entity_selection_radius){
							if (!world_frame.selected_entity || (dist < smallest_dist)){
								// printf("PORTAL SELECTED\n");
								// printf("%.1f, %.1f\n", en->pos.x, en->pos.y);
								world_frame.selected_entity = en;
							}
						}
					}
				}
			}
		}


		// Render building ui
		{
			if (world_frame.selected_entity && world_frame.selected_entity->arch == ARCH_building){
					// open chest
				if (is_key_just_pressed(MOUSE_BUTTON_RIGHT)) {
					consume_key_just_pressed(MOUSE_BUTTON_RIGHT);

					// world->ux_state = (world->ux_state == UX_chest ? UX_nil : UX_chest);
					Entity* selected_en = world_frame.selected_entity;
					switch (selected_en->building_id){
						case BUILDING_chest: {world->ux_state = UX_chest;}break;
						case BUILDING_furnace: {world->ux_state = UX_furnace;}break;
						default:{}break;
					}
				}
			}

			if (world->ux_state != UX_nil){
				render_building_ui(world->ux_state);
			}
		}


		// :Render grid (:Grid)
		if (draw_grid)
		{	
			// NOTE: rendering tiles has a big fkin impact on fps 
			int player_tile_x = world_pos_to_tile_pos(player_en->pos.x);
			int player_tile_y = world_pos_to_tile_pos(player_en->pos.y);
			int tile_radius_x = 18;
			int tile_radius_y = 12;

			for (int x = player_tile_x - tile_radius_x; x < player_tile_x + tile_radius_x; x++){
				for (int y = player_tile_y - tile_radius_y; y < player_tile_y + tile_radius_y; y++){
					if ((x +(y % 2 == 0) ) % 2 == 0) {
						Vector4 col = v4(0.1, 0.1, 0.1, 0.1);
						// Vector4 col = forest->grass_color;
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


		// :Draw ground texture || :draw_ground 
		if (render_ground_texture)
		{

			// this shit way too complex
			// disgusting

			// int player_tile_x = world_pos_to_tile_pos(player_en->pos.x);
			// int player_tile_y = world_pos_to_tile_pos(player_en->pos.y);
			float player_pos_x = player_en->pos.x;
			float player_pos_y = player_en->pos.y;

			// Texture* texture = get_texture(TEXTURE_grass);
			biomes;
			
			Texture* texture = get_texture(get_biome_data_from_id(world->dimension->biome_id).ground_texture);

			// xfrom logic:
			// A 	 B		C
			// D 	 M 	 	E
			// F 	 G		H

			Matrix4 xform_M = m4_identity;
			Matrix4 xform_A = m4_identity;
			Matrix4 xform_B = m4_identity;
			Matrix4 xform_C = m4_identity;
			Matrix4 xform_D = m4_identity;
			Matrix4 xform_E = m4_identity;
			Matrix4 xform_F = m4_identity;
			Matrix4 xform_G = m4_identity;
			Matrix4 xform_H = m4_identity;

			// attempt #4
			// Vector2 texture_size = get_texture_size(texture); // old
			Vector2 texture_size = get_texture_size(*texture); // new

			float traveled_step_x = (int)((player_pos_x / (texture_size.x * 2)) * 2) * texture_size.x;
			float traveled_step_y = (int)((player_pos_y / (texture_size.y * 2)) * 2) * texture_size.y;
			bool x_axis_fix = false;
			
			// x-axis fix
			if (player_pos_y < 0 && traveled_step_y == 0 && traveled_step_x != 0){
				x_axis_fix = true;
			}

			if (traveled_step_x < 0){ 			// left
				// printf("LEFT ");
				if (traveled_step_y < 0){
					// printf("DOWN \n");
					xform_M = m4_translate(xform_M, v3(traveled_step_x - texture_size.x		,	traveled_step_y - texture_size.y			, 0));
					xform_A = m4_translate(xform_A, v3(traveled_step_x - texture_size.x * 2	,	traveled_step_y 							, 0));
					xform_B = m4_translate(xform_B, v3(traveled_step_x - texture_size.x		,	traveled_step_y 							, 0));
					xform_C = m4_translate(xform_C, v3(traveled_step_x						,	traveled_step_y 							, 0));
					xform_D = m4_translate(xform_D, v3(traveled_step_x - texture_size.x * 2	,	traveled_step_y - texture_size.y			, 0));
					xform_E = m4_translate(xform_E, v3(traveled_step_x						,	traveled_step_y - texture_size.y			, 0));
					xform_F = m4_translate(xform_F, v3(traveled_step_x - texture_size.x * 2	,	traveled_step_y - (texture_size.y * 2)		, 0));
					xform_G = m4_translate(xform_G, v3(traveled_step_x - texture_size.x		,	traveled_step_y - (texture_size.y * 2)		, 0));
					xform_H = m4_translate(xform_H, v3(traveled_step_x						,	traveled_step_y - (texture_size.y * 2)		, 0));
				}
				else{
					// printf("UP \n");
					xform_M = m4_translate(xform_M, v3(traveled_step_x - texture_size.x		, 	traveled_step_y								, 0));
					xform_A = m4_translate(xform_A, v3(traveled_step_x - texture_size.x * 2	, 	traveled_step_y + texture_size.y			, 0));
					xform_B = m4_translate(xform_B, v3(traveled_step_x - texture_size.x		, 	traveled_step_y + texture_size.y			, 0));
					xform_C = m4_translate(xform_C, v3(traveled_step_x						, 	traveled_step_y + texture_size.y			, 0));
					xform_D = m4_translate(xform_D, v3(traveled_step_x - texture_size.x * 2	, 	traveled_step_y								, 0));
					xform_E = m4_translate(xform_E, v3(traveled_step_x						, 	traveled_step_y								, 0));
					xform_F = m4_translate(xform_F, v3(traveled_step_x - texture_size.x * 2	, 	traveled_step_y - texture_size.y			, 0));
					xform_G = m4_translate(xform_G, v3(traveled_step_x - texture_size.x		, 	traveled_step_y - texture_size.y			, 0));
					xform_H = m4_translate(xform_H, v3(traveled_step_x						, 	traveled_step_y - texture_size.y			, 0));
				}
			}
			
			else if (traveled_step_x == 0){ 	// middle x = 0
				// printf("MIDDLE\n");
				if (player_pos_x < 0.0f){
					if (player_pos_y < 0.0f){
						xform_M = m4_translate(xform_M, v3(-texture_size.x							, traveled_step_y - texture_size.y			, 0));
						xform_A = m4_translate(xform_A, v3(-traveled_step_x - texture_size.x * 2	, traveled_step_y							, 0));
						xform_B = m4_translate(xform_B, v3(-traveled_step_x - texture_size.x		, traveled_step_y							, 0));
						xform_C = m4_translate(xform_C, v3(-traveled_step_x							, traveled_step_y							, 0));
						xform_D = m4_translate(xform_D, v3(-traveled_step_x - texture_size.x * 2	, traveled_step_y - texture_size.y			, 0));
						xform_E = m4_translate(xform_E, v3(-traveled_step_x							, traveled_step_y - texture_size.y			, 0));
						xform_F = m4_translate(xform_F, v3(-traveled_step_x - texture_size.x * 2	, traveled_step_y - texture_size.y * 2		, 0));
						xform_G = m4_translate(xform_G, v3(-traveled_step_x - texture_size.x		, traveled_step_y - texture_size.y * 2		, 0));
						xform_H = m4_translate(xform_H, v3(-traveled_step_x							, traveled_step_y - texture_size.y * 2		, 0));

					}
					else{
						xform_M = m4_translate(xform_M, v3(-texture_size.x							, traveled_step_y							, 0));
						xform_A = m4_translate(xform_A, v3(-traveled_step_x - texture_size.x * 2	, traveled_step_y + texture_size.y			, 0));
						xform_B = m4_translate(xform_B, v3(-traveled_step_x - texture_size.x		, traveled_step_y + texture_size.y			, 0));
						xform_C = m4_translate(xform_C, v3(-traveled_step_x							, traveled_step_y + texture_size.y			, 0));
						xform_D = m4_translate(xform_D, v3(-traveled_step_x - texture_size.x * 2	, traveled_step_y							, 0));
						xform_E = m4_translate(xform_E, v3(-traveled_step_x							, traveled_step_y							, 0));
						xform_F = m4_translate(xform_F, v3(-traveled_step_x - texture_size.x * 2	, traveled_step_y - texture_size.y			, 0));
						xform_G = m4_translate(xform_G, v3(-traveled_step_x - texture_size.x		, traveled_step_y - texture_size.y			, 0));
						xform_H = m4_translate(xform_H, v3(-traveled_step_x							, traveled_step_y - texture_size.y			, 0));

					}
				}
				else{
					if (player_pos_y < 0.0f){
						xform_M = m4_translate(xform_M, v3(traveled_step_x							, traveled_step_y - texture_size.y			, 0));
						xform_A = m4_translate(xform_A, v3(-traveled_step_x - texture_size.x		, traveled_step_y							, 0));
						xform_B = m4_translate(xform_B, v3(-traveled_step_x							, traveled_step_y							, 0));
						xform_C = m4_translate(xform_C, v3(-traveled_step_x + texture_size.x		, traveled_step_y							, 0));
						xform_D = m4_translate(xform_D, v3(-traveled_step_x - texture_size.x		, traveled_step_y - texture_size.y			, 0));
						xform_E = m4_translate(xform_E, v3(-traveled_step_x + texture_size.x		, traveled_step_y - texture_size.y			, 0));
						xform_F = m4_translate(xform_F, v3(-traveled_step_x - texture_size.x		, traveled_step_y - texture_size.y * 2		, 0));
						xform_G = m4_translate(xform_G, v3(-traveled_step_x							, traveled_step_y - texture_size.y * 2		, 0));
						xform_H = m4_translate(xform_H, v3(-traveled_step_x + texture_size.x		, traveled_step_y - texture_size.y * 2		, 0));

					}
					else{
						xform_M = m4_translate(xform_M, v3(traveled_step_x							, traveled_step_y							, 0));
						xform_A = m4_translate(xform_A, v3(-traveled_step_x - texture_size.x		, traveled_step_y + texture_size.y			, 0));
						xform_B = m4_translate(xform_B, v3(-traveled_step_x							, traveled_step_y + texture_size.y			, 0));
						xform_C = m4_translate(xform_C, v3(-traveled_step_x + texture_size.x		, traveled_step_y + texture_size.y			, 0));
						xform_D = m4_translate(xform_D, v3(-traveled_step_x - texture_size.x		, traveled_step_y							, 0));
						xform_E = m4_translate(xform_E, v3(-traveled_step_x + texture_size.x		, traveled_step_y							, 0));
						xform_F = m4_translate(xform_F, v3(-traveled_step_x - texture_size.x		, traveled_step_y - texture_size.y			, 0));
						xform_G = m4_translate(xform_G, v3(-traveled_step_x							, traveled_step_y - texture_size.y			, 0));
						xform_H = m4_translate(xform_H, v3(-traveled_step_x + texture_size.x		, traveled_step_y - texture_size.y			, 0));

					}
				}
			}

			else{ 								// right
				// printf("RIGHT ");
				if (traveled_step_y < 0){
					// printf("DOWN\n");
					xform_M = m4_translate(xform_M, v3(traveled_step_x 						, traveled_step_y - texture_size.y			, 0));
					xform_A = m4_translate(xform_A, v3(traveled_step_x - texture_size.x		, traveled_step_y 							, 0));
					xform_B = m4_translate(xform_B, v3(traveled_step_x						, traveled_step_y 							, 0));
					xform_C = m4_translate(xform_C, v3(traveled_step_x + texture_size.x		, traveled_step_y 							, 0));
					xform_D = m4_translate(xform_D, v3(traveled_step_x - texture_size.x 	, traveled_step_y - texture_size.y			, 0));
					xform_E = m4_translate(xform_E, v3(traveled_step_x + texture_size.x		, traveled_step_y - texture_size.y			, 0));
					xform_F = m4_translate(xform_F, v3(traveled_step_x - texture_size.x 	, traveled_step_y - (texture_size.y * 2)	, 0));
					xform_G = m4_translate(xform_G, v3(traveled_step_x						, traveled_step_y - (texture_size.y * 2)	, 0));
					xform_H = m4_translate(xform_H, v3(traveled_step_x + texture_size.x		, traveled_step_y - (texture_size.y * 2)	, 0));
				}
				else{
					// printf("UP\n");
					xform_M = m4_translate(xform_M, v3(traveled_step_x 						, traveled_step_y							, 0));
					xform_A = m4_translate(xform_A, v3(traveled_step_x - texture_size.x		, traveled_step_y + texture_size.y			, 0));
					xform_B = m4_translate(xform_B, v3(traveled_step_x						, traveled_step_y + texture_size.y			, 0));
					xform_C = m4_translate(xform_C, v3(traveled_step_x + texture_size.x		, traveled_step_y + texture_size.y			, 0));
					xform_D = m4_translate(xform_D, v3(traveled_step_x - texture_size.x 	, traveled_step_y							, 0));
					xform_E = m4_translate(xform_E, v3(traveled_step_x + texture_size.x		, traveled_step_y							, 0));
					xform_F = m4_translate(xform_F, v3(traveled_step_x - texture_size.x 	, traveled_step_y - texture_size.y			, 0));
					xform_G = m4_translate(xform_G, v3(traveled_step_x						, traveled_step_y - texture_size.y			, 0));
					xform_H = m4_translate(xform_H, v3(traveled_step_x + texture_size.x		, traveled_step_y - texture_size.y			, 0));
				}
			}

			if (x_axis_fix){
				xform_M = m4_translate(xform_M, v3(0, -texture_size.y, 0));
				xform_A = m4_translate(xform_A, v3(0, -texture_size.y, 0));
				xform_B = m4_translate(xform_B, v3(0, -texture_size.y, 0));
				xform_C = m4_translate(xform_C, v3(0, -texture_size.y, 0));
				xform_D = m4_translate(xform_D, v3(0, -texture_size.y, 0));
				xform_E = m4_translate(xform_E, v3(0, -texture_size.y, 0));
				xform_F = m4_translate(xform_F, v3(0, -texture_size.y, 0));
				xform_G = m4_translate(xform_G, v3(0, -texture_size.y, 0));
				xform_H = m4_translate(xform_H, v3(0, -texture_size.y, 0));
			}

			draw_image_xform(texture->image, xform_M, texture_size, COLOR_WHITE);
			draw_image_xform(texture->image, xform_A, texture_size, COLOR_WHITE);
			draw_image_xform(texture->image, xform_B, texture_size, COLOR_WHITE);
			draw_image_xform(texture->image, xform_C, texture_size, COLOR_WHITE);
			draw_image_xform(texture->image, xform_D, texture_size, COLOR_WHITE);
			draw_image_xform(texture->image, xform_E, texture_size, COLOR_WHITE);
			draw_image_xform(texture->image, xform_F, texture_size, COLOR_WHITE);
			draw_image_xform(texture->image, xform_G, texture_size, COLOR_WHITE);
			draw_image_xform(texture->image, xform_H, texture_size, COLOR_WHITE);
		}


		// :Update entities || :Item pick up
		{
			// bool is_pickup_text_visible = false;
			// for (int i = 0; i < MAX_ENTITY_COUNT; i++) {
			for (int i = 0; i < world->dimension->entity_count; i++) {
				Entity* en = &world->dimension->entities[i];
				if (en->is_valid) {

					// item pick up
					if (en->is_item) {
						// TODO PHYSICS

						if (fabsf(v2_dist(en->pos, player_en->pos)) < player_pickup_radius) {
							// old way
							// world->inventory_items[en->item_id].amount += 1;
							// world->inventory_items[en->item_id].name = get_item_name_from_ItemID(en->item_id);
							// world->inventory_items[en->item_id].sprite_id = en->sprite_id;
							// world->inventory_items[en->item_id].valid = true;

							// new func
							add_item_to_inventory(en->item_id, get_item_name_from_ItemID(en->item_id), 1, en->arch, en->sprite_id, en->tool_id, true);
							// printf("ADDED ITEM '%s' TO INVENTORY\n", get_item_name_from_ItemID(en->item_id));

							entity_destroy(en);

							// is_pickup_text_visible = true;
						}
					}
				}
			}
		}


		// :Player attack || :Spawn item
		{
			// @PIN1: instead of switch case, maybe just do "generateLoot(selected_en->arch, 0, selected_en->pos);"
			// and in the generateLoot func decide what loot table to use based on the passed arch

			// @pin3 can the "first item not being rendered - bug" be because of world->entity_count?? like if i break a tree and spawn the item, the entity count is + - 0, and the item entity is the final instance in the entities list.
			// UPDATE ON @pin3: this is precisely why the bug happens
			// one possible fix: https://chatgpt.com/share/8e7d0f98-8706-4f0c-8a79-7aa9f02aee56


			Entity* selected_en = world_frame.selected_entity;
			bool allow_destroy = false;


			if (is_key_just_pressed(MOUSE_BUTTON_LEFT)) {
				consume_key_just_pressed(MOUSE_BUTTON_LEFT);
				
				// Play audio
				if (selected_item){

					// swing sound if tool is selected
					switch (selected_item->tool_id){
						case TOOL_pickaxe:{play_one_audio_clip(audioFiles[AUDIO_swing_fast].path);}break;
						case TOOL_axe:{play_one_audio_clip(audioFiles[AUDIO_swing_slow].path);}break;
						case TOOL_shovel:{play_one_audio_clip(audioFiles[AUDIO_swing_slow].path);}break;
						default:{printf("Failed to play specific audio. toolID = %d\n", selected_item->tool_id);}break;		// hopefully this won't cause a crash. Because of trying to print tool_id if selected_item has no tool_id
					}
				}

				// if mouse is close to an entity (selected)  AND  selected entity is destroyable  AND  player has selected an item from hotbar
				if (selected_en && selected_en->destroyable && selected_item) {

					// printf("SELECTED EN ITEM ID = '%d'\n", selected_en->item_id);
						
					// get entity pos (for playing audio at position)
					Vector3 audio_pos = v3(get_mouse_pos_in_ndc(selected_en->pos.x, selected_en->pos.y).x, get_mouse_pos_in_ndc(selected_en->pos.x, selected_en->pos.y).y, 0);


					switch (selected_en->arch) {
						case ARCH_tree: {	// |------- TREE -------|
							{
								if (selected_item->arch == ARCH_tool && selected_item->tool_id == TOOL_axe){

									// play_one_audio_clip_at_position(audioFiles[AUDIO_hit_wood1].path, audio_pos);

									selected_en->health -= 1;
									if (selected_en->health <= 0) {
										Entity* en = entity_create();
										// setup_item_pine_wood(en);
										setup_item(en, ITEM_pine_wood);
										en->pos = selected_en->pos;
										allow_destroy = true;
										// play_one_audio_clip_at_position(audioFiles[AUDIO_wood_breaking1].path, audio_pos);
									}
								}
								else{printf("WRONG TOOL\n");}
							}
						} break;

						case ARCH_rock: {	// |------- ROCK -------|
							{
								if (selected_item->arch == ARCH_tool && selected_item->tool_id == TOOL_pickaxe){

									play_one_audio_clip_at_position(audioFiles[AUDIO_hit_metal1].path, audio_pos);

									selected_en->health -= 1;
									if (selected_en->health <= 0) {
										generateLoot(lootTable_rock, 0, selected_en->pos);
										allow_destroy = true;
										play_one_audio_clip_at_position(audioFiles[AUDIO_rock_breaking1].path, audio_pos);
									}
								}
								else{printf("WRONG TOOL\n");}
							}
						} break;

						case ARCH_building: {	// |------- BUILDING -------|
							{
								Entity* en = entity_create();
								setup_item_building(en, selected_en->building_id);
								// printf("SELECTED EN BUILDING ID = %d\n", en->building_id);
								en->pos = selected_en->pos;
								allow_destroy = true;

							}
						} break;

						case ARCH_ore: {	// |------- ORES -------|
							{
								if (selected_item->arch == ARCH_tool && selected_item->tool_id == TOOL_pickaxe){
									play_one_audio_clip_at_position(audioFiles[AUDIO_hit_metal1].path, audio_pos);
									selected_en->health -= 1;
									if (selected_en->health <= 0){
										Entity* en = entity_create();
										setup_item(en, selected_en->item_id);
										en->pos = selected_en->pos;
										allow_destroy = true;
										play_one_audio_clip_at_position(audioFiles[AUDIO_rock_breaking1].path, audio_pos);
									}
								}
								else{printf("WRONG TOOL\n");}
							}
						} break;

						default: { 	// |------- OTHERS -------|
							{
								Entity* en = entity_create();
								setup_item(en, selected_en->item_id);
								en->pos = selected_en->pos;
								allow_destroy = true;
								
							}
						} break;
					}
				}	// selected_item != NULL
				else{
					if (selected_item){
						// |------- SHOVEL -------|
						// #portal
						if (selected_item->arch == ARCH_tool && selected_item->tool_id == TOOL_shovel){
							if (world->dimension->biome_id == BIOME_forest){ create_portal_to(BIOME_cave, true); }
						}
					}
				}


				if (allow_destroy){
					entity_destroy(selected_en);
					// entity_bin[entity_bin_size] = selected_en;
					// entity_bin_size++;
				}
			}
		}


		// :Player use
		{
			if (is_key_just_pressed('F')){
				consume_key_just_pressed('F');

				printf("PLAYER USE\n");

				Entity* selected_en = world_frame.selected_entity;

				if (selected_en){
					switch (selected_en->arch){

						case ARCH_portal:{ // #portal
							{
								if (selected_en->portal_data.enabled){
									BiomeID destination = selected_en->portal_data.destination;
									change_biomes(world, destination);
								}
							}
						} break;

						case ARCH_building:{
							{
								printf("SELECTED BUILDING\n");
							}
						} break;

						default:{}break;
					}
				}
			}
		}


		// Render entities
		render_entities(world);


		// render keybinding
		if (world_frame.selected_entity && world_frame.selected_entity->arch == ARCH_portal){
			render_keybinding(world_frame.selected_entity, player_use_key);
		}



		// DEBUG STUFF ------------------------------------------------------------------------------->

		// ENTER DEBUG MODE FROM GAME
		if (is_key_just_pressed(KEY_CTRL)){
			int I = 1;
		}


		// #Biome
		// printf("%s\n",get_biome_data_from_id(world->dimension.biome_id).name);

		// #dimension
		// printf("Current Dimension = %s\n", world->dimension->name);


		// DEBUG: print UX state
		// printf("UX STATE: ");
		// switch (world->ux_state){
		// 	case UX_building:{printf("BUILDING\n");}break;
		// 	case UX_inventory:{printf("INVENTORY\n");}break;
		// 	case UX_chest:{printf("CHEST\n");}break;
		// 	case UX_place_mode:{printf("PLACE MODE\n");}break;
		// 	case UX_nil:{printf("NIL\n");}break;
		// 	default:{}break;
		// }

		// if (IS_DEBUG)
		// {
		// 	Vector2 mouse_pos = get_mouse_pos_in_world_space();
		// 	printf("%.1f, %.1f\n", mouse_pos.x, mouse_pos.y);
		// }

		// debug print entity counts
		// if (1 == 0)
		// {
		// 	int total = 0;
		// 	// for (int i = 0; i < MAX_ENTITY_COUNT; i++){
		// 	for (int i = 0; i < world->entity_count; i++){
		// 		Entity* en = &world->entities[i];
		// 		if (en->arch != ARCH_nil){
		// 			total++;
		// 		}
		// 	}
		// 	printf("TOTAL ENTITY COUNT = %d\tworld->entity_count = %d\n", total, world->entity_count);
		// }

		// DEBUG: print current biome && #Biome
		// if (1 == 0)
		// {
		// 	if (world->biome_id == 1){
		// 		printf("Current Biome = FOREST\n");
		// 	}
		// 	else if (world->biome_id == 2){
		// 		printf("Current Biome = CAVE\n");
		// 	}
		// 	else{
		// 		printf("Current BiomeID = '%d'\n", world->biome_id);
		// 	}
		// }


		if (is_key_just_pressed('H')) {generateLoot(lootTable_rock, 100, v2(0,0));}
		if (is_key_just_pressed('G')) {create_portal_to(BIOME_cave, true);}

		// #Biome
		// if (is_key_just_pressed('N')) {change_biomes(world, BIOME_forest);}
		// if (is_key_just_pressed('M')) {change_biomes(world, BIOME_cave);}
		
		// #dimension
		if (is_key_just_pressed('N')) {change_dimensions(DIM_overworld);}
		if (is_key_just_pressed('M')) {change_dimensions(DIM_cavern);}

		if (is_key_just_pressed('B')) {render_list.needs_sorting = true;}




		// :Input ------------------------------------------------------------------------------------>
		if (is_key_just_pressed('X')){ // EXIT
			window.should_close = true;
		}
		if (is_key_just_pressed('V')){
			if (IS_DEBUG){
				IS_DEBUG = false;
			}
			else{
				IS_DEBUG = true;
			}
		}


		// Mousewheel ZOOM (debug for now)
		// Selecting slots with mouse wheel
		for (u64 i = 0; i < input_frame.number_of_events; i++) {
			Input_Event e = input_frame.events[i];
			switch (e.kind) {
				case (INPUT_EVENT_SCROLL):
				{
					if (e.yscroll > 0){
						if (IS_DEBUG){view_zoom -= 0.01;}
						else{
							selected_slot -= 1;
							if (selected_slot < 0){
								selected_slot = 8;
							}
						}
					}
					else{
						if (IS_DEBUG){view_zoom += 0.01;}
						else{
							selected_slot += 1;
							if (selected_slot > 8){
								selected_slot = 0;
							}
						}
					}
					break;
				}
				case (INPUT_EVENT_KEY):{break;}
				case (INPUT_EVENT_TEXT):{break;}
			}
		}


		// selecting slots with keys
		if (is_key_just_pressed('1')) {selected_slot = 1 - 1;}
		if (is_key_just_pressed('2')) {selected_slot = 2 - 1;}
		if (is_key_just_pressed('3')) {selected_slot = 3 - 1;}
		if (is_key_just_pressed('4')) {selected_slot = 4 - 1;}
		if (is_key_just_pressed('5')) {selected_slot = 5 - 1;}
		if (is_key_just_pressed('6')) {selected_slot = 6 - 1;}
		if (is_key_just_pressed('7')) {selected_slot = 7 - 1;}
		if (is_key_just_pressed('8')) {selected_slot = 8 - 1;}
		if (is_key_just_pressed('9')) {selected_slot = 9 - 1;}

		// Sprint
		if (is_key_down(KEY_SHIFT)){ player_running = true;}
		else { player_running = false;}

		// Player movement
		Vector2 input_axis = v2(0, 0);
		if (is_key_down('W')){input_axis.y += 1.0;}
		if (is_key_down('A')){input_axis.x -= 1.0;}
		if (is_key_down('S')){input_axis.y -= 1.0;}
		if (is_key_down('D')){input_axis.x += 1.0;}

		// normalize
		input_axis = v2_normalize(input_axis);

		// player_pos = player_pos + (input_axis * 10.0);
		
		if (player_running){ player_en->pos = v2_add(player_en->pos, v2_mulf(input_axis, player_running_speed * delta_t)); }
		else { player_en->pos = v2_add(player_en->pos, v2_mulf(input_axis, player_speed * delta_t)); }

		gfx_update();

		// :Timing
		seconds_counter += delta_t;
		frame_count += 1;
		if (seconds_counter > 1.0){
			if (print_fps){
				log("fps: %i", frame_count);
			}
			seconds_counter = 0.0;
			frame_count = 0;
		}
	}
	return 0;
}