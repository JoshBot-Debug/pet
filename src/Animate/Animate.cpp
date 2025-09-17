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
}

void Animate::setAnimation(Animation animation, uint32_t variation) {
  if (m_CurrentAnimation == animation)
    return;
    
  m_CurrentAnimation = animation;
  m_Animation = &m_Animations.at(animation);
  m_Variation = variation;
  m_Frame = 0;
}

void Animate::addAnimation(
    const std::pair<Animation, std::vector<std::vector<Frame>>> &animation) {
  m_Animations.emplace(animation);
}
