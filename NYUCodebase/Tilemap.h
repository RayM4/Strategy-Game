#pragma once
#define TILE_SIZE 0.1f
#define SPRITE_X 8
#define SPRITE_Y 8

#include <string>
#include <vector>

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "SpriteSheet.h"
#include "Entity.h"
using namespace std;


class Tilemap {
public:
	Tilemap(string name, SpriteSheet* sheet);

	string getName();
	void setName(string name);

	void genLevel(vector<Entity*>* p1, vector<Entity*>* p2);
	//battle
	void genLevel(Entity* p1, Entity* p2);
	
	int getMapWidth();
	int getMapHeight();

	void setMapWidth(int w);
	void setMapHeight(int h);

	unsigned char** getMapData();
private:
	bool readHeader(std::ifstream &stream);
	bool readLayerData(std::ifstream &stream);
	bool readEntityData(std::ifstream &stream, vector<Entity*>* p1, vector<Entity*>* p2);
	void placeEntity(string type, float x, float y, vector<Entity*>* p1, vector<Entity*>* p2);

	//battle
	bool readEntityData(std::ifstream &stream, Entity* p1, Entity* p2);
	void placeEntity(string type, float x, float y, Entity* p1, Entity* p2);

	float getV(int idx);
	float getU(int idx);

	unsigned char** levelData;

	string filename;
	SpriteSheet* sheet;

	int mapWidth;
	int mapHeight;
};