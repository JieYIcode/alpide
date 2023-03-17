/**
 * @file   FocalStaves.hpp
 * @author Simon Voigt Nesbo
 * @date   December 10, 2019
 * @brief  Classes for Focal staves
 */

#include <sstream>

#include <Alpide/Alpide.hpp>
#include <Alpide/AlpideInterface.hpp>
#include "Detector/Common/DetectorConfig.hpp"
#include "Detector/Common/ITSModulesStaves.hpp"
#include "Detector/Focal/Focal_constants.hpp"
#include "Detector/Focal/FocalDetectorConfig.hpp"
#include "Detector/Focal/FocalStrings.hpp"
#include "Detector/Focal/FocalStringGroups.hpp"


using namespace Focal;
using namespace FocalString;

///@brief Create an "outer stave" object for Focal. Focal outer stave consists of 15 chips in total:
///       5x OB half-module à 3 chips (15 OB chips)
///@param name SystemC module name
///@param pos DetectorPositionBase object with layer and stave information. The other position
///           parameters (sub stave, module id, chip id) are ignored, as chips are generated
///           for all the possible sub-positions by this function.
///@param position_to_global_chip_id_func Pointer to function used to determine position
///                                       based on global chip id
///@param cfg Detector configuration object
FocalString_O3_O3_O3_O3_O3::FocalString_O3_O3_O3_O3_O3(sc_core::sc_module_name const &name,
                    Detector::DetectorPosition pos,
                    Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                    const Detector::DetectorConfigBase& cfg)
  : StaveInterface(name, pos.layer_id, pos.stave_id,
                   Focal_O3_O3_O3_O3_O3::CTRL_LINKS, Focal_O3_O3_O3_O3_O3::DATA_LINKS)
{
  pos.sub_stave_id = 0;

  // Create half of the half modules for one sub stave, and half for other sub stave
  // In hindsight it would have made more sense to have a Module object instead of creating
  // two HalfModule objects, since it got pretty complicated with the indexes and positions here..
  for (unsigned int igroup = 0; igroup < Focal_O3_O3_O3_O3_O3::GROUPS; igroup++) {
    pos.module_id = igroup;

    std::string mod_name = "Mod_";
    mod_name += std::to_string(pos.layer_id) + ":";
    mod_name += std::to_string(pos.stave_id) + ":";
    mod_name += std::to_string(pos.module_id);

    std::cout << "Creating: " << mod_name << std::endl;

    mOuterModule[igroup] = std::make_shared<FocalStringGroup::FocalStringGroup_O3>(mod_name.c_str(),
                                                    pos,
                                                    position_to_global_chip_id_func,
                                                    cfg.chip_cfg);

    mOuterModule[igroup]->s_system_clk_in(s_system_clk_in);

    // Bind incoming control sockets to processCommand() in respective FocalObModule objects
    // Note: There is only one control link in this stave
    socket_control_in[igroup].register_transport(std::bind(&FocalStringGroup::FocalStringGroup_O3::processCommand,
                                                      mOuterModule[igroup],
                                                      std::placeholders::_1));

    // Forward data from HalfModule object to StaveInterface
    mOuterModule[igroup]->socket_data_out(socket_data_out[igroup]);
  }
}


///@brief Add SystemC signals to log in VCD trace file.
///@param[in,out] wf Pointer to VCD trace file object
///@param[in] name_prefix Name prefix to be added to all the trace names
void FocalString_O3_O3_O3_O3_O3::addTraces(sc_trace_file *wf, std::string name_prefix) const
{
  std::stringstream ss;
  ss << name_prefix << "FOS_" << getLayerId() << "_" << getStaveId() << ".";
  std::string FOS_stave_name_prefix = ss.str();

  for(unsigned int i = 0; i < Focal_O3_O3_O3_O3_O3::GROUPS; i++) {
    std::stringstream ss_mod;
    ss_mod << FOS_stave_name_prefix << "Mod_" << i << ".";
    std::string mod_prefix = ss_mod.str();
    mOuterModule[i]->addTraces(wf, mod_prefix);
  }
}

std::vector<std::shared_ptr<Alpide>> FocalString_O3_O3_O3_O3_O3::getChips(void) const
{
  std::vector<std::shared_ptr<Alpide>> chips;

  for(unsigned int mod = 0; mod < Focal_O3_O3_O3_O3_O3::GROUPS; mod++) {
    auto mod_chips = mOuterModule[mod]->getChips();
    chips.insert(chips.end(), mod_chips.begin(), mod_chips.end());
  }

  return chips;
}


