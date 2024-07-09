# vulkan
add_library(vulkan INTERFACE)
set(VULKAN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vulkan/include)
target_sources(vulkan INTERFACE 
    ${VULKAN_INCLUDE_DIR}/vulkan/vulkan.h
)
target_include_directories(vulkan INTERFACE ${VULKAN_INCLUDE_DIR})
target_compile_definitions(vulkan INTERFACE VK_NO_PROTOTYPES)

# DirectX12-Agility SDK
add_library(DirectX-Headers INTERFACE)
set(DIRECTX_HEADERS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DirectX-Headers/include)
target_sources(DirectX-Headers INTERFACE 
    ${DIRECTX_HEADERS_INCLUDE_DIR}/directx/d3d12.h
    ${DIRECTX_HEADERS_INCLUDE_DIR}/directx/d3dx12.h
    ${DIRECTX_HEADERS_INCLUDE_DIR}/directx/d3d12shader.h
)
target_include_directories(DirectX-Headers INTERFACE ${DIRECTX_HEADERS_INCLUDE_DIR})
