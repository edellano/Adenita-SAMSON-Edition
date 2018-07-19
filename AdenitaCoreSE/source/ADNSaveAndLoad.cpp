#pragma once

#include "ADNSaveAndLoad.hpp"


ADNPointer<ADNPart> ADNLoader::LoadPartFromJson(std::string filename)
{
  ADNPointer<ADNPart> part = new ADNPart();

  FILE* fp = fopen(filename.c_str(), "rb");
  char readBuffer[131072];
  FileReadStream is(fp, readBuffer, sizeof(readBuffer));
  Document d;
  d.ParseStream(is);

  // check for save version
  double versionValue = 0.0;
  if (Value* version = Pointer("/version").Get(d)) {
    versionValue = version->GetDouble();
  }

  if (versionValue < 0.4) {
    return LoadPartFromJsonLegacy(filename);
  }

  std::string name = d["name"].GetString();
  part->SetName(name);
  std::string position = d["position"].GetString();
  part->SetPosition(ADNAuxiliary::StringToSBPosition(position));

  std::map<int, ADNPointer<ADNNucleotide>> nts;
  std::map<int, int> nexts;
  std::map<int, int> prevs;
  std::map<int, int> pairs;
  Value& strands = d["singleStrands"];
  for (Value::ConstMemberIterator itr = strands.MemberBegin(); itr != strands.MemberEnd(); ++itr) {

    ADNPointer<ADNSingleStrand> ss = ADNPointer<ADNSingleStrand>(new ADNSingleStrand());
    ss->SetName(itr->value["chainName"].GetString());
    ss->IsScaffold(itr->value["isScaffold"].GetBool());
    
    const Value& val_nucleotides = itr->value["nucleotides"];
    for (Value::ConstMemberIterator itr2 = val_nucleotides.MemberBegin(); itr2 != val_nucleotides.MemberEnd(); ++itr2) {

      ADNPointer<ADNNucleotide> nt = ADNPointer<ADNNucleotide>(new ADNNucleotide());
      nt->SetType(ADNModel::ResidueNameToType(itr2->value["type"].GetString()[0]));
      nt->SetPosition(ADNAuxiliary::StringToSBPosition(itr2->value["position"].GetString()));
      nt->SetBackbonePosition(ADNAuxiliary::StringToSBPosition(itr2->value["backboneCenter"].GetString()));
      nt->SetSidechainPosition(ADNAuxiliary::StringToSBPosition(itr2->value["sidechainCenter"].GetString()));
      nt->SetE1(ADNAuxiliary::StringToUblasVector(itr2->value["e1"].GetString()));
      nt->SetE2(ADNAuxiliary::StringToUblasVector(itr2->value["e2"].GetString()));
      nt->SetE3(ADNAuxiliary::StringToUblasVector(itr2->value["e3"].GetString()));

      nts.insert(std::make_pair(std::stoi(itr2->name.GetString()), nt));
      nexts.insert(std::make_pair(std::stoi(itr2->name.GetString()), itr2->value["next"].GetInt()));
      prevs.insert(std::make_pair(std::stoi(itr2->name.GetString()), itr2->value["prev"].GetInt()));
      pairs.insert(std::make_pair(std::stoi(itr2->name.GetString()), itr2->value["pair"].GetInt()));
    }
    
    int f_id = itr->value["fivePrimeId"].GetInt();

    ADNPointer<ADNNucleotide> nt = nts.at(f_id);
    int currId = f_id;
    do {
      // pairing is done when parsing base segments
      int nextId = nexts.at(currId);   
      nt = nts.at(currId);
      part->RegisterNucleotideThreePrime(ss, nt);
      currId = nextId;
    } while (currId != -1);

    part->RegisterSingleStrand(ss);
  }

  std::map<int, ADNPointer<ADNBaseSegment>> bss;
  std::map<int, int> nextsBs;
  std::map<int, int> prevsBs;
  Value& doubleStrands = d["doubleStrands"];
  for (Value::ConstMemberIterator itr = doubleStrands.MemberBegin(); itr != doubleStrands.MemberEnd(); ++itr) {
    ADNPointer<ADNDoubleStrand> ds = ADNPointer<ADNDoubleStrand>(new ADNDoubleStrand());
    ds->SetInitialTwistAngle(itr->value["initialTwistAngle"].GetDouble());

    const Value& bases = itr->value["bases"];
    for (Value::ConstMemberIterator itr2 = bases.MemberBegin(); itr2 != bases.MemberEnd(); ++itr2) {
      ADNPointer<ADNBaseSegment> bs = ADNPointer<ADNBaseSegment>(new ADNBaseSegment());
      bs->SetPosition(ADNAuxiliary::StringToSBPosition(itr2->value["position"].GetString()));
      bs->SetE1(ADNAuxiliary::StringToUblasVector(itr2->value["e1"].GetString()));
      bs->SetE2(ADNAuxiliary::StringToUblasVector(itr2->value["e2"].GetString()));
      bs->SetE3(ADNAuxiliary::StringToUblasVector(itr2->value["e3"].GetString()));
      bs->SetNumber(itr2->value["number"].GetInt());

      // Load cells
      const Value& c = itr2->value["cell"];
      CellType typ = CellType(c["type"].GetInt());
      if (typ == BasePair) {
        ADNPointer<ADNBasePair> bp_cell = ADNPointer<ADNBasePair>(new ADNBasePair());
        const Value& left = c["left"];
        const Value& right = c["right"];
        int nt_id_left = left.GetInt();
        int nt_id_right = right.GetInt();
        ADNPointer<ADNNucleotide> ntLeft = nullptr;
        if (nt_id_left > -1) {
          ntLeft = nts.at(nt_id_left);
          ntLeft->SetBaseSegment(bs);
        }
        ADNPointer<ADNNucleotide> ntRight = nullptr;
        if (nt_id_right > -1) {
          ntRight = nts.at(nt_id_right);
          ntRight->SetBaseSegment(bs);
        }
        bp_cell->AddPair(ntLeft, ntRight);
        bs->SetCell(bp_cell());
      }
      else if (typ == LoopPair) {
        ADNPointer<ADNLoopPair> lp_cell = ADNPointer<ADNLoopPair>(new ADNLoopPair());

        const Value& left = c["leftLoop"];
        const Value& right = c["rightLoop"];

        ADNPointer<ADNLoop> leftLoop = ADNPointer<ADNLoop>(new ADNLoop());
        ADNPointer<ADNLoop> rightLoop = ADNPointer<ADNLoop>(new ADNLoop());

        if (left.HasMember("startNt")) {
          int startNtId = left["startNt"].GetInt();
          int endNtId = left["endNt"].GetInt();

          ADNPointer<ADNNucleotide> startNt = nts.at(startNtId);
          ADNPointer<ADNNucleotide> lastNt = nts.at(endNtId);
          ADNPointer<ADNSingleStrand> ss = startNt->GetStrand();
          leftLoop->SetStart(startNt);
          leftLoop->SetEnd(lastNt);
          
          std::string nucleotides = left["nucleotides"].GetString();
          std::vector<int> ntVec = ADNAuxiliary::StringToVector(nucleotides);
          for (auto &i : ntVec) {
            ADNPointer<ADNNucleotide> nt = nts.at(i);
            leftLoop->AddNucleotide(nt);
            nt->SetBaseSegment(bs);
          }
        }

        if (right.HasMember("startNt")) {
          int startNtId = right["startNt"].GetInt();
          int endNtId = right["endNt"].GetInt();

          ADNPointer<ADNNucleotide> startNt = nts.at(startNtId);
          ADNPointer<ADNNucleotide> lastNt = nts.at(endNtId);
          ADNPointer<ADNSingleStrand> ss = startNt->GetStrand();
          leftLoop->SetStart(startNt);
          leftLoop->SetEnd(lastNt);

          std::string nucleotides = right["nucleotides"].GetString();
          std::vector<int> ntVec = ADNAuxiliary::StringToVector(nucleotides);
          for (auto &i : ntVec) {
            ADNPointer<ADNNucleotide> nt = nts.at(i);
            rightLoop->AddNucleotide(nt);
            nt->SetBaseSegment(bs);
          }
        }

        bs->SetCell(lp_cell());
      }

      bss.insert(std::make_pair(std::stoi(itr2->name.GetString()), bs));
      nextsBs.insert(std::make_pair(std::stoi(itr2->name.GetString()), itr2->value["next"].GetInt()));
      prevsBs.insert(std::make_pair(std::stoi(itr2->name.GetString()), itr2->value["previous"].GetInt()));
    }
    int bsStartId = itr->value["firstBaseSegment"].GetInt();
    int bsEndId = itr->value["lastBaseSegment"].GetInt();

    ADNPointer<ADNBaseSegment> bs = bss.at(bsStartId);
    int currId = bsStartId;
    do {
      int nextId = nextsBs.at(currId);
      bs = bss.at(currId);
      part->RegisterBaseSegmentEnd(ds, bs);
      currId = nextId;
    } while (currId != -1);

    part->RegisterDoubleStrand(ds);
  }

  fclose(fp);

  return part;
}

