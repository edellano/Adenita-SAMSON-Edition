#include "DASEditor.hpp"

//ADNPart* DASEditor::CreateTube(size_t length, SBPosition3 start, SBVector3 direction) {
//  ANTPart* nrobot = new ANTPart();
//
//  AddDoubleStrandToANTPart(nrobot, length, start, direction);
//  
//  nrobot->SetScaffold(0);
//  nrobot->SetNtSegmentMap();
//  return nrobot;
//}
//
//ADNPart * DASEditor::CreateRoundOrigami(SBQuantity::length diameter, SBPosition3 center, SBVector3 direction, int min_length) {
//  ADNPart* nanorobot = nullptr;
//
//  // so far create only a circle
//  // transformation to global coordinates
//  ublas::vector<double> dir = ublas::vector<double>(3);
//  dir[0] = direction[0].getValue();
//  dir[1] = direction[1].getValue();
//  dir[2] = direction[2].getValue();
//  ublas::matrix<double> subspace = FindOrthogonalSubspace(dir);
//  ADNVectorMath::AddRowToMatrix(subspace, dir);
//
//  if (diameter > SBQuantity::length(0.0)) {
//    // number of double helices that fit into the circumpherence
//    SBQuantity::length r = SBQuantity::nanometer(ADNConstants::DH_DIAMETER * 0.5);
//    SBQuantity::length R = diameter * 0.5;
//
//    // in a circumpherence every double strand is going to take up the same space
//    auto up = -r*r*(4 * R*R - r*r) + (2 * R*R - r*r)*(2 * R*R - r*r);
//    auto down = r*r*(4 * R*R - r*r) + (2 * R*R - r*r)*(2 * R*R - r*r);
//    auto cosTheta = up / down;
//    auto theta = acos(cosTheta.getValue());
//    double pi = atan(1.0) * 4.0;
//    int num = ceil(2 * pi / theta);
//
//    // recalculate the exact radius so num will fit
//    auto newR = theta * num * R / (2*pi);
//    auto newTheta = 2 * pi / num;
//
//    if (num > 0) {
//      nanorobot = new ADNPart();
//      // create dsDNA
//      double t = 0.0;
//      for (int j = 0; j < num; ++j) {
//      //  // a and b are the coordinates on the plane
//        auto a = newR*sin(t);
//        auto b = newR*cos(t);
//        ublas::vector<double> pos_p(3);
//        pos_p[0] = a.getValue();
//        pos_p[1] = b.getValue();
//        pos_p[2] = 0.0;
//        ublas::vector<double> trf = ublas::prod(ublas::trans(subspace), pos_p);
//        SBPosition3 dsPosition = SBPosition3(SBQuantity::picometer(trf[0]), SBQuantity::picometer(trf[1]), SBQuantity::picometer(trf[2])) + center;
//        AddDoubleStrandToANTPart(nanorobot, min_length, dsPosition, direction);
//        //t += acos(cosTheta.getValue());
//        t += newTheta;
//      }
//      nanorobot->SetScaffold(0);
//      nanorobot->SetNtSegmentMap();
//
//      nanorobot->e1_ = ADNVectorMath::row(subspace, 0);
//      nanorobot->e2_ = ADNVectorMath::row(subspace, 1);
//      nanorobot->e3_ = ADNVectorMath::row(subspace, 2);
//
//      //ANTAuxiliary::logVector("e1", nanorobot->e1_);
//      //ANTAuxiliary::logVector("e2", nanorobot->e2_);
//      //ANTAuxiliary::logVector("e3", nanorobot->e3_);
//
//      nanorobot->setCenterOfMass();
//    }
//
//    //ANTAuxiliary::log(string("num of ds"));
//    //ANTAuxiliary::log(QString::number(num));
//    //ANTAuxiliary::log(string("bps per ds "), false);
//    //ANTAuxiliary::log(QString::number(min_length));
//    //ANTAuxiliary::log(string("total bps"), false);
//    //ANTAuxiliary::log(QString::number(min_length * num));
//  }
//
//  //generate 1D
//
//  return nanorobot;
//}
//
//ADNPart * DASEditor::CreateTwoTubes(size_t length, SBPosition3 start, SBVector3 direction, SBVector3 sepDir) {
//  ADNPart* part = new ADNPart();
//
//  // direction in which the double helices would be separated
//  //SBVector3 sepDir = SBVector3(direction[1], -direction[0], SBQuantity::dimensionless(0.0));
//  SBQuantity::length sepDist = SBQuantity::angstrom(0.5*ADNConstants::DH_DIST + 5*ADNConstants::DH_DIAMETER);
//  SBPosition3 startFirst = start - sepDist * sepDir;
//  SBPosition3 startSecond = start + sepDist * sepDir + SBQuantity::nanometer(length * ADNConstants::BP_RISE) * direction;
//
//  AddDoubleStrandToANTPart(part, length, startFirst, direction);
//  AddDoubleStrandToANTPart(part, length, startSecond, -direction);
//
//  part->SetScaffold(0);
//  part->SetNtSegmentMap();
//  return part;
//}
//
//void DASEditor::AddDoubleStrandToANTPart(ADNPart * part, size_t length, SBPosition3 start, SBVector3 direction) {
//  if (part != nullptr) {
//    // Create joints and segments
//    int j_id = part->GetLastJointId();
//    int b_id = part->GetLastBaseSegmentId();
//
//    ANTJoint* source = new ANTJoint();
//    source->id_ = j_id;
//    ++j_id;
//    source->position_ = start;
//    part->AddJoint(source);
//    SBPosition3 delt = SBQuantity::nanometer(ANTConstants::BP_RISE) * direction;
//    SBPosition3 pos = start + delt;
//
//    ANTBaseSegment* prev_bs = nullptr;
//    ANTBaseSegment* start_bs = nullptr;
//    for (size_t i = 0; i < length; ++i) {
//      ANTJoint* target = new ANTJoint();
//      target->position_ = pos;
//      target->id_ = j_id;
//      part->AddJoint(target);
//      // create base segment
//      ANTBaseSegment* bs = new ANTBaseSegment(source, target);
//      if (i == 0) start_bs = bs;
//      bs->number_ = boost::numeric_cast<unsigned int>(i);
//      bs->direction_ = direction;
//      bs->id_ = b_id;
//      bs->previous_ = prev_bs;
//      bs->SetCell(new ANTBasePair());
//      if (prev_bs != nullptr) prev_bs->next_ = bs;
//      part->AddBaseSegment(bs);
//      pos += delt;
//      source = target;
//      ++b_id;
//      ++j_id;
//      prev_bs = bs;
//    }
//
//    // create double strand
//    ANTDoubleStrand* ds = new ANTDoubleStrand();
//    ds->start_ = start_bs;
//    ds->size_ = boost::numeric_cast<unsigned int>(length);
//    part->RegisterDoubleStrand(ds);
//
//    // create nucleotides
//    ANTSingleStrand* ss = new ANTSingleStrand();
//    ANTSingleStrand* pair_ss = new ANTSingleStrand();
//    pair_ss->isScaffold_ = false;
//    ANTBaseSegment* bs = start_bs;
//    ANTNucleotide* prev_nt = nullptr;
//    int nt_id_ss = 0;
//    int nt_id_pair = 0;
//    ANTNucleotide* next_nt_pair = nullptr;
//    while (bs != nullptr) {
//      ANTNucleotide* nt = new ANTNucleotide();
//      nt->id_ = nt_id_ss;
//      nt->type_ = DNABlocks::DN_;
//      nt->prev_ = prev_nt;
//      nt->SetPosition(bs->GetSBPosition());
//      nt->SetBackboneCenter(bs->GetSBPosition());
//      nt->SetSidechainCenter(bs->GetSBPosition());
//      if (prev_nt != nullptr) prev_nt->next_ = nt;
//      ANTBasePair* bp = static_cast<ANTBasePair*>(bs->GetCell());
//      bp->SetLeftNucleotide(nt);
//      ss->nucleotides_.insert(std::make_pair(nt->id_, nt));
//      nt->strand_ = ss;
//      // create pair
//      ANTNucleotide* nt_pair = new ANTNucleotide();
//      nt_pair->id_ = nt_id_pair;
//      nt_pair->type_ = DNABlocks::DN_;
//      nt_pair->next_ = next_nt_pair;
//      nt_pair->SetPosition(bs->GetSBPosition());
//      nt_pair->SetBackboneCenter(bs->GetSBPosition());
//      nt_pair->SetSidechainCenter(bs->GetSBPosition());
//      bp->SetRightNucleotide(nt_pair);
//      if (next_nt_pair != nullptr) next_nt_pair->prev_ = nt_pair;
//      pair_ss->nucleotides_.insert(std::make_pair(nt_pair->id_, nt_pair));
//      nt_pair->strand_ = pair_ss;
//      nt->pair_ = nt_pair;
//      nt_pair->pair_ = nt;
//      ++nt_id_ss;
//      ++nt_id_pair;
//      prev_nt = nt;
//      next_nt_pair = nt_pair;
//      bs->doubleStrand_ = ds;
//      bs = bs->next_;
//    }
//    ss->SetNucleotidesEnd();
//    pair_ss->SetNucleotidesEnd();
//    part->RegisterSingleStrand(ss);
//    ss->SetDefaultName();
//    part->RegisterSingleStrand(pair_ss);
//    pair_ss->SetDefaultName();
//  }
//}
//
//ANTPart * DASEditor::CreateCrippledRoundOrigami(SBQuantity::length diameter, SBPosition3 center, SBVector3 direction, int length) {
//  ANTPart* nanorobot = nullptr;
//
//  // so far create only a circle
//  // transformation to global coordinates
//  ublas::vector<double> dir = ublas::vector<double>(3);
//  dir[0] = direction[0].getValue();
//  dir[1] = direction[1].getValue();
//  dir[2] = direction[2].getValue();
//  ublas::matrix<double> subspace = FindOrthogonalSubspace(dir);
//  ANTVectorMath::AddRowToMatrix(subspace, dir);
//
//  int j_id = 0;
//  int b_id = 0;
//
//  if (diameter > SBQuantity::length(0.0)) {
//    // number of double helices that fit into the circumpherence
//    SBQuantity::length r = SBQuantity::nanometer(ANTConstants::DH_DIAMETER * 0.5);
//    SBQuantity::length R = diameter * 0.5;
//
//    // in a circumpherence every double strand is going to take up the same space
//    auto up = -r*r*(4 * R*R - r*r) + (2 * R*R - r*r)*(2 * R*R - r*r);
//    auto down = r*r*(4 * R*R - r*r) + (2 * R*R - r*r)*(2 * R*R - r*r);
//    auto cosTheta = up / down;
//    auto theta = acos(cosTheta.getValue());
//    double pi = atan(1.0) * 4.0;
//    int num = ceil(2 * pi / theta);
//
//    // recalculate the exact radius so num will fit
//    auto newR = theta * num * R / (2 * pi);
//    auto newTheta = 2 * pi / num;
//
//    if (num > 0) {
//      nanorobot = new ANTPart();
//      // create dsDNA
//      double t = 0.0;
//      for (int j = 0; j < num; ++j) {
//        //  // a and b are the coordinates on the plane
//        auto a = newR*sin(t);
//        auto b = newR*cos(t);
//        ublas::vector<double> pos_p(3);
//        pos_p[0] = a.getValue();
//        pos_p[1] = b.getValue();
//        pos_p[2] = 0.0;
//        ublas::vector<double> trf = ublas::prod(ublas::trans(subspace), pos_p);
//        SBPosition3 startPosition = SBPosition3(SBQuantity::picometer(trf[0]), SBQuantity::picometer(trf[1]), SBQuantity::picometer(trf[2])) + center;
//        AddCrippledDoubleStrandToANTPart(startPosition, direction, length, *nanorobot);
//
//        t += newTheta;
//      }
//      nanorobot->SetScaffold(0);
//      nanorobot->SetNtSegmentMap();
//    }
//  }
//
//  nanorobot->e1_ = ANTVectorMath::row(subspace, 0);
//  nanorobot->e2_ = ANTVectorMath::row(subspace, 1);
//  nanorobot->e3_ = ANTVectorMath::row(subspace, 2);
//
//  //ANTAuxiliary::logVector("e1", nanorobot->e1_);
//  //ANTAuxiliary::logVector("e2", nanorobot->e2_);
//  //ANTAuxiliary::logVector("e3", nanorobot->e3_);
//
//  nanorobot->setCenterOfMass();
//
//  //generate 1D
//
//  return nanorobot;
//}
//
//void DASEditor::AddCrippledDoubleStrandToANTPart(SBPosition3 startPosition, SBVector3 direction, int length, ANTPart& nanorobot) {
//  ANTJoint* source = new ANTJoint();
//  source->position_ = startPosition;
//  nanorobot.AddJoint(source);
//  SBPosition3 delt = SBQuantity::nanometer(ANTConstants::BP_RISE) * direction;
//  SBPosition3 pos = startPosition + delt;
//
//  ANTBaseSegment* prev_bs = nullptr;
//  ANTBaseSegment* start_bs = nullptr;
//  for (size_t i = 0; i < length; ++i) {
//    ANTJoint* target = new ANTJoint();
//    target->position_ = pos;
//    nanorobot.AddJoint(target);
//    // create base segment
//    ANTBaseSegment* bs = new ANTBaseSegment(source, target);
//    if (i == 0) start_bs = bs;
//    bs->number_ = boost::numeric_cast<unsigned int>(i);
//    bs->direction_ = direction;
//    bs->previous_ = prev_bs;
//    if (prev_bs != nullptr) prev_bs->next_ = bs;
//    nanorobot.AddBaseSegment(bs);
//    pos += delt;
//    source = target;
//    prev_bs = bs;
//  }
//
//  // create double strand
//  ANTDoubleStrand* ds = new ANTDoubleStrand();
//  ds->start_ = start_bs;
//  ds->size_ = length;
//  nanorobot.RegisterDoubleStrand(ds);
//}
//
//ANTPart * DASEditor::CreateCrippledWireframeCuboid(SBPosition3 topLeftFront, int xSize, int ySize, int zSize) {
//  ANTPart* part = new ANTPart();
//
//  SBVector3 xDir(1.0, 0.0, 0.0);
//  SBVector3 yDir(0.0, 1.0, 0.0);
//  SBVector3 zDir(0.0, 0.0, 1.0);
//
//  SBQuantity::nanometer xLength = SBQuantity::nanometer(ANTConstants::BP_RISE * xSize);
//  SBQuantity::nanometer yLength = SBQuantity::nanometer(ANTConstants::BP_RISE * ySize);
//  SBQuantity::nanometer zLength = SBQuantity::nanometer(ANTConstants::BP_RISE * zSize);
//
//  // create 12 double helices
//  AddCrippledDoubleStrandToANTPart(topLeftFront, xDir, xSize, *part);
//  AddCrippledDoubleStrandToANTPart(topLeftFront, -yDir, ySize, *part);
//  AddCrippledDoubleStrandToANTPart(topLeftFront, -zDir, zSize, *part);
//
//  SBPosition3 bottomRightFront = topLeftFront + (xLength*xDir - yLength*yDir);
//
//  AddCrippledDoubleStrandToANTPart(bottomRightFront, -xDir, xSize, *part);
//  AddCrippledDoubleStrandToANTPart(bottomRightFront, yDir, ySize, *part);
//  AddCrippledDoubleStrandToANTPart(bottomRightFront, -zDir, zSize, *part);
//
//  SBPosition3 bottomLeftBack = topLeftFront - (yLength*yDir + zLength*zDir);
//
//  AddCrippledDoubleStrandToANTPart(bottomLeftBack, xDir, xSize, *part);
//  AddCrippledDoubleStrandToANTPart(bottomLeftBack, yDir, ySize, *part);
//  AddCrippledDoubleStrandToANTPart(bottomLeftBack, zDir, zSize, *part);
//
//  SBPosition3 topRightBack = topLeftFront + (xLength*xDir - zLength*zDir);
//
//  AddCrippledDoubleStrandToANTPart(topRightBack, -xDir, xSize, *part);
//  AddCrippledDoubleStrandToANTPart(topRightBack, -yDir, ySize, *part);
//  AddCrippledDoubleStrandToANTPart(topRightBack, zDir, zSize, *part);
//
//  return part;
//}
//
//ANTPart * DASEditor::DebugCrossoversPart() {
//  ANTPart* part = new ANTPart();
//
//  SBPosition3 init_pos = SBPosition3();
//  SBVector3 init_dir = SBVector3(0.0, 0.0, 1.0);
//  size_t length = 42;
//
//  AddDoubleStrandToANTPart(part, length, init_pos, init_dir);
//
//  SBVector3 dirPlane = SBVector3(1.0, 0.0, 1.0);
//  SBPosition3 pos = init_pos + SBQuantity::nanometer(ANTConstants::DH_DIAMETER + 0.25) * dirPlane;
//  SBPosition3 posFinal = pos + SBQuantity::nanometer(ANTConstants::BP_RISE * length) * init_dir;
//
//  AddDoubleStrandToANTPart(part, length, posFinal, -init_dir);
//  AddDoubleStrandToANTPart(part, length, -pos, init_dir);
//
//  part->SetScaffold(0);
//  part->SetNtSegmentMap();
//
//  return part;
//}

