#include <dlib/image_io.h>
#include <dlib/image_transforms.h>
#include <dlib/sqlite.h>
#include <dlib/data_io.h>
#include <dlib/image_processing.h>
#include <dlib/cmd_line_parser.h>
#include <dlib/image_keypoint.h>
#include <dlib/sqlite.h>

#include <iostream>

using namespace std;
using namespace dlib;

int main(int argc, char ** argv) {
  try {
    command_line_parser parser;
    parser.add_option("i", "input png", 1);
    parser.add_option("o", "output SURF points data", 1);
    parser.add_option("n", "max number of points to generate", 1);
    parser.add_option("t", "minimum p score threshold", 1);
    
    parser.parse(argc, argv);

    array2d<bgr_pixel> img;
    string png_path = get_option(parser, "i", "res/worlds_edge.png");
    cout <<"Loading " <<png_path <<"...";
    load_png(img, png_path);
    cout <<"done" <<endl;

    database db("surf.db");
    db.exec("create table interest_point("
            "  id integer primary key,"
            "  x real, y real,"
            "  scale real,"
            "  score real,"
            "  laplacian real);");

    db.exec("create table surf_point("
            " id integer primary key,"
            " interest_point_id integer,"
            " angle real,"
            " descriptor blob,"
            " foreign key(interest_point_id) references interest_point(id)"
            ");");

    cout <<"Extracting SURF points" <<"...";
    long max_points = get_option(parser, "n", 10000);
    double dthresh = get_option(parser, "t", 30.0);

    std::vector<surf_point> surf_points = get_surf_points(img, max_points, dthresh);
    cout <<"done" <<endl;
    string surf_path = get_option(parser, "o", "surf.dat");
    serialize(surf_path) <<surf_points;
    cout <<"Wrote " << surf_points.size() <<" SURF points to " <<surf_path <<endl;

    statement insert_point(db, "insert into interest_point(x, y, scale, score, laplacian) values(?, ?, ?, ?, ?);");
    statement insert_surf(db, "insert into surf_point(interest_point_id, angle, descriptor) values (?, ?, ?);");

    cout <<"Creating database...";
    transaction my_trans(db);
    for (auto it = begin(surf_points); it != end(surf_points); ++it) {
      insert_point.bind_double(1, it->p.center.x());
      insert_point.bind_double(2, it->p.center.y());
      insert_point.bind_double(3, it->p.scale);
      insert_point.bind_double(4, it->p.score);
      insert_point.bind_double(5, it->p.laplacian);
      insert_point.exec();

      insert_surf.bind(1, db.last_insert_rowid());
      insert_surf.bind(2, it->angle);
      insert_surf.bind_object(3, it->des);
      insert_surf.exec();
    }
    my_trans.commit();
    cout <<"done" <<endl;

    cout <<"Creating indexes...";
    db.exec("create index interest_point_x_index on interest_point(x);");
    db.exec("create index interest_point_y_index on interest_point(y);");
    cout <<"done" <<endl;

  } catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
