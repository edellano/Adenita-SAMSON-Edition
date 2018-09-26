#include "SEDSDNACreatorEditorGUI.hpp"
#include "SEDSDNACreatorEditor.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SEDSDNACreatorEditorGUI::SEDSDNACreatorEditorGUI(SEDSDNACreatorEditor* editor) {

	ui.setupUi( this );
	this->editor = editor;

}

SEDSDNACreatorEditorGUI::~SEDSDNACreatorEditorGUI() {

}

SEDSDNACreatorEditor* SEDSDNACreatorEditorGUI::getEditor() const { return editor; }

void SEDSDNACreatorEditorGUI::loadSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;
	
	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SEDSDNACreatorEditorGUI::saveSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;

	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SEDSDNACreatorEditorGUI::onSetDSDNA(bool b)
{
  SEDSDNACreatorEditor* t = getEditor();
  t->SetMode(b);
}

void SEDSDNACreatorEditorGUI::onSetSSDNA(bool b)
{
  SEDSDNACreatorEditor* t = getEditor();
  t->SetMode(!b);
}

SBCContainerUUID SEDSDNACreatorEditorGUI::getUUID() const { return SBCContainerUUID( "751903AE-14BC-F0B9-01D9-D2CF8412AEF9" );}

QPixmap SEDSDNACreatorEditorGUI::getLogo() const { 
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/icons/SEDSDNACreatorEditorIcon.png"));

}

QString SEDSDNACreatorEditorGUI::getName() const { 

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your editor inside SAMSON

	return "SEDSDNACreatorEditor"; 

}

int SEDSDNACreatorEditorGUI::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SEDSDNACreatorEditorGUI::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

	return
		"If you use this editor in your work, please cite: <br/>"
		"<br/>"
		"[1] <a href=\"https://www.samson-connect.net\">https://www.samson-connect.net</a><br/>";

}
