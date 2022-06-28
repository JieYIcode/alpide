/**
 * @file   FocalStaves.cpp
 * @author Simon Voigt Nesbo
 * @date   December 10, 2019
 * @brief  Classes for Focal staves
 */

#include "FocalStaves.hpp"
#include <misc/vcd_trace.hpp>

using namespace ITS;
using namespace Focal;

///@brief Constructor for Focal IB module (8 IB chips)
///@param name SystemC module name
///@param pos DetectorPosition object with position information.
///@param position_to_global_chip_id_func Pointer to function used to determine position
///                                       based on global chip id
///@param cfg Alpide chip config passed to Alpide constructor
FocalIbModule::FocalIbModule(sc_core::sc_module_name const &name,
                             Detector::DetectorPosition pos,
                             Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                             const AlpideConfig& cfg)
  : sc_module(name)
{
  socket_control_in.register_transport(std::bind(&FocalIbModule::processCommand,
                                                 this, std::placeholders::_1));

  // Create chips
  for(unsigned int i = 0; i < Focal::CHIPS_PER_FOCAL_IB_MODULE; i++) {
    pos.module_chip_id = i;

    unsigned int global_chip_id = (*position_to_global_chip_id_func)(pos);

    std::string chip_name = "Chip_" + std::to_string(global_chip_id);
    std::cout << "Creating chip with global ID " << global_chip_id;
    std::cout << ", layer " << pos.layer_id << ", stave " << pos.stave_id;
    std::cout << ", module " << pos.module_id;
    std::cout << ", module chip id " << pos.module_chip_id << std::endl;

    mChips.push_back(std::make_shared<Alpide>(chip_name.c_str(),
                                              global_chip_id,
                                              pos.module_chip_id,
                                              cfg,
                                              false)); // Inner barrel mode

    auto &chip = *mChips.back();
    chip.s_system_clk_in(s_system_clk_in);
    chip.s_data_output(socket_data_out[i]);
    socket_control_out[i].bind(chip.s_control_input);
  }
}


ControlResponsePayload
FocalIbModule::processCommand(const ControlRequestPayload &request) {
  ControlResponsePayload b;
  // SC_REPORT_INFO_VERB(name(), "Received Command", sc_core::SC_DEBUG);
  for (size_t i = 0; i < socket_control_out.size(); ++i) {
    auto result = socket_control_out[i]->transport(request);
    if (request.chipId == i)
      b = result;
  }

  return b;
}


///@brief Add SystemC signals to log in VCD trace file.
///@param[in,out] wf Pointer to VCD trace file object
///@param[in] name_prefix Name prefix to be added to all the trace names
void FocalIbModule::addTraces(sc_trace_file *wf, std::string name_prefix) const
{
  for(unsigned int i = 0; i < mChips.size(); i++) {
    std::stringstream ss_chip;
    ss_chip << name_prefix << "Chip_" << i << ".";
    std::string chip_prefix = ss_chip.str();
    mChips[i]->addTraces(wf, chip_prefix);
  }
}


