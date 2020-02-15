#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <utility>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

template<class I>
inline void SafeRelease(I **ppi) {
  if (!*ppi)
    return;
  (*ppi)->Release();
  (*ppi) = NULL;
}

// struct MapPoint {
//   float x, y;
//   MapPoint(float xx, float yy) : x(xx), y(yy) {}
// };


class DemoApp
{
public:
  DemoApp(HWND capture_hwnd);
  ~DemoApp();

  // Register the window class and call methods for instantiating drawing resources
  HRESULT Initialize();

  // Process and dispatch messages
  void RunMessageLoop();

  HWND get_hwnd() { return m_hwnd; }
  void add_point(float x, float y) {
    m_draw_points.push_back(D2D1::Point2F(x, y));
  }

private:
  // Initialize device-independent resources.
  HRESULT CreateDeviceIndependentResources();

  // Initialize device-dependent resources.
  HRESULT CreateDeviceResources();

  HRESULT DemoApp::LoadBitmapFromFile(ID2D1RenderTarget *pRenderTarget,
                                      IWICImagingFactory *pIWICFactory,
                                      PCWSTR uri,
                                      UINT destinationWidth,
                                      UINT destinationHeight,
                                      ID2D1Bitmap **ppBitmap
                                      );


  // Release device-dependent resource.
  void DiscardDeviceResources();

  // Draw content.
  HRESULT OnRender();

  // Resize the render target.
  void OnResize(UINT width, UINT height);

  // The windows procedure.
  static LRESULT CALLBACK WndProc(HWND hWnd,
                                  UINT message,
                                  WPARAM wParam,
                                  LPARAM lParam
                                  );

  std::vector<D2D1_POINT_2F> m_draw_points;

  HWND m_capture_hwnd;

  HWND m_hwnd;
  ID2D1Factory* m_pDirect2dFactory;
  IWICImagingFactory *m_pWICFactory;
  ID2D1HwndRenderTarget* m_pRenderTarget;
  ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
  ID2D1SolidColorBrush* m_pCornflowerBlueBrush;
  ID2D1Bitmap *m_pBitmap;
};



