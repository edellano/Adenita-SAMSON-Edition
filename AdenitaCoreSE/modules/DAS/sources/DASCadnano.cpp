#include "DASCadnano.hpp"

void DASCadnano::ParseJSON(string file) {
  //all fields are parsed in the same order as in the original cadnano file format (ver. 2.0)

  FILE* fp;
  fopen_s(&fp, file.c_str(), "rb");
  char readBuffer[65536];
  FileReadStream is(fp, readBuffer, sizeof(readBuffer));
  Document d;
  d.ParseStream(is);

  ADNLogger& logger = ADNLogger::GetLogger();

  json_.name_ = d["name"].GetString();
  logger.Log(string("name: " + json_.name_));

  Value& vstrandsVal = d["vstrands"];
  //logger.Log(string("vstrands"));
  
  for (unsigned int i = 0; i < vstrandsVal.Size(); i++) {
    Vstrand vstrand;

    Value& vstrandVal = vstrandsVal[i];
        
    //stap_colors
    Value& stapColorsVals = vstrandVal["stap_colors"];
    //logger.Log(string("\t stap_colors"));

    for (unsigned int j = 0; j < stapColorsVals.Size(); j++) {
      //logger.Log(string("\t \t" + to_string(j)));
      Value& stapColorsVal = stapColorsVals[j];

      vec2 elem;
      elem.n0 = stapColorsVal[0].GetInt();
      elem.n1 = stapColorsVal[1].GetInt();
      vstrand.stap_colors_.push_back(elem);

      //logger.Log(string("\t \t \t" + to_string(0) + " : " + to_string(vstrand.stap_colors_[j].n0)));
      //logger.Log(string("\t \t \t" + to_string(1) + " : " + to_string(vstrand.stap_colors_[j].n1)));
    }

    //num
    vstrand.num_ = vstrandVal["num"].GetInt();
    json_.numToIdMap_.insert(make_pair(vstrand.num_, i));
    //logger.Log(string("\t num: " + to_string(vstrand.num_)));

    //scafLoop todo

    //stap
    Value& stapVals = vstrandVal["stap"];
    //logger.Log(string("\t stap"));
    for (unsigned int j = 0; j < stapVals.Size(); j++) {
      //logger.Log(string("\t \t" + to_string(j)));
      Value& stapVal = stapVals[j];
      vec4 elem;
      elem.n0 = stapVal[0].GetInt();
      elem.n1 = stapVal[1].GetInt();
      elem.n2 = stapVal[2].GetInt();
      elem.n3 = stapVal[3].GetInt();
      vstrand.stap_.push_back(elem);

      //logger.Log(string("\t \t \t" + string("0: ") + to_string(vstrand.stap_[j].n0)));
      //logger.Log(string("\t \t \t" + string("1: ") + to_string(vstrand.stap_[j].n1)));
      //logger.Log(string("\t \t \t" + string("2: ") + to_string(vstrand.stap_[j].n2)));
      //logger.Log(string("\t \t \t" + string("3: ") + to_string(vstrand.stap_[j].n3)));

    }

    //skip
    Value& skipVals = vstrandVal["skip"];
    //logger.Log(string("\t skip"));
    for (unsigned int j = 0; j < skipVals.Size(); j++) {
      Value& skipVal = skipVals[j];
      vstrand.skip_.push_back(skipVals[j].GetInt());

      //logger.Log(string("\t \t \t" + to_string(j) + string(": ")  + to_string(vstrand.skip_[j])));
    }

    //scaf
    Value& scafVals = vstrandVal["scaf"];
    //logger.Log(string("\t scaf"));
    for (unsigned int j = 0; j < scafVals.Size(); j++) {
      //logger.Log(string("\t \t" + to_string(j)));
      Value& scafVal = scafVals[j];
      vec4 elem;
      elem.n0 = scafVal[0].GetInt();
      elem.n1 = scafVal[1].GetInt();
      elem.n2 = scafVal[2].GetInt();
      elem.n3 = scafVal[3].GetInt();
      vstrand.scaf_.push_back(elem);

      //logger.Log(string("\t \t \t" + string("0: ") + to_string(vstrand.scaf_[j].n0)));
      //logger.Log(string("\t \t \t" + string("1: ") + to_string(vstrand.scaf_[j].n1)));
      //logger.Log(string("\t \t \t" + string("2: ") + to_string(vstrand.scaf_[j].n2)));
      //logger.Log(string("\t \t \t" + string("3: ") + to_string(vstrand.scaf_[j].n3)));

    }

    //col
    vstrand.col_ = vstrandVal["col"].GetInt();
    //logger.Log(string("\t col: " + to_string(vstrand.col_)));

    //stapLoop todo

    //row
    vstrand.row_ = vstrandVal["row"].GetInt();
    //logger.Log(string("\t row: " + to_string(vstrand.row_)));

    //loop
    Value& loopVals = vstrandVal["loop"];
    //logger.Log(string("\t loop"));
    for (unsigned int j = 0; j < loopVals.Size(); j++) {
      Value& loopVal = loopVals[j];
      vstrand.loop_.push_back(loopVals[j].GetInt());
      
      //logger.Log(string("\t \t \t" + to_string(j) + string(": ") + to_string(vstrand.loop_[j])));
    }

    json_.vstrands_.push_back(vstrand);

  }
  
  fclose(fp);
}