///@brief Constructor for Focal OB module (5 OB chips)
///@param name SystemC module name
///@param pos DetectorPosition object with position information.
///@param position_to_global_chip_id_func Pointer to function used to determine position
///                                       based on global chip id
///@param cfg Alpide chip config passed to Alpide constructor
FocalObModule::FocalObModule(sc_core::sc_module_name const &name,
                             Detector::DetectorPosition pos,
                             Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                             const AlpideConfig& cfg)
  : sc_module(name)
{
  socket_control_in.register_transport(std::bind(&FocalObModule::processCommand,
                                                 this, std::placeholders::_1));

  // Create OB master chip first
  pos.module_chip_id = 0;
  unsigned int global_chip_id = (*position_to_global_chip_id_func)(pos);

  std::string chip_name = "Chip_" + std::to_string(global_chip_id);
  std::cout << "Creating chip with global ID " << global_chip_id;
  std::cout << ", layer " << pos.layer_id << ", stave " << pos.stave_id;
  std::cout << ", module " << pos.module_id;
  std::cout << ", module chip id " << pos.module_chip_id << std::endl;

  mChips.push_back(std::make_shared<Alpide>(chip_name.c_str(),
                                            global_chip_id,
                                            pos.module_chip_id,
                                            cfg,
                                            true, // Outer barrel mode
                                            true, // Outer barrel master
                                            Focal::CHIPS_PER_FOCAL_OB_MODULE-1)); // number of
                                                                                  // slave chips

  auto &master_chip = *mChips.back();
  master_chip.s_system_clk_in(s_system_clk_in);
  master_chip.s_data_output(socket_data_out);
  socket_control_out[0].bind(master_chip.s_control_input);


  // Create slave chips
  for(unsigned int i = 0; i < Focal::CHIPS_PER_FOCAL_OB_MODULE-1; i++) {
    pos.module_chip_id = i+1;

    unsigned int global_chip_id = (*position_to_global_chip_id_func)(pos);

    std::string chip_name = "Chip_" + std::to_string(global_chip_id);
    std::cout << "Creating chip with global ID " << global_chip_id;
    std::cout << ", layer " << pos.layer_id << ", stave " << pos.stave_id;
    std::cout << ", module " << pos.module_id;
    std::cout << ", module chip id " << pos.module_chip_id << std::endl;

    mChips.push_back(std::make_shared<Alpide>(chip_name.c_str(),
                                              global_chip_id,
                                              pos.module_chip_id,
                                              cfg,
                                              true,    // Outer barrel mode
                                              false)); // Inner barrel slave

    auto &chip = *mChips.back();
    chip.s_system_clk_in(s_system_clk_in);
    socket_control_out[i+1].bind(chip.s_control_input);

    // Connect data and busy to master chip
    master_chip.s_local_busy_in[i](chip.s_local_busy_out);
    master_chip.s_local_bus_data_in[i](chip.s_local_bus_data_out);
  }
}


ControlResponsePayload
FocalObModule::processCommand(const ControlRequestPayload &request) {
  ControlResponsePayload b;
  // SC_REPORT_INFO_VERB(name(), "Received Command", sc_core::SC_DEBUG);
  for (size_t i = 0; i < socket_control_out.size(); ++i) {
    auto result = socket_control_out[i]->transport(request);
    if (request.chipId == i)
      b = result;
  }

  return b;
}


///@brief Add SystemC signals to log in VCD trace file.
///@param[in,out] wf Pointer to VCD trace file object
///@param[in] name_prefix Name prefix to be added to all the trace names
void FocalObModule::addTraces(sc_trace_file *wf, std::string name_prefix) const
{
  for(unsigned int i = 0; i < mChips.size(); i++) {
    std::stringstream ss_chip;
    ss_chip << name_prefix << "Chip_" << i << ".";
    std::string chip_prefix = ss_chip.str();
    mChips[i]->addTraces(wf, chip_prefix);
  }
}

///@brief Constructor for Focal Inner module (3 IB chips), baseline design for 2022
///@param name SystemC module name
///@param pos DetectorPosition object with position information.
///@param position_to_global_chip_id_func Pointer to function used to determine position
///                                       based on global chip id
///@param cfg Alpide chip config passed to Alpide constructor
FocalInnerModule::FocalInnerModule(sc_core::sc_module_name const &name,
                             Detector::DetectorPosition pos,
                             Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                             const AlpideConfig& cfg)
  : sc_module(name)
{
  socket_control_in.register_transport(std::bind(&FocalInnerModule::processCommand,
                                                 this, std::placeholders::_1));

  // Create chips
  for(unsigned int i = 0; i < Focal::INNER_CHIPS_PER_FOCAL_INNER_MODULE; i++) {
    pos.module_chip_id = i;

    unsigned int global_chip_id = (*position_to_global_chip_id_func)(pos);

    std::string chip_name = "Chip_" + std::to_string(global_chip_id);
    std::cout << "Creating chip with global ID " << global_chip_id;
    std::cout << ", layer " << pos.layer_id << ", stave " << pos.stave_id;
    std::cout << ", module " << pos.module_id << " (inner)";
    std::cout << ", module chip id " << pos.module_chip_id << std::endl;

    mChips.push_back(std::make_shared<Alpide>(chip_name.c_str(),
                                              global_chip_id,
                                              pos.module_chip_id,
                                              cfg,
                                              false)); // Inner barrel mode

    auto &chip = *mChips.back();
    chip.s_system_clk_in(s_system_clk_in);
    chip.s_data_output(socket_data_out[i]);
    socket_control_out[i].bind(chip.s_control_input);
  }
}


