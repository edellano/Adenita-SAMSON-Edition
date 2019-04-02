#include "ADNPart.hpp"


ADNPart::ADNPart(const ADNPart & n) : SBStructuralModel(n) 
{
  *this = n;
}

ADNPart & ADNPart::operator=(const ADNPart& other) 
{
  SBStructuralModel::operator =(other);  

  if (this != &other) {
    
  }

  return *this;
}

void ADNPart::serialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber) const
{
  SBStructuralModel::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  serializer->writeUnsignedIntElement("ntId", nucleotideId_);
  serializer->writeUnsignedIntElement("ssId", singleStrandId_);
  serializer->writeUnsignedIntElement("dsId", doubleStrandId_);

  // atoms
  serializer->writeUnsignedIntElement("numAtoms", atomsIndex_.size());
  serializer->writeStartElement("atoms");
  SB_FOR(ADNPointer<ADNAtom> n, atomsIndex_) {
    unsigned int idx = nodeIndexer.getIndex(n());
    serializer->writeUnsignedIntElement("atomIndex", idx);
  }
  serializer->writeEndElement();
  //end atoms

  // nucleotides
  serializer->writeUnsignedIntElement("numNucleotides", nucleotidesIndex_.size());
  serializer->writeStartElement("nucleotides");
  SB_FOR(ADNPointer<ADNNucleotide> n, nucleotidesIndex_) {
    unsigned int idx = nodeIndexer.getIndex(n());
    serializer->writeUnsignedIntElement("ntIndex", idx);
  }
  serializer->writeEndElement();
  //end nucleotides

  // single strands
  serializer->writeUnsignedIntElement("numSingleStrands", singleStrandsIndex_.size());
  serializer->writeStartElement("singleStrands");
  SB_FOR(ADNPointer<ADNSingleStrand> n, singleStrandsIndex_) {
    unsigned int idx = nodeIndexer.getIndex(n());
    serializer->writeUnsignedIntElement("ssIndex", idx);
  }
  serializer->writeEndElement();
  //end single strands

  // base segments
  serializer->writeUnsignedIntElement("numBaseSegments", baseSegmentsIndex_.size());
  serializer->writeStartElement("baseSegments");
  SB_FOR(ADNPointer<ADNBaseSegment> n, baseSegmentsIndex_) {
    unsigned int idx = nodeIndexer.getIndex(n());
    serializer->writeUnsignedIntElement("bsIndex", idx);
  }
  serializer->writeEndElement();
  //end base segments

  // double strands
  serializer->writeUnsignedIntElement("numDoubleStrands", doubleStrandsIndex_.size());
  serializer->writeStartElement("double strands");
  SB_FOR(ADNPointer<ADNDoubleStrand> n, doubleStrandsIndex_) {
    unsigned int idx = nodeIndexer.getIndex(n());
    serializer->writeUnsignedIntElement("dsIndex", idx);
  }
  serializer->writeEndElement();
  //end double strands
}

