#ifndef ALPIDE_DIMENSIONS
#define ALPIDE_DIMENSIONS

namespace ALPIDE {

  static const unsigned int NPIXEL_X = 1024;
  static const unsigned int NPIXEL_Y = 512;

  static const double PIXEL_X_UM = 29.24;
  static const double PIXEL_X_MM = PIXEL_X_UM/1000.;
  static const double PIXEL_X_CM = PIXEL_X_MM/10.;

  static const double PIXEL_Y_UM = 26.88;
  static const double PIXEL_Y_MM = PIXEL_Y_UM/1000.;
  static const double PIXEL_Y_CM = PIXEL_Y_MM/10.;

  static const double CHIPSIZE_X_UM = NPIXEL_X*PIXEL_X_UM;
  static const double CHIPSIZE_X_MM = NPIXEL_X*PIXEL_X_MM;
  static const double CHIPSIZE_X_CM = NPIXEL_X*PIXEL_X_CM;

  static const double CHIPSIZE_Y_UM = NPIXEL_Y*PIXEL_Y_UM;
  static const double CHIPSIZE_Y_MM = NPIXEL_Y*PIXEL_Y_MM;
  static const double CHIPSIZE_Y_CM = NPIXEL_Y*PIXEL_Y_CM;
/*
  static const unsigned int  N_REGIONS =  32;
  static const unsigned int  N_PIXEL_ROWS=  512;
  static const unsigned int  N_PIXEL_COLS=  1024;

  static const unsigned int  N_PIXEL_COLS_PER_REGION=  (N_PIXEL_COLS/N_REGIONS);
  static const unsigned int  N_PIXEL_DOUBLE_COLS_PER_REGION=  (N_PIXEL_COLS_PER_REGION/2);
  static const unsigned int  N_PIXELS_PER_REGION=  (N_PIXEL_COLS/N_REGIONS);

  static const unsigned int  REGION_FIFO_SIZE=  128;

  static const unsigned int  TRU_FRAME_FIFO_ALMOST_FULL1 = 48;
  static const unsigned int  TRU_FRAME_FIFO_ALMOST_FULL2 = 56;
  static const unsigned int  TRU_FRAME_FIFO_SIZE = 64;

  static const unsigned int  BUSY_FIFO_SIZE=  4;
  static const unsigned int  DMU_FIFO_SIZE = 4;

  static const unsigned int  DATA_LONG_PIXMAP_SIZE =  ((unsigned int) 7);

  static const unsigned int  LHC_ORBIT_BUNCH_COUNT = 3564;
*/
}

#endif