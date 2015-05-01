#include "Application.h"
using namespace std;

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
#define PLAYER 0
#define PLAYER_ACCEL 1.0f
#define SCALE 1.0f
#define NUM_ROCKS 10
#define SHAKE 50
float timeLeftOver = 0.0f;

const Uint8 *keys = SDL_GetKeyboardState(NULL);
enum GameState { STATE_TITLE, STATE_MAIN_MENU, STATE_UNIT_SELECT, STATE_GAME_LEVEL_ONE, 
	STATE_GAME_LEVEL_TWO, STATE_GAME_LEVEL_THREE, STATE_BATTLE, STATE_GAME_OVER }; //status is optional for now

Application::Application() {
	battle = Mix_LoadWAV("mortalKombat.wav");
	inBattle = Mix_LoadMUS("mortalKombatLoop.ogg");
	jump = Mix_LoadWAV("jumpSound.wav");
	credit = Mix_LoadMUS("EpicSaxGuyLoop.ogg");
	bgm = Mix_LoadMUS("fateExtra.ogg");
	pew = Mix_LoadWAV("PewPew.wav");
	Init();
	lastFrameTicks = 0.0f;
	done = false;
	p1turn = true;
	selectToggle = false;
	line1 = "";
	line2 = "Welcome to the Game";
	status1 = "Name";
	sName = "";
	status3 = "HP";
	sHp = "";
	status4 = "Atk";
	atk = "";
	status5 = "ASpd";
	atkSpd = "";
	cdCount = 0;
	cdCount2 = 0;

	//counter = 2000;
	//move = false;
}

Application::~Application() {
	//Mix_FreeMusic(music);
	IMG_Quit();
	SDL_Quit();
}

void Application::Init() {
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	//displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 800, SDL_WINDOW_OPENGL);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 700, SDL_WINDOW_OPENGL);  
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	glViewport(0, 0, 800, 700);
	glMatrixMode(GL_MODELVIEW);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	glOrtho(-1.75, 1.75, -1.75, 1.75, -1.0, 1.0);  
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	sheet = new SpriteSheet("test.png");
	fontText = LoadTexture("pixel_font.png", GL_RGBA);

	//battle levels
	map1 = new Tilemap("platformerTile.txt", sheet);
	//map2 = new Tilemap("gravelMap.txt", sheet);
	//map3 = new Tilemap("dirtMap.txt", sheet);

	battleData = new unsigned char*[mapHeight];
	for (int i = 0; i < mapHeight; ++i) {
		battleData[i] = new unsigned char[mapWidth];
	}

	//generateNodes();

	gameState = STATE_GAME_LEVEL_ONE;
	//state = gameState;
	state = STATE_MAIN_MENU;

	//Mix_PlayMusic(bgm, -1);
}

bool Application::UpdateAndRender() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	float fixedElapsed = elapsed + timeLeftOver;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}
	while (fixedElapsed >= FIXED_TIMESTEP) {
		fixedElapsed -= FIXED_TIMESTEP;

		if (state == STATE_BATTLE) {
			p1Battle->fixedUpdate();
			p2Battle->fixedUpdate();
			levelCollisionY(p1Battle);
			levelCollisionY(p2Battle);
			levelCollisionX(p1Battle);
			levelCollisionX(p2Battle);

			for (int i = 0; i < bullets.size(); i++) {
				bullets[i]->update(fixedElapsed);
			}

			for (int i = 0; i < bullets2.size(); i++) {
				bullets2[i]->update(fixedElapsed);
			}

			for (int i = 0; i < bullets.size(); i++) {
				bool flag = doBulletCollision(bullets[i]);
				if (flag) {
					bullets[i] = bullets[bullets.size() - 1];
					bullets.pop_back();
					i -= 1;
				}
			}

			for (int i = 0; i < bullets2.size(); i++) {
				bool flag = doBulletCollision2(bullets2[i]);
				if (flag) {
					bullets2[i] = bullets2[bullets2.size() - 1];
					bullets2.pop_back();
					i -= 1;
				}
			}

			cdCount++;
			cdCount2++;

			//update particles
			//for (int i = 0; i < particles.size(); i++) {
			//	if (particles[i]->getTimer() > 1000) {
			//		delete particles[i];
			//		particles[i] = nullptr;
			//		particles.erase(particles.begin() + i);
			//		i--;
			//	}
			//	else {
			//		particles[i]->Update(elapsed);
			//	}
			//}
		}
		
	}

	updateOccupied();
	
	Update(elapsed);
	Render();

	return done;
}

void Application::Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	switch (state) {
	case STATE_TITLE:
		break;
	case STATE_MAIN_MENU:
		drawLevelSelect();
		drawInsideLevelSelect();
		break;
	case STATE_UNIT_SELECT:
		if (gameState == STATE_GAME_LEVEL_ONE ||
			gameState == STATE_GAME_LEVEL_TWO ||
			gameState == STATE_GAME_LEVEL_THREE) { //Add the other 2 when we make them
			RenderGameLevel();
		}
		renderSelected();
		renderAttack();
		break;
	case STATE_BATTLE:
		renderBattleField();
		break;
	case STATE_GAME_LEVEL_ONE:
		RenderGameLevel();
		break;
	case STATE_GAME_LEVEL_TWO:
		RenderGameLevel();
		break;
	case STATE_GAME_LEVEL_THREE:
		RenderGameLevel();
		break;
	case STATE_GAME_OVER:
		gameOver(p1turn);
		break;
	}
	

	SDL_GL_SwapWindow(displayWindow);
}


void Application::RenderGameLevel() {
	//Mix_PlayMusic(music, -1);

	drawLevel();
	drawInfoBox();
	
	for (int i = 0; i < player1.size(); i++) {
		player1[i]->draw();
	}
	for (int i = 0; i < player2.size(); i++) {
		player2[i]->draw();
	}

	/*for (int i = 0; i < particles.size(); i++) {
		particles[i]->Render();
	}*/

	drawInsideBox();
}

