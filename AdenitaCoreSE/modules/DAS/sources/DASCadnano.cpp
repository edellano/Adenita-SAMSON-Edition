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

    totalCount = count + 1;  // all vhelix have the same count
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
  part->SetName(json_.name_);

  CreateEdgeMap(part);
  CreateScaffold(part);
  CreateStaples(part);

  return part;
}

void DASCadnano::CreateEdgeMap(ADNPointer<ADNPart> nanorobot)
{
  auto tubes = vGrid_.vDoubleStrands_;
  for (auto &tube : tubes) {
    // every tube is a double strand
    ADNPointer<ADNDoubleStrand> ds = new ADNDoubleStrand();
    nanorobot->RegisterDoubleStrand(ds);
    bool firstBs = true;

    Vstrand* vs = &json_.vstrands_[tube.vStrandId_];
    SBPosition3 initPos = vGrid_.GetGridCellPos3D(tube.vStrandId_, tube.initPos_, vs->row_, vs->col_);
    SBPosition3 endPos = vGrid_.GetGridCellPos3D(tube.vStrandId_, tube.endPos_, vs->row_, vs->col_);
    int length = tube.endPos_ - tube.initPos_ + 1;
    SBVector3 dir = (endPos - initPos).normalizedVersion();

    std::map<std::pair<int, int>, ADNPointer<ADNBaseSegment>> positions;
    if (cellBsMap_.find(vs) != cellBsMap_.end()) positions = cellBsMap_.at(vs);

    SEConfig& config = SEConfig::GetInstance();
    int magic_number = config.magic_number;  //  this is a magic number to fix crossovers

    int bs_number = tube.initPos_ + magic_number;
    if ((vs->row_ + vs->col_) % 2 != 0) {
      bs_number += 6;
    }

    SBPosition3 fp = initPos;
    for (int i = 0; i < length; ++i) {
      // take into account the loops, we place for now base segments of loops in the same position as the previous base segment
      int max_iter = vs->loops_[tube.initPos_ + i];
      if (max_iter > 0) max_iter = 1;  // a loop is contained in a base segment
      for (int k = 0; k <= max_iter; k++) {
        fp = vGrid_.GetGridCellPos3D(tube.vStrandId_, tube.initPos_ + i, vs->row_, vs->col_);
        double factor = 1.0;
        if ((vs->row_ + vs->col_) % 2 == 0) factor = -1.0;

        ADNPointer<ADNBaseSegment> bs = new ADNBaseSegment();
        nanorobot->RegisterBaseSegmentEnd(ds, bs);
        bs->SetNumber(bs_number);
        bs->SetPosition(fp);
        bs->SetE3(ADNAuxiliary::SBVectorToUblasVector(dir));

        if (k > 0) {
          bs->SetCell(new ADNLoopPair());
          // for now we place base segments in the same position
        }
        else {
          bs->SetCell(new ADNBasePair());
        }
        std::pair<int, int> key = std::make_pair(tube.initPos_ + i, k);
        positions.insert(std::make_pair(key, bs));
      }
      ++bs_number;
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

  //trace scaffold through vstrands
  TraceSingleStrand(startVstrand, startVstrandPos, scaff, nanorobot);
}

void DASCadnano::CreateStaples(ADNPointer<ADNPart> nanorobot)
{
  ADNLogger& logger = ADNLogger::GetLogger();
  //find number of staples and their starting points
  std::vector<vec2> stapleStarts = json_.stapleStarts_; //vstrand id and position on vstrand
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

    TraceSingleStrand(vStrandId, z, staple, nanorobot, false);
  }

  std::string msg = "numSkips (init) " + std::to_string(numSkips * 2);
  logger.Log(msg);
}

void DASCadnano::TraceSingleStrand(int startVStrand, int startVStrandPos, ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNPart> nanorobot, bool left)
{
  ADNLogger& logger = ADNLogger::GetLogger();
  //trace scaffold through vstrands
  auto& vstrands = json_.vstrands_;
  vec4 curVstrandElem;
  if (left) curVstrandElem = vstrands[startVStrand].scaf_[startVStrandPos];
  else curVstrandElem = vstrands[startVStrand].stap_[startVStrandPos];

  int lastElem = 0;
  bool last_it = false;
  while (true) {
    int vStrandId = curVstrandElem.n2;
    int z = curVstrandElem.n3;

    //find next scaf element and check if it is last
    auto nextVstrand = json_.vstrands_[curVstrandElem.n2];
    vec4 nextVstrandElem;
    if (left) nextVstrandElem = nextVstrand.scaf_[curVstrandElem.n3];
    else nextVstrandElem = nextVstrand.stap_[curVstrandElem.n3];

    if (nextVstrandElem.n0 != -1 && nextVstrandElem.n1 != -1 && nextVstrandElem.n2 == -1 && nextVstrandElem.n3 == -1) {
      last_it = true;
    }

    std::map<std::pair<int, int>, ADNPointer<ADNBaseSegment>> bs_positions = cellBsMap_.at(&json_.vstrands_[vStrandId]);

    if (vstrands[vStrandId].skips_[z] == -1) {
      ADNPointer<ADNBaseSegment> bs = nullptr;
      std::pair<int, int> key = std::make_pair(z, 0);
      if (bs_positions.find(key) != bs_positions.end()) {
        bs = bs_positions.at(key);
      }
      if (bs != nullptr) {
        ADNPointer<ADNSkipPair> skipPair = static_cast<ADNSkipPair*>(bs->GetCell()());
        if (skipPair == nullptr) {
          skipPair = new ADNSkipPair();
          bs->SetCell(skipPair());
        }
        
        ADNPointer<ADNNucleotide> skip = new ADNNucleotide();

        SBPosition3 leftPos3D = vGrid_.GetGridCellPos3D(vStrandId, z, vstrands[vStrandId].row_, vstrands[vStrandId].col_);

        skip->SetPosition(leftPos3D);
        skip->SetBackbonePosition(leftPos3D);
        skip->SetSidechainPosition(leftPos3D);

        if (left) skipPair->SetLeftSkip(skip);
        else skipPair->SetRightSkip(skip);

        skip->SetBaseSegment(bs);
      }
    }
    else {
      int max_iter = vstrands[vStrandId].loops_[z];
      for (int k = 0; k <= max_iter; k++) {
        //add loop
        ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
        nanorobot->RegisterNucleotideThreePrime(ss, nt);

        SBPosition3 pos3D = vGrid_.GetGridCellPos3D(vStrandId, z, vstrands[vStrandId].row_, vstrands[vStrandId].col_);

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
              if (left) lp->SetLeftLoop(loop);
              else lp->SetRightLoop(loop);

              loop->SetStart(nt);
            }
            loop->AddNucleotide(nt);
            loop->SetEnd(nt);
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

    curVstrandElem = nextVstrandElem;
    lastElem = z;

    if (last_it) {
      break;
    }
  }
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

void Lattice::CreateSquareLattice()
{
  mat_ = ublas::matrix<LatticeCell>(55, 55); // in cadnano 2.0 square lattice is 50x50, honeycomb is 30 x 32

  for (unsigned row = 0; row < mat_.size1(); ++row) {
    for (unsigned column = 0; column < mat_.size2(); ++column) {
      //todo
      LatticeCell lc;
      lc.x_ = row * dh_diameter_;
      lc.y_ = column * dh_diameter_;
      mat_(row, column) = lc;
    }
  }
}

void Lattice::CreateHoneycombLattice()
{
  mat_ = ublas::matrix<LatticeCell>(30, 32); // in cadnano 2.0 square lattice is 50x50, honeycomb is 30 x 32
  double angle = 120.0;  // deg
  double delt_y = dh_diameter_ * sin(ADNVectorMath::DegToRad(angle - 90)) * 0.5;
  double delt_x = dh_diameter_ * cos(ADNVectorMath::DegToRad(angle - 90));
  double offset = delt_y;

  for (unsigned int row = 0; row < mat_.size1(); ++row) {
    for (unsigned int column = 0; column < mat_.size2(); ++column) {
      double off = offset;
      if ((row + column) % 2 == 0) off *= -1;
      LatticeCell lc;
      lc.x_ = column * delt_x;
      lc.y_ = row * (dh_diameter_ + 2 * delt_y) + off;
      mat_(row, column) = lc;
    }
  }
}

LatticeCell Lattice::GetLatticeCell(unsigned int row, unsigned int column)
{
  size_t rSize = mat_.size1();
  size_t cSize = mat_.size2();
  ADNLogger& logger = ADNLogger::GetLogger();
  if (row >= rSize || column >= cSize) {
    std::string msg = "Lattice overflow. Probably worng lattice type was selected.";
    logger.Log(msg);
  }
  return mat_(row, column);
}

size_t Lattice::GetNumberRows()
{
  return mat_.size1();
}

size_t Lattice::GetNumberCols()
{
  return mat_.size2();
}

void VGrid::CreateLattice(LatticeType lType)
{
  lattice_ = Lattice();
  if (lType == LatticeType::Honeycomb) {
    lattice_.CreateHoneycombLattice();
  }
  else {
    lattice_.CreateSquareLattice();
  }
}

void VGrid::AddTube(VTube tube)
{
  vDoubleStrands_.push_back(tube);
}

SBPosition3 VGrid::GetGridCellPos3D(int vStrandId, int z, unsigned int row, unsigned int column)
{
  auto zPos = z * bp_rise_;
  LatticeCell lc = lattice_.GetLatticeCell(row, column);
  SBPosition3 pos = SBPosition3(SBQuantity::nanometer(zPos), SBQuantity::nanometer(lc.x_), SBQuantity::nanometer(lc.y_));
  return pos;
}