ControlResponsePayload
FocalInnerModule::processCommand(const ControlRequestPayload &request) {
  ControlResponsePayload b;
  // SC_REPORT_INFO_VERB(name(), "Received Command", sc_core::SC_DEBUG);
  for (size_t i = 0; i < socket_control_out.size(); ++i) {
    auto result = socket_control_out[i]->transport(request);
    if (request.chipId == i)
      b = result;
  }

  return b;
}


///@brief Add SystemC signals to log in VCD trace file.
///@param[in,out] wf Pointer to VCD trace file object
///@param[in] name_prefix Name prefix to be added to all the trace names
void FocalInnerModule::addTraces(sc_trace_file *wf, std::string name_prefix) const
{
  for(unsigned int i = 0; i < mChips.size(); i++) {
    std::stringstream ss_chip;
    ss_chip << name_prefix << "Chip_" << i << ".";
    std::string chip_prefix = ss_chip.str();
    mChips[i]->addTraces(wf, chip_prefix);
  }
}

///@brief Constructor for Focal OB module (5 OB chips)
///@param name SystemC module name
///@param pos DetectorPosition object with position information.
///@param position_to_global_chip_id_func Pointer to function used to determine position
///                                       based on global chip id
///@param cfg Alpide chip config passed to Alpide constructor
FocalOuterLongModule::FocalOuterLongModule(sc_core::sc_module_name const &name,
                             Detector::DetectorPosition pos,
                             Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                             const AlpideConfig& cfg)
  : sc_module(name)
{
  socket_control_in.register_transport(std::bind(&FocalOuterLongModule::processCommand,
                                                 this, std::placeholders::_1));

  // Create OB master chip first
  pos.module_chip_id = 0;
  unsigned int global_chip_id = (*position_to_global_chip_id_func)(pos);

  std::string chip_name = "Chip_" + std::to_string(global_chip_id);
  std::cout << "Creating chip with global ID " << global_chip_id;
  std::cout << ", layer " << pos.layer_id << ", stave " << pos.stave_id;
  std::cout << ", module " << pos.module_id << " (outer)";
  std::cout << ", module chip id " << pos.module_chip_id << std::endl;

  mChips.push_back(std::make_shared<Alpide>(chip_name.c_str(),
                                            global_chip_id,
                                            pos.module_chip_id,
                                            cfg,
                                            true, // Outer barrel mode
                                            true, // Outer barrel master
                                            Focal::OUTER_CHIPS_PER_FOCAL_INNER_MODULE-1)); // number of
                                                                                  // slave chips

  auto &master_chip = *mChips.back();
  master_chip.s_system_clk_in(s_system_clk_in);
  master_chip.s_data_output(socket_data_out);
  socket_control_out[0].bind(master_chip.s_control_input);


  // Create slave chips
  for(unsigned int i = 0; i < Focal::OUTER_CHIPS_PER_FOCAL_INNER_MODULE-1; i++) {
    pos.module_chip_id = i+1;

    unsigned int global_chip_id = (*position_to_global_chip_id_func)(pos);

    std::string chip_name = "Chip_" + std::to_string(global_chip_id);
    std::cout << "Creating chip with global ID " << global_chip_id;
    std::cout << ", layer " << pos.layer_id << ", stave " << pos.stave_id;
    std::cout << ", module " << pos.module_id << " (outer)";
    std::cout << ", module chip id " << pos.module_chip_id << std::endl;

    mChips.push_back(std::make_shared<Alpide>(chip_name.c_str(),
                                              global_chip_id,
                                              pos.module_chip_id,
                                              cfg,
                                              true,    // Outer barrel mode
                                              false)); // Inner barrel slave

    auto &chip = *mChips.back();
    chip.s_system_clk_in(s_system_clk_in);
    socket_control_out[i+1].bind(chip.s_control_input);

    // Connect data and busy to master chip
    master_chip.s_local_busy_in[i](chip.s_local_busy_out);
    master_chip.s_local_bus_data_in[i](chip.s_local_bus_data_out);
  }

}


