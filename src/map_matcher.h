#ifndef _MAP_MATCHER_H
#define _MAP_MATCHER_H

#include <utility>

#include <dlib/matrix.h>
#include <dlib/geometry.h>
#include <dlib/image_keypoint/surf.h>
#include <dlib/sqlite.h>


struct map_match_result {
  int x, y;
  double residual;
  bool good;
map_match_result() : good(false) {}
map_match_result(int xx, int yy, double r) : x(xx), y(yy), good(true), residual(r) {}
};

struct point_match {
  const dlib::surf_point *const left;
  const dlib::surf_point *const right;
  
  point_match(const dlib::surf_point *const l, const dlib::surf_point *const r) : left(l), right(r) {}
};

class map_matcher {
 public:
  map_matcher(dlib::database &the_db, const std::vector<dlib::surf_point> &surf_points, int minimap_w, int minimap_h);

  map_match_result find_match(const std::vector<dlib::surf_point> &sub_surf_points);
  map_match_result find_match(const std::vector<dlib::surf_point> &sub_surf_points,
                              const std::vector<dlib::surf_point> &compare_points);

 private:

  const std::vector<dlib::surf_point> &m_surf_points;
  std::vector<dlib::surf_point> compare_points_buffer;

  dlib::database &db;
  dlib::statement insert_match_stmt;

  dlib::point m_last_fix_position;
  double m_search_radius;
  dlib::point m_minimap_center;

};

#endif
