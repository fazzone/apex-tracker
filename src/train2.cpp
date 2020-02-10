
#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/threads.h>
#include <dlib/data_io.h>
#include <dlib/rand.h>
#include <dlib/pipe.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace std;
using namespace dlib;
namespace fs = std::filesystem;

dlib::mutex my_mutex;

typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type; 

struct train_request {
  int size;
  array2d<bgr_pixel> *image;
  std::vector<rectangle> rects;
  std::vector<object_detector<image_scanner_type> > *detectors;
};

template<class T>
void lock_and_push(const dlib::mutex &mtx, const T& t, std::vector<T> &ts) {
  auto_mutex locker(mtx);
  cout <<"Pushing!" <<endl; 
  ts.push_back(t);
}

void do_train(train_request* req) {
  
  dlib::array<array2d<bgr_pixel> > sections_images;
  sections_images.resize(1);
  sections_images[0].set_size(req->image->nr(), req->image->nc());
  assign_image(sections_images[0], *(req->image));

  image_scanner_type scanner;
  scanner.set_detection_window_size(req->size, req->size);
  structural_object_detection_trainer<image_scanner_type> trainer(scanner);
  trainer.set_num_threads(4);
  trainer.set_c(1);
  trainer.be_verbose();
  trainer.set_epsilon(0.1);

  std::vector<std::vector<rectangle> > train_rects;
  train_rects.push_back(req->rects);

  std::cout <<"begin train" <<endl;
  object_detector<image_scanner_type> detector = trainer.train(sections_images, train_rects);
  
  lock_and_push(my_mutex, detector, *(req->detectors));
}
  
int maxi(int a, int b) {
  return std::max(a, b);
}

int mini(int a, int b) {
  return std::min(a, b);
}


rectangle make_rect(int x, int y, int w, int h) {
  return rectangle(x, y, x+w, y+h);
}

class pipe_example : private multithreaded_object {
public:
  pipe_example() : job_pipe(4) {
    // register 3 threads
    register_thread(*this,&pipe_example::thread);
    register_thread(*this,&pipe_example::thread);
    register_thread(*this,&pipe_example::thread);
      
    // start the 3 threads we registered above
    start();
  }

  ~pipe_example () {
    job_pipe.wait_until_empty();
    job_pipe.disable();
    wait();
  }
  dlib::pipe<train_request> job_pipe;

private:
    void thread () {
      train_request j;
      // Here we loop on jobs from the job_pipe.  
      while (job_pipe.dequeue(j))
        do_train(&j);
    }

};

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
        
        const int n_jitter = 10, max_jitter = 24;
        dlib::rand r;
        
        for (int sr = 10; sr < v_sections - 2; sr++) {
          for (int sc = 10; sc < h_sections - 2; sc++) {
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
                bottom = top + size;
              if (left < 0 || top < 0 || right >= map_image.nc() || bottom >= map_image.nr()) {
                continue;
              }
              
              grp.push_back(rectangle(left, top, right, bottom));
            }
            sections_groups.push_back(grp);
          }
        }

        std::vector<object_detector<image_scanner_type> > detectors;
        
        pipe_example pe;
        for (int group_index = 0; group_index < sections_groups.size() ; group_index++) {
          array2d<bgr_pixel> *ti = new array2d<bgr_pixel>;
          auto group = sections_groups[group_index];

          int
            outer_left = maxi(0, (int)group[0].left() - 100),
            outer_top = maxi(0, (int)group[0].top() - 100),
            outer_right = mini(map_image.nc(), (int)group[0].right() + 100),
            outer_bottom = mini(map_image.nr(), group[0].bottom() + 100);

          assign_image(*ti, sub_image(map_image, rectangle(outer_left, outer_top, outer_right, outer_bottom)));

          std::vector<rectangle> train_rects;
          for (int i = 0; i < group.size(); i++)
            train_rects.push_back(make_rect(group[i].left() - outer_left, group[i].top() - outer_top, size, size));

          // array2d<bgr_pixel> rsi;
          // assign_image(rsi, *ti);
          // cout <<"RSI " << rsi.nc() <<"x" <<rsi.nr() <<endl;
          // for (auto it = begin(train_rects); it != end(train_rects); ++it) {
          //   std::cout <<"drawing rect " <<it->left() << ", " <<it->top() <<"  " <<it->right() <<", " <<it->bottom() <<endl;
          //   draw_rectangle(rsi, *it, bgr_pixel(255, 255, 255));
          // }
          // save_png(rsi, "rsi.png");



          train_request *tr = new train_request;
          tr->size = size;
          tr->image = ti;
          tr->rects = train_rects;
          tr->detectors = &detectors;
          //create_new_thread(do_train, tr);
          do_train(tr);
          //pe.job_pipe.enqueue(*tr);
          
          // auto group = sections_groups[group_index];
          // std::cout <<"Training group  " <<group_index <<" of " <<sections_groups.size() << " - " <<group.size() <<" rects" <<endl;

          // array2d<bgr_pixel> rsi;
          // assign_image(rsi, map_image);
          // for (auto it = begin(group); it != end(group); ++it)
          //   draw_rectangle(rsi, *it, bgr_pixel(255, 255, 255));
          // save_png(rsi, "rsi.png");

          // dlib::array<array2d<bgr_pixel> > sections_images;
          // sections_images.resize(1);
          // sections_images[0].set_size(map_image.nr(), map_image.nc());
          // assign_image(sections_images[0], map_image);

          // image_scanner_type scanner;
          // scanner.set_detection_window_size(size, size); 
          // structural_object_detection_trainer<image_scanner_type> trainer(scanner);
          // trainer.set_num_threads(10);
          // trainer.set_c(1);
          // trainer.be_verbose();
          // trainer.set_epsilon(0.01);

          // std::vector<std::vector<rectangle> > train_groups;
          // train_groups.push_back(group);

          // std::cout <<"begin train" <<endl;
          // object_detector<image_scanner_type> detector = trainer.train(sections_images, train_groups);
          // detectors.push_back(detector);
        }

    }
    catch (exception& e)
    {
        cout << "\nexception thrown!" << endl;
        cout << e.what() << endl;
    }
}

// ----------------------------------------------------------------------------------------