void Application::UpdateGameLevel() {
	updateWin();
}

void Application::Update(float elapsed) {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (state == STATE_GAME_LEVEL_ONE ||
				state == STATE_GAME_LEVEL_TWO ||
				state == STATE_GAME_LEVEL_THREE) {
				
				float x = event.button.x;
				float y = event.button.y;
				Coord mouse = findCoord(x, y);
				
				if (p1turn && event.button.button == 1 && state != STATE_UNIT_SELECT) { //only player 1 turn and left click
					for (int i = 0; i < player1.size(); i++) {
						//if (mouse.x == (int)(player1[i]->getX() * 10.0f) && mouse.y == (int)(player1[i]->getY() * 10.0f)) {
						if (mouse.x == convertX(player1[i]->getX()) && mouse.y == convertY(player1[i]->getY()) && (!player1[i]->moved)) {
							selectedUnitX = player1[i]->getX(); 
							selectedUnitY = player1[i]->getY(); 
							state = STATE_UNIT_SELECT;
							selectToggle = true;
							selectedIndex = i;
							writeStatus();
						}
					}
				}
				else if (!p1turn && event.button.button == 1 && state != STATE_UNIT_SELECT) { //only player 2 turn

					for (int i = 0; i < player2.size(); i++) {
						if (mouse.x == convertX(player2[i]->getX()) && mouse.y == convertY(player2[i]->getY()) && (!player2[i]->moved)) {
							selectedUnitX = player2[i]->getX();
							selectedUnitY = player2[i]->getY();
							state = STATE_UNIT_SELECT;
							selectToggle = true;
							selectedIndex = i;
							writeStatus();
						}
					}
				}

				if (event.button.button == 1) {

					if (x > 732 && x < 800 && y > 640 && y < 700) {
						changeTurn();
					}
				}
			}

			else if (state == STATE_UNIT_SELECT && event.button.button == 3) { //unselect unit, gotta add click other units too
				unSelectUnit();
				state = gameState;
				//selectToggle = false;
			}
			else if (state == STATE_UNIT_SELECT && event.button.button == 1) {
				float x = event.button.x;
				float y = event.button.y;
				Coord mouse = findCoord(x, y);
				mouse.y = mouse.y * -1;
				for (int i = 0; i < possibleNodes.size(); i++) {
					if (mouse.x == possibleNodes[i]->x &&
						mouse.y == possibleNodes[i]->y) {
						Node* dNode = nodes[CoordToNodeIdx(mouse.x, mouse.y)];
						theMove(dNode);
						i = 1000; //ends for loop just in case
						//counter = 0; //these 2 lines are for mvoement animation
						//move = true;
					}
				}
				state = gameState;
				for (int i = 0; i < attack.size(); i++) {
					if (mouse.x == attack[i]->x &&
						mouse.y == attack[i]->y) {
						if (p1turn) {
							doAttack(player1[selectedIndex], entOnNode(attack[i]), 1);
						}
						else {
							doAttack(entOnNode(attack[i]), player2[selectedIndex], 1);
						}
						
					}
				}

				unSelectUnit();
			}

			if (state == STATE_MAIN_MENU) {
				float x = event.button.x;
				float y = event.button.y;

				if (x > 103 && x < 655 && y > 55 && y < 160 ) {
					gameState = STATE_GAME_LEVEL_ONE;
					state = gameState;
					Tilemap* map = new Tilemap("gameMap1.txt", sheet);
					map->genLevel(&player1, &player2);

					mapWidth = map->getMapWidth();
					mapHeight = map->getMapHeight();

					levelData = new unsigned char*[mapHeight];
					for (int i = 0; i < mapHeight; ++i) {
						levelData[i] = new unsigned char[mapWidth];
					}

					levelData = map->getMapData();

					generateNodes();


				}
				else if (x > 103 && x < 655 && y > 260 && y < 360) {
					gameState = STATE_GAME_LEVEL_TWO;
					state = gameState;

					Tilemap* map = new Tilemap("gameMap2.txt", sheet);
					map->genLevel(&player1, &player2);

					mapWidth = map->getMapWidth();
					mapHeight = map->getMapHeight();

					levelData = new unsigned char*[mapHeight];
					for (int i = 0; i < mapHeight; ++i) {
						levelData[i] = new unsigned char[mapWidth];
					}

					levelData = map->getMapData();

					generateNodes();

					
				}
				else if (x > 103 && x < 655 && y > 460 && y < 560) {
					gameState = STATE_GAME_LEVEL_THREE;
					state = gameState;

					Tilemap* map = new Tilemap("gameMap3.txt", sheet);
					map->genLevel(&player1, &player2);

					mapWidth = map->getMapWidth();
					mapHeight = map->getMapHeight();

					levelData = new unsigned char*[mapHeight];
					for (int i = 0; i < mapHeight; ++i) {
						levelData[i] = new unsigned char[mapWidth];
					}

					levelData = map->getMapData();

					generateNodes();

				}
			}
		}

	}


	switch (state) {
	case STATE_TITLE:
		break;
	case STATE_MAIN_MENU:
		updateLevelSelect();
		break;
	case STATE_UNIT_SELECT:
		updateSelect();
		if (gameState == STATE_GAME_LEVEL_ONE ||
			gameState == STATE_GAME_LEVEL_TWO || 
			gameState == STATE_GAME_LEVEL_THREE ) { //Add the other 2 when we make them
			UpdateGameLevel();
		}
		break;
	case STATE_BATTLE:
		UpdateBattleLevel();
		break;
	case STATE_GAME_LEVEL_ONE:
		UpdateGameLevel();
		break;
	case STATE_GAME_LEVEL_TWO:
		UpdateGameLevel();
		break;
	case STATE_GAME_LEVEL_THREE:
		UpdateGameLevel();
		break;
	case STATE_GAME_OVER:
		
		break;
	}

	
}

