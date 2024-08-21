#ifndef TYPES_H
#define TYPES_H

// GLOBAL VARIABLES
    #define MAX_ENTITY_COUNT 2048
    #define MAX_PORTAL_COUNT 32
    const int tile_width = 8;


    // Health
    int player_health = 3;
    const int rock_health = 8;
    const int tree_health = 5;
    const int bush_health = 1;
// 


// :ENUMS -------------------------------------------------------------------------------------------->

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

        ARCH_ore = 9,

        ARCH_mushroom = 10,

        ARCH_portal = 11,
        ARCH_MAX,
    } EntityArchetype;


    typedef enum BiomeID {
        BIOME_nil,

        // Above ground biomes
        BIOME_forest,

        // Underground biomes
        BIOME_cave,
        // BIOME_dark_cave,
        // BIOME_crystal_cave,
        // BIOME_ice_cave,
        // BIOME_desert_cave,

        BIOME_MAX,
    } BiomeID;


    typedef enum DimensionID {
        DIM_nil,

        DIM_overworld,
        DIM_cavern,

        DIM_MAX
    } DimensionID;


    typedef enum SpriteID {
        SPRITE_nil,

        // Entities
        SPRITE_player,
        SPRITE_tree_pine,
        SPRITE_tree_spruce,
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

        // Ores
        SPRITE_ORE_iron,
        SPRITE_ORE_gold,
        SPRITE_ORE_copper,

        // Ingots
        // SPRITE_INGOT_iron,
        // SPRITE_INGOT_gold,
        // SPRITE_INGOT_copper,

        // Tools
        SPRITE_tool_pickaxe,
        SPRITE_tool_axe,
        SPRITE_tool_shovel,

        // Fossils
        SPRITE_item_fossil0,
        SPRITE_item_fossil1,
        SPRITE_item_fossil2,

        // Buildings
        SPRITE_building_furnace,
        SPRITE_building_workbench,
        SPRITE_building_chest,

        // other
        SPRITE_portal,


        SPRITE_MAX,
    } SpriteID;


    typedef enum ToolID {
        TOOL_nil,

        TOOL_pickaxe,
        TOOL_axe,
        TOOL_shovel,

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


        // tools (test)
        ITEM_TOOL_pickaxe,
        ITEM_TOOL_axe,
        ITEM_TOOL_shovel,

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

        TEXTURE_MAX,
    } TextureID;


    typedef enum UXState {
        UX_nil,
        UX_inventory,
        UX_building,
        UX_place_mode,

        // buildings
        UX_chest,
        UX_furnace,



        // UX_map,
        // UX_settings,
        // UX_menu,
    } UXState;
// 


// :STRUCTS ------------------------------------------------------------------------------------------>

    // :Portal -------------------------->
    typedef struct PortalData {
        BiomeID destination;
        bool enabled;
    } PortalData;

    // :Entity -------------------------->
    typedef struct Entity {
        // main
        EntityArchetype arch;
        string name;
        int health;
        Vector2 pos;

        // ids
        SpriteID sprite_id;
        ItemID item_id;
        BuildingID building_id;
        ToolID tool_id;
        OreID ore_id;
        BiomeID biome_ids[BIOME_MAX];	// all biomes where the entity can spawn
        int biome_count;				// how many biomes the entity is in

        // booleans
        bool is_valid;
        // bool render_sprite;
        bool destroyable;
        bool is_item;
        bool is_ore;
        bool enable_shadow;

        PortalData portal_data;

        // other
        int rendering_prio;
    } Entity;

    // :InventoryItemData --------------->
    typedef struct InventoryItemData {
        int amount;
        string name;
        bool valid;
        SpriteID sprite_id;
        EntityArchetype arch;
        ToolID tool_id;
        ItemID item_id;
    } InventoryItemData;

    // :WorldFrame ---------------------->
    typedef struct WorldFrame {
        Entity* selected_entity;
        Matrix4 world_projection;
        Matrix4 world_view;
        bool hover_consumed;
        Entity* player;
    } WorldFrame;

    // :Dimension ----------------------->
    typedef struct DimensionData {
        string name;
        DimensionID dimension_id;
        Entity entities[MAX_ENTITY_COUNT];
        int entity_count;
        BiomeID biomes[BIOME_MAX];
        BiomeID biome_id;

        // color
        // enemies
    } DimensionData;

    // :World --------------------------->
    typedef struct World {
        // BiomeID biome_id;
        // struct BiomeData* biome_data;

        DimensionID dimension_id;
        DimensionData dimension;
        // int layer;
        // Entity entities[MAX_ENTITY_COUNT];
        // int entity_count;
        InventoryItemData inventory_items[ITEM_MAX];
        UXState ux_state;
        
        // ui stuff
        float inventory_alpha;
        float inventory_alpha_target;
        float building_menu_alpha;
        float building_menu_alpha_target;
        float chest_alpha;
        float chest_alpha_target;
        float furnace_alpha;
        float furnace_alpha_target;

        BuildingID placing_building;
    } World;



    // :Biome --------------------------->
    typedef struct BiomeData {
        string name;
        Vector2 size;
        bool enabled;

        bool spawn_animals;
        bool spawn_water;
        float water_weight;

        Vector4 grass_color;
        Vector4 leaves_color;
        TextureID ground_texture;

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

        // ores
        bool spawn_ores;
        bool spawn_ore_iron;
        float spawn_ore_iron_weight;
        bool spawn_ore_gold;
        float spawn_ore_gold_weight;
        bool spawn_ore_copper;
        float spawn_ore_copper_weight;

        // fossils
        bool spawn_fossils;
        float fossil0_drop_chance;
        float fossil1_drop_chance;
        float fossil2_drop_chance;
        int fossil_rarity_level;

        // portal
        bool has_portals;
        // PortalData portals[MAX_PORTAL_COUNT];
        Entity portals[MAX_PORTAL_COUNT];
        int portal_count;

    } BiomeData;

    // :Sprite -------------------------->
    typedef struct Sprite {
        Gfx_Image* image;
    } Sprite;

    // :ToolData ------------------------>
    typedef struct ToolData {
        string name;
        string tooltip;
        int durability;
        int miningLevel;
    } ToolData;

    // :ItemData ------------------------>
    typedef struct ItemData {
        string pretty_name;
        // recipe
        EntityArchetype for_structure;
        ItemID output;
        ItemID input[8];
    } ItemData;

    // :BuildingData -------------------->
    typedef struct BuildingData { 
        EntityArchetype to_build;
        SpriteID icon;
        // BuildingID id;
        // display name
        // cost
        // health
        // etc
    } BuildingData;

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
    } LootTable;


    // not in use????
    // :RenderList ---------------------->
    typedef struct RenderList {
        int* indices;
        int count;
        bool needs_sorting;
    } RenderList;
// 



// :GLOBALS ------------------------------------------------------------------------------------------>

    ItemID held_item;
    OreID ores[ORE_MAX];
    DimensionID dimensions[DIM_MAX];

    InventoryItemData* dragging_now;
    InventoryItemData* selected_item;
    WorldFrame world_frame;
    World* world = 0;
    BiomeData biomes[BIOME_MAX];
    Sprite sprites[SPRITE_MAX];
    Sprite* icon_drag = NULL;
    ItemData item_data[ITEM_MAX];
    BuildingData buildings[BUILDING_MAX];
    Texture textures[TEXTURE_MAX];
    RenderList render_list;
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


#endif