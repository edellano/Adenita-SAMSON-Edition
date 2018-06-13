#include "ADNPart.hpp"


ADNPart::ADNPart(const ADNPart & n) : Orientable(n), Identifiable(n), PositionableSB(n), SBStructuralModel(n) 
{
  *this = n;
}

ADNPart & ADNPart::operator=(const ADNPart& other) 
{
  Orientable::operator =(other);
  Identifiable::operator =(other);
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

ADNPointer<ADNDoubleStrand> ADNPart::GetDoubleStrand(int id)
{
  auto dsList = GetDoubleStrands();
  return dsList[id];
}

CollectionMap<ADNBaseSegment> ADNPart::GetBaseSegments(CellType celltype) const 
{
  CollectionMap<ADNBaseSegment> bsList;

  SBMStructuralModelNodeRoot* root = getStructuralRoot();
  SBNodeIndexer nodeIndexer;
  root->getNodes(nodeIndexer, SBNode::IsType(SBNode::StructuralModelNode));

  SB_FOR(SBNode* n, nodeIndexer) {
    ADNBaseSegment* a = static_cast<ADNBaseSegment*>(n);
    if (a->GetCellType() == celltype) {
      bsList.addReferenceTarget(a);
    }
  }

  return bsList;
}

CollectionMap<ADNSingleStrand> ADNPart::GetScaffolds() const 
{
  CollectionMap<ADNSingleStrand> chainList;

  SBMStructuralModelNodeRoot* root = getStructuralRoot();
  SBNodeIndexer nodeIndexer;
  root->getNodes(nodeIndexer, SBNode::IsType(SBNode::Chain));

  SB_FOR(SBNode* n, nodeIndexer) {
    ADNSingleStrand* a = static_cast<ADNSingleStrand*>(n);
    if (a->IsScaffold()) {
      chainList.addReferenceTarget(a);
    }
  }

  return chainList;
}

CollectionMap<ADNNucleotide> ADNPart::GetNucleotides(CellType celltype) const
{
  CollectionMap<ADNNucleotide> nucleotides;

  SBMStructuralModelNodeRoot* root = getStructuralRoot();
  SBNodeIndexer nodeIndexer;
  root->getNodes(nodeIndexer, SBNode::IsType(SBNode::Chain));

  SB_FOR(SBNode* n, nodeIndexer) {
    ADNPointer<ADNNucleotide> a = static_cast<ADNNucleotide*>(n);
    ADNPointer<ADNBaseSegment> bs = a->GetBaseSegment();
    if (bs->GetCellType() == celltype) {
      nucleotides.addReferenceTarget(a());
    }
  }

  return nucleotides;
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
  CollectionMap<ADNSingleStrand> chainList;

  SBMStructuralModelNodeRoot* root = getStructuralRoot();
  SBNodeIndexer nodeIndexer;
  root->getNodes(nodeIndexer, SBNode::IsType(SBNode::Chain));

  SB_FOR(SBNode* n, nodeIndexer) {
    ADNSingleStrand* a = static_cast<ADNSingleStrand*>(n);
    chainList.addReferenceTarget(a);
  }

  return chainList;
}

CollectionMap<ADNDoubleStrand> ADNPart::GetDoubleStrands() const
{
  // todo: this will also return base segments since they are also subclassed from Structural Groups
  CollectionMap<ADNDoubleStrand> dsList;

  SBMStructuralModelNodeRoot* root = getStructuralRoot();
  SBNodeIndexer nodeIndexer;
  root->getNodes(nodeIndexer, SBNode::IsType(SBNode::StructuralGroup));

  SB_FOR(SBNode* n, nodeIndexer) {
    ADNDoubleStrand* a = static_cast<ADNDoubleStrand*>(n);
    dsList.addReferenceTarget(a);
  }

  return dsList;
}

CollectionMap<ADNAtom> ADNPart::GetAtoms() const
{
  CollectionMap<ADNAtom> aList;

  SBMStructuralModelNodeRoot* root = getStructuralRoot();
  SBNodeIndexer nodeIndexer;
  root->getNodes(nodeIndexer, SBNode::IsType(SBNode::Atom));

  SB_FOR(SBNode* n, nodeIndexer) {
    ADNAtom* a = static_cast<ADNAtom*>(n);
    aList.addReferenceTarget(a);
  }

  return aList;
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
}

void ADNPart::DeregisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds)
{
  auto root = getStructuralRoot();
  root->removeChild(ds());
}

void ADNPart::RegisterSingleStrand(ADNPointer<ADNSingleStrand> ss) 
{
  auto root = getStructuralRoot();
  root->addChild(ss());
}

void ADNPart::RegisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds) 
{
  auto root = getStructuralRoot();
  root->addChild(ds());
}

ADNPointer<ADNSingleStrand> ADNPart::GetSingleStrand(int c_id)
{
  auto ssList = GetSingleStrands();
  return ssList[c_id];
}