void Application::drawLevel() {
	vector<float> vertexData;
	vector<float> texCoordData;
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sheet->getID());
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//glTranslatef((-TILE_SIZE * mapWidth * 0.5f), (TILE_SIZE * mapHeight * 0.5f), 0.0f);
	glTranslatef((-TILE_SIZE * mapWidth * 0.5475f), (TILE_SIZE * mapHeight * 0.5475f), 0.0f);

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			if (levelData[y][x] != -1) {
				float u = (float)(((int)levelData[y][x]) % SPRITE_X) / (float)SPRITE_X;
				float v = (float)(((int)levelData[y][x]) / SPRITE_X) / (float)SPRITE_Y;
				float spriteWidth = 1.0f / (float)SPRITE_X;
				float spriteHeight = 1.0f / (float)SPRITE_Y;
				vertexData.insert(vertexData.end(), {
					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
				});
				texCoordData.insert(texCoordData.end(), { u, v,
					u, v + (spriteHeight),
					u + spriteWidth, v + (spriteHeight),
					u + spriteWidth, v
				});
			}
		}
	}

	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDrawArrays(GL_QUADS, 0, vertexData.size() / 2);

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopMatrix();

}

float Application::lerp(float from, float to, float time) {
	return (1.0 - time)*from + time*to;
}

void Application::updateSelect() { 
	if (selectToggle) {
		float gridX = (float)(convertX(selectedUnitX) * 1.0f);
		float gridY = (float)(convertY(selectedUnitY) * -1.0f);

		Node* pNode = nodes[CoordToNodeIdx(gridX, gridY)];
		int weight = 0 - pNode->cost;

		if (p1turn) {
			findPath(pNode, weight, player1[selectedIndex]->movement);
		}
		else {
			findPath(pNode, weight, player2[selectedIndex]->movement);
		}
		//findPath(pNode, weight, 50);

		findAttack();

		selectToggle = false;
	}
}

void Application::renderSelected() {
	float u = (float)((8 % SPRITE_X) / (SPRITE_X * 1.0f));
	float v = ((8 / 8) / 8.0f);
	float w = 1.0f / (SPRITE_X * 1.0f);
	float h = 1.0f / (SPRITE_Y * 1.0f);

	vector<float> vertexData;
	vector<float> texCoordData;
	vector<float> colors;

	for (int i = 0; i < possibleNodes.size(); i++) {
		vertexData.insert(vertexData.end(), {
			TILE_SIZE * possibleNodes[i]->x, -TILE_SIZE * possibleNodes[i]->y,
			TILE_SIZE * possibleNodes[i]->x, (-TILE_SIZE * possibleNodes[i]->y) - TILE_SIZE,
			(TILE_SIZE * possibleNodes[i]->x) + TILE_SIZE, (-TILE_SIZE * possibleNodes[i]->y) - TILE_SIZE,
			(TILE_SIZE * possibleNodes[i]->x) + TILE_SIZE, -TILE_SIZE * possibleNodes[i]->y
		});
		texCoordData.insert(texCoordData.end(), { u, v,
			u, v + h,
			u + w, v + h,
			u + w, v
		});
		colors.insert(colors.end(), {
			1.0f, 0.0f, 0.0f, 0.1f,
			1.0f, 0.0f, 0.0f, 0.1f,
			1.0f, 0.0f, 0.0f, 0.1f,
			1.0f, 0.0f, 0.0f, 0.1f
		});
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sheet->getID());
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//glTranslatef((-TILE_SIZE * mapWidth * 0.5f), (TILE_SIZE * mapHeight * 0.5f), 0.0f);
	glTranslatef((-TILE_SIZE * mapWidth * 0.5475f), (TILE_SIZE * mapHeight * 0.5475f), 0.0f);

	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glColorPointer(4, GL_FLOAT, 0, colors.data());
	glEnableClientState(GL_COLOR_ARRAY);

	glDrawArrays(GL_QUADS, 0, vertexData.size() / 2);

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glPopMatrix();
}

void Application::renderAttack() {
	float u = (float)((18 % SPRITE_X) / (SPRITE_X * 1.0f));
	float v = ((18 / 8) / 8.0f);
	float w = 1.0f / (SPRITE_X * 1.0f);
	float h = 1.0f / (SPRITE_Y * 1.0f);

	vector<float> vertexData;
	vector<float> texCoordData;
	vector<float> colors;

	for (int i = 0; i < attack.size(); i++) {
		vertexData.insert(vertexData.end(), {
			TILE_SIZE * attack[i]->x, -TILE_SIZE * attack[i]->y,
			TILE_SIZE * attack[i]->x, (-TILE_SIZE * attack[i]->y) - TILE_SIZE,
			(TILE_SIZE * attack[i]->x) + TILE_SIZE, (-TILE_SIZE * attack[i]->y) - TILE_SIZE,
			(TILE_SIZE * attack[i]->x) + TILE_SIZE, -TILE_SIZE * attack[i]->y
		});
		texCoordData.insert(texCoordData.end(), { u, v,
			u, v + h,
			u + w, v + h,
			u + w, v
		});
		colors.insert(colors.end(), {
			1.0f, 1.0f, 1.0f, 0.1f,
			1.0f, 1.0f, 1.0f, 0.1f,
			1.0f, 1.0f, 1.0f, 0.1f,
			1.0f, 1.0f, 1.0f, 0.1f
		});
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sheet->getID());
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//glTranslatef((-TILE_SIZE * mapWidth * 0.5f), (TILE_SIZE * mapHeight * 0.5f), 0.0f);
	glTranslatef((-TILE_SIZE * mapWidth * 0.5475f), (TILE_SIZE * mapHeight * 0.5475f), 0.0f);

	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glColorPointer(4, GL_FLOAT, 0, colors.data());
	glEnableClientState(GL_COLOR_ARRAY);

	glDrawArrays(GL_QUADS, 0, vertexData.size() / 2);

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glPopMatrix();
}

int Application::convertX(float x) {

	int x1 = (int)(round(x * 10));
	return x1;
}

int Application::convertY(float y) {
	int y1 = (int)(round(y * 10));
	return y1;
}

float Application::round(float num) {
	return floor(num + 0.5);
}

