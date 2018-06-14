/** \file
 *   \brief    Retrieving all atom structure for DASDaedalus.
 *   \details  From the DASDaedalus routing and stapling list, this
 *             infers the all-atom structure of the object if formed properly.
 *             Stand back, I'm going to try math.
 *   \author   Elisa de Llano <elisa.dellano.fl@ait.ac.at>
 */
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
#include "ANTModel.hpp"
#include "ANTNanorobot.hpp"
#include "ANTVectorMath.hpp"


using namespace ANTVectorMath;

typedef std::map<int, std::vector<double>> BasePositions;
typedef std::map<ANTEdge*, BasePositions> SequencePositions;
typedef std::pair<ANTNucleotide*, ANTNucleotide*> NtPair;

class DASBackToTheAtom {
public:

  DASBackToTheAtom();
  ~DASBackToTheAtom() = default;

  ANTNucleotide* da_;
  ANTNucleotide* dt_;
  ANTNucleotide* dg_;
  ANTNucleotide* dc_;
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

  // Experimental methods
  /**
   * Sets the nucleotides backbone, sidechain and center of mass positions for an entire
   * double strand using the BaseSegment position information.
   * \param The double strand whose nucleotides need to be set.
   */
  void SetDoubleStrandPositions(ANTDoubleStrand& ds);

  // Tested methods

  /**
   * Sets backbone and sidechain positions for a nucleotide
   */
  void SetNucleotidePosition(ANTPart& origami, ANTNucleotide* nt, bool set_pair);
  void SetNucleotidesPostions(ANTPart& part);
  void SetNucleotidesPostionsLegacy(ANTPart& part);
  void SetNucleotidesPostions(ANTPart& origami, ANTSingleStrand& single_strand);
  void SetAllAtomsPostions(ANTPart& origami);
  // for cadnano
  void SetAllAtomsPostions2D(ANTPart& origami);
  void SetAllAtomsPostions1D(ANTPart& origami);

  /**
   * Rotates a nucleotide along base-pair plane (defined by nt->e3_).
   * \param the nucleotide you want to rotate
   * \param the amount of degrees
   * \param wether to also rotate the pair
   */
  void RotateNucleotide(ANTNucleotide* nt, double angle, bool set_pair);

  /**
   * Loads a nucleotide or base pair into SAMSON for testing purposes
   */
  void DisplayDNABlock(std::string block);
  /** Sets the center of mass position, backbone c.o.m. and sidechain c.o.m. of
  *  the nucleotides associated with a base (in global space).
  */
  void SetNucleotidePositionUnpaired(ANTBaseSegment* bs);
  void SetNucleotidePositionPaired(ANTBaseSegment* bs, bool set_pair, double initialAngleDegrees = 0.0);
  void SetPositionLoopNucleotides(ANTBaseSegment* bs);

private:
  /** Parses a nucleotide PDB file.
   *  \param a string with the location of the PDB.
   *  \return a nucleotide object.
   */
  ANTNucleotide* ParsePDB(std::string source);
  /** Parses a base pair PDB file.
  *  \param a string with the location of the PDB.
  *  \return a pair of nucleotide objects.
  */
  NtPair ParseBasePairPDB(std::string source);
  /** Sets the center of mass position, backbone c.o.m. and sidechain c.o.m. of
  *  the nucleotides associated with a base (in global space).
  */
  void FindAtomsPositions(ANTBaseSegment* bs, ANTNucleotide* nt);
  // for cadnano
  void FindAtomsPositions2D(ANTBaseSegment* bs, ANTNucleotide* nt);
  void FindAtomsPositions1D(ANTBaseSegment* bs, ANTNucleotide* nt);
  /*!
   * Calculate the center of mass of backbone and sidechain w.r.t. atom coordinates
   *  \param the nucleotide
   *  \return tuple with center values. first is total c.o.m., second is backbone, third is sidechain.
   */
  static std::tuple<SBPosition3, SBPosition3, SBPosition3> CalculateCenters(ANTNucleotide* nt);
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
  int SetAtomsPositions(std::map<int, ANTAtom *> &atoms, ublas::matrix<double> new_positions, int r_id);
};

#endif