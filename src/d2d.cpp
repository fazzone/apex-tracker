#include "d2d.h"

#include <cstdio>

DemoApp::DemoApp(HWND capture) :
  m_capture_hwnd(capture),
  m_hwnd(NULL),
  m_pDirect2dFactory(NULL),
  m_pRenderTarget(NULL),
  m_pLightSlateGrayBrush(NULL),
  m_pCornflowerBlueBrush(NULL)
{
}

    
DemoApp::~DemoApp()
{
  SafeRelease(&m_pDirect2dFactory);
  SafeRelease(&m_pRenderTarget);
  SafeRelease(&m_pLightSlateGrayBrush);
  SafeRelease(&m_pCornflowerBlueBrush);
}

void DemoApp::RunMessageLoop()
{
  MSG msg;

  while (GetMessage(&msg, NULL, 0, 0))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
}


HRESULT DemoApp::Initialize()
{
  HRESULT hr;

  // Initialize device-indpendent resources, such
  // as the Direct2D factory.
  hr = CreateDeviceIndependentResources();

  if (SUCCEEDED(hr))
    {
      // Register the window class.
      WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
      wcex.style         = CS_HREDRAW | CS_VREDRAW;
      wcex.lpfnWndProc   = DemoApp::WndProc;
      wcex.cbClsExtra    = 0;
      wcex.cbWndExtra    = sizeof(LONG_PTR);
      wcex.hInstance     = HINST_THISCOMPONENT;
      wcex.hbrBackground = NULL;
      wcex.lpszMenuName  = NULL;
      wcex.hCursor       = LoadCursor(NULL, IDI_APPLICATION);
      wcex.lpszClassName = "D2DDemoApp";

      RegisterClassEx(&wcex);

      // Create the window.
      m_hwnd = CreateWindow(
                            "D2DDemoApp",
                            "Direct2D Demo App",
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            1000,
                            1000,
                            NULL,
                            NULL,
                            HINST_THISCOMPONENT,
                            this
                            );
      hr = m_hwnd ? S_OK : E_FAIL;
      if (SUCCEEDED(hr))
        {
          ShowWindow(m_hwnd, SW_SHOWNORMAL);
          UpdateWindow(m_hwnd);
        }
    }

  return hr;
}

HRESULT DemoApp::CreateDeviceIndependentResources()
{
  HRESULT hr = S_OK;

  // Create a Direct2D factory.
  hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
  if (SUCCEEDED(hr)) {
    hr = CoCreateInstance(CLSID_WICImagingFactory,
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          IID_IWICImagingFactory,
                          reinterpret_cast<void **>(&m_pWICFactory)
                          );
  }

  return hr;
}

HRESULT DemoApp::LoadBitmapFromFile(
                                    ID2D1RenderTarget *pRenderTarget,
                                    IWICImagingFactory *pIWICFactory,
                                    PCWSTR uri,
                                    UINT destinationWidth,
                                    UINT destinationHeight,
                                    ID2D1Bitmap **ppBitmap
                                    )
{
  IWICBitmapDecoder *pDecoder = NULL;
  IWICBitmapFrameDecode *pSource = NULL;
  IWICStream *pStream = NULL;
  IWICFormatConverter *pConverter = NULL;
  IWICBitmapScaler *pScaler = NULL;

  HRESULT hr = pIWICFactory->CreateDecoderFromFilename(
                                                       uri,
                                                       NULL,
                                                       GENERIC_READ,
                                                       WICDecodeMetadataCacheOnLoad,
                                                       &pDecoder
                                                       );
  if (SUCCEEDED(hr)) {
    // Create the initial frame.
    hr = pDecoder->GetFrame(0, &pSource);
  }
  if (SUCCEEDED(hr)) {
    // Convert the image format to 32bppPBGRA
    // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
    hr = pIWICFactory->CreateFormatConverter(&pConverter);
  }

  if (SUCCEEDED(hr)) {
    hr = pConverter->Initialize(pSource,
                                GUID_WICPixelFormat32bppPBGRA,
                                WICBitmapDitherTypeNone,
                                NULL,
                                0.f,
                                WICBitmapPaletteTypeMedianCut
                                );
  }

  if (SUCCEEDED(hr)) {
    // Create a Direct2D bitmap from the WIC bitmap.
    hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, ppBitmap);
  }

  SafeRelease(&pDecoder);
  SafeRelease(&pSource);
  SafeRelease(&pStream);
  SafeRelease(&pConverter);
  SafeRelease(&pScaler);

  return hr;
}

