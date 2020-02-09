
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

        
        int approx_minimap_size = 326;
        int size = approx_minimap_size / 4;
        int h_sections = map_image.nc() / size, v_sections = map_image.nr() / size;
        std::vector<std::vector<rectangle> > sections_groups;
        
        const int n_jitter = 100, max_jitter = 24;
        dlib::rand r;
        
        for (int sr = 0; sr < v_sections; sr++) {
          for (int sc = 0; sc < h_sections; sc++) {
            std::vector<rectangle> grp;
            for (int i = 0; i < n_jitter; i++) {
              int 
                dx = (int)(r.get_double_in_range(-1, 1) * max_jitter),
                dy = (int)(r.get_double_in_range(-1, 1) * max_jitter);

              int
                left = dx + sc*size,
                top = dy + sr*size;
              int 
                right = left + size,
                bottom = right + size;
              if (left < 0 || top < 0 || right >= map_image.nc() || bottom >= map_image.nr()) {
                cout <<"Section " <<left  <<" " <<top <<" " <<right <<" " <<bottom <<"extends outside bounds" <<endl;
                continue;
              }
              
              grp.push_back(rectangle(left, top, right, bottom));
            }
            sections_groups.push_back(grp);
          }
        }


        typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type; 
        image_scanner_type scanner;
        scanner.set_detection_window_size(size, size); 

        std::vector<object_detector<image_scanner_type> > detectors;
        
        for (int group_index = 0; group_index < sections_groups.size() ; group_index++) {
          auto group = sections_groups[group_index];
          std::cout <<"Training group  " <<group_index <<" of " <<sections_groups.size() <<endl;

          std::vector<sub_image_proxy<array2d<bgr_pixel> > > sections_images;
          std::vector<std::vector<rectangle> > sections_rects;
          for (auto it = begin(group); it != end(group); ++it) {
            sections_images.push_back(sub_image(map_image, *it));
            std::vector<rectangle> rs;
            rectangle truth_rect(0, 0, size, size);
            rs.push_back(truth_rect);
            sections_rects.push_back(rs);
          }

          structural_object_detection_trainer<image_scanner_type> trainer(scanner);
          trainer.set_num_threads(10);  
          trainer.set_c(0.1);
          //trainer.be_verbose();
          trainer.set_epsilon(0.01);

          object_detector<image_scanner_type> detector = trainer.train(sections_images, sections_rects);
          detectors.push_back(detector);
        }

    }
    catch (exception& e)
    {
        cout << "\nexception thrown!" << endl;
        cout << e.what() << endl;
    }
}

// ----------------------------------------------------------------------------------------


