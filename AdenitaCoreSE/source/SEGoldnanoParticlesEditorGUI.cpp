#include "SEGoldnanoParticlesEditorGUI.hpp"
#include "SEGoldnanoParticlesEditor.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SEGoldnanoParticlesEditorGUI::SEGoldnanoParticlesEditorGUI(SEGoldnanoParticlesEditor* editor) {

	ui.setupUi( this );
	this->editor = editor;

}

SEGoldnanoParticlesEditorGUI::~SEGoldnanoParticlesEditorGUI() {

}

SEGoldnanoParticlesEditor* SEGoldnanoParticlesEditorGUI::getEditor() const { return editor; }

void SEGoldnanoParticlesEditorGUI::loadSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;
	
	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SEGoldnanoParticlesEditorGUI::saveSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;

	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

SBCContainerUUID SEGoldnanoParticlesEditorGUI::getUUID() const { return SBCContainerUUID( "EA0EDF95-028E-4DE2-236F-49EB9D68BC71" );}

QPixmap SEGoldnanoParticlesEditorGUI::getLogo() const { 
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/icons/SEGoldnanoParticlesEditorIcon.png"));

}

QString SEGoldnanoParticlesEditorGUI::getName() const { 

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your editor inside SAMSON

	return "SEGoldnanoParticlesEditor"; 

}

int SEGoldnanoParticlesEditorGUI::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SEGoldnanoParticlesEditorGUI::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

	return
		"If you use this editor in your work, please cite: <br/>"
		"<br/>"
		"[1] <a href=\"https://www.samson-connect.net\">https://www.samson-connect.net</a><br/>";

}