HRESULT DemoApp::CreateDeviceResources()
{
  HRESULT hr = S_OK;

  if (!m_pRenderTarget)
    {
      RECT rc;
      GetClientRect(m_hwnd, &rc);

      D2D1_SIZE_U size = D2D1::SizeU(
                                     rc.right - rc.left,
                                     rc.bottom - rc.top
                                     );

      // Create a Direct2D render target.
      hr = m_pDirect2dFactory->CreateHwndRenderTarget(
                                                      D2D1::RenderTargetProperties(),
                                                      D2D1::HwndRenderTargetProperties(m_hwnd, size),
                                                      &m_pRenderTarget
                                                      );


      if (SUCCEEDED(hr))
        {
          // Create a gray brush.
          hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightSlateGray),&m_pLightSlateGrayBrush);
        }
      if (SUCCEEDED(hr))
        {
          // Create a blue brush.
          hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::CornflowerBlue),&m_pCornflowerBlueBrush);
        }


      if (SUCCEEDED(hr)) {
        ID2D1Bitmap *bitmap = NULL;
        HRESULT hrb = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory,
                                         L".\\res\\worlds_edge.png",
                                         2048,
                                         2048,
                                         &m_pBitmap
                                         );
        if (!SUCCEEDED(hrb)) {
          m_pBitmap = NULL;
        }

      }
      
    }

  return hr;
}

void DemoApp::DiscardDeviceResources()
{
  SafeRelease(&m_pRenderTarget);
  SafeRelease(&m_pLightSlateGrayBrush);
  SafeRelease(&m_pCornflowerBlueBrush);
}


LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  LRESULT result = 0;

  if (message == WM_CREATE)
    {
      LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
      DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

      ::SetWindowLongPtrW(hwnd,
                          GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(pDemoApp)
                          );

      result = 1;
    }
  else
    {
      DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

      bool wasHandled = false;

      if (pDemoApp)
        {
          switch (message)
            {
            case WM_SIZE:
              {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                pDemoApp->OnResize(width, height);
              }
              result = 0;
              wasHandled = true;
              break;

            case WM_DISPLAYCHANGE:
              {
                InvalidateRect(hwnd, NULL, FALSE);
              }
              result = 0;
              wasHandled = true;
              break;

            case WM_APP:
              printf("Received WM_APP, repainting\n");

            case WM_PAINT:
              {
                pDemoApp->OnRender();
                ValidateRect(hwnd, NULL);
              }
              result = 0;
              wasHandled = true;
              break;

            case WM_DESTROY:
              {
                PostQuitMessage(0);
              }
              result = 1;
              wasHandled = true;
              break;
            }
        }

      if (!wasHandled)
        {
          result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

  return result;
}


HRESULT DemoApp::OnRender()
{
  HRESULT hr = S_OK;

  hr = CreateDeviceResources();

  if (SUCCEEDED(hr))
    {
      m_pRenderTarget->BeginDraw();

      m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

      m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));



      if (m_pBitmap) {
      D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
        D2D1_SIZE_F img_size = m_pBitmap->GetSize();
        D2D1_POINT_2F pt = D2D1::Point2F(0, 0);
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Scale(rtSize.width / img_size.width, rtSize.height / img_size.height, pt));
        m_pRenderTarget->DrawBitmap(m_pBitmap, D2D1::RectF(0, 0, img_size.width, img_size.height));
      }

      hr = m_pRenderTarget->EndDraw();
    }
  if (hr == D2DERR_RECREATE_TARGET)
    {
      hr = S_OK;
      DiscardDeviceResources();
    }

  return hr;
}

void DemoApp::OnResize(UINT width, UINT height)
{
  if (m_pRenderTarget)
    {
      // Note: This method can fail, but it's okay to ignore the
      // error here, because the error will be returned again
      // the next time EndDraw is called.
      m_pRenderTarget->Resize(D2D1::SizeU(width, height));
    }
}