ControlResponsePayload
FocalOuterLongModule::processCommand(const ControlRequestPayload &request) {
  ControlResponsePayload b;
  // SC_REPORT_INFO_VERB(name(), "Received Command", sc_core::SC_DEBUG);
  for (size_t i = 0; i < socket_control_out.size(); ++i) {
    auto result = socket_control_out[i]->transport(request);
    if (request.chipId == i)
      b = result;
  }

  return b;
}


///@brief Add SystemC signals to log in VCD trace file.
///@param[in,out] wf Pointer to VCD trace file object
///@param[in] name_prefix Name prefix to be added to all the trace names
void FocalOuterLongModule::addTraces(sc_trace_file *wf, std::string name_prefix) const
{
  for(unsigned int i = 0; i < mChips.size(); i++) {
    std::stringstream ss_chip;
    ss_chip << name_prefix << "Chip_" << i << ".";
    std::string chip_prefix = ss_chip.str();
    mChips[i]->addTraces(wf, chip_prefix);
  }
}

///@brief Constructor for Focal OB module (5 OB chips)
///@param name SystemC module name
///@param pos DetectorPosition object with position information.
///@param position_to_global_chip_id_func Pointer to function used to determine position
///                                       based on global chip id
///@param cfg Alpide chip config passed to Alpide constructor
FocalOuterModule::FocalOuterModule(sc_core::sc_module_name const &name,
                             Detector::DetectorPosition pos,
                             Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                             const AlpideConfig& cfg)
  : sc_module(name)
{
  socket_control_in.register_transport(std::bind(&FocalOuterModule::processCommand,
                                                 this, std::placeholders::_1));

  // Create OB master chip first
  pos.module_chip_id = 0;
  unsigned int global_chip_id = (*position_to_global_chip_id_func)(pos);

  std::string chip_name = "Chip_" + std::to_string(global_chip_id);
  std::cout << "Creating chip with global ID " << global_chip_id;
  std::cout << ", layer " << pos.layer_id << ", stave " << pos.stave_id;
  std::cout << ", module " << pos.module_id << " (outer)";
  std::cout << ", module chip id " << pos.module_chip_id << std::endl;

  mChips.push_back(std::make_shared<Alpide>(chip_name.c_str(),
                                            global_chip_id,
                                            pos.module_chip_id,
                                            cfg,
                                            true, // Outer barrel mode
                                            true, // Outer barrel master
                                            Focal::OUTER_CHIPS_PER_FOCAL_OUTER_MODULE-1)); // number of
                                                                                  // slave chips

  auto &master_chip = *mChips.back();
  master_chip.s_system_clk_in(s_system_clk_in);
  master_chip.s_data_output(socket_data_out);
  socket_control_out[0].bind(master_chip.s_control_input);


  // Create slave chips
  for(unsigned int i = 0; i < Focal::OUTER_CHIPS_PER_FOCAL_OUTER_MODULE-1; i++) {
    pos.module_chip_id = i+1;

    unsigned int global_chip_id = (*position_to_global_chip_id_func)(pos);

    std::string chip_name = "Chip_" + std::to_string(global_chip_id);
    std::cout << "Creating chip with global ID " << global_chip_id;
    std::cout << ", layer " << pos.layer_id << ", stave " << pos.stave_id;
    std::cout << ", module " << pos.module_id << " (outer)";
    std::cout << ", module chip id " << pos.module_chip_id << std::endl;

    mChips.push_back(std::make_shared<Alpide>(chip_name.c_str(),
                                              global_chip_id,
                                              pos.module_chip_id,
                                              cfg,
                                              true,    // Outer barrel mode
                                              false)); // Inner barrel slave

    auto &chip = *mChips.back();
    chip.s_system_clk_in(s_system_clk_in);
    socket_control_out[i+1].bind(chip.s_control_input);

    // Connect data and busy to master chip
    master_chip.s_local_busy_in[i](chip.s_local_busy_out);
    master_chip.s_local_bus_data_in[i](chip.s_local_bus_data_out);
  }
}


ControlResponsePayload
FocalOuterModule::processCommand(const ControlRequestPayload &request) {
  ControlResponsePayload b;
  // SC_REPORT_INFO_VERB(name(), "Received Command", sc_core::SC_DEBUG);
  for (size_t i = 0; i < socket_control_out.size(); ++i) {
    auto result = socket_control_out[i]->transport(request);
    if (request.chipId == i)
      b = result;
  }

  return b;
}


