#include "DASCadnano.hpp"

void DASCadnano::ParseJSON(std::string filename)
{
  FILE* fp;
  fopen_s(&fp, filename.c_str(), "rb");
  char readBuffer[65536];
  FileReadStream is(fp, readBuffer, sizeof(readBuffer));
  Document d;
  d.ParseStream(is);

  ADNLogger& logger = ADNLogger::GetLogger();

  // check for save version
  double versionValue = 0.0;
  if (Value* version = Pointer("/format").Get(d)) {
    versionValue = version->GetDouble();
  }

  if (versionValue < 3.0) {
    ParseCadnanoLegacy(d);
  }
  else {
    ParseCadnanoFormat3(d);
  }

  fclose(fp);
}

void DASCadnano::ParseCadnanoFormat3(Document & d)
{
}

void DASCadnano::ParseCadnanoLegacy(Document& d)
{
  json_.name_ = d["name"].GetString();
  Value& vstrandsVal = d["vstrands"];

  int totalCount;
  for (unsigned int i = 0; i < vstrandsVal.Size(); i++) {

    Value& vstrandVal = vstrandsVal[i];
    Vstrand vstrand;
    
    vstrand.num_ = vstrandVal["num"].GetInt();
    vstrand.col_ = vstrandVal["col"].GetInt();
    vstrand.row_ = vstrandVal["row"].GetInt();

    Value& scafVals = vstrandVal["scaf"];
    int count = 0;
    for (unsigned int k = 0; k < scafVals.Size(); k++) {
      Value& a = scafVals[k];
      vec4 elem;
      elem.n0 = a[0].GetInt();
      elem.n1 = a[1].GetInt();
      elem.n2 = a[2].GetInt();
      elem.n3 = a[3].GetInt();
      vstrand.scaf_.insert(std::make_pair(count, elem));

      //start point
      if (elem.n0 == -1 && elem.n1 == -1 && elem.n2 != -1 && elem.n3 != -1) {
        json_.scaffoldStartPosition_ = std::make_pair(vstrand.num_, count);
      }

      ++count;
    }

    totalCount = count;  // all vhelix have the same count
    vstrand.totalLength_ = totalCount;

    Value& stapVals = vstrandVal["stap"];
    count = 0;
    for (unsigned int k = 0; k < stapVals.Size(); k++) {
      Value& a = stapVals[k];
      vec4 elem;
      elem.n0 = a[0].GetInt();
      elem.n1 = a[1].GetInt();
      elem.n2 = a[2].GetInt();
      elem.n3 = a[3].GetInt();
      vstrand.stap_.insert(std::make_pair(count, elem));

      if (elem.n0 == -1 && elem.n1 == -1 && elem.n2 != -1 && elem.n3 != -1) {
        vec2 stapleStart;
        stapleStart.n0 = vstrand.num_;
        stapleStart.n1 = count;

        json_.stapleStarts_.push_back(stapleStart);
      }

      ++count;
    }

    const Value& loopVals = vstrandVal["loop"];
    count = 0;
    for (unsigned int k = 0; k < loopVals.Size(); k++) {
      vstrand.loops_.insert(std::make_pair(count, loopVals[k].GetInt()));
      ++count;
    }

    const Value& skipVals = vstrandVal["skip"];
    count = 0;
    for (unsigned int k = 0; k < skipVals.Size(); k++) {
      vstrand.skips_.insert(std::make_pair(count, skipVals[k].GetInt()));
      ++count;
    }

    // create tubes
    bool start_tube = false;
    int init_pos = -1;
    int end_pos = -1;
    for (int i = 0; i < vstrand.totalLength_; ++i) {
      vec4 scaf_pos = vstrand.scaf_[i];
      vec4 stap_pos = vstrand.stap_[i];
      if (IsThereBase(scaf_pos) || IsThereBase(stap_pos)) {
        if (!start_tube) {
          start_tube = true;
          init_pos = i;
          end_pos = -1;
        }
      }
      else {
        if (start_tube) {
          start_tube = false;
          end_pos = i - 1;
          VTube tube;
          tube.vStrandId_ = vstrand.num_;
          tube.initPos_ = init_pos;
          tube.endPos_ = end_pos;
          vGrid_.AddTube(tube);
          init_pos = -1;
        }
      }
    }

    json_.vstrands_.insert(std::make_pair(vstrand.num_, vstrand));
  }

  if (totalCount % 32 == 0) {
    json_.lType_ = LatticeType::Square;
  }
  else if (totalCount % 21 == 0) {
    json_.lType_ = LatticeType::Honeycomb;
  }
  else {
    // error
  }
  vGrid_.CreateLattice(json_.lType_);
}

