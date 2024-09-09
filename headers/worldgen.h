#ifndef WORLDGEN_H
#define WORLDGEN_H

#define STB_PERLIN_IMPLEMENTATION
#include "/thirdparty/stb_perlin.h" // Perlin noise

// #include "/thirdparty/lpng1643/png.h"
// #include "/thirdparty/lpng1643/png.h"
#include "png.h"


typedef struct {
    unsigned char r, g, b;
} Color;

typedef struct {
    float start_threshold;
    float end_threshold;
    Color color;
} Biome;

const Color forest = {0, 200, 0};
const Color pine_forest = {86, 163, 108};
const Color magical_forest = {97, 0, 194};
const Color desert = {194, 158, 0};
const Color polar = {255, 255, 255};
const Color NIL = {255, 0, 0};

// parameters
float scale = 100.0f;           // Frequency (Noise Scale)
int octaves = 10;                // Number of octaves for noise layers      // default = 5
float persistence = 0.5f;       // Persistence for amplitude of each octave // default = 0.5f
float lacunarity = 2.0f;        // Frequency increase for each octave       // default = 2.0f
float amplitude = 1.0f;         // Overall amplitude multiplier             // default = 1.0f

// world size
int width = 512;
int height = 512;

// biome lookup table
Biome biomes_table[] = {
    {0.0f, 0.4f, forest},
    {0.4f, 0.65f, pine_forest},
    {0.65f, 0.8f, magical_forest},
    {0.8f, 0.9f, desert},
    {0.9f, 1.0f, polar}
};

int num_biomes = sizeof(biomes_table) / sizeof(biomes_table[0]);

int colors_equal(Color c1, Color c2) {
    return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
}

Color apply_biome_adjacency_rules(Color biome_color, int x, int y, unsigned char* image, int width, int height) {
    int neighbors[4][2] = {
        {x - 1, y},     // Left
        {x + 1, y},     // Right
        {x, y - 1},     // Top
        {x, y + 1}      // Bottom
    };

    for (int i = 0; i < 4; i++) {
        int nx = neighbors[i][0];
        int ny = neighbors[i][1];

        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
            int neighbor_index = (ny * width + nx) * 3;
            Color neighbor_color = {image[neighbor_index], image[neighbor_index + 1], image[neighbor_index + 2]};

            // RULE 1: Desert can't be adjacent to Magical Forest
            if (colors_equal(biome_color, desert) && colors_equal(neighbor_color, magical_forest)) {
                return forest; // Or some other transitional biome
            }
            if (colors_equal(biome_color, magical_forest) && colors_equal(neighbor_color, desert)) {
                return forest;
            }

            // // RULE 2: Polar can't spawn inside Magical Forest
            // if (colors_equal(biome_color, polar) && colors_equal(neighbor_color, magical_forest)) {
            //     return pine_forest; // Or some other intermediary biome
            // }
            // if (colors_equal(biome_color, magical_forest) && colors_equal(neighbor_color, polar)) {
            //     return pine_forest;
            // }

            // RULE 3: Polar can't spawn inside Desert
            // if (colors_equal(biome_color, polar) && colors_equal(neighbor_color, desert)) {
            //     return forest; // Or another transitional biome
            // }
            // if (colors_equal(biome_color, desert) && colors_equal(neighbor_color, polar)) {
            //     return forest;
            // }
        }
    }

    return biome_color;
}

Color get_biome_color_with_buffer(float noise_value, int x, int y, unsigned char* image) {

    Color biome_color = forest;

    if (noise_value < 0.3f) {
        biome_color = desert;
    }
    
    else if (noise_value < 0.5f) {
        biome_color = forest;
    }
    
    else if (noise_value < 0.65f) {
        biome_color = pine_forest;
    }
    
    // else if (noise_value < 0.8f) {
    else{
        biome_color = magical_forest;
    }
    
    // else {
    //     biome_color = NIL;
    // }

    return apply_biome_adjacency_rules(biome_color, x, y, image, width, height);
}


void generate_world(unsigned char* image_data, int width, int height, int seed) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float xf = (float)x / scale;
            float yf = (float)y / scale;
            float noise_value = 0.0f;
            float freq = 1.0f;
            float amp = amplitude;
            // printf("%d\n", x);

            // Generate noise with multiple octaves
            for (int i = 0; i < octaves; i++) {
                noise_value += amp * stb_perlin_noise3(freq * xf, freq * yf, seed, 0, 0, 0);
                amp *= persistence;
                freq *= lacunarity;
            }

            // Normalize noise to range [0, 1]
            noise_value = (noise_value + 1.0f) / 2.0f;

            // Get the corresponding biome color based on ranges
            // Color color = get_biome_color(noise_value);
            Color color = get_biome_color_with_buffer(noise_value, x, y, image_data);

            // Store the biome color in the image_data array
            int index = (y * width + x) * 3;
            image_data[index]       = color.r;
            image_data[index + 1]   = color.g;
            image_data[index + 2]   = color.b;
        }
    }
    return;
}

bool save_png_image(const char *filename, int width, int height, unsigned char* image_data) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("Error: Could not open file %s for writing.\n", filename);
        return 0;
    }

    // Initialize write structure
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fclose(fp);
        printf("Error: Could not create PNG write struct.\n");
        return 0;
    }

    // Initialize info structure
    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, NULL);
        fclose(fp);
        printf("Error: Could not create PNG info struct.\n");
        return 0;
    }

    // Error handling
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        printf("Error: Error during PNG creation.\n");
        return 0;
    }

    png_init_io(png, fp);

    // Write header (8 bit color depth, RGB format)
    png_set_IHDR(
        png,
        info,
        width, height,
        8,  // Bit depth
        PNG_COLOR_TYPE_RGB,  // Color type: RGB
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );

    png_write_info(png, info);

    // Write image data (in rows)
    png_bytep row_pointers[height];
    for (int y = 0; y < height; y++) {
        row_pointers[y] = image_data + (y * width * 3);  // Each row is width * 3 bytes (RGB)
    }

    png_write_image(png, row_pointers);
    png_write_end(png, NULL);

    // Cleanup
    png_destroy_write_struct(&png, &info);
    fclose(fp);
    printf("PNG file %s successfully created!\n", filename);
    return 1;
}

bool generate_new_world(){
    unsigned char* image_data = (unsigned char*)malloc(image_width * image_height * 3 * sizeof(unsigned char));
    if (image_data == NULL){
        return 0;
    }

    int seed = 1;
    generate_world(image_data, image_width, image_height, seed);

    if (!save_png_image("world.png", image_width, image_height, image_data)){
        return 0;
    }

    free(image_data);

    return 1;
}

#endif