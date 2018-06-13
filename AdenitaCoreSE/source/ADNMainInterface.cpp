#include "ADNMainInterface.hpp"

SEConfig& ADNMainInterface::CreateSEConfig() {
  return SEConfig::GetInstance();
}

ADNArray<int> ADNMainInterface::CreateADNArray(size_t dim, size_t num_elem)
{
  return ADNArray<int>(dim, num_elem);
}

void ADNMainInterface::CreateLoggingFile()
{
  ADNLogger& logger = ADNLogger::GetLogger();
  logger.ClearLog();
  std::string msg = "test";
  logger.Log(msg);
  logger.Log(QString("test"));
  ublas::vector<double> v(3, 0.0);
  logger.LogVector(v);
  logger.LogVector("name", v);
  ublas::matrix<double> m(3, 3, 0.0);
  logger.LogMatrix(m);
  time_t time1;
  time(&time1);
  logger.LogPassedTime(time1, "text");
  int k = 0;
  quintptr ptr(k);
  logger.LogPtr(ptr);
}

ublas::vector<double> ADNMainInterface::CreateUblasVector(std::vector<double> v)
{
  return ADNVectorMath::CreateBoostVector(v);
}

ublas::vector<double> ADNMainInterface::CrossProduct(ublas::vector<double> v, ublas::vector<double> w)
{
  return ADNVectorMath::CrossProduct(v, w);
}

ublas::vector<double> ADNMainInterface::DirectionVector(ublas::vector<double> p, ublas::vector<double> q)
{
  return ADNVectorMath::DirectionVector(p, q);
}

ublas::matrix<double> ADNMainInterface::CreateBoostMatrix(std::vector<std::vector<double>> vecovec)
{
  return ADNVectorMath::CreateBoostMatrix(vecovec);
}

ublas::vector<double> ADNMainInterface::CalculateCM(ublas::matrix<double> positions)
{
  return ADNVectorMath::CalculateCM(positions);
}

double ADNMainInterface::DegToRad(double degree)
{
  return ADNVectorMath::DegToRad(degree);
}

ublas::matrix<double> ADNMainInterface::MakeRotationMatrix(ublas::vector<double> dir, double angle)
{
  return ADNVectorMath::MakeRotationMatrix(dir, angle);
}

ublas::matrix<double> ADNMainInterface::ApplyTransformation(ublas::matrix<double> t_mat, ublas::matrix<double> points)
{
  return ADNVectorMath::ApplyTransformation(t_mat, points);
}

ublas::matrix<double> ADNMainInterface::Translate(ublas::matrix<double> input, ublas::vector<double> t_vector)
{
  return ADNVectorMath::Translate(input, t_vector);
}

ublas::matrix<double> ADNMainInterface::CenterSystem(ublas::matrix<double> input)
{
  return ADNVectorMath::CenterSystem(input);
}

double ADNMainInterface::Determinant(ublas::matrix<double> mat)
{
  return ADNVectorMath::Determinant(mat);
}

ublas::vector<double> ADNMainInterface::CalculatePlane(ublas::matrix<double> mat)
{
  return ADNVectorMath::CalculatePlane(mat);
}

ublas::matrix<double> ADNMainInterface::FindOrthogonalSubspace(ublas::vector<double> z)
{
  return ADNVectorMath::FindOrthogonalSubspace(z);
}

ADNPointer<ADNAtom> ADNMainInterface::CreateAtom(ADNPointer<ADNAtom> other)
{
  return ADNPointer<ADNAtom>(new ADNAtom(*other.get()));
}

ADNPointer<ADNAtom> ADNMainInterface::CreateAtom(int id)
{
  ADNPointer<ADNAtom> atom(new ADNAtom());
  atom->SetId(id);
  atom->SetName("test");
  return atom;
}

ADNPointer<ADNAtom> ADNMainInterface::CreateAtom(int id, std::string name, ublas::vector<double> pos)
{
  ADNPointer<ADNAtom> a(new ADNAtom());
  a->SetId(id);
  a->SetName(name);
  a->SetPosition(pos);
  return a;
}

std::string ADNMainInterface::GetAtomName(ADNPointer<ADNAtom> atom)
{
  return atom->GetName();
}

