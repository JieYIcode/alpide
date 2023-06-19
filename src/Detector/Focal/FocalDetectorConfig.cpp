/**
 * @file   FocalDetectorConfig.cpp
 * @author Simon Voigt Nesbo
 * @date   November 24, 2019
 * @brief  Classes and functions for detector configuration
 *         and position specific for Focal detector.
 */

#include "FocalDetectorConfig.hpp"
#include <iostream>


unsigned int Focal::Focal_position_to_global_chip_id(const Detector::DetectorPosition& pos)
{
  unsigned int global_chip_id = pos.layer_id*CHIPS_PER_LAYER;

  // Quadrant 0: top right, 1: top left, 2: bottom left, 3: bottom right
  unsigned int quadrant = pos.stave_id/STAVES_PER_QUADRANT;
  unsigned int stave_in_quadrant = pos.stave_id % STAVES_PER_QUADRANT;

  global_chip_id += quadrant*CHIPS_PER_QUADRANT;
  global_chip_id += stave_in_quadrant*CHIPS_PER_STRING;

  if(stave_in_quadrant < INNER_STAVES_PER_QUADRANT) {
    // Focal Inner Stave
    if(pos.module_id < Focal_I3_I3_O3_O6::INNERGROUPS) {
      // Second module with 7 outer barrel chips in this stave
      global_chip_id += Focal_Inner3::CHIPS*pos.module_id;
    } else {
      global_chip_id += Focal_I3_I3_O3_O6::INNERGROUPS*Focal_Inner3::CHIPS;
      if(pos.module_id >= Focal_I3_I3_O3_O6::INNERGROUPS+Focal_I3_I3_O3_O6::OUTERGROUPS_O3 ){
        global_chip_id += Focal_Outer3::CHIPS;
      }
    }
  } else {
    // Focal Outer Stave
    global_chip_id += pos.module_id * Focal_Outer3::CHIPS;
  }
  

  //change here since we are dealing with modules of 3 chips in 2022
  //global_chip_id += pos.module_id*CHIPS_PER_FOCAL_MODULE;

  global_chip_id += pos.module_chip_id;

  return global_chip_id;
}


Detector::DetectorPosition Focal::Focal_global_chip_id_to_position(unsigned int global_chip_id) {
  unsigned int layer_id = 0;
  unsigned int stave_id = 0;
  unsigned int sub_stave_id = 0;
  unsigned int module_id = 0;
  unsigned int module_chip_id = 0;

  //unsigned int _gcid = global_chip_id;

  layer_id = global_chip_id/CHIPS_PER_LAYER;
  global_chip_id -= layer_id*CHIPS_PER_LAYER;

  stave_id = global_chip_id/CHIPS_PER_STRING;
  global_chip_id -= stave_id*CHIPS_PER_STRING;

  unsigned int stave_in_quadrant = stave_id % STAVES_PER_QUADRANT;

  if(stave_in_quadrant < INNER_STAVES_PER_QUADRANT) {
    // Focal Inner Stave
    if(global_chip_id >= Focal_I3_I3_O3_O6::INNERGROUPS*Focal_Inner3::CHIPS) {
      //outer modules
      global_chip_id -= Focal_I3_I3_O3_O6::INNERGROUPS*Focal_Inner3::CHIPS;
      if(global_chip_id>=Focal_Outer3::CHIPS){
        module_id = 3;
        global_chip_id -= Focal_Outer3::CHIPS;
      } else {
        module_id = 2;
      }

    } else {
      //inner modules
      module_id = global_chip_id / Focal_Inner3::CHIPS;
      global_chip_id -= module_id*Focal_Inner3::CHIPS;
    }
  } else {
    // Focal Outer Stave
    module_id = global_chip_id / Focal_Outer3::CHIPS;
    global_chip_id -= module_id * Focal_Outer3::CHIPS;
  }

  module_chip_id = global_chip_id;

  //std::cout <<_gcid<<"\t"<<layer_id<<"\t"<<stave_id<<"\t"<<module_id<<"\t"<<module_chip_id<<std::endl;

  Detector::DetectorPosition position = {layer_id,
                                         stave_id,
                                         sub_stave_id, // Not used in Focal
                                         module_id,
                                         module_chip_id};
  
  
  return position;
}
