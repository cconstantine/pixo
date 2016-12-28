#include <realsense_reader.hpp>

#include <glm/gtx/string_cast.hpp>
#include <fstream>


RealsenseReader::RealsenseReader() :
 renderedTexture(640, 480),
 depthTexture(480, 360)
{
  printf("There are %d connected RealSense devices.\n", ctx.get_device_count());

  // This tutorial will access only a single device, but it is trivial to extend to multiple devices
  dev = ctx.get_device(0);
  printf("\nUsing device 0, an %s\n", dev->get_name());
  printf("    Serial number: %s\n", dev->get_serial());
  printf("    Firmware version: %s\n", dev->get_firmware_version());

  dev->enable_stream(rs::stream::depth, rs::preset::best_quality);
  dev->enable_stream(rs::stream::color, rs::preset::best_quality);
  dev->set_option(rs::option::r200_lr_auto_exposure_enabled, 1.0);

  dev->start();
}

const Texture& RealsenseReader::getTexture() {
  return renderedTexture;
}

const Texture& RealsenseReader::getDepthTexture() {
  return depthTexture;
}

void RealsenseReader::render() {
  dev->wait_for_frames();
  const uint8_t * color_image = (const uint8_t *)dev->get_frame_data(rs::stream::color);

  glBindTexture(GL_TEXTURE_2D, renderedTexture.id);

  glTexSubImage2D( GL_TEXTURE_2D,
    0,
    0,
    0,
    640,
    480,
    GL_RGB,
    GL_UNSIGNED_BYTE,
    color_image);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glBindTexture( GL_TEXTURE_2D, NULL );


  {
    const uint16_t * depth_image = (const uint16_t *)dev->get_frame_data(rs::stream::depth);
    uint16_t max = 0;
    uint16_t min = 0xffff;
    for(int i = 0;i < depthTexture.width*depthTexture.height;i++) {
      if (depth_image[i] > max && depth_image[i] != 0xffff) {
        max = depth_image[i];
      }
      if (depth_image[i] < min && depth_image[i] != 0x0000) {
        min = depth_image[i];
      }
    }
    ALOGV("Depth: %4x, %4x\n", min, max);
  }

  // uint16_t depth_image[depthTexture.width* depthTexture.height];
  // for(int i = 0; i < depthTexture.width* depthTexture.height;i++) {
  //   depth_image[i] = (uint16_t)0x00FF;
  // }
  
  const uint16_t * depth_image = (const uint16_t *)dev->get_frame_data(rs::stream::depth);

  glBindTexture(GL_TEXTURE_2D, depthTexture.id);
  glTexImage2D(
   GL_TEXTURE_2D,
   0,
   GL_DEPTH_COMPONENT,
   depthTexture.width,
   depthTexture.height,
   0,
   GL_DEPTH_COMPONENT,
   GL_UNSIGNED_SHORT,
   depth_image);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glBindTexture( GL_TEXTURE_2D, NULL );

}

