#pragma once

#include "SEConfig.hpp"
#include "ADNArray.hpp"
#include "ADNLogger.hpp"
#include "ADNVectorMath.hpp"
#include "ADNNanorobot.hpp"
#include "ADNSaveAndLoad.hpp"
#include "ADNBasicOperations.hpp"


class __declspec(dllexport) ADNMainInterface
{
public:
  // SEConfig
  static SEConfig& CreateSEConfig();

  // ADNArray
  static ADNArray<int> CreateADNArray(size_t dim, size_t num_elem);

  // ADNLogger
  static void CreateLoggingFile();

  // ADNVectorMath
  static ublas::vector<double> CreateUblasVector(std::vector<double> v);
  static ublas::vector<double> CrossProduct(ublas::vector<double> v, ublas::vector<double> w);
  static ublas::vector<double> DirectionVector(ublas::vector<double> p, ublas::vector<double> q);
  static ublas::matrix<double> CreateBoostMatrix(std::vector<std::vector<double>> vecovec);
  static ublas::vector<double> CalculateCM(ublas::matrix<double> positions);
  static double DegToRad(double degree);
  static ublas::matrix<double> MakeRotationMatrix(ublas::vector<double> dir, double angle);
  static ublas::matrix<double> ApplyTransformation(ublas::matrix<double> t_mat, ublas::matrix<double> points);
  static ublas::matrix<double> Translate(ublas::matrix<double> input, ublas::vector<double> t_vector);
  static ublas::matrix<double> CenterSystem(ublas::matrix<double> input);
  static double Determinant(ublas::matrix<double> mat);
  static ublas::vector<double> CalculatePlane(ublas::matrix<double> mat);
  static ublas::matrix<double> FindOrthogonalSubspace(ublas::vector<double> z);
  static ADNPointer<ADNAtom> CreateAtom(ADNPointer<ADNAtom> other);
  static ADNPointer<ADNAtom> CreateAtom(int id = -1);
  static ADNPointer<ADNAtom> CreateAtom(int id, std::string name, ublas::vector<double> pos);
  static void SetAtomName(ADNPointer<ADNAtom> atom, std::string name);
  static std::string GetAtomName(ADNPointer<ADNAtom> atom);
  static void SetAtomPosition(ADNPointer<ADNAtom> atom, ublas::vector<double> pos);
  static ublas::vector<double> GetAtomPosition(ADNPointer<ADNAtom> atom);
  static void SetAtomId(ADNPointer<ADNAtom> atom, int id);
  static int GetAtomId(ADNPointer<ADNAtom> atom);
  static ADNPointer<ADNBackbone> CreateBackbone();
  static ADNPointer<ADNSidechain> CreateSidechain();
  static NucleotideGroup GetGroupType(ADNPointer<ADNNucleotideGroup> ng);
  static void AddAtomToGroup(ADNPointer<ADNNucleotideGroup> ng, ADNPointer<ADNAtom> a);
  static bool IsAtomInBackbone(ADNPointer<ADNAtom> a);
  static CollectionMap<ADNAtom> GetAtomsInGroup(ADNPointer<ADNNucleotideGroup> g);
  static ADNPointer<ADNNucleotide> CreateADNNucleotide(int id = -1);
  static ADNPointer<ADNBackbone> GetNucleotideBackbone(ADNPointer<ADNNucleotide> nt);
  static ADNPointer<ADNSidechain> GetNucleotideSidechain(ADNPointer<ADNNucleotide> nt);
  static void AddAtomToNucleotide(ADNPointer<ADNNucleotide> nt, ADNPointer<ADNNucleotideGroup> g, ADNPointer<ADNAtom> a);
  static CollectionMap<ADNAtom> GetNucleotideAtoms(ADNPointer<ADNNucleotide> nt);
  static void DeleteNucleotideAtom(ADNPointer<ADNNucleotide> nt, int id);
  static void AddConnectionToAtom(ADNPointer<ADNNucleotide> nt, int id1, int id2);
  static std::multimap<ADNPointer<ADNAtom>, ADNPointer<ADNAtom>> GetNucleotideBonds(ADNPointer<ADNNucleotide> nt);
  static ADNPointer<ADNSingleStrand> CreateSingleStrand(int id = -1);
  static void AddNucleotideToSingleStrandThreePrime(ADNPointer<ADNNucleotide> nt, ADNPointer<ADNSingleStrand> ss);
  static void AddNucleotideToSingleStrandFivePrime(ADNPointer<ADNNucleotide> nt, ADNPointer<ADNSingleStrand> ss);
  static CollectionMap<ADNNucleotide> GetSingleStrandNucleotides(ADNPointer<ADNSingleStrand> ss);
  static ADNPointer<ADNSingleStrand> GetNucleotideStrand(ADNPointer<ADNNucleotide> nt);
  static int ADNMainInterface::GetSingleStrandId(ADNPointer<ADNSingleStrand> ss);
  static ADNPointer<ADNNucleotide> GetFivePrimeFromSingleStrand(ADNPointer<ADNSingleStrand> ss);
  static ADNPointer<ADNNucleotide> GetThreePrimeFromSingleStrand(ADNPointer<ADNSingleStrand> ss);
  static ADNPointer<ADNNucleotide> GetNextNucleotide(ADNPointer<ADNNucleotide> nt);
  static int GetNucleotideId(ADNPointer<ADNNucleotide> nt);
  static void SetNucleotideType(ADNPointer<ADNNucleotide> nt, DNABlocks t);
  static std::string GetSingleStrandSequence(ADNPointer<ADNSingleStrand> ss);
  static void ShiftStartSingleStrand(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt, bool shiftSeq = true);
  static void SetSingleStrandSequence(ADNPointer<ADNSingleStrand> ss, std::string intendedSeq);
  static void CopyAtomsIntoNucleotide(ADNPointer<ADNNucleotide> nt, ADNPointer<ADNNucleotide> nt2);
  static ADNPointer<ADNBasePair> CreateBasePair();
  static void SetLeftNucleotideBasePair(ADNPointer<ADNBasePair> bp, ADNPointer<ADNNucleotide> left);
  static void SetRightNucleotideBasePair(ADNPointer<ADNBasePair> bp, ADNPointer<ADNNucleotide> right);
  static ADNPointer<ADNNucleotide> GetLeftNucleotideBasePair(ADNPointer<ADNBasePair> bp);
  static ADNPointer<ADNNucleotide> GetRightNucleotideBasePair(ADNPointer<ADNBasePair> bp);
  static ADNPointer<ADNPart> LoadPartFromJsonLegacy(ADNPointer<ADNNanorobot> nr, std::string filename);
  static ADNPointer<ADNLoopPair> CreateLoopPair();
  static ADNPointer<ADNLoop> CreateLoop();
  static void SetStartNucleotideInLoop(ADNPointer<ADNLoop> loop, ADNPointer<ADNNucleotide> nt);
  static void SetEndNucleotideInLoop(ADNPointer<ADNLoop> loop, ADNPointer<ADNNucleotide> nt);
  static void AddNucleotideToLoop(ADNPointer<ADNLoop> loop, ADNPointer<ADNNucleotide> nt);
  static void SetLeftLoop(ADNPointer<ADNLoopPair> lp, ADNPointer<ADNLoop> left);
  static ADNPointer<ADNLoop> GetLeftLoop(ADNPointer<ADNLoopPair> lp);
  static ADNPointer<ADNNucleotide> GetStartNucleotideInLoop(ADNPointer<ADNLoop> loop);
  static ADNPointer<ADNNucleotide> GetEndNucleotideInLoop(ADNPointer<ADNLoop> loop);
  static CollectionMap<ADNNucleotide> GetNucleotidesInLoop(ADNPointer<ADNLoop> loop);
  static ADNPointer<ADNBaseSegment> CreateBaseSegment(int id = -1);
  static void AddCellToBaseSegment(ADNPointer<ADNBaseSegment> bs, ADNPointer<ADNCell> cell);
  static int GetBaseSegmentId(ADNPointer<ADNBaseSegment> bs);
  static CellType GetCellType(ADNPointer<ADNBaseSegment> bs);
  static ADNPointer<ADNDoubleStrand> CreateDoubleStrand(int id = -1);
  static void AddBaseSegementToDoubleStrandEnd(ADNPointer<ADNBaseSegment> bs, ADNPointer<ADNDoubleStrand> ds);
  static CollectionMap<ADNBaseSegment> GetBaseSegments(ADNPointer<ADNDoubleStrand> ds);
  static ADNPointer<ADNBaseSegment> GetStartBaseSegment(ADNPointer<ADNDoubleStrand> ds);
  static ADNPointer<ADNBaseSegment> GetEndBaseSegment(ADNPointer<ADNDoubleStrand> ds);
  static ADNPointer<ADNPart> CreatePart(int id = -1);
  static void RegisterSingleStrandInPart(ADNPointer<ADNPart> part, ADNPointer<ADNSingleStrand> ss);
  static void RegisterDoubleStrandInPart(ADNPointer<ADNPart> part, ADNPointer<ADNDoubleStrand> ds);
  static void SetNucleotidePair(ADNPointer<ADNNucleotide> ntOne, ADNPointer<ADNNucleotide> ntTwo);
  static int GetNumberOfDoubleStrands(ADNPointer<ADNPart> part);
  static int GetNumberOfSingleStrands(ADNPointer<ADNPart> part);
  static int GetNumberOfBaseSegments(ADNPointer<ADNPart> part);
  static int GetNumberOfNucleotides(ADNPointer<ADNPart> part);
  static int GetNumberOfAtoms(ADNPointer<ADNPart> part);
  static int GetDoubleStrandId(ADNPointer<ADNDoubleStrand> ds);
  static int CheckSingleStrandLinkedSize(ADNPointer<ADNSingleStrand> ss);
  static int CheckDoubleStrandLinkedSize(ADNPointer<ADNDoubleStrand> ds);
  static ADNPointer<ADNPart> CreateMockPart(int length);
  static ADNPointer<ADNNanorobot> CreateNanorobot();
  static void RegisterPartInNanorobot(ADNPointer<ADNNanorobot> nr, ADNPointer<ADNPart> part);
  static int GetNumberOfParts(ADNPointer<ADNNanorobot> nr);
  static int GetNumberOfDoubleStrands(ADNPointer<ADNNanorobot> nr);
  static int GetNumberOfSingleStrands(ADNPointer<ADNNanorobot> nr);
  static int GetNumberOfBaseSegments(ADNPointer<ADNNanorobot> nr);
  static int GetNumberOfNucleotides(ADNPointer<ADNNanorobot> nr);
  static int GetPartId(ADNPointer<ADNPart> part);
  static CollectionMap<ADNSingleStrand> GetSingleStrandsFromPart(ADNPointer<ADNPart> part);
  static CollectionMap<ADNDoubleStrand> GetDoubleStrandsFromPart(ADNPointer<ADNPart> part);
  static void SavePartToJson(ADNPointer<ADNPart> p, std::string filename);
  static ADNPointer<ADNPart> LoadPartFromJson(ADNPointer<ADNNanorobot> nr, std::string filename);
  static int CheckSequenceEquality(ADNPointer<ADNPart> part, ADNPointer<ADNPart> part2);
  static DNABlocks GetComplementaryNucleotide(DNABlocks t);
  static ADNPointer<ADNNucleotide> GetNucleotidePair(ADNPointer<ADNNucleotide> nt);
  static DNABlocks GetNucleotideType(ADNPointer<ADNNucleotide> nt);
  static CollectionMap<ADNNucleotide> GetNucleotidesInPart(ADNPointer<ADNPart> part);
  static ADNPointer<ADNSingleStrand> ConcatenateSingleStrands(ADNPointer<ADNSingleStrand> ss1, ADNPointer<ADNSingleStrand> ss2);
  static ADNPointer<ADNDoubleStrand> ConcatenateDoubleStrands(ADNPointer<ADNDoubleStrand> ds1, ADNPointer<ADNDoubleStrand> ds2);
  static std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> BreakSingleStrand(ADNPointer<ADNNucleotide> nt);
  static std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> BreakDoubleStrand(ADNPointer<ADNBaseSegment> bs);
  static void MutateNucleotide(ADNPointer<ADNNucleotide> nt, DNABlocks t);
  static void MutateSingleStrandSequence(ADNPointer<ADNSingleStrand> ss, std::string seq, bool changePair = true);
};
