include(${SYNGINE_SOURCE_DIR}/cmake/SyngineGame.cmake)
include(${SYNGINE_SOURCE_DIR}/cmake/clangdConfig.cmake)

SyngineGame(testgame
    src/main.cpp
)

clangdConfigure(testgame)
