#include "SEAdenitaCoreSEApp.hpp"
#include "SEAdenitaCoreSEAppGUI.hpp"

SEAdenitaCoreSEApp::SEAdenitaCoreSEApp() {

	setGUI(new SEAdenitaCoreSEAppGUI(this));
	getGUI()->loadDefaultSettings();

}

SEAdenitaCoreSEApp::~SEAdenitaCoreSEApp() {

	getGUI()->saveDefaultSettings();
	delete getGUI();

}

SEAdenitaCoreSEAppGUI* SEAdenitaCoreSEApp::getGUI() const { return static_cast<SEAdenitaCoreSEAppGUI*>(SBDApp::getGUI()); }

void SEAdenitaCoreSEApp::LoadPart(QString filename)
{
  ADNPointer<ADNPart> part = ADNLoader::LoadPartFromJson(filename.toStdString());

  SAMSON::beginHolding("Add model");
  part->create();
  SAMSON::getActiveLayer()->addChild(part());
  SAMSON::endHolding();
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

  DASBackToTheAtom btta = DASBackToTheAtom();
  btta.SetNucleotidesPostions(part);
  SEConfig& config = SEConfig::GetInstance();
  if (config.use_atomic_details) {
    //btta.GenerateAllAtomModel(part);
    btta.SetAllAtomsPostions(part);
  }

  SAMSON::beginHolding("Add model");
  part->create();
  SAMSON::getActiveLayer()->addChild(part());
  SAMSON::endHolding();
}

void SEAdenitaCoreSEApp::ImportFromCadnano(QString filename, ADNConstants::CadnanoLatticeType t)
{
  DASCadnano cad = DASCadnano();
  ADNPointer<ADNPart> part = new ADNPart();
  std::string seq = "";

  cad.ParseJSON(filename.toStdString());
  cad.CreateModel(part, seq, t);
  
  DASBackToTheAtom btta = DASBackToTheAtom();
  btta.SetNucleotidesPostions(part);
  /*SEConfig& config = SEConfig::GetInstance();
  if (config.use_atomic_details) {
  btta.SetAllAtomsPostions(part);
  }*/

  SAMSON::beginHolding("Add model");
  part->create();
  SAMSON::getActiveLayer()->addChild(part());
  SAMSON::endHolding();
}
