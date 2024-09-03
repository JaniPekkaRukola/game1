#ifndef ANIMATIONS2_H
#define ANIMATIONS2_H

#define MAX_ANIMATIONS2 50
int active_animations = 0;
unsigned long id_counter = 0;

typedef enum AnimationID{
    ANIM_nil,

    ANIM_torch,
    ANIM_crafting,
    ANIM_smelting,

    ANIM_MAX,
} AnimationID;

typedef struct animation2 {
    unsigned long unique_id;
    string name;
    Gfx_Image *anim_sheet;
    AnimationID anim_id;
    Vector2 pos;
    float32 start_time;

    bool active;
    bool looping;

    // float offset_x;
    // float offset_y;

    bool has_custom_size;
    Vector2 custom_size;

    u32 number_of_columns;
	u32 number_of_rows;
	u32 total_number_of_frames;
	
	u32 anim_frame_width;
	u32 anim_frame_height;
    u32 anim_start_frame_x;
	u32 anim_start_frame_y;
	u32 anim_end_frame_x;
	u32 anim_end_frame_y;
	u32 anim_start_index;
	u32 anim_end_index;
	u32 anim_number_of_frames;

    float32 playback_fps;
	float32 anim_time_per_frame;
	float32 cycle_duration;
    float duration;
    float elapsed_time;
    float32 anim_start_time;

} Animation2;

Animation2 animations[MAX_ANIMATIONS2];

unsigned long get_new_id(){
    id_counter++;
    return id_counter;
}

Animation2* setup_torch_animation(){
    Animation2* anim;
    anim = alloc(get_heap_allocator(), sizeof(Animation2));

    anim->anim_sheet = load_image_from_disk(STR("res/animations/animation_torch.png"), get_heap_allocator());
    anim->name = STR("Torch animation");
    anim->anim_id = ANIM_torch;

    anim->active = false;
    anim->looping = true;

	assert(anim->anim_sheet, "Could not open animation sheet");
	
	// Configure information about the whole image as a sprite sheet
	anim->number_of_columns = 4;
	anim->number_of_rows = 1;
	anim->total_number_of_frames = anim->number_of_rows * anim->number_of_columns;
	
	anim->anim_frame_width  = anim->anim_sheet->width  / anim->number_of_columns;
	anim->anim_frame_height = anim->anim_sheet->height / anim->number_of_rows;
	
	// Configure the animation by setting the start & end frames in the grid of frames
	// (Inspect sheet image and count the frame indices you want)
	// In sprite sheet animations, it usually goes down. So Y 0 is actuall the top of the
	// sprite sheet, and +Y is down on the sprite sheet.
	anim->anim_start_frame_x = 0;
	anim->anim_start_frame_y = 0;
	anim->anim_end_frame_x = 3;
	anim->anim_end_frame_y = 0;
	anim->anim_start_index = anim->anim_start_frame_y * anim->number_of_columns + anim->anim_start_frame_x;
	anim->anim_end_index   = anim->anim_end_frame_y   * anim->number_of_columns + anim->anim_end_frame_x;
	anim->anim_number_of_frames = max(anim->anim_end_index, anim->anim_start_index)-min(anim->anim_end_index, anim->anim_start_index)+1;
	
	// Sanity check configuration
	assert(anim->anim_end_index > anim->anim_start_index, "The last frame must come before the first frame");
	assert(anim->anim_start_frame_x < anim->number_of_columns, "anim_start_frame_x is out of bounds");
	assert(anim->anim_start_frame_y < anim->number_of_rows, "anim_start_frame_y is out of bounds");
	assert(anim->anim_end_frame_x < anim->number_of_columns, "anim_end_frame_x is out of bounds");
	assert(anim->anim_end_frame_y < anim->number_of_rows, "anim_end_frame_y is out of bounds");
	
	// Calculate duration per frame in seconds
	anim->playback_fps = 10;
	anim->anim_time_per_frame = 1.0 / anim->playback_fps;
	anim->cycle_duration = anim->anim_time_per_frame * (float32)anim->anim_number_of_frames;

    anim->unique_id = get_new_id();

    // anim->offset_x = anim->anim_frame_width * -0.5;
    // anim->offset_y = anim->anim_frame_height * -0.5;

    return anim;
}

