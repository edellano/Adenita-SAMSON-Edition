#include "SEAdenitaCoreSEApp.hpp"
#include "SEAdenitaCoreSEAppGUI.hpp"

SEAdenitaCoreSEApp::SEAdenitaCoreSEApp() {

	setGUI(new SEAdenitaCoreSEAppGUI(this));
	getGUI()->loadDefaultSettings();

  ADNLogger& logger = ADNLogger::GetLogger();
  SEConfig& config = SEConfig::GetInstance();
  if (config.clear_log_file) {
    logger.ClearLog();
  }
  logger.LogDateTime();
  
}

SEAdenitaCoreSEApp::~SEAdenitaCoreSEApp() {

	getGUI()->saveDefaultSettings();
	delete getGUI();

}

SEAdenitaCoreSEAppGUI* SEAdenitaCoreSEApp::getGUI() const { return static_cast<SEAdenitaCoreSEAppGUI*>(SBDApp::getGUI()); }

void SEAdenitaCoreSEApp::LoadPart(QString filename)
{
  ADNPointer<ADNPart> part = ADNLoader::LoadPartFromJson(filename.toStdString());

  AddPartToActiveLayer(part);
}

void SEAdenitaCoreSEApp::SaveFile(QString filename, bool all)
{
  if (all) {
    ADNLoader::SaveNanorobotToJson(GetNanorobot(), filename.toStdString());
  }
  else {
    auto parts = GetNanorobot()->GetSelectedParts();
    auto part = parts[0];  // save first
    ADNLoader::SavePartToJson(part, filename.toStdString());
  }
}

void SEAdenitaCoreSEApp::LoadPartWithDaedalus(QString filename, int minEdgeSize)
{
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
  // Apply algorithm
  DASDaedalus *alg = new DASDaedalus();
  alg->SetMinEdgeLength(minEdgeSize);
  std::string seq = "";
  auto part = alg->ApplyAlgorithm(seq, filename.toStdString());

  AddPartToActiveLayer(part);
}

void SEAdenitaCoreSEApp::ImportFromCadnano(QString filename)
{
  DASCadnano cad = DASCadnano();
  ADNPointer<ADNPart> part = new ADNPart();
  std::string seq = "";

  part = cad.CreateCadnanoPart(filename.toStdString());
  
  AddPartToActiveLayer(part);
}

void SEAdenitaCoreSEApp::ExportToSequenceList(QString filename, bool all)
{
  // get selected part
  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNPart")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  // only take one
  CollectionMap<ADNPart> parts;
  SB_FOR(SBNode* node, nodes) {
    if (node->isSelected()) {
      auto part = static_cast<ADNPart*>(node);
      parts.addReferenceTarget(part);
    }
  }

  if (parts.size() == 0 || all) {
    auto parts = GetNanorobot()->GetParts();
  }

  QFileInfo file = QFileInfo(filename);
  ADNLoader::OutputToCSV(parts, file.fileName().toStdString(), file.path().toStdString());
}

void SEAdenitaCoreSEApp::SetScaffoldSequence(std::string seq)
{
  std::string s = seq;
  if (s.empty()) {
    std::string filename = SB_ELEMENT_PATH + "/Data/m13mp18.fasta";
    std::vector<std::string> lines;
    SBIFileReader::getFileLines(filename, lines);
    for (unsigned int i = 1; i < lines.size(); i++) {
      std::string line = lines[i];
      if (line[0] != '>') {
        s.append(line);
      }
    }
  }

  // get selected part
  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNPart")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  ADNPointer<ADNPart> part = nullptr;
  SB_FOR(SBNode* node, nodes) {
    if (node->isSelected()) {
      part = static_cast<ADNPart*>(node);
    }
  }

  if (part != nullptr) {
    auto scafs = part->GetScaffolds();
    SB_FOR(ADNPointer<ADNSingleStrand> ss, scafs) {
      ADNBasicOperations::SetSingleStrandSequence(ss, s);
    }
  }

}

void SEAdenitaCoreSEApp::ExportToOxDNA(QString folder, ADNAuxiliary::OxDNAOptions options, bool all)
{
  // get selected part
  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNPart")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  // only take one
  ADNPointer<ADNPart> part = nullptr;
  SB_FOR(SBNode* node, nodes) {
    if (node->isSelected()) {
      part = static_cast<ADNPart*>(node);
    }
  }

  if (part == nullptr || all) {
    // nothing selected: export all
    ADNLoader::OutputToOxDNA(GetNanorobot(), folder.toStdString(), options);
  }
  else {
    ADNLoader::OutputToOxDNA(part, folder.toStdString(), options);
  }
}

void SEAdenitaCoreSEApp::CenterPart()
{
  // get selected part
  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNPart")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  // only take one
  ADNPointer<ADNPart> part = nullptr;
  SB_FOR(SBNode* node, nodes) {
    if (node->isSelected()) {
      part = static_cast<ADNPart*>(node);
    }
  }

  if (part != nullptr) ADNBasicOperations::CenterPart(part);
}

