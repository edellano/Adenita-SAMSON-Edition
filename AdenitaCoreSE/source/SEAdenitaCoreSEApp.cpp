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