Animation2* setup_crafting_animation(){
    Animation2* anim;
    anim = alloc(get_heap_allocator(), sizeof(Animation2));

    anim->anim_id = ANIM_crafting;
    anim->name = STR("Crafting animation");

    anim->anim_sheet = load_image_from_disk(STR("res/animations/crafting_animation3.png"), get_heap_allocator());
    anim->number_of_columns = 10;
    anim->number_of_rows = 1;
    anim->total_number_of_frames = anim->number_of_rows * anim->number_of_columns;
    
    anim->anim_frame_width  = anim->anim_sheet->width  / anim->number_of_columns;
    anim->anim_frame_height = anim->anim_sheet->height / anim->number_of_rows;
    
    anim->anim_start_frame_x = 0;
    anim->anim_start_frame_y = 0;
    anim->anim_end_frame_x = 9;
    anim->anim_end_frame_y = 0;
    anim->anim_start_index = anim->anim_start_frame_y * anim->number_of_columns + anim->anim_start_frame_x;
    anim->anim_end_index   = anim->anim_end_frame_y   * anim->number_of_columns + anim->anim_end_frame_x;
    anim->anim_number_of_frames = max(anim->anim_end_index, anim->anim_start_index)-min(anim->anim_end_index, anim->anim_start_index)+1;

    assert(anim->anim_end_index > anim->anim_start_index, "The last frame must come before the first frame");
    assert(anim->anim_start_frame_x < anim->number_of_columns, "anim_start_frame_x is out of bounds");
    assert(anim->anim_start_frame_y < anim->number_of_rows, "anim_start_frame_y is out of bounds");
    assert(anim->anim_end_frame_x < anim->number_of_columns, "anim_end_frame_x is out of bounds");
    assert(anim->anim_end_frame_y < anim->number_of_rows, "anim_end_frame_y is out of bounds");

    // Calculate duration per frame in seconds
    anim->playback_fps = 15;
    // anim->playback_fps = (anim->number_of_columns * anim->number_of_rows) / 4.0f;
    anim->anim_time_per_frame = 1.0 / anim->playback_fps;
    anim->cycle_duration = anim->anim_time_per_frame * (float32)anim->anim_number_of_frames;

    // anim->anim_start_time = os_get_elapsed_seconds();

    anim->active = false;
    anim->pos = v2(0, 0);
    anim->unique_id = get_new_id();

    anim->has_custom_size = true;
    anim->custom_size = v2(12, 12);
    // anim->offset_x = anim->anim_frame_width * -0.5;
    // anim->offset_y = anim->anim_frame_height * -0.5;
    // anim->loop = true;

    return anim;
}

void trigger_animation(Animation2 trigger_anim, Vector2 pos, float duration){
    printf("triggering animation...\n");
    for (int i = 0; i < MAX_ANIMATIONS2; i++){
        Animation2* anim_from_list = &animations[i];

        // if animation is not already in the animations list
        if (anim_from_list->unique_id != trigger_anim.unique_id){

            if (anim_from_list->active == false){
                *anim_from_list = trigger_anim;

                anim_from_list->active = true;
                anim_from_list->pos = pos;
                anim_from_list->start_time = os_get_elapsed_seconds();
                anim_from_list->unique_id = trigger_anim.unique_id;
                anim_from_list->name = trigger_anim.name;
                anim_from_list->anim_id = trigger_anim.anim_id;
        
                active_animations++;

                if (duration == 0){
                    anim_from_list->duration = trigger_anim.cycle_duration;
                }
                else{
                    anim_from_list->duration = duration;
                }

                printf("Triggered animation '%s'\n", anim_from_list->name);
                // break;
                return;
            }
        }
        else{
            printf("ANIMATION ALREADY RUNNING\n");
            printf("Current running animations = %d\n", active_animations);
            return;
        }
    }
}