void ADNMainInterface::SetAtomPosition(ADNPointer<ADNAtom> atom, ublas::vector<double> pos)
{
  atom->SetPosition(pos);
}

ublas::vector<double> ADNMainInterface::GetAtomPosition(ADNPointer<ADNAtom> atom)
{
  return atom->GetPosition();
}

void ADNMainInterface::SetAtomId(ADNPointer<ADNAtom> atom, int id)
{
  atom->SetId(id);
}

int ADNMainInterface::GetAtomId(ADNPointer<ADNAtom> atom)
{
  return atom->GetId();
}

ADNPointer<ADNSidechain> ADNMainInterface::CreateSidechain()
{
  return ADNPointer<ADNSidechain>(new ADNSidechain());
}

NucleotideGroup ADNMainInterface::GetGroupType(ADNPointer<ADNNucleotideGroup> ng)
{
  return ng->GetGroupType();
}

void ADNMainInterface::AddAtomToGroup(ADNPointer<ADNNucleotideGroup> g, ADNPointer<ADNAtom> a)
{
  g->AddAtom(a);
  a->SetNucleotideGroup(g);
}

bool ADNMainInterface::IsAtomInBackbone(ADNPointer<ADNAtom> a)
{
  return a->IsInBackbone();
}

CollectionMap<ADNAtom> ADNMainInterface::GetAtomsInGroup(ADNPointer<ADNNucleotideGroup> g)
{
  return g->GetAtoms();
}

ADNPointer<ADNNucleotide> ADNMainInterface::CreateADNNucleotide(int id)
{
  auto nt = ADNPointer<ADNNucleotide>(new ADNNucleotide());
  nt->SetId(id);
  return nt;
}

ADNPointer<ADNBackbone> ADNMainInterface::GetNucleotideBackbone(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetBackbone();
}

ADNPointer<ADNSidechain> ADNMainInterface::GetNucleotideSidechain(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetSidechain();
}

void ADNMainInterface::AddAtomToNucleotide(ADNPointer<ADNNucleotide> nt, ADNPointer<ADNNucleotideGroup> g, ADNPointer<ADNAtom> a)
{
  nt->AddAtom(g, a);
}

CollectionMap<ADNAtom> ADNMainInterface::GetNucleotideAtoms(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetAtoms();
}

void ADNMainInterface::DeleteNucleotideAtom(ADNPointer<ADNNucleotide> nt, int id)
{
  nt->DeleteAtom(nt->GetAtom(id));
}

void ADNMainInterface::AddConnectionToAtom(ADNPointer<ADNNucleotide> nt, int id1, int id2)
{
  nt->AddBond(nt->GetAtom(id1), nt->GetAtom(id2));
}

std::multimap<ADNPointer<ADNAtom>, ADNPointer<ADNAtom>> ADNMainInterface::GetNucleotideBonds(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetBonds();
}

ADNPointer<ADNSingleStrand> ADNMainInterface::CreateSingleStrand(int id)
{
  ADNPointer<ADNSingleStrand> ss(new ADNSingleStrand());
  ss->SetId(id);
  ss->SetName("test");
  ss->IsScaffold(true);
  return ss;
}

void ADNMainInterface::AddNucleotideToSingleStrandThreePrime(ADNPointer<ADNNucleotide> nt, ADNPointer<ADNSingleStrand> ss)
{
  ss->AddNucleotideThreePrime(nt);
}

void ADNMainInterface::AddNucleotideToSingleStrandFivePrime(ADNPointer<ADNNucleotide> nt, ADNPointer<ADNSingleStrand> ss)
{
  ss->AddNucleotideFivePrime(nt);
}

CollectionMap<ADNNucleotide> ADNMainInterface::GetSingleStrandNucleotides(ADNPointer<ADNSingleStrand> ss)
{
  return ss->GetNucleotides();
}

ADNPointer<ADNSingleStrand> ADNMainInterface::GetNucleotideStrand(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetStrand();
}

int ADNMainInterface::GetSingleStrandId(ADNPointer<ADNSingleStrand> ss)
{
  return ss->GetId();
}

ADNPointer<ADNNucleotide> ADNMainInterface::GetFivePrimeFromSingleStrand(ADNPointer<ADNSingleStrand> ss)
{
  return ss->GetFivePrime();
}

