#pragma once

#include <chrono>
#include <vector>

#include "Window.h"

class Pet {
private:
  struct Viewport {
    int x = 0;
    int y = 0;

    int w = 0;
    int h = 0;
  };

  struct Frame {
    std::vector<uint8_t> pixels = {};
    uint32_t width = 0, height = 0, delay = 0;
  };

private:
  Viewport m_Viewport;
  std::vector<Frame> m_Frames = {};

  uint32_t m_CurrentFrame = 0;

  std::chrono::_V2::steady_clock::time_point m_LastFrameTime =
      std::chrono::steady_clock::now();
  std::chrono::_V2::steady_clock::time_point m_LastTime =
      std::chrono::steady_clock::now();

public:
  Pet() = default;
  ~Pet() = default;

  int initialize(int argc, char *argv[]);

  uint32_t GetWidth() { return m_Frames[0].width; };

  uint32_t GetHeight() { return m_Frames[0].height; };

  void SetViewport(int x, int y, int w, int h);

private:
  void window();

  void update(Window &w);

  void loadGif(const char *filename);
};