///@brief Create an "outer string" object for Focal. Focal outer string consists of 15 chips in total:
///       1 x OB half-module à 3 chips and 2 x OB half-module à 3 chips  (15 OB chips)
///@param name SystemC module name
///@param pos DetectorPositionBase object with layer and stave information. The other position
///           parameters (sub stave, module id, chip id) are ignored, as chips are generated
///           for all the possible sub-positions by this function.
///@param position_to_global_chip_id_func Pointer to function used to determine position
///                                       based on global chip id
///@param cfg Detector configuration object
FocalString_O3_O6_O6::FocalString_O3_O6_O6(sc_core::sc_module_name const &name,
                    Detector::DetectorPosition pos,
                    Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                    const Detector::DetectorConfigBase& cfg)
  : StaveInterface(name, pos.layer_id, pos.stave_id,
                   Focal_O3_O6_O6::CTRL_LINKS, Focal_O3_O6_O6::DATA_LINKS)
{
  pos.sub_stave_id = 0;

  // Create half of the half modules for one sub stave, and half for other sub stave
  // In hindsight it would have made more sense to have a Module object instead of creating
  // two HalfModule objects, since it got pretty complicated with the indexes and positions here..
  unsigned int igroup = 0;
  for (; igroup < Focal_O3_O6_O6::O3_GROUPS; igroup++) {
    pos.module_id = igroup;

    std::string mod_name = "Mod_";
    mod_name += std::to_string(pos.layer_id) + ":";
    mod_name += std::to_string(pos.stave_id) + ":";
    mod_name += std::to_string(pos.module_id);

    std::cout << "Creating: " << mod_name << std::endl;

    mOuterModule_O3[igroup] = std::make_shared<FocalStringGroup::FocalStringGroup_O3>(mod_name.c_str(),
                                                    pos,
                                                    position_to_global_chip_id_func,
                                                    cfg.chip_cfg);

    mOuterModule_O3[igroup]->s_system_clk_in(s_system_clk_in);

    // Bind incoming control sockets to processCommand() in respective FocalObModule objects
    // Note: There is only one control link in this stave
    socket_control_in[igroup].register_transport(std::bind(&FocalStringGroup::FocalStringGroup_O3::processCommand,
                                                      mOuterModule_O3[igroup],
                                                      std::placeholders::_1));

    // Forward data from HalfModule object to StaveInterface
    mOuterModule_O3[igroup]->socket_data_out(socket_data_out[igroup]);
  }
  for (igroup=0; igroup < Focal_O3_O6_O6::O6_GROUPS; igroup++) {
    pos.module_id = igroup+Focal_O3_O6_O6::O3_GROUPS;

    std::string mod_name = "Mod_";
    mod_name += std::to_string(pos.layer_id) + ":";
    mod_name += std::to_string(pos.stave_id) + ":";
    mod_name += std::to_string(pos.module_id);

    std::cout << "Creating: " << mod_name << std::endl;

    mOuterModule_O6[igroup] = std::make_shared<FocalStringGroup::FocalStringGroup_O6>(mod_name.c_str(),
                                                    pos,
                                                    position_to_global_chip_id_func,
                                                    cfg.chip_cfg);

    mOuterModule_O6[igroup]->s_system_clk_in(s_system_clk_in);

    // Bind incoming control sockets to processCommand() in respective FocalObModule objects
    // Note: There is only one control link in this stave
    socket_control_in[igroup].register_transport(std::bind(&FocalStringGroup::FocalStringGroup_O6::processCommand,
                                                      mOuterModule_O6[igroup],
                                                      std::placeholders::_1));

    // Forward data from HalfModule object to StaveInterface
    mOuterModule_O6[igroup]->socket_data_out(socket_data_out[igroup]);
  }
}


