#pragma once

#include "Window/Window.h"
#include "Animate/Animate.h"
#include "Type.h"

class Game {
private:
  Window *m_Window = nullptr;
  Animate *m_Animate = nullptr;
  
  State m_State = State::IDLING;

public:
  Game() = default;
  ~Game() = default;

  void update(double deltaTime);

  void setWindow(Window *window) { m_Window = window; }

  void setAnimate(Animate *animate) { m_Animate = animate; }
};