void draw_animation(Animation2* anim, float64 now, Vector2 pos){
    // printf("Drawing animation '%s'\tat %.0f, %.0f\n", anim->name, anim->pos.x, anim->pos.y);
    // printf("w = %d, h=%d\n", anim->anim_frame_width, anim->anim_frame_height);
		
    // Float modulus to "loop" around the timer over the anim duration
    float32 anim_elapsed = fmodf(now - anim->anim_start_time, anim->cycle_duration);
    
    // Get current progression in animation from 0.0 to 1.0
    float32 anim_progression_factor = anim_elapsed / anim->cycle_duration;
    
    u32 anim_current_index = anim->anim_number_of_frames * anim_progression_factor;
    u32 anim_absolute_index_in_sheet = anim->anim_start_index + anim_current_index;
    
    u32 anim_index_x = anim_absolute_index_in_sheet % anim->number_of_columns;
    u32 anim_index_y = anim_absolute_index_in_sheet / anim->number_of_columns + 1;
    
    u32 anim_sheet_pos_x = anim_index_x * anim->anim_frame_width;
    u32 anim_sheet_pos_y = (anim->number_of_rows - anim_index_y) * anim->anim_frame_height; // Remember, Y inverted.
    
    // Draw the sprite sheet, with the uv box for the current frame.
    // Uv box is a Vector4 of x1, y1, x2, y2 where each value is a percentage value 0.0 to 1.0
    // from left to right / bottom to top in the texture.
    
    // pos.x += anim->offset_x;
    // pos.y += anim->offset_y;
    pos.x += anim->custom_size.x * -0.5;
    // pos.y += anim->custom_size.y * -0.5;

    Draw_Quad *quad;

    if (anim->has_custom_size){
        quad = draw_image(anim->anim_sheet, v2(pos.x, pos.y), v2(anim->custom_size.x, anim->custom_size.y), COLOR_WHITE);
    }
    else{
        quad = draw_image(anim->anim_sheet, v2(pos.x, pos.y), v2(anim->anim_frame_width, anim->anim_frame_height), COLOR_WHITE);
    }


    quad->uv.x1 = (float32)(anim_sheet_pos_x)/(float32)anim->anim_sheet->width;
    quad->uv.y1 = (float32)(anim_sheet_pos_y)/(float32)anim->anim_sheet->height;
    quad->uv.x2 = (float32)(anim_sheet_pos_x + anim->anim_frame_width) / (float32)anim->anim_sheet->width;
    quad->uv.y2 = (float32)(anim_sheet_pos_y + anim->anim_frame_height) / (float32)anim->anim_sheet->height;
}

void update_animations(float64 delta_time){
    // for (int i = 0; i < active_animations; i++){
    for (int i = 0; i < MAX_ANIMATIONS2; i++){
        Animation2* anim = &animations[i];

        if (is_key_just_pressed('H')){
            int asd = 0;
        }

        if (anim->anim_id != ANIM_nil){
            if (anim->active == false){
                // delete animation from the animations list!
                memset(anim, 0, sizeof(Animation2));
                continue;
            } 
        }
        else continue;

        anim->elapsed_time += delta_time;

        float t = anim->elapsed_time / anim->duration;
        if (t >= 1.0f){
            // if (anim->looping == false){
                anim->active = false;
                active_animations--;
                printf("Animation '%s' deactivated. Active animations left = %d\n", anim->name, active_animations);
                continue;
            // }
            // else{
            //     t = 0;
            // }
        }


        // held animation pos changes

        draw_animation(anim, os_get_elapsed_seconds(), anim->pos);
    }
}


#endif