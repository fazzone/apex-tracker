#include "surf_match.h"

using namespace std;
using namespace dlib;

static double vec_distance(const matrix<double,64,1> &a, const matrix<double,64,1> &b) {
  auto d2 = squared(b - a);
  double d = 0;
  for (int i = 0; i < a.nr(); i++)
    d += d2(i, 0);
  return d;
}

std::vector<surf_point> match_surf_points(const std::vector<surf_point> &sub_surf_points, const std::vector<surf_point> &map_surf_points)
{
  std::vector<size_t> labels(map_surf_points.size());
  std::vector<double> d_squared(map_surf_points.size());
  std::vector<surf_point> matched_points;
  for (auto it = begin(sub_surf_points); it != end(sub_surf_points); ++it) {
    for (int i = 0; i < map_surf_points.size(); i++) {
      d_squared[i] = vec_distance(it->des, map_surf_points[i].des);
      labels[i] = i;
    }

    auto comp = [&d_squared](size_t i, size_t j) {
                  return d_squared[i] < d_squared[j];
                };

    const int k = 5;
    auto first = labels.begin(), last = labels.begin() + k, end = labels.end();
    std::make_heap(first, last + 1, comp);
    std::pop_heap(first, last + 1, comp);
    for (auto jt = last + 1; jt != end; ++jt) {
      if (d_squared[*jt] >= d_squared[*first]) continue;
      *last = *jt;
      std::pop_heap(first, last + 1, comp);
    }
    std::sort_heap(first, last, comp);
    // Filter for strong matches, where the best match is much better than the next best match
    auto nextbest = first + 1;
    if (d_squared[*nextbest] / d_squared[*first] > 5)
      matched_points.push_back(map_surf_points[*first]);
  }

  return matched_points;
}

