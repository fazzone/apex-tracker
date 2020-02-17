#include "surf_match.h"
#include <cstdio>

using namespace std;
using namespace dlib;

static double vec_distance(const matrix<double,64,1> &a, const matrix<double,64,1> &b) {
  return sum(squared(b - a));
}

std::vector<pair<size_t, size_t>> match_surf_points(const std::vector<surf_point> &first_surf_points, const std::vector<surf_point> &second_surf_points)
{
  std::vector<size_t> labels(second_surf_points.size());
  std::vector<double> d_squared(second_surf_points.size());
  std::vector<pair<size_t, size_t>> matched_points;

  for (size_t ifirst = 0; ifirst < first_surf_points.size(); ifirst++) {
    double best_d = 1e9, second_best_d = 1e9;
    size_t ibest = -1, inextbest = -1;
    for (size_t isecond = 0; isecond < second_surf_points.size(); isecond++) {

      double d = vec_distance(first_surf_points[ifirst].des, second_surf_points[isecond].des);

      if (d < best_d) {
        second_best_d = best_d;
        inextbest = ibest;

        best_d = d;
        ibest = isecond;
      } else if (d < second_best_d) {
        second_best_d = d;
        inextbest = isecond;
      }
    }
    if (ibest < 0 || inextbest < 0)
      continue;

    // if (best_d > 0.05)
    //   continue;

    if (second_best_d / best_d > 2) {
      //printf("best_d, = %0.5f, second_best_d = %0.5f, _d_theta = %0.5f\n", best_d, second_best_d, second_surf_points[ibest].angle -  first_surf_points[ifirst].angle);
      matched_points.emplace_back(ifirst, ibest);
    }
  }

  return matched_points;
}

