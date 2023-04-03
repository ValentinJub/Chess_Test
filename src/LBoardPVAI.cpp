/*
LBoardPVAI.cpp
Sun Mar 20 2022
by Valentin
-----------
Methods for class LBoardPVAI
*/

#include "LBoardPVAI.h"

extern LTexture gBackgroundTexture;
using std::vector;
extern SDL_Renderer* gRenderer;

LBoardPVAI::LBoardPVAI() {
	//grab settings from file
	readSettingsFromFile();
	//init some game variable according to settings (mPreview, mTileColor)
	initGameSettings();
	//Init textures
	mPieceTexture = new LTexture;
	mHighlightedPieceTexture = new LTexture;
	mTileTexture = new LTexture;
	mMiniPieceTexture = new LTexture;
	mHighlightedTileTexture = new LTexture;
	mWhiteTimerTexture = new LTexture;
	mWhiteScoreTexture = new LTexture;
	mBlackScoreTexture = new LTexture;
	mPauseBackgroundTexture = new LTexture;
	mPauseTextTexture = new LTexture;
	
	for(int i(0); i < INITIAL_PIECES_TOTAL; i++) {
		mPieceButtons.push_back(new LButton);
	}
	
	for(int i(0); i < EMPTY; i++) {
		mDeadWhitePiece[i] = 0;
		mDeadBlackPiece[i] = 0;
	}
	
	//Set to 9 so no pieces 
	//Are highlighted at start
	mSelectedPieceXPos = 9;
	mSelectedPieceYPos = 9;
	//No piece is selected at start
	mAPieceIsSelected = false;
	
	//White start
	mWhiteTurn = true;
	
	mIsPaused = false;
	
	//Check status
	mCheckStatus = NO_CHECK;
	
	mBlackIsThinking = false;
	
	//bools used to play sound depending on the context
	mTookAPiece = false;
	mIsCastling = false;
	mEnPassantTurn = false;
	mGameOver = false;
	
	//Bools below used to determine if we can use Castling();  
	mWKingHasMoved = false;
	mBKingHasMoved = false;
	mWRook1HasMoved = false;
	mWRook2HasMoved = false;
	mBRook1HasMoved = false;
	mBRook2HasMoved = false;
	
	//load sound effect
	mBingo = loadChunk("../Resources/chime.wav");
	mCheck = loadChunk("../Resources/checkUS.wav");
	mCheckMate = loadChunk("../Resources/checkMateUS.wav");
	mError = loadChunk("../Resources/error.wav");
	mPawnForward = loadChunk("../Resources/pawnForward.wav");
	mCastling = loadChunk("../Resources/0-0-0.wav");
	mPieceFall = loadChunk("../Resources/drop3.wav");
	mVictory = loadChunk("../Resources/victoryOrchestra.wav");
	mDefeat = loadChunk("../Resources/gameOver.wav");
	
	//start white and black timer and pause the black as white plays first
	if(mSettingsTable[TL_YES]) {
		startWhiteTimer();
	}
	mWhiteTimerRanOut = false;
	
	initPieceStr();
	
	mTurn = 0;
	mBlackCanCastle = false;
}

void LBoardPVAI::initPieceStr() {
	mPieceStr[BBISHOP] = "bBishop";
	mPieceStr[BKING] = "bKing";
	mPieceStr[BKNIGHT] = "bKnight";
	mPieceStr[BPAWN] = "bPawn";
	mPieceStr[BQUEEN] = "bQueen";
	mPieceStr[BROOK] = "bRook";
	mPieceStr[WBISHOP] = "wBishop";
	mPieceStr[WKING] = "wKing";
	mPieceStr[WKNIGHT] = "wKnight";
	mPieceStr[WPAWN] = "wPawn";
	mPieceStr[WQUEEN] = "wQueen";
	mPieceStr[WROOK] = "wRook";
}

bool LBoardPVAI::isBlackThinking() const {
	return mBlackIsThinking;
}

void LBoardPVAI::stopBlackThinking() {
	mBlackIsThinking = false;
}

void LBoardPVAI::initGameSettings() {
	//Set show legal move (highlight tile with selected piece's possible moves)
	if(mSettingsTable[SLM_YES]) mPreviewMove = true;
	else mPreviewMove = false;
	//Set tile color
	if(mSettingsTable[TC_GREY]) mTileColor = 1;
	else mTileColor = 0;
	//Set timer duration
	if(mSettingsTable[TL_YES]) {
		if(mSettingsTable[TL_5]) {
			mTimeLimit = 300; 
		}
		else {
			mTimeLimit = 600; 
		}
	}
	//Set music
	if(mSettingsTable[MT_CLASSIC]) {
		for(int i(0); i < NBR_OF_MUSIC; i++) {
			std::string str = "../Resources/class" + std::to_string(i) + ".mp3";
			mMusic[i] = loadMusic(str.c_str());
		}
	}
	//Jazzy
	else {
		for(int i(0); i < NBR_OF_MUSIC; i++) {
			std::string str = "../Resources/jazz" + std::to_string(i) + ".mp3";
			mMusic[i] = loadMusic(str.c_str());
		}
	}
	//Piece Theme 
	if(mSettingsTable[PT_1]) {
		mPieceTheme = 0;
	}
	else mPieceTheme = 1;
}

void LBoardPVAI::playMusic() {
	//if no music is playing
	if(Mix_PlayingMusic() == 0) {
		int i = rand() % 7;
		//Play the music
		Mix_FadeInMusic(mMusic[i], 0, 10000);
		Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
	}
}

bool LBoardPVAI::isPaused() const {
	return mIsPaused;
}

void LBoardPVAI::pause() {
	if(mIsPaused) {
		mIsPaused = false;
		if(mSettingsTable[TL_YES]) {
			if(mWhiteTurn) unpauseWhiteTimer();
		}
	}
	else {
		mIsPaused = true;
		if(mSettingsTable[TL_YES]) {
			if(mWhiteTurn) pauseWhiteTimer();
		}
	}
}
		
LBoardPVAI::~LBoardPVAI() {
	free();
}

void LBoardPVAI::free() {
	mPieceTexture->free();
	mPieceTexture = NULL;
	
	mHighlightedPieceTexture->free();
	mHighlightedPieceTexture = NULL;
	
	mTileTexture->free();
	mTileTexture = NULL;
	
	mWhiteTimerTexture->free();
	mWhiteTimerTexture = NULL;
	
	mWhiteScoreTexture->free();
	mWhiteScoreTexture = NULL;
	mBlackScoreTexture->free();
	mBlackScoreTexture = NULL;
	
	mHighlightedTileTexture->free();
	mHighlightedTileTexture = NULL;
	
	mPieceButtons.clear();
	mHighlightedTileXPos.clear();
	mHighlightedTileYPos.clear();
	
	Mix_FreeChunk(mError);
	mError = NULL;
	Mix_FreeChunk(mBingo);
	mBingo = NULL;
	Mix_FreeChunk(mCheck);
	mCheck = NULL;
	Mix_FreeChunk(mCheckMate);
	mCheckMate = NULL;
	Mix_FreeChunk(mCastling);
	mCastling = NULL;
	Mix_FreeChunk(mPawnForward);
	mPawnForward = NULL;
	Mix_FreeChunk(mPieceFall);
	mPieceFall = NULL;
	Mix_FreeChunk(mVictory);
	mVictory = NULL;
	Mix_FreeChunk(mDefeat);
	mDefeat = NULL;
	
	for(int i(0); i < NBR_OF_MUSIC; i++) {
		Mix_FreeMusic(mMusic[i]);
		mMusic[i] = NULL;
	}
}

void LBoardPVAI::startWhiteTimer() {
	mWhiteTimer.start();
}

void LBoardPVAI::pauseWhiteTimer() {
	mWhiteTimer.pause();
}

void LBoardPVAI::unpauseWhiteTimer() {
	mWhiteTimer.unpause();
}

void LBoardPVAI::stopWhiteTimer() {
	mWhiteTimer.stop();
}

void LBoardPVAI::readSettingsFromFile() {
	
	CFURLRef appUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("settings.config"), NULL, NULL);
	CFStringRef filePathRef = CFURLCopyPath(appUrlRef);
	const char* filePath = CFStringGetCStringPtr(filePathRef, kCFStringEncodingUTF8);

	
	std::ifstream settings;
	settings.open(filePath);
	
	CFRelease(filePathRef);
	CFRelease(appUrlRef);
	
	if(settings.is_open()) {
		char c(0);
		int i(0);
		while(settings.get(c)) {
			if(i < TOTAL_CLICKABLE_ITEMS) {
				int ac = c - '0';
				mSettingsTable[i] = ac;
				i++;
			}
		}
		settings.close();
	}
	else {
		printf("Unable to open settings.config");
	}
}

