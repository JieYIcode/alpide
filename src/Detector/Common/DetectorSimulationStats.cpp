/**
 * @file   DetectorSimulationStats.cpp
 * @author Simon Voigt Nesbo
 * @date   August 28, 2017
 * @brief  Functions etc. for writing simulation stats to file for Detector/ALPIDE
 */

#include "DetectorSimulationStats.hpp"
#include "DetectorConfig.hpp"

#include "TTree.h"
#include "TFile.h"


#include <iostream>


using namespace Detector;

///@brief Write simulation data to file. Histograms for MEB usage from the Alpide chips,
///       and event frame statistics (number of accepted/rejected) in the chips are recorded here
///@param[in] alpide_map Vector of pointers to Alpide chip objects.
///@param[in] global_chip_id_to_position_func Pointer to function used to determine global
///                                           chip id based on position
void Detector::writeAlpideStatsToFile(std::string output_path,
                                      const std::map<unsigned int, std::shared_ptr<Alpide>>& alpide_map,
                                      t_global_chip_id_to_position_func global_chip_id_to_position_func)
{
  std::vector<std::map<unsigned int, std::uint64_t> > alpide_histos;
  unsigned int all_histos_biggest_key = 0;

  std::string csv_filename = output_path + std::string("/Alpide_MEB_histograms.csv");
  ofstream csv_file(csv_filename);

  if(!csv_file.is_open()) {
    std::cerr << "Error opening CSV file for histograms: " << csv_filename << std::endl;
    return;
  }

  csv_file << "Multi Event Buffers in use";

  std::cout << "Getting MEB histograms from chips. " << std::endl;

  // Get histograms from chip objects, and finish writing CSV header
  for(auto const & chip_it : alpide_map) {
    //std::cout << "{" << item.first  <<"," << item.second << "}\n";
    if(chip_it.second != nullptr) {
    //for(auto it = alpide_vec.begin(); it != alpide_vec.end(); it++) {
    //if(*it != nullptr) {
      int chip_id = chip_it.second->getGlobalChipId();
      csv_file << ";Chip ID " << chip_id;

      alpide_histos.push_back(chip_it.second->getMEBHisto());

      // Check and possibly update the biggest MEB size (key) found in the histograms
      auto current_histo = alpide_histos.back();
      if(current_histo.rbegin() != current_histo.rend()) {
        unsigned int current_histo_biggest_key = current_histo.rbegin()->first;
        if(all_histos_biggest_key < current_histo_biggest_key)
          all_histos_biggest_key = current_histo_biggest_key;
      }
    }
  }


  std::cout << "Writing MEB histograms to file. " << std::endl;


  // Write values to CSV file
  for(unsigned int MEB_size = 0; MEB_size <= all_histos_biggest_key; MEB_size++) {
    csv_file << std::endl;
    csv_file << MEB_size;

    for(unsigned int i = 0; i < alpide_histos.size(); i++) {
      csv_file << ";";

      auto histo_it = alpide_histos[i].find(MEB_size);

      // Write value if it was found in histogram
      if(histo_it != alpide_histos[i].end())
        csv_file << histo_it->second;
      else
        csv_file << 0;
    }
  }


  std::cout << "Writing Alpide stats to file. " << std::endl;

  std::string alpide_stats_filename = output_path + std::string("/Alpide_stats.csv");
  ofstream alpide_stats_file(alpide_stats_filename);

  alpide_stats_file << "Layer ID; Stave ID; Sub-stave ID; Module ID; Local Chip ID; Unique Chip ID; ";
  alpide_stats_file << "Received triggers; Accepted triggers; Rejected triggers; ";
  alpide_stats_file << "Busy; Busy violations; Flushed Incompletes;";
  alpide_stats_file << "Latched pixel hits; Duplicate pixel hits;";
  alpide_stats_file << "ALPIDE_IDLE; ALPIDE_CHIP_HEADER; ALPIDE_CHIP_TRAILER;";
  alpide_stats_file << "ALPIDE_CHIP_EMPTY_FRAME; ALPIDE_REGION_HEADER;";
  alpide_stats_file << "ALPIDE_REGION_TRAILER; ALPIDE_DATA_SHORT; ALPIDE_DATA_LONG;";
  alpide_stats_file << "ALPIDE_BUSY_ON; ALPIDE_BUSY_OFF; ALPIDE_COMMA; ALPIDE_UNKNOWN" << std::endl;

  for(auto const & chip_it : alpide_map) {
    if(chip_it.second != nullptr) {
      unsigned int unique_chip_id = chip_it.second->getGlobalChipId();
      DetectorPosition pos = (*global_chip_id_to_position_func)(unique_chip_id);

      alpide_stats_file << pos.layer_id << ";";
      alpide_stats_file << pos.stave_id << ";";
      alpide_stats_file << pos.sub_stave_id << ";";
      alpide_stats_file << pos.module_id << ";";
      alpide_stats_file << pos.module_chip_id << ";";
      alpide_stats_file << unique_chip_id << ";";
      alpide_stats_file << chip_it.second->getTriggersReceivedCount() << ";";
      alpide_stats_file << chip_it.second->getTriggersAcceptedCount() << ";";
      alpide_stats_file << chip_it.second->getTriggersRejectedCount() << ";";
      alpide_stats_file << chip_it.second->getBusyCount() << ";";
      alpide_stats_file << chip_it.second->getBusyViolationCount() << ";";
      alpide_stats_file << chip_it.second->getFlushedIncompleteCount() << ";";
      alpide_stats_file << chip_it.second->getLatchedPixelHitCount() << ";";
      alpide_stats_file << chip_it.second->getDuplicatePixelHitCount() << ";";
      alpide_stats_file << chip_it.second->getDataWordCount(ALPIDE_IDLE) << ";";
      alpide_stats_file << chip_it.second->getDataWordCount(ALPIDE_CHIP_HEADER) << ";";
      alpide_stats_file << chip_it.second->getDataWordCount(ALPIDE_CHIP_TRAILER) << ";";
      alpide_stats_file << chip_it.second->getDataWordCount(ALPIDE_CHIP_EMPTY_FRAME) << ";";
      alpide_stats_file << chip_it.second->getDataWordCount(ALPIDE_REGION_HEADER) << ";";
      alpide_stats_file << chip_it.second->getDataWordCount(ALPIDE_REGION_TRAILER) << ";";
      alpide_stats_file << chip_it.second->getDataWordCount(ALPIDE_DATA_SHORT) << ";";
      alpide_stats_file << chip_it.second->getDataWordCount(ALPIDE_DATA_LONG) << ";";
      alpide_stats_file << chip_it.second->getDataWordCount(ALPIDE_BUSY_ON) << ";";
      alpide_stats_file << chip_it.second->getDataWordCount(ALPIDE_BUSY_OFF) << ";";
      alpide_stats_file << chip_it.second->getDataWordCount(ALPIDE_COMMA) << ";";
      alpide_stats_file << chip_it.second->getDataWordCount(ALPIDE_UNKNOWN) << std::endl;
    }
  }

  // Writing alpide stats root file
  std::string rootfile_outputpath = output_path + std::string("/AlpideStats.root");
  TFile *rootfile =  new TFile(rootfile_outputpath.c_str(), "RECREATE");
  TTree *alpidetree = new TTree("ALPIDE_STATS", "alpide statistics");

  int layerid;      alpidetree->Branch("layerId", &layerid);
  int staveid;      alpidetree->Branch("staveId", &staveid);
  int substaveid;   alpidetree->Branch("substaveId", &substaveid);
  int moduleid;     alpidetree->Branch("moduleId", &moduleid);
  int modulechipid; alpidetree->Branch("moduleChipId", &modulechipid);
  int uniquechipid; alpidetree->Branch("globalChipId", &uniquechipid);

  int triggersreceived;   alpidetree->Branch("nTriggersReceived", &triggersreceived);
  int triggersaccepted;   alpidetree->Branch("nTriggersAccepted", &triggersaccepted);
  int triggersrejected;   alpidetree->Branch("nTriggersRejected", &triggersrejected);

  int busy;       alpidetree->Branch("nBusy", &busy);
  int busyv;      alpidetree->Branch("nBusyViolations", &busyv);
  int flushed;    alpidetree->Branch("nFlushedIncomplete", &flushed);
  int latched;    alpidetree->Branch("nLatchedPixelHit", &latched);
  int duplicate;  alpidetree->Branch("nDuplicatePixelHit", &duplicate);

  int idle;       alpidetree->Branch("ALPIDE_IDLE", &idle);
  int header;     alpidetree->Branch("ALPIDE_CHIP_HEADER", &header);
  int trailer;    alpidetree->Branch("ALPIDE_CHIP_TRAILER", &trailer);
  int empty;      alpidetree->Branch("ALPIDE_CHIP_EMPTY_FRAME", &empty);

  int regionheader;   alpidetree->Branch("ALPIDE_REGION_HEADER", &regionheader);
  int regiontrailer;  alpidetree->Branch("ALPIDE_REGION_TRAILER", &regiontrailer);

  int datashort;      alpidetree->Branch("ALPIDE_DATA_SHORT", &datashort);
  int datalong;       alpidetree->Branch("ALPIDE_DATA_LONG", &datalong);

  int busyon;      alpidetree->Branch("ALPIDE_BUSY_ON", &busyon);
  int busyoff;     alpidetree->Branch("ALPIDE_BUSY_OFF", &busyoff);

  int comma;    alpidetree->Branch("ALPIDE_COMMA", &comma);
  int unknown;  alpidetree->Branch("ALPIDE_UNKNOWN", &unknown);

  for(auto const & chip_it : alpide_map) {
    if(chip_it.second != nullptr) {
      unsigned int unique_chip_id = chip_it.second->getGlobalChipId();
      DetectorPosition pos = (*global_chip_id_to_position_func)(unique_chip_id);

      layerid =  pos.layer_id ;
      staveid =pos.stave_id ;
      substaveid = pos.sub_stave_id ;
      moduleid = pos.module_id ;
      modulechipid = pos.module_chip_id ;
      uniquechipid = unique_chip_id ;
      triggersreceived = chip_it.second->getTriggersReceivedCount() ;
      triggersaccepted = chip_it.second->getTriggersAcceptedCount() ;
      triggersrejected = chip_it.second->getTriggersRejectedCount() ;
      busy = chip_it.second->getBusyCount() ;
      busyv = chip_it.second->getBusyViolationCount();
      flushed = chip_it.second->getFlushedIncompleteCount() ;
      latched = chip_it.second->getLatchedPixelHitCount() ;
      duplicate = chip_it.second->getDuplicatePixelHitCount() ;
      idle = chip_it.second->getDataWordCount(ALPIDE_IDLE);
      header = chip_it.second->getDataWordCount(ALPIDE_CHIP_HEADER);
      trailer = chip_it.second->getDataWordCount(ALPIDE_CHIP_TRAILER) ;
      empty = chip_it.second->getDataWordCount(ALPIDE_CHIP_EMPTY_FRAME) ;
      regionheader = chip_it.second->getDataWordCount(ALPIDE_REGION_HEADER);
      regiontrailer = chip_it.second->getDataWordCount(ALPIDE_REGION_TRAILER) ;
      datashort = chip_it.second->getDataWordCount(ALPIDE_DATA_SHORT);
      datalong = chip_it.second->getDataWordCount(ALPIDE_DATA_LONG) ;
      busyon = chip_it.second->getDataWordCount(ALPIDE_BUSY_ON) ;
      busyoff = chip_it.second->getDataWordCount(ALPIDE_BUSY_OFF) ;
      comma = chip_it.second->getDataWordCount(ALPIDE_COMMA) ;
      unknown = chip_it.second->getDataWordCount(ALPIDE_UNKNOWN) ;

      alpidetree->Fill();
    }
  }

  rootfile->cd();
  alpidetree->Write();
  rootfile->Delete();

}
