#ifndef ANIMATIONS_H
#define ANIMATIONS_H

typedef struct Animation{
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
} Animation;

void draw_animation(Animation* anim, float64 now, Vector2 pos){

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
    Draw_Quad *quad = draw_image(anim->anim_sheet, v2(pos.x, pos.y), v2(anim->anim_frame_width*4, anim->anim_frame_height*4), COLOR_WHITE);
    quad->uv.x1 = (float32)(anim_sheet_pos_x)/(float32)anim->anim_sheet->width;
    quad->uv.y1 = (float32)(anim_sheet_pos_y)/(float32)anim->anim_sheet->height;
    quad->uv.x2 = (float32)(anim_sheet_pos_x+anim->anim_frame_width) /(float32)anim->anim_sheet->width;
    quad->uv.y2 = (float32)(anim_sheet_pos_y+anim->anim_frame_height)/(float32)anim->anim_sheet->height;

    // Visualize sprite sheet animation
    Vector2 sheet_pos = v2(-window.width/2+40, -window.height/2+40);
    Vector2 sheet_size = v2(anim->anim_sheet->width, anim->anim_sheet->height);
    Vector2 frame_pos_in_sheet = v2(anim_sheet_pos_x, anim_sheet_pos_y);
    Vector2 frame_size = v2(anim->anim_frame_width, anim->anim_frame_height);
    draw_rect(sheet_pos, sheet_size, COLOR_BLACK); // Draw black background
    draw_rect(v2_add(sheet_pos, frame_pos_in_sheet), frame_size, COLOR_WHITE); // Draw white rect on current frame
    draw_image(anim->anim_sheet, sheet_pos, sheet_size, COLOR_WHITE); // Draw the seet
}

Animation* setup_torch_animation(){
    Animation* anim;
    anim = alloc(get_heap_allocator(), sizeof(Animation));

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

    return anim;
}

#endif