bool LBoardPVAI::loadPiecesTextures() {
	bool success = true;
	if(mPieceTheme == 0) {
		if(!(mPieceTexture->loadFromFile("../Resources/pieceSpritesheet.png"))) success = false;
		else if(!(mHighlightedPieceTexture->loadFromFile("../Resources/pieceSpritesheet.png"))) success = false;
		else mHighlightedPieceTexture->setColor(255,0,0);
		//score piece
		if(!(mMiniPieceTexture->loadFromFile("../Resources/spritesheet32.png"))) success = false;
	} else if(mPieceTheme == 1) {
		if(!(mPieceTexture->loadFromFile("../Resources/retroPieceSpriteSheet.png"))) success = false;
		else if(!(mHighlightedPieceTexture->loadFromFile("../Resources/retroPieceSpritesheet.png"))) success = false;
		else mHighlightedPieceTexture->setColor(255,0,0);
		//score piece
		if(!(mMiniPieceTexture->loadFromFile("../Resources/spritesheet32.png"))) success = false;
	}
	return success; 
}

bool LBoardPVAI::loadTileTextures() {
	bool success = true;
	if(!(mTileTexture->loadFromFile("../Resources/board.png"))) success = false;
	else if(!(mHighlightedTileTexture->loadFromFile("../Resources/board.png"))) success = false;
	else mHighlightedTileTexture->setColor(255,0,0);
	return success;
}

bool LBoardPVAI::loadPauseTexture() {
	SDL_Color white = {0xFF, 0xFF, 0xFF, 0xFF};
	TTF_Font* font64 = TTF_OpenFont("../Resources/valentin.ttf", 64);
	bool success = true;
	if(!(mPauseBackgroundTexture->loadFromFile("../Resources/blackScreen.png"))) success = false;
	else {
		mPauseBackgroundTexture->setAlpha(127);
		mPauseBackgroundTexture->setBlendMode(SDL_BLENDMODE_BLEND);
	}
	if(!(mPauseTextTexture->loadFromRenderedText(font64, "Pause", white))) {
		TTF_CloseFont(font64);
		success = false;
	}
	TTF_CloseFont(font64);
	return success;
}

void LBoardPVAI::setPiecesClip() {
	for(int i(0); i < TOTAL_PIECES - 1; i++) {
		mPieceClip[i].x = TOTAL_SQUARES*i;
		mPieceClip[i].y = 0;
		mPieceClip[i].w = TOTAL_SQUARES;
		mPieceClip[i].h = TOTAL_SQUARES;
	
		mMiniPieceClip[i].x = (TOTAL_SQUARES / 2) * i;
		mMiniPieceClip[i].y = 0;
		mMiniPieceClip[i].w = (TOTAL_SQUARES / 2);
		mMiniPieceClip[i].h = (TOTAL_SQUARES / 2);		
	}
}

void LBoardPVAI::setTileRectClip() {
	mTileRectClip[DARK1].x = 0;
	mTileRectClip[DARK1].y = 0;
	mTileRectClip[DARK1].w = TOTAL_SQUARES;
	mTileRectClip[DARK1].h = TOTAL_SQUARES;
	
	mTileRectClip[LIGHT1].x = TOTAL_SQUARES;
	mTileRectClip[LIGHT1].y = 0;
	mTileRectClip[LIGHT1].w = TOTAL_SQUARES;
	mTileRectClip[LIGHT1].h = TOTAL_SQUARES;
	
	mTileRectClip[DARK2].x = 128;
	mTileRectClip[DARK2].y = 0;
	mTileRectClip[DARK2].w = TOTAL_SQUARES;
	mTileRectClip[DARK2].h = TOTAL_SQUARES;
	
	mTileRectClip[LIGHT2].x = 192;
	mTileRectClip[LIGHT2].y = 0;
	mTileRectClip[LIGHT2].w = TOTAL_SQUARES;
	mTileRectClip[LIGHT2].h = TOTAL_SQUARES;
}

void LBoardPVAI::renderTile() {
	bool light = true,
	dark = true;
	int i(0);
	int xPos(0);
	int yPos(0);
	//color settings
	for(int y(0); y < SPL; y++) {
		for(int x(0); x < SPL; x++) {
			xPos = OFFSET + (TOTAL_SQUARES * x),
			yPos = OFFSET + (TOTAL_SQUARES * y);

			if(y % 2 == 0) {
				if(mTileColor == 0) {
					if(!light) mTileTexture->render(xPos, yPos, &mTileRectClip[DARK1]);
					else mTileTexture->render(xPos, yPos, &mTileRectClip[LIGHT1]);
				}
				else if(mTileColor == 1) {
					if(!light) mTileTexture->render(xPos, yPos, &mTileRectClip[DARK2]);
					else mTileTexture->render(xPos, yPos, &mTileRectClip[LIGHT2]);
				}
				if(light) light = false;
				else light = true;
			}
			else {
				if(mTileColor == 0) {
					if(!dark) mTileTexture->render(xPos, yPos, &mTileRectClip[LIGHT1]);
					else mTileTexture->render(xPos, yPos, &mTileRectClip[DARK1]);
				}
				else if(mTileColor == 1) {
					if(!dark) mTileTexture->render(xPos, yPos, &mTileRectClip[LIGHT2]);
					else mTileTexture->render(xPos, yPos, &mTileRectClip[DARK2]);
				}
				if(dark) dark = false;
				else dark = true;
			}
		}
	}
	//only highlight tile if a piece is selected
	if(mAPieceIsSelected) {
		//show legal move bool settings
		if(mPreviewMove) {
			for(int z(0); z < mHighlightedTileYPos.size(); z++) {
				yPos = OFFSET + (mHighlightedTileYPos[z] * TOTAL_SQUARES);
				xPos = OFFSET + (mHighlightedTileXPos[z] * TOTAL_SQUARES);
				mHighlightedTileTexture->render(xPos, yPos, &mTileRectClip[LIGHT1]);
			}
		}
	}
	if(mRightClickedTileXPos.size() > 0) {
		for(int z(0); z < mRightClickedTileXPos.size(); z++) {
			yPos = OFFSET + (mRightClickedTileYPos[z] * TOTAL_SQUARES);
			xPos = OFFSET + (mRightClickedTileXPos[z] * TOTAL_SQUARES);
			mHighlightedTileTexture->render(xPos, yPos, &mTileRectClip[LIGHT2]);
		}
	}
}
	
void LBoardPVAI::renderPieces() {
	for(int y(0); y < SPL; y++) {
		for(int x(0); x < SPL; x++) {
			if((mMap[y][x] >= 0) && (mMap[y][x] < TOTAL_PIECES - 1)) {
				if( (mAPieceIsSelected) && (mSelectedPieceXPos == x) && (mSelectedPieceYPos == y)) {
					mHighlightedPieceTexture->render(OFFSET + (TOTAL_SQUARES * x), OFFSET + (TOTAL_SQUARES * y), &mPieceClip[mMap[y][x]]);
				}
				else mPieceTexture->render(OFFSET + (TOTAL_SQUARES * x), OFFSET + (TOTAL_SQUARES * y), &mPieceClip[mMap[y][x]]);
			}
		}
	}
}

void LBoardPVAI::setButtons() {
	int i(0);
	for(int y(0); y < SPL; y++) {
		for(int x(0); x < SPL; x++) {
			if((mMap[y][x] >= 0) && (mMap[y][x] < TOTAL_PIECES - 1)) {
				mPieceButtons[i]->setPosition(OFFSET + (x * TOTAL_SQUARES), OFFSET + (y * TOTAL_SQUARES));
				mPieceButtons[i]->setWidthAndHeight(TOTAL_SQUARES,TOTAL_SQUARES);
				i++;
			}
		}
	}
}

void LBoardPVAI::drawButtons() {
	for(int i(0); i < INITIAL_PIECES_TOTAL - 1; i++) {
		SDL_Rect r;
		r.x = mPieceButtons[i]->getX();
		r.y = mPieceButtons[i]->getY();
		r.w = mPieceButtons[i]->getW();
		r.h = mPieceButtons[i]->getH();
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0, 0, 0xFF);
		SDL_RenderDrawRect(gRenderer, &r);
	}
}

void LBoardPVAI::renderPause() {
	mPauseBackgroundTexture->render();
	mPauseTextTexture->render((SCREEN_WIDTH - mPauseTextTexture->getWidth()) / 2, (SCREEN_HEIGHT - mPauseTextTexture->getHeight()) / 2); 
}

