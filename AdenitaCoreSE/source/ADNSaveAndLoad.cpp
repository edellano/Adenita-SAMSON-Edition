#pragma once

#include "ADNSaveAndLoad.hpp"


ADNPointer<ADNPart> ADNLoader::LoadPartFromJson(std::string filename)
{
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

  ADNPointer<ADNPart> part = LoadPartFromJson(d, versionValue);

  return part;
}

ADNPointer<ADNPart> ADNLoader::LoadPartFromJson(Value & val, double versionValue)
{
  ADNPointer<ADNPart> part = new ADNPart();

  Value& d = val;

  std::string name = d["name"].GetString();
  part->SetName(name);

  ElementMap<ADNNucleotide> nts;
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

      ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
      nt->Init();
      nt->SetType(ADNModel::ResidueNameToType(itr2->value["type"].GetString()[0]));
      nt->SetPosition(ADNAuxiliary::StringToSBPosition(itr2->value["position"].GetString()));
      nt->SetBackbonePosition(ADNAuxiliary::StringToSBPosition(itr2->value["backboneCenter"].GetString()));
      nt->SetSidechainPosition(ADNAuxiliary::StringToSBPosition(itr2->value["sidechainCenter"].GetString()));
      nt->SetE1(ADNAuxiliary::StringToUblasVector(itr2->value["e1"].GetString()));
      nt->SetE2(ADNAuxiliary::StringToUblasVector(itr2->value["e2"].GetString()));
      nt->SetE3(ADNAuxiliary::StringToUblasVector(itr2->value["e3"].GetString()));

      if (versionValue >= 0.5) {
        nt->setTag(itr2->value["tag"].GetString());
      }

      nts.Store(nt, std::stoi(itr2->name.GetString()));
      nexts.insert(std::make_pair(std::stoi(itr2->name.GetString()), itr2->value["next"].GetInt()));
      prevs.insert(std::make_pair(std::stoi(itr2->name.GetString()), itr2->value["prev"].GetInt()));
      pairs.insert(std::make_pair(std::stoi(itr2->name.GetString()), itr2->value["pair"].GetInt()));
    }

    int f_id = itr->value["fivePrimeId"].GetInt();

    ADNPointer<ADNNucleotide> nt = nts.Get(f_id).second;
    int currId = f_id;
    do {
      // pairing is done when parsing base segments
      int nextId = nexts.at(currId);
      nt = nts.Get(currId).second;
      part->RegisterNucleotideThreePrime(ss, nt);
      currId = nextId;
    } while (currId != -1);

    part->RegisterSingleStrand(ss);
  }

  ElementMap<ADNBaseSegment> bss;
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
          ntLeft = nts.Get(nt_id_left).second;
          ntLeft->SetBaseSegment(bs);
        }
        ADNPointer<ADNNucleotide> ntRight = nullptr;
        if (nt_id_right > -1) {
          ntRight = nts.Get(nt_id_right).second;
          ntRight->SetBaseSegment(bs);
        }
        bp_cell->AddPair(ntLeft, ntRight);
        bs->SetCell(bp_cell());
      }
      else if (typ == LoopPair) {
        ADNPointer<ADNLoopPair> lp_cell = ADNPointer<ADNLoopPair>(new ADNLoopPair());

        const Value& left = c["leftLoop"];
        const Value& right = c["rightLoop"];

        if (left.HasMember("startNt")) {
          ADNPointer<ADNLoop> leftLoop = ADNPointer<ADNLoop>(new ADNLoop());
          lp_cell->SetLeftLoop(leftLoop);

          int startNtId = left["startNt"].GetInt();
          int endNtId = left["endNt"].GetInt();

          ADNPointer<ADNNucleotide> startNt = nts.Get(startNtId).second;
          ADNPointer<ADNNucleotide> lastNt = nts.Get(endNtId).second;
          ADNPointer<ADNSingleStrand> ss = startNt->GetStrand();
          leftLoop->SetStart(startNt);
          leftLoop->SetEnd(lastNt);

          std::string nucleotides = left["nucleotides"].GetString();
          std::vector<int> ntVec = ADNAuxiliary::StringToVector(nucleotides);
          for (auto &i : ntVec) {
            ADNPointer<ADNNucleotide> nt = nts.Get(i).second;
            leftLoop->AddNucleotide(nt);
            nt->SetBaseSegment(bs);
          }
        }

        if (right.HasMember("startNt")) {
          ADNPointer<ADNLoop> rightLoop = ADNPointer<ADNLoop>(new ADNLoop());
          lp_cell->SetRightLoop(rightLoop);

          int startNtId = right["startNt"].GetInt();
          int endNtId = right["endNt"].GetInt();

          ADNPointer<ADNNucleotide> startNt = nts.Get(startNtId).second;
          ADNPointer<ADNNucleotide> lastNt = nts.Get(endNtId).second;
          ADNPointer<ADNSingleStrand> ss = startNt->GetStrand();
          rightLoop->SetStart(startNt);
          rightLoop->SetEnd(lastNt);

          std::string nucleotides = right["nucleotides"].GetString();
          std::vector<int> ntVec = ADNAuxiliary::StringToVector(nucleotides);
          for (auto &i : ntVec) {
            ADNPointer<ADNNucleotide> nt = nts.Get(i).second;
            rightLoop->AddNucleotide(nt);
            nt->SetBaseSegment(bs);
          }
        }

        bs->SetCell(lp_cell());
      }

      bss.Store(bs, std::stoi(itr2->name.GetString()));
      nextsBs.insert(std::make_pair(std::stoi(itr2->name.GetString()), itr2->value["next"].GetInt()));
      prevsBs.insert(std::make_pair(std::stoi(itr2->name.GetString()), itr2->value["previous"].GetInt()));
    }
    int bsStartId = itr->value["firstBaseSegment"].GetInt();
    int bsEndId = itr->value["lastBaseSegment"].GetInt();

    ADNPointer<ADNBaseSegment> bs = bss.Get(bsStartId).second;
    int currId = bsStartId;
    do {
      int nextId = nextsBs.at(currId);
      bs = bss.Get(currId).second;
      part->RegisterBaseSegmentEnd(ds, bs);
      currId = nextId;
    } while (currId != -1);

    part->RegisterDoubleStrand(ds);
  }

  return part;
}

