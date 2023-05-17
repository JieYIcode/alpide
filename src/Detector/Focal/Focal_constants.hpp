/**
 * @file   Focal_constants.hpp
 * @author Simon Voigt Nesbo
 * @date   November 24, 2019
 * @brief  Various constants for the Focal detector
 *
 */

#ifndef FOCAL_CONSTANTS_HPP
#define FOCAL_CONSTANTS_HPP

#include "Alpide/alpide_constants.hpp"
// 
namespace Focal_Inner3 {
  static const unsigned int CHIPS = 3;
  static const unsigned int CTRL_LINKS = 1;
  static const unsigned int DATA_LINKS = 3;
}

namespace Focal_Outer {
  static const unsigned int CTRL_LINKS = 1;
  static const unsigned int DATA_LINKS = 1;
}

namespace Focal_Outer3 {
  static const unsigned int CHIPS = 3;
  static const unsigned int CTRL_LINKS = 1;
  static const unsigned int DATA_LINKS = 1;
}

namespace Focal_Outer6 {
  static const unsigned int CHIPS = 6;
  static const unsigned int CTRL_LINKS = 1;
  static const unsigned int DATA_LINKS = 1;
}


namespace Focal_I3_I3_O3_O6{
  static const unsigned int INNERGROUPS = 2;
  static const unsigned int OUTERGROUPS_O3 = 1;
  static const unsigned int OUTERGROUPS_O6 = 1;
  static const unsigned int OUTERGROUPS = OUTERGROUPS_O3 + OUTERGROUPS_O6;
  static const unsigned int DATA_LINKS_INNER = INNERGROUPS*Focal_Inner3::DATA_LINKS ;
  static const unsigned int CTRL_LINKS_INNER = INNERGROUPS*Focal_Inner3::CTRL_LINKS ;
  static const unsigned int DATA_LINKS_OUTER = OUTERGROUPS*Focal_Outer::DATA_LINKS ;
  static const unsigned int CTRL_LINKS_OUTER = OUTERGROUPS*Focal_Outer::CTRL_LINKS ;
  static const unsigned int DATA_LINKS = DATA_LINKS_INNER + DATA_LINKS_OUTER;
  static const unsigned int CTRL_LINKS = CTRL_LINKS_INNER + CTRL_LINKS_OUTER ;
}

namespace Focal_O3_O3_O3_O3_O3{
  static const unsigned int GROUPS = 5;
  static const unsigned int DATA_LINKS = GROUPS*Focal_Outer3::DATA_LINKS ;
  static const unsigned int CTRL_LINKS = GROUPS*Focal_Outer3::CTRL_LINKS ;
}

namespace Focal_O3_O6_O6{
  static const unsigned int O3_GROUPS = 1;
  static const unsigned int O6_GROUPS = 2;
  static const unsigned int DATA_LINKS = O3_GROUPS*Focal_Outer3::DATA_LINKS + O6_GROUPS*Focal_Outer6::DATA_LINKS;
  static const unsigned int CTRL_LINKS = O3_GROUPS*Focal_Outer3::CTRL_LINKS + O6_GROUPS*Focal_Outer6::CTRL_LINKS;
}

namespace Focal_O3_O3_O3_O6{
  static const unsigned int O3_GROUPS = 3;
  static const unsigned int O6_GROUPS = 1;
  static const unsigned int DATA_LINKS = O3_GROUPS*Focal_Outer3::DATA_LINKS + O6_GROUPS*Focal_Outer6::DATA_LINKS;
  static const unsigned int CTRL_LINKS = O3_GROUPS*Focal_Outer3::CTRL_LINKS + O6_GROUPS*Focal_Outer6::CTRL_LINKS;
}


namespace Focal {
  static const unsigned int N_LAYERS = 2;

  

  // Inner Stave structure: <Module 0><Module 1>
  // Module 0: 8x IB chips
  // Module 1: 7x OB chips

  // Outer Stave structure: <Module 0><Module 1><Module 2>
  // Module 0: 5x OB chips
  // Module 1: 5x OB chips
  // Module 2: 5x OB chips

  //static const unsigned int CHIPS_PER_FOCAL_MODULE = 3;
  static const unsigned int INNER_CHIPS_PER_FOCAL_INNER_MODULE = 3;
  static const unsigned int OUTER_CHIPS_PER_FOCAL_INNER_MODULE = 6;
  static const unsigned int OUTER_CHIPS_PER_FOCAL_OUTER_MODULE = 3;
  //static const unsigned int CHIPS_PER_FOCAL_IB_MODULE = 8;
  //static const unsigned int CHIPS_PER_FOCAL_OB_MODULE = 5;
  static const unsigned int CHIPS_PER_STRING = 15;
  static const unsigned int STAVES_PER_PATCH = 6;
  static const unsigned int STAVES_PER_HALF_PATCH = 3;
  static const unsigned int HALF_PATCHES_PER_QUADRANT = 11;
  static const unsigned int HALF_PATCHES_PER_LAYER = 4*HALF_PATCHES_PER_QUADRANT;

