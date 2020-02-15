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
    const char *bwd = getenv("BUILD_WORKING_DIRECTORY");
    if (bwd)
      fs::current_path(bwd);
    
    std::vector<array2d<bgr_pixel>> images;
    for (int i = 0; i < argc; i++) {
      cout <<i <<argv[i] <<endl;
    }
    for (int i = 1; i < argc; i++) {
      array2d<bgr_pixel> img;
      load_png(img, argv[i]);
      images.push_back(img);
    }

    std::vector<std::vector<surf_point> > image_surf_points(images.size());
    for (int i = 0; i < images.size(); i++) {
      cout <<"Extracting SURF points from " <<argv[1+i]  <<"...";
      image_surf_points[i] = get_surf_points(images[i]);
      cout <<"done" <<endl;
    }


  } catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
