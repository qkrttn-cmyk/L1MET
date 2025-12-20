#include <cassert>
#include <cmath>
#include <cstdio>
#include "utils/rufl_io.h"
#include "firmware/puppimet.h"


#ifndef NEVENTS
#define NEVENTS 1
#endif

int main() {
  std::cout << "\n" <<"Start L1 MET Testbench" << "\n" << std::endl;
  
  // Initialize & Clean ref objects
  std::vector<std::vector<PuppiObj>> totalParticles;
  std::vector<std::vector<Sum>> met_ref;
  totalParticles.clear();
  met_ref.clear();

  std::vector<PuppiObj> inParticles;
  std::vector<Sum> met_hls;

  int n_Matched = 0;
  int n_Unmatched = 0;

  // Read Input Particles & Ref MET
  FILE* inParticlesFile = fopen("ParticlesIn.txt", "r");
  if(inParticlesFile != NULL){
    read_rufl_file<PuppiObj>(inParticlesFile, totalParticles, false);
  }
  
  FILE *refMETFile = fopen("METsOut.txt", "r");
  if(refMETFile != NULL){
    read_rufl_file<Sum>(refMETFile, met_ref, false);
  }

  #ifdef MET_WRITE_TB_FILE
    // Write HLS MET results
    FILE* HLS_MET_File = fopen("HLS_MET.txt", "w");
  #endif

  for (int iEvent=0; iEvent<NEVENTS; ++iEvent) {
    // Clear input/output objecs
    inParticles.clear();
    met_hls.clear();

    std::cout << "\n" << "Event " << iEvent << " Start" << std::endl;

    inParticles = totalParticles[iEvent];

    Particle_T particles[NPARTICLES];
    
    // For Debug
    double met_sw_px = 0;
    double met_sw_py = 0;
    double met_sw_pt = 0;
    double met_sw_phi = 0;

    Particle_xy met_xy;
    Sum hw_met;
    METCtrlToken token_d;
    METCtrlToken token_q;

    for(int i=0; i<NPARTICLES; i++){
      // Reshape the Deregionizer output for the HLS MET calculation
      particles[i].hwPt = (i < inParticles.size()) ? pt_t(inParticles[i].hwPt) : pt_t(0);
      particles[i].hwPhi = (i < inParticles.size()) ? phi_t(inParticles[i].hwPhi) : phi_t(0);
      
      // SW MET Calculation
      met_sw_px -= particles[i].hwPt.to_float() * cos(floatPhi(particles[i].hwPhi));
      met_sw_py -= particles[i].hwPt.to_float() * sin(floatPhi(particles[i].hwPhi));
    }

    met_sw_pt = hypot(met_sw_px, met_sw_py);
    met_sw_phi = atan2(met_sw_py, met_sw_px);

    // HLS MET Calculation
    puppimet_xy(particles, met_xy, token_d, token_q);
    pxpy_to_ptphi(met_xy, hw_met, token_d, token_q);
    met_hls.push_back(hw_met);

    #ifdef MET_WRITE_TB_FILE
      write_rufl_event(HLS_MET_File, met_hls, iEvent, "HLS MET Out");
    #endif

    bool isSame;
    isSame = met_ref[iEvent][0] == met_hls[0];
    if (isSame) n_Matched += 1;
    else n_Unmatched += 1;
    

    #ifdef MET_VERBOSE
      std::cout << "Is Same? " << std::boolalpha << (isSame) << std::endl;
      
      std::cout << "Calculated SW MET: " << met_sw_pt
                << ", phi: " << met_sw_phi
                << std::endl;

      std::cout << "Calculated REF MET: " << met_ref[iEvent][0].hwPt.to_float() 
                << ", phi: " << floatPhi(phi_t(met_ref[iEvent][0].hwPhi)) << "\n"
                << "Binary Pt: " << met_ref[iEvent][0].hwPt.to_string()
                << ", Binary Phi: " << met_ref[iEvent][0].hwPhi.to_string()
                << std::endl;

      std::cout << "Calculated HLS MET: " << met_hls[0].hwPt.to_float() 
                << ", phi: " << floatPhi(phi_t(met_hls[0].hwPhi)) << "\n"
                << "Binary Pt: " << met_hls[0].hwPt.to_string()
                << ", Binary Phi: " << met_hls[0].hwPhi.to_string()
                << std::endl;
    #endif
    }

    #ifdef MET_DEBUG
      std::cout << "\n" << "[REF & HLS] Matched Events: " << n_Matched
                << ", [REF & HLS] Unmatched Events: " << n_Unmatched << "\n"
                << std::endl;
    #endif

    return 0;
}
  