void DASCadnano::WriteJSON()
{
  StringBuffer s;
  Writer<StringBuffer> writer(s);

  writer.StartObject();               // Between StartObject()/EndObject(), 

  //name
  writer.Key("name");
  writer.String(json_.name_.c_str());

  //vstrands
  writer.Key("vstrands");
  writer.StartArray();

  for (auto vstrand : json_.vstrands_) {
    writer.StartObject();
    //stap_colors
    writer.Key("stap_colors");
    writer.StartArray();
    for (int j = 0; j < vstrand.stap_colors_.size(); j++) {
      writer.StartArray();
      writer.Int(vstrand.stap_colors_[j].n0);
      writer.Int(vstrand.stap_colors_[j].n1);
      writer.EndArray();
    }
    writer.EndArray();
    
    //num
    writer.Key("num");
    writer.Int(vstrand.num_);
    
    //scafLoop
    writer.Key("scafLoop");
    writer.StartArray(); //todo
    writer.EndArray(); //todo

    //stap
    writer.Key("stap");
    writer.StartArray();
    for (auto stap : vstrand.stap_) {
      writer.StartArray();
      writer.Int(stap.n0);
      writer.Int(stap.n1);
      writer.Int(stap.n2);
      writer.Int(stap.n3);
      writer.EndArray();
    }
    writer.EndArray();

    //skip
    writer.Key("skip");
    writer.StartArray();
    for (auto skip : vstrand.skip_) {
      writer.Int(skip);
    }
    writer.EndArray();

    //scaf
    writer.Key("scaf");
    writer.StartArray();
    for (auto scaf : vstrand.scaf_) {
      writer.StartArray();
      writer.Int(scaf.n0);
      writer.Int(scaf.n1);
      writer.Int(scaf.n2);
      writer.Int(scaf.n3);
      writer.EndArray();
    }
    writer.EndArray();

    //stap_loop
    writer.Key("stapLoop");
    writer.StartArray(); //todo
    writer.EndArray(); //todo

    //col
    writer.Key("col");
    writer.Int(vstrand.col_);

    //loop
    writer.Key("loop");
    writer.StartArray();
    for (auto loop : vstrand.loop_) {
      writer.Int(loop);
    }
    writer.EndArray();

    //row
    writer.Key("row");
    writer.Int(vstrand.row_);

    writer.EndObject();
  }
  
  writer.EndArray();

  writer.EndObject();
  

  QIODevice::OpenModeFlag mode = QIODevice::WriteOnly;
  string filename = SAMSON::getUserDataPath() + "/cylinder.json";
  QFile file(filename.c_str());
  file.open(mode);

  QTextStream out(&file);

  out << s.GetString();

  file.close();

}

void DASCadnano::CreateModel(ADNPointer<ADNPart> nanorobot, string seq, LatticeType lattice) {
  //create the grid for fetching the position
  size_t numVStrands = json_.vstrands_.size();
  size_t numZ = json_.vstrands_[0].scaf_.size();
  vGrid_.ConstructGridAndLattice(numVStrands, numZ, lattice, json_);
  //ConstructBaseSegments(nanorobot);

  //create the scaffold strand
  ADNPointer<ADNSingleStrand> scaff = new ADNSingleStrand();
  scaff->SetName("Scaffold");
  scaff->IsScaffold(true);
  nanorobot->RegisterSingleStrand(scaff);

  CreateEdgeMap(nanorobot);
  CreateScaffold(scaff, seq, nanorobot);

  //create the staple strands
  //find the number of staple
  CreateStaples(nanorobot);

  //ShiftStructure(nanorobot);

  //nanorobot.SetSingleStrands(chains_);
  //nanorobot.SetScaffold(scaff->id_);
}

void DASCadnano::CreateEdgeMap(ADNPointer<ADNPart> nanorobot) {
  // loop over vStrands to find out how many tubes we have
  // tubes are described by: vStrand id, initial position in the vHelix, end position in the vHelix
  //   direction of the vHelix in 3D, 2D and 1D
  std::vector<std::tuple<int, int, int, SBVector3, SBVector3, SBVector3>> tubes;
  int vStrandId = 0;
  for (auto &vstrand : json_.vstrands_) {
    size_t total_length = vstrand.scaf_.size();
    bool start_tube = false;
    int init_pos = -1;
    int end_pos = -1;
    for (int i = 0; i < total_length; ++i) {
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
          Vstrand* vs = &json_.vstrands_[vStrandId];
          SBPosition3 start_vec = vGrid_.GetGridScaffoldCellPos3D(vStrandId, init_pos, vs->row_, vs->col_);
          SBPosition3 end_vec = vGrid_.GetGridScaffoldCellPos3D(vStrandId, end_pos, vs->row_, vs->col_);
          SBVector3 dir = (end_vec - start_vec).normalizedVersion();
          start_vec = vGrid_.GetGridScaffoldCellPos2D(vStrandId, init_pos);
          end_vec = vGrid_.GetGridScaffoldCellPos2D(vStrandId, end_pos);
          SBVector3 dir2D = (end_vec - start_vec).normalizedVersion();
          start_vec = vGrid_.GetGridScaffoldPos1D(init_pos);
          end_vec = vGrid_.GetGridScaffoldPos1D(end_pos);
          SBVector3 dir1D = (end_vec - start_vec).normalizedVersion();
          tubes.push_back(std::make_tuple(vStrandId, init_pos, end_pos, dir, dir2D, dir1D));
          init_pos = -1;
        }
      }
    }
    ++vStrandId;
  }
  // create the tubes
  int j_id = 0;
  int bs_id = 0;
  
  for (auto &tube : tubes) {
    // every tube is a double strand
    ADNPointer<ADNDoubleStrand> ds = new ADNDoubleStrand();
    nanorobot->RegisterDoubleStrand(ds);
    bool firstBs = true;

    // offset for joint positions
    SBVector3 dir = std::get<3>(tube);
    SBVector3 dir2D = std::get<4>(tube);
    // not being currently used since for 1D we also use 2D coordinates.
    SBVector3 dir1D = std::get<5>(tube);

    Vstrand* vs = &json_.vstrands_[std::get<0>(tube)];
    SBPosition3 fp = vGrid_.GetGridScaffoldCellPos3D(std::get<0>(tube), std::get<1>(tube), vs->row_, vs->col_);
    SBPosition3 fp2D = vGrid_.GetGridScaffoldCellPos2D(std::get<0>(tube), std::get<1>(tube));
    int count = 0;
    SBPosition3 fp1D = vGrid_.GetGridScaffoldCellPos2D(std::get<0>(tube), count);  // 1D always starts at 0
    fp1D[0] = -fp1D[0];  // for displaying purposes we invert the x-axis

    int length = std::get<2>(tube) - std::get<1>(tube) + 1;

    std::map<std::pair<int, int>, ADNPointer<ADNBaseSegment>> positions;
    if (cellBsMap_.find(vs) != cellBsMap_.end()) positions = cellBsMap_.at(vs);

    SEConfig& config = SEConfig::GetInstance();
    int magic_number = config.magic_number;  //  this is a magic number to fix crossovers

    int bs_number = std::get<1>(tube) + magic_number;
    if ((vs->row_ + vs->col_) % 2 != 0) {
      bs_number += 6;
    }

    for (int i=0; i<length; ++i) {
      // take into account the loops, we place for now base segments of loops in the same position as the previous base segment
      int max_iter = json_.vstrands_[std::get<0>(tube)].loop_[std::get<1>(tube) + i];
      if (max_iter > 0) max_iter = 1;  // a loop is contained in a base segment
      for (int k = 0; k <= max_iter; k++) {
        fp = vGrid_.GetGridScaffoldCellPos3D(std::get<0>(tube), std::get<1>(tube) + i, vs->row_, vs->col_);
        fp2D = vGrid_.GetGridScaffoldCellPos2D(std::get<0>(tube), std::get<1>(tube) + i);
        fp1D = vGrid_.GetGridScaffoldCellPos2D(std::get<0>(tube), count);
        fp1D[0] = -fp1D[0];  // for displaying purposes we invert the x-axis
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
        std::pair<int, int> key = std::make_pair(std::get<1>(tube) + i, k);
        positions.insert(std::make_pair(key, bs));
      }
      ++count;
      ++bs_number;
    }
    cellBsMap_[vs] = positions;
  }
}

