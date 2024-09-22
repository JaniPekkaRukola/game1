#ifndef TYPES_H
#define TYPES_H

// GLOBALS
    #define MAX_ENTITY_COUNT 2048
    #define MAX_PORTAL_COUNT 10
    #define MAX_PORTAL_PAIRS MAX_PORTAL_COUNT
    #define MAX_PICKUP_TEXTS 10
    #define MAX_RECIPE_ITEMS 8

    const float screen_width = 240.0;
    const float screen_height = 135.0;

    // int portal_pair_count = 0;
    const int tile_width = 8;
    // const int tile_width = 32;
    int selected_slot = 0;
    Gfx_Font* font;
    u32 font_height = 48;

    // Health
    int player_health = 3;
    const int rock_health = 8;
    const int tree_health = 5;
    const int bush_health = 1;

    // forward declarations:
    typedef struct Entity Entity;
// 


// :Engine ------------------------------------------------------------------------------------------->

    // Yoinked Range.c stuff
    typedef struct Range1f {
        float min;
        float max;
    } Range1f;

    typedef struct Range2f {
        Vector2 min;
        Vector2 max;
    } Range2f;

    inline Range2f range2f_make(Vector2 min, Vector2 max) { return (Range2f) { min, max }; }
// 

// :ENUMS -------------------------------------------------------------------------------------------->

    typedef enum EntityArchetype {
        // TODO: un-number these
        ARCH_nil = 0,
        ARCH_rock = 1,
        ARCH_tree = 2,
        ARCH_bush = 3,
        ARCH_twig = 4,
        ARCH_player = 5,

        ARCH_item = 6,
        ARCH_tool = 7,
        ARCH_building = 8,

        ARCH_ore = 9,

        ARCH_mushroom = 10,

        ARCH_portal = 11,

        ARCH_torch = 12,
        
        ARCH_parallax = 13,

        ARCH_MAX,
    } EntityArchetype;


    typedef enum BiomeID {
        BIOME_nil,

        // Overworld biomes
        BIOME_forest,
        BIOME_pine_forest,
        BIOME_magical_forest,
        BIOME_desert,
        BIOME_polar,

        // Cavern biomes
        BIOME_cave,
        // BIOME_overgrown_cave,
        // BIOME_cave?
        // BIOME_crystal_cave,
        // BIOME_graveyard,

        // floating island biomes
        // BIOME_floating_island,
        // BIOME_floating_forest, ??
        // BIOME_windy_planes, ??
        // BIOME_rainy, ??
        // BIOME_bliss, ??

        BIOME_MAX,
    } BiomeID;


    typedef enum DimensionID {
        DIM_nil,

        DIM_overworld,
        DIM_cavern,
        DIM_floating_islands,

        DIM_MAX
    } DimensionID;


    typedef enum SpriteID {
        SPRITE_nil,

        // Entities
        SPRITE_player,
        SPRITE_tree_pine,
        SPRITE_tree_spruce,
        SPRITE_tree_magical0,
        SPRITE_tree_magical1,
        SPRITE_rock0,
        SPRITE_rock1,
        SPRITE_rock2,
        SPRITE_rock3,
        SPRITE_bush0,
        SPRITE_bush1,
        SPRITE_tall_grass0,
        SPRITE_tall_grass1,
        SPRITE_mushroom0,

        // Items
        SPRITE_item_rock,
        SPRITE_item_pine_wood,
        SPRITE_item_sprout,
        SPRITE_item_berry,
        SPRITE_item_twig,
        SPRITE_tree_sap,

        // Ores
        SPRITE_ORE_iron,
        SPRITE_ORE_gold,
        SPRITE_ORE_copper,

        // ores as items
        SPRITE_ITEM_ore_iron,
        SPRITE_ITEM_ore_gold,
        SPRITE_ITEM_ore_copper,

        // Ingots
        SPRITE_INGOT_iron,
        SPRITE_INGOT_gold,
        SPRITE_INGOT_copper,

        // Tools
        SPRITE_TOOL_pickaxe,
        SPRITE_TOOL_axe,
        SPRITE_TOOL_shovel,
        SPRITE_TOOL_torch,

        // Fossils
        SPRITE_item_fossil0,
        SPRITE_item_fossil1,
        SPRITE_item_fossil2,

        // Buildings
        SPRITE_building_furnace,
        SPRITE_building_workbench,
        SPRITE_building_chest,

        // Categories
        SPRITE_CATEGORY_all,
        SPRITE_CATEGORY_items,
        SPRITE_CATEGORY_tools,
        SPRITE_CATEGORY_buildings,

        // other
        SPRITE_portal0,
        SPRITE_portal1,

        SPRITE_MAX,
    } SpriteID;

    typedef enum ParallaxID {
        PARALLAX_nil,

        PARALLAX_tree0,
        PARALLAX_tree1,

        PARALLAX_MAX,
    } ParallaxID;

    typedef enum ToolID {
        TOOL_nil,

        TOOL_pickaxe,
        TOOL_axe,
        TOOL_shovel,
        TOOL_torch,

        TOOL_MAX,
    }ToolID;


    typedef enum ItemID {
        ITEM_nil,
        ITEM_rock,
        ITEM_pine_wood,
        ITEM_sprout,
        ITEM_berry,
        ITEM_mushroom0,
        ITEM_twig,
        ITEM_tree_sap,
        ITEM_furnace,
        ITEM_workbench,
        ITEM_chest,
        ITEM_fossil0,
        ITEM_fossil1,
        ITEM_fossil2,
        ITEM_fossil3,

        // ores
        ITEM_ORE_iron,
        ITEM_ORE_gold,
        ITEM_ORE_copper,

        // ingots
        ITEM_ingot_iron,
        ITEM_ingot_gold,
        ITEM_ingot_copper,

        // tools
        ITEM_TOOL_pickaxe,
        ITEM_TOOL_axe,
        ITEM_TOOL_shovel,
        ITEM_TOOL_torch,

        // buildings (test)
        ITEM_BUILDING_furnace,
        ITEM_BUILDING_workbench,
        ITEM_BUILDING_chest,
        
        ITEM_MAX,
    } ItemID;


    typedef enum OreID {
        ORE_nil,

        ORE_iron,
        ORE_gold,
        ORE_copper,

        ORE_MAX,
    } OreID;


    typedef enum BuildingID {
        BUILDING_nil,

        BUILDING_furnace,	
        BUILDING_workbench,
        BUILDING_chest,
        
        BUILDING_MAX,
    } BuildingID;


    typedef enum TextureID {
        TEXTURE_nil,

        TEXTURE_grass,
        TEXTURE_cave_floor,
        TEXTURE_vignette_torch,
        TEXTURE_vignette_no_torch,
        TEXTURE_torch_light,

        TEXTURE_TILE_forest,
        TEXTURE_TILE_pine_forest,
        TEXTURE_TILE_cave,

        TEXTURE_MAX,
    } TextureID;


    typedef enum UXState {
        UX_nil,
        // UX_inventory,
        UX_building,
        UX_place_mode,

        // buildings
        UX_chest,
        UX_furnace,
        UX_workbench,

        UX_mainmenu,


        // UX_map,
        // UX_settings,
    } UXState;