void LBoardPVAI::handleEvents(SDL_Event* e) {
	bool outside = true; 
	int x, y;
	SDL_GetMouseState( &x, &y );
	x = (x / TOTAL_SQUARES) - 1;
	y = (y / TOTAL_SQUARES) - 1;
	//check for each buttons
	for(int i(0); i < mPieceButtons.size(); i++) {
		//if mouse is inside
		if(mPieceButtons[i]->handleInside(e)) {
			//if mouse button clicked 
			if(mPieceButtons[i]->handleClick(e)) {
				//if a piece is not selected 
				//if a piece is already selected, count as clicking outside 
				if(!(mAPieceIsSelected)) {
					mSelectedPieceType = mMap[y][x];
					//if white's turn - select white
					if((mWhiteTurn) && (mSelectedPieceType >= WBISHOP) && (mSelectedPieceType < EMPTY)) {
						outside = false;
						mSelectedPieceXPos = x;
						mSelectedPieceYPos = y;
						mAPieceIsSelected = true;
						mRightClickedTileXPos.clear();
						mRightClickedTileYPos.clear();
					}
				}
			}
		}
	}
	//if a piece is selected - can move
	//to pos set in shwlegalMove()
	if(mAPieceIsSelected) {
		showLegalMove(mMap, mSelectedPieceType, mSelectedPieceXPos, mSelectedPieceYPos);
		movePiece(e);
	}
	if(outside) {
		if((e->type == SDL_MOUSEBUTTONUP) && (e->button.button == SDL_BUTTON_LEFT)) {
			mAPieceIsSelected = false;
			mSelectedPieceXPos = 9;
			mSelectedPieceYPos = 9;
			mHighlightedTileXPos.clear();
			mHighlightedTileYPos.clear();
			mRightClickedTileXPos.clear();
			mRightClickedTileYPos.clear();
		}
	}
	if((e->type == SDL_MOUSEBUTTONUP) && (e->button.button == SDL_BUTTON_RIGHT)) {
		bool dont = false;
		for(int z(0); z < mRightClickedTileXPos.size(); z++) {
			if((mRightClickedTileXPos[z] == x) && (mRightClickedTileYPos[z] == y)) {
				mRightClickedTileXPos[z] = 8;
				mRightClickedTileYPos[z] = 8;
				dont = true;
			}
		}
		if(!(dont)) {
			mRightClickedTileXPos.push_back(x);
			mRightClickedTileYPos.push_back(y);
		}
	} 
}

//fill mHighlightedTileXPos/YPos with
//possible moves according to selected piece
void LBoardPVAI::showLegalMove(const int map[SPL][SPL], const int pieceType, const int pieceXPos, const int pieceYPos, bool preCheck) {
	//clear to make sure we don't add to 
	//existing vector content
	mHighlightedTileXPos.clear();
	mHighlightedTileYPos.clear();
	switch(pieceType) {
		case BPAWN:
			if(preCheck) {
				mPawn.blackMoveOld(map, pieceXPos, pieceYPos, &mHighlightedTileXPos, &mHighlightedTileYPos, mLastMovedPiece, mLastMovedPieceXSrc, mLastMovedPieceYSrc, mLastMovedPieceXDes, mLastMovedPieceYDes, &mEnPassantTurn, preCheck);
			}
			else {
				mPawn.blackMoveOld(map, pieceXPos, pieceYPos, &mHighlightedTileXPos, &mHighlightedTileYPos, mLastMovedPiece, mLastMovedPieceXSrc, mLastMovedPieceYSrc, mLastMovedPieceXDes, mLastMovedPieceYDes, &mEnPassantTurn);
			}
			break;
		case WPAWN:
			if(preCheck) {
				mPawn.whiteMoveOld(map, pieceXPos, pieceYPos, &mHighlightedTileXPos, &mHighlightedTileYPos, mLastMovedPiece, mLastMovedPieceXSrc, mLastMovedPieceYSrc, mLastMovedPieceXDes, mLastMovedPieceYDes, &mEnPassantTurn, preCheck);
			}
			else {
				mPawn.whiteMoveOld(map, pieceXPos, pieceYPos, &mHighlightedTileXPos, &mHighlightedTileYPos, mLastMovedPiece, mLastMovedPieceXSrc, mLastMovedPieceYSrc, mLastMovedPieceXDes, mLastMovedPieceYDes, &mEnPassantTurn); 
			}
			break;
		case BKNIGHT:
			mKnight.blackMove(map, pieceXPos, pieceYPos, &mHighlightedTileXPos, &mHighlightedTileYPos);
			break;
		case WKNIGHT:
			mKnight.whiteMove(map, pieceXPos, pieceYPos, &mHighlightedTileXPos, &mHighlightedTileYPos);
			break;
		case BBISHOP:
			mBishop.blackMove(map, pieceXPos, pieceYPos, &mHighlightedTileXPos, &mHighlightedTileYPos);
			break;
		case WBISHOP:
			mBishop.whiteMove(map, pieceXPos, pieceYPos, &mHighlightedTileXPos, &mHighlightedTileYPos);
			break;
		case BROOK:
			mRook.blackMove(map, pieceXPos, pieceYPos, &mHighlightedTileXPos, &mHighlightedTileYPos);
			break;
		case WROOK:
			mRook.whiteMove(map, pieceXPos, pieceYPos, &mHighlightedTileXPos, &mHighlightedTileYPos);
			break;
		case BQUEEN:
			mQueen.blackMove(map, pieceXPos, pieceYPos, &mHighlightedTileXPos, &mHighlightedTileYPos);
			break;
		case WQUEEN:
			mQueen.whiteMove(map, pieceXPos, pieceYPos, &mHighlightedTileXPos, &mHighlightedTileYPos);
			break;
		case WKING:
			mKing.whiteMove(map, pieceXPos, pieceYPos, &mHighlightedTileXPos, &mHighlightedTileYPos, mWKingHasMoved, mWRook1HasMoved, mWRook2HasMoved, mCheckStatus, &mBlackCanCastle);
			break;
		case BKING:
			mKing.blackMove(map, pieceXPos, pieceYPos, &mHighlightedTileXPos, &mHighlightedTileYPos, mBKingHasMoved, mBRook1HasMoved, mBRook2HasMoved, mCheckStatus, &mBlackCanCastle);
			break;
		default:
			break;
	}
}

void LBoardPVAI::movePiece(SDL_Event* e) {
	int destinationPosX, destinationPosY;
	SDL_GetMouseState( &destinationPosX, &destinationPosY );
	destinationPosX = (destinationPosX / TOTAL_SQUARES) - 1;
	destinationPosY = (destinationPosY / TOTAL_SQUARES) - 1;
	if(e->type == SDL_MOUSEBUTTONUP) {
		//if click is in possible square posX&Y[i]
		for(int i(0); i < mHighlightedTileYPos.size(); i++) {
			if((destinationPosX == mHighlightedTileXPos[i]) && (destinationPosY == mHighlightedTileYPos[i])) {
				int piece = mSelectedPieceType;
				int srcPosX = mSelectedPieceXPos;
				int srcPosY = mSelectedPieceYPos;
				//check if the move we're about to play is goint to check
				//our own King - returns true if it does check own King
				//false if it's not checking our own King
				if(!(pollDiscoverAttack(mMap, piece, destinationPosX, destinationPosY, srcPosX, srcPosY))) { 
					//check if the move we're about to play is a castling move
					//if castling returns false then it was a castling move but it
					//cant be done - returns true in all other screnarios
					if(castling(piece, destinationPosX, destinationPosY, srcPosX, srcPosY)) {
							move(destinationPosX, destinationPosY, srcPosX, srcPosY, piece);
							//exit loop
							i = 99;
					}
					else {
						//Play unable to move sound 
						Mix_PlayChannel(-1, mError, 0);
					}
				}
				//Unable to move due to move checking its own King
				else {
					//PPlay unable to move sound 
					Mix_PlayChannel(-1, mError, 0);
				}
			}
		}
	}
}


void LBoardPVAI::enPassant(int destinationPosX) { 
	if(destinationPosX == mLastMovedPieceXDes) {
		mMap[mLastMovedPieceYDes][mLastMovedPieceXDes] = EMPTY;
		mPieceButtons.resize(mPieceButtons.size() - 1);
		if(mWhiteTurn) {
			int fallenPiece = BPAWN;
			fillDeadPieceTab(fallenPiece);
		}
		else {
			int fallenPiece = WPAWN;
			fillDeadPieceTab(fallenPiece);
		}
		mTookAPiece = true;
	}
}

void LBoardPVAI::move(int destinationPosX, int destinationPosY, int srcPosX, int srcPosY, int piece) {
	
	//only enter if there's an EnPassant opportunity
	if(mEnPassantTurn) {
		enPassant(destinationPosX);
	} 
	
	if(mMap[destinationPosY][destinationPosX] != EMPTY) {
		mPieceButtons.resize(mPieceButtons.size() - 1);
		mTookAPiece = true;
		int fallenPiece = mMap[destinationPosY][destinationPosX];
		fillDeadPieceTab(fallenPiece);
	}
	
		
	//move selected piece to destination
	mMap[destinationPosY][destinationPosX] = piece;
	//empty source cell 
	mMap[srcPosY][srcPosX] = EMPTY;
	
	//manage playing sound according to played move
	playMoveSound();
	
	//turn castling bools to true if king or rook moves
	setCastlingBools(srcPosX, srcPosY, piece); 
	
	//turn Pawn into Queen if possible
	checkPromotion(destinationPosX, destinationPosY, piece);
	
	//Poll for check 
	//if there's check
	if(pollCheck(mMap)) {
		//poll for checkmate
		//if theres checkmate
		if(checkMate()) {
			mGameOver = true;
		}
		//check
		else {
			Mix_PlayChannel(-1, mCheck, 0 );
		}
	}
	
	
	setButtons();
	
	
	setEnPassantVar(piece, srcPosX, srcPosY, destinationPosX, destinationPosY);
	
	//change turn
	changeTurn();
	
	mTurn++;
	
	//refresh screen so that black turn is not played during the same frame as white
	renderBeforeBlackPlays();
	
	
	if(mTurn % 2 != 0) {
		blackTurn();
		setButtons();
		
		if(pollCheck(mMap)) {
			//poll for checkmate
			//if theres checkmate
			if(checkMate()) {
				mGameOver = true;
			}
			//check
			else {
				Mix_PlayChannel(-1, mCheck, 0 );
			}
		}
		mWhiteTurn = true;
		//clear vectors for next turn
		clearBlackVectors();
		mBlackIsThinking = true;
		mTurn++;
	}
	
}

