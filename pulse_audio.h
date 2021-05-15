#ifndef PULSE_AUDIO_H     //header guard
#define PULSE_AUDIO_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/gccmacro.h>

/* The sample type to use */
static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
};

  /* A simple routine calling UNIX write() in a loop */
static ssize_t loop_write(int fd, const void*data, size_t size) {
  ssize_t ret = 0;
  while (size > 0) {
    ssize_t r;
    if ((r = write(fd, data, size)) < 0)
      return r;

    if (r == 0)
      break;

    ret += r;
    data = (const uint8_t*) data + r;
    size -= (size_t) r;
  }

  return ret;
}

#endif