void ADNPart::unserialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber)
{
  SBStructuralModel::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  nucleotideId_ = serializer->readUnsignedIntElement();
  singleStrandId_ = serializer->readUnsignedIntElement();
  doubleStrandId_ = serializer->readUnsignedIntElement();

  // atoms index
  unsigned int numAtoms = serializer->readUnsignedIntElement();
  serializer->readStartElement();
  for (unsigned int i = 0; i < numAtoms; ++i) {
    unsigned int idx = serializer->readUnsignedIntElement();
    atomsIndex_.addReferenceTarget((ADNAtom*)nodeIndexer.getNode(idx));
  }
  serializer->readEndElement();

  // nucleotides index
  unsigned int numNucleotides = serializer->readUnsignedIntElement();
  serializer->readStartElement();
  for (unsigned int i = 0; i < numNucleotides; ++i) {
    unsigned int idx = serializer->readUnsignedIntElement();
    nucleotidesIndex_.addReferenceTarget((ADNNucleotide*)nodeIndexer.getNode(idx));
  }
  serializer->readEndElement();

  // single strands index
  unsigned int numSingleStrands = serializer->readUnsignedIntElement();
  serializer->readStartElement();
  for (unsigned int i = 0; i < numSingleStrands; ++i) {
    unsigned int idx = serializer->readUnsignedIntElement();
    singleStrandsIndex_.addReferenceTarget((ADNSingleStrand*)nodeIndexer.getNode(idx));
  }
  serializer->readEndElement();

  // base segments
  unsigned int numBaseSegments = serializer->readUnsignedIntElement();
  serializer->readStartElement();
  for (unsigned int i = 0; i < numBaseSegments; ++i) {
    unsigned int idx = serializer->readUnsignedIntElement();
    baseSegmentsIndex_.addReferenceTarget((ADNBaseSegment*)nodeIndexer.getNode(idx));
  }
  serializer->readEndElement();

  // double strands
  unsigned int numDoubleStrands = serializer->readUnsignedIntElement();
  serializer->readStartElement();
  for (unsigned int i = 0; i < numDoubleStrands; ++i) {
    unsigned int idx = serializer->readUnsignedIntElement();
    doubleStrandsIndex_.addReferenceTarget((ADNDoubleStrand*)nodeIndexer.getNode(idx));
  }
  serializer->readEndElement();

  loadedViaSAMSON(true);
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

int ADNPart::GetNumberOfNucleotides() const
{
  return boost::numeric_cast<int>(GetNucleotides().size());
}

int ADNPart::getNumberOfNucleotides() const
{
  return GetNumberOfNucleotides();
}

int ADNPart::GetNumberOfAtoms() const
{
  return boost::numeric_cast<int>(GetAtoms().size());
}

int ADNPart::getNumberOfAtoms() const
{
  return GetNumberOfAtoms();
}

int ADNPart::GetNumberOfBaseSegments() const
{
  return boost::numeric_cast<int>(GetBaseSegments().size());
}

int ADNPart::getNumberOfBaseSegments() const
{
  return GetNumberOfBaseSegments();
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

int ADNPart::GetNumberOfDoubleStrands() const
{
  return boost::numeric_cast<int>(GetDoubleStrands().size());
}

int ADNPart::getNumberOfDoubleStrands() const
{
  return GetNumberOfDoubleStrands();
}

int ADNPart::GetNumberOfSingleStrands() const
{
  return boost::numeric_cast<int>(GetSingleStrands().size());
}

int ADNPart::getNumberOfSingleStrands() const
{
  return GetNumberOfSingleStrands();
}

void ADNPart::DeregisterSingleStrand(ADNPointer<ADNSingleStrand> ss, bool removeFromParent, bool removeFromIndex) 
{
  if (removeFromParent) {
    auto root = getStructuralRoot();
    root->removeChild(ss());
  }

  if (removeFromIndex) singleStrandsIndex_.removeReferenceTarget(ss());
}

void ADNPart::DeregisterNucleotide(ADNPointer<ADNNucleotide> nt, bool removeFromSs, bool removeFromBs, bool removeFromIndex)
{
  if (removeFromSs) {
    ADNPointer<ADNSingleStrand> ss = nt->GetStrand();
    ss->removeChild(nt());
  }
  if (removeFromBs) {
    auto bs = nt->GetBaseSegment();
    bs->RemoveNucleotide(nt);
  }
  
  if (removeFromIndex) nucleotidesIndex_.removeReferenceTarget(nt());
}

void ADNPart::DeregisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds, bool removeFromParent, bool removeFromIndex)
{
  if (removeFromParent) {
    auto root = getStructuralRoot();
    root->removeChild(ds());
  }

  if (removeFromIndex) doubleStrandsIndex_.removeReferenceTarget(ds());
}

void ADNPart::DeregisterBaseSegment(ADNPointer<ADNBaseSegment> bs, bool removeFromDs, bool removeFromIndex)
{
  if (removeFromDs) bs->getParent()->removeChild(bs());
  if (removeFromIndex) baseSegmentsIndex_.removeReferenceTarget(bs());
}

void ADNPart::DeregisterAtom(ADNPointer<ADNAtom> atom, bool removeFromAtom)
{
  if (removeFromAtom) atom->getParent()->removeChild(atom());
  atomsIndex_.removeReferenceTarget(atom());
}

bool ADNPart::loadedViaSAMSON()
{
  return loadedViaSAMSON_;
}

void ADNPart::loadedViaSAMSON(bool l)
{
  loadedViaSAMSON_ = l;
}

void ADNPart::RegisterSingleStrand(ADNPointer<ADNSingleStrand> ss) 
{
  if (ss->getName().empty()) {
    ss->setName("Single Strand " + std::to_string(singleStrandId_));
    ++singleStrandId_;
  }

  auto root = getStructuralRoot();
  root->addChild(ss());

  singleStrandsIndex_.addReferenceTarget(ss());
}

void ADNPart::RegisterNucleotideThreePrime(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt, bool addToSs)
{
  if (nt->getName().empty()) {
    nt->setName(ADNModel::GetResidueName(nt->GetType()) + " " + std::to_string(nucleotideId_));
    ++nucleotideId_;
  }
  if (addToSs) ss->AddNucleotideThreePrime(nt);

  nucleotidesIndex_.addReferenceTarget(nt());
}

void ADNPart::RegisterNucleotideFivePrime(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt, bool addToSs)
{
  if (nt->getName().empty()) {
    nt->setName(ADNModel::GetResidueName(nt->GetType()) + " " + std::to_string(nucleotideId_));
    ++nucleotideId_;
  }

  if (addToSs) ss->AddNucleotideFivePrime(nt);

  nucleotidesIndex_.addReferenceTarget(nt());
}

void ADNPart::RegisterNucleotide(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt, 
  ADNPointer<ADNNucleotide> ntNext, bool addToSs)
{
  if (nt->getName().empty()) {
    nt->setName(ADNModel::GetResidueName(nt->GetType()) + " " + std::to_string(nucleotideId_));
    ++nucleotideId_;
  }

  if (addToSs) ss->AddNucleotide(nt, ntNext);

  nucleotidesIndex_.addReferenceTarget(nt());
}

void ADNPart::RegisterAtom(ADNPointer<ADNNucleotide> nt, NucleotideGroup g, ADNPointer<ADNAtom> at, bool create)
{
  if (create) {
    at->create();
  }

  nt->AddAtom(g, at);

  atomsIndex_.addReferenceTarget(at());
}

void ADNPart::RegisterAtom(ADNPointer<ADNBaseSegment> bs, ADNPointer<ADNAtom> at, bool create)
{
  if (create) {
    at->create();
  }

  bs->addChild(at());

  atomsIndex_.addReferenceTarget(at());
}

void ADNPart::RegisterBaseSegmentEnd(ADNPointer<ADNDoubleStrand> ds, ADNPointer<ADNBaseSegment> bs, bool addToDs)
{
  if (addToDs) ds->AddBaseSegmentEnd(bs);

  baseSegmentsIndex_.addReferenceTarget(bs());
}

unsigned int ADNPart::GetBaseSegmentIndex(ADNPointer<ADNBaseSegment> bs)
{
  return baseSegmentsIndex_.getIndex(bs());
}

void ADNPart::RegisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds) 
{
  if (ds->getName().empty()) {
    ds->setName("Double Strand " + std::to_string(doubleStrandId_));
    ++doubleStrandId_;
  }
  auto root = getStructuralRoot();
  root->addChild(ds());

  doubleStrandsIndex_.addReferenceTarget(ds());
}