void DASCadnano::CreateScaffold(ADNPointer<ADNSingleStrand> scaffold, string seq, ADNPointer<ADNPart> nanorobot) {

  //look for stating point of scaffold in vstrand
  int startVstrand = 0;
  int startVstrandPos = 0;
  vec4 curVstrandElem;
  curVstrandElem.n0 = -1;
  curVstrandElem.n1 = -1;

  ADNLogger& logger = ADNLogger::GetLogger();

  int vStrandId = 0;
  for (auto &vstrand : json_.vstrands_) {

    //starting point of scaffold in vstrand must be 0 == -1, 1 == -1, 2 != -1, 3 != -1 
    //multiple scaffolds per cadnano are not considered yet
    int posAtVstrand = 0;

    // skip empty vstrands
    if (cellBsMap_.find(&vstrand) == cellBsMap_.end()) continue;
    std::map<std::pair<int, int>, ADNPointer<ADNBaseSegment>> bs_positions = cellBsMap_.at(&vstrand);
    for (auto scaf : vstrand.scaf_) {
      
      //start point
      if (scaf.n0 == -1 && scaf.n1 == -1 && scaf.n2 != -1 && scaf.n3 != -1) {
        
        startVstrand = vStrandId;
        startVstrandPos = posAtVstrand;

        curVstrandElem.n2 = scaf.n2;
        curVstrandElem.n3 = scaf.n3;

        //add first nucleotide to scaffold chain
        ADNPointer<ADNNucleotide> nt = new ADNNucleotide();

        int z = startVstrandPos;
        //SBPosition3 pos1D = vGrid_.GetGridScaffoldPos1D(nt->id_);
        //SBPosition3 pos2D = vGrid_.GetGridScaffoldCellPos2D(vStrandId, z);
        SBPosition3 pos3D = vGrid_.GetGridScaffoldCellPos3D(vStrandId, z, vstrand.row_, vstrand.col_);

        //nt->SetPosition1D(pos1D);
        //nt->SetPosition2D(pos2D);
        nt->SetPosition(pos3D);
        nt->SetBackbonePosition(pos3D);
        nt->SetSidechainPosition(pos3D);

        auto helix_pos_loop = std::make_tuple(vStrandId, z, 0);
        scaffold_nucleotides_.insert(std::make_pair(helix_pos_loop, nt));
        ADNPointer<ADNBaseSegment> bs = nullptr;
        std::pair<int, int> key = std::make_pair(posAtVstrand, 0);
        if (bs_positions.find(key) != bs_positions.end()) {
          bs = bs_positions.at(key);
        }
        if (bs != nullptr) {
          // first one can't be a loop
          ADNPointer<ADNCell> c = bs->GetCell();
          if (c->GetType() == BasePair) {
            ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(c());
            bp->SetLeftNucleotide(nt);
            nt->SetBaseSegment(bs);
          }
          else {
            std::string msg = "Expected BasePair but found another cell type";
            logger.Log(msg);
          }
        }

        //logger.Log(string("found starting point of scaffold at " + to_string(startVstrand) + " " + to_string(startVstrandPos)));
        break;
      }
     
      ++posAtVstrand;
    }
    ++vStrandId;

  }

  //trace scaffold through vstrands
  int nid = 1; // because the starting nucleotide is already in the map
  auto vstrands = json_.vstrands_;
  int lastElem = 0;
  bool last_it = false;
  while (true) {
    int vStrandId = json_.numToIdMap_[curVstrandElem.n2];
    int z = curVstrandElem.n3;
    std::map<std::pair<int, int>, ADNPointer<ADNBaseSegment>> bs_positions = cellBsMap_.at(&json_.vstrands_[vStrandId]);
    if (vstrands[vStrandId].skip_[z] == -1) { 
      //add skips
      auto nextVstrand = vstrands[json_.numToIdMap_[curVstrandElem.n2]];
      vec4 nextVstrandElem = nextVstrand.scaf_[curVstrandElem.n3];

      std::string msg = "scaffold skip " + std::to_string(vStrandId);
      logger.Log(msg);
      logger.Log(std::to_string(z));
      
      ADNPointer<ADNBaseSegment> bs = nullptr;
      std::pair<int, int> key = std::make_pair(z, 0);
      if (bs_positions.find(key) != bs_positions.end()) {
        bs = bs_positions.at(key);
      }
      if (bs != nullptr) {
        ADNPointer<ADNSkipPair> skipPair = new ADNSkipPair();
        ADNPointer<ADNNucleotide> leftSkip = new ADNNucleotide();
        
        SBPosition3 leftPos1D = vGrid_.GetGridScaffoldPos1D(nid);
        SBPosition3 leftPos2D = vGrid_.GetGridScaffoldCellPos2D(vStrandId, z);
        SBPosition3 leftPos3D = vGrid_.GetGridScaffoldCellPos3D(vStrandId, z, vstrands[vStrandId].row_, vstrands[vStrandId].col_);

        //leftSkip->SetPosition1D(leftPos1D);
        //leftSkip->SetPosition2D(leftPos2D);
        leftSkip->SetPosition(leftPos3D);
        leftSkip->SetBackbonePosition(leftPos3D);
        leftSkip->SetSidechainPosition(leftPos3D);

        skipPair->SetLeftSkip(leftSkip);

        bs->SetCell(skipPair());
        leftSkip->SetBaseSegment(bs);
      }

      curVstrandElem = nextVstrandElem;
      lastElem = z;

      continue;
    }

    ////find next scaf element
    auto nextVstrand = vstrands[json_.numToIdMap_[curVstrandElem.n2]];
    vec4 nextVstrandElem = nextVstrand.scaf_[curVstrandElem.n3];

    if (nextVstrandElem.n0 != -1 && nextVstrandElem.n1 != -1 && nextVstrandElem.n2 == -1 && nextVstrandElem.n3 == -1) {
      last_it = true;
    }

    int max_iter = vstrands[vStrandId].loop_[z];
    //if (max_iter > 0) --max_iter;
    for (int k = 0; k <= max_iter; k++) {
      //add loop
      ADNPointer<ADNNucleotide> nt = new ADNNucleotide();

      //SBPosition3 pos1D = vGrid_.GetGridScaffoldPos1D(nt->id_);
      SBPosition3 pos2D = vGrid_.GetGridScaffoldCellPos2D(vStrandId, z);
      SBPosition3 pos3D = vGrid_.GetGridScaffoldCellPos3D(vStrandId, z, vstrands[vStrandId].row_, vstrands[vStrandId].col_);

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
          bp->SetLeftNucleotide(nt);
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
          ADNPointer<ADNLoop> left = lp->GetLeftLoop();
          if (left == nullptr) {
            // first time we need to create
            left = new ADNLoop();
            lp->SetLeftLoop(left);
            left->SetStart(nt);
          }
          left->AddNucleotide(nt);
          left->SetEnd(nt);
          nt->SetBaseSegment(bs);
        }
        else {
          std::string msg = "Expected LoopPair but found another cell type";
          logger.Log(msg);
        }
      }
      
      //nt->SetPosition1D(pos1D);
      //nt->SetPosition2D(pos2D);
      nt->SetPosition(pos3D);
      nt->SetBackbonePosition(pos3D);
      nt->SetSidechainPosition(pos3D);
      nanorobot->RegisterNucleotideThreePrime(scaffold, nt);
      auto helix_pos_loop = std::make_tuple(vStrandId, z, k);
      scaffold_nucleotides_.insert(std::make_pair(helix_pos_loop, nt));

      nid++;
    }

    curVstrandElem = nextVstrandElem;
    lastElem = z;

    if (last_it) {
      break;
    }
  }

  logger.Log(string("num of nucleotides " + to_string(nid)));

  scaffold->SetSequence(seq);
}

