#pragma once
#include "Entity.h"
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <time.h>
#include "Bullet.h"
#include <SDL_mixer.h>
#include "ParticleEmitter.h"
#include "Coord.h"
#include "Node.h"
#include <queue>
#include "ToolBox.h"

#include "Tilemap.h"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#define TILE_OFFSET 22.8125f
#define BULLET_SPEED 0.1f
//#define SPRITE_COUNT_X 8
//#define SPRITE_COUNT_Y 8
//#define TILE_SIZE 0.09f


class Application {
public:
	Application();
	~Application();

	void Init();
	bool UpdateAndRender();
	void Render();
	void Update(float elapsed);

	void RenderGameLevel();
	void UpdateGameLevel();

	void drawLevel();

	bool checkCollision(Entity* e1, Entity* e2);
	void collisionResp(Entity* e1, Entity* e2);

	bool isSolid(Entity* entity);

	//maybe remove
	void glutMouseFunc(void(Application::*func)(int button, int state, int x, int y));

	void updateSelect();
	void renderSelected();//run this after renderGameLevel#
	void unSelectUnit();

	int convertX(float x); //converts x coord to grid x
	int convertY(float y); //converts y coord to grid y

	Coord findCoord(int x, int y);
	void generateNodes();

	void findPath(Node* node, int currWeight, int maxWeight); //dont make it loop more than 8 times, after 8 it will go to 250k then 1mil loops
	//void moveUnit(Node* startNode, Node* endNode);
	void moveUnit(Node* node, Node* endNode, int currWeight, int maxWeight);
	void theMove(Node* node);
	void setVisited();
	void resetVisited();
	int CoordToNodeIdx(float x, float y);
	Entity* entOnNode(Node* node);

	void updateOccupied();
	void setNodesNull();

	vector<Node*> findNodePath(Node* start, Node* end);

	void changeTurn();
	void clearNodeVecs(); //clears possibleNodes, moveNodes, and reset the selections

	void findAttack();
	void renderAttack();
	void doAttack(Entity* e1, Entity* e2, int stage);

	void UpdateBattleLevel();
	void updateWin();
	void drawLevelSelect();
	void updateLevelSelect();
	void gameOver(bool p1);

	//textbox code
	void toTextBox(string stuff);
	void writeStatus(); 
	void drawInfoBox();
	void drawInsideBox();
	void drawInsideLevelSelect();

	//battle code
	void fixedUpdate();
	void drawBattleLevel();
	void renderBattleField();
	void buildBattleField(Entity* e1, Entity* e2, int terrain);
	void endBattle();

	//collision code

	bool isSolid(unsigned char tile);
	void levelCollisionX(Entity* entity);
	void levelCollisionY(Entity* entity);
	float checkforCollisionX(float x, float y);
	float checkforCollisionY(float x, float y);
	void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);

	void shootBullet(int idx);
	bool doBulletCollision(Bullet* bullet);
	void shootBullet2(int idx);
	bool doBulletCollision2(Bullet* bullet);

	//particles
	void boom(float x, float y);
private:
	bool done;
	int state;
	int gameState; //this is the gameLevel we are on doesnt change
	bool selectToggle;
	int cdCount;
	int cdCount2;

	unsigned char** levelData;
	unsigned char** levelData2;
	unsigned char** levelData3;
	unsigned char** battleData;

	int mapWidth;
	int mapHeight;

	float lastFrameTicks;

	float lerp(float from, float to, float time);
	float round(float num);

	bool p1turn;

	//battle
	Entity* p1Battle;
	Entity* p2Battle;
	float p1x;
	float p2x;
	float p1y;
	float p2y;

	//to be displayed in textBox
	string line1;
	string line2;
	string status1;
	string sName;
	string status3;
	string sHp;
	string status4;
	string atk;
	string status5;
	string atkSpd;

	//selected Unit
	float selectedUnitX;
	float selectedUnitY;
	int selectedIndex;

	std::vector<Coord> selected; //vector of red tiles make sure to clear with state change
	std::vector<ParticleEmitter*> particles;
	std::vector<Node*> nodes;
	std::vector<Node*> possibleNodes;
	std::vector<Node*> moveNodes;
	std::vector<Node*> attack;

	//entities are our level objects if any
	//player1 and 2 is self explanatory
	std::vector<Entity*> entities; //remove if we never use
	std::vector<Entity*> player1;
	std::vector<Entity*> player2;
	std::vector<Bullet*> bullets;
	std::vector<Bullet*> bullets2;

	Tilemap* map1;
	Tilemap* map2;
	Tilemap* map3;

	Mix_Chunk* battle;
	Mix_Music* inBattle;
	Mix_Chunk* jump;
	Mix_Music* credit;
	Mix_Music* bgm;
	Mix_Chunk* pew;

	SDL_Event event;

	SpriteSheet* sheet;
	GLuint fontText;
	SDL_Window* displayWindow;
};