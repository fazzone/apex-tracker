#include "screenshot.h"
#include <cstdio>

screenshot::screenshot(HWND capture_hwnd, int width, int height) {
  m_capture_hwnd = capture_hwnd;
  hdc_screen = GetDC(NULL);
  hdc_memory = CreateCompatibleDC(hdc_screen);

  void* lpBitmapBits;
  BITMAPINFO bi; 
  ZeroMemory(&bi, sizeof(BITMAPINFO));
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = width;
  bi.bmiHeader.biHeight = -height; //negative so 0,0 in top left
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 32;
  HBITMAP hbmp = CreateDIBSection(hdc_memory, &bi, DIB_RGB_COLORS, &lpBitmapBits, NULL, 0);
  SelectObject(hdc_memory, hbmp);

  PrintWindow(capture_hwnd, hdc_memory, PW_CLIENTONLY);

  BITMAP bitmap;
  GetObjectW(hbmp, sizeof(BITMAP), &bitmap);

    
  m_width_step = bitmap.bmWidthBytes;
  bitmap_data = lpBitmapBits;
}

void screenshot::refresh() {
  
  //PrintWindow(m_capture_hwnd, hdc_memory, PW_CLIENTONLY);
}

screenshot::~screenshot() {
  printf("destruct screenshot %p\n", this);
  DeleteDC(hdc_memory);
  ReleaseDC(NULL, hdc_screen);
  DeleteObject(hbmp);
}
