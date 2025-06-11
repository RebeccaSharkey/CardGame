project(Engine)

# Collect source files
file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS "Source/*.cpp" "Source/*.h")

# Create static library
add_library(Engine STATIC ${ENGINE_SRC})

# Engine headers (public so Game can include them)
target_include_directories(Engine PUBLIC Source)

# Vulkan setup
find_package(Vulkan REQUIRED)
target_include_directories(Engine PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(Engine PRIVATE ${Vulkan_LIBRARIES})