Coord Application::findCoord(int x, int y){
	// 730 = pixel length of map, 730/32 = 22.8125f 
	// 640 = pixel height of map, 640/32 = 20.0f
	float xOver = x / 22.8125f;
	float yOver = y / 20.0f;
	int xCoord = (x - (22.8125f * (xOver - (int)(xOver)))) / 22.8125f;
	int yCoord = -((y - (20.0f * (yOver - (int)(yOver)))) / 20.0f);
	Coord gridCoords;
	gridCoords.x = xCoord;
	gridCoords.y = yCoord;

	return gridCoords;
}

void Application::generateNodes() {
	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			Node *temp = new Node(x, y);
			temp->visited = false;
			temp->cameFrom = nullptr;
			temp->top = nullptr;
			temp->bot = nullptr;
			temp->left = nullptr;
			temp->right = nullptr;
			temp->occupied = false;

			if (levelData[y][x] == '\x0') { //grass
				temp->cost = 10;
			}
			else if (levelData[y][x] == '\xb') { //water
				temp->cost = 200;
			}
			else if (levelData[y][x] == '\x2') {
				temp->cost = 15; //dirt
			}
			else if (levelData[y][x] == '\x1b') {
				temp->cost = 20; // "sand"
			}
			else {
				temp->cost = 200; //just in case
			}

			temp->curCost = temp->cost;
			nodes.push_back(temp);
		}
	}

	for (int i = 0; i < nodes.size(); i++) {
		int top = nodes[i]->y - 1;
		int left = nodes[i]->x - 1;
		int right = nodes[i]->x + 1;
		int bot = nodes[i]->y + 1;

		for (int j = 0; j < nodes.size(); j++) {
			if (top == nodes[j]->y && nodes[i]->x == nodes[j]->x) {
				nodes[i]->top = nodes[j];
			}
			else if (left == nodes[j]->x && nodes[i]->y == nodes[j]->y) {
				nodes[i]->left = nodes[j];
			}
			else if (right == nodes[j]->x && nodes[i]->y == nodes[j]->y) {
				nodes[i]->right = nodes[j];
			}
			else if (bot == nodes[j]->y && nodes[i]->x == nodes[j]->x) {
				nodes[i]->bot = nodes[j];
			}
		}
	}
	
}

void Application::findPath(Node* node, int currWeight, int maxWeight){  
	if (currWeight + node->cost <= maxWeight) {
		currWeight += node->cost;
	}
	else
		return;
	if (node->visited != true && (!node->occupied)) {
		node->visited = true;
		possibleNodes.push_back(node);
	}
	if (node->left != nullptr)
		findPath(node->left, currWeight, maxWeight);
	if (node->right != nullptr)
		findPath(node->right, currWeight, maxWeight);
	if (node->top != nullptr)
		findPath(node->top, currWeight, maxWeight);
	if (node->bot != nullptr)
		findPath(node->bot, currWeight, maxWeight);
}


vector<Node*> Application::findNodePath(Node* start, Node* end) {
	//int diffX = start->x - end->x;
	//int diffY = start->y - end->y;

	vector<Node *> path;
	//for (int i = 1; i < abs(diffX) + 1; i++) {
	//	Node* temp = nodes[CoordToNodeIdx(start->x - i, start->y)];
	//	path.push_back(temp);
	//}
	//
	//int newX = start->x;
	//if (diffX > 0) {
	//	newX = path[path.size() - 1]->x;
	//}
	//

	//for (int i = 1; i < abs(diffY) + 1; i++) {
	//	Node* temp = nodes[CoordToNodeIdx(newX, start->y - i)];
	//	path.push_back(temp);
	//}

	path.push_back(end);

	return path;
}

int Application::CoordToNodeIdx(float x, float y) {
	return (int)(((y + 1) * 32.0) - (32.0 - x));
}

void Application::updateOccupied() {
	for (int i = 0; i < player1.size(); i++) {
		float gridX = (float)(convertX(player1[i]->getX()) * 1.0f);
		float gridY = (float)(convertY(player1[i]->getY()) * -1.0f);
		Node* temp = nodes[CoordToNodeIdx(gridX, gridY)];
		temp->occupied = false;
		if (!entOnNode(temp)->isDead()) {
			temp->occupied = true;
		}
	}

	for (int i = 0; i < player2.size(); i++) {
		float gridX = (float)(convertX(player2[i]->getX()) * 1.0f);
		float gridY = (float)(convertY(player2[i]->getY()) * -1.0f);
		Node* temp = nodes[CoordToNodeIdx(gridX, gridY)];
		temp->occupied = false;
		if (!entOnNode(temp)->isDead()) {
			temp->occupied = true;
		}
	}
}

void Application::unSelectUnit() {
	selectedUnitX = 0.0f;
	selectedUnitY = 0.0f;
	int size = possibleNodes.size();
	for (int i = 0; i < size; i++) {
		possibleNodes[possibleNodes.size() - 1]->visited = false;
		possibleNodes.pop_back();
	}

	for (int i = 0; i < attack.size(); i++) {
		attack.pop_back();
	}

	sName = "";
	sHp = "";
	atk = "";
	atkSpd = "";
	selectToggle = false;
	//state = gameState;
}

void Application::setNodesNull() {
	for (int i = 0; i < nodes.size(); i++) {
		nodes[i]->cameFrom = nullptr;
	}
}

void Application::setVisited() {
	for (int i = 0; i < nodes.size(); i++) {
		nodes[i]->visited = true;
	}

	for (int i = 0; i < possibleNodes.size(); i++) {
		possibleNodes[i]->visited = false;
	}
}

void Application::resetVisited() {
	for (int i = 0; i < nodes.size(); i++) {
		nodes[i]->visited = false;
	}
}