//returns false if a castling move is unable to be performed
//true in all other cases
//+ does the tower move in this function, King move is handled in move();
bool LBoardPVAI::castling(int piece, int desPosX, int desPosY, int srcPosX, int srcPosY) {
	int castling = NO_CASTLING;
	if(piece == WKING) {
		if(!(mWKingHasMoved)) {
			//WR1
			if((desPosY == 7) && (desPosX == 2)) {
				if(!(pollDiscoverAttack(mMap, piece, desPosX + 1, desPosY, srcPosX, srcPosY))) {
					castling = CASTLE_WR1;
				} else castling = NO_MOVE;
			}
			//WR2
			else if((desPosY == 7) && (desPosX == 6)) {
				if(!(pollDiscoverAttack(mMap, piece, desPosX - 1, desPosY, srcPosX, srcPosY))) {
					castling = CASTLE_WR2;
				} else castling = NO_MOVE;
			}
		}
	}
	
	else if(piece == BKING) {
		if(!(mBKingHasMoved)) {
			//BR1
			if((desPosY == 0) && (desPosX == 2)) {
				if(!(pollDiscoverAttack(mMap, piece, desPosX + 1, desPosY, srcPosX, srcPosY))) {
					castling = CASTLE_BR1;
				} else castling = NO_MOVE;
			}
			//BR2
			else if((desPosY == 0) && (desPosX == 6)) {
				if(!(pollDiscoverAttack(mMap, piece, desPosX - 1, desPosY, srcPosX, srcPosY))) {
					castling = CASTLE_BR2;
				} else castling = NO_MOVE;
			}
		}
	}
	switch(castling) {
		case CASTLE_WR1:
			mMap[7][3] = WROOK;
			mMap[7][0] = EMPTY;
			//for sound effect
			mIsCastling = true;
			return true;
		case CASTLE_WR2:
			mMap[7][5] = WROOK;
			mMap[7][7] = EMPTY;
			//for sound effect
			mIsCastling = true;
			return true;
		case CASTLE_BR1:
			mMap[0][3] = BROOK;
			mMap[0][0] = EMPTY;
			//for sound effect
			mIsCastling = true;
			return true;
		case CASTLE_BR2:
			mMap[0][5] = BROOK;
			mMap[0][7] = EMPTY;
			//for sound effect
			mIsCastling = true;
			return true;
		case NO_CASTLING:
			return true;
		case NO_MOVE:
			return false;
	}
}

void LBoardPVAI::setCastlingBools(int piecePosX, int piecePosY, int piece) {
	if(piece == WKING) mWKingHasMoved = true;
	else if(piece == BKING) mBKingHasMoved = true;
	else if((piece == WROOK) && (piecePosX == 0) && (piecePosY == 7)) {
		mWRook1HasMoved = true;
	} 
	else if((piece == WROOK) && (piecePosX == 7) && (piecePosY == 7)) {
		mWRook2HasMoved = true;
	} 
	else if((piece == BROOK) && (piecePosX == 7) && (piecePosY == 0)) {
		mBRook2HasMoved = true;
	}
	else if((piece == BROOK) && (piecePosX == 0) && (piecePosY == 0)) {
		mBRook1HasMoved = true;
	}
}

bool LBoardPVAI::checkPromotion(int x, int y, int piece) {
	bool promotion = false;
	if((piece == WPAWN) && (y == 0)) {
		mMap[y][x] = WQUEEN;
		promotion = true;
	}
	if((piece == BPAWN) && (y == 7)) {
		mMap[y][x] = BQUEEN;
		promotion = true;
	}
	return promotion;
}
		
bool LBoardPVAI::initMap() {
	
	CFURLRef appUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("map.map"), NULL, NULL);
	CFStringRef filePathRef = CFURLCopyPath(appUrlRef);
	const char* filePath = CFStringGetCStringPtr(filePathRef, kCFStringEncodingUTF8);
	
	std::ifstream map(filePath);
	
	CFRelease(filePathRef);
	CFRelease(appUrlRef);
	
	bool success = true;
	if(map.fail()) {
		printf( "Unable to load map file!\n" );
		success = false;
	}
	else {
		for(int y(0); y < SPL; y++) {
			for(int x(0); x < SPL; x++) {
				//Determines what kind of tile will be made
				int pieceType = -1;
				
				//Read piece from map file
				map >> pieceType;
				
				//If the was a problem in reading the map
				if(map.fail()) {
					//Stop loading map
					printf("Error loading map: Unexpected end of file!\n");
					success = false;
					break;
				}
				//If the number is a valid piece number
				if((pieceType >= 0) && (pieceType < TOTAL_PIECES)) {
					mMap[y][x] = pieceType;
				}
				//If we don't recognize the tile type
				else {
					//Stop loading map
					printf( "Error loading map: Invalid piece type at line %d element %d!\n", y, x);
					success = false;
					break;
				}
			}
		}
	}
	return success;
}

void LBoardPVAI::playVictorySound() const {
	if(Mix_PlayingMusic()) {
		Mix_FadeOutMusic(3000);
	}
	Mix_PlayChannel(-1, mCheckMate, 0 );
	while(Mix_Playing(-1) > 0) {
		SDL_Delay(16);
	}
	Mix_PlayChannel(-1, mVictory, 0 );
	while(Mix_Playing(-1) > 0) {
		SDL_Delay(16);
	}
}

void LBoardPVAI::playMoveSound() {
	if((!(mTookAPiece)) && (!(mIsCastling))) {
		//Play move sound 
		Mix_PlayChannel(-1, mPawnForward, 0);
	}
	else if(mTookAPiece) {
		//Play falling piece sound 
		Mix_PlayChannel(-1, mPieceFall, 0);
		mTookAPiece = false;
	}
	else if(mIsCastling) {
		//Play castling sound 
		Mix_PlayChannel(-1, mCastling, 0);
		mIsCastling = false;
	}
}

bool LBoardPVAI::pollCheck(const int map[SPL][SPL]) { 
	bool check = false;
	
	//check all black pieces on the map if black's turn
	//for each b piece, check its possible moves 
	//if one of these possible move pos contain 
	//a white King - check 
	if(!(mWhiteTurn)) {
		for(int y(0); y < SPL; y++) {
			for(int x(0); x < SPL; x++) {
				if(map[y][x] < WBISHOP) {
					//Select piece from map otherwise selected piece will be the one 
					//that just moved
					int pieceType = map[y][x];
					int pieceYPos = y;
					int pieceXPos = x;
					//fill possible moves
					showLegalMove(map, pieceType, pieceXPos, pieceYPos);
					for(int i(0); i < mHighlightedTileYPos.size(); i++) {
						if(map[mHighlightedTileYPos[i]][mHighlightedTileXPos[i]] == WKING) {
							mCheckStatus = WCHECK;
							check = true;
							return check;
						}
					}
				}
			}
		}
	}
	//check white
	else {
		for(int y(0); y < SPL; y++) {
			for(int x(0); x < SPL; x++) {
				if((map[y][x] >= WBISHOP) && (map[y][x] < EMPTY)) {
					//Select piece from map otherwise selected piece will be the one 
					//that just moved
					int pieceType = map[y][x];
					int pieceYPos = y;
					int pieceXPos = x;
					//fill possible moves
					showLegalMove(map, pieceType, pieceXPos, pieceYPos);
					for(int i(0); i < mHighlightedTileYPos.size(); i++) {
						if(map[mHighlightedTileYPos[i]][mHighlightedTileXPos[i]] == BKING) {
							mCheckStatus = BCHECK;
							check = true;
							return check;
						}
					}
				}
			}
		}
	}
	if(!(check)) mCheckStatus = NO_CHECK;
	return check;
}