ADNPointer<ADNPart> ADNLoader::LoadPartFromJsonLegacy(std::string filename)
{
  // ids are reset since old format didn't use unique ids

  ADNPointer<ADNPart> part = new ADNPart();

  FILE* fp = fopen(filename.c_str(), "rb");
  char readBuffer[131072];
  FileReadStream is(fp, readBuffer, sizeof(readBuffer));
  Document d;
  d.ParseStream(is);

  // check for save version
  double versionValue = 0.0;
  if (Value* version = Pointer("/version").Get(d)) {
    versionValue = version->GetDouble();
  }
  else {
    std::string msg = "Format is too old and can't be loaded with the current Adenita version";
    ADNLogger& logger = ADNLogger::GetLogger();
    logger.Log(msg);
  }

  std::string name = d["name"].GetString();
  part->SetName(name);
  std::string position = d["position"].GetString();
  part->SetPosition(ADNAuxiliary::StringToSBPosition(position));

  Value& strands = d["strands"];
  std::map<int, ADNPointer<ADNSingleStrand>> origSingleStrandId;
  std::map<std::pair<int, int>, ADNPointer<ADNNucleotide>> origNucleotideId;

  for (Value::ConstMemberIterator itr = strands.MemberBegin(); itr != strands.MemberEnd(); ++itr) {
    int strandId = itr->value["id"].GetInt();
    ADNPointer<ADNSingleStrand> ss = ADNPointer<ADNSingleStrand>( new ADNSingleStrand());
    part->RegisterSingleStrand(ss);

    ss->SetName(itr->value["chainName"].GetString());
    ss->IsScaffold(itr->value["isScaffold"].GetBool());

    int fivePrimeId = itr->value["fivePrimeId"].GetInt();
    int threePrimeId = itr->value["fivePrimeId"].GetInt();

    const Value& val_nucleotides = itr->value["nucleotides"];
    for (Value::ConstMemberIterator itr2 = val_nucleotides.MemberBegin(); itr2 != val_nucleotides.MemberEnd(); ++itr2) {
      ADNPointer<ADNNucleotide> nt = ADNPointer<ADNNucleotide>(new ADNNucleotide());
      part->RegisterNucleotideThreePrime(ss, nt);
      std::string test = itr2->value["type"].GetString();
      auto test2 = test.c_str();
      nt->SetType(ADNModel::ResidueNameToType(test2[0]));
      nt->SetE1(ADNAuxiliary::StringToUblasVector(itr2->value["e1"].GetString()));
      nt->SetE2(ADNAuxiliary::StringToUblasVector(itr2->value["e2"].GetString()));
      nt->SetE3(ADNAuxiliary::StringToUblasVector(itr2->value["e3"].GetString()));
      nt->SetPosition(ADNAuxiliary::StringToSBPosition(itr2->value["position"].GetString()));

      nt->SetName(itr2->value["type"].GetString() + std::to_string(nt->getNodeIndex()));

      nt->SetBackbonePosition(ADNAuxiliary::StringToSBPosition(itr2->value["backboneCenter"].GetString()));
      nt->SetSidechainPosition(ADNAuxiliary::StringToSBPosition(itr2->value["sidechainCenter"].GetString()));

      int nucleotideId = itr2->value["id"].GetInt();

      if (nucleotideId == fivePrimeId) {
        nt->SetEnd(FivePrime);
        ss->SetFivePrime(nt);
      }
      if (nucleotideId == threePrimeId) {
        nt->SetEnd(ThreePrime);
        ss->SetThreePrime(nt);
      }

      std::pair<int, int> key = std::make_pair(strandId, nucleotideId);
      origNucleotideId.insert(std::make_pair(key, nt));
    }
    origSingleStrandId.insert(std::make_pair(strandId, ss));
  }
  // pair nucleotides
  for (Value::ConstMemberIterator itr = strands.MemberBegin(); itr != strands.MemberEnd(); ++itr) {

    int old_strand_id = itr->value["id"].GetInt();
    const Value& val_nucleotides = itr->value["nucleotides"];

    for (Value::ConstMemberIterator itr2 = val_nucleotides.MemberBegin(); itr2 != val_nucleotides.MemberEnd(); ++itr2) {
      int curr_id = itr2->value["id"].GetInt();
      int next_id = itr2->value["next"].GetInt();
      int prev_id = itr2->value["prev"].GetInt();

      const Value& val_pair_info = itr2->value["pair"];
      int ss_pair_id = val_pair_info["strandId"].GetInt();
      int nt_pair_id = val_pair_info["pairId"].GetInt();

      std::pair<int, int> keyCurr = std::make_pair(old_strand_id, curr_id);
      std::pair<int, int> keyPair = std::make_pair(ss_pair_id, nt_pair_id);
      std::pair<int, int> keyNext = std::make_pair(old_strand_id, next_id);
      std::pair<int, int> keyPrev = std::make_pair(old_strand_id, prev_id);

      ADNPointer<ADNNucleotide> pairNt = nullptr;
      ADNPointer<ADNNucleotide> nextNt = nullptr;
      ADNPointer<ADNNucleotide> prevNt = nullptr;
      ADNPointer<ADNNucleotide> nt = origNucleotideId.at(keyCurr);

      if (origNucleotideId.find(keyPair) != origNucleotideId.end()) {
        pairNt = origNucleotideId.at(keyPair);
      }
      if (origNucleotideId.find(keyNext) != origNucleotideId.end()) {
        nextNt = origNucleotideId.at(keyNext);
      }
      
      if (origNucleotideId.find(keyPrev) != origNucleotideId.end()) {
        prevNt = origNucleotideId.at(keyPrev);
      }
      nt->SetPair(pairNt);
    }
  }

  Value& doubleStrands = d["doubleStrands"];
  std::map<std::pair<int, int>, ADNPointer<ADNBaseSegment>> origBssId;
  std::map<int, ADNPointer<ADNDoubleStrand>> origDoubleStrandId;
  for (Value::ConstMemberIterator itr = doubleStrands.MemberBegin(); itr != doubleStrands.MemberEnd(); ++itr) {
    ADNPointer<ADNDoubleStrand> ds = ADNPointer<ADNDoubleStrand>(new ADNDoubleStrand());
    part->RegisterDoubleStrand(ds);

    int dsId = itr->value["id"].GetInt();
    ds->SetInitialTwistAngle(itr->value["initialTwistAngle"].GetDouble());

    int size = itr->value["size"].GetInt();
    int bsStartId = itr->value["bsStartId"].GetInt();

    ADNPointer<ADNBaseSegment> startBs = nullptr;
    for (int i = 0; i < size; ++i) {
      ADNPointer<ADNBaseSegment> bs = new ADNBaseSegment();
      part->RegisterBaseSegmentEnd(ds, bs);
      if (i == 0) {
        startBs = bs;
      }
    }
    std::pair<int, int> bsKey = std::make_pair(dsId, bsStartId);
    origBssId.insert(std::make_pair(bsKey, startBs));
    origDoubleStrandId.insert(std::make_pair(dsId, ds));
  }

  Value& joints = d["joints"];
  std::map<int, ublas::vector<double>> jointPositions;
  for (Value::ConstMemberIterator itr = joints.MemberBegin(); itr != joints.MemberEnd(); ++itr) {
    int jId = itr->value["id"].GetInt();
    ublas::vector<double> pos = ADNAuxiliary::StringToUblasVector(itr->value["position"].GetString());
    jointPositions.insert(std::make_pair(jId, pos));
  }

  Value& bases = d["bases"];
  for (Value::ConstMemberIterator itr = bases.MemberBegin(); itr != bases.MemberEnd(); ++itr) {
    int doubleStrandId = itr->value["double_strand"].GetInt();
    auto ds = origDoubleStrandId.at(doubleStrandId);

    int sourceId = itr->value["source"].GetInt();
    int targetId = itr->value["target"].GetInt();
    auto sourcePos = jointPositions.at(sourceId);
    auto targetPos = jointPositions.at(targetId);
    int number = itr->value["number"].GetInt();

    ADNPointer<ADNBaseSegment> bs = ds->GetNthBaseSegment(number);

    bs->SetNumber(number);
    bs->SetE2(ADNAuxiliary::StringToUblasVector(itr->value["normal"].GetString()));
    bs->SetE3(ADNAuxiliary::StringToUblasVector(itr->value["direction"].GetString()));
    bs->SetE1(ADNAuxiliary::StringToUblasVector(itr->value["u"].GetString()));
    bs->SetPosition(ADNAuxiliary::UblasVectorToSBPosition(sourcePos + targetPos)*0.5);

    if (versionValue > 0.1) {
      // Load cells
      const Value& c = itr->value["cell"];
      CellType typ = CellType(c["type"].GetInt());
      if (typ == BasePair) {
        ADNPointer<ADNBasePair> bp_cell = ADNPointer<ADNBasePair>(new ADNBasePair());
        
        const Value& left = c["left"];
        const Value& right = c["right"];
        int ss_id_left = left["strand_id"].GetInt();
        int nt_id_left = left["nt_id"].GetInt();
        std::pair<int, int> leftKey = std::make_pair(ss_id_left, nt_id_left);

        int ss_id_right = right["strand_id"].GetInt();
        int nt_id_right = right["nt_id"].GetInt();
        std::pair<int, int> rightKey = std::make_pair(ss_id_right, nt_id_right);

        if (origNucleotideId.find(leftKey) != origNucleotideId.end()) {
          auto nt = origNucleotideId.at(leftKey);
          bp_cell->SetLeftNucleotide(nt);
          nt->SetBaseSegment(bs);
        }
        if (origNucleotideId.find(rightKey) != origNucleotideId.end()) {
          auto nt = origNucleotideId.at(rightKey);
          bp_cell->SetRightNucleotide(nt);
          nt->SetBaseSegment(bs);
        }

        bs->SetCell(bp_cell());
      }
      else if (typ == LoopPair) {
        ADNPointer<ADNLoopPair> lp_cell = ADNPointer<ADNLoopPair>(new ADNLoopPair());
        
        const Value& left = c["left_loop"];
        const Value& right = c["right_loop"];

        if (left.HasMember("id")) {
          auto leftLoop = ADNPointer<ADNLoop>(new ADNLoop());
          
          const Value& startNt = left["start_nt"];
          const Value& endNt = left["end_nt"];
          int id = left["id"].GetInt();
          int strandId = left["strand_id"].GetInt();
          int startNtId = startNt["nt_id"].GetInt();
          int startNtSsId = startNt["strand_id"].GetInt();
          std::pair<int, int> startKey = std::make_pair(startNtSsId, startNtId);
          int endNtId = endNt["nt_id"].GetInt();
          int endNtSsId = endNt["strand_id"].GetInt();
          std::pair<int, int> endKey = std::make_pair(endNtSsId, endNtId);

          ADNPointer<ADNNucleotide> sNt;
          ADNPointer<ADNNucleotide> eNt;
          if (origNucleotideId.find(startKey) != origNucleotideId.end()) {
            sNt = origNucleotideId.at(startKey);
          }
          if (origNucleotideId.find(endKey) != origNucleotideId.end()) {
            eNt = origNucleotideId.at(endKey);
          }

          auto order = ADNBasicOperations::OrderNucleotides(sNt, eNt);
          leftLoop->SetStart(order.first);
          leftLoop->SetEnd(order.second);

          const Value& nucleotides = left["nucleotides_list"];
          for (Value::ConstMemberIterator itr = nucleotides.MemberBegin(); itr != nucleotides.MemberEnd(); ++itr) {
            int nt_id = itr->value["nt_id"].GetInt();
            int ss_id = itr->value["strand_id"].GetInt();
            std::pair<int, int> ntKey = std::make_pair(ss_id, nt_id);
            if (origNucleotideId.find(ntKey) != origNucleotideId.end()) {
              auto nt = origNucleotideId.at(ntKey);
              leftLoop->AddNucleotide(nt);
              nt->SetBaseSegment(bs);
            }
          }

          lp_cell->SetLeftLoop(leftLoop);
        }

        if (right.HasMember("id")) {
          auto rightLoop = ADNPointer<ADNLoop>(new ADNLoop());
          
          const Value& startNt = right["start_nt"];
          const Value& endNt = right["end_nt"];
          int id = right["id"].GetInt();
          int strandId = right["strand_id"].GetInt();
          int startNtId = startNt["nt_id"].GetInt();
          int startNtSsId = startNt["strand_id"].GetInt();
          std::pair<int, int> startKey = std::make_pair(startNtSsId, startNtId);
          int endNtId = endNt["nt_id"].GetInt();
          int endNtSsId = endNt["strand_id"].GetInt();
          std::pair<int, int> endKey = std::make_pair(endNtSsId, endNtId);

          ADNPointer<ADNNucleotide> sNt;
          ADNPointer<ADNNucleotide> eNt;
          if (origNucleotideId.find(startKey) != origNucleotideId.end()) {
            sNt = origNucleotideId.at(startKey);
          }
          if (origNucleotideId.find(endKey) != origNucleotideId.end()) {
            eNt = origNucleotideId.at(endKey);
          }

          auto order = ADNBasicOperations::OrderNucleotides(sNt, eNt);
          rightLoop->SetStart(order.first);
          rightLoop->SetEnd(order.second);

          const Value& nucleotides = right["nucleotides_list"];
          for (Value::ConstMemberIterator itr = nucleotides.MemberBegin(); itr != nucleotides.MemberEnd(); ++itr) {
            int nt_id = itr->value["nt_id"].GetInt();
            int ss_id = itr->value["strand_id"].GetInt();
            std::pair<int, int> ntKey = std::make_pair(ss_id, nt_id);
            if (origNucleotideId.find(ntKey) != origNucleotideId.end()) {
              auto nt = origNucleotideId.at(ntKey);
              rightLoop->AddNucleotide(nt);
              nt->SetBaseSegment(bs);
            }
          }

          lp_cell->SetRightLoop(rightLoop);
        }

        bs->SetCell(lp_cell());
      }
    }
    else {
      // base segment stores the nt as double strand
      ADNPointer<ADNBasePair> bp_cell = ADNPointer<ADNBasePair>(new ADNBasePair());
      const Value& val_nt = itr->value["nt"];
      int ss_id = val_nt["strandId"].GetInt();
      int nt_id = val_nt["ntId"].GetInt();
      std::pair<int, int> ntKey = std::make_pair(ss_id, nt_id);
      if (origNucleotideId.find(ntKey) != origNucleotideId.end()) {
        auto nt = origNucleotideId.at(ntKey);
        auto ntPair = nt->GetPair();
        bp_cell->SetLeftNucleotide(nt);
        bp_cell->SetRightNucleotide(ntPair);
        nt->SetBaseSegment(bs);
        if (ntPair != nullptr) ntPair->SetBaseSegment(bs);
      }
      bs->SetCell(bp_cell());
    }
  }

  fclose(fp);

  return part;
}

