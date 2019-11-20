ADD_COMPILE_DEFINITIONS(CE_SDL2_WINDOW_TITLE="CE Tests")

ADD_COMPILE_DEFINITIONS(__MODUS_TARGET_SDL2_AUDIO__=true)
ADD_COMPILE_DEFINITIONS(__MODUS_TARGET_DESKTOP_CONTROLS__=true)
ADD_COMPILE_DEFINITIONS(__MODUS_TARGET_DESKTOP_DISPLAY__=true)
ADD_COMPILE_DEFINITIONS(LIBXMP_CORE_PLAYER=true)

FILE(GLOB_RECURSE TESTS "${CREATIVE_ENGINE_PATH}/test/src/tests/*.cpp")

# resource compiler
SET(RCOMP "${CREATIVE_ENGINE_PATH}/tools/rcomp")

# build Resources.bin
ADD_CUSTOM_COMMAND(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/test/src/Resources.bin
#  COMMAND find ${CMAKE_CURRENT_SOURCE_DIR}/test/src -name "Resources.bin" -exec rm -f {} +
#  COMMAND find ${CMAKE_CURRENT_SOURCE}/test/src -name "BResourceManager.cpp.o" -exec rm -f {} +
  COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/test/src && ls && ${RCOMP} Resources.r
  DEPENDS rcomp
  COMMENT "Compiling Resources ${CMAKE_CURRENT_SOURCE_DIR}/test/src"
)

ADD_EXECUTABLE(
  test
#  Resources.bin
  ${CREATIVE_ENGINE_PATH}/test/src/main.cpp
  ${CREATIVE_ENGINE_PATH}/test/src/test.h
  ${TESTS}
  ${CREATIVE_ENGINE_SOURCE_FILES}
  ${CMAKE_CURRENT_SOURCE_DIR}/test/src/Resources.bin
  ${CREATIVE_ENGINE_PATH}/test/src/GResources.h
  )

TARGET_INCLUDE_DIRECTORIES(test PUBLIC
  ${CREATIVE_ENGINE_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIRS}
  ${CREATIVE_ENGINE_PATH}/test/src
  )

TARGET_LINK_LIBRARIES(test ${SDL2_LIBRARIES})