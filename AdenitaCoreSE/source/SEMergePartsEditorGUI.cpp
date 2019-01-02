#include "SEMergePartsEditorGUI.hpp"
#include "SEMergePartsEditor.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SEMergePartsEditorGUI::SEMergePartsEditorGUI(SEMergePartsEditor* editor) {

	ui.setupUi( this );
	this->editor = editor;

}

SEMergePartsEditorGUI::~SEMergePartsEditorGUI() {

}

SEMergePartsEditor* SEMergePartsEditorGUI::getEditor() const { return editor; }

void SEMergePartsEditorGUI::loadSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;
	
	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SEMergePartsEditorGUI::saveSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;

	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

SBCContainerUUID SEMergePartsEditorGUI::getUUID() const { return SBCContainerUUID( "3F52AD7B-A478-D380-AA01-2041081D06CB" );}

QPixmap SEMergePartsEditorGUI::getLogo() const {
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/icons/twistDS.png"));

}

QString SEMergePartsEditorGUI::getName() const {

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your editor inside SAMSON

	return "SEMergePartsEditor"; 

}

int SEMergePartsEditorGUI::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SEMergePartsEditorGUI::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

	return
		"If you use this editor in your work, please cite: <br/>"
		"<br/>"
		"[1] <a href=\"https://www.samson-connect.net\">https://www.samson-connect.net</a><br/>";

}