ADNNanorobot * ADNLoader::LoadNanorobotFromJson(std::string filename)
{
  ADNNanorobot* nr = new ADNNanorobot();

  FILE* fp = fopen(filename.c_str(), "rb");
  char readBuffer[131072];
  FileReadStream is(fp, readBuffer, sizeof(readBuffer));
  Document d;
  d.ParseStream(is);

  // check for save version
  double versionValue = 0.0;
  if (Value* version = Pointer("/version").Get(d)) {
    versionValue = version->GetDouble();
  }


  return nr;
}

void ADNLoader::SavePartToJson(ADNPointer<ADNPart> p, rapidjson::Writer<StringBuffer>& writer)
{

  std::string st_position = ADNAuxiliary::SBPositionToString(p->GetPosition());

  writer.Key("position");
  writer.String(st_position.c_str());

  writer.Key("name");
  writer.String(p->GetName().c_str());

  auto doubleStrands = p->GetDoubleStrands();
  writer.Key("doubleStrands");
  writer.StartObject();
  SB_FOR(SBStructuralNode* p, doubleStrands) {
    ADNPointer<ADNDoubleStrand> ds = static_cast<ADNDoubleStrand*>(p);

    std::string key = std::to_string(ds->getNodeIndex());
    writer.Key(key.c_str());
    writer.StartObject();

    writer.Key("firstBaseSegment");
    writer.Int(ds->GetFirstBaseSegment()->getNodeIndex());

    writer.Key("lastBaseSegment");
    writer.Int(ds->GetLastBaseSegment()->getNodeIndex());

    writer.Key("initialTwistAngle");
    writer.Double(ds->GetInitialTwistAngle());

    auto bases = ds->GetBaseSegments();
    writer.Key("bases");
    writer.StartObject();
    SB_FOR(SBStructuralNode* pair, bases) {
      ADNPointer<ADNBaseSegment> bs = static_cast<ADNBaseSegment*>(pair);

      std::string key = std::to_string(bs->getNodeIndex());
      writer.Key(key.c_str());
      writer.StartObject();

      writer.Key("number");
      writer.Int(bs->GetNumber());

      writer.Key("position");
      writer.String(ADNAuxiliary::SBPositionToString(bs->GetPosition()).c_str());

      int nextId = -1;
      if (bs->GetNext() != nullptr) nextId = bs->GetNext()->getNodeIndex();
      writer.Key("next");
      writer.Int(nextId);

      int prevId = -1;
      if (bs->GetPrev() != nullptr) prevId = bs->GetPrev()->getNodeIndex();
      writer.Key("previous");
      writer.Int(prevId);

      writer.Key("e1");
      writer.String(ADNAuxiliary::UblasVectorToString(bs->GetE1()).c_str());

      writer.Key("e2");
      writer.String(ADNAuxiliary::UblasVectorToString(bs->GetE2()).c_str());

      writer.Key("e3");
      writer.String(ADNAuxiliary::UblasVectorToString(bs->GetE3()).c_str());

      writer.Key("cell");
      writer.StartObject();

      auto type = bs->GetCellType();
      writer.Key("type");
      writer.Int(type);

      ADNPointer<ADNCell> cell = bs->GetCell();
      if (type == CellType::BasePair) {
        ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(cell());

        int idLeft = -1;
        if (bp->GetLeftNucleotide() != nullptr) idLeft = bp->GetLeftNucleotide()->getNodeIndex();
        writer.Key("left");
        writer.Int(idLeft);

        int idRight = -1;
        if (bp->GetRightNucleotide() != nullptr) idRight = bp->GetRightNucleotide()->getNodeIndex();
        writer.Key("right");
        writer.Int(idRight);
      }
      else if (type == CellType::LoopPair) {
        ADNPointer<ADNLoopPair> lp = static_cast<ADNLoopPair*>(cell());

        ADNPointer<ADNLoop> left = lp->GetLeftLoop();
        writer.Key("leftLoop");
        writer.StartObject();

        if (left != nullptr) {
          int startNtId = -1;
          if (left->GetStart() != nullptr) startNtId = left->GetStart()->getNodeIndex();
          writer.Key("startNt");
          writer.Int(startNtId);

          int endNtId = -1;
          if (left->GetEnd() != nullptr) endNtId = left->GetEnd()->getNodeIndex();
          writer.Key("endNt");
          writer.Int(endNtId);

          auto nts = left->GetNucleotides();
          std::vector<int> ntList;
          writer.Key("nucleotides");
          SB_FOR(SBStructuralNode* n, nts) {
            ADNPointer<ADNNucleotide> nt = static_cast<ADNNucleotide*>(n);
            ntList.push_back(nt->getNodeIndex());
          }
          std::string str = ADNAuxiliary::VectorToString(ntList);
          writer.String(str.c_str());
        }

        writer.EndObject();  // end left loop

        ADNPointer<ADNLoop> right = lp->GetRightLoop();
        writer.Key("rightLoop");
        writer.StartObject();

        if (right != nullptr) {
          int startNtId = -1;
          if (right->GetStart() != nullptr) startNtId = right->GetStart()->getNodeIndex();
          writer.Key("startNt");
          writer.Int(startNtId);

          int endNtId = -1;
          if (right->GetEnd() != nullptr) endNtId = right->GetEnd()->getNodeIndex();
          writer.Key("endNt");
          writer.Int(endNtId);

          auto nts = right->GetNucleotides();
          std::vector<int> ntList;
          writer.Key("nucleotides");
          SB_FOR(SBStructuralNode* n, nts) {
            ADNPointer<ADNNucleotide> nt = static_cast<ADNNucleotide*>(n);
            ntList.push_back(nt->getNodeIndex());
          }
          std::string strRight = ADNAuxiliary::VectorToString(ntList);
          writer.String(strRight.c_str());
        }
        writer.EndObject();  // end right loop
      }

      writer.EndObject();  // end cell

      writer.EndObject();  // end base segment
    }
    writer.EndObject();  // end bases

    writer.EndObject();  // end of ds
  }
  writer.EndObject();  // end of double strands

  auto singleStrands = p->GetSingleStrands();
  writer.Key("singleStrands");
  writer.StartObject();
  SB_FOR(SBStructuralNode* p, singleStrands) {
    ADNPointer<ADNSingleStrand> ss = static_cast<ADNSingleStrand*>(p);

    std::string key = std::to_string(ss->getNodeIndex());
    writer.Key(key.c_str());
    writer.StartObject();

    writer.Key("chainName");
    writer.String(ss->GetName().c_str());

    writer.Key("isScaffold");
    writer.Bool(ss->IsScaffold());

    writer.Key("fivePrimeId");
    writer.Int(ss->GetFivePrime()->getNodeIndex());

    writer.Key("threePrimeId");
    writer.Int(ss->GetThreePrime()->getNodeIndex());

    auto nucleotides = ss->GetNucleotides();
    writer.Key("nucleotides");
    writer.StartObject();
    SB_FOR(SBStructuralNode* pair, nucleotides) {
      ADNPointer<ADNNucleotide> nt = static_cast<ADNNucleotide*>(pair);

      std::string key = std::to_string(nt->getNodeIndex());
      writer.Key(key.c_str());
      writer.StartObject();

      writer.Key("type");
      char t = ADNModel::ResidueNameToType(nt->GetType());
      std::string typ = std::string(&t, 0, 1);
      writer.String(typ.c_str());

      int pairId = -1;
      if (nt->GetPair() != nullptr) pairId = nt->GetPair()->getNodeIndex();
      writer.Key("pair");
      writer.Int(pairId);

      int prevId = -1;
      if (nt->GetPrev() != nullptr) prevId = nt->GetPrev()->getNodeIndex();
      writer.Key("prev");
      writer.Int(prevId);

      int nextId = -1;
      if (nt->GetNext() != nullptr) nextId = nt->GetNext()->getNodeIndex();
      writer.Key("next");
      writer.Int(nextId);

      writer.Key("e1");
      writer.String(ADNAuxiliary::UblasVectorToString(nt->GetE1()).c_str());

      writer.Key("e2");
      writer.String(ADNAuxiliary::UblasVectorToString(nt->GetE2()).c_str());

      writer.Key("e3");
      writer.String(ADNAuxiliary::UblasVectorToString(nt->GetE3()).c_str());

      writer.Key("position");
      writer.String(ADNAuxiliary::SBPositionToString(nt->GetPosition()).c_str());

      writer.Key("backboneCenter");
      writer.String(ADNAuxiliary::SBPositionToString(nt->GetBackbonePosition()).c_str());

      writer.Key("sidechainCenter");
      writer.String(ADNAuxiliary::SBPositionToString(nt->GetSidechainPosition()).c_str());

      writer.EndObject();  // end nt
    }
    writer.EndObject();  // end nucleotides

    writer.EndObject();  // end ss
  }
  writer.EndObject();
}