//returns true if the move you're about to play checks your own King
//returns false if the move you're about to play does not check your own King
bool LBoardPVAI::pollDiscoverAttack(const int mapCopy[SPL][SPL], const int piece, const int desX, const int desY, const int srcX, const int srcY) { 
	
	//default false, no check
	bool check = false;

	//make a local copy of the map to preview the move
	//if the move checks its own color - pollDiscoverAttack 
	//returns true, error sound is played + no move
	int map[SPL][SPL] = {0};
	for(int a(0); a < SPL; a++) {
		for(int b(0); b < SPL; b++) {
			map[a][b] = mapCopy[a][b];
		}
	}
	
	//move selected piece to destination in the local map
	map[desY][desX] = piece;
	//empty source cell 
	map[srcY][srcX] = EMPTY;
	
	//check all white pieces on the map if black's turn
	//for each white piece, check its possible moves 
	//if one of these possible move pos contain 
	//a black King - check 
	if(!(mWhiteTurn)) {
		for(int y(0); y < SPL; y++) {
			for(int x(0); x < SPL; x++) {
				if((map[y][x] >= WBISHOP) && (map[y][x] < EMPTY)) {
					//Select piece from map otherwise selected piece will be the one 
					//that just moved
					int pieceType = map[y][x];
					int selectedPieceYPos = y;
					int selectedPieceXPos = x;
					//fill possible moves
					showLegalMove(map, pieceType, selectedPieceXPos, selectedPieceYPos, true);
					//check if on one if these moves there is a BKING, in which case it checks 
					//its own color and returns true
					for(int i(0); i < mHighlightedTileYPos.size(); i++) {
						if(map[mHighlightedTileYPos[i]][mHighlightedTileXPos[i]] == BKING) {
							check = true;
							return check;
						}
					}
				}
			}
		}
	}
	//White turn check all Black piece possible moves
	else {
		for(int y(0); y < SPL; y++) {
			for(int x(0); x < SPL; x++) {
				if(map[y][x] < WBISHOP) {
					//Select piece from map otherwise selected piece will be the one 
					//that just moved
					int pieceType = map[y][x];
					int selectedPieceYPos = y;
					int selectedPieceXPos = x;
					//fill possible moves
					showLegalMove(map, pieceType, selectedPieceXPos, selectedPieceYPos, true);
					//check if on one if these moves there is a WKING, in which case it checks 
					//its own color and returns true
					for(int i(0); i < mHighlightedTileYPos.size(); i++) {
						if(map[mHighlightedTileYPos[i]][mHighlightedTileXPos[i]] == WKING) {
							check = true;
							return check;
						}
					}
				}
			}
		}
	}
	return check; //default false
}

void LBoardPVAI::renderTimer() {
	TTF_Font* font64 = TTF_OpenFont("../Resources/valentin.ttf", 64);
	//only render time if TL_YES == 1 in mSettingsTable
	if(mSettingsTable[TL_YES] == 1) {
		SDL_Color black = {0,0,0,0xFF};
		
		int wtime = mTimeLimit - (mWhiteTimer.getTicks() / 1000);
		int wminutes = wtime / 60;
		int ws = wtime % 60;
		
		if(wtime < 0) mWhiteTimerRanOut = true;
		
		//In memory text stream
		std::stringstream whiteTimeText;
		if(ws > 9) {
			whiteTimeText << std::to_string(wminutes) + ":" + std::to_string(ws);
		}
		else whiteTimeText << std::to_string(wminutes) + ":" + "0" + std::to_string(ws);
		
		mWhiteTimerTexture->loadFromRenderedText(font64, whiteTimeText.str().c_str(), black);
		mWhiteTimerTexture->render(0,SCREEN_HEIGHT - OFFSET);
		TTF_CloseFont(font64);
	}
}

void LBoardPVAI::changeTurn() {
	if(mWhiteTurn) {
		mWhiteTurn = false;
		pauseWhiteTimer();
	}
	else {
		mWhiteTurn = true;
		unpauseWhiteTimer();
	}
}

//Piece has just moved and the enemy King is checked
//checkMate() determines if the enemy King can be defended
//if returns true then there is checkMate
//false if one possible move from opponent cancels the check
bool LBoardPVAI::checkMate() {
	//win condition
	bool checkMate = true;
	
	//local copy of the mMap to not mess with anything
	int map[SPL][SPL] = {0};
	for(int a(0); a < SPL; a++) {
		for(int b(0); b < SPL; b++) {
			map[a][b] = mMap[a][b];
		}
	}
	
	//White turn
	if(mWhiteTurn) {
		for(int y(0); y < SPL; y++) {
			for(int x(0); x < SPL; x++) {
				//check each black piece
				if(map[y][x] < WBISHOP) {
					int pieceType = map[y][x];
					int selectedPieceYPos = y;
					int selectedPieceXPos = x;
					//fill possible moves
					showLegalMove(map, pieceType, selectedPieceXPos, selectedPieceYPos);
					//local copy of hilghlighted tile pos as it
					//is going to be messed with when checking if 
					//the possible move checks its own king
					std::vector<int> destPosX;
					std::vector<int> destPosY;
					//only fill if there's something to fill
					if(mHighlightedTileXPos.size() > 0) {
						for(int i(0); i < mHighlightedTileXPos.size(); i++) {
							destPosX.push_back(mHighlightedTileXPos[i]);
							destPosY.push_back(mHighlightedTileYPos[i]);
						}
					}
					//for each possible move
					for(int i(0); i < destPosX.size(); i++) {
						//copy of local copy of the map to preview the moves
						int mapCopy[SPL][SPL] = {0};
						for(int a(0); a < SPL; a++) {
							for(int b(0); b < SPL; b++) {
								mapCopy[a][b] = map[a][b];
							}
						}
						//if pollDiscoverAttack returns false then
						//the move in the copy canceled the check
						if(!(pollDiscoverAttack(mapCopy, pieceType, destPosX[i], destPosY[i], selectedPieceXPos, selectedPieceYPos))) {
							checkMate = false;
							return checkMate;
						}
					}
				}
			}
		}
	}
	//Black turn
	else {
		for(int y(0); y < SPL; y++) {
			for(int x(0); x < SPL; x++) {
				//check each white piece
				if((map[y][x] >= WBISHOP) && (map[y][x] < EMPTY)) {
					int pieceType = map[y][x];
					int selectedPieceYPos = y;
					int selectedPieceXPos = x;
					mHighlightedTileXPos.clear();
					mHighlightedTileYPos.clear();
					
					//fill possible moves
					showLegalMove(map, pieceType, selectedPieceXPos, selectedPieceYPos);
					
					//local copy of hilghlighted tile pos as it
					//is going to be messed with when checking if 
					//the possible move checks its own king
					std::vector<int> destPosX;
					std::vector<int> destPosY;
					//only fill if there's something to fill
					if(mHighlightedTileXPos.size() > 0) {
						for(int i(0); i < mHighlightedTileXPos.size(); i++) {
							destPosX.push_back(mHighlightedTileXPos[i]);
							destPosY.push_back(mHighlightedTileYPos[i]);
						}
					}
					//for each possible move
					for(int i(0); i < destPosX.size(); i++) {
						//copy of local copy of the map to preview the moves
						int mapCopy[SPL][SPL] = {0};
						for(int a(0); a < SPL; a++) {
							for(int b(0); b < SPL; b++) {
								mapCopy[a][b] = map[a][b];
							}
						}
						//if pollCheckMate returns false then
						//the move in the copy canceled the check
						mWhiteTurn = true;
						if(!(pollDiscoverAttack(mapCopy, pieceType, destPosX[i], destPosY[i], selectedPieceXPos, selectedPieceYPos))) {
							checkMate = false;
							return checkMate;
						}
						mWhiteTurn = false;
					}
				}
			}
		}
	}
	//default true 
	return checkMate; 
}


bool LBoardPVAI::pollVictory() const {
	return mGameOver;
}

bool LBoardPVAI::pollTimeOut() {
	if(mWhiteTimerRanOut) {
		if(Mix_PlayingMusic()) {
			Mix_FadeOutMusic(500);
		}
		Mix_PlayChannel(-1, mDefeat, 0);
		while(Mix_Playing(-1) > 0) {
			SDL_Delay(16);
		}
		return 1;
	}
	return 0;
}

void LBoardPVAI::blackTurn() {

	//for each black piece on the local map[][]
	//list all possible moves srcX|Y desX|Y
	for(int y(0); y < SPL; y++) {
		for(int x(0); x < SPL; x++) {
			if(mMap[y][x] < WBISHOP) {
				
				//Select piece from map otherwise selected piece will be the one 
				//that just moved
				int pieceType = mMap[y][x];
				int srcPieceYPos = y;
				int srcPieceXPos = x;

				//fill possible moves
				showLegalMove(mMap, pieceType, srcPieceXPos, srcPieceYPos);
				
				//local copy of hilghlighted tile pos as it
				//is going to be messed with when evaluating
				//the possible move
				std::vector<int> destPosX;
				std::vector<int> destPosY;
				//only fill if there's something to fill
				if(mHighlightedTileXPos.size() > 0) {
					for(int i(0); i < mHighlightedTileXPos.size(); i++) {
						destPosX.push_back(mHighlightedTileXPos[i]);
						destPosY.push_back(mHighlightedTileYPos[i]);
					}
				}
				//for each possible move
				for(int i(0); i < destPosX.size(); i++) {
					//copy of local copy of the map to preview the moves
					int mapCopy[SPL][SPL] = {0};
					for(int a(0); a < SPL; a++) {
						for(int b(0); b < SPL; b++) {
							mapCopy[a][b] = mMap[a][b];
						}
					}
					//evaluate the possible move
					//and fill black vectors accordingly
					eveluateBlackMove(mapCopy, pieceType, destPosX[i], destPosY[i], srcPieceXPos, srcPieceYPos);
				}
			}
		}
	}
	
	//actually move
	blackMove();
	//reset it to false;
	mBlackCanCastle = false;
}