void Application::theMove(Node* node) {
	float gridX = (float)(convertX(selectedUnitX) * 1.0f);
	float gridY = (float)(convertY(selectedUnitY) * -1.0f);
	
	Node* pNode = nodes[CoordToNodeIdx(gridX, gridY)];

	moveNodes = findNodePath(pNode, node);
	//need to remove this part if we do make the animation
	float tx = (moveNodes[moveNodes.size() - 1]->x) / 10.0f;
	float ty = (moveNodes[moveNodes.size() - 1]->y) / -10.0f;
	pNode->visited = false;
	pNode->occupied = false;
	if (p1turn) {
		player1[selectedIndex]->setX(tx);
		player1[selectedIndex]->setY(ty);
		player1[selectedIndex]->moved = true;
	}
	else {
		player2[selectedIndex]->setX(tx);
		player2[selectedIndex]->setY(ty);
		player2[selectedIndex]->moved = true;
	}
}

void Application::drawInfoBox(){   //new func in application  //runs after drawLevel()
	//Text
	GLfloat infoBox[] = { -0.35f, 0.0854f, -0.35f, -0.201f, 2.83f, -0.201f, 2.83f, 0.0854f };
	glVertexPointer(2, GL_FLOAT, 0, infoBox);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat boxColor[] = { 0.6f, 0.6f, 0.6f, 1.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.6f, 0.6f, 0.6f, 1.0f };
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-1.39, -1.54, 0.0);
	glColorPointer(4, GL_FLOAT, 0, boxColor);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_COLOR_ARRAY);
	glPopMatrix();
	
	//endturn
	GLfloat endBox[] = { 2.85f, 0.0854f, 2.85f, -0.201f, 3.5f, -0.201f, 3.5f, 0.0854f };
	glVertexPointer(2, GL_FLOAT, 0, endBox);
	glEnableClientState(GL_VERTEX_ARRAY);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-1.39, -1.54, 0.0);
	glColorPointer(4, GL_FLOAT, 0, boxColor);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_COLOR_ARRAY);
	glPopMatrix();


	//rightBox
	GLfloat unitInfoBox[] = { 2.85f, 3.5f, 2.85f, 0.1f, 3.5f, 0.1f, 3.5f, 3.5f };
	glVertexPointer(2, GL_FLOAT, 0, unitInfoBox);
	glEnableClientState(GL_VERTEX_ARRAY);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-1.39, -1.54, 0.0);
	glColorPointer(4, GL_FLOAT, 0, boxColor);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_COLOR_ARRAY);
	glPopMatrix();
}

void Application::changeTurn() {
	if (p1turn) {
		for (int i = 0; i < player1.size(); i++) {
			player1[i]->moved = false;
		}
		toTextBox("Player 2 Turn");
	}
	else {
		for (int i = 0; i < player2.size(); i++) {
			player2[i]->moved = false;
		}
		toTextBox("Player 1 Turn");
	}
	p1turn = !p1turn;
	clearNodeVecs();
}

void Application::clearNodeVecs() {
	selectedUnitX = 0.0f;
	selectedUnitY = 0.0f;
	selectToggle = false;
	int size = possibleNodes.size();
	for (int i = 0; i < size; i++) {
		possibleNodes[possibleNodes.size() - 1]->visited = false;
		possibleNodes.pop_back();
	}
	size = moveNodes.size();
	for (int i = 0; i < size; i++) {
		moveNodes[moveNodes.size() - 1]->visited = false;
		moveNodes.pop_back();
	}

}

void Application::findAttack() {
	float gridX = (float)(convertX(selectedUnitX) * 1.0f);
	float gridY = (float)(convertY(selectedUnitY) * -1.0f);

	Node* pNode = nodes[CoordToNodeIdx(gridX, gridY)];

	if (pNode->top->occupied) {
		attack.push_back(pNode->top);
	}
	if (pNode->bot->occupied) {
		attack.push_back(pNode->bot);
	}
	if (pNode->left->occupied) {
		attack.push_back(pNode->left);
	}
	if (pNode->right->occupied) {
		attack.push_back(pNode->right);
	}
	//corners
	if (pNode->top->left->occupied) {
		attack.push_back(pNode->top->left);
	}
	if (pNode->top->right->occupied) {
		attack.push_back(pNode->top->right);
	}
	if (pNode->bot->left->occupied) {
		attack.push_back(pNode->bot->left);
	}
	if (pNode->bot->right->occupied) {
		attack.push_back(pNode->bot->right);
	}
}


void Application::drawInsideBox() {
	//draw information in boxes
	drawText(fontText, "End", 0.05f, 0.02f, 1.0f, 0.1f, 0.1f, 1.0f, 1.5, -1.54);
	drawText(fontText, "Turn", 0.05f, 0.02f, 1.0f, 0.1f, 0.1f, 1.0f, 1.5, -1.64);

	//diagoue box
	drawText(fontText, line1, 0.05f, 0.02f, 0.6f, 1.0f, 1.0f, 1.0f, -1.7, -1.54);
	drawText(fontText, line2, 0.05f, 0.02f, 0.6f, 1.0f, 1.0f, 1.0f, -1.7, -1.64);

	//player turn indicator
	if (p1turn) {
		drawText(fontText, "P1", 0.05f, 0.02f, 0.0f, 0.0f, 0.0f, 1.0f, 1.5, 1.7);	
	}
	else {
		drawText(fontText, "P2", 0.05f, 0.02f, 0.0f, 0.0f, 0.0f, 1.0f, 1.5, 1.7);
	}
	drawText(fontText, "Turn", 0.05f, 0.02f, 0.0f, 0.0f, 0.0f, 1.0f, 1.5, 1.6);
	drawText(fontText, "____", 0.05f, 0.02f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5, 1.5);

	//status
	drawText(fontText, status1, 0.05f, 0.02f, 0.0f, 0.0f, 0.0f, 1.0f, 1.5, 1.4);
	drawText(fontText, sName, 0.05f, 0.02f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5, 1.3);
	drawText(fontText, status3, 0.05f, 0.02f, 0.0f, 0.0f, 0.0f, 1.0f, 1.5, 1.2);
	drawText(fontText, sHp, 0.05f, 0.02f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5, 1.1);
	drawText(fontText, status4, 0.05f, 0.02f, 0.0f, 0.0f, 0.0f, 1.0f, 1.5, 1.0);
	drawText(fontText, atk, 0.05f, 0.02f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5, 0.9);
	drawText(fontText, status5, 0.05f, 0.02f, 0.0f, 0.0f, 0.0f, 1.0f, 1.5, 0.8);
	drawText(fontText, atkSpd, 0.05f, 0.02f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5, 0.7);
}

