#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#define MAX_ANIMATIONS 50
int active_animations = 0;
unsigned long id_counter = 0;


typedef enum AnimationID {
    ANIM_nil,

    ANIM_held_torch,
    ANIM_ground_torch,
    ANIM_crafting,

    ANIM_MAX,
} AnimationID;

typedef struct Animation{

    unsigned long unique_id;
    const char* name;
    Gfx_Image *anim_sheet;
    AnimationID anim_id;
    Vector2 pos;
    bool has_offset;
    Vector2 offset;
    float start_time;
    bool active;
    bool looping;
    bool has_custom_size;
    Vector2 custom_size;
    u32 number_of_columns;
    u32 number_of_rows;
    u32 total_number_of_frames;
    u32 anim_frame_width;
    u32 anim_frame_height;
    u32 anim_start_index;
    u32 anim_end_index;
    u32 anim_number_of_frames;
    float playback_fps;
    float anim_time_per_frame;
    float cycle_duration;
    float duration;
    float elapsed_time;

} Animation;

Animation animations[MAX_ANIMATIONS];

// animation instances
Animation* crafting_animation;
Animation* smelting_animation;
Animation* held_torch_animation;
Animation* ground_torch_animation;





unsigned long get_new_id() {
    return ++id_counter;
}

void draw_animation(Animation* anim, float64 now){

    // printf("Drawing '%s'\n", anim->name);

    float anim_elapsed = fmodf(now - anim->start_time, anim->cycle_duration);
    float anim_progression_factor = anim_elapsed / anim->cycle_duration;
    
    u32 anim_current_index = anim->anim_number_of_frames * anim_progression_factor;
    u32 anim_absolute_index_in_sheet = anim->anim_start_index + anim_current_index;
    u32 anim_index_x = anim_absolute_index_in_sheet % anim->number_of_columns;
    // u32 anim_index_y = anim_absolute_index_in_sheet / anim->number_of_columns + 1;
    u32 anim_index_y = anim_absolute_index_in_sheet / anim->number_of_columns;
    u32 anim_sheet_pos_x = anim_index_x * anim->anim_frame_width;
    // u32 anim_sheet_pos_y = (anim->number_of_rows - anim_index_y) * anim->anim_frame_height; // Remember, Y inverted.
    u32 anim_sheet_pos_y = anim_index_y * anim->anim_frame_height;

    
    Vector2 draw_pos = anim->pos;
    if (anim->has_custom_size) draw_pos.x += anim->custom_size.x * -0.5f;

    if (anim->has_offset){
        draw_pos.x += anim->offset.x;
        draw_pos.y += anim->offset.y;
    }

    // if (anim->has_custom_size){
    //     size = anim->custom_size;
    //     xform = m4_translate(xform, v3(size.x * -0.5, 0, 0));
    // }
    // else{
    //     size.x = anim->anim_frame_width;
    //     size.y = anim->anim_frame_height;
    //     xform = m4_translate(xform, v3(anim->anim_frame_width * -0.5, anim->anim_frame_height * -0.5, 0));
    // }

    Draw_Quad* quad;

    if (anim->has_custom_size) quad = draw_image(anim->anim_sheet, draw_pos, anim->custom_size, COLOR_WHITE);
    else quad = draw_image(anim->anim_sheet, draw_pos, v2(anim->anim_frame_width, anim->anim_frame_height), COLOR_WHITE);
    
    quad->uv.x1 = (float32)(anim_sheet_pos_x) / (float32)anim->anim_sheet->width;
    quad->uv.y1 = (float32)(anim_sheet_pos_y) / (float32)anim->anim_sheet->height;
    quad->uv.x2 = (float32)(anim_sheet_pos_x+anim->anim_frame_width) / (float32)anim->anim_sheet->width;
    quad->uv.y2 = (float32)(anim_sheet_pos_y+anim->anim_frame_height) / (float32)anim->anim_sheet->height;
}

