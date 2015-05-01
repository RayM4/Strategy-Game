#pragma once
#include "SpriteSheet.h"

SpriteSheet::SpriteSheet() {};

SpriteSheet::SpriteSheet(const char * image_path)
	: textureID(LoadTexture(image_path)) {};

void SpriteSheet::draw(float scale, float x, float y, float u, float v, float width, float height) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	//glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	//glTranslatef(x, y, 0.0);

	//glTranslatef(-2.385f, 2.38f, 0.0f);
	glTranslatef(-2.62f, 2.62f, 0.0f);  //spriteSheet draw

	GLfloat quad[] = { -width * scale, height * scale, -width * scale, -height * scale, width * scale, -height * scale, width * scale, height * scale };

	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };

	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();
}



void SpriteSheet::draw(float scale, float x, float y, float u, float v, float width, float height, bool flip) {

	if (flip){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);

		//glMatrixMode(GL_MODELVIEW);
		//glPushMatrix();
		//glTranslatef(x, y, 0.0);

		//glTranslatef(-2.385f, 2.38f, 0.0f);
		glTranslatef(-2.62f, 2.62f, 0.0f);  //spriteSheet draw

		GLfloat quad[] = { -width * scale, height * scale, -width * scale, -height * scale, width * scale, -height * scale, width * scale, height * scale };

		glVertexPointer(2, GL_FLOAT, 0, quad);
		glEnableClientState(GL_VERTEX_ARRAY);

		GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };

		glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawArrays(GL_QUADS, 0, 4);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}
	else if (!flip){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);

		//glMatrixMode(GL_MODELVIEW);
		//glPushMatrix();
		//glTranslatef(x, y, 0.0);

		//glTranslatef(-2.385f, 2.38f, 0.0f);
		glTranslatef(-2.62f, 2.62f, 0.0f);  //spriteSheet draw

		GLfloat quad[] = { width * scale, height * scale, width * scale, -height * scale, -width * scale, -height * scale, -width * scale, height * scale };

		glVertexPointer(2, GL_FLOAT, 0, quad);
		glEnableClientState(GL_VERTEX_ARRAY);

		GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };

		glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawArrays(GL_QUADS, 0, 4);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}
}



void SpriteSheet::drawf(float scale, float x, float y, float u, float v, float width, float height) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	GLfloat quad[] = { -width * scale, height * scale, -width * scale, -height * scale, width * scale, -height * scale, width * scale, height * scale };

	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };

	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
	
}

GLuint SpriteSheet::getID() {
	return textureID;
}

GLuint SpriteSheet::LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	return textureID;
}