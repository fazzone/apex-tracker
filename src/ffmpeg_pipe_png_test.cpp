#include <dlib/matrix.h>
#include <dlib/image_io.h>
#include <dlib/image_transforms.h>
#include <dlib/data_io.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_keypoint/draw_surf_points.h>
#include <dlib/cmd_line_parser.h>
#include <dlib/image_keypoint.h>
#include <dlib/graph_utils.h>
#include <dlib/sqlite.h>

#include <io.h>
#include <fcntl.h> 
#include <cstdio>

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

#include "map_matcher.h"

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

    string surf_path = get_option(parser, "s", "surf.dat");
    cout <<"Loading SURF data from " <<surf_path <<"...";
    std::vector<surf_point> map_surf_points;
    deserialize(surf_path) >> map_surf_points;

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

    
    array2d<bgr_pixel> path_img;
    assign_image(path_img, map_image);

    point last_fix_position(0, 0);
    bool good = false;
    image_window path_window(map_image);

    database db(":memory:");
    db.exec("create table fix_result(id integer primary key, inserted_at datetime, x integer, y integer)");
    map_matcher matcher(db, map_surf_points, section_width, section_height);

    while (in) {
      in.read(buffer, buffer_size);

      if (1) {
        std::vector<surf_point> sub_surf_points = get_surf_points(subimage);

        auto r = matcher.find_match(sub_surf_points);
        good = r.good;
        if (good) {
          point new_position(r.x, r.y);
          path_window.add_overlay(image_display::overlay_line(last_fix_position, new_position, bgr_pixel(255, 255, 255)));
          last_fix_position = new_position;
          draw_solid_circle(path_img, last_fix_position, 2.0, bgr_pixel(255, 255, 255));
          path_window.add_overlay(image_display::overlay_circle(last_fix_position, 4, rgb_pixel(255,255,255)));
        }

      }

      frame++;
    }
    path_window.wait_until_closed();

    return 0;
  } catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
