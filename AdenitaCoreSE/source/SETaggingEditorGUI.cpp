#include "SETaggingEditorGUI.hpp"
#include "SETaggingEditor.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SETaggingEditorGUI::SETaggingEditorGUI(SETaggingEditor* editor) {

	ui.setupUi( this );
	this->editor = editor;

}

SETaggingEditorGUI::~SETaggingEditorGUI() {

}

SETaggingEditor* SETaggingEditorGUI::getEditor() const { return editor; }

void SETaggingEditorGUI::loadSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;
	
	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SETaggingEditorGUI::saveSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;

	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

SBCContainerUUID SETaggingEditorGUI::getUUID() const { return SBCContainerUUID( "EA0EDF95-028E-4DE2-236F-49EB9D68BC71" );}

QPixmap SETaggingEditorGUI::getLogo() const { 
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/icons/SETaggingEditorIcon.png"));

}

QString SETaggingEditorGUI::getName() const { 

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your editor inside SAMSON

	return "Tagging Editor"; 

}

int SETaggingEditorGUI::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SETaggingEditorGUI::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

  return ADNAuxiliary::AdenitaCitation();
}

void SETaggingEditorGUI::onModeChanged(bool tags) {
  int m = 0;
  if (!tags) m = 1;

  SETaggingEditor* t = getEditor();
  t->changeMode(m);
}