void ADNLoader::SaveNanorobotToJson(ADNNanorobot * nr, std::string filename)
{
  StringBuffer s;
  rapidjson::Writer<StringBuffer> writer(s);
  writer.StartObject();

  writer.Key("version");
  writer.Double(ADNConstants::JSON_FORMAT_VERSION);

  auto parts = nr->GetParts();
  writer.Key("doubleStrands");
  writer.StartObject();
  SB_FOR(ADNPointer<ADNPart> p, parts) {
    SavePartToJson(p, writer);
  }

  writer.EndObject();  // end parts

  writer.EndObject();  // end json document

  QIODevice::OpenModeFlag mode = QIODevice::WriteOnly;

  QFile file(QString::fromStdString(filename));
  file.open(mode);

  QTextStream out(&file);

  out << s.GetString();

  file.close();
}

void ADNLoader::OutputToOxDNA(ADNPointer<ADNPart> part, std::string folder, ADNAuxiliary::OxDNAOptions options)
{
  std::string fnameConf = "config.conf";
  std::ofstream outConf = CreateOutputFile(fnameConf, folder);
  std::string fnameTopo = "topo.top";
  std::ofstream outTopo = CreateOutputFile(fnameTopo, folder);

  auto singleStrands = part->GetSingleStrands();
  SingleStrandsToOxDNA(singleStrands, outConf, outTopo, options);

  outConf.close();
  outTopo.close();
}

