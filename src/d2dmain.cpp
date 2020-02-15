#include "d2d.h"
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <sstream>
#include <mutex>

#include <dlib/image_io.h>
#include <dlib/geometry.h>
#include <dlib/data_io.h>
#include <dlib/image_processing.h>

#include "surf_match.h"

#include "screenshot.h"
#include "messages.h"

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
    if (ts.rfind("Apex", 0) == 0) {
      capture = hwnd;
      std::cout <<"Capturing window: " <<ts.c_str() <<std::endl;
      //break;
    }

  }

  const int fw = 2560, fh = 1440;
  const int screenw = GetSystemMetrics(SM_CXSCREEN), screenh = GetSystemMetrics(SM_CYSCREEN);
  const int fs_x = 67, fs_y = 79, fs_w = 319, fs_h = 294;
  const double
    fs_x_ratio = (double)fs_x / fw,
    fs_y_ratio = (double)fs_y / fh,
    fs_w_ratio = (double)fs_w / fw,
    fs_h_ratio = (double)fs_h / fh;
  const int
    s_x = (int)(fs_x_ratio * screenw),
    s_y = (int)(fs_y_ratio * screenh),
    s_w = (int)(fs_w_ratio * screenw),
    s_h = (int)(fs_h_ratio * screenh);

  //const int s_x = 0, s_y = 0, s_w = 320, s_h = 320;
  std::vector<dlib::surf_point> map_surf_points;
  dlib::deserialize("s4launch_small.surf.dat") >> map_surf_points;

  screenshot mss(capture, s_x, s_y, s_w, s_h);

  message::AddMapPoint amp;
  if (SUCCEEDED(CoInitialize(NULL)))
    {
      {
        DemoApp app(capture);

        if (SUCCEEDED(app.Initialize()))
          {
            screenshot mss(capture, s_x, s_y, s_w, s_h);
            std::thread clock_pub([&app, &mss, &amp, &map_surf_points, s_w, s_h] {
                                    for  (int i = 0; ; i++) {
                                      std::cout  <<"Hello " <<i <<std::endl;
                                      std::this_thread::sleep_for(0.2s);
                                      std::cout  <<"Hello " <<i <<std::endl;


                                      std::ostringstream oss;
                                      oss <<"apx screenshot" <<i <<".png";
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

                                      std::vector<dlib::surf_point> sub_surf_points = dlib::get_surf_points(dimg);
                                      std::vector<std::pair<size_t, size_t> > matched_points = match_surf_points(sub_surf_points, map_surf_points);
                                      std::vector<dlib::point> sub_points, map_points;
                                      for (auto it = begin(matched_points); it != end(matched_points); ++it) {
                                        sub_points.push_back(sub_surf_points[it->first].p.center);
                                        map_points.push_back(map_surf_points[it->second].p.center);
                                      }

                                      // if (i % 5 == 0) {
                                      //   dlib::save_png(dimg, oss.str());
                                      //   std::cout <<"Saved " <<oss.str() <<std::endl;
                                      // }

                                      if (sub_points.size() < 3) {
                                        std::cout <<"Not enough matched points" <<std::endl;
                                        continue;
                                      }
                                      
                                      auto transform = dlib::find_affine_transform(sub_points, map_points);
                                      dlib::point sub_center(s_w/2, s_h/2);
                                      dlib::point new_position = transform(sub_center);
                                      


                                      amp.x = new_position(0);
                                      amp.y = new_position(1);
                                      
                                      SendMessage(app.get_hwnd(), message::AddMapPoint::message_type, (LPARAM)(&amp), 0);

                                    }
                                  });


            app.RunMessageLoop();
          }
      }
      CoUninitialize();
    }

  return 0;
}
