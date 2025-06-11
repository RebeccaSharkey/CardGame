project(CardGameApp)

# Collect game source files
file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS "Source/*.cpp" "Source/*.h")

# Create the game executable
add_executable(CardGameApp ${GAME_SRC})

# Include game headers
target_include_directories(CardGameApp PRIVATE Source)

# Link against engine
target_link_libraries(CardGameApp PRIVATE Engine)