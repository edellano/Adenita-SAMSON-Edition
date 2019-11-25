#include "SENanotubeCreatorEditorGUI.hpp"
#include "SENanotubeCreatorEditor.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SENanotubeCreatorEditorGUI::SENanotubeCreatorEditorGUI(SENanotubeCreatorEditor* editor) {

	ui.setupUi( this );
	this->editor = editor;

}

SENanotubeCreatorEditorGUI::~SENanotubeCreatorEditorGUI() {

}

SENanotubeCreatorEditor* SENanotubeCreatorEditorGUI::getEditor() const { return editor; }

void SENanotubeCreatorEditorGUI::loadSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;
	
	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SENanotubeCreatorEditorGUI::saveSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;

	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SENanotubeCreatorEditorGUI::updateInfo(SBQuantity::length radius, int numDs, int numBp, bool clear)
{
  if (!clear) {
    ui.txtInfo->setText("Radius (nm): " + QString::number(radius.getValue() / 1000));
    ui.txtInfo->append("Number of double strands: " + QString::number(numDs));
    ui.txtInfo->append("Length (base pairs): " + QString::number(numBp));
  }
  else {
    ui.txtInfo->clear();
  }
}

void SENanotubeCreatorEditorGUI::onPredefinedNanotube(bool predefined)
{
  double radius = ui.spnRadius->value();
  int numBp = ui.spnNumBp->value();
  SENanotubeCreatorEditor* editor = getEditor();
  editor->SetPredefined(predefined, radius, numBp);
}

void SENanotubeCreatorEditorGUI::onRadiusChanged()
{
  double r = ui.spnRadius->value();
  int numDs = ADNVectorMath::CalculateNanotubeDoubleStrands(SBQuantity::nanometer(r));
  ui.spnNumDs->setValue(numDs);
  SENanotubeCreatorEditor* editor = getEditor();
  editor->SetRadius(r);
}

void SENanotubeCreatorEditorGUI::onBpChanged()
{
  int bp = ui.spnNumBp->value();
  SENanotubeCreatorEditor* editor = getEditor();
  editor->SetBp(bp);
}

void SENanotubeCreatorEditorGUI::onNumDsChanged()
{
  int numDs = ui.spnNumDs->value();
  auto r = ADNVectorMath::CalculateNanotubeRadius(numDs);
  ui.spnRadius->setValue(r.getValue() / 1000.0);
}

void SENanotubeCreatorEditorGUI::onChangeRouting()
{
  RoutingType t = RoutingType::None;

  if (ui.chkRoutingSeamless->isChecked()) {
    t = RoutingType::Seamless;
  }
  else if (ui.chkRoutingTiles->isChecked()) {
    t = RoutingType::Tiles;
  }
  else if (ui.chkRoutingNonSeamless->isChecked()) {
    t = RoutingType::NonSeamless;
  }

  SENanotubeCreatorEditor* editor = getEditor();
  editor->SetRouting(t);
}

SBCContainerUUID SENanotubeCreatorEditorGUI::getUUID() const { return SBCContainerUUID( "23177033-1C9B-B441-1916-35EE442706CC" );}

QPixmap SENanotubeCreatorEditorGUI::getLogo() const { 
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/icons/SENanotubeCreatorEditorIcon.png"));

}

QString SENanotubeCreatorEditorGUI::getName() const { 

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your editor inside SAMSON

	return "DNA Nanotube Creator"; 

}

int SENanotubeCreatorEditorGUI::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SENanotubeCreatorEditorGUI::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

  return ADNAuxiliary::AdenitaCitation();

}