///@brief Add SystemC signals to log in VCD trace file.
///@param[in,out] wf Pointer to VCD trace file object
///@param[in] name_prefix Name prefix to be added to all the trace names
void FocalString_O3_O6_O6::addTraces(sc_trace_file *wf, std::string name_prefix) const
{
  std::stringstream ss;
  ss << name_prefix << "FOS_" << getLayerId() << "_" << getStaveId() << ".";
  std::string FOS_stave_name_prefix = ss.str();

  unsigned int i = 0;
  for(; i < Focal_O3_O6_O6::O3_GROUPS; i++) {
    std::stringstream ss_mod;
    ss_mod << FOS_stave_name_prefix << "Mod_" << i << ".";
    std::string mod_prefix = ss_mod.str();
    mOuterModule_O3[i]->addTraces(wf, mod_prefix);
  }
  for(i=0; i < Focal_O3_O6_O6::O6_GROUPS; i++) {
    std::stringstream ss_mod;
    ss_mod << FOS_stave_name_prefix << "Mod_" << i << ".";
    std::string mod_prefix = ss_mod.str();
    mOuterModule_O6[i]->addTraces(wf, mod_prefix);
  }
}

std::vector<std::shared_ptr<Alpide>> FocalString_O3_O6_O6::getChips(void) const
{
  std::vector<std::shared_ptr<Alpide>> chips;

  unsigned int mod=0;
  for(; mod < Focal_O3_O6_O6::O3_GROUPS; mod++) {
    auto mod_chips = mOuterModule_O3[mod]->getChips();
    chips.insert(chips.end(), mod_chips.begin(), mod_chips.end());
  }
  for(mod=0; mod < Focal_O3_O6_O6::O6_GROUPS; mod++) {
    auto mod_chips = mOuterModule_O6[mod]->getChips();
    chips.insert(chips.end(), mod_chips.begin(), mod_chips.end());
  }

  return chips;
}

///@brief Create an "inner stave" object for Focal. Focal inner stave consists of 15 chips in total:
///       1x IB chip + 1x IB "module" (7 IB chips) + 1x OB half-module
///@param name SystemC module name
///@param pos DetectorPositionBase object with layer and stave information. The other position
///           parameters (sub stave, module id, chip id) are ignored, as chips are generated
///           for all the possible sub-positions by this function.
///@param position_to_global_chip_id_func Pointer to function used to determine position
///                                       based on global chip id
///@param cfg Detector configuration object
FocalString_I3_I3_O3_O6::FocalString_I3_I3_O3_O6(sc_core::sc_module_name const &name,
                                 Detector::DetectorPosition pos,
                                 Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                                 const Detector::DetectorConfigBase& cfg)
  : StaveInterface(name, pos.layer_id, pos.stave_id,
                   Focal_I3_I3_O3_O6::CTRL_LINKS,  Focal_I3_I3_O3_O6::DATA_LINKS)
{
  pos.sub_stave_id = 0;
  pos.module_id = 0;

  //----------------------------------------------------------------------------
  // Create first modules ("IB modules")
  //----------------------------------------------------------------------------
  for(unsigned int i=0;i<Focal_I3_I3_O3_O6::INNERGROUPS;i++){
    pos.module_id = i;
    std::string mod_name = "Mod_";
    mod_name += std::to_string(pos.layer_id) + ":";
    mod_name += std::to_string(pos.stave_id) + ":";
    mod_name += std::to_string(pos.module_id);

    std::cout << "Creating: " << mod_name << std::endl;

    mInnerModule[i] = std::make_shared<FocalStringGroup::FocalStringGroup_I3>(mod_name.c_str(),
                                                pos,
                                                position_to_global_chip_id_func,
                                                cfg.chip_cfg);

    mInnerModule[i]->s_system_clk_in(s_system_clk_in);

    // Bind incoming control sockets to processCommand() in FocalIbModule object
    // There is only one control link in this stave
    socket_control_in[i].register_transport(std::bind(&FocalStringGroup::FocalStringGroup_I3::processCommand,
                                                      mInnerModule[i],
                                                      std::placeholders::_1));

    // Forward data from chips in FocalIbModule object to StaveInterface
    for(unsigned int link_id = 0; link_id < Focal_Inner3::CHIPS; link_id++){
      mInnerModule[i]->socket_data_out[link_id](socket_data_out[link_id]);
    }

  }


  //----------------------------------------------------------------------------
  // Create second module (OB half-module)
  //----------------------------------------------------------------------------
  for(unsigned int i=0;i<Focal_I3_I3_O3_O6::OUTERGROUPS_O3;i++){
    pos.module_id++;
    unsigned int i_ctrl = i+Focal_I3_I3_O3_O6::INNERGROUPS;
    std::string mod_name = "Mod_";
    mod_name += std::to_string(pos.layer_id) + ":";
    mod_name += std::to_string(pos.stave_id) + ":";
    mod_name += std::to_string(pos.module_id);

    std::cout << "Creating: " << mod_name << std::endl;

    mOuterModule0[i] = std::make_shared<FocalStringGroup::FocalStringGroup_O3>(mod_name.c_str(),
                                            pos,
                                            position_to_global_chip_id_func,
                                            cfg.chip_cfg);

    mOuterModule0[i]->s_system_clk_in(s_system_clk_in);

    // Bind incoming control sockets to processCommand() in HalfModule object
    //  Note: There is only one control link in this stave
    socket_control_in[i_ctrl].register_transport(std::bind(&FocalStringGroup::FocalStringGroup_O3::processCommand,
                                                      mOuterModule0[i],
                                                      std::placeholders::_1));
    // Forward data from HalfModule object to StaveInterface
    mOuterModule0[i]->socket_data_out(socket_data_out[i_ctrl]);

  }

  //----------------------------------------------------------------------------
  // Create third module (OB half-module)
  //----------------------------------------------------------------------------
  for(unsigned int i=0;i<Focal_I3_I3_O3_O6::OUTERGROUPS_O6;i++){
    pos.module_id++;
    unsigned int i_ctrl = i+Focal_I3_I3_O3_O6::INNERGROUPS + Focal_I3_I3_O3_O6::OUTERGROUPS_O3;
    std::string mod_name = "Mod_";
    mod_name += std::to_string(pos.layer_id) + ":";
    mod_name += std::to_string(pos.stave_id) + ":";
    mod_name += std::to_string(pos.module_id);

    std::cout << "Creating: " << mod_name << std::endl;

    mOuterModule1[i] = std::make_shared<FocalStringGroup::FocalStringGroup_O6>(mod_name.c_str(),
                                            pos,
                                            position_to_global_chip_id_func,
                                            cfg.chip_cfg);

    mOuterModule1[i]->s_system_clk_in(s_system_clk_in);

    // Bind incoming control sockets to processCommand() in HalfModule object
    //  Note: There is only one control link in this stave
    socket_control_in[i_ctrl].register_transport(std::bind(&FocalStringGroup::FocalStringGroup_O6::processCommand,
                                                      mOuterModule1[i],
                                                      std::placeholders::_1));
    // Forward data from HalfModule object to StaveInterface
    mOuterModule1[i]->socket_data_out(socket_data_out[i_ctrl]);

  }
}


