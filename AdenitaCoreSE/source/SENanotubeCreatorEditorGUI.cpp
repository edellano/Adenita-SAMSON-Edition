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

	return
		"If you use this editor in your work, please cite: <br/>"
		"<br/>"
		"[1] <a href=\"https://www.samson-connect.net\">https://www.samson-connect.net</a><br/>";

}
