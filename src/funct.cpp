#include "funct_headers.h"
#include "LWindow.h"
#include "LTimer.h"

extern SDL_Renderer* gRenderer;
extern LTexture gBackgroundTexture;

const std::string TITLE_STR = "Chess";
const std::string TITLE_AUTHOR_STR = "by Valentin with Love";
const std::string MENU_PLAY_STR = "Play PvP";
const std::string MENU_PLAY_AI_STR = "Play vs AI";
const std::string MENU_SETTINGS_STR = "Game Settings";


bool loadMenu(LTexture menuTextures[TOTAL_MENU_ITEMS]) {
	TTF_Font* font64 = TTF_OpenFont("../Resources/valentin.ttf", 64);
	bool success = true; 
	SDL_Color black = {0,0,0,0xFF};
	if(!(menuTextures[PLAY].loadFromRenderedText(font64, MENU_PLAY_STR.c_str() , black))) {
		TTF_CloseFont(font64);
		success = false;
	}
	else if(!(menuTextures[PLAY_AI].loadFromRenderedText(font64, MENU_PLAY_AI_STR.c_str() , black))) {
		TTF_CloseFont(font64);
		success = false;
	}
	else if(!(menuTextures[SETTINGS].loadFromRenderedText(font64, MENU_SETTINGS_STR.c_str() , black))) {
		TTF_CloseFont(font64);
		success = false;
	}
	else if(!(gBackgroundTexture.loadFromFile("../Resources/bg1.jpg"))) {
		TTF_CloseFont(font64);
		success = false;
	}
	TTF_CloseFont(font64);
	return success;
}

void loadMenuHighlight(LTexture menuTextures[TOTAL_MENU_ITEMS], int position) {
	bool success = true; 
	TTF_Font* font64 = TTF_OpenFont("../Resources/valentin.ttf", 64);
	SDL_Color black = {0,0,0,0xFF};
	SDL_Color red = {0xFF,0,0,0xFF};
	switch(position) {
		case PLAY:
			menuTextures[PLAY].loadFromRenderedText(font64, MENU_PLAY_STR.c_str() , red);
			menuTextures[PLAY_AI].loadFromRenderedText(font64, MENU_PLAY_AI_STR.c_str() , black);
			menuTextures[SETTINGS].loadFromRenderedText(font64, MENU_SETTINGS_STR.c_str() , black);
			break;
		case PLAY_AI:
			menuTextures[PLAY].loadFromRenderedText(font64, MENU_PLAY_STR.c_str() , black);
			menuTextures[PLAY_AI].loadFromRenderedText(font64, MENU_PLAY_AI_STR.c_str() , red);
			menuTextures[SETTINGS].loadFromRenderedText(font64, MENU_SETTINGS_STR.c_str() , black);
			break;
		case SETTINGS:
			menuTextures[PLAY].loadFromRenderedText(font64, MENU_PLAY_STR.c_str() , black);
			menuTextures[PLAY_AI].loadFromRenderedText(font64, MENU_PLAY_AI_STR.c_str() , black);
			menuTextures[SETTINGS].loadFromRenderedText(font64, MENU_SETTINGS_STR.c_str() , red);
			break;
		default: 
			menuTextures[PLAY].loadFromRenderedText(font64, MENU_PLAY_STR.c_str() , black);
			menuTextures[PLAY_AI].loadFromRenderedText(font64, MENU_PLAY_AI_STR.c_str() , black);
			menuTextures[SETTINGS].loadFromRenderedText(font64, MENU_SETTINGS_STR.c_str() , black);
			break;
	}
	TTF_CloseFont(font64);
}


