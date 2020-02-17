#include "map_matcher.h"

#include <dlib/matrix.h>
#include <dlib/geometry.h>

#include "surf_match.h"
#include <stdio.h>

using namespace dlib;
using namespace std;

map_matcher::map_matcher(database &dbref, const std::vector<surf_point> &surf_points, int mini_w, int mini_h)
  : db(dbref),
    insert_match_stmt(db, "insert into fix_result(inserted_at, x, y) values (datetime('now'), ?, ?);"),
    m_surf_points(surf_points),
    compare_points_buffer(surf_points.size()),
    m_minimap_center(mini_w/2, mini_h/2)
{
}

map_match_result map_matcher::find_match(const std::vector<surf_point> &sub_surf_points) {

  compare_points_buffer.clear();
  
  printf("Computing distance\n");
  for (size_t i = 0; i < m_surf_points.size(); i++) {
    double d = length_squared(m_last_fix_position - m_surf_points[i].p.center);
    if (d < m_search_radius * m_search_radius) {
      compare_points_buffer.push_back(m_surf_points[i]);
    }
  }

  printf("Finding match\n");
  if (compare_points_buffer.size() < 5) {
    m_search_radius = 99999;
    return find_match(sub_surf_points, m_surf_points);
  } else {
    return find_match(sub_surf_points, compare_points_buffer);
  }
}

map_match_result map_matcher::find_match(const std::vector<surf_point> &sub_surf_points,
                                         const std::vector<surf_point> &compare_points) {
        
  printf("Doing the actual match\n");
  std::vector<std::pair<size_t, size_t> > matched_points = match_surf_points(sub_surf_points, compare_points);
  printf("done \n");

    
  std::vector<point> sub_points, map_points;
  for (auto it = begin(matched_points); it != end(matched_points); ++it) {
    sub_points.push_back(sub_surf_points[it->first].p.center);
    map_points.push_back(compare_points[it->second].p.center);
  }

  if (sub_points.size() < 3) {
    cout <<"Not enough matched points" <<endl;

    m_search_radius *= 1.5;
    return map_match_result();
  }

  auto transform = find_affine_transform(sub_points, map_points);
  point new_position = transform(m_minimap_center);
  double dist_from_last = sqrt(length_squared(new_position - m_last_fix_position));

  m_search_radius = 224;

  cout <<"==== Matched " <<matched_points.size() <<" points" <<endl;
  cout <<"===== Fix position " <<new_position.x() <<", " <<new_position.y() <<endl;
  printf("===== distance from last %0.05f\n", dist_from_last);

  m_last_fix_position = new_position;
  insert_match_stmt.bind(1, new_position.x());
  insert_match_stmt.bind(2, new_position.y());
  insert_match_stmt.exec();

  return map_match_result(new_position(0), new_position(1));
}