Animation* setup_held_torch_animation(){
    Animation* anim = alloc(get_heap_allocator(), sizeof(Animation));

    anim->anim_id = ANIM_held_torch;
    anim->name = "Torch animation";
    anim->anim_sheet = load_image_from_disk(STR("res/animations/animation_torch.png"), get_heap_allocator());

    anim->number_of_columns = 4;
    anim->number_of_rows = 1;
    anim->total_number_of_frames = anim->number_of_rows * anim->number_of_columns;
    
    anim->anim_frame_width  = anim->anim_sheet->width  / anim->number_of_columns;
    anim->anim_frame_height = anim->anim_sheet->height / anim->number_of_rows;
    
    // anim->anim_start_frame_x = 0;
    // anim->anim_start_frame_y = 0;
    // anim->anim_end_frame_x = 3;
    // anim->anim_end_frame_y = 0;
    // anim->anim_start_index = anim->anim_start_frame_y * anim->number_of_columns + anim->anim_start_frame_x;
    // anim->anim_end_index   = anim->anim_end_frame_y   * anim->number_of_columns + anim->anim_end_frame_x;
    // anim->anim_number_of_frames = max(anim->anim_end_index, anim->anim_start_index)-min(anim->anim_end_index, anim->anim_start_index)+1;

    // assert(anim->anim_end_index > anim->anim_start_index, "The last frame must come before the first frame");
    // assert(anim->anim_start_frame_x < anim->number_of_columns, "anim_start_frame_x is out of bounds");
    // assert(anim->anim_start_frame_y < anim->number_of_rows, "anim_start_frame_y is out of bounds");
    // assert(anim->anim_end_frame_x < anim->number_of_columns, "anim_end_frame_x is out of bounds");
    // assert(anim->anim_end_frame_y < anim->number_of_rows, "anim_end_frame_y is out of bounds");

    anim->anim_start_index = 0;
    anim->anim_end_index = 3;
    anim->anim_number_of_frames = anim->anim_end_index - anim->anim_start_index + 1;

    anim->playback_fps = 10;
    anim->anim_time_per_frame = 1.0 / anim->playback_fps;
    anim->cycle_duration = anim->anim_time_per_frame * (float32)anim->anim_number_of_frames;

    anim->has_offset = true;
    anim->offset = v2(3, -1);

    anim->active = false;
    anim->looping = true;
    anim->has_custom_size = false;
    // anim->custom_size = v2(8, 12);
    anim->unique_id = get_new_id();

    return anim;
}

Animation* setup_ground_torch_animation(){
    Animation* anim = alloc(get_heap_allocator(), sizeof(Animation));

    anim->anim_id = ANIM_ground_torch;
    anim->name = "Torch animation (ground)";
    anim->anim_sheet = load_image_from_disk(STR("res/animations/animation_torch.png"), get_heap_allocator());

    anim->number_of_columns = 4;
    anim->number_of_rows = 1;
    anim->total_number_of_frames = anim->number_of_rows * anim->number_of_columns;
    
    anim->anim_frame_width  = anim->anim_sheet->width  / anim->number_of_columns;
    anim->anim_frame_height = anim->anim_sheet->height / anim->number_of_rows;
    
    // anim->anim_start_frame_x = 0;
    // anim->anim_start_frame_y = 0;
    // anim->anim_end_frame_x = 3;
    // anim->anim_end_frame_y = 0;
    // anim->anim_start_index = anim->anim_start_frame_y * anim->number_of_columns + anim->anim_start_frame_x;
    // anim->anim_end_index   = anim->anim_end_frame_y   * anim->number_of_columns + anim->anim_end_frame_x;
    // anim->anim_number_of_frames = max(anim->anim_end_index, anim->anim_start_index)-min(anim->anim_end_index, anim->anim_start_index)+1;

    // assert(anim->anim_end_index > anim->anim_start_index, "The last frame must come before the first frame");
    // assert(anim->anim_start_frame_x < anim->number_of_columns, "anim_start_frame_x is out of bounds");
    // assert(anim->anim_start_frame_y < anim->number_of_rows, "anim_start_frame_y is out of bounds");
    // assert(anim->anim_end_frame_x < anim->number_of_columns, "anim_end_frame_x is out of bounds");
    // assert(anim->anim_end_frame_y < anim->number_of_rows, "anim_end_frame_y is out of bounds");

    anim->anim_start_index = 0;
    anim->anim_end_index = 3;
    anim->anim_number_of_frames = anim->anim_end_index - anim->anim_start_index + 1;

    anim->playback_fps = 10;
    anim->anim_time_per_frame = 1.0 / anim->playback_fps;
    anim->cycle_duration = anim->anim_time_per_frame * (float32)anim->anim_number_of_frames;

    anim->has_offset = false;
    // anim->offset = v2(3, -1);

    anim->active = false;
    anim->looping = true;
    anim->has_custom_size = false;
    // anim->custom_size = v2(8, 12);
    anim->unique_id = get_new_id();

    return anim;
}