void DASCadnano::CreateStaples(ADNPointer<ADNPart>nanorobot) {

  ADNLogger& logger = ADNLogger::GetLogger();
  //find number of staples and their starting points
  vector<vec2> stapleStarts; //vstrand id and position on vstrand
  
  int vStrandId = 0;
  for (auto vstrand : json_.vstrands_) {

    //starting point of staple in vstrand must be 0 == -1, 1 == -1, 2 != -1, 3 != -1 
    //multiple staples per cadnano are not considered yet
    int posAtVstrand = 0;
    for (auto stap : vstrand.stap_) {

      if (stap.n0 == -1 && stap.n1 == -1 && stap.n2 != -1 && stap.n3 != -1) {
        vec2 stapleStart;
        stapleStart.n0 = vStrandId;
        stapleStart.n1 = posAtVstrand;

        stapleStarts.push_back(stapleStart);
        //logger.Log(string("found starting point of staple at " + to_string(stapleStart.n0) + " " + to_string(stapleStart.n1)));
      }

      ++posAtVstrand;
    }
    ++vStrandId;
  }
  
  string numStaplesString;
  numStaplesString += "num of staple strands ";
  numStaplesString += to_string(stapleStarts.size());
  logger.Log(numStaplesString);

  auto vstrands = json_.vstrands_;
  int sid = 1; //because scaffold is chain 0
  int numSkips = 0;

  for (vec2 curStapleStart : stapleStarts) {
    auto vstrands = json_.vstrands_;
    vec4 curVstrandElem = json_.vstrands_[curStapleStart.n0].stap_[curStapleStart.n1];
    int vStrandId = curStapleStart.n0;
    int z = curStapleStart.n1;

    std::map<std::pair<int, int>, ADNPointer<ADNBaseSegment>> bs_positions = cellBsMap_.at(&json_.vstrands_[vStrandId]);

    ADNPointer<ADNSingleStrand> staple = new ADNSingleStrand();
    staple->SetName("Staple" + to_string(sid));
    staple->IsScaffold(false);
    nanorobot->RegisterSingleStrand(staple);
    
    if (vstrands[vStrandId].skip_[z] == 0) { //skip at the staple start
      ADNPointer<ADNNucleotide> nt = new ADNNucleotide(); //add starting nucleotide to chain
      nanorobot->RegisterNucleotideThreePrime(staple, nt);

      //SBPosition3 pos1D = vGrid_.GetGridStaplePos1D(nt->id_, nt->strand_->id_);
      SBPosition3 pos2D = vGrid_.GetGridStapleCellPos2D(vStrandId, z);
      SBPosition3 pos3D = vGrid_.GetGridStapleCellPos3D(vStrandId, z, vstrands[vStrandId].row_, vstrands[vStrandId].col_);

      //nt->SetPosition1D(pos1D);
      //nt->SetPosition2D(pos2D);
      nt->SetPosition(pos3D);
      nt->SetBackbonePosition(pos3D);
      nt->SetSidechainPosition(pos3D);

      ADNPointer<ADNNucleotide> scaff_nt = nullptr;
      if (scaffold_nucleotides_.find(std::make_tuple(vStrandId, z, 0)) != scaffold_nucleotides_.end()) {
        scaff_nt = scaffold_nucleotides_.at(std::make_tuple(vStrandId, z, 0));
        scaff_nt->SetPair(nt);
        nt->SetType(GetComplementaryBase(scaff_nt->GetType()));
      }
      nt->SetPair(scaff_nt);

      ADNPointer<ADNBaseSegment> bs = nullptr;
      std::pair<int, int> key = std::make_pair(z, 0);
      if (bs_positions.find(key) != bs_positions.end()) {
        bs = bs_positions.at(key);
      }
      // first one has to be BasePair
      ADNPointer<ADNCell> c = bs->GetCell();
      if (c->GetType() == BasePair) {
        ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(c());
        bp->SetRightNucleotide(nt);
        nt->SetBaseSegment(bs);
      }
      else {
        std::string msg = "Expected BasePair but found another cell type";
        logger.Log(msg);
      }
    }

    int nid = 1;
    if (vstrands[vStrandId].skip_[z] == -1) nid = 0; //start the id at 0 if the first nucleotide was skipped

    while (true) {
      int vStrandId = json_.numToIdMap_[curVstrandElem.n2];
      int z = curVstrandElem.n3;
      if (z == -1) break;

      bs_positions = cellBsMap_.at(&json_.vstrands_[vStrandId]);
      
      if (vstrands[vStrandId].skip_[z] == -1) {
        //add skips
        auto nextVstrand = vstrands[json_.numToIdMap_[curVstrandElem.n2]];
        vec4 nextVstrandElem = nextVstrand.stap_[curVstrandElem.n3];

        std::string msg = "staple skip " + std::to_string(vStrandId);
        logger.Log(msg);
        logger.Log(std::to_string(z));

        ADNPointer<ADNBaseSegment> bs = nullptr;
        std::pair<int, int> key = std::make_pair(z, 0);
        if (bs_positions.find(key) != bs_positions.end()) {
          bs = bs_positions.at(key);
        }
        if (bs != nullptr) {
          ADNPointer<ADNSkipPair> skipPair = static_cast<ADNSkipPair*>(bs->GetCell()());
          ADNPointer<ADNNucleotide> rightSkip = new ADNNucleotide();
          rightSkip->SetBaseSegment(bs);

          //SBPosition3 rightPos1D = vGrid_.GetGridStaplePos1D(nid, staple->id_);
          SBPosition3 rightPos2D = vGrid_.GetGridStapleCellPos2D(vStrandId, z);
          SBPosition3 rightPos3D = vGrid_.GetGridStapleCellPos3D(vStrandId, z, vstrands[vStrandId].row_, vstrands[vStrandId].col_);

          //rightSkip->SetPosition1D(rightPos1D);
          //rightSkip->SetPosition2D(rightPos2D);
          rightSkip->SetPosition(rightPos3D);
          rightSkip->SetBackbonePosition(rightPos3D);
          rightSkip->SetSidechainPosition(rightPos3D);

          skipPair->SetRightSkip(rightSkip);
          numSkips++;
        }

        ++nid;
        curVstrandElem = nextVstrandElem;

        continue;
      }
      
      int max_iter = 0;
      
      if (z != -1) {
        max_iter = vstrands[vStrandId].loop_[z];
      }
      else {
        break;
      }

      for (int k = 0; k <= max_iter; k++) {
        //add loop
        ADNPointer<ADNNucleotide> nt = new ADNNucleotide();

        //SBPosition3 pos1D = vGrid_.GetGridStaplePos1D(nt->id_, nt->strand_->id_);
        SBPosition3 pos2D = vGrid_.GetGridStapleCellPos2D(vStrandId, z);
        SBPosition3 delta2D = SBPosition3(SBQuantity::picometer(0.5f), SBQuantity::picometer(0), SBQuantity::picometer(0));
        pos2D = pos2D + delta2D; //make a parametric function
        SBPosition3 pos3D = vGrid_.GetGridStapleCellPos3D(vStrandId, z, vstrands[vStrandId].row_, vstrands[vStrandId].col_);
        
        ADNPointer<ADNBaseSegment> bs = nullptr;
        int p = 0;
        if (k > 0) p = 1;
        std::pair<int, int> key = std::make_pair(z, p);
        if (bs_positions.find(key) != bs_positions.end()) {
          bs = bs_positions.at(key);
        }

        if (bs == nullptr) continue;

        if (k == 0) {
          ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(bs->GetCell()());
          bp->SetRightNucleotide(nt);
        }
        else {
          ADNPointer<ADNLoopPair> loopPair = static_cast<ADNLoopPair*>(bs->GetCell()());
          ADNPointer<ADNLoop> right = loopPair->GetRightLoop();
          if (right == nullptr) {
            // first time we need to create
            right = new ADNLoop();
            loopPair->SetRightLoop(right);
            right->SetStart(nt);
          }
          right->AddNucleotide(nt);
          right->SetEnd(nt);
        }

        ADNPointer<ADNNucleotide> scaff_nt = nullptr;
        if (scaffold_nucleotides_.find(std::make_tuple(vStrandId, z, k)) != scaffold_nucleotides_.end()) {
          //find the scaffold nucleotide within loop
          scaff_nt = scaffold_nucleotides_.at(std::make_tuple(vStrandId, z, k));
          scaff_nt->SetPair(nt);
          nt->SetPair(scaff_nt);
          nt->SetType(GetComplementaryBase(scaff_nt->GetType()));
        }

        //nt->SetPosition1D(pos1D);
        //nt->SetPosition2D(pos2D);
        nt->SetPosition(pos3D);
        nt->SetBackbonePosition(pos3D);
        nt->SetSidechainPosition(pos3D);
        nt->SetBaseSegment(bs);

        nanorobot->RegisterNucleotideThreePrime(staple, nt);
      }

      //find next stap element
      auto nextVstrand = vstrands[json_.numToIdMap_[curVstrandElem.n2]];
      vec4 nextVstrandElem = nextVstrand.stap_[curVstrandElem.n3];
      curVstrandElem = nextVstrandElem;

      if (curVstrandElem.n0 != -1 && curVstrandElem.n1 != -1 && curVstrandElem.n2 == -1 && curVstrandElem.n3 == -1) {
        break;
      }
    }
    //set color of strand
    /*auto vstrandColors = json_.vstrands_[curStapleStart.n0].stap_colors_;
    for (auto color : vstrandColors) {
      if (color.n0 == curStapleStart.n1) {
        int colorDec = color.n1;

        stringstream ssHexColor;
        ssHexColor << hex << colorDec;
        string hexColor(ssHexColor.str());

        while (hexColor.size() < 6) {
          reverse(hexColor.begin(), hexColor.end());
          hexColor += "0";
          reverse(hexColor.begin(), hexColor.end());
        }

        string hexR = hexColor.substr(0, 2);
        string hexG = hexColor.substr(2, 2);
        string hexB = hexColor.substr(4, 2);

        int decR;
        std::stringstream ssDecR;
        ssDecR << hexR;
        ssDecR >> hex >> decR;
        double r = decR / 255.0f;

        int decG;
        std::stringstream ssDecG;
        ssDecG << hexG;
        ssDecG >> hex >> decG;
        double g = decG / 255.0f;;

        int decB;
        std::stringstream ssDecB;
        ssDecB << hexB;
        ssDecB >> hex >> decB;
        double b = decB / 255.0f;;

        staple->color_.setRed(r);
        staple->color_.setGreen(g);
        staple->color_.setBlue(b);
        staple->color_.setAlpha(1.0f);
        
        break;
      }
    }
    ++sid;*/
  }

  std::string msg = "numSkips (init) " + std::to_string(numSkips * 2);
  logger.Log(msg);
}