std::vector<ADNPointer<ADNPart>> ADNLoader::LoadPartsFromJson(std::string filename)
{
  std::vector<ADNPointer<ADNPart>> parts;

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

  if (versionValue < 0.4) return parts;

  Value& partList = d["parts"];
  for (Value::MemberIterator itr = partList.MemberBegin(); itr != partList.MemberEnd(); ++itr) {
    Value& v = itr->value;
    ADNPointer<ADNPart> p = LoadPartFromJson(v, versionValue);
    parts.push_back(p);
  }

  return parts;
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
    ADNLogger::LogError(msg);
  }

  std::string name = d["name"].GetString();
  part->SetName(name);
  //std::string position = d["position"].GetString();
  //part->SetPosition(ADNAuxiliary::StringToSBPosition(position));

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
      ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
      nt->Init();
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

void ADNLoader::SavePartToJson(ADNPointer<ADNPart> p, rapidjson::Writer<StringBuffer>& writer)
{
  writer.Key("name");
  writer.String(p->GetName().c_str());

  auto doubleStrands = p->GetDoubleStrands();
  writer.Key("doubleStrands");
  writer.StartObject();

  ElementMap<ADNNucleotide> ntsMap;
  ElementMap<ADNBaseSegment> bssMap;

  SB_FOR(SBStructuralNode* p, doubleStrands) {
    ADNPointer<ADNDoubleStrand> ds = static_cast<ADNDoubleStrand*>(p);

    std::string key = std::to_string(ds->getNodeIndex());
    writer.Key(key.c_str());
    writer.StartObject();

    writer.Key("firstBaseSegment");
    writer.Int(bssMap.GetIndex(ds->GetFirstBaseSegment()));

    writer.Key("lastBaseSegment");
    writer.Int(bssMap.GetIndex(ds->GetLastBaseSegment()));

    writer.Key("initialTwistAngle");
    writer.Double(ds->GetInitialTwistAngle());

    auto bases = ds->GetBaseSegments();
    writer.Key("bases");
    writer.StartObject();
    SB_FOR(SBStructuralNode* pair, bases) {
      ADNPointer<ADNBaseSegment> bs = static_cast<ADNBaseSegment*>(pair);

      std::string key = std::to_string(bssMap.GetIndex(bs));
      writer.Key(key.c_str());
      writer.StartObject();

      writer.Key("number");
      writer.Int(bs->GetNumber());

      writer.Key("position");
      writer.String(ADNAuxiliary::SBPositionToString(bs->GetPosition()).c_str());

      int nextId = -1;
      if (bs->GetNext() != nullptr) nextId = bssMap.GetIndex(bs->GetNext());
      writer.Key("next");
      writer.Int(nextId);

      int prevId = -1;
      if (bs->GetPrev() != nullptr) prevId = bssMap.GetIndex(bs->GetPrev());
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
        if (bp->GetLeftNucleotide() != nullptr) idLeft = ntsMap.GetIndex(bp->GetLeftNucleotide());
        writer.Key("left");
        writer.Int(idLeft);

        int idRight = -1;
        if (bp->GetRightNucleotide() != nullptr) idRight = ntsMap.GetIndex(bp->GetRightNucleotide());
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
          if (left->GetStart() != nullptr) startNtId = ntsMap.GetIndex(left->GetStart());
          writer.Key("startNt");
          writer.Int(startNtId);

          int endNtId = -1;
          if (left->GetEnd() != nullptr) endNtId = ntsMap.GetIndex(left->GetEnd());
          writer.Key("endNt");
          writer.Int(endNtId);

          auto nts = left->GetNucleotides();
          std::vector<int> ntList;
          writer.Key("nucleotides");
          SB_FOR(SBStructuralNode* n, nts) {
            ADNPointer<ADNNucleotide> nt = static_cast<ADNNucleotide*>(n);
            ntList.push_back(ntsMap.GetIndex(nt));
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
          if (right->GetStart() != nullptr) startNtId = ntsMap.GetIndex(right->GetStart());
          writer.Key("startNt");
          writer.Int(startNtId);

          int endNtId = -1;
          if (right->GetEnd() != nullptr) endNtId = ntsMap.GetIndex(right->GetEnd());
          writer.Key("endNt");
          writer.Int(endNtId);

          auto nts = right->GetNucleotides();
          std::vector<int> ntList;
          writer.Key("nucleotides");
          SB_FOR(SBStructuralNode* n, nts) {
            ADNPointer<ADNNucleotide> nt = static_cast<ADNNucleotide*>(n);
            ntList.push_back(ntsMap.GetIndex(nt));
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
    writer.Int(ntsMap.GetIndex(ss->GetFivePrime()));

    writer.Key("threePrimeId");
    writer.Int(ntsMap.GetIndex(ss->GetThreePrime()));

    auto nucleotides = ss->GetNucleotides();
    writer.Key("nucleotides");
    writer.StartObject();
    SB_FOR(SBStructuralNode* pair, nucleotides) {
      ADNPointer<ADNNucleotide> nt = static_cast<ADNNucleotide*>(pair);

      std::string key = std::to_string(ntsMap.GetIndex(nt));
      writer.Key(key.c_str());
      writer.StartObject();

      writer.Key("type");
      char t = ADNModel::ResidueNameToType(nt->GetType());
      std::string typ = std::string(&t, 0, 1);
      writer.String(typ.c_str());

      int pairId = -1;
      if (nt->GetPair() != nullptr) pairId = ntsMap.GetIndex(nt->GetPair());
      writer.Key("pair");
      writer.Int(pairId);

      int prevId = -1;
      if (nt->GetPrev() != nullptr) prevId = ntsMap.GetIndex(nt->GetPrev());
      writer.Key("prev");
      writer.Int(prevId);

      int nextId = -1;
      if (nt->GetNext() != nullptr) nextId = ntsMap.GetIndex(nt->GetNext());
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

      writer.Key("tag");
      writer.String(nt->getTag().c_str());

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
  writer.Key("parts");
  writer.StartObject();
  SB_FOR(ADNPointer<ADNPart> p, parts) {
    writer.Key(p->GetName().c_str());
    writer.StartObject();
    SavePartToJson(p, writer);
    writer.EndObject();  // end part
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

ADNPointer<ADNPart> ADNLoader::GenerateModelFromDatagraph(SBNode* n)
{
  ADNPointer<ADNPart> part = new ADNPart();

  SBNodeIndexer nodes;
  n->getNodes(nodes, SBNode::IsType(SBNode::Chain));

  SB_FOR(SBNode* node, nodes) {
    
    SBPointer<SBChain> chain = static_cast<SBChain*>(node);

    ADNPointer<ADNSingleStrand> ss = new ADNSingleStrand();
    ss->SetDefaultName();
    part->RegisterSingleStrand(ss);

    SBPointerList<SBStructuralNode> children = *chain->getChildren();
    SBPosition3 prevPos;

    SB_FOR(SBStructuralNode* n, children) {
      SBPointer<SBResidue> res = static_cast<SBResidue*>(n);
      if (!res->isNucleicAcid()) continue;

      SBPosition3 pos = SBPosition3();
      SBPosition3 bbPos = SBPosition3();
      SBPosition3 scPos = SBPosition3();
      int count = 0;
      SBPointer<SBBackbone> bb = res->getBackbone();
      SBPointerList<SBStructuralNode> bbAtoms = *bb->getChildren();
      SB_FOR(SBStructuralNode* at, bbAtoms) {
        if (at->getType() == SBNode::Atom) {
          SBPointer<SBAtom> atom = static_cast<SBAtom*>(at);
          pos += atom->getPosition();
          bbPos += atom->getPosition();
          ++count;
        }
      }
      bbPos /= count;

      int scCount = 0;
      SBPointer<SBSideChain> sc = res->getSideChain();
      SBPointerList<SBStructuralNode> scAtoms = *sc->getChildren();
      ublas::matrix<double> scPoints(0, 3);
      SB_FOR(SBStructuralNode* at, scAtoms) {
        if (at->getType() == SBNode::Atom) {
          SBPointer<SBAtom> atom = static_cast<SBAtom*>(at);
          pos += atom->getPosition();
          scPos += atom->getPosition();
          ++count;
          ++scCount;
          // fill matrix
          ublas::vector<double> r = ADNAuxiliary::SBPositionToUblas(atom->getPosition());
          ADNVectorMath::AddRowToMatrix(scPoints, r);
        }
      }
      pos /= count;
      scPos /= scCount;

      // Calculate local axis

      // to calculate e3 we fit the sidechain atoms to a plain, and set e3 as the normal to that plain in the 5' -> 3' direction
      ublas::vector<double> e3 = ADNVectorMath::CalculatePlane(scPoints);

      // we only need C1 position and e3 to find pair:
      // - check neighbors
      // -(e3, e3n) needs to be negative
      // - |(e3, e3n)| amounts to just a few grades:
      //   --> Build planes with C1 and C1n as center, check (C1n-C1) vector is almost contained in the plain
      //   --> Check (C1n-C1) doesn't intersect backbone
      //SBVector3 e3SB = (pos - prevPos).normalizedVersion();
      //SBVector3 e2SB = (scPos - bbPos).normalizedVersion();

      //ublas::vector<double> e3 = ADNAuxiliary::SBVectorToUblasVector(e3SB);
      //ublas::vector<double> e2 = ADNAuxiliary::SBVectorToUblasVector(e2SB);
      //ublas::vector<double> e1 = ADNVectorMath::CrossProduct(e2, e3);

      ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
      nt->Init();
      nt->SetPosition(pos);
      nt->SetBackbonePosition(bbPos);
      nt->SetSidechainPosition(scPos);
      nt->SetE3(e3);
      //nt->SetE2(e2);
      //nt->SetE1(e1);
      nt->SetType(res->getResidueType());

      part->RegisterNucleotideThreePrime(ss, nt);
      prevPos = pos;
    }
    // fix directionality of first nucleotide
    /*if (ss->getNumberOfNucleotides() > 1) {
      auto fPrime = ss->GetFivePrime();
      SBVector3 newE3 = (fPrime->GetNext()->GetPosition() - fPrime->GetPosition()).normalizedVersion();
      ublas::vector<double> e3 = ADNAuxiliary::SBVectorToUblasVector(newE3);
      fPrime->SetE3(e3);
      ublas::vector<double> e1 = ADNVectorMath::CrossProduct(fPrime->GetE2(), e3);
      fPrime->SetE1(e1);
    }*/
    if (ss->getNumberOfNucleotides() == 0) {
      // delete single strands since it's empty
      part->DeregisterSingleStrand(ss);
    }
    
  }

  BuildTopScales(part);

  return part;
}

ADNPointer<ADNPart> ADNLoader::GenerateModelFromDatagraphParametrized(SBNode * sn, SBQuantity::length maxCutOff, SBQuantity::length minCutOff, double maxAngle)
{
  ADNPointer<ADNPart> part = new ADNPart();

  SBNodeIndexer nodes;
  sn->getNodes(nodes, SBNode::IsType(SBNode::Chain));

  SB_FOR(SBNode* node, nodes) {

    SBPointer<SBChain> chain = static_cast<SBChain*>(node);

    ADNPointer<ADNSingleStrand> ss = new ADNSingleStrand();
    ss->SetDefaultName();
    part->RegisterSingleStrand(ss);

    SBPointerList<SBStructuralNode> children = *chain->getChildren();
    SBPosition3 prevPos;

    ublas::vector<double> e3avg(3, 0.0);

    SB_FOR(SBStructuralNode* n, children) {
      SBPointer<SBResidue> res = static_cast<SBResidue*>(n);
      if (!res->isNucleicAcid()) continue;

      SBPosition3 pos = SBPosition3();
      SBPosition3 bbPos = SBPosition3();
      SBPosition3 scPos = SBPosition3();
      int count = 0;
      SBPointer<SBBackbone> bb = res->getBackbone();
      SBPointerList<SBStructuralNode> bbAtoms = *bb->getChildren();
      SB_FOR(SBStructuralNode* at, bbAtoms) {
        if (at->getType() == SBNode::Atom) {
          SBPointer<SBAtom> atom = static_cast<SBAtom*>(at);
          pos += atom->getPosition();
          bbPos += atom->getPosition();
          ++count;
        }
      }
      bbPos /= count;
      int scCount = 0;

      SBPointer<SBSideChain> sc = res->getSideChain();
      SBPointerList<SBStructuralNode> scAtoms = *sc->getChildren();
      SB_FOR(SBStructuralNode* at, scAtoms) {
        if (at->getType() == SBNode::Atom) {
          SBPointer<SBAtom> atom = static_cast<SBAtom*>(at);
          pos += atom->getPosition();
          scPos += atom->getPosition();
          ++count;
          ++scCount;
        }
      }
      pos /= count;
      scPos /= scCount;

      // Calculate local axis
      SBVector3 e3SB = (pos - prevPos).normalizedVersion();
      SBVector3 e2SB = -(scPos - bbPos).normalizedVersion();

      ublas::vector<double> e3 = ADNAuxiliary::SBVectorToUblasVector(e3SB);
      e3avg += e3;
      ublas::vector<double> e2 = ADNAuxiliary::SBVectorToUblasVector(e2SB);
      //ublas::vector<double> e1 = ADNVectorMath::CrossProduct(e2, e3);

      ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
      nt->Init();
      nt->SetPosition(pos);
      //nt->SetE3(e3);
      nt->SetE2(e2);
      //nt->SetE1(e1);
      nt->SetType(res->getResidueType());

      part->RegisterNucleotideThreePrime(ss, nt);
      prevPos = pos;
    }
    // fix directionality of nucleotides
    if (ss->getNumberOfNucleotides() > 1) {
      ublas::vector<double> e3 = e3avg / ss->getNumberOfNucleotides();
      e3 /= ublas::norm_2(e3);
      auto nucleotides = ss->GetNucleotides();
      SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
        auto e2 = nt->GetE2();
        ublas::vector<double> e1 = ADNVectorMath::CrossProduct(e2, e3);
        nt->SetE3(e3);
        nt->SetE1(e1);
      }
    }
    else if (ss->getNumberOfNucleotides() == 0) {
      // delete single strands since it's empty
      part->DeregisterSingleStrand(ss);
    }
    //if (ss->getNumberOfNucleotides() > 1) {
    //  auto fPrime = ss->GetFivePrime();
    //  SBVector3 newE3 = (fPrime->GetNext()->GetPosition() - fPrime->GetPosition()).normalizedVersion();
    //  ublas::vector<double> e3 = ADNAuxiliary::SBVectorToUblasVector(newE3);
    //  fPrime->SetE3(e3);
    //  ublas::vector<double> e1 = ADNVectorMath::CrossProduct(fPrime->GetE2(), e3);
    //  fPrime->SetE1(e1);
    //}
    //else if (ss->getNumberOfNucleotides() == 0) {
    //  // delete single strands since it's empty
    //  part->DeregisterSingleStrand(ss);
    //}

  }

  BuildTopScalesParemetrized(part, maxCutOff, minCutOff, maxAngle);

  return part;
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
    // calculate five prime and three prime ids
    int numNt = ss->getNumberOfNucleotides();
    int fivePrimeId = ntId;
    int threePrimeId = fivePrimeId + numNt;
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
      if (base == "N") base = "R";  // oxDNA uses R for random

      std::string threePrime = "-1";
      auto ntPrev = nt->GetPrev(true);
      if (ntPrev != nullptr) {
        if (ntPrev->GetEnd() == ThreePrime) threePrime = std::to_string(threePrimeId);
        else threePrime = std::to_string(ntId - 1);
      }
      std::string fivePrime = "-1";
      auto ntNext = nt->GetNext(true);
      if (ntNext != nullptr) {
        if (ntNext->GetEnd() == FivePrime) fivePrime = std::to_string(fivePrimeId);
        else fivePrime = std::to_string(ntId + 1);
      }

      outTopo << std::to_string(strandId) + " " + base + " " + threePrime + " " + fivePrime << std::endl;

      nt = nt->GetNext();
      ntId++;
    } while (nt != nullptr);

    ++strandId;
  }
}

std::ofstream ADNLoader::CreateOutputFile(std::string fname, std::string folder, bool sign)
{
  std::ofstream output(folder + "/" + fname);

  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer, sizeof(buffer), "%d-%m-%Y %I:%M:%S", timeinfo);
  std::string str(buffer);

  if (sign) {
    output << "## File created with Adenita on " + str + "\n";
  }
  
  return output;
}

std::pair<bool, ADNPointer<ADNPart>> ADNLoader::InputFromOxDNA(std::string topoFile, std::string configFile)
{
  ADNPointer<ADNPart> part = new ADNPart();
  bool error = false;
  std::vector<NucleotideWrap> oxDNAIndices;

  // parse topology file
  std::ifstream topo(topoFile);
  if (topo.is_open()) {

    std::string line;
    ADNPointer<ADNSingleStrand> ss;
    int currChain = -1;
    int currNt = 0;
    bool fstLine = true;
    error = false;

    while (std::getline(topo, line)) {
      if (fstLine) {
        fstLine = false;
        continue;  // first line of topology contains number of chains and nucleotides
      }

      std::vector<std::string> cont;
      boost::split(cont, line, boost::is_any_of(" "));
      if (cont.size() != 4) {
        error = true;
        break;
      }

      int numChain = std::stoi(cont[0]);
      char base = cont[1][0];
      int numPrevNt = std::stoi(cont[2]);
      int numNextNt = std::stoi(cont[3]);

      if (numChain != currChain) {
        ss = new ADNSingleStrand();
        ss->SetDefaultName();
        part->RegisterSingleStrand(ss);
        currChain = numChain;
      }

      ADNPointer<ADNNucleotide> nt = new ADNNucleotide();
      nt->Init();
      nt->SetType(ADNModel::ResidueNameToType(base));
      part->RegisterNucleotideThreePrime(ss, nt);

      // if last nucleotide next is same as first close

      // insert wraper
      NucleotideWrap w = NucleotideWrap();
      w.elem_ = nt;
      w.id_ = currNt;
      w.strandId_ = currChain;
      oxDNAIndices.push_back(w);

      currNt++;
    }
  }

  // parse config file and set positions if topology file was parsed correctly
  if (!error) {
    std::ifstream config(configFile);

    if (config.is_open()) {

      std::string line;
      int lineCount = 0;
      error = false;

      while (std::getline(config, line)) {
        if (lineCount > 2) {

          std::vector<std::string> cont;
          boost::split(cont, line, boost::is_any_of(" "));
          if (cont.size() != 15) {
            error = true;
            break;
          }

          double x = std::stod(cont[0]);
          double y = std::stod(cont[1]);
          double z = std::stod(cont[2]);
          double e2x = std::stod(cont[3]);
          double e2y = std::stod(cont[4]);
          double e2z = std::stod(cont[5]);
          double e1x = std::stod(cont[6]);
          double e1y = std::stod(cont[7]);
          double e1z = std::stod(cont[8]);

          if (oxDNAIndices.size() > (lineCount - 3)) {
            SBPosition3 pos = SBPosition3(SBQuantity::nanometer(x), SBQuantity::nanometer(y), SBQuantity::nanometer(z));
            ublas::vector<double> e2 = ublas::vector<double>(3, 0.0);
            e2[0] = e2x;
            e2[1] = e2y;
            e2[2] = e2z;
            ublas::vector<double> e1 = ublas::vector<double>(3, 0.0); 
            e1[0] = e1x;
            e1[1] = e1y;
            e1[2] = e1z;
            ublas::vector<double> e3 = ADNVectorMath::CrossProduct(e1, e2);

            auto w = oxDNAIndices.at(lineCount - 3);
            auto nt = w.elem_;
            nt->SetPosition(pos);
            nt->SetE2(e2);
            nt->SetE1(e1);
            nt->SetE3(e3);
          }
          else {
            error = true;
            break;
          }
        }

        lineCount++;
      }
    }
  }

  if (!error) {
    // create base pairs and double strands
    BuildTopScales(part);

  }
  
  return std::make_pair(error, part);
}

void ADNLoader::OutputToCanDo(ADNNanorobot * nanorobot, std::string filename)
{
  std::ofstream file(filename);
  std::string header = "\"CanDo (.cndo) file format version 1.0, Keyao Pan, Laboratory for Computational Biology "
    "and Biophysics, Massachusetts Institute of Technology, November 2015\"";
  file << header << std::endl;

  // set nucleotide indices
  std::map<ADNNucleotide*, int> nucleotidesId;
  auto singleStrands = nanorobot->GetSingleStrands();
  int ntId = 1;
  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
    auto nt = ss->GetFivePrime();
    while (nt != nullptr) {
      nucleotidesId.insert(std::make_pair(nt(), ntId));
      ++ntId;
    }
  }

  std::string line = "dnaTop,id,up,down,across,seq";
  file << line << std::endl;
  for (auto& p : nucleotidesId) {
    int idx = p.second;
    ADNNucleotide* nt = p.first;
    ADNNucleotide* prevNt = nt->GetPrev(true)();
    ADNNucleotide* nextNt = nt->GetNext(true)();
    ADNNucleotide* pairNt = nt->GetPair()();
    int prevIdx = -1;
    if (prevNt != nullptr) prevIdx = nucleotidesId[prevNt];
    int nextIdx = -1;
    if (nextNt != nullptr) nextIdx = nucleotidesId[nextNt];
    int pairIdx = -1;
    if (pairNt != nullptr) prevIdx = nucleotidesId[pairNt];

    std::string line = std::to_string(idx) + "," + std::to_string(prevIdx) + "," + std::to_string(nextIdx) + "," + std::to_string(pairIdx) + ADNModel::GetResidueName(nt->GetType());
    file << line << std::endl;
  }
  file << line << std::endl;

  auto parts = nanorobot->GetParts();

  line = "dNode, \"e0(1)\", \"e0(2)\", \"e0(3)\"";
  file << line << std::endl;

  std::vector<std::string> triads;
  std::vector<std::string> basePairs;
  int bsId = 1;
  SB_FOR(ADNPointer<ADNPart> part, parts) {
    auto baseSegments = part->GetBaseSegments();
    SB_FOR(ADNPointer<ADNBaseSegment> bs, baseSegments) {
      auto pos = bs->GetPosition();
      std::string line = std::to_string(bsId) + "," + std::to_string(pos[0].getValue() / 1000) + "," + std::to_string(pos[1].getValue() / 1000) + "," + std::to_string(pos[2].getValue() / 1000);
      file << line << std::endl;

      auto e3 = bs->GetE3();
      auto e2 = -1.0 * bs->GetE2();
      auto e1 = ADNVectorMath::CrossProduct(e2, e3);
      std::string t = std::to_string(bsId) + "," + std::to_string(e1[0]) + "," + std::to_string(e1[1]) + "," + std::to_string(e1[2]) + ","
        + std::to_string(e2[0]) + "," + std::to_string(e2[1]) + "," + std::to_string(e2[2]) + ","
        + std::to_string(e3[0]) + "," + std::to_string(e3[1]) + "," + std::to_string(e3[2]);
      triads.push_back(t);

      auto cell = bs->GetCell();
      if (bs->GetCellType() == BasePair) {
        ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(cell());
        int id1 = nucleotidesId[bp->GetLeftNucleotide()()];
        int id2 = nucleotidesId[bp->GetRightNucleotide()()];
        std::string s = std::to_string(bsId) + "," + std::to_string(id1) + "," + std::to_string(id2);
        file << s << std::endl;
      }
      ++bsId;
    }
  }
  file << std::endl;

  line = "triad,\"e1(1)\",\"e1(2)\",\"e1(3)\",\"e2(1)\",\"e2(2)\",\"e2(3)\",\"e3(1)\",\"e3(2)\",\"e3(3)\"";
  file << line << std::endl;
  for (auto s : triads) {
    file << s << std::endl;
  }

  line = "id_nt,id1,id2";
  file << line << std::endl;
  for (auto bp : basePairs) {
    file << bp << std::endl;
  }
}

void ADNLoader::OutputToCanDo(ADNPointer<ADNPart> part, std::string filename)
{
  std::ofstream file(filename);
  std::string header = "\"CanDo (.cndo) file format version 1.0, Keyao Pan, Laboratory for Computational Biology "
    "and Biophysics, Massachusetts Institute of Technology, November 2015\"";
  file << header << std::endl;

  // set nucleotide indices
  std::map<ADNNucleotide*, int> nucleotidesId;
  auto singleStrands = part->GetSingleStrands();
  int ntId = 1;
  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
    auto nt = ss->GetFivePrime();
    while (nt != nullptr) {
      nucleotidesId.insert(std::make_pair(nt(), ntId));
      ++ntId;
      nt = nt->GetNext();
    }
  }
  
  std::string line = "dnaTop,id,up,down,across,seq";
  file << line << std::endl;
  for (auto& p : nucleotidesId) {
    int idx = p.second;
    ADNNucleotide* nt = p.first;
    ADNNucleotide* prevNt = nt->GetPrev(true)();
    ADNNucleotide* nextNt = nt->GetNext(true)();
    ADNNucleotide* pairNt = nt->GetPair()();
    int prevIdx = -1;
    if (prevNt != nullptr) prevIdx = nucleotidesId[prevNt];
    int nextIdx = -1;
    if (nextNt != nullptr) nextIdx = nucleotidesId[nextNt];
    int pairIdx = -1;
    if (pairNt != nullptr) pairIdx = nucleotidesId[pairNt];

    std::string line = std::to_string(idx) + "," + std::to_string(prevIdx) + "," + std::to_string(nextIdx) + "," + std::to_string(pairIdx) + ADNModel::GetResidueName(nt->GetType());
    file << line << std::endl;
  }
  file << line << std::endl;

  line = "dNode, \"e0(1)\", \"e0(2)\", \"e0(3)\"";
  file << line << std::endl;
  auto baseSegments = part->GetBaseSegments();
  std::vector<std::string> triads;
  std::vector<std::string> basePairs;
  int bsId = 1;
  SB_FOR(ADNPointer<ADNBaseSegment> bs, baseSegments) {
    auto pos = bs->GetPosition();
    std::string line = std::to_string(bsId) + "," + std::to_string(pos[0].getValue() / 1000) + "," + std::to_string(pos[1].getValue() / 1000) + "," + std::to_string(pos[2].getValue() / 1000);
    file << line << std::endl;

    auto e3 = bs->GetE3();
    auto e2 = bs->GetE2();
    e2 *= -1.0;
    auto e1 = ADNVectorMath::CrossProduct(e2, e3);
    std::string t = std::to_string(bsId) + "," + std::to_string(e1[0]) + "," + std::to_string(e1[1]) + "," + std::to_string(e1[2]) + ","
      + std::to_string(e2[0]) + "," + std::to_string(e2[1]) + "," + std::to_string(e2[2]) + ","
      + std::to_string(e3[0]) + "," + std::to_string(e3[1]) + "," + std::to_string(e3[2]);
    triads.push_back(t);

    auto cell = bs->GetCell();
    if (bs->GetCellType() == BasePair) {
      ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(cell());
      int id1 = nucleotidesId[bp->GetLeftNucleotide()()];
      int id2 = nucleotidesId[bp->GetRightNucleotide()()];
      std::string s = std::to_string(bsId) + "," + std::to_string(id1) + "," + std::to_string(id2);
      file << s << std::endl;
    }
    ++bsId;
  }
  file << std::endl;

  line = "triad,\"e1(1)\",\"e1(2)\",\"e1(3)\",\"e2(1)\",\"e2(2)\",\"e2(3)\",\"e3(1)\",\"e3(2)\",\"e3(3)\"";
  file << line << std::endl;
  for (auto s : triads) {
    file << s << std::endl;
  }

  line = "id_nt,id1,id2";
  file << line << std::endl;
  for (auto bp : basePairs) {
    file << bp << std::endl;
  }
}

void ADNLoader::BuildTopScalesParemetrized(ADNPointer<ADNPart> part, SBQuantity::length maxCutOff, SBQuantity::length minCutOff, double maxAngle)
{
  auto neighbors = ADNNeighbors();
  neighbors.SetMaxCutOff(maxCutOff);
  neighbors.SetMinCutOff(minCutOff);
  neighbors.SetIncludePairs(true);
  neighbors.InitializeNeighbors(part);

  auto singleStrands = part->GetSingleStrands();
  ADNPointer<ADNDoubleStrand> ds = nullptr;

  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
    ADNPointer<ADNNucleotide> nt = ss->GetFivePrime();
    int number = 0;

    while (nt != nullptr) {
      if (nt->GetBaseSegment() != nullptr) {
        nt = nt->GetNext();
        continue;
      }

      SBPosition3 posNt = nt->GetPosition();
      auto e2Nt = nt->GetE2();
      auto e3Nt = nt->GetE3();

      auto ntBors = neighbors.GetNeighbors(nt);
      SBQuantity::length minDist = SBQuantity::nanometer(ADNConstants::DH_DIAMETER);

      ADNPointer<ADNNucleotide> pair = nullptr;
      // check possible base pairing against the neighbors
      SB_FOR(ADNPointer<ADNNucleotide> bor, ntBors) {
        SBPosition3 posBor = bor->GetPosition();
        SBPosition3 dif = posBor - posNt;
        auto e2Bor = bor->GetE2();
        auto e3Bor = bor->GetE3();
        // check right directionality and co-planarity
        //double s = ublas::inner_prod(e3Nt, e3Bor);
        //if (!ADNVectorMath::IsNearlyZero(s + 1)) continue;

        double t = ublas::inner_prod(e2Nt, e2Bor);
        // check that they are "in front" of each other
        ublas::vector<double> df = ADNAuxiliary::SBPositionToUblas(dif);
        double n = ublas::inner_prod(-e2Nt, df);
        double angle_threshold = maxAngle;
        // check they are complementary
        bool comp = ADNModel::GetComplementaryBase(nt->GetType()) == bor->GetType();
        if (n > 0 && t < 0.0 && abs(t) > cos(ADNVectorMath::DegToRad(angle_threshold)) && comp) {
          // possible paired, take closest
          if (dif.norm() < minDist) {
            pair = bor;
            minDist = dif.norm();
          }
        }
      }

      ADNPointer<ADNBaseSegment> bs = new ADNBaseSegment(CellType::BasePair);
      SBPosition3 bsPos = posNt - SBQuantity::nanometer(ADNConstants::DH_DIAMETER * 0.5) * ADNAuxiliary::UblasVectorToSBVector(e2Nt);
      ublas::vector<double> e3 = nt->GetE3();
      /*ADNLogger& logger = ADNLogger::GetLogger();
      std::string msg = ss->GetName() + ": " + std::to_string(e3[0]) + " " + std::to_string(e3[1]) + " " + std::to_string(e3[2]);
      logger.LogDebug(msg);*/
      ublas::vector<double> e1 = nt->GetE1();
      ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(bs->GetCell()());
      bp->SetLeftNucleotide(nt);
      nt->SetBaseSegment(bs);
      if (pair != nullptr) {
        bp->SetRightNucleotide(pair);
        pair->SetBaseSegment(bs);
        bsPos = (nt->GetPosition() + pair->GetPosition())*0.5;
        bp->PairNucleotides();
      }

      bs->SetPosition(bsPos);
      bs->SetE3(e3);
      bs->SetE2(e2Nt);
      bs->SetE1(e1);
      bs->SetNumber(number);

      // add to corresponding double strand
      if (ds == nullptr) {
        ds = new ADNDoubleStrand();
        part->RegisterDoubleStrand(ds);
      }
      part->RegisterBaseSegmentEnd(ds, bs);

      bool breakDs = false;
      double turningThreshold = 0.0;
      //if (pair == nullptr && nt->GetEnd() != NotEnd) breakDs = true;
      //else if (pair != nullptr && nt->GetEnd() == ThreePrime && pair->GetEnd() == FivePrime) breakDs = true;
      if (nt->GetEnd() != ThreePrime) {
        // if huge change in directionality, make new strand
        ADNPointer<ADNNucleotide> ntNext = nt->GetNext();
        ublas::vector<double> e3Next = ntNext->GetE3();
        auto theta = ublas::inner_prod(e3, e3Next);
        if (theta < 0.9) {
          int test = 1;
        }
        if (theta < turningThreshold) breakDs = true;
      }
      if (pair != nullptr && pair->GetEnd() != FivePrime) {
        // if huge change in directionality, make new strand
        ADNPointer<ADNNucleotide> pairPrev = pair->GetPrev();
        ublas::vector<double> e3Prev = pairPrev->GetE3();
        auto theta = ublas::inner_prod(pair->GetE3(), e3Prev);
        if (theta < turningThreshold) breakDs = true;
      }

      if (breakDs) ds = nullptr;

      ++number;
      nt = nt->GetNext();
    }
  }
}

