#pragma once
#ifndef BACKTOTHEATOM_H
#define BACKTOTHEATOM_H


#include <vector>
#include <string>
#include <map>
#include <stdlib.h>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "SBProxy.hpp"
#include "SBDDataGraphNode.hpp"
#include "SBStructuralModel.hpp"
#include "SAMSON.hpp"
#include "SBChain.hpp"
#include "SBMolecule.hpp"
#include "ADNNanorobot.hpp"
#include "ADNVectorMath.hpp"
#include "ADNBasicOperations.hpp"
#include "DASPolyhedron.hpp"


typedef std::map<int, std::vector<double>> BasePositions;
typedef std::map<DASEdge*, BasePositions> SequencePositions;
typedef std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>> NtPair;

class DASBackToTheAtom {
public:

  DASBackToTheAtom();
  ~DASBackToTheAtom();

  /**
   * Sets the nucleotides backbone, sidechain and center of mass positions for an entire
   * double strand using the BaseSegment position information.
   * \param The double strand whose nucleotides need to be set.
   */
  void SetDoubleStrandPositions(ADNPointer<ADNDoubleStrand> ds);
  void SetNucleotidesPostions(ADNPointer<ADNPart> part);

  void SetPositionLoopNucleotides(ADNPointer<ADNBaseSegment> bs);

  void GenerateAllAtomModel(ADNPointer<ADNPart> origami);
  
  void CheckDistances(ADNPointer<ADNPart> part);

  void PopulateWithMockAtoms(ADNPointer<ADNPart> origami, bool positionsFromNucleotide = false);
  //void SetAllAtomsPostions(ADNPointer<ADNPart> origami);
  // for cadnano
  /*void SetAllAtomsPostions2D(ADNPointer<ADNPart> origami);
  void SetAllAtomsPostions1D(ADNPointer<ADNPart> origami);*/

  /**
   * Rotates a nucleotide along base-pair plane (defined by nt->e3_).
   * \param the nucleotide you want to rotate
   * \param the amount of degrees
   * \param wether to also rotate the pair
   */
  //void RotateNucleotide(ADNPointer<ADNNucleotide> nt, double angle, bool set_pair);

  /**
   * Loads a nucleotide or base pair into SAMSON for testing purposes
   */
  //void DisplayDNABlock(std::string block);

private:
  ADNPointer<ADNNucleotide> da_;
  ADNPointer<ADNNucleotide> dt_;
  ADNPointer<ADNNucleotide> dg_;
  ADNPointer<ADNNucleotide> dc_;
  NtPair da_dt_;
  NtPair dt_da_;
  NtPair dc_dg_;
  NtPair dg_dc_;
  /** Loads the four types of nucleotide as members
  */
  void LoadNucleotides();
  /** Loads the four base pairs as members
  */
  void LoadNtPairs();
  /** Parses a nucleotide PDB file.
   *  \param a string with the location of the PDB.
   *  \return a nucleotide object.
   */
  ADNPointer<ADNNucleotide> ParsePDB(std::string source);
  /** Parses a base pair PDB file.
  *  \param a string with the location of the PDB.
  *  \return a pair of nucleotide objects.
  */
  NtPair ParseBasePairPDB(std::string source);

  void SetNucleotidePosition(ADNPointer<ADNBaseSegment> bs, bool set_pair, double initialAngleDegrees = 0.0);
  void PositionLoopNucleotides(ADNPointer<ADNLoop> loop, SBPosition3 bsPositionPrev, SBPosition3 bsPositionNext);

  void PopulateNucleotideWithAllAtoms(ADNPointer<ADNPart> origami, ADNPointer<ADNNucleotide> nt);
  void CreateBonds(ADNPointer<ADNPart> origami);
  void FindAtomsPositions(ADNPointer<ADNNucleotide> nt);

  /*!
   * Calculate the center of mass of backbone and sidechain w.r.t. atom coordinates
   *  \param the nucleotide
   *  \return tuple with center values. first is total c.o.m., second is backbone, third is sidechain.
   */
  static std::tuple<SBPosition3, SBPosition3, SBPosition3> CalculateCenters(ADNPointer<ADNNucleotide> nt);
  static std::tuple<SBPosition3, SBPosition3, SBPosition3> CalculateCentersOfMass(ADNPointer<ADNNucleotide> nt);
  /*!
   * Generate a SBPosition from a ublas vector assuming picometers -> angstrom conversion
   */
  static SBPosition3 UblasToSBPosition(ublas::vector<double> vec);
  /*!
   * Sets the reference frame for the pair to the standard basis
   */
  void SetReferenceFrame(NtPair pair);
  /** Sets the positions of a list of atoms according to a matrix positions
  */
  int SetAtomsPositions(CollectionMap<ADNAtom> atoms, ublas::matrix<double> new_positions, int r_id);
  /**
   * 
   */
  ADNPointer<ADNAtom> CopyAtom(ADNPointer<ADNAtom> atom);
};

#endif