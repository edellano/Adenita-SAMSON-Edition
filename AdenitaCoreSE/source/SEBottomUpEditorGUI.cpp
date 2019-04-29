#include "SEBottomUpEditorGUI.hpp"
#include "SEBottomUpEditor.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SEBottomUpEditorGUI::SEBottomUpEditorGUI(SEBottomUpEditor* editor) {

	ui.setupUi( this );
	this->editor = editor;

  auto dh_radius = SBQuantity::nanometer(ADNConstants::DH_DIAMETER)*0.5;
  SBQuantity::length maxCutOff = dh_radius + SBQuantity::nanometer(0.2);
  SBQuantity::length minCutOff = dh_radius - SBQuantity::nanometer(0.1);
  double maxAngle = 49.0;

  ui.sldMax->setValue(int( maxCutOff.getValue() / 10 ));
  ui.sldMin->setValue(int( minCutOff.getValue() / 10 ));
  ui.sldAngle->setValue(int(maxAngle*10));
}

SEBottomUpEditorGUI::~SEBottomUpEditorGUI() {

}

SEBottomUpEditor* SEBottomUpEditorGUI::getEditor() const { return editor; }

void SEBottomUpEditorGUI::loadSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;
	
	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SEBottomUpEditorGUI::saveSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;

	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SEBottomUpEditorGUI::onGenerateModel()
{
  SEBottomUpEditor* editor = getEditor();

  // as the sliders use ints, we increase the range
  double maxCutOff = 0.1 * ui.sldMax->value();
  double minCutOff = 0.1 * ui.sldMin->value();
  double maxAngle = 0.1 * ui.sldAngle->value();

  auto part = editor->Build(maxCutOff, minCutOff, maxAngle);
  if (part != nullptr) editor->sendPartToAdenita(part);
}

void SEBottomUpEditorGUI::onRefreshPartList()
{
  SEBottomUpEditor* editor = getEditor();
  auto indexParts = editor->getPartsList();

  int sel1 = ui.cmbStructuralModel->currentIndex();
  ui.cmbStructuralModel->clear();

  ui.cmbStructuralModel->insertItem(0, QString::fromStdString("None"));

  for (auto& pair : indexParts) {
    int i = pair.first;
    auto p = static_cast<SBStructuralModel*>(pair.second());
    std::string n = p->getName();
    ui.cmbStructuralModel->insertItem(i, QString::fromStdString(n));
  }

  if (indexParts.find(sel1) != indexParts.end()) ui.cmbStructuralModel->setCurrentIndex(sel1);
}

void SEBottomUpEditorGUI::onSelectNode(int s)
{
  SEBottomUpEditor* editor = getEditor();
  editor->SetSelected(s);
}

void SEBottomUpEditorGUI::onMinSliderChanged(int val)
{
  int w = ui.sldMax->value();
  if (w < val) ui.sldMax->setValue(val);
}

SBCContainerUUID SEBottomUpEditorGUI::getUUID() const { return SBCContainerUUID( "CEDFA9EB-32FA-0A21-6216-C053116FCA36" );}

QPixmap SEBottomUpEditorGUI::getLogo() const {
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/icons/SEBottomUpEditorIcon.png"));

}

QString SEBottomUpEditorGUI::getName() const {

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your editor inside SAMSON

	return "SEBottomUpEditor"; 

}

int SEBottomUpEditorGUI::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SEBottomUpEditorGUI::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

  return ADNAuxiliary::AdenitaCitation();
}
