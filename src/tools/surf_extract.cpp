#include <dlib/image_io.h>
#include <dlib/image_transforms.h>
#include <dlib/data_io.h>
#include <dlib/image_processing.h>
#include <dlib/cmd_line_parser.h>
#include <dlib/image_keypoint.h>

#include <iostream>

using namespace std;
using namespace dlib;

int main(int argc, char ** argv) {
  try {
    command_line_parser parser;
    parser.add_option("i", "input png", 1);
    parser.add_option("o", "output SURF points data", 1);
    parser.add_option("n", "max number of points to generate", 1);
    parser.add_option("t", "minimum p score threshold", 1);
    
    parser.parse(argc, argv);

    array2d<bgr_pixel> img;
    string png_path = get_option(parser, "i", "res/worlds_edge.png");
    cout <<"Loading " <<png_path <<"...";
    load_png(img, png_path);
    cout <<"done" <<endl;

    cout <<"Extracting SURF points" <<"...";
    long max_points = get_option(parser, "n", 10000);
    double dthresh = get_option(parser, "t", 30.0);

    std::vector<surf_point> surf_points = get_surf_points(img, max_points, dthresh);
    cout <<"done" <<endl;
    string surf_path = get_option(parser, "o", "surf.dat");
    serialize(surf_path) <<surf_points;
    cout <<"Wrote " << surf_points.size() <<" SURF points to " <<surf_path <<endl;

  } catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
