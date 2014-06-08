#include "libretro.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdint.h>
#include "rpng.h"
static retro_log_printf_t log_cb;
static retro_video_refresh_t video_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;

static uint32_t* image_buffer;
static unsigned image_width;
static unsigned image_height;


void retro_get_system_info(struct retro_system_info *info)
{
   info->library_name = "image display";
   info->library_version = "v0.0.1";
   info->need_fullpath = true;
   info->block_extract = false;
   info->valid_extensions = "png";
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   info->geometry.base_width = image_width ;
   info->geometry.base_height = image_height ;
   info->geometry.max_width = image_width ;
   info->geometry.max_height = image_height ;
   info->geometry.aspect_ratio = 0;
   info->timing.fps = 60.0;
   info->timing.sample_rate = 44100.0;
}

void retro_init()
{
   struct retro_log_callback log;

   if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
      log_cb = log.log;
   else
      log_cb = NULL;

   image_buffer = NULL;
   image_width = 0;
   image_height = 0;

}

void retro_deinit()
{
   if (image_buffer)
      free(image_buffer);
   image_buffer = NULL;
   image_width = 0;
   image_height = 0;
}

void retro_set_environment(retro_environment_t cb)
{
   environ_cb = cb;

   static const struct retro_variable vars[] = {
      { NULL, NULL },
   };

   cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars);
}

void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }
void retro_set_audio_sample(retro_audio_sample_t) { }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) { audio_batch_cb = cb; }
void retro_set_input_poll(retro_input_poll_t cb) { input_poll_cb = cb; }
void retro_set_input_state(retro_input_state_t cb) { input_state_cb = cb; }

void retro_set_controller_port_device(unsigned, unsigned) {}

void retro_reset()
{
   if (image_buffer)
      free(image_buffer);
   image_buffer = NULL;
   image_width = 0;
   image_height = 0;
}

size_t retro_serialize_size()
{
   return 0;
}

bool retro_serialize(void *data, size_t size)
{
   (void)data;
   (void)size;
   return false;
}

bool retro_unserialize(const void *data, size_t size)
{
   (void)data;
   (void)size;
   return false;
}

void retro_cheat_reset() {}
void retro_cheat_set(unsigned, bool, const char *) {}

bool retro_load_game(const struct retro_game_info *info)
{
   rpng_load_image_argb(info->path, &image_buffer, &image_width, &image_height);

   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
      if (log_cb)
         log_cb(RETRO_LOG_INFO, "XRGB8888 is not supported.\n");
      return false;
   }

   return true;
}


bool retro_load_game_special(unsigned, const struct retro_game_info*, size_t) { return false; }

void retro_unload_game()
{
   if (image_buffer)
      free(image_buffer);
}

unsigned retro_get_region() { return RETRO_REGION_NTSC; }

void *retro_get_memory_data(unsigned id)
{
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   return 0;
}


void retro_run()
{
   input_poll_cb();
   video_cb(image_buffer, image_width, image_height, image_width * sizeof(uint32_t));

}

unsigned retro_api_version() { return RETRO_API_VERSION; }

