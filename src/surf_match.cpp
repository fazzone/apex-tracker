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

std::vector<pair<size_t, size_t>> match_surf_points(const std::vector<surf_point> &first_surf_points, const std::vector<surf_point> &second_surf_points)
{
  std::vector<size_t> labels(second_surf_points.size());
  std::vector<double> d_squared(second_surf_points.size());
  std::vector<pair<size_t, size_t>> matched_points;
  for (int ifirst = 0; ifirst < first_surf_points.size(); ifirst++) {
    for (int isecond = 0; isecond < second_surf_points.size(); isecond++) {
      d_squared[isecond] = vec_distance(first_surf_points[ifirst].des, second_surf_points[isecond].des);
      labels[isecond] = isecond;
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
      matched_points.emplace_back(ifirst, *first);
  }

  return matched_points;
}