void SEAdenitaCoreSEApp::ResetVisualModel() {
  //create visual model per nanorobot
  SBNodeIndexer allNodes;
  SAMSON::getActiveDocument()->getNodes(allNodes);
  ADNLogger& logger = ADNLogger::GetLogger();


  clock_t start = clock();

  bool vmAlreadyExist = false;
  SBPointer<SBVisualModel> vm;
  SB_FOR(SBNode* node, allNodes) {
    if (node->getType() == SBNode::VisualModel) {
      vm = static_cast<SBVisualModel*>(node);
      if (vm->getProxy()->getName() == "SEAdenitaVisualModel") {
        vmAlreadyExist = true;
        break;
      }
    }
  }
  
  if (vmAlreadyExist) {
    SBPointer<SEAdenitaVisualModel> adenitaVm = static_cast<SEAdenitaVisualModel*>(vm());
    adenitaVm->changeScale(adenitaVm->getScale());
  }
  else {
    SBProxy* vmProxy = SAMSON::getProxy("SEAdenitaVisualModel");
    SEAdenitaVisualModel* adenitaVm = vmProxy->createInstance(allNodes);
    adenitaVm->create();
    SAMSON::getActiveLayer()->addChild(adenitaVm);
  }

  logger.LogDebugPassedMilliseconds(start, "ResetVisualModel");
}

void SEAdenitaCoreSEApp::ConnectSingleStrands()
{

  int size1 = GetNanorobot()->GetSingleStrands().size();

  auto nts = GetNanorobot()->GetSelectedNucleotides();
  if (nts.size() == 2) {
    ADNPointer<ADNNucleotide> fPrime = nts[0];
    ADNPointer<ADNNucleotide> tPrime = nts[1];
    if ((fPrime->GetStrand() != tPrime->GetStrand()) && fPrime->IsEnd() && tPrime->IsEnd()) {
      if (fPrime->GetEnd() == ThreePrime && tPrime->GetEnd() == FivePrime) {
        fPrime = nts[1];
        tPrime = nts[0];
      }
      auto fPrimeStrand = fPrime->GetStrand();
      auto tPrimeStrand = tPrime->GetStrand();
      ADNPointer<ADNPart> part = GetNanorobot()->GetPart(tPrimeStrand);
      auto newStrand = ADNBasicOperations::MergeSingleStrands(part, tPrimeStrand, fPrimeStrand);
      GetNanorobot()->RemoveSingleStrand(tPrimeStrand);
      GetNanorobot()->RemoveSingleStrand(fPrimeStrand);

      ResetVisualModel();
    }
  }

}

void SEAdenitaCoreSEApp::BreakSingleStrand()
{
  auto nts = GetNanorobot()->GetSelectedNucleotides();
  if (nts.size() == 1) {
    ADNPointer<ADNNucleotide> nt = nts[0];
    if (nt->GetEnd() != ThreePrime) {
      ADNPointer<ADNSingleStrand> ss = nt->GetStrand();
      ADNPointer<ADNPart> part = GetNanorobot()->GetPart(ss);
      // to break in the 3' direction
      auto ntNext = nt->GetNext();
      if (ntNext != nullptr) {
        auto newStrands = ADNBasicOperations::BreakSingleStrand(part, ntNext);
        GetNanorobot()->RemoveSingleStrand(ss);

        ResetVisualModel();
      }
    }
  }
}

void SEAdenitaCoreSEApp::DeleteNucleotide()
{
  auto numNts = GetNanorobot()->GetNumberOfNucleotides();
  auto numSss = GetNanorobot()->GetNumberOfSingleStrands();

  auto nts = GetNanorobot()->GetSelectedNucleotides();
  if (nts.size() == 1) {
    ADNPointer<ADNNucleotide> nt = nts[0];
    ADNPointer<ADNSingleStrand> ss = nt->GetStrand();
    ADNPointer<ADNPart> part = GetNanorobot()->GetPart(ss);
    auto newStrands = ADNBasicOperations::DeleteNucleotide(part, nt);
    if (ss->getNumberOfNucleotides() == 0) {
      // also delete single strand if was left empty
      ADNBasicOperations::DeleteSingleStrand(ss);
    }
       
    ResetVisualModel();
  }
}

void SEAdenitaCoreSEApp::CreateDSRing(SBQuantity::length radius, SBPosition3 center, SBVector3 normal)
{
  auto part = DASCreator::CreateDSRing(radius, center, normal);
  AddPartToActiveLayer(part);
  ResetVisualModel();
}

void SEAdenitaCoreSEApp::LinearCatenanes(SBQuantity::length radius, SBPosition3 center, SBVector3 normal, int num)
{
  auto part = DASCreator::CreateLinearCatenanes(radius, center, normal, num);
  AddPartToActiveLayer(part);
  ResetVisualModel();
}