ADNPointer<ADNNucleotide> ADNMainInterface::GetThreePrimeFromSingleStrand(ADNPointer<ADNSingleStrand> ss)
{
  return ss->GetThreePrime();
}

ADNPointer<ADNNucleotide> ADNMainInterface::GetNextNucleotide(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetNext();
}

int ADNMainInterface::GetNucleotideId(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetId();
}

void ADNMainInterface::SetNucleotideType(ADNPointer<ADNNucleotide> nt, DNABlocks t)
{
  nt->SetType(t);
}

std::string ADNMainInterface::GetSingleStrandSequence(ADNPointer<ADNSingleStrand> ss)
{
  return ss->GetSequence();
}

void ADNMainInterface::ShiftStartSingleStrand(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt, bool shiftSeq)
{
  ss->ShiftStart(nt, shiftSeq);
}

void ADNMainInterface::SetSingleStrandSequence(ADNPointer<ADNSingleStrand> ss, std::string intendedSeq)
{
  ss->SetSequence(intendedSeq);
}

void ADNMainInterface::CopyAtomsIntoNucleotide(ADNPointer<ADNNucleotide> nt, ADNPointer<ADNNucleotide> nt2)
{
  nt->CopyAtoms(nt2);
}

ADNPointer<ADNBasePair> ADNMainInterface::CreateBasePair()
{
  return ADNPointer<ADNBasePair>(new ADNBasePair());
}

void ADNMainInterface::SetLeftNucleotideBasePair(ADNPointer<ADNBasePair> bp, ADNPointer<ADNNucleotide> left)
{
  bp->SetLeftNucleotide(left);
}

void ADNMainInterface::SetRightNucleotideBasePair(ADNPointer<ADNBasePair> bp, ADNPointer<ADNNucleotide> right)
{
  bp->SetRightNucleotide(right);
}

ADNPointer<ADNNucleotide> ADNMainInterface::GetLeftNucleotideBasePair(ADNPointer<ADNBasePair> bp)
{
  return bp->GetLeftNucleotide();
}

ADNPointer<ADNNucleotide> ADNMainInterface::GetRightNucleotideBasePair(ADNPointer<ADNBasePair> bp)
{
  return bp->GetRightNucleotide();
}

ADNPointer<ADNPart> ADNMainInterface::LoadPartFromJsonLegacy(ADNPointer<ADNNanorobot> nr, std::string filename)
{
  return ADNLoader::LoadPartFromJsonLegacy(nr, filename);
}

ADNPointer<ADNLoopPair> ADNMainInterface::CreateLoopPair()
{
  return ADNPointer<ADNLoopPair>(new ADNLoopPair());
}

ADNPointer<ADNLoop> ADNMainInterface::CreateLoop()
{
  return ADNPointer<ADNLoop>(new ADNLoop());
}

void ADNMainInterface::SetStartNucleotideInLoop(ADNPointer<ADNLoop> loop, ADNPointer<ADNNucleotide> nt)
{
  loop->SetStart(nt);
}

void ADNMainInterface::SetEndNucleotideInLoop(ADNPointer<ADNLoop> loop, ADNPointer<ADNNucleotide> nt)
{
  loop->SetEnd(nt);
}

void ADNMainInterface::AddNucleotideToLoop(ADNPointer<ADNLoop> loop, ADNPointer<ADNNucleotide> nt)
{
  loop->AddNucleotide(nt);
}

void ADNMainInterface::SetLeftLoop(ADNPointer<ADNLoopPair> lp, ADNPointer<ADNLoop> left)
{
  lp->SetLeftLoop(left);
}

ADNPointer<ADNLoop> ADNMainInterface::GetLeftLoop(ADNPointer<ADNLoopPair> lp)
{
  return lp->GetLeftLoop();
}

ADNPointer<ADNNucleotide> ADNMainInterface::GetStartNucleotideInLoop(ADNPointer<ADNLoop> loop)
{
  return loop->GetStart();
}

ADNPointer<ADNNucleotide> ADNMainInterface::GetEndNucleotideInLoop(ADNPointer<ADNLoop> loop)
{
  return loop->GetEnd();
}

CollectionMap<ADNNucleotide> ADNMainInterface::GetNucleotidesInLoop(ADNPointer<ADNLoop> loop)
{
  return loop->GetNucleotides();
}

