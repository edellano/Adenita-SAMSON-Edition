#include "SEConnectDSDNAEditorGUI.hpp"
#include "SEConnectDSDNAEditor.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SEConnectDSDNAEditorGUI::SEConnectDSDNAEditorGUI(SEConnectDSDNAEditor* editor) {

	ui.setupUi( this );
	this->editor = editor;

}

SEConnectDSDNAEditorGUI::~SEConnectDSDNAEditorGUI() {

}

SEConnectDSDNAEditor* SEConnectDSDNAEditorGUI::getEditor() const { return editor; }

void SEConnectDSDNAEditorGUI::loadSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;
	
	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SEConnectDSDNAEditorGUI::saveSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;

	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

SBCContainerUUID SEConnectDSDNAEditorGUI::getUUID() const { return SBCContainerUUID( "DF5209B7-1EE0-500C-9BCB-51411BAEC626" );}

QPixmap SEConnectDSDNAEditorGUI::getLogo() const {
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/icons/connectDS.png"));

}

QString SEConnectDSDNAEditorGUI::getName() const {

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your editor inside SAMSON

	return "dsDNA Connect"; 

}

int SEConnectDSDNAEditorGUI::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SEConnectDSDNAEditorGUI::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

	return
		"If you use this editor in your work, please cite: <br/>"
		"<br/>"
		"[1] <a href=\"https://www.samson-connect.net\">https://www.samson-connect.net</a><br/>";

}
