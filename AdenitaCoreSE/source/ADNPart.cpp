#include "ADNPart.hpp"


ADNPart::ADNPart(const ADNPart & n) : Orientable(n), Identifiable(n), Positionable(n), Collection<ADNDoubleStrand>(n), Collection<ADNSingleStrand>(n), Nameable(n) 
{
  *this = n;
}

ADNPart & ADNPart::operator=(const ADNPart& other) 
{

  Orientable::operator =(other);
  Identifiable::operator =(other);
  Positionable::operator =(other);
  Collection<ADNDoubleStrand>::operator =(other);
  Collection<ADNSingleStrand>::operator =(other);
  Nameable::operator =(other);
  

  if (this != &other) {
    
  }
  return *this;
}

ADNPointer<ADNDoubleStrand> ADNPart::GetDoubleStrand(int id)
{
  return Collection<ADNDoubleStrand>::GetElement(id);
}

CollectionMap<ADNBaseSegment> ADNPart::GetBaseSegments(CellType celltype) const 
{
  CollectionMap<ADNBaseSegment> bases;

  auto doublestrands = Collection<ADNDoubleStrand>::GetCollection();
  int num = 0;
  for (auto & ds : doublestrands) {
    auto bs = ds.second->GetBaseSegments();
    bases.insert(bs.begin(), bs.end());
  }

  if (celltype == ALL) return bases;

  for (auto it = bases.begin(); it != bases.end(); ++ it) {
    ADNPointer<ADNBaseSegment> bs = it->second;
    ADNPointer<ADNCell> c = bs->GetCell();
    if (c->GetType() != celltype) bases.erase(it);
  }

  return bases;
}

CollectionMap<ADNSingleStrand> ADNPart::GetScaffolds() const 
{
  CollectionMap<ADNSingleStrand> scaffolds;
  for (auto &sit : GetSingleStrands()) {
    if (sit.second->IsScaffold()) scaffolds.insert(sit);
  }
  return scaffolds;
}

CollectionMap<ADNNucleotide> ADNPart::GetNucleotides(CellType celltype) const
{
  CollectionMap<ADNNucleotide> nucleotides;
  auto singleStrands = GetSingleStrands();

  for (auto s : singleStrands) {
    auto nts = s.second->GetNucleotides();
    nucleotides.insert(nts.begin(), nts.end());
  }

  if (celltype != CellType::ALL) {
    for (auto it = nucleotides.begin(); it != nucleotides.end(); ) {
      auto bs = it->second->GetBaseSegment();
      if (bs->GetCellType() != celltype) {
        nucleotides.erase(it);
      }
      else {
        it++;
      }
    }
  }

  return nucleotides;
}

//SBVector3 ADNPart::GetPolygonNorm(ANTPolygon* face) {
//  ANTHalfEdge* he = face->halfEdge_;
//  ANTVertex* v1 = he->prev_->source_;
//  ANTVertex* v2 = he->source_;
//  ANTVertex* v3 = he->next_->source_;
//  // todo:
//  // this will only work if face doesn't have consecutive vertex along a line
//  ublas::vector<double> p1 = ANTVectorMath::CreateBoostVector(v1->GetVectorCoordinates());
//  ublas::vector<double> p2 = ANTVectorMath::CreateBoostVector(v2->GetVectorCoordinates());
//  ublas::vector<double> p3 = ANTVectorMath::CreateBoostVector(v3->GetVectorCoordinates());
//  //ublas::vector<double> p3 = ublas::vector<double>(3);
//  /*double proj;
//  auto n_he = he->next_->next_;
//  do {
//  v3 = n_he->source_->GetVectorCoordinates();
//  p3 = ANTVectorMath::CreateBoostVector(v3);
//  auto test = he->next_->source_->GetSBPosition() - he->source_->GetSBPosition();
//  auto test2 = n_he->source_->GetSBPosition() - he->next_->source_->GetSBPosition();
//  proj = ublas::inner_prod(p2 - p1, p3 - p2) / (ublas::norm_2(p2-p1)*ublas::norm_2(p3-p2));
//  n_he = n_he->next_;
//  } while (abs(abs(proj)-1.0) <= ANTConstants::EPSILON);*/
//
//  ublas::vector<double> crs = ANTVectorMath::CrossProduct(p2 - p1, p3 - p2);
//  crs /= ublas::norm_2(crs);
//  std::vector<double> crss = ANTVectorMath::CreateStdVector(crs);
//
//  SBVector3 w = SBVector3();
//  w[0] = crss[0];
//  w[1] = crss[1];
//  w[2] = crss[2];
//  return w;
//}

int ADNPart::GetNumberOfNucleotides() 
{
  auto singlestrands = Collection<ADNSingleStrand>::GetCollection();
  int num = 0;
  for (auto & strand : singlestrands) {
    auto nucleotides = strand.second->GetNucleotides();
    num += boost::numeric_cast<int>(nucleotides.size());
  }

  return num;
}

int ADNPart::GetNumberOfAtoms() 
{
  auto singlestrands = Collection<ADNSingleStrand>::GetCollection();
  int num = 0;
  for (auto & strand : singlestrands) {
    auto nucleotides = strand.second->GetNucleotides();
    for (auto nt : nucleotides) {
      auto atoms = nt.second->GetAtoms();
      num += boost::numeric_cast<int>(atoms.size());
    }
  }

  return num;
}

int ADNPart::GetNumberOfBaseSegments() 
{
  auto doublestrands = Collection<ADNDoubleStrand>::GetCollection();
  int num = 0;
  for (auto & ds : doublestrands) {
    auto bases = ds.second->GetBaseSegments();
    num += boost::numeric_cast<int>(bases.size());
  }

  return num;
}

CollectionMap<ADNSingleStrand> ADNPart::GetSingleStrands() const
{
  return Collection<ADNSingleStrand>::GetCollection();
}

CollectionMap<ADNDoubleStrand> ADNPart::GetDoubleStrands() const
{
  return Collection<ADNDoubleStrand>::GetCollection();
}

int ADNPart::GetLastSingleStrandId() 
{
  return Collection<ADNSingleStrand>::GetLastKey();
}

int ADNPart::GetLastDoubleStrandId() 
{
  return Collection<ADNDoubleStrand>::GetLastKey();
}

int ADNPart::GetNumberOfDoubleStrands()
{
  return boost::numeric_cast<int>(Collection<ADNDoubleStrand>::GetCollection().size());
}

int ADNPart::GetNumberOfSingleStrands()
{
  return boost::numeric_cast<int>(Collection<ADNSingleStrand>::GetCollection().size());
}

void ADNPart::DeregisterSingleStrand(ADNPointer<ADNSingleStrand> ss) 
{
  Collection<ADNSingleStrand>::DeleteElement(ss->GetId());
}

void ADNPart::DeregisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds)
{
  Collection<ADNDoubleStrand>::DeleteElement(ds->GetId());
}

void ADNPart::RegisterSingleStrand(ADNPointer<ADNSingleStrand> ss) 
{
  if (ss->GetId() < 0) {
    ss->SetId(GetLastSingleStrandId() + 1);
  }
  Collection<ADNSingleStrand>::AddElement(ss, ss->GetId());
}

void ADNPart::RegisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds) 
{
  if (ds->GetId() < 0) {
    ds->SetId(GetLastDoubleStrandId() + 1);
  }
  Collection<ADNDoubleStrand>::AddElement(ds, ds->GetId());
}

ADNPointer<ADNSingleStrand> ADNPart::GetSingleStrand(int c_id)
{
  return Collection<ADNSingleStrand>::GetElement(c_id);
}