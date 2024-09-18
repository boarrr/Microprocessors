#include <stdint.h>

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pixel;

pixel display[256];

void clear_display()
{
    for (int i = 0; i < 256; i++)
    {
        display[i].red = 0;
        display[i].green = 0;
        display[i].blue = 0;
    }
}

void clear_image(pixel* image)
{
    for (int i = 0; i < 256; i++)
    {
        (*(image + i)).red = 0;
        (*(image + i)).blue = 0;
        (*(image + i)).green = 0;
    }
}

int main(void)
{

}