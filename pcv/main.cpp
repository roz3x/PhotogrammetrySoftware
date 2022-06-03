#include <bits/stdc++.h>
using namespace std;

#define PI 3.141592f
#define PI2 2*PI

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#define TOTAL 1

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h> 

glm::mat4 mvp;
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

float xRot; 
float yRot; 
float zRot; 

float  floatMod(float &a) {if (a > 360.f) a-=360.f; if(a < 0) a+=360.f; return a; } 

float rot = 6.0f; 
unsigned int program;

const int w  = 800, h = 450 ; 

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

const string vertexShader =
    "#version 330 core\n"
    "layout(location = 0) in vec3 vertex_position;\n"
    "layout(location = 1) in vec3 vertex_color;\n"
    "uniform mat4 MVP;\n"
    "out vec3 vs_color;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = MVP * vec4(vertex_position, 1.0f);\n"
    "   vs_color = vertex_color;\n"
    "}\n"
    "\n";

const string fragmentShader =
    "#version 330 core\n"
    "in vec3 vs_color;\n"
    "out vec4 fs_color;\n"
    "void main()\n"
    "{\n"
    "	fs_color = vec4(vs_color, 1.0f);\n"
    "}\n";


void setMVP() { 
  mvp = projection * view * model;
  unsigned int MatrixID = glGetUniformLocation(program, "MVP");
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
}
void rotateAroundOrigin(const bool isClockwise) {
  if (isClockwise) {
    model = glm::rotate(model, glm::radians(rot),
                        glm::vec3(0.f, 1.f, 0.f));
    xRot += rot; floatMod(xRot);
  } else {
    model = glm::rotate(model, glm::radians(rot),
                        glm::vec3(0.f, -1.f, 0.f));
                        xRot -= rot; floatMod(xRot); 
  }
}

void rotateYAxis(const bool isClockwise) {
  if (isClockwise) {
    model = glm::rotate(model, glm::radians(rot),
                        glm::vec3(1.f, 0.f, 0.f));
                        yRot += rot; floatMod(yRot); 
  } else {
    model = glm::rotate(model, glm::radians(rot),
                        glm::vec3(-1.f, 0.f, 0.f));
                        yRot += rot; floatMod(yRot); 

  }
}

void rotateZAxis(const bool isClockwise) {
  if (isClockwise) {
    model = glm::rotate(model, glm::radians(6.f),
                        glm::vec3(0.f, 0.f, 1.f));
                        zRot += rot; floatMod(zRot); 
  } else {
    model = glm::rotate(model, glm::radians(6.f),
                        glm::vec3(0.f, 0.f, -1.f));
                        zRot -= rot; floatMod(zRot); 
  }

}

void translateForward() { 
  view = glm::translate(view,glm::vec3(-10.f , 0.f , 0.f )); 
}

void translateBack() { 
  view = glm::translate(view, glm::vec3(10.f , 0.f , 0.f )); 
}
void resetView() {
  model = glm::mat4(1.f);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  if (key == GLFW_KEY_W) rotateAroundOrigin(false);
  else if (key == GLFW_KEY_S) rotateAroundOrigin(true);
  else if (key == GLFW_KEY_Q) rotateYAxis(false);
  else if (key == GLFW_KEY_E)  rotateYAxis(true);
  else if (key == GLFW_KEY_A) rotateZAxis(false);
  else if (key == GLFW_KEY_D) rotateZAxis(true);
  else if (key == GLFW_KEY_T)   resetView();
  else if (key == GLFW_KEY_UP ) translateForward();
  else if ( key==GLFW_KEY_DOWN) translateBack(); 
  setMVP(); 
}

void loadBuffers(const std::vector<float> vertexPositionData,
                 const std::vector<float> vertexColorData) {
  unsigned int vertexArrayId;
  unsigned int vertexBufferId;
  unsigned int colorBufferId;

  glCreateVertexArrays(1, &vertexArrayId);
  glBindVertexArray(vertexArrayId);

  glCreateBuffers(1, &vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER,
               vertexPositionData.size() * sizeof(vertexPositionData[0]),
               &vertexPositionData[0], GL_STATIC_DRAW);

  glCreateBuffers(1, &colorBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, colorBufferId);
  glBufferData(GL_ARRAY_BUFFER,
               vertexColorData.size() * sizeof(vertexColorData[0]),
               &vertexColorData[0], GL_STATIC_DRAW);

  glEnableVertexArrayAttrib(vertexArrayId, 0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), 0);

  glEnableVertexArrayAttrib(vertexArrayId, 1);
  glBindBuffer(GL_ARRAY_BUFFER, colorBufferId);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
}