void Application::toTextBox(string stuff) {
	line1 = line2;
	line2 = stuff;
}

void Application::writeStatus() {
	int idx;

	if (p1turn) {
		sHp = to_string(player1[selectedIndex]->hp);
		atk = to_string(player1[selectedIndex]->atk);
		idx = player1[selectedIndex]->unitIdx;
	}
	else {
		sHp = to_string(player2[selectedIndex]->hp);
		atk = to_string(player2[selectedIndex]->atk);
		idx = player2[selectedIndex]->unitIdx;
	}
	
	switch (idx) {
	case 13:
		sName = "rInf";
		atkSpd = "fast";
		break;
	case 14:
		sName = "bInf";
		atkSpd = "fast";
		break;
	case 15:
		sName = "rMeC";
		atkSpd = "med";
		break;
	case 23:
		sName = "bMeC";
		atkSpd = "med";
		break;
	case 21:
		sName = "rTnk";
		atkSpd = "slow";
		break;
	case 22:
		sName = "bTnk";
		atkSpd = "slow";
		break;

	default:
		sName = "";
		break;
	}

	
}

//collision

void Application::worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY)
{
	*gridX = (int)((worldX) / TILE_SIZE);
	*gridY = (int)((-worldY) / TILE_SIZE);
}


bool Application::isSolid(unsigned char tile)
{
	//Mark the tiles that are solid
	switch (tile){
	case 17:
	case 24:
	case 25:
		return true;
		break;
	default:
		return false;
		break;
	}
}


float Application::checkforCollisionY(float x, float y)
{
	int gridX, gridY;
	worldToTileCoordinates(x, y, &gridX, &gridY);
	if (gridX < 0 || gridX > 730 || gridY < 0 || gridY > 730)
		return 0.0f;

	if (isSolid(battleData[gridY][gridX]))
	{
		float yCoord = (gridY * TILE_SIZE) - (TILE_SIZE*0.0f);
		return -y - yCoord;
	}
	return 0.0;
}

float Application::checkforCollisionX(float x, float y)
{
	int gridX, gridY;
	worldToTileCoordinates(x, y, &gridX, &gridY);
	if (gridX < 0 || gridX > 730 || gridY < 0 || gridY > 730)
		return 0.0f;

	if (isSolid(battleData[gridY][gridX]))
	{
		float xCoord = (gridX * TILE_SIZE) - (TILE_SIZE*0.0f);
		return x - xCoord;
	}
	return 0.0;
}

void Application::levelCollisionY(Entity* entity)
{
	//check bottom
	float adjust = checkforCollisionY(entity->getX(), entity->getY() - entity->getH() + 0.025);
	if (adjust != 0.0f)
	{
		entity->setY(entity->getY() + adjust);
		entity->velocityY = 0.0f;
		entity->collidedBottom = true;
	}

	//check top
	adjust = checkforCollisionY(entity->getX(), entity->getY() + entity->getH() - 0.12);
	if (adjust != 0.0f)
	{
		entity->setY(entity->getY() + adjust - TILE_SIZE);
		entity->velocityY = 0.0f;
		entity->collidedTop = true;
	}
}

void Application::levelCollisionX(Entity* entity)
{
	//check left
	float adjust = checkforCollisionX(entity->getX() - entity->getW() * 0.1 + .005, entity->getY());
	if (adjust != 0.0f)
	{
		entity->setX(entity->getX() + adjust * TILE_SIZE);
		entity->velocityX = 0.0f;
		entity->collidedLeft = true;
	}

	//check right
	adjust = checkforCollisionX(entity->getX() + entity->getW() * 0.1f + .09, entity->getY());
	if (adjust != 0.0f)
	{
		entity->setX(entity->getX() + (adjust - TILE_SIZE) * TILE_SIZE);
		entity->velocityX = 0.0f;
		entity->collidedRight = true;
	}
}

//battle code

void Application::doAttack(Entity* e1, Entity* e2, int stage) {
	p1Battle = e1;
	p2Battle = e2;
	p1x = e1->getX();
	p1y = e1->getY();
	p2x = e2->getX();
	p2y = e2->getY();

	buildBattleField(e1, e2, stage);
	//drawBattleLevel();
	//renderBattleField();
	state = STATE_BATTLE;

	//Mix_PlayChannel(-1, battle, 0);
}

void Application::buildBattleField(Entity* e1, Entity* e2, int terrain){
	if (terrain == 1) {
		map1->genLevel(e1, e2);
		battleData = map1->getMapData();
	}
	else if (terrain == 2){
		map2->genLevel(e1, e2);
		battleData = map2->getMapData();
	}
	else if (terrain == 3){
		map3->genLevel(e1, e2);
		battleData = map3->getMapData();
	}
}

void Application::drawBattleLevel(){
	vector<float> vertexData;
	vector<float> texCoordData;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sheet->getID());
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef((-TILE_SIZE * mapWidth * 0.5475f), (TILE_SIZE * mapHeight * 0.5475f), 0.0f);

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			if (battleData[y][x] != -1) {
				float u = (float)(((int)battleData[y][x]) % SPRITE_X) / (float)SPRITE_X;
				float v = (float)(((int)battleData[y][x]) / SPRITE_X) / (float)SPRITE_Y;
				float spriteWidth = 1.0f / (float)SPRITE_X;
				float spriteHeight = 1.0f / (float)SPRITE_Y;
				vertexData.insert(vertexData.end(), {
					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
				});
				texCoordData.insert(texCoordData.end(), { u, v,
					u, v + (spriteHeight),
					u + spriteWidth, v + (spriteHeight),
					u + spriteWidth, v
				});
			}
		}
	}

	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDrawArrays(GL_QUADS, 0, vertexData.size() / 2);

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopMatrix();
}