//void DASCadnano::ShiftStructure(ADNPointer<ADNPart> part) {
//
//  //calc center
//  SBPosition3 center3DStructure;
//  SBPosition3 center2DStructure;
//  SBPosition3 center1DStructure;
//
//  int numNucleotides = 0;
//  int numStapleNucleotides = 0;
//  SB_FOR(ADNPointer<ADNSingleStrand> strand, chains_) {
//    auto nts = strand->GetNucleotides();
//    SB_FOR(ADNPointer<ADNNucleotide> nucleotide, nts) {
//      SBPosition3 pos3D = nucleotide->GetPosition();
//      //SBPosition3 pos2D = nucleotide.second->GetSBPosition2D();
//      center3DStructure += pos3D;
//      //center2DStructure += pos2D;
//      ++numNucleotides;
//    }
//
//    //1D positions don't take the scaffold into account
//    //if (!strand->IsScaffold()) {
//    //  for (auto nucleotide : strand->nucleotides_) {
//    //    SBPosition3 pos1D = nucleotide.second->GetSBPosition1D();
//    //    center1DStructure += pos1D;
//    //    ++numStapleNucleotides;
//    //  }
//    //}
//  }
//
//  center3DStructure /= numNucleotides;
//  center2DStructure /= numNucleotides;
//  center1DStructure /= numStapleNucleotides;
//
//  //shift entire structure to origin
//
//  for (auto chain : chains_) {
//    auto strand = chain.second;
//
//    for (auto nucleotide : strand->nucleotides_) {
//      auto nt = nucleotide.second;
//      SBPosition3 pos3D = nucleotide.second->GetSBPosition();
//      //SBPosition3 pos2D = nucleotide.second->GetSBPosition2D();
//      //SBPosition3 pos1D = nucleotide.second->GetSBPosition1D();
//      
//      nt->SetPosition(pos3D - center3DStructure);
//      nt->SetBackbonePosition(pos3D - center3DStructure);
//      nt->SetSidechainPosition(pos3D - center3DStructure);
//      //nt->SetPosition2D(pos2D - center2DStructure);
//      //nt->SetPosition1D(pos1D - center1DStructure);
//    }
//  }
//
//  //shift positions of skips
//  SBPosition3 centerSkips3D;
//  SBPosition3 centerSkips2D;
//  SBPosition3 centerSkips1D;
//  int numSkips = 0;
//  for (auto const &ent : part.GetBaseSegments()) {
//    ADNBaseSegment * bs = ent.second;
//
//    if (bs->GetCell()->GetType() == CellType::SkipPair) {
//
//      ADNPointer<ADNSkipPair> sp = static_cast<ADNPointer<ADNSkipPair>>(bs->GetCell());
//      ADNPointer<ADNNucleotide> left = sp->GetLeftSkip();
//      ADNPointer<ADNNucleotide> right = sp->GetRightSkip();
//
//      if (left != nullptr) {
//        centerSkips3D += left->GetSBPosition();
//        centerSkips2D += left->GetSBPosition2D();
//        centerSkips1D += left->GetSBPosition1D();
//        numSkips++;
//      }
//      if (right != nullptr) {
//        centerSkips3D += right->GetSBPosition();
//        centerSkips2D += right->GetSBPosition2D();
//        centerSkips1D += right->GetSBPosition1D();
//        numSkips++;
//      }
//    }
//  }
//
//  centerSkips3D /= numSkips;
//  centerSkips2D /= numSkips;
//  centerSkips1D /= numSkips;
//
//  for (auto const &ent : part.GetBaseSegments()) {
//    ANTBaseSegment * bs = ent.second;
//
//    if (bs->GetCell()->GetType() == CellType::SkipPair) {
//
//      ADNPointer<ADNSkipPair> sp = static_cast<ADNPointer<ADNSkipPair>>(bs->GetCell());
//      ADNPointer<ADNNucleotide> left = sp->GetLeftSkip();
//      ADNPointer<ADNNucleotide> right = sp->GetRightSkip();
//
//      if (left != nullptr) {
//        SBPosition3 left3D = left->GetSBPosition();
//        SBPosition3 left2D = left->GetSBPosition2D();
//        SBPosition3 left1D = left->GetSBPosition1D();
//        left->SetPosition(left3D - centerSkips3D);
//        left->SetPosition2D(left2D - centerSkips2D);
//        left->SetPosition1D(left1D - centerSkips1D);
//      }
//      if (right != nullptr) {
//        SBPosition3 right3D = right->GetSBPosition();
//        SBPosition3 right2D = right->GetSBPosition2D();
//        SBPosition3 right1D = right->GetSBPosition1D();
//        right->SetPosition(right3D - centerSkips3D);
//        right->SetPosition2D(right2D - centerSkips2D);
//        right->SetPosition1D(right1D - centerSkips1D);
//      }
//    }
//  }
//}