  static const unsigned int STAVES_PER_QUADRANT = HALF_PATCHES_PER_QUADRANT*STAVES_PER_HALF_PATCH;
  static const unsigned int STRINGS_PER_LAYER = STAVES_PER_HALF_PATCH*HALF_PATCHES_PER_LAYER;
  static const unsigned int CHIPS_PER_QUADRANT = STAVES_PER_QUADRANT*CHIPS_PER_STRING;
  static const unsigned int CHIPS_PER_LAYER = STRINGS_PER_LAYER*CHIPS_PER_STRING;

  static const unsigned int CUMULATIVE_CHIP_COUNT_AT_LAYER[2] = {0,
                                                                 CHIPS_PER_LAYER};

  // Half-patches of FocalInnerStave (IB+OB modules)
  static const unsigned int INNER_HALF_PATCHES_PER_QUADRANT = 5;
  static const unsigned int INNER_STAVES_PER_QUADRANT = STAVES_PER_HALF_PATCH*INNER_HALF_PATCHES_PER_QUADRANT;

  static const unsigned int MODULES_PER_STAVE = 5;
  static const unsigned int INNER_MODULES_PER_INNER_STAVE = 1;
  static const unsigned int OUTER_MODULES_PER_INNER_STAVE = 2;
  static const unsigned int OUTER_MODULES_PER_OUTER_STAVE = 5;

  static const unsigned int CHIPS_PER_FOCAL_INNER_STAVE = INNER_CHIPS_PER_FOCAL_INNER_MODULE*INNER_MODULES_PER_INNER_STAVE + OUTER_CHIPS_PER_FOCAL_INNER_MODULE*OUTER_MODULES_PER_INNER_STAVE;
  static const unsigned int CHIPS_PER_FOCAL_OUTER_STAVE = OUTER_CHIPS_PER_FOCAL_OUTER_MODULE*OUTER_MODULES_PER_OUTER_STAVE ;

  static const unsigned int CHIPS_PER_FOCAL_STAVE = 15;

  static const unsigned int MODULES_PER_INNER_STAVE = INNER_MODULES_PER_INNER_STAVE+OUTER_MODULES_PER_INNER_STAVE;
  static const unsigned int MODULES_PER_OUTER_STAVE = OUTER_MODULES_PER_OUTER_STAVE;

  static const unsigned int CHIPS = CHIPS_PER_LAYER * N_LAYERS;

  //static const unsigned int MODULES_PER_INNER_STAVE = 2; // 8 IB chips + 7 OB chips
  //static const unsigned int MODULES_PER_OUTER_STAVE = 3; // 5 + 5 + 5 OB chips

  //static const unsigned int CTRL_LINKS_PER_INNER_STAVE = 1;
  //static const unsigned int CTRL_LINKS_PER_INNER_STAVE = 2;
  //static const unsigned int DATA_LINKS_PER_INNER_STAVE = CHIPS_PER_FOCAL_IB_MODULE+1;

  static const unsigned int CTRL_LINKS_PER_INNER_STAVE = INNER_MODULES_PER_INNER_STAVE+OUTER_MODULES_PER_INNER_STAVE;
  static const unsigned int DATA_LINKS_PER_INNER_STAVE = INNER_MODULES_PER_INNER_STAVE*INNER_CHIPS_PER_FOCAL_INNER_MODULE + OUTER_MODULES_PER_INNER_STAVE;

  static const unsigned int CTRL_LINKS_PER_OUTER_STAVE = OUTER_MODULES_PER_OUTER_STAVE;
  static const unsigned int DATA_LINKS_PER_OUTER_STAVE = OUTER_MODULES_PER_OUTER_STAVE;

  //static const unsigned int CTRL_LINKS_PER_OUTER_STAVE = 1;
  //static const unsigned int CTRL_LINKS_PER_OUTER_STAVE = 3;
  //static const unsigned int DATA_LINKS_PER_OUTER_STAVE = 3;

  static const double STAVE_SIZE_X_MM = (CHIP_WIDTH_CM*10) * CHIPS_PER_STRING;
  static const double STAVE_SIZE_Y_MM = (CHIP_HEIGHT_CM*10);

  static const double PATCH_SIZE_X_MM = STAVE_SIZE_X_MM;
  static const double PATCH_SIZE_Y_MM = STAVE_SIZE_Y_MM*STAVES_PER_PATCH;

  static const double HALF_PATCH_SIZE_X_MM = PATCH_SIZE_X_MM;
  static const double HALF_PATCH_SIZE_Y_MM = PATCH_SIZE_Y_MM/2.;


// some dimensions for adjusting for the beam pipe gap
  static const double MACRO_CELL_SIZE_X_MM = 0.5;
  static const double MACRO_CELL_SIZE_Y_MM = 0.5;

  static const double GAP_SIZE_X_MM = 100;
  static const double GAP_SIZE_Y_MM = PATCH_SIZE_Y_MM;

  static const double GAP_SIZE_X_CELLS = GAP_SIZE_X_MM/MACRO_CELL_SIZE_X_MM;
  static const double GAP_SIZE_Y_CELLS = GAP_SIZE_Y_MM/MACRO_CELL_SIZE_Y_MM;

  static const double SHIFT_X_MM = 12;
  static const double SHIFT_Y_MM = 9;
  
  static const double SHIFT_X_CELLS = SHIFT_X_MM/MACRO_CELL_SIZE_X_MM;
  static const double SHIFT_Y_CELLS = SHIFT_Y_MM/MACRO_CELL_SIZE_Y_MM;
}

#endif