void Application::renderBattleField(){
	drawBattleLevel();
	p1Battle->draw(p1Battle->flip);
	p2Battle->draw(p2Battle->flip);
	for (int i = 0; i < bullets.size(); i++) {
		bullets[i]->draw();
	}
	for (int i = 0; i < bullets2.size(); i++) {
		bullets2[i]->draw();
	}
	//for (int i = 0; i < particles.size(); i++) {
	//	particles[i]->Render();
	//}

}

void Application::endBattle() {
	p1Battle->setX(p1x);
	p2Battle->setX(p2x);
	p1Battle->setY(p1y);
	p2Battle->setY(p2y);

	if (p1Battle->hp > 0) {
		toTextBox("Player1 was victorious in the battle");
	}
	else if (p2Battle->hp > 0) {
		toTextBox("Player2 was victorious in the battle");
	}
	int size = bullets.size();
	int size2 = bullets2.size();
	for (int i = 0; i < size; i++) {
		delete bullets[bullets.size()-1];
		bullets.pop_back();
	}

	for (int i = 0; i < size2; i++) {
		delete bullets2[bullets2.size() - 1];
		bullets2.pop_back();
	}

	state = gameState;
}

Entity* Application::entOnNode(Node* node) {
	float gridX;
	float gridY;
	Node* pNode;

	for (int i = 0; i < player1.size(); i++) {
		gridX = (float)(convertX(player1[i]->getX()) * 1.0f);
		gridY = (float)(convertY(player1[i]->getY()) * -1.0f);
		pNode = nodes[CoordToNodeIdx(gridX, gridY)];
		if (pNode->x == node->x && pNode->y == node->y) {
			return player1[i];
		}
	}
	for (int i = 0; i < player2.size(); i++) {
		gridX = (float)(convertX(player2[i]->getX()) * 1.0f);
		gridY = (float)(convertY(player2[i]->getY()) * -1.0f);
		pNode = nodes[CoordToNodeIdx(gridX, gridY)];
		if (pNode->x == node->x && pNode->y == node->y) {
			return player2[i];
		}
	}

	return nullptr;

}

void Application::UpdateBattleLevel() {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	p1Battle->accelerationX = -0.0f;
	p2Battle->accelerationX = -0.0f;
	//p1 movement
	if (keys[SDL_SCANCODE_A]){
		p1Battle->accelerationX = -5.0f;
		p1Battle->flip = false;
	}
	if (keys[SDL_SCANCODE_D]){
		p1Battle->accelerationX = 5.0f;
		p1Battle->flip = true;
	}
	if (keys[SDL_SCANCODE_W]){
		if (p1Battle->collidedBottom == true){
			p1Battle->velocityY = 9.0f;
			p1Battle->collidedBottom = false;
			Mix_PlayChannel(-1, jump, 0);
		}
	}
	//p2 movement
	if (keys[SDL_SCANCODE_LEFT]){
		p2Battle->accelerationX = -5.0f;
		p2Battle->flip = true;
	}
	if (keys[SDL_SCANCODE_UP]){
		if (p2Battle->collidedBottom == true){
			p2Battle->velocityY = 9.0f;
			p2Battle->collidedBottom = false;
			Mix_PlayChannel(-1, jump, 0);
		}
	}
	if (keys[SDL_SCANCODE_RIGHT]){
		p2Battle->accelerationX = 5.0f;
		p2Battle->flip = false;
	}

	if (keys[SDL_SCANCODE_SPACE]) {
		if (cdCount > 10) {
			if (p1Battle->atk == 1) {
				shootBullet(37);
			}
			else if (p1Battle->atk == 3) {
				shootBullet(38);
			}
			else if (p1Battle->atk == 5) {
				shootBullet(28);
			}
			Mix_PlayChannel(-1, pew, 0);
		}
	}

	if (keys[SDL_SCANCODE_RETURN]) {
		if (cdCount2 > 10) {
			if (p2Battle->atk == 1) {
				shootBullet2(37);
			}
			else if (p2Battle->atk == 3) {
				shootBullet2(38);
			}
			else if (p2Battle->atk == 5) {
				shootBullet2(28);
			}
			Mix_PlayChannel(-1, pew, 0);
		}
	}

	//other
	if (keys[SDL_SCANCODE_K]) { //killbutton
		p2Battle->hp = 0;
	}

	if (keys[SDL_SCANCODE_U]) { //killbutton
		state=STATE_GAME_OVER;
	}

	if (p1Battle->hp <= 0 || p2Battle->hp <= 0) {
		endBattle(); 
	}
}

void Application::updateWin() {
	int deadCount1 = 0;
	int deadCount2 = 0;
	for (int i = 0; i < player1.size(); i++) {
		if (!player1[i]->isDead()) {
			deadCount1++;
		}
	}
	for (int i = 0; i < player2.size(); i++) {
		if (!player2[i]->isDead()) {
			deadCount2++;
		}
	}

	if (deadCount1 < 1) {
		toTextBox("Player2 wins!");
		p1turn = false;
		state = STATE_GAME_OVER;
	}
	else if (deadCount2 < 1) {
		toTextBox("Player1 wins!");
		p1turn = true;
		state = STATE_GAME_OVER;
	}

	//need to change state and draw new screen as well
}

