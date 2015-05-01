#include "Tilemap.h"


Tilemap::Tilemap(string name, SpriteSheet* sheet) :
	filename(name), sheet(sheet) {};

string Tilemap::getName() { return filename; }
void Tilemap::setName(string name) { filename = name; }

void Tilemap::genLevel(vector<Entity*>* p1, vector<Entity*>* p2) {
	ifstream infile(filename);
	string line;
	bool flag = true;
	while (getline(infile, line) && flag == true) {
		if (line == "[header]") {
			if (!readHeader(infile)) {
				flag = false;
			}
		}
		else if (line == "[layer]") {
			readLayerData(infile);
		}
		else if (line == "[Object Layer]") {
			readEntityData(infile, p1, p2);
		}
	}
}

void Tilemap::genLevel(Entity* p1, Entity* p2) {
	ifstream infile(filename);
	string line;
	bool flag = true;
	while (getline(infile, line) && flag == true) {
		if (line == "[header]") {
			if (!readHeader(infile)) {
				flag = false;
			}
		}
		else if (line == "[layer]") {
			readLayerData(infile);
		}
		else if (line == "[Object Layer]") {
			readEntityData(infile, p1, p2);
		}
	}
}

bool Tilemap::readHeader(std::ifstream &stream) {
	string line;
	mapWidth = -1;
	mapHeight = -1;
	while (getline(stream, line)) {

		if (line == "") { break; }

		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);

		if (key == "width") {
			mapWidth = atoi(value.c_str());
		}
		else if (key == "height"){
			mapHeight = atoi(value.c_str());
		}
	}

	if (mapWidth == -1 || mapHeight == -1) {
		return false;
	}
	else {
		levelData = new unsigned char*[mapHeight];
		for (int i = 0; i < mapHeight; ++i) {
			levelData[i] = new unsigned char[mapWidth];
		}
		return true;
	}
}

bool Tilemap::readLayerData(std::ifstream &stream) {
	string line;
	while (getline(stream, line)) {

		if (line == "") { break; }

		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);

		if (key == "data") {

			for (int y = 0; y < mapHeight; y++) {

				getline(stream, line);
				istringstream lineStream(line);
				string tile;

				for (int x = 0; x < mapWidth; x++) {
					getline(lineStream, tile, ',');
					unsigned char val = (unsigned char)atoi(tile.c_str());
					if (val > 0) {
						levelData[y][x] = val - 1;
					}
					else {
						levelData[y][x] = 0;
					}
				}
			}
		}
	}
	return true;
}

bool Tilemap::readEntityData(std::ifstream &stream, vector<Entity*>* p1, vector<Entity*>* p2) {
	string line;
	string type;

	while (getline(stream, line)) {
		if (line == "") { break; }

		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);

		if (key == "type") {
			type = value;
		}
		else if (key == "location") {
			istringstream lineStream(value);
			string xPosition, yPosition;
			getline(lineStream, xPosition, ',');
			getline(lineStream, yPosition, ',');

			//float placeX = (float)(atoi(xPosition.c_str()) / (mapWidth * 1.0f));
			//float placeY = (float)(atoi(yPosition.c_str()) / (mapHeight * -1.0f));
			float placeX = (float)(atoi(xPosition.c_str()) * TILE_SIZE);
			float placeY = (float)(atoi(yPosition.c_str()) * -TILE_SIZE);
			placeEntity(type, placeX, placeY, p1, p2);
		}
	}
	return true;
}

bool Tilemap::readEntityData(std::ifstream &stream, Entity* p1, Entity* p2) {
	string line;
	string type;

	while (getline(stream, line)) {
		if (line == "") { break; }

		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);

		if (key == "type") {
			type = value;
		}
		else if (key == "location") {
			istringstream lineStream(value);
			string xPosition, yPosition;
			getline(lineStream, xPosition, ',');
			getline(lineStream, yPosition, ',');

			//float placeX = (float)(atoi(xPosition.c_str()) / (mapWidth * 1.0f));
			//float placeY = (float)(atoi(yPosition.c_str()) / (mapHeight * -1.0f));
			float placeX = (float)(atoi(xPosition.c_str()) * TILE_SIZE);
			float placeY = (float)(atoi(yPosition.c_str()) * -TILE_SIZE);
			placeEntity(type, placeX, placeY, p1, p2);
		}
	}
	return true;
}

