#include "ADNPart.hpp"


ADNPart::ADNPart(const ADNPart & n) : Orientable(n), PositionableSB(n), SBStructuralModel(n) 
{
  *this = n;
}

ADNPart & ADNPart::operator=(const ADNPart& other) 
{
  Orientable::operator =(other);
  PositionableSB::operator =(other);
  SBStructuralModel::operator =(other);  

  if (this != &other) {
    
  }

  return *this;
}

std::string const & ADNPart::GetName() const
{
  return getName();
}

void ADNPart::SetName(const std::string & name)
{
  setName(name);
}

CollectionMap<ADNBaseSegment> ADNPart::GetBaseSegments(CellType celltype) const 
{
  CollectionMap<ADNBaseSegment> bsList;
  if (celltype == ALL) {
    bsList = baseSegmentsIndex_;
  }
  else {
    SB_FOR(ADNPointer<ADNBaseSegment> bs, baseSegmentsIndex_) {
      if (bs->GetCellType() == celltype) {
        bsList.addReferenceTarget(bs());
      }
    }
  }

  return bsList;
}

CollectionMap<ADNSingleStrand> ADNPart::GetScaffolds() const 
{
  CollectionMap<ADNSingleStrand> chainList;

  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrandsIndex_) {
    if (ss->IsScaffold()) {
      chainList.addReferenceTarget(ss());
    }
  }

  return chainList;
}

CollectionMap<ADNNucleotide> ADNPart::GetNucleotides(CellType celltype) const
{
  return nucleotidesIndex_;
}

int ADNPart::GetNumberOfNucleotides() 
{
  return boost::numeric_cast<int>(GetNucleotides().size());
}

int ADNPart::GetNumberOfAtoms() 
{
  return boost::numeric_cast<int>(GetAtoms().size());
}

int ADNPart::GetNumberOfBaseSegments() 
{
  return boost::numeric_cast<int>(GetBaseSegments().size());
}

CollectionMap<ADNSingleStrand> ADNPart::GetSingleStrands() const
{
  return singleStrandsIndex_;
}

CollectionMap<ADNDoubleStrand> ADNPart::GetDoubleStrands() const
{
  return doubleStrandsIndex_;
}

CollectionMap<ADNAtom> ADNPart::GetAtoms() const
{
  return atomsIndex_;
}

int ADNPart::GetNumberOfDoubleStrands()
{
  return boost::numeric_cast<int>(GetDoubleStrands().size());
}

int ADNPart::GetNumberOfSingleStrands()
{
  return boost::numeric_cast<int>(GetSingleStrands().size());
}

void ADNPart::DeregisterSingleStrand(ADNPointer<ADNSingleStrand> ss) 
{
  auto root = getStructuralRoot();
  root->removeChild(ss());

  singleStrandsIndex_.removeReferenceTarget(ss());
}

void ADNPart::DeregisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds)
{
  auto root = getStructuralRoot();
  root->removeChild(ds());

  doubleStrandsIndex_.removeReferenceTarget(ds());
}

void ADNPart::RegisterSingleStrand(ADNPointer<ADNSingleStrand> ss) 
{
  auto root = getStructuralRoot();
  root->addChild(ss());

  singleStrandsIndex_.addReferenceTarget(ss());
}

void ADNPart::RegisterNucleotideThreePrime(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt)
{
  ss->AddNucleotideThreePrime(nt);

  nucleotidesIndex_.addReferenceTarget(nt());
}

void ADNPart::RegisterNucleotideFivePrime(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt)
{
  ss->AddNucleotideFivePrime(nt);

  nucleotidesIndex_.addReferenceTarget(nt());
}

void ADNPart::RegisterAtom(ADNPointer<ADNNucleotide> nt, NucleotideGroup g, ADNPointer<ADNAtom> at)
{
  nt->AddAtom(g, at);

  atomsIndex_.addReferenceTarget(at());
}

void ADNPart::RegisterBaseSegmentEnd(ADNPointer<ADNDoubleStrand> ds, ADNPointer<ADNBaseSegment> bs)
{
  ds->AddBaseSegmentEnd(bs);

  baseSegmentsIndex_.addReferenceTarget(bs());
}

unsigned int ADNPart::GetBaseSegmentIndex(ADNPointer<ADNBaseSegment> bs)
{
  return baseSegmentsIndex_.getIndex(bs());
}

void ADNPart::RegisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds) 
{
  ds->setName("Double Strand " + std::to_string(ds->getNodeIndex()));
  auto root = getStructuralRoot();
  root->addChild(ds());

  doubleStrandsIndex_.addReferenceTarget(ds());
}