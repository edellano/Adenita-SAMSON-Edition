#pragma once

namespace ADNConstants {
  // Structural constants
  double const BP_RISE = 0.345; // nm
  double const DH_DIST = 8; // distance between double helices (angstroms)
  double const SZ_TRUN = 4.0; // size of truncated vertices
  double const BP_ROT = 34.3; // rotation in degrees of nts around Z axis
  double const DH_DIAMETER = 2.0; // nm
  double const BP_CADNANO_DIST = 0.5f; //0.25 * dh_diameter_; //nm
  double const CROSSOVER_THRESHOLD = 15.0;  // threshold to detect crossover by distance (angstroms)
  double const CROSSOVER_ANGLE_THRESHOLD = 25.0;  // threshold to detect crossover by angle (degrees)

  // Math constants
  double const EPSILON = 1.0e-6;

  // Other constants
  double const JSON_FORMAT_VERSION = 0.4;  // version of the json format that we are using, to keep compatibility 
}