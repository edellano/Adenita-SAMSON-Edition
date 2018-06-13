#pragma once

#include "ADNConstants.hpp"
#include "ADNModel.hpp"


using BaseSegments = std::map<int, ADNBaseSegment*>;
using NtSegments = std::map<ADNNucleotide*, ADNBaseSegment*>;
using NTMap = boost::bimap<unsigned int, ADNNucleotide*>;

class ADNPart : public Orientable, public Identifiable, public Positionable, public Collection<ADNDoubleStrand>, public Collection<ADNSingleStrand>, public Nameable {
public:
  ADNPart() : Orientable(), Identifiable(), Positionable(), Collection<ADNDoubleStrand>(), Collection<ADNSingleStrand>(), Nameable() {};
  ADNPart(const ADNPart &n);
  ~ADNPart() = default;

  ADNPart& operator=(const ADNPart& other);

  ADNPointer<ADNDoubleStrand> GetDoubleStrand(int id);
  void RegisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds);
  ADNPointer<ADNSingleStrand> GetSingleStrand(int c_id);
  void RegisterSingleStrand(ADNPointer<ADNSingleStrand> ss);

  CollectionMap<ADNSingleStrand> GetSingleStrands() const;
  CollectionMap<ADNDoubleStrand> GetDoubleStrands() const;
  int GetLastSingleStrandId();
  int GetLastDoubleStrandId();
  int GetNumberOfDoubleStrands();
  int GetNumberOfSingleStrands();
  int GetNumberOfNucleotides();
  int GetNumberOfAtoms();
  int GetNumberOfBaseSegments();
  CollectionMap<ADNBaseSegment> GetBaseSegments(CellType celltype = ALL) const;
  CollectionMap<ADNSingleStrand> GetScaffolds() const;
  CollectionMap<ADNNucleotide> GetNucleotides(CellType celltype = ALL) const;

  void DeregisterSingleStrand(ADNPointer<ADNSingleStrand> ss);
  void DeregisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds);

  /*SBPosition3 GetPosition() const;
  ANTPolyhedron& GetPolyhedron();
  const ANTPolyhedron& GetPolyhedron() const;
  Vertices GetVertices();
  Vertices GetOriginalVertices() const;
  Edges GetEdges() const;
  Faces GetFaces() const;*/
protected:
private:
};