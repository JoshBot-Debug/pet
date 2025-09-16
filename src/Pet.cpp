#include "Pet.h"

#include <gif_lib.h>
#include <string>

#include "CLI11.h"
#include "Utility.h"

static const std::string HOME_DIR = getHomeDirectory();

int Pet::initialize(int argc, char *argv[]) {

  std::string filePath;

  // CLI
  {
    CLI::App app{"Secure Shell Remote Desktop"};

    app.set_help_flag("--help", "Display help information.");

    app.add_option("-p,--pet", filePath,
                   "The IP address of the destination server eg. 127.0.0.1")
        ->required();

    CLI11_PARSE(app, argc, argv);
  }

  loadGif(filePath.c_str());

  // Main loop
  window();

  return EXIT_SUCCESS;
}

static void onKeyPress(GLFWwindow *window, int key, int scancode, int action,
                       int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    return glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Pet::SetViewport(int x, int y, int w, int h) {
  m_Viewport.x = x;
  m_Viewport.y = y;
  m_Viewport.w = w;
  m_Viewport.h = h;
}

void Pet::window() {
  Window w;

  w.initialize({
      .data = this,
      .width = GetWidth(),
      .height = GetHeight(),
      .onKeyPress = onKeyPress,
  });

  while (!w.shouldClose()) {
    update(w);

    const Frame &frame = m_Frames[m_CurrentFrame];
    w.present(frame.pixels, frame.width, frame.height);
  }
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

  // Increment frame
  {
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                       now - m_LastFrameTime)
                       .count();
    if (elapsed >= m_Frames[m_CurrentFrame].delay) {
      m_CurrentFrame = (m_CurrentFrame + 1) % m_Frames.size();
      m_LastFrameTime = now;
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

static void drawFrameOntoCanvas(std::vector<uint8_t> &canvas, GifFileType *gif,
                                SavedImage &img, int transparentColor) {

  GifImageDesc &desc = img.ImageDesc;
  ColorMapObject *map = desc.ColorMap ? desc.ColorMap : gif->SColorMap;

  for (int y = 0; y < desc.Height; y++) {
    for (int x = 0; x < desc.Width; x++) {
      int srcIndex = y * desc.Width + x;
      int dstX = desc.Left + x;
      int dstY = desc.Top + y;

      if (dstX >= gif->SWidth || dstY >= gif->SHeight)
        continue;

      int dstIndex = (dstY * gif->SWidth + dstX) * 4;
      int colorIndex = img.RasterBits[srcIndex];

      if (colorIndex == transparentColor)
        continue;

      GifColorType color = map->Colors[colorIndex];
      canvas[dstIndex + 0] = color.Red;
      canvas[dstIndex + 1] = color.Green;
      canvas[dstIndex + 2] = color.Blue;
      canvas[dstIndex + 3] = 255;
    }
  }
}

static void clearBackground(std::vector<uint8_t> &canvas, GifImageDesc &desc,
                            int canvasWidth, int canvasHeight) {
  for (int y = 0; y < desc.Height; y++) {
    int dstY = desc.Top + y;
    if (dstY >= canvasHeight)
      continue;

    for (int x = 0; x < desc.Width; x++) {
      int dstX = desc.Left + x;
      if (dstX >= canvasWidth)
        continue;

      int dstIndex = (dstY * canvasWidth + dstX) * 4;

      canvas[dstIndex + 0] = 0;
      canvas[dstIndex + 1] = 0;
      canvas[dstIndex + 2] = 0;
      canvas[dstIndex + 3] = 0;
    }
  }
}

void Pet::loadGif(const char *filename) {
  int error;
  GifFileType *gif = DGifOpenFileName(filename, &error);
  if (!gif)
    throw std::runtime_error("Failed to open GIF");

  DGifSlurp(gif);

  int pDisposalMode = 0;
  GifImageDesc pImageDesc = {};
  std::vector<uint8_t> canvas(gif->SWidth * gif->SHeight * 4, 0);

  for (int i = 0; i < gif->ImageCount; i++) {
    SavedImage &img = gif->SavedImages[i];

    int delay = 0;
    int disposalMode = 0;
    int transparentColor = -1;

    for (int j = 0; j < img.ExtensionBlockCount; j++) {
      if (img.ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE) {
        GraphicsControlBlock gcb;
        DGifExtensionToGCB(img.ExtensionBlocks[j].ByteCount,
                           img.ExtensionBlocks[j].Bytes, &gcb);
        delay = gcb.DelayTime * 10; // ms
        transparentColor = gcb.TransparentColor;
        disposalMode = gcb.DisposalMode;
      }
    }

    switch (pDisposalMode) {
    case DISPOSE_DO_NOT:
      break;

    case DISPOSE_BACKGROUND:
      clearBackground(canvas, pImageDesc, gif->SWidth, gif->SHeight);
      break;

    default:
      std::memset(canvas.data(), 0, gif->SWidth * gif->SHeight * 4);
      break;
    }

    drawFrameOntoCanvas(canvas, gif, img, transparentColor);

    m_Frames.emplace_back(Frame{
        .pixels = canvas,
        .width = static_cast<uint32_t>(gif->SWidth),
        .height = static_cast<uint32_t>(gif->SHeight),
        .delay = static_cast<uint32_t>(delay),
    });

    pImageDesc = img.ImageDesc;
    pDisposalMode = disposalMode;
  }

  DGifCloseFile(gif, &error);
}