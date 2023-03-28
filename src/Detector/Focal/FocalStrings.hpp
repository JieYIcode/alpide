/**
 * @file   FocalStaves.hpp
 * @author Simon Voigt Nesbo
 * @date   December 10, 2019
 * @brief  Classes for Focal staves
 */

#ifndef FOCAL_STRINGS_HPP
#define FOCAL_STRINGS_HPP



#include <Alpide/Alpide.hpp>
#include <Alpide/AlpideInterface.hpp>
#include "Detector/Common/DetectorConfig.hpp"
#include "Detector/Common/ITSModulesStaves.hpp"
#include "Detector/Focal/Focal_constants.hpp"
#include "Detector/Focal/FocalDetectorConfig.hpp"
#include "Detector/Focal/FocalStringGroups.hpp"

namespace FocalString {



#ifdef LEGACY_SVN
  ///@brief A module for Focal consisting of 8 chips in IB configuration
  struct FocalIbModule : public sc_module
  {
    FocalIbModule(sc_core::sc_module_name const &name,
                  Detector::DetectorPosition pos,
                  Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                  const AlpideConfig& cfg);

    void addTraces(sc_trace_file *wf, std::string name_prefix) const;

    sc_in_clk s_system_clk_in;
    ControlTargetSocket socket_control_in;                                 // IB module control in
    DataInitiatorSocket socket_data_out[Focal::CHIPS_PER_FOCAL_IB_MODULE]; // IB module data out

    std::vector<std::shared_ptr<Alpide>> getChips(void) const {
      return mChips;
    }

    ControlResponsePayload processCommand(ControlRequestPayload const &request);

  private:
    std::vector<std::shared_ptr<Alpide>> mChips;

    // Distribution of ctrl to individual chips in half-module
    std::array<ControlInitiatorSocket, Focal::CHIPS_PER_FOCAL_IB_MODULE> socket_control_out;
  };
  

  ///@brief A module for Focal consisting of FOCAL_CHIPS_PER_MODULE chips in IB configuration
  struct FocalInnerModule : public sc_module
  {
    FocalInnerModule(sc_core::sc_module_name const &name,
                  Detector::DetectorPosition pos,
                  Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                  const AlpideConfig& cfg);

    void addTraces(sc_trace_file *wf, std::string name_prefix) const;

    sc_in_clk s_system_clk_in;
    ControlTargetSocket socket_control_in;                                 // IB module control in
    DataInitiatorSocket socket_data_out[Focal::INNER_CHIPS_PER_FOCAL_INNER_MODULE]; // IB module data out

    std::vector<std::shared_ptr<Alpide>> getChips(void) const {
      return mChips;
    }

    ControlResponsePayload processCommand(ControlRequestPayload const &request);

  private:
    std::vector<std::shared_ptr<Alpide>> mChips;

    // Distribution of ctrl to individual chips in half-module
    std::array<ControlInitiatorSocket, Focal::INNER_CHIPS_PER_FOCAL_INNER_MODULE> socket_control_out;
  };



  ///@brief A module for Focal consisting of 5 chips in OB configuration
  struct FocalObModule : public sc_module
  {
    FocalObModule(sc_core::sc_module_name const &name,
                  Detector::DetectorPosition pos,
                  Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                  const AlpideConfig& cfg);

    void addTraces(sc_trace_file *wf, std::string name_prefix) const;

    sc_in_clk s_system_clk_in;
    ControlTargetSocket socket_control_in; // Half-module control in
    DataInitiatorSocket socket_data_out;   // Half-module data out

    std::vector<std::shared_ptr<Alpide>> getChips(void) const {
      return mChips;
    }

    ControlResponsePayload processCommand(ControlRequestPayload const &request);

  private:
    std::vector<std::shared_ptr<Alpide>> mChips;

    // Distribution of ctrl in half-module
    std::array<ControlInitiatorSocket, Focal::CHIPS_PER_FOCAL_OB_MODULE> socket_control_out;
  };

