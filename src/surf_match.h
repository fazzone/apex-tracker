#include <dlib/image_keypoint.h>
#include <vector>
#include <utility>

std::vector<std::pair<size_t, size_t>> match_surf_points(const std::vector<dlib::surf_point> &first_points, const std::vector<dlib::surf_point> &second_points);
