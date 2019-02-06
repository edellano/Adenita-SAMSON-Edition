#pragma once

namespace ADNConstants {
  // Structural constants
  double const BP_RISE = 0.345;  // nm
  double const DH_DIST = 8;  // distance between double helices (angstroms)
  double const BP_ROT = 34.3;  // rotation in degrees of nts around Z axis
  double const DH_DIAMETER = 2.0;  // nm
  double const BP_CADNANO_DIST = 0.5f;  //0.25 * dh_diameter_; //nm

  // Algorithm constants
  double const BP_THRESHOLD = 1.0;  // threshold for detecting pairs (nm)
  double const DS_ANGLE_THRESHOLD = 30.0;  // threshold for detecting double strands (grad)
  double const MAX_CUTOFF_XO = 0.55;  // upper limit for detecting crossovers (nm)
  double const MIN_CUTOFF_XO = 0.20;  // lower limit for detecting crossovers (nm)

  // Other constants
  double const JSON_FORMAT_VERSION = 0.4;  // version of the json format that we are using, to keep compatibility 
  enum LatticeType {
    Unknown = -1,
    Honeycomb = 0,
    Square = 1,
  };
}