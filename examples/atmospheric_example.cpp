 /******************************************************************************
 *    This program is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by      *
 *   the Free Software Foundation, either version 3 of the License, or         *
 *   (at your option) any later version.                                       *
 *                                                                             *
 *   This program is distributed in the hope that it will be useful,           *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *   GNU General Public License for more details.                              *
 *                                                                             *
 *   You should have received a copy of the GNU General Public License         *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                             *
 *   Authors:                                                                  *
 *      Carlos Arguelles (University of Wisconsin Madison)                     *
 *         carguelles@icecube.wisc.edu                                         *
 *      Jordi Salvado (University of Wisconsin Madison)                        *
 *         jsalvado@icecube.wisc.edu                                           *
 *      Christopher Weaver (University of Wisconsin Madison)                   *
 *         chris.weaver@icecube.wisc.edu                                       *
 ******************************************************************************/



#include <vector>
#include <iostream>
#include "nuSQuIDS.h"

/*
 * This file demostrate the use of nuSQUIDSAtm class
 * which is design to handle a bundle of trayectories
 * such as those involved in atmospheric neutrino
 * experiments.
 */

using namespace nusquids;
squids::Const units;

double flux_function(double enu){
  return 2.2e-8*pow(enu/(100.0*units.TeV),-2.58);
  //return 1.;
}

int main()
{
  squids::Const units;
  unsigned int numneu = 3;
  std::cout << "Begin: constructing nuSQuIDS-Atm object" << std::endl;
  nuSQUIDSAtm<> nus_atm(-1.,1.0,60,1.e3*units.GeV,1.e7*units.GeV,150,numneu,both,true,true);
  //nuSQUIDSAtm<> nus_atm(-1.,0.2,50,1.e2*units.GeV,4.e4*units.GeV,150,numneu,both,true,true);
  std::cout << "End: constructing nuSQuIDS-Atm object" << std::endl;

  std::cout << "Begin: setting mixing angles." << std::endl;
  // set mixing angles and masses
  nus_atm.Set_MixingAngle(0,1,0.563942);
  nus_atm.Set_MixingAngle(0,2,0.154085);
  nus_atm.Set_MixingAngle(1,2,0.785398);

  nus_atm.Set_SquareMassDifference(1,7.65e-05);
  nus_atm.Set_SquareMassDifference(2,0.00247);

  nus_atm.Set_CPPhase(0,2,0);
  if(numneu > 3){
    nus_atm.Set_SquareMassDifference(3,10.);
    nus_atm.Set_MixingAngle(1,3,0.392699);
  }
  std::cout << "End: setting mixing angles." << std::endl;

  // setup integration settings
  nus_atm.Set_rel_error(1.0e-20);
  nus_atm.Set_abs_error(1.0e-20);

  auto e_range = nus_atm.GetERange();

  std::cout << "Begin: setting initial state." << std::endl;
  // construct the initial state
  marray<double,4> inistate{nus_atm.GetNumCos(),nus_atm.GetNumE(),2,numneu};
  std::fill(inistate.begin(),inistate.end(),0);
  for ( int ci = 0 ; ci < nus_atm.GetNumCos(); ci++){
    for ( int ei = 0 ; ei < nus_atm.GetNumE(); ei++){
      for ( int rho = 0; rho < 2; rho ++ ){
        for (int flv = 0; flv < numneu; flv++){
          // initialze muon state
          //inistate[ci][ei][rho][flv] = (flv == 1) ? flux_function(e_range[ei]) : 0.0;
          inistate[ci][ei][rho][flv] = flux_function(e_range[ei]);
        }
      }
    }
  }

  nus_atm.Set_ProgressBar(true);
  nus_atm.Set_TauRegeneration(true);

  // set the initial state
  nus_atm.Set_initial_state(inistate,flavor);
  std::cout << "End: setting initial state." << std::endl;

  //nus_atm.Set_ProgressBar(true);
  std::cout << "Begin: Evolution" << std::endl;
  nus_atm.EvolveState();
  std::cout << "End: Evolution" << std::endl;
  // we can save the current state in HDF5 format
  // for future use.
  nus_atm.WriteStateHDF5("./atmospheric_example_numneu_"+std::to_string(numneu)+".hdf5");

  return 0;
}