// todo: calculate positions
ADNPointer<ADNDoubleStrand> DASEditor::CreateDoubleStrand(int length, SBPosition3 start, SBVector3 direction)
{
  SBPosition3 delt = SBQuantity::nanometer(ADNConstants::BP_RISE) * direction;
  SBPosition3 pos = start;

  ADNPointer<ADNDoubleStrand> ds = new ADNDoubleStrand();

  // create nucleotides
  ADNPointer<ADNSingleStrand> ss = new ADNSingleStrand();
  ADNPointer<ADNSingleStrand> pair_ss = new ADNSingleStrand();

  for (size_t i = 0; i < length; ++i) {
    ADNPointer<ADNBaseSegment> bs = new ADNBaseSegment();
    ds->AddBaseSegmentEnd(bs);

    ADNPointer<ADNBasePair> bp = new ADNBasePair();
    bs->SetCell(bp());
    bs->SetPosition(pos);

    ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
    nt->SetType(DNABlocks::DI);
    nt->SetPosition(bs->GetPosition());
    nt->SetBackbonePosition(bs->GetPosition());
    nt->SetSidechainPosition(bs->GetPosition());
    nt->SetBaseSegment(bs);
    bp->SetLeftNucleotide(nt);

    ADNPointer<ADNNucleotide> ntPair = new ADNNucleotide();
    ntPair->SetType(DNABlocks::DI);
    ntPair->SetPosition(bs->GetPosition());
    ntPair->SetBackbonePosition(bs->GetPosition());
    ntPair->SetSidechainPosition(bs->GetPosition());
    ntPair->SetBaseSegment(bs);
    bp->SetRightNucleotide(ntPair);

    nt->SetPair(ntPair);
    ntPair->SetPair(nt);

    ss->AddNucleotideThreePrime(nt);
    pair_ss->AddNucleotideFivePrime(ntPair);

    pos += delt;
  }
  
  ss->SetDefaultName();
  pair_ss->SetDefaultName();

  return ds;
}

ADNPointer<ADNSingleStrand> DASEditor::CreateSingleStrand(int length, SBPosition3 start, SBVector3 direction)
{
  SBPosition3 delt = SBQuantity::nanometer(ADNConstants::BP_RISE) * direction;
  SBPosition3 pos = start;

  ADNPointer<ADNDoubleStrand> ds = new ADNDoubleStrand();

  // create nucleotides
  ADNPointer<ADNSingleStrand> ss = new ADNSingleStrand();

  for (size_t i = 0; i < length; ++i) {
    ADNPointer<ADNBaseSegment> bs = new ADNBaseSegment();
    ds->AddBaseSegmentEnd(bs);

    ADNPointer<ADNBasePair> bp = new ADNBasePair();
    bs->SetCell(bp());
    bs->SetPosition(pos);

    ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
    nt->SetType(DNABlocks::DI);
    nt->SetPosition(bs->GetPosition());
    nt->SetBackbonePosition(bs->GetPosition());
    nt->SetSidechainPosition(bs->GetPosition());
    nt->SetBaseSegment(bs);
    bp->SetLeftNucleotide(nt);

    ss->AddNucleotideThreePrime(nt);

    pos += delt;
  }

  ss->SetDefaultName();

  return ss;
}
