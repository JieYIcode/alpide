#include <fstream>

const double occupancy = 1e-6;

void write_focal_multipl_distr(){

	std::ofstream outfile;
	outfile.open("multipl_dist_focal_raw_bins.txt");
	for(unsigned int i=0;i<15*33*4*2;i++){
		outfile<<i<<"\t"<<occupancy<<std::endl;
	}
	outfile.close();

}
