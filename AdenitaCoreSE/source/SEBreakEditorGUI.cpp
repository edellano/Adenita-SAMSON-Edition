#include "SEBreakEditorGUI.hpp"
#include "SEBreakEditor.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SEBreakEditorGUI::SEBreakEditorGUI(SEBreakEditor* editor) {

	ui.setupUi( this );
	this->editor = editor;

}

SEBreakEditorGUI::~SEBreakEditorGUI() {

}

SEBreakEditor* SEBreakEditorGUI::getEditor() const { return editor; }

void SEBreakEditorGUI::loadSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;
	
	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SEBreakEditorGUI::saveSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;

	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

SBCContainerUUID SEBreakEditorGUI::getUUID() const { return SBCContainerUUID( "1D8AA945-60B9-A01E-64E9-1998AF6B1418" );}

QPixmap SEBreakEditorGUI::getLogo() const { 
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/icons/break.png"));

}

QString SEBreakEditorGUI::getName() const { 

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your editor inside SAMSON

	return "Break strands"; 

}

int SEBreakEditorGUI::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SEBreakEditorGUI::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

  return ADNAuxiliary::AdenitaCitation();
}

void SEBreakEditorGUI::onSetMode() {
  bool m = ui.rdnFive->isChecked();
  SEBreakEditor* editor = getEditor();
  editor->SetMode(m);
}