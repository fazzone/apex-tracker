
#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <dlib/rand.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace std;
using namespace dlib;
namespace fs = std::filesystem;

int main(int argc, char** argv)
{  

    try
    {
        const char *bwd = getenv("BUILD_WORKING_DIRECTORY");
        if (bwd)
          fs::current_path(bwd);

        array2d<bgr_pixel> map_image;
        load_png(map_image, "res/worlds_edge.png");

        const int n_sections = 1000;
        const double section_scale = 80;
        std::vector<sub_image_proxy<array2d<bgr_pixel> > > sections_images;
        std::vector<std::vector<rectangle> > sections_rects;
        
        dlib::rand r;
        int i = 0;
        while (i < n_sections) {
          double
            da = r.get_random_double(),
            db = r.get_random_double(),
            dc = r.get_random_double(),
            dd = r.get_random_double();
            
          long
            left = (int)(map_image.nc() * std::min(da, db)),
            top = (int)(map_image.nc() * std::max(da, db)),
            width = (int)(section_scale),
            height = (int)(section_scale);
          
          if (width >= map_image.nc()) {
            cout <<"width " <<width <<" too big" <<endl;
            continue;
          }
          if (height >= map_image.nr()) {
            cout <<"height " <<height <<" too big" <<endl;
            continue;
          }
          rectangle subimage_rect(left, top, left + width, top + height);
          sections_images.push_back(sub_image(map_image, subimage_rect));

          ostringstream ss;
          ss <<"subimage_" <<i <<".png";
          save_png(sections_images.back(), ss.str());

          rectangle truth_rect(0, 0, width, height);
          std::vector<rectangle> rs;
          rs.push_back(truth_rect);
          sections_rects.push_back(rs);

          i++;
        }

        typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type; 
        image_scanner_type scanner;

        scanner.set_detection_window_size(section_scale, section_scale); 
        structural_object_detection_trainer<image_scanner_type> trainer(scanner);

        trainer.set_num_threads(10);  
        trainer.set_c(0.1);
        trainer.be_verbose();
        trainer.set_epsilon(0.01);

        cout <<"starting training" <<endl;
        object_detector<image_scanner_type> detector = trainer.train(sections_images, sections_rects);

        cout << "training results: " << test_object_detection_function(detector, sections_images, sections_rects) << endl;

    }
    catch (exception& e)
    {
        cout << "\nexception thrown!" << endl;
        cout << e.what() << endl;
    }
}

// ----------------------------------------------------------------------------------------