void ADNLoader::OutputToOxDNA(ADNNanorobot* nanorobot, std::string folder, ADNAuxiliary::OxDNAOptions options)
{
  if (nanorobot == nullptr) return;

  std::string fnameConf = "config.conf";
  std::ofstream& outConf = CreateOutputFile(fnameConf, folder);
  std::string fnameTopo = "topo.top";
  std::ofstream& outTopo = CreateOutputFile(fnameTopo, folder);

  auto singleStrands = nanorobot->GetSingleStrands();
  SingleStrandsToOxDNA(singleStrands, outConf, outTopo, options);

  outConf.close();
  outTopo.close();
}

void ADNLoader::SingleStrandsToOxDNA(CollectionMap<ADNSingleStrand> singleStrands, std::ofstream& outConf, std::ofstream& outTopo, ADNAuxiliary::OxDNAOptions options)
{
  // config file header
  std::string timeStep = "0";
  std::string boxSizeX = std::to_string(options.boxSizeX_);
  std::string boxSizeY = std::to_string(options.boxSizeY_);
  std::string boxSizeZ = std::to_string(options.boxSizeZ_);
  auto energies = std::tuple<std::string, std::string, std::string>("0.0", "0.0", "0.0");

  outConf << "t = " + timeStep << std::endl;
  outConf << "b = " + boxSizeX + " " + boxSizeY + " " + boxSizeZ << std::endl;
  outConf << "E = " + std::get<0>(energies) + " " + std::get<1>(energies) + " " + std::get<2>(energies) << std::endl;

  // topology file header
  size_t numNt = 0;
  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) numNt += ss->GetNucleotides().size();
  std::string numberNucleotides = std::to_string(numNt);
  std::string numberStrands = std::to_string(singleStrands.size());

  outTopo << numberNucleotides << " " << numberStrands << std::endl;

  // config file: velocity and angular velocity are zero for all
  std::string L = "0 0 0";
  std::string v = "0 0 0";

  // we assign new ids
  unsigned int strandId = 1;
  unsigned int ntId = 0;
  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
    ADNPointer<ADNNucleotide> nt = ss->GetFivePrime();
    do {
      // config file info
      SBPosition3 pos = nt->GetPosition();
      ublas::vector<double> bbVector = nt->GetE2() * (-1.0);
      ublas::vector<double> normal = nt->GetE1() * (-1.0);

      // box size is in nm, so position of nt has to be too
      std::string positionVector = std::to_string(pos[0].getValue() / 1000.0) + " " + std::to_string(pos[1].getValue() / 1000.0) + " " + std::to_string(pos[2].getValue() / 1000.0);
      std::string backboneBaseVector = std::to_string(bbVector[0]) + " " + std::to_string(bbVector[1]) + " " + std::to_string(bbVector[2]);
      std::string normalVector = std::to_string(normal[0]) + " " + std::to_string(normal[1]) + " " + std::to_string(normal[2]);

      outConf << positionVector + " " + backboneBaseVector + " " + normalVector + " " + v + " " + L << std::endl;

      // topology file info
      std::string base(1, ADNModel::GetResidueName(nt->GetType()));
      std::string threePrime = "-1";
      if (nt->GetPrev() != nullptr) threePrime = std::to_string(ntId - 1);
      std::string fivePrime = "-1";
      if (nt->GetNext() != nullptr) fivePrime = std::to_string(ntId + 1);

      outTopo << std::to_string(strandId) + " " + base + " " + threePrime + " " + fivePrime << std::endl;

      nt = nt->GetNext();
      ntId++;
    } while (nt != nullptr);

    ++strandId;
  }
}

