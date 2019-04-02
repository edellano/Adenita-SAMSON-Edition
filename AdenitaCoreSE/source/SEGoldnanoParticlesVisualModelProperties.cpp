#include "SEGoldnanoParticlesVisualModelProperties.hpp"
#include "SEGoldnanoParticlesVisualModel.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SEGoldnanoParticlesVisualModelProperties::SEGoldnanoParticlesVisualModelProperties() {

	visualModel = 0;
	ui.setupUi( this );
	observer = new Observer(this);

}

SEGoldnanoParticlesVisualModelProperties::~SEGoldnanoParticlesVisualModelProperties() {

	if (!visualModel.isValid()) return;

	visualModel->disconnectBaseSignalFromSlot(observer(), SB_SLOT(&SEGoldnanoParticlesVisualModelProperties::Observer::onBaseEvent));
	visualModel->disconnectVisualSignalFromSlot(observer(), SB_SLOT(&SEGoldnanoParticlesVisualModelProperties::Observer::onVisualEvent));

}

void SEGoldnanoParticlesVisualModelProperties::loadSettings( SBGSettings *settings ) {

	if ( settings == 0 ) return;
	
	// SAMSON Element generator pro tip: complete this function so this property window can save its GUI state from one session to the next

}

void SEGoldnanoParticlesVisualModelProperties::saveSettings( SBGSettings *settings ) {

	if ( settings == 0 ) return;

	// SAMSON Element generator pro tip: complete this function so this property window can save its GUI state from one session to the next

}

SBCContainerUUID SEGoldnanoParticlesVisualModelProperties::getUUID() const { return SBCContainerUUID( "4B3559C3-7B93-93FE-66B6-EE1B535C3470" );}

QPixmap SEGoldnanoParticlesVisualModelProperties::getLogo() const { 
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your visual model.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/Icons/SEGoldnanoParticlesVisualModelPropertiesIcon.png"));

}

QString SEGoldnanoParticlesVisualModelProperties::getName() const { 

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your visual model inside SAMSON

	return "SEGoldnanoParticlesVisualModel properties"; 

}

int SEGoldnanoParticlesVisualModelProperties::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SEGoldnanoParticlesVisualModelProperties::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

	return
		"If you use this visual model in your work, please cite: <br/>"
		"<br/>"
		"[1] <a href=\"https://www.samson-connect.net\">https://www.samson-connect.net</a><br/>";

}

bool SEGoldnanoParticlesVisualModelProperties::setup() {

	SBNodeIndexer nodeIndexer;
	SB_FOR(SBNode* node, *SAMSON::getActiveDocument()->getSelectedNodes()) node->getNodes(nodeIndexer, SBNode::GetClass() == std::string("SEGoldnanoParticlesVisualModel") && SBNode::GetElement() == std::string("SEGoldnanoParticles") && SBNode::GetElementUUID() == SBUUID(SB_ELEMENT_UUID));

	if (nodeIndexer.size() == 1) {

		visualModel = static_cast<SEGoldnanoParticlesVisualModel*>((nodeIndexer)[0]);
		visualModel->connectBaseSignalToSlot(observer(), SB_SLOT(&SEGoldnanoParticlesVisualModelProperties::Observer::onBaseEvent));
		visualModel->connectVisualSignalToSlot(observer(), SB_SLOT(&SEGoldnanoParticlesVisualModelProperties::Observer::onVisualEvent));

		return true;

	}

	return false;

}

bool SEGoldnanoParticlesVisualModelProperties::setup(SBNode* node) {

	if (node->getProxy()->getName() != "SEGoldnanoParticlesVisualModel") return false;
	if (node->getProxy()->getElement() != "SEGoldnanoParticles") return false;
	if (node->getProxy()->getElementUUID() != SBUUID(SB_ELEMENT_UUID)) return false;

	visualModel = static_cast<SEGoldnanoParticlesVisualModel*>(node);
	visualModel->connectBaseSignalToSlot(observer(), SB_SLOT(&SEGoldnanoParticlesVisualModelProperties::Observer::onBaseEvent));
	visualModel->connectVisualSignalToSlot(observer(), SB_SLOT(&SEGoldnanoParticlesVisualModelProperties::Observer::onVisualEvent));

	return true;

}

SEGoldnanoParticlesVisualModelProperties::Observer::Observer(SEGoldnanoParticlesVisualModelProperties* properties) { this->properties = properties; }
SEGoldnanoParticlesVisualModelProperties::Observer::~Observer() {}

void SEGoldnanoParticlesVisualModelProperties::Observer::onBaseEvent(SBBaseEvent* baseEvent) {

	if (baseEvent->getType() == SBBaseEvent::NodeEraseBegin) properties->hide();

}

void SEGoldnanoParticlesVisualModelProperties::Observer::onVisualEvent(SBVisualEvent* visualEvent) {

	if (visualEvent->getType() == SBVisualEvent::VisualModelChanged) {
		
		// SAMSON Element generator pro tip: modify this function if the property window
		// needs to be updated when the VisualModelChanged event is sent
				
	}

}
