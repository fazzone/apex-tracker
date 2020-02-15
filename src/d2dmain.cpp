#include "d2d.h"
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <sstream>

#include <dlib/image_io.h>

#include "screenshot.h"

using namespace std::chrono_literals;

int WINAPI WinMain(
                   HINSTANCE /* hInstance */,
                   HINSTANCE /* hPrevInstance */,
                   LPSTR /* lpCmdLine */,
                   int /* nCmdShow */
                   )
{
  // Use HeapSetInformation to specify that the process should
  // terminate if the heap manager detects an error in any heap used
  // by the process.
  // The return value is ignored, because we want to continue running in the
  // unlikely event that HeapSetInformation fails.
  HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

  AllocConsole();
  freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

  HWND capture = NULL; 

  for (HWND hwnd = GetTopWindow(NULL); hwnd != NULL; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) {   
    if (!IsWindowVisible(hwnd)) {
      continue;
    }

    int length = GetWindowTextLength(hwnd);
    if (length == 0)
      continue;

    LPSTR title = new char[length+1];
    GetWindowText(hwnd, title, length+1);
    
    std::string ts(title);
    if (ts.rfind("emacs", 0) == 0) {
      capture = hwnd;
    }

  }


  const int s_w = 640, s_h = 480;

  screenshot ss(capture, s_w, s_h);

  dlib::array2d<dlib::bgr_pixel> dimg;
  dimg.set_size(s_h, s_w);
  for (int r = 0; r < s_h; r++) {
    unsigned char *row = (unsigned char *)ss.get_bitmap_data() + (r * ss.width_step());
    for (int c = 0; c < s_w; c++) {
      unsigned char *pix = row + (4 * c);
      dlib::assign_pixel(dimg[r][c], dlib::rgb_pixel(pix[2], pix[1], pix[0]));
    }
  }
  dlib::save_png(dimg, "cannot_into_mem_mgmt.png");


  if (SUCCEEDED(CoInitialize(NULL)))
    {
      {
        DemoApp app(capture);

        if (SUCCEEDED(app.Initialize()))
          {
            screenshot mss(capture, s_w, s_h);
            std::thread clock_pub([&app, &mss, s_w, s_h] {
                                    for  (int i = 0; ; i++) {
                                      std::ostringstream oss;
                                      oss <<"screenshot" <<i <<".png";
                                      printf("Hello from the thread\n");
                                      
                                      mss.refresh();
                                      dlib::array2d<dlib::bgr_pixel> dimg;
                                      dimg.set_size(s_h, s_w);
                                      for (int r = 0; r < s_h; r++) {
                                        unsigned char *row = (unsigned char *)mss.get_bitmap_data() + (r * mss.width_step());
                                        for (int c = 0; c < s_w; c++) {
                                          unsigned char *pix = row + (4 * c);
                                          dlib::assign_pixel(dimg[r][c], dlib::rgb_pixel(pix[2], pix[1], pix[0]));
                                        }
                                      }
                                      dlib::save_png(dimg, oss.str());

                                      SendMessage(app.get_hwnd(), WM_APP, 0, 0);
                                      std::this_thread::sleep_for(1s);
                                    }
                                  });


            app.RunMessageLoop();
          }
      }
      CoUninitialize();
    }

  return 0;
}
