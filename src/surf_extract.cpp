#include <dlib/image_io.h>
#include <dlib/image_transforms.h>
#include <dlib/data_io.h>
#include <dlib/image_processing.h>
#include <dlib/cmd_line_parser.h>
#include <dlib/image_keypoint.h>

#include <iostream>
#include <filesystem>

using namespace std;
using namespace dlib;
namespace fs = std::filesystem;

int main(int argc, char ** argv) {
  try {
    command_line_parser parser;
    parser.add_option("i", "input png", 1);
    parser.add_option("o", "output SURF points data", 1);
    
    parser.parse(argc, argv);

    const char *bwd = getenv("BUILD_WORKING_DIRECTORY");
    if (bwd)
      fs::current_path(bwd);
    
    array2d<bgr_pixel> img;
    string png_path = get_option(parser, "i", "res/worlds_edge.png");
    cout <<"Loading " <<png_path <<"...";
    load_png(img, png_path);
    cout <<"done" <<endl;

    
    cout <<"Extracting SURF points" <<"...";
    std::vector<surf_point> surf_points = get_surf_points(img);
    cout <<"done" <<endl;
    string surf_path = get_option(parser, "o", "surf.dat");
    serialize(surf_path) <<surf_points;
    cout <<"Wrote " << surf_points.size() <<" SURF points to " <<surf_path <<endl;

  } catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