std::ofstream ADNLoader::CreateOutputFile(std::string fname, std::string folder)
{
  std::ofstream output(folder + "/" + fname);

  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer, sizeof(buffer), "%d-%m-%Y %I:%M:%S", timeinfo);
  std::string str(buffer);

  output << "## File created with Adenita on " + str + "\n";
  return output;
}

void ADNLoader::OutputToCSV(CollectionMap<ADNPart> parts, std::string fname, std::string folder)
{
  int num = 0;
  std::ofstream& out = CreateOutputFile(fname, folder);
  SB_FOR(ADNPointer<ADNPart> part, parts) {
    auto singleStrands = part->GetSingleStrands();
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto seq = ss->GetSequence();
      out << std::to_string(num) + " " + ss->GetName() + " | length: " + std::to_string(seq.size());
      out << ",";
      out << seq;
      out << "\n";
    }
  }

  out.close();
}

void ADNLoader::SavePartToJson(ADNPointer<ADNPart> p, std::string filename)
{
  StringBuffer s;
	rapidjson::Writer<StringBuffer> writer(s);
	writer.StartObject();
	
  writer.Key("version");
	writer.Double(ADNConstants::JSON_FORMAT_VERSION);
	
  SavePartToJson(p, writer);

  writer.EndObject();  // end json document
	
	QIODevice::OpenModeFlag mode = QIODevice::WriteOnly;
	
	QFile file(QString::fromStdString(filename));
	file.open(mode);
	
	QTextStream out(&file);
	
	out << s.GetString();
	
	file.close();
}