ADNPointer<ADNBaseSegment> ADNMainInterface::CreateBaseSegment(int id)
{
  ADNPointer<ADNBaseSegment> bs = ADNPointer<ADNBaseSegment>(new ADNBaseSegment());
  bs->SetId(id);
  return bs;
}

void ADNMainInterface::AddCellToBaseSegment(ADNPointer<ADNBaseSegment> bs, ADNPointer<ADNCell> cell)
{
  bs->SetCell(cell);
}

int ADNMainInterface::GetBaseSegmentId(ADNPointer<ADNBaseSegment> bs)
{
  return bs->GetId();
}

CellType ADNMainInterface::GetCellType(ADNPointer<ADNBaseSegment> bs)
{
  return bs->GetCellType();
}

ADNPointer<ADNDoubleStrand> ADNMainInterface::CreateDoubleStrand(int id)
{
  ADNPointer<ADNDoubleStrand> ds = ADNPointer<ADNDoubleStrand>(new ADNDoubleStrand());
  ds->SetId(id);
  return ds;
}

void ADNMainInterface::AddBaseSegementToDoubleStrandEnd(ADNPointer<ADNBaseSegment> bs, ADNPointer<ADNDoubleStrand> ds)
{
  ds->AddBaseSegmentEnd(bs);
}

CollectionMap<ADNBaseSegment> ADNMainInterface::GetBaseSegments(ADNPointer<ADNDoubleStrand> ds)
{
  return ds->GetBaseSegments();
}

ADNPointer<ADNBaseSegment> ADNMainInterface::GetStartBaseSegment(ADNPointer<ADNDoubleStrand> ds)
{
  return ds->GetFirstBaseSegment();
}

ADNPointer<ADNBaseSegment> ADNMainInterface::GetEndBaseSegment(ADNPointer<ADNDoubleStrand> ds)
{
  return ds->GetLastBaseSegment();
}

ADNPointer<ADNPart> ADNMainInterface::CreatePart(int id)
{
  ADNPointer<ADNPart> part = ADNPointer<ADNPart>(new ADNPart());
  part->SetId(id);
  return part;
}

void ADNMainInterface::RegisterSingleStrandInPart(ADNPointer<ADNPart> part, ADNPointer<ADNSingleStrand> ss)
{
  part->RegisterSingleStrand(ss);
}

void ADNMainInterface::RegisterDoubleStrandInPart(ADNPointer<ADNPart> part, ADNPointer<ADNDoubleStrand> ds)
{
  part->RegisterDoubleStrand(ds);
}

void ADNMainInterface::SetNucleotidePair(ADNPointer<ADNNucleotide> ntOne, ADNPointer<ADNNucleotide> ntTwo)
{
  ntOne->SetPair(ntTwo);
  ntTwo->SetPair(ntOne);
}

int ADNMainInterface::GetNumberOfDoubleStrands(ADNPointer<ADNPart> part)
{
  return part->GetNumberOfDoubleStrands();
}

int ADNMainInterface::GetNumberOfDoubleStrands(ADNPointer<ADNNanorobot> nr)
{
  return nr->GetNumberOfDoubleStrands();
}

int ADNMainInterface::GetNumberOfSingleStrands(ADNPointer<ADNPart> part)
{
  return part->GetNumberOfSingleStrands();
}

int ADNMainInterface::GetNumberOfSingleStrands(ADNPointer<ADNNanorobot> nr)
{
  return nr->GetNumberOfSingleStrands();
}

int ADNMainInterface::GetNumberOfBaseSegments(ADNPointer<ADNPart> part)
{
  return part->GetNumberOfBaseSegments();
}

int ADNMainInterface::GetNumberOfBaseSegments(ADNPointer<ADNNanorobot> nr)
{
  return nr->GetNumberOfBaseSegments();
}

int ADNMainInterface::GetNumberOfNucleotides(ADNPointer<ADNPart> part)
{
  return part->GetNumberOfNucleotides();
}

int ADNMainInterface::GetNumberOfNucleotides(ADNPointer<ADNNanorobot> nr)
{
  return nr->GetNumberOfNucleotides();
}

int ADNMainInterface::GetPartId(ADNPointer<ADNPart> part)
{
  return part->GetId();
}

