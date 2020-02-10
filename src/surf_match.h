#include <dlib/matrix.h>
#include <dlib/image_keypoint.h>
#include <algorithm>
#include <vector>

std::vector<dlib::surf_point> match_surf_points(const std::vector<dlib::surf_point> &sub_surf_points, const std::vector<dlib::surf_point> &map_surf_points);