ADNPointer<ADNPart> DASCadnano::CreateCadnanoModel()
{
  ADNPointer<ADNPart> part = new ADNPart();

  CreateEdgeMap(part);
  CreateScaffold(part);
  CreateStaples(part);

  return part;
}

void DASCadnano::CreateEdgeMap(ADNPointer<ADNPart> nanorobot)
{
  auto tubes = vGrid_.vDoubleStrands_;
  for (auto &tube : tubes) {

    Vstrand* vs = &json_.vstrands_[tube.vStrandId_];
    SBPosition3 initPos = vGrid_.GetGridCellPos3D(tube.initPos_, vs->row_, vs->col_);
    SBPosition3 endPos = vGrid_.GetGridCellPos3D(tube.endPos_, vs->row_, vs->col_);
    int length = tube.endPos_ - tube.initPos_ + 1;
    SBVector3 dir = (endPos - initPos).normalizedVersion();

    std::map<std::pair<int, int>, ADNPointer<ADNBaseSegment>> positions;
    if (cellBsMap_.find(vs) != cellBsMap_.end()) positions = cellBsMap_.at(vs);

    SEConfig& config = SEConfig::GetInstance();

    int bs_number = tube.initPos_;
    // fix crossovers for square lattice
    double initAng = 0.0;
    if (json_.lType_ == LatticeType::Square) initAng = 7*ADNConstants::BP_ROT;

    // every tube is a double strand
    ADNPointer<ADNDoubleStrand> ds = new ADNDoubleStrand();
    nanorobot->RegisterDoubleStrand(ds);
    bool firstBs = true;
    ds->SetInitialTwistAngle(initAng);

    SBPosition3 fp = initPos;
    for (int i = 0; i < length; ++i) {
      // take into account the loops, we place for now base segments of loops in the same position as the previous base segment
      int max_iter = vs->loops_[tube.initPos_ + i];
      bool skip = vs->skips_[tube.initPos_ + i] == -1;
      if (max_iter > 0) max_iter = 1;  // a loop is contained in a base segment
      for (int k = 0; k <= max_iter; k++) {
        fp = vGrid_.GetGridCellPos3D(tube.initPos_ + i, vs->row_, vs->col_);
        double factor = 1.0;
        if ((vs->row_ + vs->col_) % 2 == 0) factor = -1.0;

        ADNPointer<ADNBaseSegment> bs = new ADNBaseSegment();
        nanorobot->RegisterBaseSegmentEnd(ds, bs);
        bs->SetNumber(bs_number);
        bs->SetPosition(fp);
        bs->SetE3(ADNAuxiliary::SBVectorToUblasVector(dir));

        if (skip) {
          bs->SetCell(new ADNSkipPair());
        }
        else if (k > 0) {
          bs->SetCell(new ADNLoopPair());
        }
        else {
          bs->SetCell(new ADNBasePair());
        }
        std::pair<int, int> key = std::make_pair(tube.initPos_ + i, k);
        positions.insert(std::make_pair(key, bs));
        ++bs_number;
      }
    }
    cellBsMap_[vs] = positions;
  }
}

void DASCadnano::CreateScaffold(ADNPointer<ADNPart> nanorobot)
{
  //look for stating point of scaffold in vstrand
  int startVstrand = json_.scaffoldStartPosition_.first;
  int startVstrandPos = json_.scaffoldStartPosition_.second;

  ADNLogger& logger = ADNLogger::GetLogger();

  //create the scaffold strand
  ADNPointer<ADNSingleStrand> scaff = new ADNSingleStrand();
  scaff->SetName("Scaffold");
  scaff->IsScaffold(true);
  nanorobot->RegisterSingleStrand(scaff);
  AddSingleStrandToMap(scaff);

  //trace scaffold through vstrands
  TraceSingleStrand(startVstrand, startVstrandPos, scaff, nanorobot);
}

