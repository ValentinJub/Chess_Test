#include "CApp.h"

SDL_Renderer* gRenderer = NULL; 
LTexture gBackgroundTexture;

CApp::CApp() {
	mIsRunning = true;
	mWaitBool = true;
	mButtonsSet = false;
	mWindow = new LWindow;
}

void CApp::close() {
	//Remember to close local and global var!!!!
	for(int i(0); i < TOTAL_MENU_ITEMS; i++) {
		mMenuTextures[i].free();
	}
	for(int i(0); i < TOTAL_TITLE_ITEMS; i++) {
		mTitleTextures[i].free();
	}
	
	gBackgroundTexture.free();
	
	Mix_FreeChunk(mStartupSound);
	mStartupSound = NULL;
	
	Mix_FreeChunk(mMenuClick);
	mMenuClick = NULL;
	
	Mix_FreeMusic(mMenuMusic);
	mMenuMusic = NULL;
	
	mWindow->free();
	mWindow = NULL;
	
	SDL_DestroyRenderer(gRenderer);
	
	IMG_Quit();
	TTF_Quit();
	Mix_Quit();
	SDL_Quit();
}

bool CApp::init() {
	bool success = true;
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		printf("SDL could not initialise! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) {
			printf( "Warning: Linear texture filtering not enabled!" );
		}
		//Create window
		if(!(mWindow->init())) {
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else {
			//Create renderer for window
			gRenderer = mWindow->createRenderer();
			if( gRenderer == NULL ) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else {
				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if(!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
				if(TTF_Init() == -1) {
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
				// open 22.05KHz, signed 16bit in system byte order,
				// stereo audio, using 2048 byte chunks
				if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
					success = false;
				}
				else {
					mStartupSound = loadChunk("../Resources/gc.mp3"); 
					mMenuClick = loadChunk("../Resources/START.wav"); 
					mMenuMusic = loadMusic("../Resources/menu.mp3");
				}
				//enable using rand n 
				srand(time(0));
			}
		}
	}
	return success;
}

void CApp::handleEvents(SDL_Event* e) {
	if(e->type == SDL_QUIT) {
		mIsRunning = false;
	}
	else if(e->type == SDL_KEYDOWN) {
		switch(e->key.keysym.sym) {
			case SDLK_ESCAPE:
				mIsRunning = false;
				break;
			case SDLK_1:
				stopMusic();
				//Unset menu buttons and reset menuBool
				unsetButtons(mMenuButtons);
				mButtonsSet = false;
				playPVP();
				break;
			case SDLK_2:
				stopMusic();
				//Unset menu buttons and reset menuBool
				unsetButtons(mMenuButtons);
				mButtonsSet = false;
				playerVersusComputer();
				break;
			case SDLK_3:
				//Unset menu buttons and reset menuBool
				unsetButtons(mMenuButtons);
				mButtonsSet = false;
				SDL_Delay(200);
				//flush events to prevent clicks clicking the 
				//menu buttons when going back to menu from settings
				flushEvents();
				settings();
				break;
		}
	}
	for(int i(0); i < TOTAL_MENU_ITEMS; i++) {
		if(mMenuButtons[i].handleInside(e)) {
			loadMenuHighlight(mMenuTextures, i);
			if(mMenuButtons[i].handleClick(e)) {
				if(i == PLAY) {
					stopMusic();
					//Unset menu buttons and reset menuBool
					unsetButtons(mMenuButtons);
					mButtonsSet = false;
					playPVP();
					flushEvents();
				}
				else if(i == PLAY_AI) {
					stopMusic();
					//Unset menu buttons and reset menuBool
					unsetButtons(mMenuButtons);
					mButtonsSet = false;
					playerVersusComputer();
					flushEvents();
				}
				else if(i == SETTINGS) {
					//Unset menu buttons and reset menuBool
					unsetButtons(mMenuButtons);
					mButtonsSet = false;
					SDL_Delay(200);
					//flush events to prevent clicks clicking the 
					//menu buttons when going back to menu from settings
					flushEvents();
					settings();
				}
			}
		}
	}
}

void CApp::loop() {
	SDL_Event e;
	while(SDL_PollEvent(&e) > 0) {
		handleEvents(&e);
	}
	/*Title screen
	if(mWaitBool) {
		displayTitle(mTitleTextures);
		Mix_PlayChannel(-1, mStartupSound, 0);
		while(Mix_Playing(-1) > 0) {
			SDL_Delay(16);
		}
		mWaitBool = false;
	}
	else { */
		playMusic();
		displayMenu(mMenuTextures);
		if(!(mButtonsSet)) {
			setButtons(mMenuButtons, mMenuTextures);
			mButtonsSet = true;
		}
	SDL_Delay(16);
	
}

int CApp::execute() {
	//Initialise SDL and sub-system
	if(!init()) {
		printf("Failed to initialise!");
	}
	else {
		//load title textures
		if(!loadTitle(mTitleTextures)) {
			printf("Failed to load title media!");
		}
		//load menu textures
		else if(!loadMenu(mMenuTextures)) {
			printf("Failed to load menu media!");
		}
		else {
			while(mIsRunning) {
				//main loop
				//->eventHandler
				//->displayTitle
				//->displayMenu
				loop();
			}
		}
	}
	//Close SDL and sub-system
	CApp::close();
	return 0;
}

void CApp::playMusic() {
	//loop infinitely menu music
	if(Mix_PlayingMusic() == 0) {
		Mix_PlayMusic(mMenuMusic, -1);
		Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
	}
}

void CApp::stopMusic() {
	if(Mix_PlayingMusic()) {
		Mix_FadeOutMusic(300);
	}
}