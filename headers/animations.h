#ifndef ANIMATIONS_H
#define ANIMATIONS_H

typedef enum AnimationID {
    ANIM_nil,

    ANIM_torch,
    ANIM_crafting,

    ANIM_MAX,
} AnimationID;

typedef struct Animation{

    AnimationID animation_id;
    string name;

    Gfx_Image *anim_sheet;
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
    float32 anim_duration;

    float32 anim_start_time;

    bool active;
    Vector2 pos;
    float elapsed_time;
    bool has_custom_size;
    Vector2 custom_size;

    // bool needs_fast_updating; // basically if the pos of the animation changes
    bool is_held; // if animation is in players hand (or travels with player)

} Animation;

Animation animations[MAX_ANIMATIONS];


void draw_animation(Animation* anim, float64 now, Vector2 pos){

    // printf("DRAWGIN ANIMATION\n");

    // UPDATE ANIMATION
    float32 anim_elapsed = fmodf(now - anim->anim_start_time, anim->anim_duration);

    // Get current progression in animation from 0.0 to 1.0
    float32 anim_progression_factor = anim_elapsed / anim->anim_duration;
    
    u32 anim_current_index = anim->anim_number_of_frames * anim_progression_factor;
    u32 anim_absolute_index_in_sheet = anim->anim_start_index + anim_current_index;
    
    u32 anim_index_x = anim_absolute_index_in_sheet % anim->number_of_columns;
    u32 anim_index_y = anim_absolute_index_in_sheet / anim->number_of_columns + 1;
    
    u32 anim_sheet_pos_x = anim_index_x * anim->anim_frame_width;
    u32 anim_sheet_pos_y = (anim->number_of_rows - anim_index_y) * anim->anim_frame_height; // Remember, Y inverted.

    
    // DRAW ANIMATION
    // Draw_Quad *quad = draw_image(anim->anim_sheet, v2(pos.x, pos.y), v2(anim->anim_frame_width*4, anim->anim_frame_height*4), COLOR_WHITE);
    // Draw_Quad *quad = draw_image(anim->anim_sheet, v2(pos.x, pos.y), v2(anim->anim_frame_width, anim->anim_frame_height), COLOR_WHITE);
    Matrix4 xform = m4_identity;
    Vector2 size;

    if (anim->has_custom_size){
        size = anim->custom_size;
        xform = m4_translate(xform, v3(size.x * -0.5, 0, 0));
    }
    else{
        size.x = anim->anim_frame_width;
        size.y = anim->anim_frame_height;
        xform = m4_translate(xform, v3(anim->anim_frame_width * -0.5, anim->anim_frame_height * -0.5, 0));
    }

    xform = m4_translate(xform, v3(pos.x, pos.y, 0));
    // Draw_Quad *quad = draw_image_xform(anim->anim_sheet, xform, v2(anim->anim_frame_width, anim->anim_frame_height), COLOR_WHITE);
    Draw_Quad *quad = draw_image_xform(anim->anim_sheet, xform, v2(size.x, size.y), COLOR_WHITE);
    quad->uv.x1 = (float32)(anim_sheet_pos_x)/(float32)anim->anim_sheet->width;
    quad->uv.y1 = (float32)(anim_sheet_pos_y)/(float32)anim->anim_sheet->height;
    quad->uv.x2 = (float32)(anim_sheet_pos_x+anim->anim_frame_width) /(float32)anim->anim_sheet->width;
    quad->uv.y2 = (float32)(anim_sheet_pos_y+anim->anim_frame_height)/(float32)anim->anim_sheet->height;

    // Visualize sprite sheet animation
    Vector2 sheet_pos = v2(-window.width/2+40, -window.height/2+40);
    Vector2 sheet_size = v2(anim->anim_sheet->width, anim->anim_sheet->height);
    // Vector2 sheet_size = v2(10, 10);
    Vector2 frame_pos_in_sheet = v2(anim_sheet_pos_x, anim_sheet_pos_y);
    Vector2 frame_size = v2(anim->anim_frame_width, anim->anim_frame_height);

    // draw_rect(v2_add(sheet_pos, frame_pos_in_sheet), frame_size, COLOR_WHITE); // Draw white rect on current frame
    // draw_image(anim->anim_sheet, sheet_pos, sheet_size, COLOR_WHITE); // Draw the seet
}

