#pragma once

#include "ADNNanorobot.hpp"
#include "ADNConstants.hpp"
#include "DASBackToTheAtom.hpp"
#include "ADNBasicOperations.hpp"

class DASCrossover {
public:
  DASCrossover() = default;
  ~DASCrossover() = default;

  ADNPointer<ADNNucleotide> firstNt_ = nullptr;
  ADNPointer<ADNNucleotide> secondNt_ = nullptr;

  void CreateCrossover(ADNPointer<ADNPart> part);
  bool IsScaffoldCrossover();
};

class DASAlgorithms {
public:
  DASAlgorithms() = default;
  ~DASAlgorithms() = default;

  static bool CheckCrossoverBetweenNucleotides(ADNPointer<ADNNucleotide> ntFirst, ADNPointer<ADNNucleotide> ntSecond, double angle_threshold, double dist_threshold);
  static std::vector<std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>>> DetectPossibleCrossovers(ADNPointer<ADNPart> nanorobot, double angle_threshold, double dist_threshold);
  static std::vector<DASCrossover*> DetectPossibleCrossovers(ADNPointer<ADNDoubleStrand> dsF, ADNPointer<ADNDoubleStrand> dsS, double angle_threshold, double dist_threshold);
  static std::vector<std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>>> DetectCrossovers(ADNPointer<ADNNanorobot> nanorobot);
  static void TraceScaffoldOrigami(ADNPointer<ADNPart> part);
  //static void MinimizeDSconnections(ADNPointer<ADNDoubleStrand> ds);
  static double CalculateTwistAngle(ADNPointer<ADNDoubleStrand> ds);
  static std::vector<std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>>> GetLinkingNucleotides(ADNPointer<ADNBaseSegment> bs);
  static SBQuantity::length CalculateAverageDistance(std::vector<std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>>> ntPairsList);
};