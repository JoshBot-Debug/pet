#include "GIF.h"

#include <cstring>
#include <gif_lib.h>
#include <stdexcept>
#include <stdint.h>

namespace GIF {
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

static std::vector<Frame> load(const char *filename) {
  std::vector<Frame> frames = {};

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

    frames.emplace_back(Frame{
        .pixels = canvas,
        .width = static_cast<uint32_t>(gif->SWidth),
        .height = static_cast<uint32_t>(gif->SHeight),
        .delay = static_cast<uint32_t>(delay),
    });

    pImageDesc = img.ImageDesc;
    pDisposalMode = disposalMode;
  }

  DGifCloseFile(gif, &error);

  return frames;
}

std::vector<std::vector<Frame>>
load(const std::vector<std::string> &filePaths) {

  std::vector<std::vector<Frame>> frames = {};

  for (size_t i = 0; i < filePaths.size(); i++)
    frames.push_back(load(filePaths[i].c_str()));

  return frames;
}

} // namespace GIF
