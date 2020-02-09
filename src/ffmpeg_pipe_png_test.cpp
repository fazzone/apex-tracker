#include <dlib/matrix.h>
#include <dlib/image_io.h>
#include <dlib/image_transforms.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_keypoint/draw_surf_points.h>
#include <dlib/cmd_line_parser.h>
#include <dlib/image_keypoint.h>
#include <dlib/graph_utils.h>

#include <io.h>
#include <fcntl.h> 

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

using namespace std;
using namespace dlib;
namespace fs = std::filesystem;

int main(int argc, char ** argv) {
  try {
    command_line_parser parser;
    parser.add_option("w","width", 1);
    parser.add_option("h","height", 1);
    parser.add_option("i", "input raw video, use - for stdin", 1);
    parser.add_option("m", "input map png", 1);

    parser.add_option("s", "input surf points", 1);
    
    parser.parse(argc, argv);

    const char *bwd = getenv("BUILD_WORKING_DIRECTORY");
    if (bwd)
      fs::current_path(bwd);
    
    int ncols = get_option(parser, "w", 0), nrows = get_option(parser, "h", 0);

    array2d<bgr_pixel> map_image, img;
    string map_png_path = get_option(parser, "m", "res/worlds_edge.png");
    load_png(map_image, map_png_path);


    set_image_size(img, nrows, ncols);

    if (width_step(img) != ncols * sizeof(bgr_pixel))
      throw "no padding allowed";


    std::vector<surf_point> sp;
    string input_surf_path = get_option(parser, "s", "surf.ser");
    ifstream surf_ifs(input_surf_path, ios::binary);
    deserialize(sp, surf_ifs);
    cout <<"Loaded " <<sp.size() <<" SURF points" <<endl;

    string input_file = get_option(parser, "i", "-");
    std::istream *pin = &cin;
    ifstream ifs;
    if (input_file != "-") {
      ifs.open(input_file, ifstream::binary);
      pin = &ifs;
    } else if (-1 == _setmode(_fileno(stdin), _O_BINARY)) {
      throw "error setting stdin to binary";
    }
    std::istream &in = *pin;

    int buffer_size = sizeof(bgr_pixel) * ncols * nrows;
    char *buffer = (char *)image_data(img);
    int frame = 0;
    string frame_base = "frame";

    int
      section_left = 67,
      section_top = 79,
      section_width = 319,
      section_height = 294;
    
    auto subimage = sub_image(img, rectangle(section_left, section_top, section_left + section_width, section_top + section_height));
    array2d<bgr_pixel> eq_img;
    
    while (in) {
      //cout <<"======Reading " <<buffer_size << " bytes into frame #" <<frame <<endl;
      in.read(buffer, buffer_size);

      // ostringstream sout;
      // sout <<"frame" <<frame <<".png";
      // cout <<"Writing " <<sout.str() <<endl;

      //save_png(eq_img, sout.str());

      if (frame > 300) {
        equalize_histogram(subimage, eq_img);
        std::vector<surf_point> mini_sp = get_surf_points(eq_img, 100, 200.0);
        cout << "number of SURF points found: "<< mini_sp.size() << endl;

        squared_euclidean_distance dc;

        std::vector<surf_point> matched;
        
        for (int i = 0; i < mini_sp.size(); i++) {
          double dt = 0.001;
          int min_d = 99;
          surf_point &best_match = sp[0];
          for (int j = 0; j < sp.size(); j++) {
            int d = 0;
            double delta_angle = mini_sp[i].angle - sp[j].angle;
            if (delta_angle * delta_angle > 0.1)
              continue;

            for (int di = 0; di < 64; di++) {
              double diff = mini_sp[i].des(di, 0) - sp[j].des(di, 0);
              if (diff*diff < dt)
                d += 1;
            }
            if (d < min_d) {
              min_d = d;
              best_match = sp[j];
            }
          }
          cout <<"point " <<i <<" ?maps to " <<best_match.p.center(0) <<", " <<best_match.p.center(1) <<endl;
          matched.push_back(best_match);
        }
        array2d<bgr_pixel> matches_image;
        assign_image(matches_image, map_image);

        // for (unsigned long i = 0; i < matched.size(); ++i)
        //   {
        //     const unsigned long radius = static_cast<unsigned long>(sp[i].p.scale*3);
        //     const point center(sp[i].p.center);
        //     point direction = center + point(radius,0);
        //     // SURF descriptors are rotated by sp[i].angle.  So we want to include a visual
        //     // indication of this rotation on our overlay.
        //     direction = rotate_point(center, direction, sp[i].angle);

            
        //     win.add_overlay(image_display::overlay_circle(center, radius, rgb_pixel(0,255,0)));
        //     // Draw a line showing the orientation of the SURF descriptor.
        //     win.add_overlay(center, direction, rgb_pixel(255,0,0));
        //   }
        
        save_png(subimage, "subimage.png");
        
        image_window my_window(matches_image);
        draw_surf_points(my_window, matched);
        my_window.wait_until_closed();
    
        //return 0;
      }


      frame++;
    }

    return 0;
  } catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
