/**
 * @file   EventRootFocal.hpp
 * @author Simon Voigt Nesbo
 * @date   March 8, 2019
 * @brief  Class for handling events for Focal stored in .root files
 */

#ifndef EVENT_ROOT_FOCAL_H
#define EVENT_ROOT_FOCAL_H

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <QString>
#include <memory>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include "Detector/Common/DetectorConfig.hpp"
#include "EventDigits.hpp"
#include <QSettings>

#define C_MAX_HITS 1000000

typedef struct {
  Int_t iEvent;
  Int_t iFolder;

  // first pixel layer
  Int_t nPix1;
  Int_t rowPix1[C_MAX_HITS];
  Int_t colPix1[C_MAX_HITS];

  //second pixel layer
  Int_t nPix2;
  Int_t rowPix2[C_MAX_HITS];
  Int_t colPix2[C_MAX_HITS];
} MacroPixelEvent;

typedef struct {
  Int_t iEvent;
  Int_t iFolder;
  
  Int_t nPixLfirst;
  Int_t yLfirst[C_MAX_HITS];
  Int_t xLfirst[C_MAX_HITS];

  Int_t nPixLsecond;
  Int_t yLsecond[C_MAX_HITS];
  Int_t xLsecond[C_MAX_HITS];
} MetricPixelEvent;

class EventRootFocal {
private:

  Detector::DetectorConfigBase mConfig;

  Detector::t_global_chip_id_to_position_func mGlobalChipIdToPositionFunc;
  Detector::t_position_to_global_chip_id_func mPositionToGlobalChipIdFunc;

  TFile* mRootFile;
  TChain* mEventTree;
  TClonesArray *mTCA;

  TFile* mHitsFile;
  TTree* mHitsTree;
  std::string mOutputPath;
  double mX;
  double mY;
  int mChipId;
  int mLayer;
  bool mHitValid;


  float _pixX;
  float _pixY; 
  int _iEvent;
  short _segment; // Segment in the FoCal detector

/*
  TBranch *mBranch_iEvent;
  TBranch *mBranch_iFolder;
  TBranch *mBranch_nPixS1;
  TBranch *mBranch_nPixS3;

  TBranch *mBranchRowS1;
  TBranch *mBranchColS1;
  TBranch *mBranchAmpS1;
  TBranch *mBranchRowS3;
  TBranch *mBranchColS3;
  TBranch *mBranchAmpS3;
*/

  MacroPixelEvent* mEvent;

  EventDigits* mEventDigits = nullptr;

  std::string mEventFileName = "";
  
  bool mRandomEventOrder;
  bool mMoreEventsLeft = true;
  uint64_t mNumEntries; // Number of entries in TTree
  uint64_t mNumEvents; // Number of events in TTree
  uint64_t mEntryCounter = 0;
  uint64_t mEventCounter = 0;

  /// Number of staves per quadrant included in the simulation
  const unsigned int mStavesPerQuadrant;

  bool mAliROOT; //indicates if the given ROOT file is an aliroot file from 2022 or later

  boost::random::mt19937 mRandHitGen;
  boost::random::uniform_real_distribution<double> *mRandHitMacroCellX, *mRandHitMacroCellY;

  boost::random::mt19937 mRandEventIdGen;
  boost::random::uniform_int_distribution<int> *mRandEventIdDist;

  void createHits(unsigned int macro_cell_col, unsigned int macro_cell_row,
                  unsigned int num_hits, unsigned int layer, EventDigits* event);
  void createHitsFromAliFOCALCellCM(double global_cm_x, double global_cm_y,
                  unsigned int num_hits, unsigned int layer, EventDigits* event);

public:
  EventRootFocal(Detector::DetectorConfigBase config,
                 Detector::t_global_chip_id_to_position_func global_chip_id_to_position_func,
                 Detector::t_position_to_global_chip_id_func position_to_global_chip_id_func,
                 const QString& event_filename,
                 unsigned int staves_per_quadrant,
                 unsigned int random_seed,
                 std::string _outpath,
                 bool random_event_order = true
);
  ~EventRootFocal();
  /// Indicates if there are more events left, or if we reached the end
  bool getMoreEventsLeft() const {return mMoreEventsLeft;}
  EventDigits* getNextROOTEvent(void);
  EventDigits* getNextAliROOTEvent(void);
  EventDigits* getNextEvent(void);


  void enableAliROOT();
  void disableAliROOT();



};


#endif /* EVENT_ROOT_FOCAL_H */