void ADNLoader::BuildTopScales(ADNPointer<ADNPart> part)
{
  auto dh_radius = SBQuantity::nanometer(ADNConstants::DH_DIAMETER)*0.5;
  SBQuantity::length maxCutOff = dh_radius + SBQuantity::nanometer(0.2);
  SBQuantity::length minCutOff = dh_radius - SBQuantity::nanometer(0.1);
  auto neighbors = ADNNeighbors();
  neighbors.SetMaxCutOff(maxCutOff);
  neighbors.SetMinCutOff(minCutOff);
  neighbors.SetIncludePairs(true);
  neighbors.InitializeNeighbors(part);

  auto singleStrands = part->GetSingleStrands();
  ADNPointer<ADNDoubleStrand> ds = nullptr;

  SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
    ADNPointer<ADNNucleotide> nt = ss->GetFivePrime();
    int number = 0;

    while (nt != nullptr) {
      if (nt->GetBaseSegment() != nullptr) {
        nt = nt->GetNext();
        continue;
      }

      SBPosition3 posNt = nt->GetPosition();
      auto e2Nt = nt->GetE2();

      auto ntBors = neighbors.GetNeighbors(nt);
      double maxCos = 0.0;
      SBQuantity::length minDist = SBQuantity::nanometer(ADNConstants::DH_DIAMETER);

      ADNPointer<ADNNucleotide> pair = nullptr;
      // check possible base pairing against the neighbors
      SB_FOR(ADNPointer<ADNNucleotide> bor, ntBors) {
        SBPosition3 posBor = bor->GetPosition();
        SBPosition3 dif = posBor - posNt;
        auto e2Bor = bor->GetE2();
        // check right directionality and co-planarity
        double t = ublas::inner_prod(e2Nt, e2Bor);
        // check that they are "in front" of each other
        ublas::vector<double> df = ADNAuxiliary::SBPositionToUblas(dif);
        double n = ublas::inner_prod(e2Nt, df);
        double angle_threshold = 49.0;
        // check they are complementary
        bool comp = ADNModel::GetComplementaryBase(nt->GetType()) == bor->GetType();
        if (n > 0 && t < 0.0 && abs(t) > cos(ADNVectorMath::DegToRad(angle_threshold)) && comp) {
          // possible paired, take closest
          if (dif.norm() < minDist) {
            pair = bor;
            minDist = dif.norm();
          }
        }
      }

      ADNPointer<ADNBaseSegment> bs = new ADNBaseSegment(CellType::BasePair);
      SBPosition3 bsPos = posNt + SBQuantity::nanometer(ADNConstants::DH_DIAMETER * 0.5) * ADNAuxiliary::UblasVectorToSBVector(e2Nt);
      ublas::vector<double> e3 = nt->GetE3();
      std::string msg = ss->GetName() + ": " + std::to_string(e3[0]) + " " + std::to_string(e3[1]) + " " + std::to_string(e3[2]);
      ADNLogger::LogDebug(msg);
      ublas::vector<double> e1 = nt->GetE1();
      ADNPointer<ADNBasePair> bp = static_cast<ADNBasePair*>(bs->GetCell()());
      bp->SetLeftNucleotide(nt);
      nt->SetBaseSegment(bs);
      if (pair != nullptr) {
        bp->SetRightNucleotide(pair);
        pair->SetBaseSegment(bs);
        bsPos = (posNt + pair->GetPosition())*0.5;
        bp->PairNucleotides();
      }

      bs->SetPosition(bsPos);
      bs->SetE3(e3);
      bs->SetE2(e2Nt);
      bs->SetE1(e1);
      bs->SetNumber(number);

      // add to corresponding double strand
      if (ds == nullptr) {
        ds = new ADNDoubleStrand();
        part->RegisterDoubleStrand(ds);
      }
      part->RegisterBaseSegmentEnd(ds, bs);

      bool breakDs = false;
      double turningThreshold = 0.0;
      //if (pair == nullptr && nt->GetEnd() != NotEnd) breakDs = true;
      //else if (pair != nullptr && nt->GetEnd() == ThreePrime && pair->GetEnd() == FivePrime) breakDs = true;
      if (nt->GetEnd() != ThreePrime) {
        // if huge change in directionality, make new strand
        ADNPointer<ADNNucleotide> ntNext = nt->GetNext();
        ublas::vector<double> e3Next = ntNext->GetE3();
        auto theta = ublas::inner_prod(e3, e3Next);
        if (theta < 0.9) {
          int test = 1;
        }
        if (theta < turningThreshold) breakDs = true;
      }
      if (pair != nullptr && pair->GetEnd() != FivePrime) {
        // if huge change in directionality, make new strand
        ADNPointer<ADNNucleotide> pairPrev = pair->GetPrev();
        ublas::vector<double> e3Prev = pairPrev->GetE3();
        auto theta = ublas::inner_prod(pair->GetE3(), e3Prev);
        if (theta < turningThreshold) breakDs = true;
      }

      if (breakDs) ds = nullptr;

      ++number;
      nt = nt->GetNext();
    }
  }
}

void ADNLoader::OutputToCSV(CollectionMap<ADNPart> parts, std::string fname, std::string folder)
{
  int num = 0;
  std::ofstream& out = CreateOutputFile(fname, folder);
  SB_FOR(ADNPointer<ADNPart> part, parts) {
    auto singleStrands = part->GetSingleStrands();
    SB_FOR(ADNPointer<ADNSingleStrand> ss, singleStrands) {
      auto seq = ss->GetSequenceWithTags();
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
