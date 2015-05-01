#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

class SpriteSheet {
public:
	SpriteSheet();
	SpriteSheet(const char * image_path);

	void draw(float scale, float x, float y, float u, float v, float w, float h);
	void draw(float scale, float x, float y, float u, float v, float w, float h, bool flip);
	void drawf(float scale, float x, float y, float u, float v, float w, float h);

	GLuint getID();

private:
	GLuint LoadTexture(const char *image_path);

	GLuint textureID;
};