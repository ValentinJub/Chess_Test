cmake_minimum_required(VERSION 3.2)

project(Chess)
	
add_executable(Chess
	src/CApp.h
	src/CApp.cpp
	src/check.h
	src/check.cpp
	src/constantes.h
	src/funct.cpp
	src/funct_headers.h
	src/headers.h
	src/LBishop.h
	src/LBishop.cpp
	src/LBoardPVAI.h
	src/LBoardPVAI.cpp
	src/LBoardPVP.h
	src/LBoardPVP.cpp
	src/LButton.h
	src/LButton.cpp
	src/LKing.h
	src/LKing.cpp
	src/LKnight.h
	src/LKnight.cpp
	src/LMenuSettings.h
	src/LMenuSettings.cpp
	src/LPawn.h
	src/LPawn.cpp
	src/LPiece.h
	src/LPiece.cpp
	src/LQueen.h
	src/LQueen.cpp
	src/LRook.h
	src/LRook.cpp
	src/LTexture.h
	src/LTexture.cpp
	src/LTimer.h
	src/LTimer.cpp
	src/LWindow.h
	src/LWindow.cpp
	src/main.cpp
	src/playPVAI.h
	src/playPVAI.cpp
	src/playPVP.h
	src/playPVP.cpp
	src/settings.h
	src/settings.cpp
	resources/0-0-0.wav
	resources/checkMateUS.wav
	resources/checkUS.wav
	resources/chime.wav
	resources/drop.wav
	resources/drop2.wav
	resources/drop3.wav
	resources/error.wav
	resources/error2.wav
	resources/error3.wav
	resources/error4.wav
	resources/gameOver.wav
	resources/pawnForward.wav
	resources/promotion.wav
	resources/shuffle.wav
	resources/shuffle2.wav
	resources/START.wav
	resources/victoryOrchestra.wav
	resources/gc.mp3
	resources/ps2.mp3
	resources/menu.mp3
	resources/class0.mp3
	resources/class1.mp3
	resources/class2.mp3
	resources/class3.mp3
	resources/class4.mp3
	resources/class5.mp3
	resources/class6.mp3
	resources/jazz0.mp3
	resources/jazz1.mp3
	resources/jazz2.mp3
	resources/jazz3.mp3
	resources/jazz4.mp3
	resources/jazz5.mp3
	resources/jazz6.mp3
	resources/valentin.ttf
	resources/map.map
	resources/settings.config
	resources/settings.left
	resources/settings.right
	resources/blackScreen.png
	resources/board.png
	resources/pieceSpritesheet.png
	resources/spritesheet32.png
	resources/retroPieceSpriteSheet.png
	resources/queen0.png
	resources/queen1.png
	resources/bg1.jpg
	)

target_compile_features(Chess PRIVATE cxx_std_11)

target_compile_options(Chess PRIVATE -Wall -Wextra -Wpedantic)

set(RESOURCE_FILES
	resources/0-0-0.wav
	resources/checkMateUS.wav
	resources/checkUS.wav
	resources/chime.wav
	resources/drop.wav
	resources/drop2.wav
	resources/drop3.wav
	resources/error.wav
	resources/error2.wav
	resources/error3.wav
	resources/error4.wav
	resources/gameOver.wav
	resources/pawnForward.wav
	resources/promotion.wav
	resources/shuffle.wav
	resources/shuffle2.wav
	resources/START.wav
	resources/victoryOrchestra.wav
	resources/gc.mp3
	resources/ps2.mp3
	resources/menu.mp3
	resources/class0.mp3
	resources/class1.mp3
	resources/class2.mp3
	resources/class3.mp3
	resources/class4.mp3
	resources/class5.mp3
	resources/class6.mp3
	resources/jazz0.mp3
	resources/jazz1.mp3
	resources/jazz2.mp3
	resources/jazz3.mp3
	resources/jazz4.mp3
	resources/jazz5.mp3
	resources/jazz6.mp3
	resources/valentin.ttf
	resources/map.map
	resources/settings.config
	resources/settings.left
	resources/settings.right
	resources/blackScreen.png
	resources/board.png
	resources/pieceSpritesheet.png
	resources/spritesheet32.png
	resources/retroPieceSpriteSheet.png
	resources/queen0.png
	resources/queen1.png
	resources/bg1.jpg)

set_target_properties(Chess PROPERTIES
	MACOSX_BUNDLE TRUE
	MACOSX_FRAMEWORK_IDENTIFIER org.cmake.ExecutableTarget
	RESOURCE "${RESOURCE_FILES}")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

# Find and link SDL2
find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

# Find and link SDL2_image
find_package(SDL2_image REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_IMAGE_LIBRARIES})

# Find and link SDL2_ttf
find_package(SDL2_ttf REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_TTF_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_TTF_LIBRARIES})

# Find and link SDL2_mixer
find_package(SDL2_mixer REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_MIXER_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_MIXER_LIBRARIES})