CollectionMap<ADNSingleStrand> ADNMainInterface::GetSingleStrandsFromPart(ADNPointer<ADNPart> part)
{
  return part->GetSingleStrands();
}

CollectionMap<ADNDoubleStrand> ADNMainInterface::GetDoubleStrandsFromPart(ADNPointer<ADNPart> part)
{
  return part->GetDoubleStrands();
}

void ADNMainInterface::SavePartToJson(ADNPointer<ADNPart> p, std::string filename)
{
  ADNLoader::SavePartToJson(p, filename);
}

ADNPointer<ADNPart> ADNMainInterface::LoadPartFromJson(ADNPointer<ADNNanorobot> nr, std::string filename)
{
  return ADNLoader::LoadPartFromJson(nr, filename);
}

int ADNMainInterface::CheckSequenceEquality(ADNPointer<ADNPart> part, ADNPointer<ADNPart> part2)
{
  auto singleStrands1 = part->GetSingleStrands();
  auto singleStrands2 = part2->GetSingleStrands();

  if (singleStrands1.size() != singleStrands2.size()) {
    return -1;
  }

  int count = boost::numeric_cast<int>(singleStrands1.size());
  for (auto &s : singleStrands1) {
    ADNPointer<ADNSingleStrand> ss = s.second;
    std::string seq = ss->GetSequence();
    for (auto &s2 : singleStrands2) {
      ADNPointer<ADNSingleStrand> ss2 = s2.second;
      std::string seq2 = ss2->GetSequence();
      if (seq == seq2) {
        count--;
        break;
      }
    }
  }

  return count;
}

DNABlocks ADNMainInterface::GetComplementaryNucleotide(DNABlocks t)
{
  return ADNModel::GetComplementaryBase(t);
}

ADNPointer<ADNNucleotide> ADNMainInterface::GetNucleotidePair(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetPair();
}

DNABlocks ADNMainInterface::GetNucleotideType(ADNPointer<ADNNucleotide> nt)
{
  return nt->GetType();
}

CollectionMap<ADNNucleotide> ADNMainInterface::GetNucleotidesInPart(ADNPointer<ADNPart> part)
{
  return part->GetNucleotides();
}

ADNPointer<ADNSingleStrand> ADNMainInterface::ConcatenateSingleStrands(ADNPointer<ADNSingleStrand> ss1, ADNPointer<ADNSingleStrand> ss2)
{
  return ADNBasicOperations::MergeSingleStrands(ss1, ss2);
}

ADNPointer<ADNDoubleStrand> ADNMainInterface::ConcatenateDoubleStrands(ADNPointer<ADNDoubleStrand> ds1, ADNPointer<ADNDoubleStrand> ds2)
{
  return ADNBasicOperations::MergeDoubleStrand(ds1, ds2);
}

std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> ADNMainInterface::BreakSingleStrand(ADNPointer<ADNNucleotide> nt)
{
  return ADNBasicOperations::BreakSingleStrand(nt);
}

std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> ADNMainInterface::BreakDoubleStrand(ADNPointer<ADNBaseSegment> bs)
{
  return ADNBasicOperations::BreakDoubleStrand(bs);
}

void ADNMainInterface::MutateNucleotide(ADNPointer<ADNNucleotide> nt, DNABlocks t)
{
  ADNBasicOperations::MutateNucleotide(nt, t);
}

void ADNMainInterface::MutateSingleStrandSequence(ADNPointer<ADNSingleStrand> ss, std::string seq, bool changePair)
{
  ADNBasicOperations::SetSingleStrandSequence(ss, seq, changePair);
}

int ADNMainInterface::GetNumberOfAtoms(ADNPointer<ADNPart> part)
{
  return part->GetNumberOfAtoms();
}

int ADNMainInterface::GetDoubleStrandId(ADNPointer<ADNDoubleStrand> ds)
{
  return ds->GetId();
}

int ADNMainInterface::CheckSingleStrandLinkedSize(ADNPointer<ADNSingleStrand> ss)
{
  ADNPointer<ADNNucleotide> nt = ss->GetFivePrime();

  int count = 0;

  do {
    count++;
    nt = nt->GetNext();
  } while (nt != nullptr);

  return count;
}

