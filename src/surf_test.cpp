#include <dlib/matrix.h>
#include <dlib/geometry.h>
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
#include <ctime>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cstdlib>

#include "surf_match.h"

using namespace std;
using namespace dlib;
namespace fs = std::filesystem;

// Chooses random minimap-sized sections of the map and tries to find where they came from.

int main(int argc, char ** argv) {
  try {
    command_line_parser parser;
    parser.add_option("i", "input SURF data", 1);
    parser.add_option("m", "input map png", 1);
    parser.add_option("n", "number of iterations", 1);
    
    parser.parse(argc, argv);

    const char *bwd = getenv("BUILD_WORKING_DIRECTORY");
    if (bwd)
      fs::current_path(bwd);
    
    array2d<bgr_pixel> map_image;

    string map_png_path = get_option(parser, "m", "res/worlds_edge.png");
    cout <<"Loading " <<map_png_path <<"...";
    load_png(map_image, map_png_path);
    cout <<"done" <<endl;

    string surf_path = get_option(parser, "i", "surf.dat");
    cout <<"Loading SURF data from " <<surf_path <<"...";
    std::vector<surf_point> map_surf_points;
    deserialize(surf_path) >> map_surf_points;
    cout <<"done" <<endl;

    const int size = 326;

    dlib::rand r;
    ostringstream seed;
    seed << (unsigned int)time(0);
    r.set_seed(seed.str());

    int max_iter = get_option(parser, "n", 10);
    for (int iter = 0; iter < max_iter; iter++ ) {
      int
        left = (int)(r.get_random_double() * (map_image.nc() - size)),
        top  = (int)(r.get_random_double() * (map_image.nr() - size));
      int right = left + size, bottom = top + size;

      auto subimg = sub_image(map_image, rectangle(left, top, right, bottom));
      cout <<"Finding SURF points in subimage...";
      std::vector<surf_point> sub_surf_points = get_surf_points(subimg);
      cout <<"done" <<endl;

      cout <<"Comparing descriptors...";
      std::vector<std::pair<size_t, size_t> > matched_points = match_surf_points(sub_surf_points, map_surf_points);
      cout <<"done" <<endl;
      std::vector<point> sub_points, map_points;
      for (auto it = begin(matched_points); it != end(matched_points); ++it) {
        sub_points.push_back(sub_surf_points[it->first].p.center);
        map_points.push_back(map_surf_points[it->second].p.center);
      }

      if (sub_points.size() < 3) {
        cout <<"Not enough matched points" <<endl;
        continue;
      }

      auto transform = find_affine_transform(sub_points, map_points);
      point sub_center(size/2, size/2);
      point true_center((left + right) / 2, (top + bottom) / 2);
      
      double dist = length_squared(true_center - transform(sub_center));
      cout <<"Distance to true center: " <<dist <<endl;

      array2d<bgr_pixel> draw_img;
      assign_image(draw_img, map_image);
      draw_rectangle(draw_img, rectangle(left, top, right, bottom), bgr_pixel(255, 255, 255), 4);
      draw_solid_circle(draw_img, transform(sub_center), 8.0, bgr_pixel(255, 255, 255));
      draw_solid_circle(draw_img, true_center, 4.0, bgr_pixel(0, 255, 0));
      // image_window iw(draw_img);
      // iw.wait_until_closed();

      ostringstream sfn;
      sfn <<"draw " <<iter <<" distance " <<dist <<".png";
      save_png(draw_img, sfn.str());

    }
  } catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