//void DASCadnano::CylinderToCadnano(ADNPointer<ADNPart> nanorobot) {
//
//  ADNLogger& logger = ADNLogger::GetLogger();
//  //create the grid for fetching the position
//  auto doubleStrands = nanorobot->GetDoubleStrands();
//  size_t numVStrands = doubleStrands.size();
//
//  size_t numZ = 0;
//  SB_FOR(auto did : doubleStrands) {
//    ADNPointer<ADNDoubleStrand> doubleStrand = did.second;
//    if (numZ < doubleStrand->size_) {
//      numZ = doubleStrand->size_;
//    }
//  }
//
//  // closest multiple of 21 bigger than 21
//  div_t d = div(numZ, 21);
//  numZ = d.quot * 21;
//  if (numZ < 42) numZ = 42;
//
//  json_.name_ = nanorobot->GetName();
//  logger.Log(string("name: " + json_.name_));
//
//  for (unsigned int i = 0; i < numVStrands; i++) {
//    Vstrand vstrand;
//    vec2 col0;
//    col0.n0 = 0;
//    col0.n1 = 0;
//
//    vec2 col1;
//    col1.n0 = 0;
//    col1.n1 = 0;
//
//    vec2 col2;
//    col2.n0 = 0;
//    col2.n1 = 0;
//
//    vstrand.stap_colors_.push_back(col0);
//    vstrand.stap_colors_.push_back(col1);
//    vstrand.stap_colors_.push_back(col2);
//
//    //num
//    vstrand.num_ = i;
//    //logger.Log(string("\t num: " + to_string(vstrand.num_)));
//
//    //scafLoop todo
//
//    //stap
//    for (unsigned int j = 0; j < numZ; j++) {
//      
//      vec4 elem;
//      elem.n0 = -1;
//      elem.n1 = -1;
//      elem.n2 = -1;
//      elem.n3 = -1;
//      vstrand.stap_.push_back(elem);
//
//    }
//
//    //skip
//    for (unsigned int j = 0; j < numZ; j++) {
//      vstrand.skip_.push_back(0);
//    }
//
//    //scaf
//    for (unsigned int j = 0; j < numZ; j++) {
//      vec4 elem;
//      elem.n0 = -1;
//      elem.n1 = -1;
//      elem.n2 = -1;
//      elem.n3 = -1;
//      vstrand.scaf_.push_back(elem);
//
//    }
//
//    //col todo
//    vstrand.col_ = i;
//
//    //stapLoop todo
//
//    //row todo
//    vstrand.row_ = i;
//
//    //loop
//    for (unsigned int j = 0; j < numZ; j++) {
//      vstrand.loop_.push_back(0);
//
//    }
//
//    json_.vstrands_.push_back(vstrand);
//
//  }
//  vGrid_.ConstructGridAndLattice(numVStrands, numZ, LatticeType::LatticeFree, json_);
//  
//  //set to hexacomb when there are 6vstrands
//  if (numVStrands == 6) {
//    json_.vstrands_[0].col_ = 2;
//    json_.vstrands_[0].row_ = 0;
//
//    json_.vstrands_[1].col_ = 3;
//    json_.vstrands_[1].row_ = 0;
//
//    json_.vstrands_[2].col_ = 3;
//    json_.vstrands_[2].row_ = 1;
//
//    json_.vstrands_[3].col_ = 2;
//    json_.vstrands_[3].row_ = 1;
//
//    json_.vstrands_[4].col_ = 1;
//    json_.vstrands_[4].row_ = 1;
//
//    json_.vstrands_[5].col_ = 1;
//    json_.vstrands_[5].row_ = 0;
//  }
//  else if (numVStrands == 4) {
//    json_.vstrands_[0].col_ = 0;
//    json_.vstrands_[0].row_ = 0;
//
//    json_.vstrands_[1].col_ = 1;
//    json_.vstrands_[1].row_ = 0;
//
//    json_.vstrands_[2].col_ = 1;
//    json_.vstrands_[2].row_ = 1;
//
//    json_.vstrands_[3].col_ = 0;
//    json_.vstrands_[3].row_ = 1;
//  }
//
//  //trace double strands and set vStrands pos
//  for (auto did : doubleStrands) {
//    ADNPointer<ADNDoubleStrand> doubleStrand = did.second;
//    int vStrandId = doubleStrand->id_;
//    logger.Log(string("vStrandId: "), false);
//    logger.Log(QString::number(vStrandId));
//
//    ADNPointer<ADNBaseSegment> startBs = doubleStrand->start_;
//    ADNPointer<ADNBaseSegment> bs = startBs;
//    ADNPointer<ADNBaseSegment> lastBs = doubleStrand->GetLastBaseSegment();
//
//    unsigned int i = 0;
//    while (bs != nullptr && i < doubleStrand->size_) { // because of daedalus (bs->next_ is never null)
//      int z = bs->number_;
//
//      //logger.Log(QString::number(curBS->id_));
//      //logger.Log(string("z: "), false);
//      //logger.Log(QString::number(z));
//      
//      ADNPointer<ADNNucleotide> sc = nullptr;
//      ADNPointer<ADNNucleotide> st = nullptr;
//
//      ADNPointer<ADNNucleotide> bsNt = bs->GetNucleotide();
//      if (bsNt->strand_->isScaffold_) {
//        sc = bsNt;
//        st = bsNt->pair_;
//      }
//      else {
//        sc = bsNt->pair_;
//        st = bsNt;
//      }
//
//      //trace on vStrand
//      vec4 & scaf = json_.vstrands_[vStrandId].scaf_[z];
//      vec4 & stap = json_.vstrands_[vStrandId].stap_[z];
//
//      if (bs != startBs && bs != lastBs) {
//        scaf.n0 = vStrandId;
//        scaf.n1 = z - 1;
//        scaf.n2 = vStrandId;
//        scaf.n3 = z + 1;
//
//        stap.n0 = vStrandId;
//        stap.n1 = z + 1;
//        stap.n2 = vStrandId;
//        stap.n3 = z - 1;
//      }
//      else if (bs == startBs) {
//        scaf.n0 = vStrandId;
//        scaf.n1 = z - 1;
//        scaf.n2 = -1;
//        scaf.n3 = -1;
//
//        stap.n0 = vStrandId;
//        stap.n1 = z + 1;
//        stap.n2 = -1;
//        stap.n3 = -1;
//      }
//      else if (bs == lastBs) {
//        scaf.n0 = -1;
//        scaf.n1 = -1;
//        scaf.n2 = vStrandId;
//        scaf.n3 = z + 1;
//
//        stap.n0 = -1;
//        stap.n1 = -1;
//        stap.n2 = vStrandId;
//        stap.n3 = z - 1;
//      }
//
//      if (vStrandId % 2 != 0) {
//        vec4 temp = stap;
//        stap = scaf;
//        scaf = temp;
//      }
//      
//      //set 2D positions
//      SBPosition3 scPos = vGrid_.GetGridScaffoldCellPos2D(vStrandId, z);
//      SBPosition3 stPos = vGrid_.GetGridStapleCellPos2D(vStrandId, z);
//
//      sc->SetPosition2D(scPos);
//      st->SetPosition2D(stPos);
//      
//      bs = bs->next_;
//      i++;
//    } 
//
//  }
//
//  WriteJSON();
//
//  //get the shift to origin
//  {
//    SBPosition3 c3D;
//    SBPosition3 c2D;
//    int n = 0;
//    for (auto & sid : nanorobot.GetSingleStrands()) {
//      ADNPointer<ADNSingleStrand> singleStrand = sid.second;
//      ANTNucleotideList nucleotides = singleStrand->nucleotides_;
//
//      for (auto & nit : nucleotides) {
//        ADNPointer<ADNNucleotide> nucleotide = nit.second;
//        c3D += nucleotide->GetSBPosition();
//        c2D += nucleotide->GetSBPosition2D();
//        ++n;
//      }
//    }
//
//    c3D /= n;
//    c2D /= n;
//
//    SBPosition3 shift = c3D - c2D;
//
//    //get the rotation of the plane the cylinder unfolds to
//    ublas::vector<double> cylinderDir = nanorobot.e3_;
//    ublas::vector<double> planeDir = ublas::zero_vector<double>(3);; //plane normal
//    planeDir[0] = 1;
//
//    double dot = cylinderDir[0] * planeDir[0] + cylinderDir[1] * planeDir[1] + cylinderDir[2] * planeDir[2];
//    double len_cylinderDir = cylinderDir[0] * cylinderDir[0] + cylinderDir[1] * cylinderDir[1] + cylinderDir[2] * cylinderDir[2];
//    double len_normal = planeDir[0] * planeDir[0] + planeDir[1] * planeDir[1] + planeDir[2] * planeDir[2];
//    double theta = acos(dot / sqrt(len_cylinderDir * len_normal));
//
//    logger.LogVector("e3", cylinderDir);
//    logger.LogVector("normal", planeDir);
//
//    ublas::vector<double> rotDir = ANTVectorMath::CrossProduct(cylinderDir, planeDir);
//    ublas::matrix<double> rotMat = ANTVectorMath::MakeRotationMatrix(rotDir, theta);
//
//    //apply shift and initial orientation of plane
//    for (auto & sid : nanorobot.GetSingleStrands()) {
//      ADNPointer<ADNSingleStrand> singleStrand = sid.second;
//      ANTNucleotideList nucleotides = singleStrand->nucleotides_;
//
//      for (auto & nit : nucleotides) {
//        ADNPointer<ADNNucleotide> nucleotide = nit.second;
//        SBPosition3 pos2D = nucleotide->GetSBPosition2D();
//
//        pos2D -= c2D;
//
//        ublas::vector<double> pos = ANTVectorMath::CreateBoostVector(pos2D);
//        ublas::vector<double> newPos = ublas::prod(rotMat, pos);
//
//        pos2D = ANTVectorMath::CreateSBPosition(newPos);
//
//        pos2D += c2D;
//
//        pos2D += shift;
//
//        nucleotide->SetPosition2D(pos2D);
//
//      }
//    }
//  }
//
//  {
//    //rotation the plane to opposite of the cylinder cut
//    SBPosition3 c3D;
//    SBPosition3 c2D;
//    int n = 0;
//    for (auto & sid : nanorobot.GetSingleStrands()) {
//      ADNPointer<ADNSingleStrand> singleStrand = sid.second;
//      ANTNucleotideList nucleotides = singleStrand->nucleotides_;
//
//      for (auto & nit : nucleotides) {
//        ADNPointer<ADNNucleotide> nucleotide = nit.second;
//        c3D += nucleotide->GetSBPosition();
//        c2D += nucleotide->GetSBPosition2D();
//        ++n;
//      }
//    }
//
//    c3D /= n;
//    c2D /= n;
//
//    auto pos1 = doubleStrands[0]->start_->GetNucleotide()->GetSBPosition();
//    auto pos2 = doubleStrands[0]->GetLastBaseSegment()->GetNucleotide()->GetSBPosition();
//    SBPosition3 pos3 = (pos1 + pos2) / 2;
//
//    ublas::vector<double> rotDir = nanorobot.e3_;
//    ublas::vector<double> dirToPlane = ANTVectorMath::CreateBoostVector(c3D - c2D); 
//    ublas::vector<double> dirToCut = ANTVectorMath::CreateBoostVector(pos3 - c3D); 
//    
//    SBPosition3 shift = c2D - pos3;
//
//    double dot = dirToPlane[0] * dirToCut[0] + dirToPlane[1] * dirToCut[1] + dirToPlane[2] * dirToCut[2];
//    double len_dirToPlane = dirToPlane[0] * dirToPlane[0] + dirToPlane[1] * dirToPlane[1] + dirToPlane[2] * dirToPlane[2];
//    double len_dirToCut = dirToCut[0] * dirToCut[0] + dirToCut[1] * dirToCut[1] + dirToCut[2] * dirToCut[2];
//    double theta = acos(dot / sqrt(len_dirToPlane * len_dirToCut));
//
//    logger.LogVector("dirToPlane", dirToPlane);
//    logger.LogVector("dirToCut", dirToCut);
//
//    //ublas::vector<double> rotDir = ANTVectorMath::CrossProduct(dirToPlane, dirToCut);
//    ublas::matrix<double> rotMat = ANTVectorMath::MakeRotationMatrix(rotDir, theta);
//
//    for (auto & sid : nanorobot.GetSingleStrands()) {
//      ADNPointer<ADNSingleStrand> singleStrand = sid.second;
//      ANTNucleotideList nucleotides = singleStrand->nucleotides_;
//
//      for (auto & nit : nucleotides) {
//        ADNPointer<ADNNucleotide> nucleotide = nit.second;
//        SBPosition3 pos2D = nucleotide->GetSBPosition2D();
//
//        pos2D -= c2D;
//
//        ublas::vector<double> pos = ANTVectorMath::CreateBoostVector(pos2D);
//        ublas::vector<double> newPos = ublas::prod(rotMat, pos);
//
//        pos2D = ANTVectorMath::CreateSBPosition(newPos);
//
//        pos2D += c2D;
//        pos2D += shift;
//
//        nucleotide->SetPosition2D(pos2D);
//      }
//    }
//  }
//}

DNABlocks DASCadnano::GetComplementaryBase(DNABlocks type) {
  return ADNModel::GetComplementaryBase(type);
}

bool DASCadnano::IsThereBase(vec4 data) {
  bool base = false;

  if ((data.n0 != -1 && data.n1 != -1) || (data.n2 != -1 && data.n3 != -1)) base = true;

  return base;
}