void displayMenu(LTexture menuTextures[TOTAL_MENU_ITEMS]) {
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);
	
	//render BG 1st
	gBackgroundTexture.render();
	
	//render center text texture
	if(menuTextures[PLAY_AI].x() == 0 || menuTextures[PLAY_AI].y() == 0) {
		menuTextures[PLAY_AI].setX((SCREEN_WIDTH - menuTextures[PLAY_AI].getWidth()) / 2);
		menuTextures[PLAY_AI].setY((SCREEN_HEIGHT / 2) - (menuTextures[PLAY_AI].getHeight() / 2)); 
	}
	
	menuTextures[PLAY_AI].render(menuTextures[PLAY_AI].x(), menuTextures[PLAY_AI].y());
	
	//render above center text texture
	if(menuTextures[PLAY].x() == 0 || menuTextures[PLAY].y() == 0) {
		menuTextures[PLAY].setX((SCREEN_WIDTH - menuTextures[PLAY].getWidth()) / 2);
		menuTextures[PLAY].setY(menuTextures[PLAY_AI].y() - menuTextures[PLAY].getHeight());
	}
	
	menuTextures[PLAY].render(menuTextures[PLAY].x(), menuTextures[PLAY].y());
	
	//render below center text texture
	if(menuTextures[SETTINGS].x() == 0 || menuTextures[SETTINGS].y() == 0) {
		menuTextures[SETTINGS].setX((SCREEN_WIDTH - menuTextures[SETTINGS].getWidth()) / 2);
		menuTextures[SETTINGS].setY(menuTextures[PLAY_AI].y() + menuTextures[PLAY_AI].getHeight());
	}
	
	menuTextures[SETTINGS].render(menuTextures[SETTINGS].x(), menuTextures[SETTINGS].y());
	
	SDL_RenderPresent(gRenderer);
}

bool loadTitle(LTexture titleTexture[TOTAL_TITLE_ITEMS]) {
	TTF_Font* font64 = TTF_OpenFont("../Resources/valentin.ttf", 64);
	TTF_Font* font32 = TTF_OpenFont("../Resources/valentin.ttf", 32);
	bool success = true;
	SDL_Color black = {0,0,0,0xFF};
	
	if( font64 == NULL  || font32 == NULL) {
			printf( "Failed to load valentin font! SDL_ttf Error: %s\n", TTF_GetError() );
			success = false;
	}
	else if(!(titleTexture[TITLE].loadFromRenderedText(font64, TITLE_STR.c_str() , black))) {
		printf("Unable to load rendered text!");
		TTF_CloseFont(font64);
		success = false;
	}
	else if(!(titleTexture[AUTHOR].loadFromRenderedText(font32, TITLE_AUTHOR_STR.c_str() , black))) {
		printf("Unable to load rendered text!");
		TTF_CloseFont(font32);
		success = false;
	}
	TTF_CloseFont(font64);
	TTF_CloseFont(font32);
	return success;
}

void displayTitle(LTexture titleTexture[TOTAL_TITLE_ITEMS]) {
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);
	gBackgroundTexture.render();
	titleTexture[TITLE].render((SCREEN_WIDTH - titleTexture[TITLE].getWidth()) / 2, (SCREEN_HEIGHT - titleTexture[TITLE].getHeight()) /2);
	titleTexture[AUTHOR].render(SCREEN_WIDTH - titleTexture[AUTHOR].getWidth(), SCREEN_HEIGHT - titleTexture[AUTHOR].getHeight());
	SDL_RenderPresent(gRenderer);
}

void setButtons(LButton menuButtons[TOTAL_MENU_ITEMS], LTexture menuTextures[TOTAL_MENU_ITEMS]) {
	for(int i(0); i < TOTAL_MENU_ITEMS; i++) {
		menuButtons[i].setWidthAndHeight(menuTextures[i].getWidth(), menuTextures[i].getHeight());
		menuButtons[i].setPosition(menuTextures[i].x(), menuTextures[i].y());
	}
}

void unsetButtons(LButton menuButtons[TOTAL_MENU_ITEMS]) {
	for(int i(0); i < TOTAL_MENU_ITEMS; i++) {
		menuButtons[i].setWidthAndHeight(1,1);
		menuButtons[i].setPosition(0,0);
	}
}

void flushEvents() {
	SDL_PumpEvents();
	SDL_FlushEvents(SDL_MOUSEMOTION, SDL_MOUSEBUTTONUP);
}

Mix_Chunk* loadChunk(const char *src) {
	Mix_Chunk* chunk = Mix_LoadWAV(src);
	if(chunk == NULL) {
		printf( "Failed to load chunk sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
	}
	return chunk;
}

Mix_Music* loadMusic(const char *src) {
	Mix_Music* music = Mix_LoadMUS(src);
	if(music == NULL) {
		printf( "Failed to load music %c! SDL_mixer Error: %s\n", *src, Mix_GetError() );
	}
	return music;
}

void wait10s() {
	LTimer timer;
	timer.start();
	while(timer.getTicks() < 10000) {
		SDL_Delay(25);
	}
}