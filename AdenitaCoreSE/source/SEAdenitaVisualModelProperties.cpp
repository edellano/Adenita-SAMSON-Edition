#include "SEAdenitaVisualModelProperties.hpp"
#include "SEAdenitaVisualModel.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SEAdenitaVisualModelProperties::SEAdenitaVisualModelProperties() {

	visualModel = 0;
	ui.setupUi( this );
	observer = new Observer(this);

}

SEAdenitaVisualModelProperties::~SEAdenitaVisualModelProperties() {

	if (!visualModel.isValid()) return;

	visualModel->disconnectBaseSignalFromSlot(observer(), SB_SLOT(&SEAdenitaVisualModelProperties::Observer::onBaseEvent));
	visualModel->disconnectVisualSignalFromSlot(observer(), SB_SLOT(&SEAdenitaVisualModelProperties::Observer::onVisualEvent));

}

void SEAdenitaVisualModelProperties::loadSettings( SBGSettings *settings ) {

	if ( settings == 0 ) return;
	
	// SAMSON Element generator pro tip: complete this function so this property window can save its GUI state from one session to the next

}

void SEAdenitaVisualModelProperties::saveSettings( SBGSettings *settings ) {

	if ( settings == 0 ) return;

	// SAMSON Element generator pro tip: complete this function so this property window can save its GUI state from one session to the next

}

SBCContainerUUID SEAdenitaVisualModelProperties::getUUID() const { return SBCContainerUUID( "D4A5CB4D-13E4-9C27-AE35-B6F690D17E3E" );}

QPixmap SEAdenitaVisualModelProperties::getLogo() const { 
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your visual model.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/Icons/SEAdenitaVisualModelPropertiesIcon.png"));

}

QString SEAdenitaVisualModelProperties::getName() const { 

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your visual model inside SAMSON

	return "SEAdenitaVisualModel properties"; 

}

int SEAdenitaVisualModelProperties::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SEAdenitaVisualModelProperties::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

	return
		"If you use this visual model in your work, please cite: <br/>"
		"<br/>"
		"[1] <a href=\"https://www.samson-connect.net\">https://www.samson-connect.net</a><br/>";

}

bool SEAdenitaVisualModelProperties::setup() {

	SBNodeIndexer nodeIndexer;
  SB_FOR(SBNode* node, *SAMSON::getActiveDocument()->getSelectedNodes()) {
    node->getNodes(nodeIndexer, SBNode::GetClass() == std::string("SEAdenitaVisualModel") && SBNode::GetElement() == std::string("SEAdenitaCoreSE") && SBNode::GetElementUUID() == SBUUID(SB_ELEMENT_UUID));
  }
	if (nodeIndexer.size() == 1) {

		visualModel = static_cast<SEAdenitaVisualModel*>((nodeIndexer)[0]);
		visualModel->connectBaseSignalToSlot(observer(), SB_SLOT(&SEAdenitaVisualModelProperties::Observer::onBaseEvent));
		visualModel->connectVisualSignalToSlot(observer(), SB_SLOT(&SEAdenitaVisualModelProperties::Observer::onVisualEvent));
    connect(ui.hslScale, SIGNAL(sliderMoved(int)), this, SLOT(onSliderScaleChanged(int)));

		return true;

	}

	return false;

}

bool SEAdenitaVisualModelProperties::setup(SBNode* node) {

	if (node->getProxy()->getName() != "SEAdenitaVisualModel") return false;
	if (node->getProxy()->getElement() != "SEAdenitaCoreSE") return false;
	if (node->getProxy()->getElementUUID() != SBUUID(SB_ELEMENT_UUID)) return false;

	visualModel = static_cast<SEAdenitaVisualModel*>(node);
	visualModel->connectBaseSignalToSlot(observer(), SB_SLOT(&SEAdenitaVisualModelProperties::Observer::onBaseEvent));
	visualModel->connectVisualSignalToSlot(observer(), SB_SLOT(&SEAdenitaVisualModelProperties::Observer::onVisualEvent));
  connect(ui.hslScale, SIGNAL(sliderMoved(int)), this, SLOT(onSliderScaleChanged(int)));

	return true;

}

void SEAdenitaVisualModelProperties::onDiscreteSliderScaleChanged(int val)
{
  visualModel->changeDiscreteScale(val, true);
  ui.lblScale->setText(QString::number(val));
}

void SEAdenitaVisualModelProperties::onSliderScaleChanged(int val)
{
  double scale = (double)val / 10.0f;
  visualModel->changeScale(scale, true); //todo this should depend on the previous scale
  ui.lblScale->setText(QString::number(scale));
}

void SEAdenitaVisualModelProperties::onSliderVisibilityChanged(int val)
{
  visualModel->changeVisibility(val / 100.0f);
  SAMSON::requestViewportUpdate();

}

void SEAdenitaVisualModelProperties::onSpinboxVisibilityChanged(double val)
{
  visualModel->changeVisibility(val / 100.0f);
  SAMSON::requestViewportUpdate();
}

void SEAdenitaVisualModelProperties::onSliderDimensionChanged(int val)
{
  visualModel->changeDimension(val); //todo this should depend on the previous scale
  ui.lblDimension->setText(QString::number(val));
}

void SEAdenitaVisualModelProperties::onPropertyColorsChanged(int propertyIdx)
{
  visualModel->changePropertyColors(propertyIdx, 1);
  SAMSON::requestViewportUpdate();
}

SEAdenitaVisualModelProperties::Observer::Observer(SEAdenitaVisualModelProperties* properties) { this->properties = properties; }
SEAdenitaVisualModelProperties::Observer::~Observer() {}

void SEAdenitaVisualModelProperties::Observer::onBaseEvent(SBBaseEvent* baseEvent) {

	if (baseEvent->getType() == SBBaseEvent::NodeEraseBegin) properties->hide();

}

void SEAdenitaVisualModelProperties::Observer::onVisualEvent(SBVisualEvent* visualEvent) {

	if (visualEvent->getType() == SBVisualEvent::VisualModelChanged) {
		
		// SAMSON Element generator pro tip: modify this function if the property window
		// needs to be updated when the VisualModelChanged event is sent
				
	}

}
