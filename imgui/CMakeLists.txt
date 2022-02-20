cmake_minimum_required (VERSION 3.20)

project ("IMGUI-01")

set (CMAKE_CXX_STANDARD 17)

set (CMAKE_CXX_FLAGS "-DImTextureID=ImU64 -D_UNICODE=1 -DUNICODE=1")

add_executable (IMGUI-01 
    "IMGUI-01.cpp"
    "imconfig.h"
    "imgui.cpp"
    "imgui.h"
    "imgui_demo.cpp"
    "imgui_draw.cpp"
    "imgui_impl_dx12.cpp"
    "imgui_impl_dx12.h"
    "imgui_impl_win32.cpp"
    "imgui_impl_win32.h"
    "imgui_internal.h"
    "imgui_tables.cpp"
    "imgui_widgets.cpp"
    "imstb_rectpack.h"
    "imstb_textedit.h"
    "imstb_truetype.h"
)

target_link_libraries(IMGUI-01 d3d12.lib d3dcompiler.lib dxgi.lib)
