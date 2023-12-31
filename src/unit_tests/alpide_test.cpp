/**
 * @file   alpide_test.cpp
 * @author Simon Voigt Nesbo
 * @date   March 7, 2017
 * @brief  "Unit test" for the whole Alpide chip.
 *         Boost test could presumably not be used for this test, because both SystemC and boost test
 *         expects to be in charge of main.
 *         Anyway, the test does the following:
 *         1) Sets up an Alpide chip object
 *         2) Sets up an AlpideDataParser object
 *         3) Sets up necessary clocks and signals, and connects the alpide and parser objects.
 *         4) Creates an event with some hits, and feeds it to the alpide
 *         5) Starts the SystemC simulation and lets it run for a little while
 *         6) Verifies that the parser has received the event and that the hits are the same
 */

#include "Alpide/Alpide.hpp"
#include "../AlpideDataParser/AlpideDataParser.hpp"
#include <boost/random/random_device.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <vector>
#include <iostream>


int sc_main(int argc, char** argv)
{
  std::vector<PixelHit> hit_vector;
  int rand_x, rand_y;
  int chip_id = 0;
  int event_id = 0;
  int dtu_delay_cycles = 10;
  bool continuous_mode = false;
  bool enable_data_long = true;
  bool matrix_readout_speed = true;
  bool test_passed = true;

  // Set up random number generators
  boost::random::mt19937 rand_gen;
  boost::random::uniform_int_distribution<int> rand_x_dist(0, N_PIXEL_COLS-1);
  boost::random::uniform_int_distribution<int> rand_y_dist(0, N_PIXEL_ROWS-1);
  boost::random::random_device r;
  std::cout << "Boost random_device entropy: " << r.entropy() << std::endl;
  unsigned int random_seed = r();
  rand_gen.seed(random_seed);


  std::cout << "Setting up Alpide SystemC simulation" << std::endl;

  // Setup SystemC stuff
  AlpideConfig alpidecfg;
  alpidecfg.dtu_delay_cycles = dtu_delay_cycles;
  alpidecfg.data_long_en = enable_data_long;
  alpidecfg.chip_continuous_mode = continuous_mode;
  alpidecfg.matrix_readout_speed  = matrix_readout_speed;
  
  Alpide alpide(sc_core::sc_module_name("alpide"),64,128,alpidecfg, false, false, 0);
  AlpideDataParser parser(sc_core::sc_module_name("parser"), enable_data_long, 100000, false);

  sc_trace_file *wf = NULL;
  sc_core::sc_set_time_resolution(1, sc_core::SC_NS);

  // 25ns period, 0.5 duty cycle, first edge at 2 time units, first value is true
  sc_clock clock_40MHz("clock_40MHz", 25, 0.5, 2, true);
  sc_signal<bool> strobe_n;
  sc_signal<bool> chip_ready;

  sc_signal<sc_uint<24> > alpide_serial_data;

  alpide.s_system_clk_in(clock_40MHz);
  //alpide.s_strobe_n_in(strobe_n);
  alpide.s_chip_ready_out(chip_ready);
  //alpide.s_serial_data_output(alpide_serial_data);

  // Initialize SystemC stuff and connect signals to Alpide here
  parser.s_serial_data_in(alpide_serial_data);
  parser.s_clk_in(clock_40MHz);

  strobe_n = true;

  wf = sc_create_vcd_trace_file("alpide_test_waveforms");

  //sc_trace(wf, clock_40MHz, "clock");
  //sc_trace(wf, clock_matrix_readout, "clock_matrix_readout");
  sc_trace(wf, strobe_n, "STROBE_N");
  alpide.addTraces(wf, "");
  parser.addTraces(wf, "");

  // Start/run for x number of clock cycles
  sc_core::sc_start(1000, sc_core::SC_NS);

  // Set strobe active - the Alpide will create an event itself then
  strobe_n = false;
  sc_core::sc_start(100, sc_core::SC_NS);

  // The chip_ready signal should have been set now by the Alpide class after
  // receiving the strobe, indicating that we can feed hits to the Alpide
  std::cout << "Checking that the chip is ready...";
  if(chip_ready) {
    std::cout << "  Ok" << std::endl;
  } else {
    std::cout << "  Not ok. Chip not ready." << std::endl;
    test_passed = false;
    return -1;
  }

  // Feed hits to the chip before resuming simulation

  std::cout << "Creating event with 100 random hits" << std::endl;

  uint64_t time_now = sc_time_stamp().value();

  // Create an event frame object
  EventFrame e(time_now, time_now+1000, event_id++);

  // Create 100 random hits
  for(int i = 0; i < 100; i++) {
    rand_x = rand_x_dist(rand_gen);
    rand_y = rand_y_dist(rand_gen);

    std::cout << rand_x << ";" << rand_y << std::endl;

    // Store hits in vector - used later to check against data from serial output
    hit_vector.emplace_back(rand_x, rand_y, time_now, time_now+1000);

    // Store hit in event frame object
    const std::shared_ptr<PixelHit> hvb(&(hit_vector.back()));
    e.addHit(hvb);
  }

  // Feed event frame to Alpide
  e.feedHitsToPixelMatrix(alpide);

  // Set strobe inactive again to indicate that the chip can start working
  // on reading out the event
  strobe_n = true;

  // Start/run for x number of clock cycles
  sc_core::sc_start(10, sc_core::SC_US);


  // By now the chip object should have finished transmitting the hits,
  // so the parser should have one full hit
  std::cout << "Checking that the chip has transmitted 1 full event." << std::endl;
  if(parser.getNumEvents() == 1) {
    std::cout << "Alpide parser correctly contains 1 event." << std::endl;
  } else {
    std::cout << "Error: Alpide parser contains " << parser.getNumEvents();
    std::cout << " events, should have 1." << std::endl;
    test_passed = false;
  }

  const AlpideEventFrame* event = parser.getNextEvent();
  if(event == nullptr) {
    std::cout << "Error: parser returned null pointer for next event." << std::endl;
    test_passed = false;
    return -1;
  }

  std::cout << "Checking that the parsed event has the right amount of hits." << std::endl;
  if(event->getEventSize() == hit_vector.size()) {
    std::cout << "Alpide parser event size correctly matches input hit vector size." << std::endl;
  } else {
    std::cout << "Error: Alpide parser event size is " << event->getEventSize();
    std::cout << ", should equal input hit vector size which is " << hit_vector.size();
    std::cout << "." << std::endl;
    test_passed = false;
  }

  std::cout << "Checking that the event contains all the hits that were generated, and nothing more." << std::endl;
  while(!hit_vector.empty()) {
    if(event->pixelHitInEvent(hit_vector.back()) == false) {
      std::cout << "Error: missing pixel " << hit_vector.back().getCol() << ":";
      std::cout << hit_vector.back().getRow() << " in Alpide parser event." << std::endl;
      test_passed = false;
    } else {
      std::cout << "Success: Pixel " << hit_vector.back().getCol() << ":";
      std::cout << hit_vector.back().getRow() << " found in Alpide parser event." << std::endl;
    }
    hit_vector.pop_back();
  }

  std::cout << "Pixels in parser: " << std::endl;
  auto pix_iter = event->getPixelSetIterator();
  while(pix_iter != event->getPixelSetEnd()) {
    std::cout << pix_iter->getCol() << ";" << pix_iter->getRow() << std::endl;
    pix_iter++;
  }

  sc_core::sc_stop();

  if(wf != NULL) {
    sc_close_vcd_trace_file(wf);
  }

  ///@todo Create more advanced tests of Alpide chip here.. test that clusters (data long) are generated
  ///      correctly, that data is read out sufficiently fast, etc.

  if(test_passed == true) {
    std::cout << "All tests passed. " << std::endl;
    return 0;
  } else {
    std::cout << "One or more tests failed." << std::endl;
    return -1;
  }
}