void DASCadnano::CreateStaples(ADNPointer<ADNPart> nanorobot)
{
  ADNLogger& logger = ADNLogger::GetLogger();
  //find number of staples and their starting points
  std::vector<vec2> stapleStarts = json_.stapleStarts_;  //vstrand id and position on vstrand
  std::string numStaplesString;
  numStaplesString += "num of staple strands ";
  numStaplesString += to_string(stapleStarts.size());
  logger.Log(numStaplesString);

  auto& vstrands = json_.vstrands_;
  int sid = 1; //because scaffold is chain 0
  int numSkips = 0;

  for (vec2 curStapleStart : stapleStarts) {
    int vStrandId = curStapleStart.n0;
    int z = curStapleStart.n1;
    vec4 curVstrandElem = vstrands[vStrandId].stap_[z];
    
    std::map<std::pair<int, int>, ADNPointer<ADNBaseSegment>> bs_positions = cellBsMap_.at(&vstrands[vStrandId]);

    ADNPointer<ADNSingleStrand> staple = new ADNSingleStrand();
    staple->SetName("Staple" + to_string(sid));
    ++sid;
    staple->IsScaffold(false);
    nanorobot->RegisterSingleStrand(staple);
    AddSingleStrandToMap(staple);

    TraceSingleStrand(vStrandId, z, staple, nanorobot, false);
  }

  std::string msg = "numSkips (init) " + std::to_string(numSkips * 2);
  logger.Log(msg);
}

void DASCadnano::TraceSingleStrand(int startVStrand, int startVStrandPos, ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNPart> nanorobot, bool scaf)
{
  ADNLogger& logger = ADNLogger::GetLogger();
  //trace scaffold through vstrands
  auto& vstrands = json_.vstrands_;

  int vStrandId = startVStrand;
  int z = startVStrandPos;
  vec4 curVstrandElem;
  if (scaf) curVstrandElem = vstrands[startVStrand].scaf_[startVStrandPos];
  else curVstrandElem = vstrands[startVStrand].stap_[startVStrandPos];
  int count = 0;
  while (true) {

    std::map<std::pair<int, int>, ADNPointer<ADNBaseSegment>> bs_positions = cellBsMap_.at(&json_.vstrands_[vStrandId]);

    if (vstrands[vStrandId].skips_[z] != -1) {
      int max_iter = vstrands[vStrandId].loops_[z];
      bool left = true;
      Vstrand vs = vstrands[vStrandId];
      if (scaf && vStrandId % 2 == 0) left = false;
      else if (!scaf && vStrandId % 2 != 0) left = false;

      for (int k = 0; k <= max_iter; k++) {
        //add loop
        ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
        nt->Init();
        nanorobot->RegisterNucleotideFivePrime(ss, nt);
        ntPositions_.insert(std::make_pair(nt(), count));
        ++count;

        SBPosition3 pos3D = vGrid_.GetGridCellPos3D(z, vstrands[vStrandId].row_, vstrands[vStrandId].col_);

        // fetch base segment
        int p = 0;
        if (k > 0) p = 1;
        ADNPointer<ADNBaseSegment> bs = nullptr;
        std::pair<int, int> key = std::make_pair(z, p);
        if (bs_positions.find(key) != bs_positions.end()) {
          bs = bs_positions.at(key);
        }
        if (bs == nullptr) continue;

        if (k == 0) {
          // even for a loop nfirst base is always BasePair
          ADNPointer<ADNCell> c = bs->GetCell();
          if (c->GetType() == BasePair) {
            ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(c());
            if (left) bp->SetLeftNucleotide(nt);
            else bp->SetRightNucleotide(nt);

            bp->PairNucleotides();
            nt->SetBaseSegment(bs);
          }
          else {
            std::string msg = "Expected BasePair but found another cell type";
            logger.Log(msg);
          }
        }
        else {
          // We have a loop
          ADNPointer<ADNCell> c = bs->GetCell();
          if (c->GetType() == LoopPair) {
            ADNPointer<ADNLoopPair> lp = static_cast<ADNLoopPair*>(c());
            ADNPointer<ADNLoop> loop;
            if (left) loop = lp->GetLeftLoop();
            else loop = lp->GetRightLoop();

            if (loop == nullptr) {
              // first time we need to create
              loop = new ADNLoop();
              if (left) {
                lp->SetLeftLoop(loop);
                loop->SetStart(nt);
              }
              else {
                lp->SetRightLoop(loop);
                loop->SetEnd(nt);
              }
            }
            loop->AddNucleotide(nt);
            if (left) loop->SetEnd(nt);
            else loop->SetStart(nt);
            nt->SetBaseSegment(bs);
          }
          else {
            std::string msg = "Expected LoopPair but found another cell type";
            logger.Log(msg);
          }
        }

        nt->SetPosition(pos3D);
        nt->SetBackbonePosition(pos3D);
        nt->SetSidechainPosition(pos3D);
      }
    }

    if (curVstrandElem.n0 != -1 && curVstrandElem.n1 != -1 && curVstrandElem.n2 == -1 && curVstrandElem.n3 == -1) {
      break;
    }

    //find next scaf element
    auto nextVstrand = json_.vstrands_[curVstrandElem.n2];
    vec4 nextVstrandElem;
    if (scaf) nextVstrandElem = nextVstrand.scaf_[curVstrandElem.n3];
    else nextVstrandElem = nextVstrand.stap_[curVstrandElem.n3];

    vStrandId = curVstrandElem.n2;
    z = curVstrandElem.n3;
    curVstrandElem = nextVstrandElem;
  }
}