void LBoardPVAI::renderBeforeBlackPlays() {
	//paint screen white
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);
	
	gBackgroundTexture.render();
	
	//display tiles
	renderTile();
	//display Pieces 
	renderPieces();
	//display timer
	renderTimer();
	//display score
	renderScore();
	//display deadpieces
	renderDeadPieces();
	
	if(isPaused()) {
		renderPause();
	}
	SDL_RenderPresent(gRenderer);
}

void LBoardPVAI::blackMove() {
	bool canMove = false;
	bool hasMoved = false;
	if(mSSSrcMoveX.size() > 0) {
		int r = rand() % mSSSrcMoveX.size();
		int piece = mMap[mSSSrcMoveY[r]][mSSSrcMoveX[r]];
		if(!(hasMoved)) {
			if((mSSDesMoveX[r] < 8) && (mSSDesMoveY[r] < 8)) {
				if(mMap[mSSDesMoveY[r]][mSSDesMoveX[r]] != EMPTY) {
					mPieceButtons.resize(mPieceButtons.size() - 1);
					int fallenPiece = mMap[mSSDesMoveY[r]][mSSDesMoveX[r]];
					fillDeadPieceTab(fallenPiece);
					mTookAPiece = true;
				}
				if(mBlackCanCastle) {
					if(piece == BKING) {
						if(mSSSrcMoveX[r] - 2 == mSSDesMoveX[r]) {
							mMap[mSSSrcMoveY[r]][mSSSrcMoveX[r] - 1] = BROOK;
							mMap[0][0] = EMPTY;
						}
						else if(mSSSrcMoveX[r] + 2 == mSSDesMoveX[r]) {
							mMap[mSSSrcMoveY[r]][mSSSrcMoveX[r] + 1] = BROOK;
							mMap[0][7] = EMPTY;
						}
					}
				}
				mMap[mSSDesMoveY[r]][mSSDesMoveX[r]] = piece;
				mMap[mSSSrcMoveY[r]][mSSSrcMoveX[r]] = EMPTY;
				printf("Turn %d %s moves from %d,%d to %d,%d" , mTurn, mPieceStr[piece].c_str(), mSSSrcMoveY[r], mSSSrcMoveX[r], mSSDesMoveY[r], mSSDesMoveX[r]);
				hasMoved = true;
			}
		}
		else {
			printf("Unexpected value for %s srcY: %d srcX: %d desY: %d desX: %d", mPieceStr[piece].c_str(), mSSSrcMoveY[r], mSSSrcMoveX[r], mSSDesMoveY[r], mSSDesMoveX[r]);
		}
		
		//turn Pawn into Queen if possible
		checkPromotion(mSSDesMoveX[r], mSSDesMoveY[r], piece);
		setCastlingBools(mSSSrcMoveX[r], mSSSrcMoveY[r], piece);
		setEnPassantVar(piece, mSSSrcMoveX[r], mSSSrcMoveY[r], mSSDesMoveX[r], mSSDesMoveY[r]);
		
		canMove = true;
	}
	else if(mSSrcMoveX.size() > 0) {
		int r = rand() % mSSrcMoveX.size();
		int piece = mMap[mSSrcMoveY[r]][mSSrcMoveX[r]];
		if(!(hasMoved)) {
			if((mSDesMoveX[r] < 8) && (mSDesMoveY[r] < 8)) {
				if(mMap[mSDesMoveY[r]][mSDesMoveX[r]] != EMPTY) {
					mPieceButtons.resize(mPieceButtons.size() - 1);
					mTookAPiece = true;
					int fallenPiece = mMap[mSDesMoveY[r]][mSDesMoveX[r]];
					fillDeadPieceTab(fallenPiece);
				}
			
				mMap[mSDesMoveY[r]][mSDesMoveX[r]] = piece;
				mMap[mSSrcMoveY[r]][mSSrcMoveX[r]] = EMPTY;
				printf("Turn %d %s moves from %d,%d to %d,%d" , mTurn, mPieceStr[piece].c_str(), mSSrcMoveY[r], mSSrcMoveX[r], mSDesMoveY[r], mSDesMoveX[r]);
				hasMoved = true;
			}
		}
		else {
			printf("Unexpected value for %s move srcY: %d srcX: %d desY: %d desX: %d",mPieceStr[piece].c_str(), mSSrcMoveY[r], mSSrcMoveX[r], mSDesMoveY[r], mSDesMoveX[r]);
		}
		
		//turn Pawn into Queen if possible
		checkPromotion(mSDesMoveX[r], mSDesMoveY[r], piece);
		setCastlingBools(mSSrcMoveX[r], mSSrcMoveY[r], piece);
		setEnPassantVar(piece, mSSrcMoveX[r], mSSrcMoveY[r], mSDesMoveX[r], mSDesMoveY[r]);
		
		canMove = true;
	}
	else if(mEPSrcMoveX.size() > 0) {
		int r = rand() % mEPSrcMoveX.size();
		int piece = mMap[mEPSrcMoveY[r]][mEPSrcMoveX[r]];
		if(!(hasMoved)) {
			if((mEPDesMoveX[r] < 8) && (mEPDesMoveY[r] < 8)) {
				//move piece
				mMap[mEPDesMoveY[r]][mEPDesMoveX[r]] = piece;
				
				//take enemy en passant
				int fallenPiece = mMap[mEPDesMoveY[r] - 1][mEPDesMoveX[r]];
				fillDeadPieceTab(fallenPiece);
				mMap[mEPDesMoveY[r] - 1][mEPDesMoveX[r]] = EMPTY;
				mPieceButtons.resize(mPieceButtons.size() - 1);
				mTookAPiece = true;
				
				//empty src
				mMap[mEPSrcMoveY[r]][mEPSrcMoveX[r]] = EMPTY;
				printf("Turn %d %s moves from %d,%d to %d,%d" , mTurn, mPieceStr[piece].c_str(), mEPSrcMoveY[r], mEPSrcMoveX[r], mEPDesMoveY[r], mEPDesMoveX[r]);
				hasMoved = true;
			}
		}
		else {
			printf("Unexpected value for %s move srcY: %d srcX: %d desY: %d desX: %d",mPieceStr[piece].c_str(), mEPSrcMoveY[r], mEPSrcMoveX[r], mEPDesMoveY[r], mEPDesMoveX[r]);
		}
		
		//turn Pawn into Queen if possible
		checkPromotion(mEPDesMoveX[r], mEPDesMoveY[r], piece);
		setCastlingBools(mEPSrcMoveX[r], mEPSrcMoveY[r], piece);
		setEnPassantVar(piece, mEPSrcMoveX[r], mEPSrcMoveY[r], mEPDesMoveX[r], mEPDesMoveY[r]);
		
		canMove = true;
	}
	else if(mASrcMoveX.size() > 0) {
		int r = rand() % mASrcMoveX.size();
		int piece = mMap[mASrcMoveY[r]][mASrcMoveX[r]];
		
		if((mADesMoveX[r] < 8) && (mADesMoveY[r] < 8)) {
			if(!(hasMoved)) {
				if(mMap[mADesMoveY[r]][mADesMoveX[r]] != EMPTY) {
					mPieceButtons.resize(mPieceButtons.size() - 1);
					mTookAPiece = true;
					int fallenPiece = mMap[mADesMoveY[r]][mADesMoveX[r]];
					fillDeadPieceTab(fallenPiece);
				}
				mMap[mADesMoveY[r]][mADesMoveX[r]] = piece;
				mMap[mASrcMoveY[r]][mASrcMoveX[r]] = EMPTY;
				printf("Turn %d %s moves from %d,%d to %d,%d" , mTurn, mPieceStr[piece].c_str(), mASrcMoveY[r], mASrcMoveX[r], mADesMoveY[r], mADesMoveX[r]);
				hasMoved = true;
			}
		}
		else {
			printf("Unexpected value for %s move srcY: %d srcX: %d desY: %d desX: %d",mPieceStr[piece].c_str() ,mASrcMoveY[r], mASrcMoveX[r], mADesMoveY[r], mADesMoveX[r]);
		}
		//turn Pawn into Queen if possible
		checkPromotion(mADesMoveX[r], mADesMoveY[r], piece);
		setCastlingBools(mASrcMoveX[r], mASrcMoveY[r], piece);
		setEnPassantVar(piece, mASrcMoveX[r], mASrcMoveY[r], mADesMoveX[r], mADesMoveY[r]);

		
		canMove = true;
	}
	else if(mBSrcMoveX.size() > 0) {
		int r = rand() % mBSrcMoveX.size();
		int piece = mMap[mBSrcMoveY[r]][mBSrcMoveX[r]];
	
		if((mBDesMoveX[r] < 8) && (mBDesMoveY[r] < 8)) {
			if(!(hasMoved)) {
				if(mMap[mBDesMoveY[r]][mBDesMoveX[r]] != EMPTY) {
					mPieceButtons.resize(mPieceButtons.size() - 1);
					mTookAPiece = true;
					int fallenPiece = mMap[mBDesMoveY[r]][mBDesMoveX[r]];
					fillDeadPieceTab(fallenPiece);
				}
				mMap[mBDesMoveY[r]][mBDesMoveX[r]] = piece;
				mMap[mBSrcMoveY[r]][mBSrcMoveX[r]] = EMPTY;
				printf("Turn %d %s moves from %d,%d to %d,%d" , mTurn, mPieceStr[piece].c_str(), mBSrcMoveY[r], mBSrcMoveX[r], mBDesMoveY[r], mBDesMoveX[r]);
				hasMoved = true;
			}
		}
		else {
			printf("Unexpected value for %s move srcY: %d srcX: %d desY: %d desX: %d",mPieceStr[piece].c_str(), mBSrcMoveY[r], mBSrcMoveX[r], mBDesMoveY[r], mBDesMoveX[r]);
		}
		//turn Pawn into Queen if possible
		checkPromotion(mBDesMoveX[r], mBDesMoveY[r], piece);
		setCastlingBools(mBSrcMoveX[r], mBSrcMoveY[r], piece);
		setEnPassantVar(piece, mBSrcMoveX[r], mBSrcMoveY[r], mBDesMoveX[r], mBDesMoveY[r]);

		
		canMove = true;
	}
	else if(mDSrcMoveX.size() > 0) {
		int r = rand() % mDSrcMoveX.size();
		int piece = mMap[mDSrcMoveY[r]][mDSrcMoveX[r]];
		if(!(hasMoved)) {
			if((mDDesMoveX[r] < 8) && (mDDesMoveY[r] < 8)) {
				if(mMap[mDDesMoveY[r]][mDDesMoveX[r]] != EMPTY) {
					mPieceButtons.resize(mPieceButtons.size() - 1);
					mTookAPiece = true;
					int fallenPiece = mMap[mDDesMoveY[r]][mDDesMoveX[r]];
					fillDeadPieceTab(fallenPiece);
				}
				mMap[mDDesMoveY[r]][mDDesMoveX[r]] = piece;
				mMap[mDSrcMoveY[r]][mDSrcMoveX[r]] = EMPTY;
				printf("Turn %d %s moves from %d,%d to %d,%d" , mTurn, mPieceStr[piece].c_str(), mDSrcMoveY[r], mDSrcMoveX[r], mDDesMoveY[r], mDDesMoveX[r]);
				hasMoved = true;
			}
		}
		else {
			printf("Unexpected value for %s move srcY: %d srcX: %d desY: %d desX: %d",mPieceStr[piece].c_str(), mDSrcMoveY[r], mDSrcMoveX[r], mDDesMoveY[r], mDDesMoveX[r]);

		}
		//turn Pawn into Queen if possible
		checkPromotion(mDDesMoveX[r], mDDesMoveY[r], piece);
		setCastlingBools(mDSrcMoveX[r], mDSrcMoveY[r], piece);
		setEnPassantVar(piece, mDSrcMoveX[r], mDSrcMoveY[r], mDDesMoveX[r], mDDesMoveY[r]);
		canMove = true;
	}
	
	if(!(canMove)) {
		mGameOver = true;
	}
}