///@brief Add SystemC signals to log in VCD trace file.
///@param[in,out] wf Pointer to VCD trace file object
///@param[in] name_prefix Name prefix to be added to all the trace names
void FocalString_I3_I3_O3_O6::addTraces(sc_trace_file *wf, std::string name_prefix) const
{
  std::stringstream ss;
  ss << name_prefix << "FIbS_" << getLayerId() << "_" << getStaveId() << ".";
  std::string FIbS_stave_name_prefix = ss.str();

  for(unsigned int i = 0; i < 2; i++) {
    std::stringstream ss_mod;
    ss_mod << FIbS_stave_name_prefix << "Mod_" << i << ".";
    std::string mod_prefix = ss_mod.str();

    // disabling here 23/6/2022
    // probably not needed for FOCAL for the time being
    //if(i==0)
      //mInnerModule->addTraces(wf, mod_prefix);
    //else
      //mOuterModule->addTraces(wf, mod_prefix);
  }
}

std::vector<std::shared_ptr<Alpide>> FocalString_I3_I3_O3_O6::getChips(void) const
{

  std::vector<std::shared_ptr<Alpide>> chips;

  for(unsigned int mod = 0; mod <Focal_I3_I3_O3_O6::INNERGROUPS; mod++) {
    auto mod_chips = mInnerModule[mod]->getChips();
    chips.insert(chips.end(), mod_chips.begin(), mod_chips.end());
  }

  for(unsigned int mod =0 ; mod < Focal_I3_I3_O3_O6::OUTERGROUPS_O3; mod++) {
    auto mod_chips = mOuterModule0[mod]->getChips();
    chips.insert(chips.end(), mod_chips.begin(), mod_chips.end());
  }

  for(unsigned int mod = 0; mod < Focal_I3_I3_O3_O6::OUTERGROUPS_O6; mod++) {
    auto mod_chips = mOuterModule1[mod]->getChips();
    chips.insert(chips.end(), mod_chips.begin(), mod_chips.end());
  }

  return chips;

}

