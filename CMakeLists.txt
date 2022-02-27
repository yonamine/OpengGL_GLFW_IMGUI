cmake_minimum_required (VERSION 3.8)

project ("OpenGL_20220219")

set (CMAKE_CXX_STANDARD 17)

set (CMAKE_CXX_FLAGS "-DImTextureID=ImU64 -D_UNICODE=1 -DUNICODE=1 /EHsc")

set (BIN "a.out")

set (SOURCE_FILES
    "main.cpp"
    "stb_image.h"
    "WavefrontObj.cpp"
    "WavefrontObj.hpp"
    "imgui/imconfig.h"
    "imgui/imgui.cpp"
    "imgui/imgui.h"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_impl_glfw.cpp"
    "imgui/imgui_impl_glfw.h"
    "imgui/imgui_impl_opengl3.cpp"
    "imgui/imgui_impl_opengl3.h"
    "imgui/imgui_impl_opengl3_loader.h"
    "imgui/imgui_internal.h"
    "imgui/imgui_tables.cpp"
    "imgui/imgui_widgets.cpp"
    "imgui/imstb_rectpack.h"
    "imgui/imstb_textedit.h"
    "imgui/imstb_truetype.h")


find_package (GLEW REQUIRED)
find_package (glfw3 CONFIG REQUIRED)
find_package (glm CONFIG REQUIRED)

set (LIBRARIES "GLEW::GLEW"
               "glm::glm"
               "glfw")

add_executable (${BIN} ${SOURCE_FILES})
target_link_libraries (${BIN} PRIVATE ${LIBRARIES})


find_package (GTest REQUIRED)
add_executable (UnitTest "WavefrontObjTest.cpp"
                         "WavefrontObj.cpp"
                         "WavefrontObj.hpp")
target_link_libraries (UnitTest PRIVATE
                       "GTest::GTest"
                       "GTest::gtest_main")
add_test(AllTestsInTest UnitTest)