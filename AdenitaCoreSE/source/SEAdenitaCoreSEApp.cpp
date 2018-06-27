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

  nanorobot_ = new ADNNanorobot();
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

void SEAdenitaCoreSEApp::SavePart(QString filename)
{
  SBDocument* doc = SAMSON::getActiveDocument();
  SBNodeIndexer nodes;
  doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNPart")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")) );

  // only take one
  ADNPointer<ADNPart> part = nullptr;
  SB_FOR(SBNode* node, nodes) {
    if (node->isSelected()) {
      part = static_cast<ADNPart*>(node);
    }
  }

  ADNLoader::SavePartToJson(part, filename.toStdString());

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

void SEAdenitaCoreSEApp::ImportFromCadnano(QString filename, ADNConstants::CadnanoLatticeType t)
{
  DASCadnano cad = DASCadnano();
  ADNPointer<ADNPart> part = new ADNPart();
  std::string seq = "";

  cad.ParseJSON(filename.toStdString());
  cad.CreateModel(part, seq, t);
  
  AddPartToActiveLayer(part);
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

void SEAdenitaCoreSEApp::ExportToOxDNA(QString folder, ADNAuxiliary::OxDNAOptions options)
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

  if (part == nullptr) {
    // nothing selected: export all
    ADNLoader::OutputToOxDNA(nanorobot_, folder.toStdString(), options);
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

ADNNanorobot * SEAdenitaCoreSEApp::GetNanorobot()
{
  return nanorobot_;
}

void SEAdenitaCoreSEApp::AddPartToActiveLayer(ADNPointer<ADNPart> part)
{
  DASBackToTheAtom btta = DASBackToTheAtom();
  btta.CheckDistances(part);
  btta.SetNucleotidesPostions(part);
  SEConfig& config = SEConfig::GetInstance();
  if (config.use_atomic_details) {
    btta.GenerateAllAtomModel(part);
  }

  SAMSON::beginHolding("Add model");
  part->create();
  SAMSON::getActiveLayer()->addChild(part());
  SAMSON::endHolding();
}