void SEAdenitaCoreSEApp::Kinetoplast(SBQuantity::length radius, SBPosition3 center, SBVector3 normal, int rows, int cols)
{
  auto part = DASCreator::CreateHexagonalCatenanes(radius, center, normal, rows, cols);
  AddPartToActiveLayer(part);
  ResetVisualModel();
}

void SEAdenitaCoreSEApp::SetStart()
{
  auto nts = GetNanorobot()->GetSelectedNucleotides();
  if (nts.size() > 1) {
    // order the nts w.r.t. the single strand they belong
    // and perform the operation only once per ss
  }
  else if (nts.size() == 1) {
    auto nt = nts[0];
    ADNBasicOperations::SetStart(nt);
  }

  ResetVisualModel();
}

void SEAdenitaCoreSEApp::MergeComponents()
{
  auto parts = GetNanorobot()->GetSelectedParts();
  if (parts.size() > 1) {
    // Merge the parts in batches of 2
    ADNPointer<ADNPart> fPart = parts[0];
    SB_FOR(ADNPointer<ADNPart> part, parts) {
      if (part == fPart) continue;

      ADNPointer<ADNPart> newPart = ADNBasicOperations::MergeParts(fPart, part);
      GetNanorobot()->DeregisterPart(part);
      part->getParent()->removeChild(part());
      fPart = newPart;
    }
    //GetNanorobot()->RegisterPart(fPart);
    //ResetVisualModel();
  }
}

void SEAdenitaCoreSEApp::CalculateBindingRegions()
{
  // get selected part
  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNPart")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  // only take one
  ADNPointer<ADNPart> part = nullptr;
  SB_FOR(SBNode* node, nodes) {
    if (node->isSelected()) {
      part = static_cast<ADNPart*>(node);
    }
  }

  if (part != nullptr) {
    auto p = PIPrimer3();
    p.Calculate(part, 100, 5, 16);
  }
}

void SEAdenitaCoreSEApp::TwistDoubleHelix()
{
  double deg = ADNConstants::BP_ROT;
  auto dss = GetNanorobot()->GetSelectedDoubleStrands();

  DASBackToTheAtom btta = DASBackToTheAtom();
  SEConfig& config = SEConfig::GetInstance();

  SB_FOR(ADNPointer<ADNDoubleStrand> ds, dss) {
    ADNBasicOperations::TwistDoubleHelix(ds, deg);
    // recalculate positions
    btta.SetDoubleStrandPositions(ds);
    if (config.use_atomic_details) {
      // todo: calculate all atoms just for a double strand
      //btta.GenerateAllAtomModel(ds);
    }
  }
}

void SEAdenitaCoreSEApp::onDocumentEvent(SBDocumentEvent* documentEvent)
{
  ADNLogger& logger = ADNLogger::GetLogger();
  logger.LogDebug(QString("document has been changed"));
}

void SEAdenitaCoreSEApp::onStructuralEvent(SBStructuralEvent* documentEvent)
{
  //ResetVisualModel();
}

ADNNanorobot * SEAdenitaCoreSEApp::GetNanorobot()
{
  auto doc = SAMSON::getActiveDocument();
  ADNNanorobot* nanorobot = nullptr;
  if (nanorobots_.find(doc) == nanorobots_.end()) {
    // create new nanorobot for this document
    nanorobot = new ADNNanorobot();
    nanorobots_.insert(std::make_pair(doc, nanorobot));
  }
  else {
    nanorobot = nanorobots_.at(doc);
  }
  return nanorobot;
}

void SEAdenitaCoreSEApp::AddPartToActiveLayer(ADNPointer<ADNPart> part)
{
  DASBackToTheAtom btta = DASBackToTheAtom();
  btta.SetNucleotidesPostions(part);
  SEConfig& config = SEConfig::GetInstance();
  if (config.use_atomic_details) {
    btta.GenerateAllAtomModel(part);
  }
  btta.CheckDistances(part);

  GetNanorobot()->RegisterPart(part);

  //events
  ConnectStructuralSignalSlots(part);

  SAMSON::beginHolding("Add model");
  part->create();
  SAMSON::getActiveLayer()->addChild(part());
  SAMSON::endHolding();
}

void SEAdenitaCoreSEApp::ConnectStructuralSignalSlots(ADNPointer<ADNPart> part)
{
  auto singleStrands = part->GetSingleStrands();

  part->connectStructuralSignalToSlot(
    this,
    SB_SLOT(&SEAdenitaCoreSEApp::onStructuralEvent)
    );

  //SB_FOR(auto singleStrand, singleStrands) {
  //  auto nucleotides = singleStrand->GetNucleotides();

  //  singleStrand->connectStructuralSignalToSlot(
  //    this,
  //    SB_SLOT(&SEAdenitaCoreSEApp::onStructuralEvent)
  //    );

  //  SB_FOR(auto nucleotide, nucleotides) {
  //    nucleotide->connectStructuralSignalToSlot(
  //      this,
  //      SB_SLOT(&SEAdenitaCoreSEApp::onStructuralEvent)
  //      );
  //  }
  //}


}