void Application::drawLevelSelect() {
	//top
	GLfloat topBox[] = { 0.1f, 3.0f, 2.5f, 3.0f, 2.5f, 2.5f, 0.1f, 2.5f };
	glVertexPointer(2, GL_FLOAT, 0, topBox);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat boxColor[] = { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-1.39, -1.54, 0.0);
	glColorPointer(4, GL_FLOAT, 0, boxColor);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_COLOR_ARRAY);
	glPopMatrix();

	//middle
	GLfloat midBox[] = { 0.1f, 2.0f, 2.5f, 2.0f, 2.5f, 1.5f, 0.1f, 1.5f };
	GLfloat boxColor2[] = { 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	glVertexPointer(2, GL_FLOAT, 0, midBox);
	glEnableClientState(GL_VERTEX_ARRAY);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-1.39, -1.54, 0.0);
	glColorPointer(4, GL_FLOAT, 0, boxColor2);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_COLOR_ARRAY);
	glPopMatrix();


	//bot
	GLfloat botBox[] = { 0.1f, 1.0f, 2.5f, 1.0f, 2.5f, 0.5f, 0.1f, 0.5f };
	GLfloat boxColor3[] = { 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	glVertexPointer(2, GL_FLOAT, 0, botBox);
	glEnableClientState(GL_VERTEX_ARRAY);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-1.39, -1.54, 0.0);
	glColorPointer(4, GL_FLOAT, 0, boxColor3);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopMatrix();

}

void Application::updateLevelSelect() {

}

void Application::drawInsideLevelSelect() {
	drawText(fontText, "Level 1", 0.05f, 0.02f, 1.0f, 1.0f, 1.0f, 1.0f, -0.3f, 1.2f);
	drawText(fontText, "Level 2", 0.05f, 0.02f, 1.0f, 1.0f, 1.0f, 1.0f, -0.3f, 0.2f);
	drawText(fontText, "Level 3", 0.05f, 0.02f, 1.0f, 1.0f, 1.0f, 1.0f, -0.3f, -0.8f);
}

void Application::shootBullet(int idx) {
	float u = (float)((idx % SPRITE_X) / (SPRITE_X * 1.0f));
	float v = (idx / 8) / 8.0f;
	float w = (1.0f / (SPRITE_X * 1.0f));
	float h = (1.0f / (SPRITE_Y * 1.0f));
	
	Bullet *newBullet = new Bullet(p1Battle->getX(), p1Battle->getY(), u, v, w, h, BULLET_SPEED, sheet);
	newBullet->flip = p1Battle->flip;
	//if (!p1Battle->flip) {
	//	newBullet->setAngle(270.0f);
	//}
	bullets.push_back(newBullet);
	cdCount = 0;
}

bool Application::doBulletCollision(Bullet* bullet) {
	int gridX, gridY;
	worldToTileCoordinates(bullet->getX(), bullet->getY(), &gridX, &gridY);

	int grid2, grid3;
	worldToTileCoordinates(p2Battle->getX(), p2Battle->getY(), &grid2, &grid3);
	if (gridX >(grid2 - p2Battle->getW()) && gridX < (grid2 + p2Battle->getW()) 
		&& gridY >(grid3 - p2Battle->getH()) && gridY < (grid3 + p2Battle->getH())
		&& (!p2Battle->isDead())) {
		p2Battle->hp = p2Battle->hp - p1Battle->atk;
		//boom(p2Battle->getX(), p2Battle->getY());
		return true;
	}

	return false;
}

void Application::shootBullet2(int idx) {
	float u = (float)((idx % SPRITE_X) / (SPRITE_X * 1.0f));
	float v = (idx / 8) / 8.0f;
	float w = (1.0f / (SPRITE_X * 1.0f));
	float h = (1.0f / (SPRITE_Y * 1.0f));
	
	Bullet *newBullet = new Bullet(p2Battle->getX(), p2Battle->getY(), u, v, w, h, BULLET_SPEED, sheet);
	bullets2.push_back(newBullet);
	newBullet->flip = !p2Battle->flip;
	cdCount2 = 0;
}

bool Application::doBulletCollision2(Bullet* bullet) {
	int gridX, gridY;
	worldToTileCoordinates(bullet->getX(), bullet->getY(), &gridX, &gridY);

	int grid2, grid3;
	worldToTileCoordinates(p1Battle->getX(), p1Battle->getY(), &grid2, &grid3);
	if (gridX >(grid2 - p1Battle->getW()) && gridX < (grid2 + p1Battle->getW()) 
		&& gridY >(grid3 - p1Battle->getH()) && gridY < (grid3 + p1Battle->getH())
		&& (!p1Battle->isDead())) {
		p1Battle->hp = p1Battle->hp - p2Battle->atk;
		//boom(p1Battle->getX(), p1Battle->getY());
		return true;
	}

	return false;
}

void Application::gameOver(bool p1) {

	//top
	GLfloat topBox[] = { 0.1f, 3.0f, 2.5f, 3.0f, 2.5f, 1.0f, 0.1f, 1.0f };
	glVertexPointer(2, GL_FLOAT, 0, topBox);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat boxColor[] = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-1.39, -1.54, 0.0);
	glColorPointer(4, GL_FLOAT, 0, boxColor);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_COLOR_ARRAY);
	glPopMatrix();

	if (p1turn) {
		drawText(fontText, "Player 1 is victorious!", 0.08f, 0.02f, 0.0f, 0.0f, 0.0f, 1.0f, -1.2, 1.0);
	}
	else {
		drawText(fontText, "Player 2 is victorious!", 0.08f, 0.02f, 0.0f, 0.0f, 0.0f, 1.0f, -1.2, 1.0);
	}

	drawText(fontText, "Developers:", 0.08f, 0.02f, 0.0f, 0.0f, 0.0f, 1.0f, -1.2, 0.7);
	drawText(fontText, "Raymond Mak", 0.08f, 0.02f, 0.0f, 0.0f, 0.0f, 1.0f, -1.2, 0.5);
	drawText(fontText, "Michael Chan", 0.08f, 0.02f, 0.0f, 0.0f, 0.0f, 1.0f, -1.2, 0.35);

}

void Application::boom(float x, float y) {
	ParticleEmitter* boom = new ParticleEmitter(300, x, y, 0.0f, -2.0f, 0.0f, 0.7f, 0.7f, 1.0f);
	boom->setSColor(1.0f, 1.0f, 0.0f, 1.0f);
	boom->setEColor(0.0f, 0.15f, 1.0f, 1.0f);
	particles.push_back(boom);
}