#include "SELatticeCreatorEditorGUI.hpp"
#include "SELatticeCreatorEditor.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SELatticeCreatorEditorGUI::SELatticeCreatorEditorGUI(SELatticeCreatorEditor* editor) {

	ui.setupUi( this );
	this->editor = editor;

}

SELatticeCreatorEditorGUI::~SELatticeCreatorEditorGUI() {

}

SELatticeCreatorEditor* SELatticeCreatorEditorGUI::getEditor() const { return editor; }

void SELatticeCreatorEditorGUI::loadSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;
	
	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SELatticeCreatorEditorGUI::saveSettings( SBGSettings *settings ) {

	if ( settings == NULL ) return;

	// SAMSON Element generator pro tip: complete this function so your editor can save its GUI state from one session to the next

}

void SELatticeCreatorEditorGUI::setHoneycomb()
{
  SELatticeCreatorEditor* t = getEditor();
  t->setLatticeType(LatticeType::Honeycomb);
}

void SELatticeCreatorEditorGUI::setSquare()
{
  SELatticeCreatorEditor* t = getEditor();
  t->setLatticeType(LatticeType::Square);
}

void SELatticeCreatorEditorGUI::onZPatternChanged(int index)
{
  SELatticeCreatorEditor* t = getEditor();
  if (index == 0)
  {
    t->setZPattern(ZLatticePattern::ALLZ);
  }
  else if (index == 1)
  {
    t->setZPattern(ZLatticePattern::TRIANGLE);
  }
  else if (index == 2)
  {
    t->setZPattern(ZLatticePattern::CIRCLE);
  }

}

SBCContainerUUID SELatticeCreatorEditorGUI::getUUID() const { return SBCContainerUUID( "A9C48A6E-8BD1-B387-6A63-1CB8A19C1948" );}

QPixmap SELatticeCreatorEditorGUI::getLogo() const { 
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/icons/SELatticeCreatorEditorIcon.png"));

}

QString SELatticeCreatorEditorGUI::getName() const { 

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your editor inside SAMSON

	return "SELatticeCreatorEditor"; 

}

int SELatticeCreatorEditorGUI::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SELatticeCreatorEditorGUI::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

	return
		"If you use this editor in your work, please cite: <br/>"
		"<br/>"
		"[1] <a href=\"https://www.samson-connect.net\">https://www.samson-connect.net</a><br/>";

}
