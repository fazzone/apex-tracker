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

#include <io.h>
#include <fcntl.h> 

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

#include "surf_match.h"

using namespace std;
using namespace dlib;
namespace fs = std::filesystem;

struct fix_result {
  bool good;
  point position;
};
  

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
    double search_radius = 1e9;

    int n_sub_surf = 1000;

    image_window path_window(map_image);

    while (in) {
      in.read(buffer, buffer_size);

      //if (frame > 300 && frame % 5 == 0) {
      if (1) {
        cout <<"Finding SURF points in subimage...";
        std::vector<surf_point> sub_surf_points = get_surf_points(subimage, n_sub_surf);
        cout <<"done (" <<sub_surf_points.size() <<" points)" <<endl;

        std::vector<surf_point> compare_points;
        for (int i = 0; i < map_surf_points.size(); i++) {
          double d = length_squared(last_fix_position - map_surf_points[i].p.center);
          if (d < search_radius * search_radius) {
            compare_points.push_back(map_surf_points[i]);
          }
        }
        if (compare_points.size() == 0)
          compare_points = map_surf_points;
        
        cout <<"Comparing against " <<compare_points.size() <<" descriptors..." <<endl;
        std::vector<std::pair<size_t, size_t> > matched_points = match_surf_points(sub_surf_points, compare_points);
    
        std::vector<point> sub_points, map_points;
        for (auto it = begin(matched_points); it != end(matched_points); ++it) {
          sub_points.push_back(sub_surf_points[it->first].p.center);
          map_points.push_back(compare_points[it->second].p.center);
        }

        if (sub_points.size() < 3) {
          cout <<"Not enough matched points" <<endl;

          // ostringstream sfn;
          // sfn <<"hiccup"  <<frame <<".png";
          // save_png(path_img, sfn.str());
          // sfn <<".minimap.png";
          // save_png(subimage, sfn.str());
          
          search_radius *= 1.5;
          n_sub_surf = 1000;
          continue;
        }

        auto transform = find_affine_transform(sub_points, map_points);
        point sub_center(section_width/2, section_height/2);
        last_fix_position = transform(sub_center);

        search_radius = 180;
        n_sub_surf = 50;
        cout <<"===== Fix position " <<last_fix_position.x() <<", " <<last_fix_position.y() <<endl;
        path_window.add_overlay(image_display::overlay_circle(last_fix_position, 4, rgb_pixel(255,255,255)));

        //array2d<bgr_pixel> draw_img;
        //assign_image(draw_img, map_image);
        //auto rect_transform = rectangle_transform(transform);
        //auto section_rect = rectangle(0, 0, section_width, section_height);
        //draw_rectangle(draw_img, rect_transform(section_rect), bgr_pixel(255, 255, 255), 4);
        //draw_solid_circle(draw_img, transform(sub_center), 8.0, bgr_pixel(255, 255, 255));

        draw_solid_circle(path_img, last_fix_position, 2.0, bgr_pixel(255, 255, 255));
      
        // if (frame % 100 == 0) {
        //   ostringstream sfn;
        //   sfn <<"path"  <<frame <<".png";
        //   save_png(path_img, sfn.str());
        // }
      }

      frame++;
    }

    return 0;
  } catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