Animation* setup_torch_animation(){
    Animation* anim;
    anim = alloc(get_heap_allocator(), sizeof(Animation));

    anim->animation_id = ANIM_torch;
    anim->name = STR("Torch animation");

    anim->anim_sheet = load_image_from_disk(STR("res/animations/animation_torch.png"), get_heap_allocator());
    anim->number_of_columns = 4;
    anim->number_of_rows = 1;
    anim->total_number_of_frames = anim->number_of_rows * anim->number_of_columns;
    
    anim->anim_frame_width  = anim->anim_sheet->width  / anim->number_of_columns;
    anim->anim_frame_height = anim->anim_sheet->height / anim->number_of_rows;
    
    anim->anim_start_frame_x = 0;
    anim->anim_start_frame_y = 0;
    anim->anim_end_frame_x = 3;
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
    anim->playback_fps = 10;
    anim->anim_time_per_frame = 1.0 / anim->playback_fps;
    anim->anim_duration = anim->anim_time_per_frame * (float32)anim->anim_number_of_frames;

    anim->anim_start_time = os_get_elapsed_seconds();

    anim->is_held = true;

    return anim;
}

Animation* setup_crafting_animation(){
    Animation* anim;
    anim = alloc(get_heap_allocator(), sizeof(Animation));

    anim->animation_id = ANIM_crafting;
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
    anim->anim_duration = anim->anim_time_per_frame * (float32)anim->anim_number_of_frames;

    // anim->anim_start_time = os_get_elapsed_seconds();

    anim->active = false;
    anim->pos = v2(0, 0);
    anim->has_custom_size = true;
    anim->custom_size = v2(12,12);

    return anim;
}

Animation* setup_smelting_animation() {
    Animation* anim;
    anim = alloc(get_heap_allocator(), sizeof(Animation));

    anim->animation_id = ANIM_crafting;
    anim->name = STR("Smelting animation");

    anim->anim_sheet = load_image_from_disk(STR("res/animations/smoke_test.png"), get_heap_allocator());
    anim->number_of_columns = 8;
    anim->number_of_rows = 7;
    anim->total_number_of_frames = anim->number_of_rows * anim->number_of_columns;
    
    anim->anim_frame_width  = anim->anim_sheet->width  / anim->number_of_columns;
    anim->anim_frame_height = anim->anim_sheet->height / anim->number_of_rows;
    
    anim->anim_start_frame_x = 0;
    anim->anim_start_frame_y = 0;
    anim->anim_end_frame_x = 1;
    anim->anim_end_frame_y = 6;
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
    anim->anim_duration = anim->anim_time_per_frame * (float32)anim->anim_number_of_frames;

    // anim->anim_start_time = os_get_elapsed_seconds();

    anim->active = false;
    anim->pos = v2(0, 0);
    anim->has_custom_size = true;
    anim->custom_size = v2(30, 30);

    return anim;
}

void trigger_animation(Animation* anim, float64 start_time, Vector2 pos){
    for (int i = 0; i < MAX_ANIMATIONS; i++){
        if (!animations[i].active) { // && animations[i].animation_id == anim->animation_id
            anim->anim_start_time = os_get_elapsed_seconds();
            anim->active = true;
            anim->pos = pos;
            animations[i] = *anim;
            // printf("triggered animation at %.0f, %.0f\n", anim->pos);
            break;
        }
    }
}

void update_animations(float64 delta_t){
    // NOTE: should "MAX_ANIMATIONS" be replaced with active animation count?
    for (int i = 0; i < MAX_ANIMATIONS; i++){
        Animation* anim = &animations[i];
        if (!anim->active) continue;

        anim->elapsed_time += delta_t;

        float t = anim->elapsed_time / anim->anim_duration;
        if (t >= 1.0f) {
            anim->active = false;
            printf("ANIMATION '%s' not active\n", anim->name);
            continue;
        }

        if (anim->is_held){
            anim->pos = get_player_pos();
            anim->pos.x += 4;
            anim->pos.y += 3;
        }

        // draw_animation(anim, os_get_current_time_in_seconds(), anim->pos); // deprecated
        draw_animation(anim, os_get_elapsed_seconds(), anim->pos);
    }
}



#endif