Animation* setup_crafting_animation(){
    Animation* anim  = alloc(get_heap_allocator(), sizeof(Animation));

    anim->anim_id = ANIM_crafting;
    anim->name = "Crafting animation";
    anim->anim_sheet = load_image_from_disk(STR("res/animations/crafting_animation3.png"), get_heap_allocator());

    anim->number_of_columns = 10;
    anim->number_of_rows = 1;
    anim->total_number_of_frames = anim->number_of_rows * anim->number_of_columns;
    
    anim->anim_frame_width  = anim->anim_sheet->width  / anim->number_of_columns;
    anim->anim_frame_height = anim->anim_sheet->height / anim->number_of_rows;
    
    // anim->anim_start_frame_x = 0;
    // anim->anim_start_frame_y = 0;
    // anim->anim_end_frame_x = 9;
    // anim->anim_end_frame_y = 0;
    // anim->anim_start_index = anim->anim_start_frame_y * anim->number_of_columns + anim->anim_start_frame_x;
    // anim->anim_end_index   = anim->anim_end_frame_y   * anim->number_of_columns + anim->anim_end_frame_x;
    // anim->anim_number_of_frames = max(anim->anim_end_index, anim->anim_start_index)-min(anim->anim_end_index, anim->anim_start_index)+1;


    // assert(anim->anim_end_index > anim->anim_start_index, "The last frame must come before the first frame");
    // assert(anim->anim_start_frame_x < anim->number_of_columns, "anim_start_frame_x is out of bounds");
    // assert(anim->anim_start_frame_y < anim->number_of_rows, "anim_start_frame_y is out of bounds");
    // assert(anim->anim_end_frame_x < anim->number_of_columns, "anim_end_frame_x is out of bounds");
    // assert(anim->anim_end_frame_y < anim->number_of_rows, "anim_end_frame_y is out of bounds");

    anim->anim_start_index = 0;
    anim->anim_end_index = 9;
    anim->anim_number_of_frames = anim->anim_end_index - anim->anim_start_index + 1;

    anim->playback_fps = 15;
    // anim->playback_fps = (anim->number_of_columns * anim->number_of_rows) / 4.0f;
    anim->anim_time_per_frame = 1.0 / anim->playback_fps;
    anim->cycle_duration = anim->anim_time_per_frame * (float32)anim->anim_number_of_frames;

    anim->active = false;
    anim->has_custom_size = true;
    anim->custom_size = v2(12,12);
    anim->unique_id = get_new_id();
    // anim->pos = v2(0, 0);
    // anim->loop = true;

    return anim;
}

Animation* setup_smelting_animation() {
    Animation* anim = alloc(get_heap_allocator(), sizeof(Animation));

    anim->anim_id = ANIM_crafting;
    anim->name = "Smelting animation";
    anim->anim_sheet = load_image_from_disk(STR("res/animations/smoke_test.png"), get_heap_allocator());

    anim->number_of_columns = 8;
    anim->number_of_rows = 7;
    anim->total_number_of_frames = anim->number_of_rows * anim->number_of_columns;
    
    anim->anim_frame_width  = anim->anim_sheet->width  / anim->number_of_columns;
    anim->anim_frame_height = anim->anim_sheet->height / anim->number_of_rows;
    
    // anim->anim_start_frame_x = 0;
    // anim->anim_start_frame_y = 0;
    // anim->anim_end_frame_x = 1;
    // anim->anim_end_frame_y = 6;
    // anim->anim_start_index = anim->anim_start_frame_y * anim->number_of_columns + anim->anim_start_frame_x;
    // anim->anim_end_index   = anim->anim_end_frame_y   * anim->number_of_columns + anim->anim_end_frame_x;
    // anim->anim_number_of_frames = max(anim->anim_end_index, anim->anim_start_index)-min(anim->anim_end_index, anim->anim_start_index)+1;

    // assert(anim->anim_end_index > anim->anim_start_index, "The last frame must come before the first frame");
    // assert(anim->anim_start_frame_x < anim->number_of_columns, "anim_start_frame_x is out of bounds");
    // assert(anim->anim_start_frame_y < anim->number_of_rows, "anim_start_frame_y is out of bounds");
    // assert(anim->anim_end_frame_x < anim->number_of_columns, "anim_end_frame_x is out of bounds");
    // assert(anim->anim_end_frame_y < anim->number_of_rows, "anim_end_frame_y is out of bounds");

    anim->anim_start_index = 0;
    anim->anim_end_index = 6;
    anim->anim_number_of_frames = anim->anim_end_index - anim->anim_start_index + 1;

    // Calculate duration per frame in seconds
    anim->playback_fps = 15;
    // anim->playback_fps = (anim->number_of_columns * anim->number_of_rows) / 4.0f;
    anim->anim_time_per_frame = 1.0 / anim->playback_fps;
    anim->cycle_duration = anim->anim_time_per_frame * (float32)anim->anim_number_of_frames;

    anim->active = false;
    anim->has_custom_size = false;
    anim->custom_size = v2(30, 30);
    anim->unique_id = get_new_id();

    return anim;
}

