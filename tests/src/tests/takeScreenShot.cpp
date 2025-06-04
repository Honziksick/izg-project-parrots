#include <tests/takeScreenShot.hpp>
#include <tests/renderMethodFrame.hpp>
#include <tests/testCommon.hpp>
#include <framework/application.hpp>

#include <libs/stb_image/stb_image_write.h>

#include <string>

#ifndef CMAKE_ROOT_DIR
#define CMAKE_ROOT_DIR "."
#endif//CMAKE_ROOT_DIR

void takeScreenShot(){

  auto aframe = tests::createFramebuffer(500,500,true);
  auto&frame = aframe.frame;
  renderMethodFrame(aframe.frame);

  auto file = CMAKE_ROOT_DIR "/screenshot.png";
  stbi_write_png(file,frame.width,frame.height,frame.color.channels,frame.color.data,0);

  std::cerr << "storing screenshot to: \"" << file << "\"" << std::endl;
}
