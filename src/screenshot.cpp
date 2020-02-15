#include "screenshot.h"
#include <cstdio>

screenshot::screenshot(HWND capture_hwnd, int x, int y, int width, int height)
  : m_x(x), m_y(y), m_width(width), m_height(height), m_capture_hwnd(capture_hwnd)
{
  //HDC hdc_window = GetDC(capture_hwnd);
  HDC hdc_window = GetDC(NULL);
  m_hdc_memory = CreateCompatibleDC(hdc_window);
  ReleaseDC(NULL, hdc_window);

  void* lpBitmapBits;
  BITMAPINFO bi; 
  ZeroMemory(&bi, sizeof(BITMAPINFO));
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = width;
  bi.bmiHeader.biHeight = -height; //negative so 0,0 in top left
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 32;
  HBITMAP hbmp = CreateDIBSection(m_hdc_memory, &bi, DIB_RGB_COLORS, &lpBitmapBits, NULL, 0);
  SelectObject(m_hdc_memory, hbmp);

  BITMAP bitmap;
  GetObjectW(hbmp, sizeof(BITMAP), &bitmap);

  refresh();

  m_width_step = bitmap.bmWidthBytes;
  bitmap_data = lpBitmapBits;
}

void screenshot::refresh() {
  //HDC hdc_target = GetDC(m_capture_hwnd);
  HDC hdc_target = GetDC(NULL);

  printf("HDC %p\n", hdc_target);

  BOOL r = BitBlt(m_hdc_memory, 0, 0, m_width, m_height, hdc_target, m_x, m_y, SRCCOPY);
  if (!r) {
    printf("BitBlt failed with error %d\n", GetLastError());
  }

  ReleaseDC(NULL, hdc_target);
}

screenshot::~screenshot() {
  DeleteDC(m_hdc_memory);
  DeleteObject(hbmp);
}
