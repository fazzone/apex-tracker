#ifndef _APEX_MESSAGES_H
#define _APEX_MESSAGES_H

#include "windows.h"

/* namespace message_type { */
/*   const int add_map_point = WM_APP + 1; */
/* } */

namespace message {

  struct AddMapPoint {
    static const int message_type = WM_APP + 1;
    int x, y;
  };

  struct Repaint {
    static const int message_type = WM_APP + 2;
  };

};


#endif