////ANTPart & ANTPart::GenerateANTNanorobot() {
////  ANTNanorobot nr = ANTNanorobot();
////
////  SBNodeIndexer chainIndexer;
////  SAMSON::getActiveDocument()->getNodes(chainIndexer, SBNode::IsType(SBNode::Chain));
////  
////  int chainIndex = 0;
////  std::pair<int, size_t> max_strand = std::make_pair(-1, 0);
////  int j_id = 0;
////  int bs_id = 0;
////  SBPosition3 offset = SBPosition3();
////  offset[0] = 0.5 * SBQuantity::angstrom(ANTConstants::BP_RISE * 10);
////  SBVector3 dir = SBVector3();
////
////  for (unsigned int i = 0; i < chainIndexer.size(); i++) {
////    SBNode* chain = chainIndexer[i];
////    ANTSingleStrand * singleStrand = new ANTSingleStrand();
////    singleStrand->chain_ = static_cast<SBChain*>(chain);
////    singleStrand->chainName_ = singleStrand->chain_->getName();
////    singleStrand->id_ = chainIndex;
////    chainIndex++;
////    nr.AddStrand(singleStrand);
////
////    SBNodeIndexer residues;
////    chain->getNodes(residues, SBNode::IsType(SBNode::Residue));
////
////    //add nodes to singlestrands according to the chain index
////    int nucleotideIndex = 0;
////    ANTNucleotide* nt_prev = nullptr;
////    BaseSegment* prev_bs = nullptr;
////    Joint* prev_j = new Joint();
////    prev_j->id_ = j_id;
////    ++j_id;
////    nr.AddJoint(prev_j);
////
////    for (unsigned int j = 0; j < residues.size(); j++) {
////      SBResidue* residue = static_cast<SBResidue*>(residues[j]);
////      std::string residueName = residue->getName();
////
////      SBNodeIndexer backboneList;
////      residue->getNodes(backboneList, SBNode::IsType(SBNode::Backbone));
////      SBNode* backbone = backboneList[0];
////
////      SBNodeIndexer sidechainList;
////      residue->getNodes(sidechainList, SBNode::IsType(SBNode::SideChain));
////      SBNode* sidechain = sidechainList[0];
////
////      ANTNucleotide * nucleotide = new ANTNucleotide(residue, backbone, sidechain);
////      nucleotide->id_ = nucleotideIndex;
////      singleStrand->nucleotides_.insert(std::make_pair(nucleotide->id_, nucleotide));
////
////      nucleotide->SetBackboneCenter(ANTVectorMath::CalculateCM(backbone));
////      nucleotide->SetSidechainCenter(ANTVectorMath::CalculateCM(sidechain));
////      SBPosition3 s = ANTVectorMath::CalculateCM(residue);
////      nucleotide->SetPosition(s);
////
////      // update dir and offset
////      if (j + 1 < residues.size()) {
////        SBResidue* next_res = static_cast<SBResidue*>(residues[j + 1]);
////        SBPosition3 t = ANTVectorMath::CalculateCM(next_res);
////        dir = (t - s).normalizedVersion();
////        offset = 0.5 * SBQuantity::angstrom(ANTConstants::BP_RISE * 10) * (prev_j->position_ - s).normalizedVersion();
////      }
////
////      SBSideChain* sc = static_cast<SBSideChain*>(sidechain);
////      SBBackbone* bb = static_cast<SBBackbone*>(backbone);
////      nucleotide->residue_ = residue;
////      nucleotide->sidechain_ = sc;
////      nucleotide->backbone_ = bb;
////
////      char inputBaseSymbol = ANTAuxiliary::getBaseSymbol(sidechain);
////      nucleotide->type_ = residue_names_.right.at(inputBaseSymbol);
////
////      nucleotide->prev_ = nt_prev;
////      if (nt_prev != nullptr) nt_prev->next_ = nucleotide;
////
////      // add basesegments and joints
////      if (j == 0) prev_j->position_ = s - offset;
////      Joint* jo = new Joint();
////      jo->id_ = j_id;
////      ++j_id;
////      jo->position_ = s + offset;
////      JointPair jpair = std::make_pair(prev_j, jo);
////      BaseSegment* bs = new BaseSegment(prev_j, jo);
////      nr.AddJoint(jo);
////      nr.AddBaseSegment(jpair, bs);
////      bs->previous_ = prev_bs;
////      if (prev_bs != nullptr) prev_bs->next_ = bs;
////      bs->id_ = bs_id;
////      ++bs_id;
////      bs->number_ = j;
////      bs->direction_ = dir;
////      bs->nt_ = nucleotide;
////
////      nucleotideIndex++;
////      nt_prev = nucleotide;
////      prev_j = jo;
////    }
////
////    //set nucleotide as threePrime and fivePrime ends
////    singleStrand->SetNucleotidesEnd();
////    size_t num_nts = singleStrand->nucleotides_.size();
////    if (num_nts > max_strand.second) max_strand = std::make_pair(singleStrand->id_, num_nts);
////  }
////  nr.SetScaffold(max_strand.first);
////  nr.SetNtSegmentMap();
////  return nr;
////}
