#include "Game.h"
#include "Type.h"
#include <GLFW/glfw3.h>

#include "Utility.h"

void Game::update(double deltaTime) {
  GLFWwindow *window = m_Window->getWindow();

  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

  int winX, winY;
  glfwGetWindowPos(window, &winX, &winY);

  int winW, winH;
  glfwGetWindowSize(window, &winW, &winH);

  double cursorX, cursorY;
  glfwGetCursorPos(window, &cursorX, &cursorY);

  // To screen position
  cursorY += winY;
  cursorX += winX;

  int gravity = 9;

  AABB floorAABB{0, mode->height - 1, mode->width, 1};
  AABB characterAABB{winX, winY, winW, winH};

  int nX = winX, nY = winY;

  bool onGround = characterAABB.intersects(floorAABB);

  if (!onGround) {
    nY += gravity;
    m_State = State::FALLING;
  }

  if (onGround) {
    m_State = State::IDLING;

    int bias = 5;
    int offset = m_Window->u_FlipX ? 0 : winW;

    LOG(cursorX, nX, offset);

    // If we are in view
    if (cursorY > nY)
      if (nX < (cursorX - offset) + bias || nX > (cursorX - offset) + bias) {
        nX += cursorX > nX ? 1 : -1;
        m_State = State::WALKING;
        m_Window->u_FlipX = nX > cursorX;
      }
  }

  if (winX != nX || winY != nY)
    glfwSetWindowPos(window, nX, nY);

  switch (m_State) {
  case State::IDLING:
    m_Animate->setAnimation(Animation::IDLE);
    break;
  case State::WALKING:
    m_Animate->setAnimation(Animation::WALK);
    break;
  case State::RUNNING:
    m_Animate->setAnimation(Animation::RUN);
    break;
  case State::SITTING:
    m_Animate->setAnimation(Animation::SIT);
    break;
  case State::FALLING:
    m_Animate->setAnimation(Animation::SIT);
    break;
  case State::JUMPING:
    m_Animate->setAnimation(Animation::JUMP);
    break;
  default:
    m_Animate->setAnimation(Animation::IDLE);
    break;
  }
}