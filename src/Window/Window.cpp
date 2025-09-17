#include "Window.h"

#include <stdio.h>
#include <stdlib.h>

static void errorCallback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static const char *vertexShaderT = R"(
#version 110
attribute vec2 vPos;
attribute vec2 vTexCoord;
varying vec2 TexCoord;
void main()
{
    gl_Position = vec4(vPos, 0.0, 1.0);
    TexCoord = vTexCoord;
}
)";

static const char *fragmentShaderT = R"(
#version 110
varying vec2 TexCoord;
uniform sampler2D m_Texture;
void main()
{
    vec2 flippedUV = vec2(TexCoord.x, 1.0 - TexCoord.y);
    vec4 texColor = texture2D(m_Texture, flippedUV);

    // float threshold = 0.05;
    // vec3 target = vec3(252.0/255.0, 247.0/255.0, 252.0/255.0);

    // if (all(lessThan(abs(texColor.rgb - target), vec3(threshold))))
    //   texColor.a = 0.;

    gl_FragColor = texColor;
}
)";

Window::~Window() {
  if (m_Program)
    glDeleteProgram(m_Program);

  if (m_Window)
    glfwDestroyWindow(m_Window);

  glfwTerminate();
}

void Window::initialize(const Init &init) {
  m_Init = init;

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

  m_Window = glfwCreateWindow(init.width, init.height, "pet", nullptr, nullptr);

  glfwSetWindowUserPointer(m_Window, init.data);

  if (init.onKeyPress)
    glfwSetKeyCallback(m_Window, init.onKeyPress);
  if (init.onMouseMove)
    glfwSetCursorPosCallback(m_Window, init.onMouseMove);
  if (init.onMouseButton)
    glfwSetMouseButtonCallback(m_Window, init.onMouseButton);
  if (init.onScroll)
    glfwSetScrollCallback(m_Window, init.onScroll);

  glfwSetWindowPos(m_Window, init.positionX, init.positionY);

  if (!m_Window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(m_Window);
  gladLoadGL();
  glfwSwapInterval(1);

  Vertex quad[4] = {{-1.0f, -1.0f, 0.0f, 0.0f},
                    {1.0f, -1.0f, 1.0f, 0.0f},
                    {-1.0f, 1.0f, 0.0f, 1.0f},
                    {1.0f, 1.0f, 1.0f, 1.0f}};

  glGenBuffers(1, &m_VertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

  m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(m_VertexShader, 1, &vertexShaderT, NULL);
  glCompileShader(m_VertexShader);

  m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(m_FragmentShader, 1, &fragmentShaderT, NULL);
  glCompileShader(m_FragmentShader);

  m_Program = glCreateProgram();
  glAttachShader(m_Program, m_VertexShader);
  glAttachShader(m_Program, m_FragmentShader);
  glLinkProgram(m_Program);

  glDeleteShader(m_VertexShader);
  glDeleteShader(m_FragmentShader);

  m_VPosLocation = glGetAttribLocation(m_Program, "vPos");
  m_VTexLocation = glGetAttribLocation(m_Program, "vTexCoord");

  glEnableVertexAttribArray(m_VPosLocation);
  glVertexAttribPointer(m_VPosLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)0);

  glEnableVertexAttribArray(m_VTexLocation);
  glVertexAttribPointer(m_VTexLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(2 * sizeof(float)));

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glViewport(0, 0, init.width, init.height);
}

bool Window::resize(uint32_t width, uint32_t height) {
  if (m_TexWidth == width && m_TexHeight == height)
    return false;

  m_TexWidth = width;
  m_TexHeight = height;

  if (m_Texture)
    glDeleteTextures(1, &m_Texture);

  glGenTextures(1, &m_Texture);
  glBindTexture(GL_TEXTURE_2D, m_Texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, NULL);

  //  Trigger a resize
  {
    glfwSetWindowSize(m_Window, width, height);
    glViewport(0, 0, width, height);
  }

  return true;
}

void Window::present(const std::vector<uint8_t> &buffer, uint32_t width,
                     uint32_t height) {
  resize(width, height);

  glBindTexture(GL_TEXTURE_2D, m_Texture);

  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA,
                  GL_UNSIGNED_BYTE, buffer.data());

  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(m_Program);
  glBindTexture(GL_TEXTURE_2D, m_Texture);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glfwSwapBuffers(m_Window);
  glfwPollEvents();
}
