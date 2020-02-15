#ifndef _SCREENSHOT_H
#define _SCREENSHOT_H

#include <windows.h>

//RAII class for screenshot bitmap

struct screenshot {
  HWND m_capture_hwnd;
  HDC m_hdc_memory;
  HBITMAP hbmp;
  void *bitmap_data;
  size_t m_width_step;
  int m_x, m_y, m_width, m_height;
  
  screenshot(HWND capture_hwnd, int x, int y, int width, int height);
  ~screenshot();

  void refresh();
  const void *const get_bitmap_data() const { return bitmap_data; }
  size_t width_step() const { return m_width_step; }
};

#endif