///@brief Add SystemC signals to log in VCD trace file.
///@param[in,out] wf Pointer to VCD trace file object
///@param[in] name_prefix Name prefix to be added to all the trace names
void FocalOuterModule::addTraces(sc_trace_file *wf, std::string name_prefix) const
{
  for(unsigned int i = 0; i < mChips.size(); i++) {
    std::stringstream ss_chip;
    ss_chip << name_prefix << "Chip_" << i << ".";
    std::string chip_prefix = ss_chip.str();
    mChips[i]->addTraces(wf, chip_prefix);
  }
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
FocalInnerStave::FocalInnerStave(sc_core::sc_module_name const &name,
                                 Detector::DetectorPosition pos,
                                 Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                                 const Detector::DetectorConfigBase& cfg)
  : StaveInterface(name, pos.layer_id, pos.stave_id,
                   Focal::CTRL_LINKS_PER_INNER_STAVE, Focal::DATA_LINKS_PER_INNER_STAVE)
{
  pos.sub_stave_id = 0;
  pos.module_id = 0;

  //----------------------------------------------------------------------------
  // Create first modules ("IB modules")
  //----------------------------------------------------------------------------
  for(unsigned int i=0;i<INNER_MODULES_PER_INNER_STAVE;i++){
    pos.module_id = i;
    std::string mod_name = "Mod_";
    mod_name += std::to_string(pos.layer_id) + ":";
    mod_name += std::to_string(pos.stave_id) + ":";
    mod_name += std::to_string(pos.module_id);

    std::cout << "Creating: " << mod_name << std::endl;

    mInnerModule[i] = std::make_shared<FocalInnerModule>(mod_name.c_str(),
                                                pos,
                                                position_to_global_chip_id_func,
                                                cfg.chip_cfg);

    mInnerModule[i]->s_system_clk_in(s_system_clk_in);

    // Bind incoming control sockets to processCommand() in FocalIbModule object
    // There is only one control link in this stave
    socket_control_in[i].register_transport(std::bind(&FocalInnerModule::processCommand,
                                                      mInnerModule[i],
                                                      std::placeholders::_1));

    // Forward data from chips in FocalIbModule object to StaveInterface
    for(unsigned int link_id = 0; link_id < Focal::INNER_CHIPS_PER_FOCAL_INNER_MODULE; link_id++){
      mInnerModule[i]->socket_data_out[link_id](socket_data_out[link_id]);
    }


  }


  //----------------------------------------------------------------------------
  // Create second module (OB half-module)
  //----------------------------------------------------------------------------
  for(unsigned int i=0;i<OUTER_MODULES_PER_INNER_STAVE;i++){
    pos.module_id++;
    unsigned int i_ctrl = i+INNER_MODULES_PER_INNER_STAVE;
    std::string mod_name = "Mod_";
    mod_name += std::to_string(pos.layer_id) + ":";
    mod_name += std::to_string(pos.stave_id) + ":";
    mod_name += std::to_string(pos.module_id);

    std::cout << "Creating: " << mod_name << std::endl;

    mOuterModule[i] = std::make_shared<FocalOuterLongModule>(mod_name.c_str(),
                                            pos,
                                            position_to_global_chip_id_func,
                                            cfg.chip_cfg);

    mOuterModule[i]->s_system_clk_in(s_system_clk_in);

    // Bind incoming control sockets to processCommand() in HalfModule object
    //  Note: There is only one control link in this stave
    socket_control_in[i_ctrl].register_transport(std::bind(&FocalOuterLongModule::processCommand,
                                                      mOuterModule[i],
                                                      std::placeholders::_1));
    // Forward data from HalfModule object to StaveInterface
    mOuterModule[i]->socket_data_out(socket_data_out[i_ctrl]);

  }
}


///@brief Add SystemC signals to log in VCD trace file.
///@param[in,out] wf Pointer to VCD trace file object
///@param[in] name_prefix Name prefix to be added to all the trace names
void FocalInnerStave::addTraces(sc_trace_file *wf, std::string name_prefix) const
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

std::vector<std::shared_ptr<Alpide>> FocalInnerStave::getChips(void) const
{

  std::vector<std::shared_ptr<Alpide>> chips;

  for(unsigned int mod = 0; mod < INNER_MODULES_PER_INNER_STAVE; mod++) {
    auto mod_chips = mInnerModule[mod]->getChips();
    chips.insert(chips.end(), mod_chips.begin(), mod_chips.end());
  }

  for(unsigned int mod = 0; mod < OUTER_MODULES_PER_INNER_STAVE; mod++) {
    auto mod_chips = mOuterModule[mod]->getChips();
    chips.insert(chips.end(), mod_chips.begin(), mod_chips.end());
  }

  return chips;

}


///@brief Create an "outer stave" object for Focal. Focal outer stave consists of 15 chips in total:
///       1x IB chip + 2x OB half-module (14 OB chips)
///@param name SystemC module name
///@param pos DetectorPositionBase object with layer and stave information. The other position
///           parameters (sub stave, module id, chip id) are ignored, as chips are generated
///           for all the possible sub-positions by this function.
///@param position_to_global_chip_id_func Pointer to function used to determine position
///                                       based on global chip id
///@param cfg Detector configuration object
FocalOuterStave::FocalOuterStave(sc_core::sc_module_name const &name,
                                 Detector::DetectorPosition pos,
                                 Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                                 const Detector::DetectorConfigBase& cfg)
  : StaveInterface(name, pos.layer_id, pos.stave_id,
                   Focal::CTRL_LINKS_PER_OUTER_STAVE, Focal::DATA_LINKS_PER_OUTER_STAVE)
{
  pos.sub_stave_id = 0;

  // Create half of the half modules for one sub stave, and half for other sub stave
  // In hindsight it would have made more sense to have a Module object instead of creating
  // two HalfModule objects, since it got pretty complicated with the indexes and positions here..
  for (unsigned int i = 0; i < OUTER_MODULES_PER_OUTER_STAVE; i++) {
    pos.module_id = i;

    std::string mod_name = "Mod_";
    mod_name += std::to_string(pos.layer_id) + ":";
    mod_name += std::to_string(pos.stave_id) + ":";
    mod_name += std::to_string(pos.module_id);

    std::cout << "Creating: " << mod_name << std::endl;

    mOuterModules[i] = std::make_shared<FocalOuterModule>(mod_name.c_str(),
                                                    pos,
                                                    position_to_global_chip_id_func,
                                                    cfg.chip_cfg);

    mOuterModules[i]->s_system_clk_in(s_system_clk_in);

    // Bind incoming control sockets to processCommand() in respective FocalObModule objects
    // Note: There is only one control link in this stave
    socket_control_in[i].register_transport(std::bind(&FocalOuterModule::processCommand,
                                                      mOuterModules[i],
                                                      std::placeholders::_1));

    // Forward data from HalfModule object to StaveInterface
    mOuterModules[i]->socket_data_out(socket_data_out[i]);
  }
}


///@brief Add SystemC signals to log in VCD trace file.
///@param[in,out] wf Pointer to VCD trace file object
///@param[in] name_prefix Name prefix to be added to all the trace names
void FocalOuterStave::addTraces(sc_trace_file *wf, std::string name_prefix) const
{
  std::stringstream ss;
  ss << name_prefix << "FOS_" << getLayerId() << "_" << getStaveId() << ".";
  std::string FOS_stave_name_prefix = ss.str();

  for(unsigned int i = 0; i < MODULES_PER_OUTER_STAVE; i++) {
    std::stringstream ss_mod;
    ss_mod << FOS_stave_name_prefix << "Mod_" << i << ".";
    std::string mod_prefix = ss_mod.str();
    mOuterModules[i]->addTraces(wf, mod_prefix);
  }
}

std::vector<std::shared_ptr<Alpide>> FocalOuterStave::getChips(void) const
{
  std::vector<std::shared_ptr<Alpide>> chips;

  for(unsigned int mod = 0; mod < OUTER_MODULES_PER_OUTER_STAVE; mod++) {
    auto mod_chips = mOuterModules[mod]->getChips();
    chips.insert(chips.end(), mod_chips.begin(), mod_chips.end());
  }

  return chips;
}
