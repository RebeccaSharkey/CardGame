project(Engine)

# Collect Source Files
file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS "Source/*.cpp" "Source/*.h")

# Create Static Library
add_library(Engine STATIC ${ENGINE_SRC})

# Headers available to anything that links to it
target_include_directories(Engine PUBLIC Source)

# Vulkan dependency
find_package(Vulkan REQUIRED)
target_include_directories(Engine PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(Engine PRIVATE ${Vulkan_LIBRARIES})