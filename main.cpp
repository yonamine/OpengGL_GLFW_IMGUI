/**
 * 
 * 
 * 
 * References:
 * http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
 * https://learnopengl.com/Getting-started/Hello-Triangle
 * https://open.gl/drawing
 * 
 * 
 * 
 * 
 */
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

// HD
// constexpr int32_t kWidth{1280};
// constexpr int32_t kHeight{720};

// FHD or 2K
constexpr int32_t kWidth{1920};
constexpr int32_t kHeight{1080};

// UHD or 4K
// constexpr int32_t kWidth{3840};
// constexpr int32_t kHeight{2160};

const std::string glsl_version{"#version 130"};
const std::string kTitle{"OpenGL: GLFW / GLEW / ImGui"};

enum class ShaderType { FRAGMENT = 0x01, VERTEX = 0x02, INVALID = 0xFF };

static GLFWwindow* window{nullptr};

static ImVec4 bgColor{0.0f, 0.0f, 0.4f, 0.0f}; // Dark Blue
static bool isRunning_{true};


// 
// 
// Camera: https://learnopengl.com/Getting-started/Camera
// 
// 
//glm::vec3 eye{0.0f, 0.0f, 3.0f};    // It's the position of the camera's viewpoint.
//                                    // It's the point where your virtual 3D camera
//                                    // is located.
glm::vec3 cameraPos{0.0f, 0.0f, 3.0f};

//glm::vec3 center{0.0f, 0.0f, 0.0f}; // It's where the camera is looking at (a position).
//                                    // It's the point which the camera looks at 
//                                    // (center of the scene).
glm::vec3 cameraFront{0.0f, 0.0f, -1.0f};

//glm::vec3 up{0.0f, 1.0f, 0.0f};     // Basically, it's a vector defining your world's
//                                    // "upwards" direction, towards positive Y.
glm::vec3 cameraUp{0.0f, 1.0f,  0.0f};


glm::mat4 projection_matrix_{0.0f};
glm::mat4 view_matrix_{0.0f};
glm::mat4 model_matrix_{0.0f};

static std::string LoadFile(const std::string &filename);
static GLuint CreateShader(const ShaderType type, const std::string &shaderSrc);
static GLuint LinkShaders(GLuint vertexShaderID, GLuint fragmentShaderID);
static GLuint LoadShaders(const std::string &vertex_file_path, const std::string &fragment_file_path);



std::string LoadFile(const std::string &filename) {
  std::stringstream buffer;
  std::ifstream in;

  in.open(filename, std::ios::in);
  assert(in.is_open());

  buffer << in.rdbuf();
  in.close();
  return buffer.str();
}

GLuint CreateShader(const ShaderType type, const std::string &shaderSrc) {
  GLuint shaderID = 0;

  assert((type == ShaderType::VERTEX) || (type == ShaderType::FRAGMENT));

  // Create a Shader
  if (type == ShaderType::VERTEX) {
    shaderID = glCreateShader(GL_VERTEX_SHADER);
  } else if (type == ShaderType::FRAGMENT) {
    shaderID = glCreateShader(GL_FRAGMENT_SHADER);
  }

  assert(!shaderSrc.empty());

  // Compile Shader
  const char *source = shaderSrc.c_str();
  glShaderSource(shaderID, 1, &source, NULL);
  glCompileShader(shaderID);

  // Check Vertex Shader
  GLint compilationResult = GL_FALSE;
  int compileLogSize = 0;

  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compilationResult);
  glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &compileLogSize);

  if (compileLogSize > 0) {
    std::vector<char> errorMessage;
    errorMessage.resize(static_cast<size_t>(compileLogSize) + 1);
    glGetShaderInfoLog(shaderID, compileLogSize, NULL, &errorMessage[0]);
    std::cerr << "Shader Compilation Error: '" << (&errorMessage[0]) << "'\n";
  }

  return shaderID;
}

