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

    typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type;
    object_detector<image_scanner_type> detector;
    deserialize("map_detector.svm") >> detector;

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
        std::vector<rectangle> detections = detector(subimage);
        array2d<bgr_pixel> draw;
        assign_image(draw, subimage);
        for (auto it = begin(detections); it != end(detections); ++it)
          draw_rectangle(draw, *it, bgr_pixel(255, 255, 255));
        save_png(subimage, "subimage.png");
        image_window iw(draw);
        iw.wait_until_closed();
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