void LBoardPVAI::setEnPassantVar(const int piece, const int srcX, const int srcY , const int desX, const int desY) {
	if(mEnPassantTurn) mEnPassantTurn = false;
	//en passant
	mLastMovedPiece = piece;
	mLastMovedPieceXSrc = srcX;
	mLastMovedPieceYSrc = srcY;
	mLastMovedPieceXDes = desX;
	mLastMovedPieceYDes = desY; 
}


void LBoardPVAI::eveluateBlackMove(int map[SPL][SPL], const int piece, const int desX, const int desY, const int srcX, const int srcY) {
	
	int trade (0);
	const int dx = desX,
			dy = desY,
			sx = srcX,
			sy = srcY;
	//if dest move is an enemy
	if((map[desY][desX] >= WBISHOP) && (map[desY][desX] < EMPTY)) {
		//check if move will check its own King
		//if yes the move won't be stored
		if(!(pollDiscoverAttack(map, piece, desX, desY, srcX, srcY))) {
			
			//calculate trade
			int whitePiece = map[desY][desX];
			//trade should have a value between 1 and 20
			trade += pieceValue(whitePiece);
			
			//do the move in local map
			map[desY][desX] = piece;
			map[srcY][srcX] = EMPTY;
			
			//true white can take
			if(checkIfWhiteCanTake(map, piece, desX, desY, &trade)) {
				int piece2 = map[desY][desX];
				if(checkIfBlackCanTake(map, piece2, desX, desY, &trade)) {
					int piece3 = map[desY][desX];
					if(checkIfWhiteCanTake(map, piece3, desX, desY, &trade)) {
						int piece4 = map[desY][desX];
						if(checkIfBlackCanTake(map, piece4, desX, desY, &trade)) {
							if(pollCheck(map)) {
								trade +=10;
							}
							fillBlackVectors(trade, dx, dy, sx, sy);
						}
						//black cant take
						else {
							fillBlackVectors(trade, dx, dy, sx, sy);
						}
					}
					//white cant take
					else {
						if(pollCheck(map)) {
							trade +=10;
						}
						fillBlackVectors(trade, dx, dy, sx, sy);
					}
				}
				//black cnat take
				else fillBlackVectors(trade, dx, dy, sx, sy);
			}
			//white can't take, check trade value and 
			//store move according to that value in concording vectors
			else {
				if(pollCheck(map)) {
					trade +=10;
				}
				fillBlackVectors(trade, dx, dy, sx, sy);
			}
		}
		//move checks its own King
		else {
			// do nothing
		}
	}
	//if black can potentially castle
	else if(mBlackCanCastle) {
		if(piece == BKING) {
			if((srcX + 2 == desX) || (srcX - 2 == desX)) {
				trade += 20;
				fillBlackVectors(trade, dx, dy, sx, sy);
			}
		}
	}
	
	else if(mEnPassantTurn) {
		if(piece == BPAWN) {
			if((map[desY][desX] == EMPTY) && (desX != srcX)) {
				mEPSrcMoveX.push_back(srcX);
				mEPSrcMoveY.push_back(srcY);
				mEPDesMoveX.push_back(desX);
				mEPDesMoveY.push_back(desY);
				mEnPassantTurn = false;
			}
		}
	}
	
	//if dest pos is EMPTY
	else if(map[desY][desX] == EMPTY) {
		
		//check that the move does not check own King
		if(!(pollDiscoverAttack(map, piece, desX, desY, srcX, srcY))) {
			
			//do the move in local map
			map[desY][desX] = piece;
			map[srcY][srcX] = EMPTY;
			
			

			//check if white can take
			if(checkIfWhiteCanTake(map, piece, desX, desY, &trade)) {
				int piece2 = map[desY][desX];
				if(checkIfBlackCanTake(map, piece2, desX, desY, &trade)) {
					int piece3 = map[desY][desX];
					if(checkIfWhiteCanTake(map, piece, desX, desY, &trade)) {
						int piece4 = map[desY][desX];
						if(checkIfBlackCanTake(map, piece2, desX, desY, &trade)) {
							if(pollCheck(map)) {
								trade +=10;
							}
							fillBlackVectors(trade,  dx, dy, sx, sy);
						}
						//black cant take
						else {
							fillBlackVectors(trade,  dx, dy, sx, sy);
						}
					}
					//white cant take
					else {
						if(pollCheck(map)) {
							trade +=10;
						}
						fillBlackVectors(trade,  dx, dy, sx, sy);
					}
				}
				//black cant take
				else {
					fillBlackVectors(trade,  dx, dy, sx, sy);
				}
			}
			//white can't take, check trade value and 
			//store move according to that value in concording vectors
			else {
				if(pollCheck(map)) {
					trade +=10;
				}
				fillBlackVectors(trade,  dx, dy, sx, sy);
			}
		}
		//move checks own king
		else {
			//do nothing
		}
	}
}

bool LBoardPVAI::checkIfWhiteCanTake(int map[SPL][SPL], const int piece, const int desX, const int desY, int* trade) {
	//if desX|Y is encountered within any possible move
	//turn canTake to true and return canTake
	//else return default false at the end
	bool canTake = false;
	
	for(int y(0); y < SPL; y++) {
		for(int x(0); x < SPL; x++) {
			if((map[y][x] >= WBISHOP) && (map[y][x] < EMPTY)) {
				int pieceType = map[y][x];
				int selectedPieceYPos = y;
				int selectedPieceXPos = x;
				//fill possible moves
				showLegalMove(map, pieceType, selectedPieceXPos, selectedPieceYPos);
				//check if on one if these moves there is a BKING, in which case it checks 
				//its own color and returns true
				for(int i(0); i < mHighlightedTileYPos.size(); i++) {
					if((mHighlightedTileYPos[i] == desY) && (mHighlightedTileXPos[i] == desX)) {
						//check if white moves discovers white king, if yes, dnt do move
						//change turn for white to be checked
						mWhiteTurn = true;
						if(!(pollDiscoverAttack(map, pieceType, desX, desY, selectedPieceXPos, selectedPieceYPos))) {
							canTake = true;
							int blackPiece = pieceValue(piece);
							//refresh trade value deducting the piece black has lost in the trade
							*trade -= blackPiece;
							//refresh the map
							map[desY][desX] = pieceType;
							map[selectedPieceYPos][selectedPieceXPos] = EMPTY;
							mWhiteTurn = false;
							return canTake;
						}
						else {
							mWhiteTurn = false;
						}
					}
				}
			}
		}
	}
	//default false
	return canTake;
}
	
