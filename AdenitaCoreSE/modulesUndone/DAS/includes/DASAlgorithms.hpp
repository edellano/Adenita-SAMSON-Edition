#pragma once

#include "ANTNanorobot.hpp"
#include "ANTConstants.hpp"
#include "DASBackToTheAtom.hpp"

class DASCrossover {
public:
  DASCrossover() = default;
  ~DASCrossover() = default;

  ANTNucleotide* firstNt = nullptr;
  ANTNucleotide* secondNt = nullptr;

  void CreateCrossover(ANTNanorobot& nanorobot);
  bool IsScaffoldCrossover();
};

class DASAlgorithms {
public:
  DASAlgorithms() = default;
  ~DASAlgorithms() = default;

  static bool CheckCrossoverBetweenNucleotides(ANTNucleotide* ntFirst, ANTNucleotide* ntSecond, double angle_threshold, double dist_threshold);
  static std::vector<std::pair<ANTNucleotide*, ANTNucleotide*>> DetectPossibleCrossovers(ANTNanorobot& nanorobot, double angle_threshold, double dist_threshold);
  static std::vector<DASCrossover*> DetectPossibleCrossovers(ANTDoubleStrand* dsF, ANTDoubleStrand* dsS, double angle_threshold, double dist_threshold);
  static std::vector<std::pair<ANTNucleotide*, ANTNucleotide*>> DetectCrossovers(ANTNanorobot& nanorobot);
  static void TraceScaffoldOrigami(ANTPart* part);
  //static void MinimizeDSconnections(ANTDoubleStrand* ds);
  static double CalculateTwistAngle(ANTDoubleStrand* ds);
  static std::vector<std::pair<ANTNucleotide*, ANTNucleotide*>> GetLinkingNucleotides(ANTBaseSegment* bs);
  static SBQuantity::length CalculateAverageDistance(std::vector<std::pair<ANTNucleotide*, ANTNucleotide*>> ntPairsList);
};