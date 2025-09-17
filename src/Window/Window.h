#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <vector>

class Window {
private:
  struct Vertex {
    float x, y;
    float u, v;
  };

  struct Init {
    void *data = nullptr;
    uint32_t width = 800, height = 600;
    int positionX = 0, positionY = 0;
    GLFWkeyfun onKeyPress = nullptr;
    GLFWcursorposfun onMouseMove = nullptr;
    GLFWmousebuttonfun onMouseButton = nullptr;
    GLFWscrollfun onScroll = nullptr;
  };

private:
  GLFWwindow *m_Window = nullptr;

  GLuint m_VertexBuffer = 0, m_Texture = 0, m_VertexShader = 0,
         m_FragmentShader = 0, m_Program = 0;
  GLint m_VPosLocation = 0, m_VTexLocation = 0;

  uint32_t m_TexWidth = 0, m_TexHeight = 0;

  Init m_Init;

public:
  Window() = default;

  ~Window();

  void initialize(const Init &init);

  bool resize(uint32_t width, uint32_t height);

  void present(const std::vector<uint8_t> &buffer, uint32_t width,
               uint32_t height);

  int shouldClose() { return glfwWindowShouldClose(m_Window); }

  GLFWwindow *getWindow() { return m_Window; }
};