void DASCadnano::CreateConformations(ADNPointer<ADNPart> nanorobot)
{
  std::string name = json_.name_;
  SBNodeIndexer nodeIndexer;
  nanorobot->getNodes(nodeIndexer, (SBNode::GetClass() == std::string("ADNAtom")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  conformation3D_ = new SBMStructuralModelConformation(name + " 3D", nodeIndexer);
  conformation2D_ = new SBMStructuralModelConformation(name + " 2D", nodeIndexer);
  conformation1D_ = new SBMStructuralModelConformation(name + " 1D", nodeIndexer);

  SBPosition3 center3D = SBPosition3();
  //SB_FOR(SBNode* n, nodeIndexer) {
  //  SBAtom* atom = static_cast<SBAtom*>(n);
  //  center3D += atom->getPosition();
  //}
  //center3D /= nodeIndexer.size();

  SBPosition3 center2D = SBPosition3();
  SBPosition3 center1D = SBPosition3();

  int n1D = 0;
  int n3D = 0;
  for (auto it = cellBsMap_.begin(); it != cellBsMap_.end(); ++it) {
    Vstrand* vs = it->first;
    int vStrandId = vs->num_;
    std::map<std::pair<int, int>, ADNPointer<ADNBaseSegment>> values = it->second;

    for (auto jt = values.begin(); jt != values.end(); ++jt) {
      std::pair<int, int> bsNumAndLoop = jt->first;
      ADNPointer<ADNBaseSegment> bs = jt->second;
      int z = bsNumAndLoop.first;
      auto nts = bs->GetNucleotides();
      SB_FOR(ADNPointer<ADNNucleotide> nt, nts) {
        ADNPointer<ADNSingleStrand> ss = nt->GetStrand();
        SBPosition3 pos2D = vGrid_.GetGridCellPos2D(vStrandId, z, ss->IsScaffold());
        SBPosition3 pos1D = vGrid_.GetGridCellPos1D(ssId_[ss()], ntPositions_[nt()]);
        auto ats = nt->GetBackbone()->GetAtoms();
        SB_FOR(ADNPointer<ADNAtom> at, ats) {
          if (!ss->IsScaffold()) {
            center1D += pos1D;
            n1D++;
          }
          center2D += pos2D;
          center3D += nt->GetBackbonePosition();
          n3D++;
          conformation2D_->setPosition(at(), pos2D);
          conformation1D_->setPosition(at(), pos1D);
        }
        ats = nt->GetSidechain()->GetAtoms();
        SB_FOR(ADNPointer<ADNAtom> at, ats) {
          if (!ss->IsScaffold()) {
            center1D += pos1D;
            n1D++;
          }
          center2D += pos2D;
          center3D += nt->GetSidechainPosition();
          n3D++;
          conformation2D_->setPosition(at(), pos2D);
          conformation1D_->setPosition(at(), pos1D);
        }
      }
    }
  }

  center1D /= n1D;
  center2D /= n3D;
  center3D /= n3D;

  for (auto it = cellBsMap_.begin(); it != cellBsMap_.end(); ++it) {
    std::map<std::pair<int, int>, ADNPointer<ADNBaseSegment>> values = it->second;
    for (auto jt = values.begin(); jt != values.end(); ++jt) {
      ADNPointer<ADNBaseSegment> bs = jt->second;
      auto nts = bs->GetNucleotides();
      SB_FOR(ADNPointer<ADNNucleotide> nt, nts) {
        auto ats = nt->GetBackbone()->GetAtoms();
        SB_FOR(ADNPointer<ADNAtom> at, ats) {
          SBPosition3 pos2D;
          conformation2D_->getPosition(at(), pos2D);
          pos2D -= center2D;
          pos2D += center3D;
          conformation2D_->setPosition(at(), pos2D);
          SBPosition3 pos1D;
          conformation1D_->getPosition(at(), pos1D);
          pos1D -= center1D;
          pos1D += center3D;
          conformation1D_->setPosition(at(), pos1D);
        }
        ats = nt->GetSidechain()->GetAtoms();
        SB_FOR(ADNPointer<ADNAtom> at, ats) {
          SBPosition3 pos2D;
          conformation2D_->getPosition(at(), pos2D);
          pos2D -= center2D;
          pos2D += center3D;
          conformation2D_->setPosition(at(), pos2D);
          SBPosition3 pos1D;
          conformation1D_->getPosition(at(), pos1D);
          pos1D -= center1D;
          pos1D += center3D;
          conformation1D_->setPosition(at(), pos1D);
        }
      }
    }
  }

  //shift 2D and 1D position to center of 3D
  //for (unsigned int i = 0; i < n; i++) {
  //  SBPosition3 pos2D;
  //  conformation2D_->getPosition(i, pos2D);
  //  pos2D -= center2D;
  //  pos2D += center3D;
  //  conformation2D_->setPosition(i, pos2D);
  //}


}

SBPointer<SBMStructuralModelConformation> DASCadnano::Get3DConformation()
{
  return conformation3D_;
}

SBPointer<SBMStructuralModelConformation> DASCadnano::Get2DConformation()
{
  return conformation2D_;
}

SBPointer<SBMStructuralModelConformation> DASCadnano::Get1DConformation()
{
  return conformation1D_;
}

ADNPointer<ADNPart> DASCadnano::CreateCadnanoPart(std::string file)
{
  ParseJSON(file);
  return CreateCadnanoModel();
}

DNABlocks DASCadnano::GetComplementaryBase(DNABlocks type) {
  return ADNModel::GetComplementaryBase(type);
}

bool DASCadnano::IsThereBase(vec4 data) {
  bool base = false;

  if ((data.n0 != -1 && data.n1 != -1) || (data.n2 != -1 && data.n3 != -1)) base = true;

  return base;
}

void DASCadnano::AddSingleStrandToMap(ADNPointer<ADNSingleStrand> ss)
{
  int key = lastKey + 1;
  ssId_.insert(std::make_pair(ss(), key));
  lastKey = key;
}

void VGrid::CreateLattice(LatticeType lType)
{
  // in cadnano 2.0 square lattice is 50x50, honeycomb is 30 x 32
  int maxRows = 55;
  int maxCols = 55;
  if (lType == LatticeType::Honeycomb) {
    maxRows = 30;
    maxCols = 32;
  }

  lattice_ = DASLattice(lType, dh_diameter_, maxRows, maxCols);
}

void VGrid::AddTube(VTube tube)
{
  vDoubleStrands_.push_back(tube);
}

SBPosition3 VGrid::GetGridCellPos3D(int z, unsigned int row, unsigned int column)
{
  auto zPos = z * bp_rise_;
  LatticeCell lc = lattice_.GetLatticeCell(row, column);
  SBPosition3 pos = SBPosition3(SBQuantity::nanometer(zPos), SBQuantity::nanometer(lc.x_), SBQuantity::nanometer(lc.y_));
  return pos;
}

SBPosition3 VGrid::GetGridCellPos2D(int vStrandId, int z, bool isScaffold)
{
  SBPosition3 pos;
  double hPos = vStrandId * ADNConstants::DH_DIAMETER;
  double zPos = z * ADNConstants::BP_RISE;
  
  if (vStrandId % 2 == 0) {
    if (!isScaffold) hPos += ADNConstants::BP_CADNANO_DIST;
  }
  else {
    if (isScaffold) hPos += ADNConstants::BP_CADNANO_DIST;
  }

  pos[0] = SBQuantity::nanometer(zPos);
  pos[1] = SBQuantity::nanometer(hPos);
  pos[2] = SBQuantity::nanometer(0.0);

  return pos;
}

SBPosition3 VGrid::GetGridCellPos1D(int ssId, int ntNum)
{
  SBPosition3 pos;

  double x = ssId * ADNConstants::DH_DIAMETER * 0.25f;
  double y = -ntNum / 2.0f;

  pos[0] = SBQuantity::nanometer(x);
  pos[1] = SBQuantity::nanometer(y);
  pos[2] = SBQuantity::nanometer(0.0);

  return pos;
}
