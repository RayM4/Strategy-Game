#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <random>
#include <vector>
#include <string>

using namespace std;

float RNG();

GLuint LoadTexture(const char*, GLuint);

void drawText(GLuint fontTexture, string text, float size, float spacing, float r, float g, float b, float a, float x, float y);
