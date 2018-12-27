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

void ADNPart::serialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber) const
{
  //SBStructuralModel::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  //serializer->writeIntElement("number_ss", GetNumberOfSingleStrands());

  //serializer->writeStartElement("single_strands");
  //SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrandsIndex_) {
  //  serializer->writeUnsignedIntElement("ss_idx", nodeIndexer.getIndex(ss()));
  //  serializer->writeStringElement("name", ss->GetName());
  //  serializer->writeBoolElement("isScaffold", ss->IsScaffold());
  //  serializer->writeBoolElement("isCircular", ss->IsCircular());
  //  serializer->writeIntElement("num_nts", ss->getNumberOfNucleotides());

  //  ADNPointer<ADNNucleotide> nt = ss->GetFivePrime();

  //  serializer->writeStartElement("nucleotides");
  //  while (nt != nullptr) {
  //    serializer->writeUnsignedIntElement("nt_idx", nodeIndexer.getIndex(nt()));
  //    serializer->writeStartElement("nt_data");

  //    serializer->writeIntElement("type", nt->GetType());
  //    serializer->writeStringElement("pos", ADNAuxiliary::SBPositionToString(nt->GetPosition()));
  //    serializer->writeStringElement("bbPos", ADNAuxiliary::SBPositionToString(nt->GetBackbonePosition()));
  //    serializer->writeStringElement("scPos", ADNAuxiliary::SBPositionToString(nt->GetSidechainPosition()));
  //    serializer->writeStringElement("e1", ADNAuxiliary::UblasVectorToString(nt->GetE1()));
  //    serializer->writeStringElement("e2", ADNAuxiliary::UblasVectorToString(nt->GetE2()));
  //    serializer->writeStringElement("e3", ADNAuxiliary::UblasVectorToString(nt->GetE3()));

  //    serializer->writeEndElement();
  //    nt = nt->GetNext();
  //  }
  //  serializer->writeEndElement();
  //}
  //serializer->writeEndElement();
}

void ADNPart::unserialize(SBCSerializer * serializer, const SBNodeIndexer & nodeIndexer, const SBVersionNumber & sdkVersionNumber, const SBVersionNumber & classVersionNumber)
{
  //SBStructuralModel::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

  //int numSss = serializer->readIntElement();

  //serializer->readStartElement();  // begin single strands

  //// build bottom-up
  //for (int i = 0; i < numSss; ++i) {
  //  unsigned int id = serializer->readUnsignedIntElement();
  //  std::string name = serializer->readStringElement();
  //  bool isScaf = serializer->readBoolElement();
  //  bool isCirc = serializer->readBoolElement();
  //  int numNt = serializer->readIntElement();

  //  ADNPointer<ADNSingleStrand> ss = new ADNSingleStrand();
  //  RegisterSingleStrand(ss);
  //  ss->SetName(name);
  //  ss->IsScaffold(isScaf);
  //  ss->IsCircular(isCirc);

  //  serializer->readStartElement();  // begin nucleotides

  //  for (int j = 0; j < numNt; ++j) {
  //    unsigned int nt_id = serializer->readUnsignedIntElement();
  //    ADNPointer<ADNNucleotide> nt = new ADNNucleotide();

  //    serializer->readStartElement();  // nucleotide data

  //    DNABlocks t = DNABlocks(serializer->readIntElement());
  //    SBPosition3 pos = ADNAuxiliary::StringToSBPosition(serializer->readStringElement());
  //    SBPosition3 bbPos = ADNAuxiliary::StringToSBPosition(serializer->readStringElement());
  //    SBPosition3 scPos = ADNAuxiliary::StringToSBPosition(serializer->readStringElement());
  //    auto e1 = ADNAuxiliary::StringToUblasVector(serializer->readStringElement());
  //    auto e2 = ADNAuxiliary::StringToUblasVector(serializer->readStringElement());
  //    auto e3 = ADNAuxiliary::StringToUblasVector(serializer->readStringElement());

  //    nt->SetType(t);
  //    nt->SetPosition(pos);
  //    nt->SetBackbonePosition(bbPos);
  //    nt->SetSidechainPosition(scPos);
  //    nt->SetE1(e1);
  //    nt->SetE2(e2);
  //    nt->SetE3(e3);

  //    RegisterNucleotideThreePrime(ss, nt);

  //    serializer->readEndElement();  // end nucleotide data
  //  }

  //  serializer->readEndElement();  // end nucleotides
  //}

  //serializer->readEndElement();  // end single strands

  //// build top-down

  //loadedViaSAMSON(true);
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

void ADNPart::DeregisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds)
{
  auto root = getStructuralRoot();
  root->removeChild(ds());

  doubleStrandsIndex_.removeReferenceTarget(ds());
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
  auto root = getStructuralRoot();
  root->addChild(ss());

  singleStrandsIndex_.addReferenceTarget(ss());
}

void ADNPart::RegisterNucleotideThreePrime(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt, bool addToSs)
{
  if (addToSs) ss->AddNucleotideThreePrime(nt);

  nucleotidesIndex_.addReferenceTarget(nt());
}

void ADNPart::RegisterNucleotideFivePrime(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt, bool addToSs)
{
  if (addToSs) ss->AddNucleotideFivePrime(nt);

  nucleotidesIndex_.addReferenceTarget(nt());
}

void ADNPart::RegisterNucleotide(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt, 
  ADNPointer<ADNNucleotide> ntNext, bool addToSs)
{
  if (addToSs) ss->AddNucleotide(nt, ntNext);

  nucleotidesIndex_.addReferenceTarget(nt());
}

void ADNPart::RegisterAtom(ADNPointer<ADNNucleotide> nt, NucleotideGroup g, ADNPointer<ADNAtom> at, bool create)
{
  nt->AddAtom(g, at);

  atomsIndex_.addReferenceTarget(at());

  if (create) {
    at->create();
  }
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
  ds->setName("Double Strand " + std::to_string(ds->getNodeIndex()));
  auto root = getStructuralRoot();
  root->addChild(ds());

  doubleStrandsIndex_.addReferenceTarget(ds());
}