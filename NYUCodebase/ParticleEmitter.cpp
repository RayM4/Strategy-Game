#pragma once
#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(unsigned int particleCount, float x, float y, float gx, float gy, float vx, float vy, float dx, float dy) {
	particles.resize(particleCount);

	position = Vector(x, y, 0.0f);
	velocity = Vector(vx, vy, 0.0f);
	velocityDeviation = Vector(dx, dy, 0.0f);
	srand(time(NULL));

	for (int i = 0; i < particleCount; i++) {
		particles[i].lifetime = (rand() % 50) * 0.1f;
		particles[i].position = position;
		particles[i].velocity = velocity;
	}

	gravity = Vector(gx, gy, 0.0f);
	maxLifetime = 5.0f;
}

ParticleEmitter::ParticleEmitter() {

}

ParticleEmitter::~ParticleEmitter() { 

}

void ParticleEmitter::Update(float elapsed) {
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i].lifetime > maxLifetime) { //checking if particle is expired
			particles[i].lifetime = 0.0f;
			particles[i].velocity.x = velocity.x - (velocityDeviation.x * 0.5f) + (velocityDeviation.x * (((float)rand() / (float)RAND_MAX)));
			particles[i].velocity.y = velocity.y - (velocityDeviation.y * 0.5f) + (velocityDeviation.y * (((float)rand() / (float)RAND_MAX)));
			//particles[i] = particles[particles.size() - 1];
			//particles.pop_back();
			//i--;
		}
		else { //particle update
			particles[i].velocity.y += (gravity.y * elapsed);
			particles[i].position.x += (particles[i].velocity.x * elapsed);
			particles[i].position.y += (particles[i].velocity.y * elapsed);
			particles[i].lifetime += elapsed;
		}
	}
	timer += elapsed;
}

void ParticleEmitter::Render() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-2.62f, 2.62f, 0.0f);
	std::vector<float> particleVertices;
	for (int i = 0; i < particles.size(); i++) {
		particleVertices.push_back(particles[i].position.x);
		particleVertices.push_back(particles[i].position.y);
	}
	glVertexPointer(2, GL_FLOAT, 0, particleVertices.data());
	glEnableClientState(GL_VERTEX_ARRAY);

	std::vector<float> particleColors;
	for (int i = 0; i < particles.size(); i++) {
		float m = (particles[i].lifetime / maxLifetime);
		particleColors.push_back(lerp(startColor.r, endColor.r, m));
		particleColors.push_back(lerp(startColor.g, endColor.g, m));
		particleColors.push_back(lerp(startColor.b, endColor.b, m));
		particleColors.push_back(lerp(startColor.a, endColor.a, m));
	}

	glColorPointer(4, GL_FLOAT, 0, particleColors.data());
	glEnableClientState(GL_COLOR_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPointSize(5.0f);
	glDrawArrays(GL_POINTS, 0, particleVertices.size() * 0.5f);
	//glDrawArrays(GL_POINTS, 0, particleVertices.size() * 0.5f);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glPopMatrix();
}

void ParticleEmitter::Render2(const char *image_path) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, LoadTexture(image_path));

	std::vector<float> particleVertices;
	std::vector<float> particleUVs;
	std::vector<unsigned int> indices;

	for (int i = 0; i < particles.size(); i++) {
		float relativeLifetime = (particles[i].lifetime / maxLifetime);
		float size = lerp(startSize, endSize, relativeLifetime) + particles[i].sizeDeviation;

		unsigned int vertexOffset = particleVertices.size() / 2;

		particleVertices.insert(particleVertices.end(), {
			particles[i].position.x - size, particles[i].position.y + size,
			particles[i].position.x - size, particles[i].position.y - size,
			particles[i].position.x + size, particles[i].position.y - size,
			particles[i].position.x + size, particles[i].position.y + size
		});

		particleUVs.insert(particleUVs.end(), {
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f
		});

		indices.insert(indices.end(), { vertexOffset + 0, vertexOffset + 1, vertexOffset + 2, vertexOffset + 0, vertexOffset + 2, vertexOffset + 3 });
	}
	glTexCoordPointer(2, GL_FLOAT, 0, particleUVs.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
}

float ParticleEmitter::lerp(float from, float to, float time) {
	return (1.0 - time)*from + time*to;
}

void ParticleEmitter::setSColor(float r, float g, float b, float a) { startColor = Color(r, g, b, a); }
void ParticleEmitter::setEColor(float r, float g, float b, float a) { endColor = Color(r, g, b, a); }

float ParticleEmitter::getTimer() { return timer;  }

GLuint ParticleEmitter::LoadTexture(const char *image_path) {
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