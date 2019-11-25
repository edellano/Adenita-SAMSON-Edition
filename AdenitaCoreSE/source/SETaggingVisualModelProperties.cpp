#include "SETaggingVisualModelProperties.hpp"
#include "SETaggingVisualModel.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"
#include "ADNAuxiliary.hpp"


SETaggingVisualModelProperties::SETaggingVisualModelProperties() {

	visualModel = 0;
	ui.setupUi( this );
	observer = new Observer(this);

}

SETaggingVisualModelProperties::~SETaggingVisualModelProperties() {

	if (!visualModel.isValid()) return;

	visualModel->disconnectBaseSignalFromSlot(observer(), SB_SLOT(&SETaggingVisualModelProperties::Observer::onBaseEvent));
	visualModel->disconnectVisualSignalFromSlot(observer(), SB_SLOT(&SETaggingVisualModelProperties::Observer::onVisualEvent));

}

void SETaggingVisualModelProperties::loadSettings( SBGSettings *settings ) {

	if ( settings == 0 ) return;
	
	// SAMSON Element generator pro tip: complete this function so this property window can save its GUI state from one session to the next

}

void SETaggingVisualModelProperties::saveSettings( SBGSettings *settings ) {

	if ( settings == 0 ) return;

	// SAMSON Element generator pro tip: complete this function so this property window can save its GUI state from one session to the next

}

SBCContainerUUID SETaggingVisualModelProperties::getUUID() const { return SBCContainerUUID( "4B3559C3-7B93-93FE-66B6-EE1B535C3470" );}

QPixmap SETaggingVisualModelProperties::getLogo() const { 
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your visual model.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/Icons/SETaggingVisualModelPropertiesIcon.png"));

}

QString SETaggingVisualModelProperties::getName() const { 

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your visual model inside SAMSON

	return "SETaggingVisualModel properties"; 

}

int SETaggingVisualModelProperties::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SETaggingVisualModelProperties::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

  return ADNAuxiliary::AdenitaCitation();
}

bool SETaggingVisualModelProperties::setup() {

	SBNodeIndexer nodeIndexer;
	SB_FOR(SBNode* node, *SAMSON::getActiveDocument()->getSelectedNodes()) node->getNodes(nodeIndexer, SBNode::GetClass() == std::string("SETaggingVisualModel") && SBNode::GetElement() == std::string("SETagging") && SBNode::GetElementUUID() == SBUUID(SB_ELEMENT_UUID));

	if (nodeIndexer.size() == 1) {

		visualModel = static_cast<SETaggingVisualModel*>((nodeIndexer)[0]);
		visualModel->connectBaseSignalToSlot(observer(), SB_SLOT(&SETaggingVisualModelProperties::Observer::onBaseEvent));
		visualModel->connectVisualSignalToSlot(observer(), SB_SLOT(&SETaggingVisualModelProperties::Observer::onVisualEvent));

		return true;

	}

	return false;

}

bool SETaggingVisualModelProperties::setup(SBNode* node) {

	if (node->getProxy()->getName() != "SETaggingVisualModel") return false;
	if (node->getProxy()->getElement() != "SETagging") return false;
	if (node->getProxy()->getElementUUID() != SBUUID(SB_ELEMENT_UUID)) return false;

	visualModel = static_cast<SETaggingVisualModel*>(node);
	visualModel->connectBaseSignalToSlot(observer(), SB_SLOT(&SETaggingVisualModelProperties::Observer::onBaseEvent));
	visualModel->connectVisualSignalToSlot(observer(), SB_SLOT(&SETaggingVisualModelProperties::Observer::onVisualEvent));

	return true;

}

SETaggingVisualModelProperties::Observer::Observer(SETaggingVisualModelProperties* properties) { this->properties = properties; }
SETaggingVisualModelProperties::Observer::~Observer() {}

void SETaggingVisualModelProperties::Observer::onBaseEvent(SBBaseEvent* baseEvent) {

	if (baseEvent->getType() == SBBaseEvent::NodeEraseBegin) properties->hide();

}

void SETaggingVisualModelProperties::Observer::onVisualEvent(SBVisualEvent* visualEvent) {

	if (visualEvent->getType() == SBVisualEvent::VisualModelChanged) {
		
		// SAMSON Element generator pro tip: modify this function if the property window
		// needs to be updated when the VisualModelChanged event is sent
				
	}

}
