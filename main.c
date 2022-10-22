#include <stdio.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "./headers/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./headers/stb_image_write.h"

unsigned char* uc_arrayNew_1d(int _size) {
    return (unsigned char*)calloc(_size, sizeof(unsigned char));
}

unsigned char* subtract_background(unsigned char* bg1, unsigned char* bg2, 
        unsigned char* observed_image, int w, int h, float forgiveness) {
    // Extract masked image
    unsigned char* masked_image = uc_arrayNew_1d(w * h * 4);
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            int pixel_index = (y*w + x) * 4;

            // Check if the 2 pixels have the same color with provided forgiveness
            // Assuming the 2 pixels aren't transparent
            int is_same_color = 1;
            float difference = 0;
            for (int ch = 0; ch < 3; ch++) {
                int index = pixel_index + ch;
                difference += abs(bg1[index] - observed_image[index]);
            }
            if (difference > forgiveness * 3) {
                is_same_color = 0;
            }

            if (!is_same_color) {
                for (int ch = 0; ch < 4; ch++) {
                    int index = pixel_index + ch;
                    masked_image[index] = observed_image[index];
                }
            }   
        }
    }

    // Put masked image on background 2
    unsigned char* new_image = uc_arrayNew_1d(w * h * 3);
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            if (masked_image[(y*w + x) * 4 + 3] == 0) {
                for (int ch = 0; ch < 3; ch++) {
                    int index = (y*w + x) * 3 + ch;
                    new_image[index] = bg2[index];
                }
            }
            else {
                for (int ch = 0; ch < 3; ch++) {
                    new_image[(y*w + x) * 3 + ch] = masked_image[(y*w + x) * 4 + ch];
                }
            }
        }
    }

    return new_image;
}

int main() {
    printf("\n");

    int w, h, channel;
    char bg1_path[] = "./images/background.png";
    char bg2_path[] = "./images/weather_forecast.jpg";
    char observed_image_path[] = "./images/foreground.png";
    char save_path[] = "./images/new_image.png";

    unsigned char* bg1 = stbi_load(bg1_path, &w, &h, NULL, 0);
    unsigned char* bg2 = stbi_load(bg2_path, &w, &h, NULL, 0);
    unsigned char* observed_image = stbi_load(observed_image_path, &w, &h, NULL, 0);
    if (bg1 == NULL || bg2 == NULL || observed_image == NULL) {
        printf("bro forgor the actual image\n");
        exit(1);
    }
    printf("width: %d, height: %d\n", w, h);

    float forgiveness = 50;
    unsigned char* new_image = subtract_background(bg1, bg2, observed_image,
            w, h, forgiveness);

    stbi_write_jpg(save_path, w, h, 3, new_image, 100);
    printf("so i got this new image in the path you give me\n");
}