#include "./utils/PathFinder.cpp"

TH2Poly *h1_pixels_avg = new TH2Poly();
TH2Poly *h1_busy_avg = new TH2Poly();
TH2Poly *h1_busyv_avg = new TH2Poly();
TH2Poly *h1_flush_avg = new TH2Poly();
TH2Poly *h1_frame_efficiency = new TH2Poly();
TH2Poly *h1_frame_loss = new TH2Poly();
TH2Poly *h1_data = new TH2Poly();
TH2Poly *h3_pixels_avg = new TH2Poly();
TH2Poly *h3_busy_avg = new TH2Poly();
TH2Poly *h3_busyv_avg = new TH2Poly();
TH2Poly *h3_flush_avg = new TH2Poly();
TH2Poly *h3_frame_efficiency = new TH2Poly();
TH2Poly *h3_frame_loss = new TH2Poly();
TH2Poly *h3_data = new TH2Poly();

void sumFocalData(){

	std::string basedir = "/software/max/alpide_systemc/sim_output_rate1000_trigger10000/";
	std::string head = "focal";
	std::string ext = ".root";
	std::vector<string> focalpaths;
	PathFinder(basedir, head, ext, focalpaths);

	for(std::string fp : focalpaths){
		std::cout << fp << std::endl;
	}


}