void readData(string filename, vector<float>& vertexPositionData,
              vector<float>& vertexColorData) {
  ifstream file(filename);
  int counter = 0;
  if (file.is_open()) {
    for (string line; getline(file, line);) {
#ifndef TOTAL
      if (counter++ % 20 != 0) continue;
#endif
      float x, y, z, red, green, blue, intensity;
      file >> x >> y >> z >> intensity >> red >> green >> blue;
      vertexPositionData.push_back(z/10);
      vertexPositionData.push_back(x/10);
      vertexPositionData.push_back(y/10);

      vertexColorData.push_back(red/ 255.0f);
      vertexColorData.push_back(green / 255.0f);
      vertexColorData.push_back(blue / 255.0f);
    }
  }
}

void initialMVP() {
  projection =
      glm::perspective(glm::radians(45.f), (float)w/h, 0.1f, 1500.f);
  model = glm::mat4(1.0f);
  view = glm::lookAt(glm::vec3(0,0,0), glm::vec3(1, 0, 0 ),
                     glm::vec3(0, 0, 1));
  mvp = projection * view * model;
}

unsigned int compileShader(unsigned int shaderType, const string source) {
  const char* src = source.c_str();
  unsigned int shaderId = glCreateShader(shaderType);
  glShaderSource(shaderId, 1, &src, nullptr);
  glCompileShader(shaderId);
  return shaderId;
}


int main(int argc, const char* argv[]) {
  vector<float> vertexPositionData, vertexColorData;
  readData("input.txt", vertexPositionData, vertexColorData);
  int positionNumberIndices = 3 * vertexPositionData.size();

  GLFWwindow* window;

  if (!glfwInit()) {
    return -1;
  }

  window = glfwCreateWindow(w, h, "testing toasts", NULL, NULL);

  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  

  if (glewInit() != GLEW_OK) {
    cout << "something went wrong" << endl;
    return -1;
  }

  int imageWidth  = 0;
  int imageHeight = 0;
  GLuint textureId  = 0;
  bool ret = LoadTextureFromFile("../im1.png", &textureId, &imageWidth, &imageHeight);
  IM_ASSERT(ret);


  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  IMGUI_CHECKVERSION(); 
  ImGui::CreateContext(); 
  ImGuiIO& io = ImGui::GetIO(); 
  (void)io;

  ImGui::StyleColorsDark(); 
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 100"); 

  initialMVP();
  loadBuffers(vertexPositionData, vertexColorData);

  program = glCreateProgram();
  unsigned int compiled_vertShader =
      compileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int compiled_fragShader =
      compileShader(GL_FRAGMENT_SHADER, fragmentShader);

  glAttachShader(program, compiled_vertShader);
  glAttachShader(program, compiled_fragShader);
  glLinkProgram(program);
  glValidateProgram(program);
  glDetachShader(program, compiled_vertShader);
  glDetachShader(program, compiled_fragShader);
  glUseProgram(program);

  unsigned int MatrixID = glGetUniformLocation(program, "MVP");
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

  glPointSize(2);
  glEnable(GL_DEPTH_TEST);
  glfwSwapInterval(1);
  glfwSetKeyCallback(window, keyCallback);
  // TODO:  mouse



  // {
  //   // animation part 

  //   rot = -90.f; 
  //   rotateAroundOrigin(true);

  //   rot = 1.f; 

  // }
  int count = 0 ; 
  glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
  glClearColor(0.0f, 0.f, 0.f, 0.1f);
  bool myWindow = true; 
  while (!glfwWindowShouldClose(window)) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, positionNumberIndices);
    ImGui::Begin("info window ", &myWindow );
      glm::vec3  pos = glm::inverse(view) * glm::vec4(0.f , 0.f , 0.f , 1.f );
      ImGui::Text("camera pos : %f %f %f" , pos.x, pos.y , pos.z ); 
      ImGui::Text("x rot: %f",xRot); 
      ImGui::Text("y rot: %f",yRot);
      ImGui::Text("z rot: %f",zRot); 
      if (ImGui::Button("close"))  myWindow = false;
    ImGui::End();


    ImGui::Begin("texture", NULL, ImGuiWindowFlags_AlwaysAutoResize);
      ImGui::Text("pointer = %d", textureId);
      ImGui::Text("size = %d x %d", 250, 150);
      ImGui::Image((void*)(intptr_t)textureId, ImVec2(250, 150));
    ImGui::End();


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);


    glfwPollEvents();
 
    // if (count % 10==0) {
    //   rotateYAxis(true); 
    //   setMVP();
    // }
    // count++; 
  }


  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
  return 0;
}