GLuint LinkShaders(GLuint vertexShaderID, GLuint fragmentShaderID) {
  GLint linkageResult = GL_FALSE;
  int linkageLogSize = 0;
  GLuint programID = 0;

  // Link the program
  programID = glCreateProgram();

  glAttachShader(programID, vertexShaderID);
  glAttachShader(programID, fragmentShaderID);
  glLinkProgram(programID);

  // Check the program
  glGetProgramiv(programID, GL_LINK_STATUS, &linkageResult);
  glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &linkageLogSize);

  if (linkageLogSize > 0) {
    std::vector<char> errorMessage;
    errorMessage.resize(static_cast<size_t>(linkageLogSize) + 1);
    glGetProgramInfoLog(programID, linkageLogSize, NULL, &errorMessage[0]);
    std::cerr << "Shader Linkage Error: '" << (&errorMessage[0]) << "'\n";
  }

  return programID;
}

GLuint LoadShaders(const std::string &vertex_file_path, const std::string &fragment_file_path) {
  assert((!vertex_file_path.empty()) && (!fragment_file_path.empty()));

  // Read the Vertex Shader code from the file
  std::string VertexShaderCode = LoadFile(vertex_file_path);
  std::string FragmentShaderCode = LoadFile(fragment_file_path);

  // Create the shaders
  GLuint VertexShaderID = CreateShader(ShaderType::VERTEX, VertexShaderCode);
  GLuint FragmentShaderID = CreateShader(ShaderType::FRAGMENT, FragmentShaderCode);

  // Link the program
  GLuint ProgramID = LinkShaders(VertexShaderID, FragmentShaderID);

  glDetachShader(ProgramID, VertexShaderID);
  glDetachShader(ProgramID, FragmentShaderID);

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}



/**
 *
 *
 */
auto main(int argc, char **argv) -> int {
  glfwSetErrorCallback([](int error, const char *description) {
    std::cerr << "Glfw Error: " << error << " : " << description << '\n';
  });

  if (!glfwInit()) {
    std::cerr << "Failed to initialized GLFW...\n";
    return EXIT_FAILURE;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(static_cast<int>(kWidth), static_cast<int>(kHeight), kTitle.c_str(), nullptr, nullptr);
  if (window == nullptr) {
    std::cerr << "Failed to create a GLFW window...\n";
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);

  glewExperimental = true;
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    std::cerr << "Failed to initialized GLEW. Error: "
              << glewGetErrorString(err) << "\n";
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
    printf("%s::%d (key, scancode, action, mods) = ('%d', '%d', '%d', '%d')\n", __PRETTY_FUNCTION__, __LINE__, key, scancode, action, mods);

    if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE)) {
      isRunning_ = false;
    }


    if (action == GLFW_REPEAT) {
        constexpr float cameraSpeed{0.05f}; // adjust accordingly

        if(key == GLFW_KEY_LEFT) {
          cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }

        if(key == GLFW_KEY_RIGHT) {
          cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }

        if(key == GLFW_KEY_UP) {
          cameraPos += cameraSpeed * cameraFront;
        }

        if(key == GLFW_KEY_DOWN) {
          cameraPos -= cameraSpeed * cameraFront;
        }
    }

  });


  {
    int32_t width{0};
    int32_t height{0};
    int32_t nrChannels{0};

    unsigned char *data = stbi_load("../../../wall.jpg", &width, &height, &nrChannels, 0);
    printf("%s::%d Image Size: %d x %d\nNumber of Channels: '%d'\nData pointer: '%p'\n", __PRETTY_FUNCTION__, __LINE__, width, height, nrChannels, data);
  }


  const std::string relative_path_{"../../../"};
  const std::string shader_filename{"SolidColor"};
  //const std::string shader_filename{"AnimatedSolidColor"};
  std::string vertexShaderFilename_{relative_path_ + shader_filename + ".vertexshader"};
  std::string fragmentShaderFilename_{relative_path_ + shader_filename + ".fragmentshader"};


    // glm::ortho(xmin, xmax, ymin, ymax, zmin, zmax)
  // Specifies a logical 2D coordinate system which is to be mapped into the window positions
  // indicated.  Often one matches the coordinates to match the size of the window being
  // rendered to.
  //projection_matrix_ = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);
  projection_matrix_ = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

  