bool LBoardPVAI::checkIfBlackCanTake(int map[SPL][SPL], const int piece, const int desX, const int desY, int* trade) {
	//if desX|Y is encountered within any possible move
	//turn canTake to true and return canTake
	//else return default false at the end
	bool canTake = false;
	
	for(int y(0); y < SPL; y++) {
		for(int x(0); x < SPL; x++) {
			if(map[y][x] < WBISHOP) {
				int pieceType = map[y][x];
				int selectedPieceYPos = y;
				int selectedPieceXPos = x;
				//fill possible moves
				showLegalMove(map, pieceType, selectedPieceXPos, selectedPieceYPos);
				//check if on one if these moves there is a BKING, in which case it checks 
				//its own color and returns true
				for(int i(0); i < mHighlightedTileYPos.size(); i++) {
					if((mHighlightedTileYPos[i] == desY) && (mHighlightedTileXPos[i] == desX)) {
						//check if black move discovers black king, if yes, dnt do move
						//no need to change turn for black to be checked
						mWhiteTurn = false;
						if(!(pollDiscoverAttack(map, pieceType, desX, desY, selectedPieceXPos, selectedPieceYPos))) {
							canTake = true;
							int whitePiece = pieceValue(piece);
							//refresh trade value deducting the piece black has lost in the trade
							*trade += whitePiece;
							//refresh the map
							map[desY][desX] = pieceType;
							map[selectedPieceYPos][selectedPieceXPos] = EMPTY;
							return canTake;
						}
					}
				}
			}
		}
	}
	//default false
	return canTake;
}

void LBoardPVAI::fillBlackVectors(int tradeValue, const int desX, const int desY, const int srcX, const int srcY) {
	//if black can take again, evaluate trade value
	if(tradeValue >= 12) {
		mSSSrcMoveX.push_back(srcX);
		mSSSrcMoveY.push_back(srcY);
		mSSDesMoveX.push_back(desX);
		mSSDesMoveY.push_back(desY);
	}
	else if((tradeValue > 6) &&( tradeValue <= 11)) {
		mSSrcMoveX.push_back(srcX);
		mSSrcMoveY.push_back(srcY);
		mSDesMoveX.push_back(desX);
		mSDesMoveY.push_back(desY);
	}
	// < 3 then good move
	else if((tradeValue >= 3) && (tradeValue <= 5)) {
		mASrcMoveX.push_back(srcX);
		mASrcMoveY.push_back(srcY);
		mADesMoveX.push_back(desX);
		mADesMoveY.push_back(desY);
	}
	//okay move
	else if((tradeValue >= 1) && (tradeValue < 3)) {
		mBSrcMoveX.push_back(srcX);
		mBSrcMoveY.push_back(srcY);
		mBDesMoveX.push_back(desX);
		mBDesMoveY.push_back(desY);
	}
	//bad move
	else if(tradeValue <= 0) {
		mDSrcMoveX.push_back(srcX);
		mDSrcMoveY.push_back(srcY);
		mDDesMoveX.push_back(desX);
		mDDesMoveY.push_back(desY);
	}
}

int LBoardPVAI::pieceValue(int const pieceType) const {
	switch(pieceType) {
		case BBISHOP: 
			return BISHOP_VALUE;
		case BKING:
			return KING_VALUE;
		case BKNIGHT:
			return KNIGHT_VALUE;
		case BPAWN:
			return PAWN_VALUE;
		case BQUEEN:
			return QUEEN_VALUE;
		case BROOK:
			return ROOK_VALUE;
		case WBISHOP:
			return BISHOP_VALUE;
		case WKING:
			return KING_VALUE;
		case WKNIGHT:
			return KNIGHT_VALUE;
		case WPAWN:
			return PAWN_VALUE;
		case WQUEEN:
			return QUEEN_VALUE;
		case WROOK:
			return ROOK_VALUE;
		default:
			return 0;
	}
}


void LBoardPVAI::clearBlackVectors() {	
	mSSSrcMoveX.clear();
	mSSSrcMoveY.clear();
	
	mSSDesMoveX.clear();
	mSSDesMoveY.clear();
	
	mEPSrcMoveX.clear();
	mEPSrcMoveY.clear();
	
	mEPDesMoveX.clear();
	mEPDesMoveY.clear();
	
	mSSrcMoveX.clear();
	mSSrcMoveY.clear();
	
	mSDesMoveX.clear();
	mSDesMoveY.clear();
	
	mASrcMoveX.clear();
	mASrcMoveY.clear();
	
	mADesMoveX.clear();
	mADesMoveY.clear();
	
	mBSrcMoveX.clear();
	mBSrcMoveY.clear();
	
	mBDesMoveX.clear();
	mBDesMoveY.clear();
	
	mDSrcMoveX.clear();
	mDSrcMoveY.clear();
	
	mDDesMoveX.clear();
	mDDesMoveY.clear();
}

void LBoardPVAI::checkRemainingPieces() {
	bool blackLose = true;
	bool whiteLose = true;
	
	for(int y(0); y < SPL; y++) {
		for(int x(0); x < SPL; x++) {
			if(mMap[y][x] < WBISHOP) {
				if(mMap[y][x] != BKING) {
					blackLose = false;
				} 
			}
			else if((mMap[y][x] >= WBISHOP) && (mMap[y][x] < EMPTY)) {
				if(mMap[y][x] != WKING) {
					whiteLose = false;
				} 
			}
		}
	}
	if((blackLose) || (whiteLose)) mGameOver = true; 
}

void LBoardPVAI::renderScore() {
	TTF_Font* font64 = TTF_OpenFont("../Resources/valentin.ttf", 64);
	//sum up each score
	int whiteScore(0);
	int blackScore(0);
	const int MAX = 60;
	for(int y(0); y < SPL; y++) {
		for(int x(0); x < SPL; x++) {
			//black piece
			if((mMap[y][x] < WBISHOP)) {
				int piece = mMap[y][x];
				blackScore += pieceValue(piece);
			}
			//white piece
			else if((mMap[y][x] >= WBISHOP) && (mMap[y][x] < EMPTY)) {
				int piece = mMap[y][x];
				whiteScore += pieceValue(piece);
			}
		}
	}
	
	int a(blackScore);
	blackScore = MAX - whiteScore;
	whiteScore = MAX - a;
	
	SDL_Color black = {0,0,0,0xFF};
	std::stringstream whiteScoreStr;
	std::stringstream blackScoreStr;
	
	whiteScoreStr << std::to_string(whiteScore);
	blackScoreStr << std::to_string(blackScore);
	
	
	mWhiteScoreTexture->loadFromRenderedText(font64, whiteScoreStr.str().c_str(), black);
	mBlackScoreTexture->loadFromRenderedText(font64, blackScoreStr.str().c_str(), black);
	
	if(mSettingsTable[TL_NO]) {
		mWhiteScoreTexture->render(0, SCREEN_HEIGHT - 64);
		mBlackScoreTexture->render(0, 0);
	}
	else {
		mWhiteScoreTexture->render(OFFSET * 3, SCREEN_HEIGHT - 64);
		mBlackScoreTexture->render(OFFSET * 3, 0);
	}
	
	TTF_CloseFont(font64);
}

void LBoardPVAI::renderDeadPieces() {
	int whiteOffset = 0;
	int blackOffset = 0;
	int whitePosX = OFFSET * 4;
	int whitePosY = (OFFSET * 9);
	int blackPosX = OFFSET * 4;
	int blackPosY = 0;
	for(int i(0); i < EMPTY; i++) {
		//black side
		if(mDeadWhitePiece[i] != 0) {
			for(int y(0); y < mDeadWhitePiece[i]; y++) {
				//make a second line once 8 elements
				if(blackOffset >= 32 * 8) {
					blackOffset = 0;
					blackPosY = 32;
				}
				mMiniPieceTexture->render(blackPosX + blackOffset, blackPosY, &mMiniPieceClip[i]);
				blackOffset += 32;
			}
		}
		//white side
		if(mDeadBlackPiece[i] != 0) {
			for(int y(0); y < mDeadBlackPiece[i]; y++) {
				//make a second line once 8 elements
				if(whiteOffset >= 32 * 8) {
					whiteOffset = 0;
					whitePosY = (OFFSET * 9) + 32;
				}
				mMiniPieceTexture->render(whitePosX + whiteOffset, whitePosY, &mMiniPieceClip[i]);
				whiteOffset += 32;
			}
		}
	}
}

void LBoardPVAI::fillDeadPieceTab(const int fallenPiece) {
	if(mWhiteTurn) {
		mDeadBlackPiece[fallenPiece]++;
	}
	else {
		mDeadWhitePiece[fallenPiece]++;
	}
}