// 


// :STRUCTS ------------------------------------------------------------------------------------------>

    // :Portal -------------------------->
    typedef struct PortalData {
        DimensionID dim_destination;
        bool enabled;
        int id; // unique id for linking portals
        Vector2 pos;
    } PortalData;

    typedef struct PortalPair {
        int id; // unique id for portal pair
        DimensionID dim1;
        DimensionID dim2;
        Vector2 pos1;
        Vector2 pos2;
    } PortalPair;

    // :InventoryItemData --------------->
    typedef struct InventoryItemData {
        int amount;
        string name;
        bool valid;
        SpriteID sprite_id;
        EntityArchetype arch;
        ToolID tool_id;
        ItemID item_id;
        EntityArchetype category;
    } InventoryItemData;

    // ItemAmount
    typedef struct ItemAmount{
        // string name;
        ItemID id;
        int amount;
    } ItemAmount;

    // :ItemData ------------------------>
    typedef struct ItemData {
        string name;
        EntityArchetype arch;
        EntityArchetype category;
        ItemAmount crafting_recipe[MAX_RECIPE_ITEMS];
        int crafting_recipe_count; // how many types of items in recipe ????? or output????
        ToolID tool_id;
        SpriteID sprite_id;
        ItemID item_id;
        float cooking_time;
    } ItemData;

    // :BuildingData -------------------->
    typedef struct BuildingData { 
        EntityArchetype to_build;
        SpriteID sprite_id;
        BuildingID building_id;
        bool has_inventory;
        InventoryItemData inventory[ITEM_MAX];
        ItemData *selected_crafting_item;
        int crafting_queue;
        float64 crafting_end_time;
        Entity* en;
        // display name
        // cost
        // health
        // etc
    } BuildingData;

    // :Entity -------------------------->
    typedef struct Entity {
        // main
        EntityArchetype arch;
        string name;
        Vector2 pos;
        int health;

        // id
        SpriteID sprite_id;
        ParallaxID parallax_id;
        ItemID item_id;
        BuildingID building_id;
        ToolID tool_id;
        OreID ore_id;
        BiomeID biome_ids[BIOME_MAX];	// all biomes where the entity can spawn

        // data
        PortalData portal_data;
        BuildingData building_data;
        ItemData *current_crafting_item;

        // booleans
        bool destroyable;
        bool is_valid;
        bool is_item;
        bool is_ore;
        bool is_crafting_station;
        bool is_selectable_by_mouse;
        bool enable_shadow;
        bool has_animation;
        // bool render_sprite;

        // other
        int biome_count;				// how many biomes the entity is in
        int rendering_prio;
        Vector4 pickup_text_col;

    } Entity;

    // :WorldFrame ---------------------->
    typedef struct WorldFrame {
        Entity* selected_entity;
        Matrix4 world_projection;
        Matrix4 world_view;
        bool hover_consumed;
        // Entity* player;
    } WorldFrame;

    typedef struct BiomeData {
        string name;
        BiomeID id;
        Vector2 size;
        Range2f position;
        bool enabled;

        bool spawn_animals;
        bool spawn_water;
        float water_weight;

        Vector4 grass_color;
        Vector4 leaves_color;
        TextureID ground_texture;

        // parallax
        bool enable_parallax;
        float parallax_weight;

        // trees
        bool spawn_pine_trees;
        float pine_tree_weight;
        bool spawn_spruce_trees;
        float spruce_tree_weight;
        bool spawn_magical_trees;
        float magical_tree_weight;
        bool spawn_birch_trees;
        float birch_tree_weight;
        bool spawn_palm_trees;
        float palm_tree_weight;

        bool spawn_rocks;
        float rocks_weight;
        bool spawn_mushrooms;
        float mushrooms_weight;
        bool spawn_twigs;
        float twigs_weight;
        bool spawn_berries;
        float berries_weight;

        // ores
        bool spawn_ores;
        bool spawn_ore_iron;
        float ore_iron_weight;
        bool spawn_ore_gold;
        float ore_gold_weight;
        bool spawn_ore_copper;
        float ore_copper_weight;

        // fossils
        bool spawn_fossils;
        float fossil0_drop_chance;
        float fossil1_drop_chance;
        float fossil2_drop_chance;
        int fossil_rarity_level;

        // portal
        bool has_portals;
        // PortalData portals[MAX_PORTAL_COUNT];
    } BiomeData;

    typedef struct Chunk Chunk;

    typedef struct WorldData { // map
        int width;
        int height;
        BiomeID* tiles;
        // WorldGenerationConfig config; // current worldgeneration config
    } WorldData;
    // WorldData map = {0};

    // :Dimension ----------------------->
    typedef struct DimensionData {
        string name;
        DimensionID id;

        Chunk*** chunks; // 2d array of chunk pointers
        // Chunk** chunks; // 2d array of chunk pointers
        int chunk_count;
        int chunk_size;
        WorldData map; // dimension biome map

        int entity_count;
        Entity entities[MAX_ENTITY_COUNT];
        
        // BiomeID biomes[BIOME_MAX];
        // BiomeData* biomes[BIOME_MAX];
        // TODO: get rid of this
        BiomeID current_biome_id;

        SpriteID portal_sprite_in;
        SpriteID portal_sprite_out;
        // int portal_count;

    } DimensionData;

    // :Player -------------------------->
    typedef struct Player {
        Entity* en; // player position is inside en
        // EntityArchetype arch;
        // Vector2 position; // this is kinda useless. USE "en->pos" for position
        float walking_speed;
        float running_speed_amount;
        bool is_running;
        DimensionID dimension_id;

        float health;
        float item_pickup_radius;
        float entity_selection_radius;
        // ItemData *selected_item;
        Entity *selected_entity;
        BuildingData* selected_building; // selected building || currently open building

        // UXState ux_state;

        bool inventory_ui_open;
        InventoryItemData inventory[ITEM_MAX]; // this is players inventory. "ITEM_MAX" is basically max inventory size.
        int inventory_items_count; // might be useless, used in checking for items in player inventory. Could be replaced by a for loop at the check items func! 

    } Player;

    typedef Vector2i Tile;

    typedef struct TileData {
        Tile tile;
        // Entity* entity_at_tile;
    } TileData;



    // :World --------------------------->
    typedef struct World {
        // BiomeID current_biome_id;
        // struct BiomeData* biome_data;

        // DimensionID dimension_id; // current dimension id
        DimensionData *dimension; // current dimension data
        DimensionData* dimensions[DIM_MAX];
        DimensionID current_dim;

        UXState ux_state; // move this into player struct !?
        
        // ui stuff
        float inventory_alpha;
        float inventory_alpha_target;
        float building_menu_alpha;
        float building_menu_alpha_target;
        float chest_alpha;
        float chest_alpha_target;
        float furnace_alpha;
        float furnace_alpha_target;
        float workbench_alpha;
        float workbench_alpha_target;

        BuildingID placing_building; // bad name
        Entity* open_crafting_station; // currently opened crafting station

        Entity portals[MAX_PORTAL_COUNT];
        int portal_count;
        PortalPair portal_pairs[MAX_PORTAL_PAIRS];
        int portal_pair_count;

        Player *player;
    } World;


    // :Sprite -------------------------->
    typedef struct Sprite {
        Gfx_Image* image;
    } Sprite;

    // :Parallax
    typedef struct Parallax {
        Gfx_Image* image;
        float threshold_min;
        float threshold_max;
    } Parallax;

    // :ToolData ------------------------>
    typedef struct ToolData {
        string name;
        string tooltip;
        int durability;
        int miningLevel;
    } ToolData;

    // :Texture ------------------------->
    typedef struct Texture {
        Gfx_Image* image;
    } Texture;

    // :LootItem ------------------------>
    typedef struct LootItem {
        string *name;
        // EntityArchetype arch;
        ItemID id;
        float baseDropChance;
        struct LootItem *next;
    } LootItem;

    // :LootTable ----------------------->
    typedef struct LootTable {
        LootItem *head;
        int itemCount;
        string table_name;
    } LootTable;



    // not in use????
        // :RenderList ---------------------->
        // typedef struct RenderList {
        //     int* indices;
        //     int count;
        //     bool needs_sorting;
        // } RenderList;
    // 