//the index of the sprite is the num in front of u and v
void Tilemap::placeEntity(string type, float x, float y, vector<Entity*>* p1, vector<Entity*>* p2) {
	if (type == "p1_u1") {
		int idx = 13;
		float u = (float)((idx % SPRITE_X) / (SPRITE_X * 1.0f));
		float v = getV(idx);
		float w = (1.0f / (SPRITE_X * 1.0f));
		float h = (1.0f / (SPRITE_Y * 1.0f));
		Entity* player = new Entity(x, y, u, v, w, h, SCALE, sheet);
		player->unitIdx = idx;
		player->movement = 50;
		player->hp = 10;
		player->atk = 1;
		p1->push_back(player);
	}
	else if (type == "p2_u1") {
		int idx = 14;
		float u = (float)((idx % SPRITE_X) / (SPRITE_X * 1.0f));
		float v = getV(idx);
		float w = 1.0f / (SPRITE_X * 1.0f);
		float h = 1.0f / (SPRITE_Y * 1.0f);
		Entity* player = new Entity(x, y, u, v, w, h, SCALE, sheet);
		player->unitIdx = idx;
		player->movement = 50;
		player->hp = 10;
		player->atk = 1;
		p2->push_back(player);
	}
	else if (type == "p1_u2") {
		int idx = 15;
		float u = (float)((idx % SPRITE_X) / (SPRITE_X * 1.0f));
		float v = getV(idx);
		float w = (1.0f / (SPRITE_X * 1.0f));
		float h = (1.0f / (SPRITE_Y * 1.0f));
		Entity* player = new Entity(x, y, u, v, w, h, SCALE, sheet);
		player->unitIdx = idx;
		player->movement = 50;
		player->hp = 10;
		player->atk = 3;
		p1->push_back(player);
	}
	else if (type == "p2_u2") {
		int idx = 23;
		float u = (float)((idx % SPRITE_X) / (SPRITE_X * 1.0f));
		float v = getV(idx);
		float w = (1.0f / (SPRITE_X * 1.0f));
		float h = (1.0f / (SPRITE_Y * 1.0f));
		Entity* player = new Entity(x, y, u, v, w, h, SCALE, sheet);
		player->unitIdx = idx;
		player->movement = 50;
		player->hp = 10;
		player->atk = 3;
		p2->push_back(player);
	}
	else if (type == "p1_u3") {
		int idx = 21;
		float u = (float)((idx % SPRITE_X) / (SPRITE_X * 1.0f));
		float v = getV(idx);
		float w = (1.0f / (SPRITE_X * 1.0f));
		float h = (1.0f / (SPRITE_Y * 1.0f));
		Entity* player = new Entity(x, y, u, v, w, h, SCALE, sheet);
		player->unitIdx = idx;
		player->movement = 100;
		player->hp = 30;
		player->atk = 5;
		p1->push_back(player);
	}
	else if (type == "p2_u3") {
		int idx = 22;
		float u = (float)((idx % SPRITE_X) / (SPRITE_X * 1.0f));
		float v = getV(idx);
		float w = (1.0f / (SPRITE_X * 1.0f));
		float h = (1.0f / (SPRITE_Y * 1.0f));
		Entity* player = new Entity(x, y, u, v, w, h, SCALE, sheet);
		player->unitIdx = idx;
		player->movement = 100;
		player->hp = 30;
		player->atk = 5;
		p2->push_back(player);
	}
}

void Tilemap::placeEntity(string type, float x, float y, Entity* p1, Entity* p2) {
	if (type == "p1_u1") {
		p1->setX(x);
		p1->setY(y);
	}
	else if (type == "p2_u1") {
		p2->setX(x); 
		p2->setY(y);
	}
}

int Tilemap::getMapHeight() { return mapHeight; }
int Tilemap::getMapWidth() { return mapWidth; }
void Tilemap::setMapHeight(int h) { mapHeight = h; }
void Tilemap::setMapWidth(int w) { mapWidth = w; }
unsigned char** Tilemap::getMapData() { return levelData; }

float Tilemap::getV(int idx) {
	return ((idx / 8) / 8.0f);
}