/*
  // @TODO I think it does not work in Ortho
  const float radius = 10.0f;
  float camX = sin(glfwGetTime()) * radius;
  float camZ = cos(glfwGetTime()) * radius;
  eye = glm::vec3(camX, 0.0, camZ);
*/
  view_matrix_ = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
  model_matrix_ = glm::mat4{1.0f};


  

  // @BEGIN - Vertex Buffer Object - VBO
  //
  static constexpr int32_t kMaxVertexBuffer{9};
  // Calculate a buffer size in bytes
  static constexpr int8_t kVertexBufferSize{kMaxVertexBuffer * sizeof(GLfloat)};
  //
  // An array of 3 vectors which represents 3 vertices
  // Coordinates:
  // - X : it's in on your right
  // - Y : it's in on your up
  // - Z : it's towards your back (yes, behind, not in front of you)
  // 
  // Or, you can use the RIGHT HAND RULE
  // - X : it's your thumb
  // - Y : it's your index
  // - Z : it's your middle finger. If you put your thumb to the right and your index
  //       to the sky, it will point to your back, too.
  //
  // The triangle below consists of 3 vertices in clockwise order.
  //
  using VertexBuffer = std::array<GLfloat, kMaxVertexBuffer>;
  static const VertexBuffer vertex_buffer_data_{ -0.5f, -0.5f, 0.0f,
                                                  0.5f, -0.5f, 0.0f,
                                                  0.0f,  0.5f, 0.0f, };
  //
  //
  //
  auto vertex_buffer_data = reinterpret_cast<const void *>(vertex_buffer_data_.data());
  //
  // 
  // Upload the vertex data to the Graphics Card.
  // It's important because the memory o your Graphics Card is much faster and won't
  // have to send the data again every time your scene needs to be rendered (about
  // 60 times per second).
  // 
  // Identify our vertex buffer
  GLuint vertex_buffer_index_;
  // Generate one buffer, put the resulting identifier in vertex buffer
  glGenBuffers(1, &vertex_buffer_index_);
  // The following commands will talk about our 'vertex buffer index' buffer.
  // Upload the actual data 
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_index_);
  // Create and initialize a buffer object's data store. Give our vertices to OpenGL
  // The final parameter is very important and its value depends on the usage of
  // the vertex data.
  // 'GL_STATIC_DRAW': the vertex data will be uploaded once and drawn many 
  // times (e.g. the world).
  glBufferData(GL_ARRAY_BUFFER, kVertexBufferSize, vertex_buffer_data, GL_STATIC_DRAW);
  // @END - Vertex Buffer Object - VBO




  // @BEGIN - Vertex Array Object - VAO
  // 
  // 
  GLuint vertex_array_index_;
  // Create Vertex Array Object.
  glGenVertexArrays(1, &vertex_array_index_);
  // Bind it
  // Every time you call 'glVertexAttribPointer', the information will be stored in
  // the VAO. This makes switching between different vertex data and vertex formats
  // as easy as binding a different VAO.
  // VAO does not store any vertex data by itself.
  // It references the VBOs.
  glBindVertexArray(vertex_array_index_);
  GLuint program_id_ = LoadShaders(vertexShaderFilename_, fragmentShaderFilename_);
  GLuint matrix_id_ = glGetUniformLocation(program_id_, "MVP");
  //glBindFragDataLocation(program_id_, 0, "outColor");
  // Retrieve a reference to the 'position' input in the vertex shader.
  //GLint positionAttrib = glGetAttribLocation(program_id_, "position");
  GLint positionAttrib{0};
  // @END - Vertex Array Object - VAO


  GLuint triangleColor = glGetUniformLocation(program_id_, "triangleColor");
  glUniform3f(triangleColor, 1.0f, 0.0f, 0.0f);


  IMGUI_CHECKVERSION();

  ImGui::CreateContext();

  auto io = ImGui::GetIO();

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  ImGui::StyleColorsDark();

  auto style = ImGui::GetStyle();

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version.c_str());


  auto t_start = std::chrono::high_resolution_clock::now();


  while (isRunning_) {

    // @BEGIN - ???
    auto t_now = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
    glUniform3f(triangleColor, (sin(time * 4.0f) + 1.0f) / 2.0f, 0.0f, 0.0f);
    // @END - ???


    glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    //view_matrix_ = glm::lookAt(eye, center, up);
    view_matrix_ = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 MVP = projection_matrix_ * view_matrix_ * model_matrix_;

    // Use the shader
    glUseProgram(program_id_);

    //
    glUniformMatrix4fv(matrix_id_, 1, GL_FALSE, &MVP[0][0]);

    // @BEGIN - Draw a triangle
    // 1st attribute buffer: vertices
    glEnableVertexAttribArray(positionAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_index_);
    glVertexAttribPointer(positionAttrib, // Reference the input attribute.
                                          // No particular reason for 0 (check vertex
                                          // the shader: "layout(location = 0)"),
                                          // but must match the layout in the shader.
                          3,              // number of values for that input.
                          GL_FLOAT,       // type of each component.
                          GL_FALSE,       // Should it be normalized? [-1.0, 1.0].
                          0,              // stride: how many bytes are between each
                                          // position attribute in the array. '0' means
                                          // that there's no data in between
                          (void *) 0      // array buffer offset: how many bytes from
                                          // the start of the array the attribute occurs.
                                          // There're no other attributes, use '0'
    );
    // Draw the triangle. Starting from vertex 0; 3 vertices total -> 1 triangle
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(positionAttrib);
    // @END - Draw a triangle



    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // BEGIN - MainMenuBar
    if (ImGui::BeginMainMenuBar()) {

        if (ImGui::BeginMenu("File")) {
            // File/Quit
            if (ImGui::MenuItem("Quit", "Alt+F4")) {
                isRunning_ = false;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
    // END - MainMenuBar

    // BEGIN - Toolbar
    ImGui::Begin("Camera");
      ImGui::Text("Eye Position");
      ImGui::SliderFloat("eye_x", &cameraPos.x, -100.0f, 100.0f);
      ImGui::SliderFloat("eye_y", &cameraPos.y, -100.0f, 100.0f);
      ImGui::SliderFloat("eye_z", &cameraPos.z, -100.0f, 100.0f);
      if (ImGui::Button("Reset Eye Position")) cameraPos = glm::vec3{0.0f, 0.0f, 3.0f}; // @TODO It's not a good idea to create a new object

      ImGui::Text("Center Position");
      ImGui::SliderFloat("cam_x", &cameraFront.x, -10.0f, 10.0f);
      ImGui::SliderFloat("cam_y", &cameraFront.y, -10.0f, 10.0f);
      ImGui::SliderFloat("cam_z", &cameraFront.z, -10.0f, 10.0f);
      if (ImGui::Button("Reset Center Position")) cameraFront = glm::vec3{0.0f, 0.0f, -1.0f}; // @TODO It's not a good idea to create a new object
    ImGui::End();



    ImGui::Begin("Hello, world!");
      static float colorEdit[4] = { bgColor.x, bgColor.y, bgColor.z, bgColor.w };
      ImGui::ColorEdit4("Background Color", colorEdit);
      bgColor.x = colorEdit[0];
      bgColor.y = colorEdit[1];
      bgColor.z = colorEdit[2];
      bgColor.w = colorEdit[3];

      //ImGui::SameLine();
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
    // END - MainMenuBar




    ImGui::Render();


    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    auto io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      auto backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }


    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup VBO
  glDeleteBuffers(1, &vertex_buffer_index_);
  glDeleteVertexArrays(1, &vertex_array_index_);
  glDeleteProgram(program_id_);

  std::cout << "Closing Window: '" << window << "'...\n";
  glfwTerminate();
  std::cout << "Bye!\n";
  return 0;
}  // End of Main
