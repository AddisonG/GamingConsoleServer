#ifndef _LCD_STATUS_FB_H 
#define _LCD_STATUS_FB_H

#include <stdbool.h>

struct fb {
	int fb_width;
	int fb_height;
	unsigned char *fb_buf;
};

struct fb *fb_init(const char *fbdev);
void swap_buffer(struct fb *fb);
void clear_buffer(struct fb *fb);
void close_fb(struct fb *fb);

/* General Drawing */
void set_bit(struct fb *fb, int x, int y);

/* Bitmap Drawing */
void render_bitmap(struct fb *fb, const char *pngfile, int x_orig, int y_orig);

/* Glyph Drawing */
struct font {
	unsigned int glyph_height;
	unsigned int glyph_width;
	unsigned int glyph_size;
	unsigned int num_glyphs;
	unsigned char *glyphs;
};

struct font * load_font(const char *fontfile);
void free_font(struct font *ft);
void render_string(struct fb *fb, struct font *ft, const unsigned char *string, bool invert, int x_orig, int y_orig);


#endif /* _LCD_STATUS_FB_H */

