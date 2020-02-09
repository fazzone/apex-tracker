#include <dlib/matrix.h>
#include <dlib/image_io.h>
#include <dlib/image_transforms.h>
#include <dlib/gui_widgets.h>
#include <dlib/cmd_line_parser.h>
#include <dlib/image_keypoint/draw_surf_points.h>
#include <dlib/image_keypoint.h>
#include <dlib/clustering.h>
#include <dlib/rand.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace dlib;

namespace fs = std::filesystem;

struct compare_lab_pixel {
  bool operator() (const lab_pixel& a, const lab_pixel& b) const {
    if (a.l < b.l)
      return true;
    if (a.a < b.a)
      return true;
    return (a.b < b.b);
  }
};

int main(int argc, char ** argv) {
  try {
    command_line_parser parser;
    parser.add_option("i", "input full map png", 1);
    parser.add_option("o", "output surf points", 1);
    parser.parse(argc, argv);

    const char *bwd = getenv("BUILD_WORKING_DIRECTORY");
    if (bwd)
      fs::current_path(bwd);

    array2d<bgr_pixel> map_image;
    string png_path = get_option(parser, "i", "map.png");
    cout <<"loading " <<png_path <<endl;
    load_png(map_image, png_path);
    

    array2d<bgr_pixel> &img = map_image;

    std::vector<surf_point> sp = get_surf_points(map_image, 100, 200.0);
    cout << "number of SURF points found: "<< sp.size() << endl;
    string out_path = get_option(parser, "o", "surf.ser");
    ofstream out(out_path, ios::binary);
    cout <<"writing " <<out_path <<endl;
    serialize(sp, out);

    return 0;
  } catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
