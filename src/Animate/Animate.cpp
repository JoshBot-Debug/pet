#include "Animate.h"

void Animate::update(double deltaTime) {
  GLFWwindow *window = m_Window->getWindow();

  auto now = std::chrono::steady_clock::now();

  // Increment frame
  {
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastTime)
            .count();
    if (elapsed >= m_Animation->at(m_Variation)[m_Frame].delay) {
      m_Frame = (m_Frame + 1) % m_Animation->at(m_Variation).size();
      m_LastTime = now;
    }
  }

  int winX, winY;
  glfwGetWindowPos(window, &winX, &winY);

  int winW, winH;
  glfwGetWindowSize(window, &winW, &winH);

  int gravity = 9;

  AABB floorAABB{0, winH, 1, 1};
  AABB characterAABB{winX, winY, winW, winH};

  int nX = winX, nY = winY;

  if (!characterAABB.intersects(floorAABB))
    nY += gravity;

  if (winX != nX || winY != nY)
    glfwSetWindowPos(window, nX, nY);
}

void Animate::setAnimation(const std::string &name, uint32_t variation) {
  m_Animation = &m_Animations.at(name);
  m_Variation = variation;
}

void Animate::addAnimation(
    const std::pair<std::string, std::vector<std::vector<Frame>>> &animation) {
  m_Animations.emplace(animation);
}