// 

// :ANIMATIONS --------------------------------------------------------------------------------------->

    // :Pickup text animation ------------>
    typedef struct pickup_text_animation {
        Vector2 start_pos;
        Vector2 end_pos;
        Vector2 pos_increase;
        float start_alpha;
        float end_alpha;
        float duration;
        float elapsed_time;
        bool active;
        Entity en;
    } pickup_text_animation;

    // :Dimension change animation ------->
    typedef struct dim_change_animation {
        Vector2 start_pos;
        Vector2 end_pos;
        Vector2 pos_increase;
        float duration;
        float elapsed_time;
        bool active;
    } dim_change_animation;


// 

// :GLOBALS ------------------------------------------------------------------------------------------>

    Vector2 camera_pos = {0, 0};
	float view_zoom = 0.1875;   // view zoom ratio x (pixelart layer width / window width = 240 / 1280 = 0.1875)

    // ItemID held_item; // old
    // OreID ores[ORE_MAX];

    // InventoryItemData* dragging_now;
    WorldFrame world_frame;
    DimensionData dimensions[DIM_MAX];
    World* world = 0;
    BiomeData biomes[BIOME_MAX];
    Sprite sprites[SPRITE_MAX];
    Parallax parallaxes[PARALLAX_MAX];
    BuildingData buildings[BUILDING_MAX];
    Texture textures[TEXTURE_MAX];
    pickup_text_animation pickup_texts[MAX_PICKUP_TEXTS];
    dim_change_animation animation_dim_change = {};


    // ui rendering
    InventoryItemData inventory_selected_item;	// hardcopy of item from inventory, when starting to drag item
    InventoryItemData inventory_selected_item_in_chest_ui;	// hardcopy of item from inventory, when starting to drag item
    InventoryItemData chest_selected_item;	// hardcopy of item from chest, when starting to drag item
    InventoryItemData* item_in_hand = NULL;	// selected item in hotbar. Renders in hand
    BuildingData* selected_building_buildmode = NULL; // selected building in build mode
    int selected_slot_index = 0;
    Draw_Quad* chest_quad = NULL;
    EntityArchetype workbench_tab_category = ARCH_nil; // workbench recipes sorting. ARCH_nil = all recipes
    ItemData* selected_recipe_furnace = NULL; // selected recipe in furnace ui
    ItemData* selected_recipe_workbench = NULL; // selected recipe in workbench ui
    Matrix4 selected_recipe_xfrom;
    bool is_recipe_selected = false;

    // recipes
    ItemData crafting_recipes[ITEM_MAX];
    ItemData furnace_recipes[ITEM_MAX];

    // loot tables
    LootTable *lootTable_rock;
    LootTable *lootTable_pine_tree;

    // #CHUNK
    #define CHUNK_SIZE 256
    #define MAX_CHUNKS 100


// 




#endif