  ///@brief Focal stave consisting of 15 chips in total:
  ///       3x Focal OB modules of five 5 OB chips each,
  ///       that is: 5 + 5 + 5 OB chips
  struct FocalOuterStave : public ITS::StaveInterface
  {
    FocalOuterStave(sc_core::sc_module_name const &name,
                    Detector::DetectorPosition pos,
                    Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                    const Detector::DetectorConfigBase& cfg);
    void addTraces(sc_trace_file *wf, std::string name_prefix) const;
    std::vector<std::shared_ptr<Alpide>> getChips(void) const;

  private:
    std::shared_ptr<Focal::FocalOuterModule> mOuterModules[OUTER_MODULES_PER_OUTER_STAVE];
  };
  
#endif   


 
  ///@brief Focal string consisting of 15 chips in total:
  ///       2x "Focal IB string group" (2 x 3 IB chips) + 2x "Focal OB string group" (3 + 6 OB chips)
  struct FocalString_I3_I3_O3_O6 : public ITS::StaveInterface
  {
    FocalString_I3_I3_O3_O6(sc_core::sc_module_name const &name,
                    Detector::DetectorPosition pos,
                    Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                    const Detector::DetectorConfigBase& cfg);
    void addTraces(sc_trace_file *wf, std::string name_prefix) const;
    std::vector<std::shared_ptr<Alpide>> getChips(void) const;

  private:
    std::shared_ptr<FocalStringGroup::FocalStringGroup_I3> mInnerModule[Focal_I3_I3_O3_O6::INNERGROUPS];
    std::shared_ptr<FocalStringGroup::FocalStringGroup_O3> mOuterModule0[Focal_I3_I3_O3_O6::OUTERGROUPS_O3];
    std::shared_ptr<FocalStringGroup::FocalStringGroup_O6> mOuterModule1[Focal_I3_I3_O3_O6::OUTERGROUPS_O6];

  };

  ///@brief Focal string consisting of 15 chips in total:
  ///       5x "Focal IB string group" (5 x 3 OB chips) 
  struct FocalString_O3_O3_O3_O3_O3 : public ITS::StaveInterface
  {
    FocalString_O3_O3_O3_O3_O3(sc_core::sc_module_name const &name,
                    Detector::DetectorPosition pos,
                    Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                    const Detector::DetectorConfigBase& cfg);
    void addTraces(sc_trace_file *wf, std::string name_prefix) const;
    std::vector<std::shared_ptr<Alpide>> getChips(void) const;

  private:
    std::shared_ptr<FocalStringGroup::FocalStringGroup_O3> mOuterModule[Focal_O3_O3_O3_O3_O3::GROUPS];
  };

  ///@brief Focal string consisting of 15 chips in total:
  ///       5x "Focal IB string group" (5 x 3 OB chips) 
  struct FocalString_O3_O6_O6 : public ITS::StaveInterface
  {
    FocalString_O3_O6_O6(sc_core::sc_module_name const &name,
                    Detector::DetectorPosition pos,
                    Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                    const Detector::DetectorConfigBase& cfg);
    void addTraces(sc_trace_file *wf, std::string name_prefix) const;
    std::vector<std::shared_ptr<Alpide>> getChips(void) const;

  private:
    std::shared_ptr<FocalStringGroup::FocalStringGroup_O3> mOuterModule_O3[Focal_O3_O6_O6::O3_GROUPS];
    std::shared_ptr<FocalStringGroup::FocalStringGroup_O6> mOuterModule_O6[Focal_O3_O6_O6::O6_GROUPS];
  };

  ///@brief Focal string consisting of 15 chips in total:
  ///       5x "Focal IB string group" (5 x 3 OB chips) 
  struct FocalString_O3_O3_O3_O6 : public ITS::StaveInterface
  {
    FocalString_O3_O3_O3_O6(sc_core::sc_module_name const &name,
                    Detector::DetectorPosition pos,
                    Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                    const Detector::DetectorConfigBase& cfg);
    void addTraces(sc_trace_file *wf, std::string name_prefix) const;
    std::vector<std::shared_ptr<Alpide>> getChips(void) const;

  private:
    std::shared_ptr<FocalStringGroup::FocalStringGroup_O3> mOuterModule_O3[Focal_O3_O3_O3_O6::O3_GROUPS];
    std::shared_ptr<FocalStringGroup::FocalStringGroup_O6> mOuterModule_O6[Focal_O3_O3_O3_O6::O6_GROUPS];
  };


};

#endif
