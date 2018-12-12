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

  cad.CreateConformations(part);
  AddConformationToActiveLayer(cad.Get3DConformation());
  AddConformationToActiveLayer(cad.Get2DConformation());
  AddConformationToActiveLayer(cad.Get1DConformation());
}

void SEAdenitaCoreSEApp::ExportToSequenceList(QString filename, ADNPointer<ADNPart> part)
{
  // get selected part
  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNPart")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  CollectionMap<ADNPart> parts;
  if (part == nullptr) {
    parts = GetNanorobot()->GetParts();
  }
  else {
    parts.addReferenceTarget(part());
  }

  QFileInfo file = QFileInfo(filename);
  ADNLoader::OutputToCSV(parts, file.fileName().toStdString(), file.path().toStdString());
}

void SEAdenitaCoreSEApp::SetScaffoldSequence(std::string filename)
{

  if (filename.empty()) {
    filename = SB_ELEMENT_PATH + "/Data/m13mp18.fasta";
  }

  std::string s = ReadScaffoldFilename(filename);

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

void SEAdenitaCoreSEApp::ExportToOxDNA(QString folder, ADNAuxiliary::OxDNAOptions options, ADNPointer<ADNPart> part)
{
  // get selected part
  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNPart")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

  CollectionMap<ADNPart> parts;
  if (part == nullptr) {
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
  ADNLogger& logger = ADNLogger::GetLogger();

  clock_t start = clock();
  bool vmAlreadyExist = false;

  SEAdenitaVisualModel* adenitaVm = static_cast<SEAdenitaVisualModel*>(GetVisualModel());
  if (adenitaVm != nullptr) {
    vmAlreadyExist = true;
  }

  if (vmAlreadyExist) {
    adenitaVm->changeScale(adenitaVm->getScale());
  }
  else {
    SBProxy* vmProxy = SAMSON::getProxy("SEAdenitaVisualModel");
    SEAdenitaVisualModel* newVm = vmProxy->createInstance(allNodes);
    newVm->create();
    SAMSON::getActiveLayer()->addChild(newVm);
  }

  logger.LogDebugPassedMilliseconds(start, "ResetVisualModel");
}

SBVisualModel* SEAdenitaCoreSEApp::GetVisualModel()
{
    SBNodeIndexer allNodes;
    SAMSON::getActiveDocument()->getNodes(allNodes);
  
    bool vmAlreadyExist = false;
    SBVisualModel * vm = nullptr;
    SEAdenitaVisualModel * adenitaVm = nullptr; 
    SB_FOR(SBNode* node, allNodes) {
      if (node->getType() == SBNode::VisualModel) {
        vm = static_cast<SBVisualModel*>(node);
        if (vm->getProxy()->getName() == "SEAdenitaVisualModel") {
          vmAlreadyExist = true;
          adenitaVm = static_cast<SEAdenitaVisualModel*>(vm);
          break;
        }
      }
    }
    
    return adenitaVm;
  
  
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

void SEAdenitaCoreSEApp::TwistDoubleHelix(CollectionMap<ADNDoubleStrand> dss, double angle)
{
  DASBackToTheAtom btta = DASBackToTheAtom();
  SEConfig& config = SEConfig::GetInstance();

  SB_FOR(ADNPointer<ADNDoubleStrand> ds, dss) {
    double newDeg = ds->GetInitialTwistAngle() + angle;
    ADNBasicOperations::TwistDoubleHelix(ds, newDeg);
    // recalculate positions
    btta.SetDoubleStrandPositions(ds);
    if (config.use_atomic_details) {
      // todo: calculate all atoms just for a double strand
      //btta.GenerateAllAtomModel(ds);
    }
  }

  if (dss.size() > 0) {
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
    ResetVisualModel();
  }
  
}

void SEAdenitaCoreSEApp::CalculateBindingRegions(int oligoConc, int monovalentConc, int divalentConc)
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
    PIPrimer3& p = PIPrimer3::GetInstance();
    p.Calculate(part, oligoConc, monovalentConc, divalentConc);
  }


}

void SEAdenitaCoreSEApp::TwistDoubleHelix()
{
  double deg = ADNConstants::BP_ROT;
  auto dss = GetNanorobot()->GetSelectedDoubleStrands();

  DASBackToTheAtom btta = DASBackToTheAtom();
  SEConfig& config = SEConfig::GetInstance();

  SB_FOR(ADNPointer<ADNDoubleStrand> ds, dss) {
    double newDeg = ds->GetInitialTwistAngle() + deg;
    ADNBasicOperations::TwistDoubleHelix(ds, newDeg);
    // recalculate positions
    btta.SetDoubleStrandPositions(ds);
    if (config.use_atomic_details) {
      // todo: calculate all atoms just for a double strand
      //btta.GenerateAllAtomModel(ds);
    }
  }
}

void SEAdenitaCoreSEApp::TestNeighbors()
{
  // get selected nucleotide and part
  auto nts = GetNanorobot()->GetSelectedNucleotides();
  if (nts.size() == 0) return;

  ADNPointer<ADNNucleotide> nt = nts[0];
  ADNPointer<ADNPart> part = GetNanorobot()->GetPart(nt->GetStrand());
  // create neighbor list
  SEConfig& config = SEConfig::GetInstance();
  auto neighbors = ADNNeighbors();
  neighbors.SetMaxCutOff(SBQuantity::nanometer(config.debugOptions.maxCutOff));
  neighbors.SetMinCutOff(SBQuantity::nanometer(config.debugOptions.minCutOff));
  neighbors.SetIncludePairs(true);
  neighbors.InitializeNeighbors(part);

  // highlight neighbors of selected nucleotide
  auto ntNeighbors = neighbors.GetNeighbors(nt);
  SB_FOR(ADNPointer<ADNNucleotide> ntN, ntNeighbors) {
    //SBPosition3 posBor = ntN->GetPosition();
    //SBPosition3 dif = posBor - nt->GetPosition();
    //auto e2Bor = ntN->GetE2();
    //// check right directionality and co-planarity
    //double t = ublas::inner_prod(nt->GetE2(), e2Bor);
    //// check that they are "in front" of each other
    //ublas::vector<double> df = ADNAuxiliary::SBPositionToUblas(dif);
    //double n = ublas::inner_prod(nt->GetE2(), df);
    //double angle_threshold = config.debugOptions.customDouble;
    //double th = cos(ADNVectorMath::DegToRad(angle_threshold));
    //if (n > 0 && t < 0.0 && abs(t) > th) {
      ntN->setSelectionFlag(true);
    //}
  }

  ResetVisualModel();
}

void SEAdenitaCoreSEApp::ImportFromOxDNA(std::string topoFile, std::string configFile)
{
  auto res = ADNLoader::InputFromOxDNA(topoFile, configFile);
  if (!res.first) {
    ADNPointer<ADNPart> p = res.second;
    AddPartToActiveLayer(p, false, true);
    ResetVisualModel();
  }
}

void SEAdenitaCoreSEApp::FromDatagraph()
{
  ADNPointer<ADNPart> part = ADNLoader::GenerateModelFromDatagraph();
  AddPartToActiveLayer(part, false, true);
  ResetVisualModel();
}

void SEAdenitaCoreSEApp::onDocumentEvent(SBDocumentEvent* documentEvent)
{
  //ADNLogger& logger = ADNLogger::GetLogger();
  //logger.LogDebug(QString("document has been changed"));
}

void SEAdenitaCoreSEApp::onStructuralEvent(SBStructuralEvent* documentEvent)
{
  if (documentEvent->getType() == SBStructuralEvent::ChainRemoved) {
    auto node = documentEvent->getAuxiliaryNode();
    ADNPointer<ADNSingleStrand> ss = dynamic_cast<ADNSingleStrand*>(node);
    if (ss != nullptr) {
      auto part = static_cast<ADNPart*>(documentEvent->getSender()->getParent());
      part->DeregisterSingleStrand(ss, false);
    }
  }

  //if (documentEvent->getType() == SBStructuralEvent::ResidueRemoved) {
  //  auto node = documentEvent->getAuxiliaryNode();
  //  ADNPointer<ADNSingleStrand> ss = static_cast<ADNSingleStrand*>(documentEvent->getSender());
  //  ADNPointer<ADNNucleotide> nt = dynamic_cast<ADNNucleotide*>(node);
  //  auto part = GetNanorobot()->GetPart(ss);
  //  part->DeregisterNucleotide(nt, false, true, true);
  //}
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

std::string SEAdenitaCoreSEApp::ReadScaffoldFilename(std::string filename)
{
  std::string seq = "";
  if (filename.size() > 0) {
    std::vector<std::string> lines;
    SBIFileReader::getFileLines(filename, lines);
    for (unsigned int i = 1; i < lines.size(); i++) {
      std::string line = lines[i];
      if (line[0] != '>') {
        seq.append(line);
      }
    }
  }

  return seq;
}

QStringList SEAdenitaCoreSEApp::GetPartsNameList()
{
  QStringList names;

  auto parts = GetNanorobot()->GetParts();
  SB_FOR(ADNPointer<ADNPart> p, parts) {
    std::string n = p->GetName();
    names << n.c_str();
  }

  return names;
}

void SEAdenitaCoreSEApp::AddPartToActiveLayer(ADNPointer<ADNPart> part, bool calculatePositions, bool positionsFromNucleotides)
{
  DASBackToTheAtom btta = DASBackToTheAtom();
  btta.PopulateWithMockAtoms(part, positionsFromNucleotides);
  if (calculatePositions) {
    btta.SetNucleotidesPostions(part);
    SEConfig& config = SEConfig::GetInstance();
    if (config.use_atomic_details) {
      btta.GenerateAllAtomModel(part);
    }
    //btta.CheckDistances(part);
  }

  GetNanorobot()->RegisterPart(part);

  SEConfig& c = SEConfig::GetInstance();
  if (c.auto_set_scaffold_sequence) {
    SEAdenitaCoreSEAppGUI* gui = getGUI();
    std::string fname = gui->GetScaffoldFilename();
    std::string seq = ReadScaffoldFilename(fname);
    auto scafs = part->GetScaffolds();
    SB_FOR(ADNPointer<ADNSingleStrand> ss, scafs) {
      ADNBasicOperations::SetSingleStrandSequence(ss, seq);
    }
  }

  //events
  ConnectStructuralSignalSlots(part);

  SAMSON::beginHolding("Add model");
  part->create();
  SAMSON::getActiveLayer()->addChild(part());
  SAMSON::endHolding();

  if (c.auto_calculate_binding_regions) {
    PIPrimer3& p = PIPrimer3::GetInstance();
    //p.Calculate(part, 100, 5, 16);
  }
}

void SEAdenitaCoreSEApp::AddConformationToActiveLayer(ADNPointer<ADNConformation> conf)
{
  GetNanorobot()->RegisterConformation(conf);

  conf->create();

  SAMSON::getActiveDocument()->addChild(conf());
}

void SEAdenitaCoreSEApp::ConnectStructuralSignalSlots(ADNPointer<ADNPart> part)
{
  auto singleStrands = part->GetSingleStrands();

  part->connectStructuralSignalToSlot(
    this,
    SB_SLOT(&SEAdenitaCoreSEApp::onStructuralEvent)
    );
}