int ADNMainInterface::CheckDoubleStrandLinkedSize(ADNPointer<ADNDoubleStrand> ds)
{
  ADNPointer<ADNBaseSegment> bs = ds->GetFirstBaseSegment();

  int count = 0;

  do {
    count++;
    bs = bs->GetNext();
  } while (bs != nullptr);

  return count;
}

ADNPointer<ADNPart> ADNMainInterface::CreateMockPart(int length)
{
  ADNPointer<ADNPart> part = CreatePart();

  ADNPointer<ADNSingleStrand> ssFF = CreateSingleStrand();
  ADNPointer<ADNSingleStrand> ssFS = CreateSingleStrand();
  ADNPointer<ADNSingleStrand> ssSF = CreateSingleStrand();
  ADNPointer<ADNSingleStrand> ssSS = CreateSingleStrand();
  RegisterSingleStrandInPart(part, ssFF);
  RegisterSingleStrandInPart(part, ssFS);
  RegisterSingleStrandInPart(part, ssSF);
  RegisterSingleStrandInPart(part, ssSS);

  ADNPointer<ADNDoubleStrand> dsF = ADNMainInterface::CreateDoubleStrand();
  ADNPointer<ADNDoubleStrand> dsS = ADNMainInterface::CreateDoubleStrand();
  ADNMainInterface::RegisterDoubleStrandInPart(part, dsF);
  ADNMainInterface::RegisterDoubleStrandInPart(part, dsS);

  int nt_id = 0;
  int bs_id = 0;
  for (int i = 0; i < length; ++i) {
    ADNPointer<ADNNucleotide> ntFF = CreateADNNucleotide(nt_id);
    ++nt_id;
    ADNPointer<ADNNucleotide> ntFS = CreateADNNucleotide(nt_id);
    ++nt_id;
    SetNucleotidePair(ntFF, ntFS);

    ADNPointer<ADNNucleotide> ntSF = CreateADNNucleotide(nt_id);
    ++nt_id;
    ADNPointer<ADNNucleotide> ntSS = CreateADNNucleotide(nt_id);
    ++nt_id;
    SetNucleotidePair(ntSF, ntSS);

    AddNucleotideToSingleStrandThreePrime(ntFF, ssFF);
    AddNucleotideToSingleStrandFivePrime(ntFS, ssFS);
    AddNucleotideToSingleStrandThreePrime(ntSF, ssSF);
    AddNucleotideToSingleStrandFivePrime(ntSS, ssSS);

    ADNPointer<ADNBaseSegment> bsF = CreateBaseSegment(bs_id);
    ++bs_id;
    ADNPointer<ADNBaseSegment> bsS = CreateBaseSegment(bs_id);
    ++bs_id;

    AddBaseSegementToDoubleStrandEnd(bsF, dsF);
    AddBaseSegementToDoubleStrandEnd(bsS, dsS);

    ADNPointer<ADNBasePair> bpF = CreateBasePair();
    ADNPointer<ADNBasePair> bpS = CreateBasePair();

    AddCellToBaseSegment(bsF, bpF);
    AddCellToBaseSegment(bsS, bpS);

    ADNMainInterface::SetLeftNucleotideBasePair(bpF, ntFF);
    ADNMainInterface::SetRightNucleotideBasePair(bpF, ntFS);
    ADNMainInterface::SetLeftNucleotideBasePair(bpS, ntSF);
    ADNMainInterface::SetRightNucleotideBasePair(bpS, ntSS);
  }
  
  return part;
}

ADNPointer<ADNNanorobot> ADNMainInterface::CreateNanorobot()
{
  return ADNPointer<ADNNanorobot>(new ADNNanorobot());
}

void ADNMainInterface::RegisterPartInNanorobot(ADNPointer<ADNNanorobot> nr, ADNPointer<ADNPart> part)
{
  nr->RegisterPart(part);
}

int ADNMainInterface::GetNumberOfParts(ADNPointer<ADNNanorobot> nr)
{
  return boost::numeric_cast<int>(nr->GetParts().size());
}

ADNPointer<ADNBackbone> ADNMainInterface::CreateBackbone()
{
  return ADNPointer<ADNBackbone>(new ADNBackbone());
}

void ADNMainInterface::SetAtomName(ADNPointer<ADNAtom> atom, std::string name)
{
  atom->SetName(name);
}
