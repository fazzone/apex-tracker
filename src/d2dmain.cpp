#include "d2d.h"
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <chrono>

#include <dlib/data_io.h>
#include <dlib/image_processing.h>
#include <dlib/sqlite.h>

#include "map_matcher.h"

#include "screenshot.h"
#include "messages.h"
#include "schema.h"

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

  // for (HWND hwnd = GetTopWindow(NULL); hwnd != NULL; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) {   
  //   if (!IsWindowVisible(hwnd)) {
  //     continue;
  //   }

  //   int length = GetWindowTextLength(hwnd);
  //   if (length == 0)
  //     continue;

  //   LPSTR title = new char[length+1];
  //   GetWindowText(hwnd, title, length+1);
    
  //   std::string ts(title);
  //   if (ts.rfind("Apex", 0) == 0) {
  //     capture = hwnd;
  //     std::cout <<"Capturing window: " <<ts.c_str() <<std::endl;
  //     //break;
  //   }

  // }

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

  std::vector<dlib::surf_point> map_surf_points;
  dlib::deserialize("s4launch_small.surf.dat") >> map_surf_points;

  screenshot mss(capture, s_x, s_y, s_w, s_h);

  dlib::database db("tracker.db");
  dlib::statement st_table_exists(db, "select count(*) from sqlite_master where name = ?");
  st_table_exists.bind(1, std::string("fix_result"));
  st_table_exists.exec();
  st_table_exists.move_next();
  int table_count = 0;
  st_table_exists.get_column(0, table_count);
  std::cout <<"table count? " <<table_count <<std::endl;
  if (0 == table_count) {
    db.exec(schema_tables::create_fix_result);
  }
  
  map_matcher matcher(db, map_surf_points, s_w, s_h);

  message::AddMapPoint amp;
  if (SUCCEEDED(CoInitialize(NULL)))
    {
      {
        DemoApp app(capture);

        if (SUCCEEDED(app.Initialize()))
          {
            screenshot mss(capture, s_x, s_y, s_w, s_h);
            std::thread clock_pub([&app, &mss, &amp, &map_surf_points, &matcher, s_w, s_h] {
                                    dlib::array2d<dlib::bgr_pixel> dimg;
                                    dimg.set_size(s_h, s_w);

                                    for  (int i = 0; ; i++) {
                                      std::this_thread::sleep_for(0.3s);

                                      mss.refresh();
                                      for (int r = 0; r < s_h; r++) {
                                        unsigned char *row = (unsigned char *)mss.get_bitmap_data() + (r * mss.width_step());
                                        for (int c = 0; c < s_w; c++) {
                                          unsigned char *pix = row + (4 * c);
                                          dlib::assign_pixel(dimg[r][c], dlib::rgb_pixel(pix[2], pix[1], pix[0]));
                                        }
                                      }

                                      std::vector<dlib::surf_point> sub_surf_points = dlib::get_surf_points(dimg);

                                      auto r = matcher.find_match(sub_surf_points);
                                      if (r.good) {
                                        amp.x = r.x;
                                        amp.y = r.y;
                                      
                                        SendMessage(app.get_hwnd(), message::AddMapPoint::message_type, (LPARAM)(&amp), 0);
                                      }
                                    }
                                  });


            app.RunMessageLoop();
          }
      }
      CoUninitialize();
    }

  return 0;
}