void trigger_animation(Animation* base_anim, Vector2 pos, float duration){

    if (active_animations >= MAX_ANIMATIONS){
        printf("Max animations reached\n");
        return;
    }

    for (int i = 0; i < MAX_ANIMATIONS; i++) {
        // Check if the animation with the same ID is already active
        if (animations[i].active && animations[i].anim_id == base_anim->anim_id) {
            // Update the position of the existing animation
            animations[i].pos = pos;
            return;
        }
    }

    for (int i = 0; i < MAX_ANIMATIONS; i++){
        if (!animations[i].active) {
            animations[i] = *base_anim;
            animations[i].active = true;
            animations[i].pos = pos;
            animations[i].start_time = os_get_elapsed_seconds();
            // animations[i].unique_id = get_new_id();
            animations[i].duration = (duration == 0) ? animations[i].cycle_duration : duration;
            active_animations++;

            printf("Triggered animation '%s' @ %.0f, %.0f\tduration = %.1f\n", animations[i].name, pos.x, pos.y, animations[i].duration);
            return;
        }
    }
}

void update_animations(float64 d_time){
    for (int i = 0; i < MAX_ANIMATIONS; i++){
        Animation* anim = &animations[i];
        if (!anim->active) continue;

        // if (anim->is_held){
        //     anim->pos = get_player_pos();
        //     anim->pos.x += 4;
        //     anim->pos.y += 3;
        // }

        anim->elapsed_time += d_time;
        if (anim->elapsed_time >= anim->duration) {

            if (anim->looping){
                anim->elapsed_time = 0;
                anim->start_time = os_get_elapsed_seconds();
            }
            else{
                anim->active = false;
                active_animations--;
                printf("Animation '%s' completed. Active animations left: %d\n", anim->name, active_animations);
            }
        }
        else {
            draw_animation(anim, os_get_elapsed_seconds());
        }
    }
}

void update_player_torch_animation(Vector2 player_pos) {
    if (held_torch_animation == NULL) { //  || !held_torch_animation->active
        // Trigger animation only once when starting to hold the torch
        held_torch_animation = setup_held_torch_animation();
        trigger_animation(held_torch_animation, player_pos, 0);
        return;
    }
    else {
        held_torch_animation->active = true;
        trigger_animation(held_torch_animation, player_pos, 0);
        // Update position to follow the player
        // for (int i = 0; i < MAX_ANIMATIONS; i++){
        //     Animation* anim = &animations[i];
        //     if (anim->active && anim->anim_id == ANIM_torch) {
        //         anim->pos = player_pos; // Update animation position to match player position
        //         break;
        //     }
        // }
    }
}

void kill_animation(Animation* anim_to_kill){
    if (anim_to_kill){
        for (int i = 0; i < MAX_ANIMATIONS; i++) {
            Animation* anim = &animations[i];
            if (anim->unique_id == anim_to_kill->unique_id) {
                printf("Animation '%s' killed\n", anim_to_kill->name);
                anim->active = false;
                active_animations--;
                memset(anim, 0, sizeof(Animation));
                return;
            }
        }
    }
    else printf("Can't kill animation. Its NULL\n");
}

void stop_player_torch_animation() {
    if (held_torch_animation != NULL && held_torch_animation->active) {
        kill_animation(held_torch_animation);
        held_torch_animation = NULL;
    }
}

// void kill_animation_now(Animation* animation){
    //     for (int i = 0; i < active_animations; i++){
    //         Animation* active_animation = &animations[i];
    //         if (active_animation->animation_id == animation->animation_id){
    //             active_animation->active = false;
    //             memset(&animations[i], 0, sizeof(Animation));
    //             printf("KILLED animation '%s'\n", animation->name);
    //             break;
    //         }
    //     }
// }


#endif