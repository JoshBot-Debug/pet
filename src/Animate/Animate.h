#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

#include "Type.h"
#include "Window/Window.h"

class Animate {
private:
  Window *m_Window = nullptr;

  std::vector<std::vector<Frame>> *m_Animation = nullptr;

  std::unordered_map<std::string, std::vector<std::vector<Frame>>> m_Animations;

  uint32_t m_Frame = 0;

  uint32_t m_Variation = 0;

  std::chrono::_V2::steady_clock::time_point m_LastTime =
      std::chrono::steady_clock::now();

public:
  Animate() = default;
  ~Animate() = default;

  void update(double deltaTime);

  Frame &getFrame() { return m_Animation->at(m_Variation)[m_Frame]; };

  void setAnimation(const std::string &name, uint32_t variation = 0);

  void addAnimation(
      const std::pair<std::string, std::vector<std::vector<Frame>>> &animation);

  void setWindow(Window *window) { m_Window = window; }
};