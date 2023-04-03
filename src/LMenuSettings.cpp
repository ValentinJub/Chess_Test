/*
LMenu.cpp
by Valentin
--------------
Methods for LMenu class

*/

#include "LMenuSettings.h"

extern SDL_Renderer* gRenderer;
extern LTexture gBackgroundTexture;

LMenu::LMenu(){
    mFont = NULL;
    mMenuTextTextures = new LTexture;
    mPieceTheme1Texture = new LTexture;
    mPieceTheme2Texture = new LTexture;
    for(int i(0); i < TOTAL_CLICKABLE_ITEMS; i++) {
        mButtons[i] = new LButton;
    }
    //open font
    initFont();
    //fill text menu into str[] from 2 txt files
    initMenuStrings();
    //set settingsTable to default values
    initCurrentItemList();
    
    loadPieceThemeTextures();
    
    renderTextureFromTextLeft();
    renderTextureFromTextRight();
    
    mSound = loadChunk("../SoundEffects/START.wav");
    
    //texture have to be created before calling these 2
    setButtonWH();
    setButtonPosition();
    
    mRun = true;
    
}

LMenu::~LMenu(){}
void LMenu::free() {
    mMenuTextTextures->freeLeftTab();
    mMenuTextTextures->freeRightTab();
    mFont = NULL;
}

void LMenu::loadPieceThemeTextures() {
    mPieceTheme1Texture->loadFromFile("../Resources/queen0.png");
    mPieceTheme2Texture->loadFromFile("../Resources/queen1.png");
}

void LMenu::renderPieceTheme() {
    int const hPadding = 25;
    mPieceTheme1Texture->render((((SCREEN_WIDTH - mPieceTheme1Texture->getWidth()) / 2) - mPieceTheme1Texture->getWidth() / 2), ((SCREEN_HEIGHT - mPieceTheme1Texture->getHeight()) / 2) + mPieceTheme1Texture->getHeight() / 2);
    mPieceTheme2Texture->render((((SCREEN_WIDTH - mPieceTheme1Texture->getWidth()) / 2) - mPieceTheme2Texture->getWidth() / 2) + mPieceTheme1Texture->getWidth() + hPadding, ((SCREEN_HEIGHT - mPieceTheme1Texture->getHeight()) / 2) + mPieceTheme1Texture->getHeight() / 2);
}

void LMenu::initMenuStrings() {
    
    CFURLRef appUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("settings.left"), NULL, NULL);
    CFStringRef filePathRef = CFURLCopyPath(appUrlRef);
    const char* filePath = CFStringGetCStringPtr(filePathRef, kCFStringEncodingUTF8);

    
    std::ifstream settingsLeft(filePath);
    
    CFRelease(filePathRef);
    CFRelease(appUrlRef);

    
    bool success = true;
    if(settingsLeft.fail()) {
        printf("Unable to load settings file!\n");
        success = false;
    }
    else {
        for(int i(0); i < LEFT_MENU; i++) {
            std::string line;
            std::getline(settingsLeft, line);
            mMenuLeftStr[i] = line;
        }
        settingsLeft.close();
    }
    
    CFURLRef appUrlRef2 = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("settings.right"), NULL, NULL);
    CFStringRef filePathRef2 = CFURLCopyPath(appUrlRef2);
    const char* filePath2 = CFStringGetCStringPtr(filePathRef2, kCFStringEncodingUTF8);

    
    std::ifstream settingsRight(filePath2);
    
    CFRelease(filePathRef2);
    CFRelease(appUrlRef2);
    
    success = true;
    if(settingsRight.fail()) {
        printf("Unable to load settings file!\n");
        success = false;
    }
    else {
        for(int i(0); i < TOTAL_CLICKABLE_ITEMS; i++) {
            std::string line;
            std::getline(settingsRight, line);
            mMenuRightStr[i] = line;
        }
        settingsRight.close();
    }
}

void LMenu::initCurrentItemList() {
    mSettingsTable[SLM_YES] = 0;
    mSettingsTable[SLM_NO] = 1;
    mSettingsTable[TL_YES] = 0;
    mSettingsTable[TL_NO] = 1;
    mSettingsTable[TL_5] = 0;
    mSettingsTable[TL_10] = 0;
    mSettingsTable[TC_BROWN] = 0;
    mSettingsTable[TC_GREY] = 1;
    mSettingsTable[MT_JAZZY] = 0;
    mSettingsTable[MT_CLASSIC] = 1;
    mSettingsTable[PT_1] = 1;
    mSettingsTable[PT_2] = 0;
}

