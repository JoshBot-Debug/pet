#include "Pet.h"

#include <string>

#include "CLI11.h"
#include "GIF/GIF.h"
#include "Utility.h"

static const std::string HOME_DIR = getHomeDirectory();

static void onKeyPress(GLFWwindow *window, int key, int scancode, int action,
                       int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    return glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int Pet::initialize(int argc, char *argv[]) {

  std::vector<std::string> idle;
  std::vector<std::string> walk;
  std::vector<std::string> run;
  std::vector<std::string> jump;
  std::vector<std::string> sit;
  std::vector<std::string> extra;

  // CLI
  {
    CLI::App app{"Ubuntu Pet"};

    app.set_help_flag("--help", "Display help information.");

    app.add_option("--idle", idle, "The idle animations")->required();
    app.add_option("--walk", walk, "The walk animations")->required();
    app.add_option("--run", run, "The run animations")->required();
    app.add_option("--jump", jump, "The jump animations")->required();
    app.add_option("--sit", sit, "The sit animations")->required();
    app.add_option("--extra", extra, "Extra animations")->required();

    CLI11_PARSE(app, argc, argv);
  }

  // Load gif animations
  {
    m_Animate.addAnimation(std::pair{"idle", GIF::load(idle)});
    m_Animate.addAnimation(std::pair{"walk", GIF::load(walk)});
    m_Animate.addAnimation(std::pair{"run", GIF::load(run)});
    m_Animate.addAnimation(std::pair{"jump", GIF::load(jump)});
    m_Animate.addAnimation(std::pair{"sit", GIF::load(sit)});
    m_Animate.addAnimation(std::pair{"extra", GIF::load(extra)});
  }

  // Main loop
  {
    Window w;

    m_Animate.setWindow(&w);

    m_Animate.setAnimation("idle");

    w.initialize({
        .data = this,
        .onKeyPress = onKeyPress,
    });

    while (!w.shouldClose()) {
      update(w);

      const Frame &frame = m_Animate.getFrame();
      w.present(frame.pixels, frame.width, frame.height);
    }
  }

  return EXIT_SUCCESS;
}

void Pet::update(Window &w) {
  GLFWwindow *window = w.getWindow();

  auto now = std::chrono::steady_clock::now();

  double deltaTime = 0;

  // Delta time
  {
    deltaTime = std::chrono::duration<double>(now - m_LastTime).count();
    m_LastTime = now;
  }

  m_Animate.update(deltaTime);
}
