#ifndef RECIPES_H
#define RECIPES_H

#include "types.h"

// :SMELTING RECIPES ---------------------------------------------------------------------------->
    void setup_smelting_recipes(){
        // can this be automated ????

        // Iron ingot
        furnace_recipes[ITEM_ingot_iron] = (ItemData){
            .name = STR("Iron ingot"), 
			.category = ARCH_item,
            .sprite_id = SPRITE_INGOT_iron, 
            .item_id = ITEM_ingot_iron, 
            .crafting_recipe = {{ITEM_ORE_iron, 2}},
            .crafting_recipe_count = 1, 
            .cooking_time = 2.0f
        };
        
        // Gold ingot
        furnace_recipes[ITEM_ingot_gold] = (ItemData){
            .name = STR("Gold ingot"), 
			.category = ARCH_item,
            .sprite_id = SPRITE_INGOT_gold, 
            .item_id = ITEM_ingot_copper, 
            .crafting_recipe = {{ITEM_ORE_gold, 3}},
            .crafting_recipe_count = 1, 
            .cooking_time = 5.0f
        };

        // Copper ingot
        furnace_recipes[ITEM_ingot_copper] = (ItemData){
            .name = STR("Copper ingot"), 
			.category = ARCH_item,
            .sprite_id = SPRITE_INGOT_copper, 
            .item_id = ITEM_ingot_copper, 
            .crafting_recipe = {{ITEM_ORE_copper, 4}},
            .crafting_recipe_count = 1, 
            .cooking_time = 3.0f
        };

    }

// 


// :CRAFTING RECIPES ---------------------------------------------------------------------------->
	void setup_crafting_recipes(){
		// Pickaxe
		crafting_recipes[ITEM_TOOL_pickaxe] = (ItemData){
			.name = STR("PickAxe"),
			.arch = ARCH_tool,
			.category = ARCH_tool,
			.tool_id = TOOL_pickaxe,
			.sprite_id = SPRITE_TOOL_pickaxe,
			.item_id = ITEM_TOOL_pickaxe,
			.crafting_recipe = {{ITEM_rock, 4},{ITEM_twig, 2}},
			.crafting_recipe_count = 2,
			.cooking_time = 2.0f
		};

		// Axe
		crafting_recipes[ITEM_TOOL_axe] = (ItemData){
			.name = STR("Axe"),
			.arch = ARCH_tool,
			.category = ARCH_tool,
			.tool_id = TOOL_axe,
			.sprite_id = SPRITE_TOOL_axe,
			.item_id = ITEM_TOOL_axe,
			.crafting_recipe = {{ITEM_rock, 3},{ITEM_twig, 2}},
			.crafting_recipe_count = 2,
			.cooking_time = 2.0f
		};
		
		// Shovel
		crafting_recipes[ITEM_TOOL_shovel] = (ItemData){
			.name = STR("Shovel"),
			.arch = ARCH_tool,
			.category = ARCH_tool,
			.tool_id = TOOL_shovel,
			.sprite_id = SPRITE_TOOL_shovel,
			.item_id = ITEM_TOOL_shovel,
			.crafting_recipe = {{ITEM_rock, 2},{ITEM_twig, 2}},
			.crafting_recipe_count = 2,
			.cooking_time = 2.0f
		};

        // Torch
        crafting_recipes[ITEM_TOOL_torch] = (ItemData){
           .name = STR("Torch"),
           .arch = ARCH_item,
		   .category = ARCH_tool,
           .tool_id = TOOL_torch,
           .sprite_id = SPRITE_TOOL_torch,
           .item_id = ITEM_TOOL_torch,
           .crafting_recipe = {{ITEM_twig, 2}, {ITEM_tree_sap, 2}},
           .crafting_recipe_count = 2,
           .cooking_time = 4.0f
        };

		// Chest
		crafting_recipes[ITEM_chest] = (ItemData){
		   .name = STR("Chest"),
		   .arch = ARCH_item,
		   .category = ARCH_building,
		   .tool_id = TOOL_nil,
		   .sprite_id = SPRITE_building_chest,
		   .item_id = ITEM_chest,
		   .crafting_recipe = {{ITEM_pine_wood, 5},{ITEM_ingot_iron, 2}},
		   .crafting_recipe_count = 2,
		   .cooking_time = 2.0f
		};

		// Furnace
		crafting_recipes[ITEM_furnace] = (ItemData){
		   .name = STR("Furnace"),
		   .arch = ARCH_item,
		   .category = ARCH_building,
		   .tool_id = TOOL_nil,
		   .sprite_id = SPRITE_building_furnace,
		   .item_id = ITEM_furnace,
		   .crafting_recipe = {{ITEM_rock, 10},{ITEM_twig, 5}},
		   .crafting_recipe_count = 2,
		   .cooking_time = 2.0f
		};

		// Workbench
		crafting_recipes[ITEM_workbench] = (ItemData){
		   .name = STR("Workbench"),
		   .arch = ARCH_item,
		   .category = ARCH_building,
		   .tool_id = TOOL_nil,
		   .sprite_id = SPRITE_building_workbench,
		   .item_id = ITEM_workbench,
		   .crafting_recipe = {{ITEM_pine_wood, 5}},
		   .crafting_recipe_count = 1,
		   .cooking_time = 2.0f
		};
	}
// 


void setup_all_recipes(){
    setup_smelting_recipes();
    setup_crafting_recipes();
}

#endif