/*
bool LMenu::initController() {
    bool success = true;
    mGameController->init();
    if(mGameController->gc() == NULL) {
        printf("failed to init controller! Error %s\n", SDL_GetError());
        success = false;
    }
    return success;

}

*/

void LMenu::initFont() {
    mFont = TTF_OpenFont("../Resources/valentin.ttf", 28);
    if(mFont == NULL) {
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    }
}

bool LMenu::getRun() const {
    return mRun;
}

void LMenu::underlineSelected() const {
    SDL_SetRenderDrawColor(gRenderer, 0, 0xFF, 0, 0xFF);
    for(int i(0); i < TOTAL_CLICKABLE_ITEMS - 1; i++) {
        if(mSettingsTable[i] == 1) {
            for(int y(-6); y < -3; y++) {
                //y is the offset to render a thicker line
                SDL_RenderDrawLine(gRenderer, mButtons[i]->getX(), (mButtons[i]->getY() + mButtons[i]->getH() + y), (mButtons[i]->getX() + mButtons[i]->getW()), (mButtons[i]->getY() + mButtons[i]->getH() + y));
            }
        }
    }
}

void LMenu::crossOut() const {
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0, 0, 0xFF);
    for(int i(0); i < TOTAL_CLICKABLE_ITEMS - 1; i++) {
        if(mSettingsTable[i] == 0) {
            for(int y(-6); y < -3; y++) {
                //y is the offset to render a thicker line
                SDL_RenderDrawLine(gRenderer, mButtons[i]->getX(), ((mButtons[i]->getY() + mButtons[i]->getH() / 2) + y), (mButtons[i]->getX() + mButtons[i]->getW()), ((mButtons[i]->getY() + mButtons[i]->getH() / 2) + y));
            }
        }
    }
}

void LMenu::handleEvent(SDL_Event* e) {
    if(e->type == SDL_QUIT) {
        mRun = false;
    }
    for(int i(0); i < TOTAL_CLICKABLE_ITEMS; i++) {
        if(mButtons[i]->handleInside(e)) {
            switch(i) {
                case SLM_YES:
                    if(mButtons[i]->handleClick(e)) {
                        //Switch on yes and switch off no
                        mSettingsTable[i] = 1;
                        mSettingsTable[i + 1] = 0;
                    }
                    break;
                case SLM_NO:
                    if(mButtons[i]->handleClick(e)) {
                        //Switch on no and switch off yes
                        mSettingsTable[i] = 1;
                        mSettingsTable[i - 1] = 0;
                    }
                    break;
                case TL_YES:
                    if(mButtons[i]->handleClick(e)) {
                        //Switch on yes and switch off no
                        mSettingsTable[i] = 1;
                        mSettingsTable[i + 1] = 0;
                        //Automatically switch on 5"
                        if(mSettingsTable[i] == 1) {
                            mSettingsTable[TL_10] = 0;
                            mSettingsTable[TL_5] = 1;
                        }
                    }
                    break;
                case TL_NO:
                    if(mButtons[i]->handleClick(e)) {
                        mSettingsTable[i] = 1;
                        mSettingsTable[i - 1] = 0;
                        //Automatically switch off both
                        if(mSettingsTable[i] == 1) {
                            mSettingsTable[TL_10] = 0;
                            mSettingsTable[TL_5] = 0;
                        }
                    }
                    break;
                case TL_5:
                    if(mButtons[i]->handleClick(e)) {
                        //Only allow switching if time limit is on
                        if(mSettingsTable[TL_YES] == 1) {
                            mSettingsTable[i] = 1;
                            mSettingsTable[i + 1] = 0;
                        }
                    }
                    break;
                case TL_10:
                    if(mButtons[i]->handleClick(e)) {
                        //Only allow switching if time limit is on
                        if(mSettingsTable[TL_YES] == 1) {
                            mSettingsTable[i] = 1;
                            mSettingsTable[i - 1] = 0;
                        }
                    }
                    break;
                case TC_BROWN:
                    if(mButtons[i]->handleClick(e)) {
                        mSettingsTable[i] = 1;
                        mSettingsTable[i + 1] = 0;
                    }
                    break;
                case TC_GREY:
                    if(mButtons[i]->handleClick(e)) {
                        mSettingsTable[i] = 1;
                        mSettingsTable[i - 1] = 0;
                    }
                    break;
                case MT_JAZZY:
                    if(mButtons[i]->handleClick(e)) {
                        mSettingsTable[i] = 1;
                        mSettingsTable[i + 1] = 0;
                    }
                    break;
                case MT_CLASSIC:
                    if(mButtons[i]->handleClick(e)) {
                        mSettingsTable[i] = 1;
                        mSettingsTable[i - 1] = 0;
                    }
                    break;
                case PT_1:
                    if(mButtons[i]->handleClick(e)) {
                        mSettingsTable[i] = 1;
                        mSettingsTable[i + 1] = 0;
                    }
                    break;
                case PT_2:
                    if(mButtons[i]->handleClick(e)) {
                        mSettingsTable[i] = 1;
                        mSettingsTable[i - 1] = 0;
                    }
                    break;
                case BACK:
                    if(mButtons[i]->handleClick(e)) {
                        mRun = false;
                        saveSettingsToFile();
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

void LMenu::saveSettingsToFile() {
    
    CFURLRef appUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("settings.config"), NULL, NULL);
    CFStringRef filePathRef = CFURLCopyPath(appUrlRef);
    const char* filePath = CFStringGetCStringPtr(filePathRef, kCFStringEncodingUTF8);
    
    std::ofstream settings;
    
    CFRelease(filePathRef);
    CFRelease(appUrlRef);
    
    settings.open(filePath, std::ios::trunc);
    
    if(settings.is_open()) {
        for(int i(0); i < TOTAL_CLICKABLE_ITEMS - 1; i++) {
            int a = mSettingsTable[i];
            std::stringstream ss;
            ss << a;
            std::string str = ss.str();
            settings << str;
        }
        settings.close();
    }
    else {
        printf("Unable to load settings file!\n");
    }
}
/*
SDL_GameController* LMenu::getGameController() {
    return mGameController->gc();
}

LGameController* LMenu::getLGameController() {
    return mGameController;
}

void LMenu::generateText(LGameController* controller){
    mMenuTextTextures->fillScript(controller);
}
*/

void LMenu::renderTextureFromTextLeft() {
    SDL_Color colorBlack = {0,0,0,0xFF};
    mMenuTextTextures->loadFromRenderedTextTabLeft(mMenuLeftStr, mFont, LEFT_MENU, colorBlack);
}
void LMenu::renderTextureFromTextRight() {
    SDL_Color colorBlack = {0,0,0,0xFF};
    mMenuTextTextures->loadFromRenderedTextTabRight(mMenuRightStr, mFont, TOTAL_CLICKABLE_ITEMS - 2, colorBlack);
}

void LMenu::renderLeftTextureToRenderer() {
    mMenuTextTextures->renderFromTabLeftSide(LEFT_MENU);
}

void LMenu::renderRightTextureToRenderer() {
    mMenuTextTextures->renderFromTabRightSide(TOTAL_CLICKABLE_ITEMS - 2);
}

//function used to check the buttons box are set properly
void LMenu::drawButtons() {
    for(int i(0); i < TOTAL_CLICKABLE_ITEMS; i++) {
        SDL_Rect r;
        r.x = mButtons[i]->getX();
        r.y = mButtons[i]->getY();
        r.w = mButtons[i]->getW();
        r.h = mButtons[i]->getH();
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0, 0, 0xFF);
        SDL_RenderDrawRect(gRenderer, &r);
    }
}

void LMenu::setButtonPosition() {
    const int initialVerticalSpace(5);
    const int titlePadding(5);
    const int initialHorizontalSpace(10);
    const int initialSpace(5);
    int const hPadding (25);
    for(int i(0); i < TOTAL_CLICKABLE_ITEMS; i++) {
        if(i % 2 == 0) {
            if(i == SLM_YES) {
            mButtons[i]->setPosition(SCREEN_WIDTH / 2, (initialSpace + titlePadding + (mMenuTextTextures->getLeftHeightTab(0) * 1)));
            }
            else if(i == TL_YES) {
                mButtons[i]->setPosition(SCREEN_WIDTH / 2, (initialSpace + titlePadding + (mMenuTextTextures->getLeftHeightTab(0) * i)));
            }
            else if(i == TL_5) {
                mButtons[i]->setPosition(SCREEN_WIDTH / 2, (initialSpace + titlePadding + (mMenuTextTextures->getLeftHeightTab(0) * (i - 1))));
            }
            else if(i == TC_BROWN) {
                mButtons[i]->setPosition(SCREEN_WIDTH / 2, (initialSpace + titlePadding + (mMenuTextTextures->getLeftHeightTab(0) * (i - 2))));
            }
            else if(i == MT_JAZZY) {
                mButtons[i]->setPosition(SCREEN_WIDTH / 2, (initialSpace + titlePadding + (mMenuTextTextures->getLeftHeightTab(0) * (i - 3))));
            }
            else if(i == PT_1) {
                mButtons[i]->setPosition((((SCREEN_WIDTH - mPieceTheme1Texture->getWidth()) / 2) - mPieceTheme1Texture->getWidth() / 2), ((SCREEN_HEIGHT - mPieceTheme1Texture->getHeight()) / 2) + mPieceTheme1Texture->getHeight() / 2);
            }
            else if(i == BACK) {
                mButtons[i]->setPosition(initialHorizontalSpace, SCREEN_HEIGHT - mMenuTextTextures->getRightHeightTab(i - 2));
            }
        }
        else {
            if(i == SLM_NO) {
                mButtons[i]->setPosition((SCREEN_WIDTH / 2) + (initialHorizontalSpace + mMenuTextTextures->getRightWidthTab(i - 1)), (initialSpace + titlePadding + (mMenuTextTextures->getLeftHeightTab(0) * 1)));
            }
            else if(i == TL_NO) {
                mButtons[i]->setPosition((SCREEN_WIDTH / 2) + (initialHorizontalSpace + mMenuTextTextures->getRightWidthTab(i - 1)), (initialSpace + titlePadding + (mMenuTextTextures->getLeftHeightTab(0) * (i - 1))));
            }
            else if(i == TL_10) {
                mButtons[i]->setPosition((SCREEN_WIDTH / 2) + (initialHorizontalSpace + mMenuTextTextures->getRightWidthTab(i - 1)), (initialSpace + titlePadding + (mMenuTextTextures->getLeftHeightTab(0) * (i - 2))));
            }
            else if(i == TC_GREY) {
                mButtons[i]->setPosition((SCREEN_WIDTH / 2) + (initialHorizontalSpace + mMenuTextTextures->getRightWidthTab(i - 1)), (initialSpace + titlePadding + (mMenuTextTextures->getLeftHeightTab(0) * (i - 3))));
            }
            else if(i == MT_CLASSIC) {
                mButtons[i]->setPosition((SCREEN_WIDTH / 2) + (initialHorizontalSpace + mMenuTextTextures->getRightWidthTab(i - 1)), (initialSpace + titlePadding + (mMenuTextTextures->getLeftHeightTab(0) * (i - 4))));
            }
            else if(i == PT_2) {
                mButtons[i]->setPosition((((SCREEN_WIDTH - mPieceTheme1Texture->getWidth()) / 2) - mPieceTheme2Texture->getWidth() / 2) + mPieceTheme1Texture->getWidth() + hPadding, ((SCREEN_HEIGHT - mPieceTheme2Texture->getHeight()) / 2) + mPieceTheme2Texture->getHeight() / 2);
            }

        }
    }
}

void LMenu::setButtonWH() {
    for(int i(0); i < TOTAL_CLICKABLE_ITEMS; i++) {
        if(i == PT_1) mButtons[i]->setWidthAndHeight(mPieceTheme1Texture->getWidth(), (mPieceTheme1Texture->getHeight()));
        else if(i == PT_2) mButtons[i]->setWidthAndHeight(mPieceTheme2Texture->getWidth(), (mPieceTheme2Texture->getHeight()));
        else if(i == BACK) mButtons[i]->setWidthAndHeight(mMenuTextTextures->getRightWidthTab(i - 2), (mMenuTextTextures->getRightHeightTab(i - 2)));
        else mButtons[i]->setWidthAndHeight(mMenuTextTextures->getRightWidthTab(i), (mMenuTextTextures->getRightHeightTab(i)));
    }
}