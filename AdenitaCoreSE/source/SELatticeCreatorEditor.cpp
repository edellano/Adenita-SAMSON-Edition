#include "SELatticeCreatorEditor.hpp"
#include "SAMSON.hpp"


SELatticeCreatorEditor::SELatticeCreatorEditor() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

	propertyWidget = new SELatticeCreatorEditorGUI(this);
	propertyWidget->loadDefaultSettings();
	SAMSON::addWidget(propertyWidget);

}

SELatticeCreatorEditor::~SELatticeCreatorEditor() {

	// SAMSON Element generator pro tip: disconnect from signals you might have connected to.

	propertyWidget->saveDefaultSettings();
	delete propertyWidget;

}

SELatticeCreatorEditorGUI* SELatticeCreatorEditor::getPropertyWidget() const { return static_cast<SELatticeCreatorEditorGUI*>(propertyWidget); }

ADNPointer<ADNPart> SELatticeCreatorEditor::generateLattice(bool mock /*= false*/)
{
  //ADNLogger& logger = ADNLogger::GetLogger();

  ADNPointer<ADNPart> part = nullptr;

  SBPosition3 firstPos = positions_.FirstPosition;
  SBPosition3 secondPos = positions_.SecondPosition;

  SBPosition3 xPos = positions_.SecondPosition;
  xPos[1].setValue(0);
  SBPosition3 yPos = positions_.SecondPosition;
  yPos[2].setValue(0);
  
  auto x = (positions_.FirstPosition - xPos).norm();
  auto y = (positions_.FirstPosition - yPos).norm();
  auto z = SBQuantity::nanometer(3.4);
  //auto z = (positions_.ThirdPosition - positions_.SecondPosition).norm();
  
  auto xNumStrands = round((x / SBQuantity::nanometer(ADNConstants::DH_DIAMETER)).getValue());
  auto yNumStrands = round((y / SBQuantity::nanometer(ADNConstants::DH_DIAMETER)).getValue());
  auto numNucleotides = round((z / SBQuantity::nanometer(ADNConstants::BP_RISE)).getValue());

  if (numNucleotides > 0 && (xNumStrands > 0 || yNumStrands > 0)) {
    part = new ADNPart();
    
    SBVector3 dir = SBVector3(1, 0, 0);

    for (int xt = 0; xt < xNumStrands; xt++) {
      for (int yt = 0; yt < yNumStrands; yt++) {
        auto pos = vGrid_.GetGridCellPos3D(0, xt, yt);
        pos += positions_.FirstPosition;
        auto ds = DASCreator::CreateDoubleStrand(part, numNucleotides, pos, dir, mock);
      }
    }
    
  }

  return part;
}

void SELatticeCreatorEditor::displayLattice()
{
  ADNDisplayHelper::displayPart(tempPart_);
}

void SELatticeCreatorEditor::sendPartToAdenita(ADNPointer<ADNPart> lattice)
{
  if (lattice != nullptr) {
    SEAdenitaCoreSEApp* adenita = static_cast<SEAdenitaCoreSEApp*>(SAMSON::getApp(SBCContainerUUID("85DB7CE6-AE36-0CF1-7195-4A5DF69B1528"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
    adenita->AddPartToActiveLayer(lattice);
    adenita->ResetVisualModel();
  }
}

SBCContainerUUID SELatticeCreatorEditor::getUUID() const { return SBCContainerUUID("7297F9AE-9237-0720-03B1-B4BDF45D33F9"); }

QString SELatticeCreatorEditor::getName() const { 

	// SAMSON Element generator pro tip: this name should not be changed

	return "SELatticeCreatorEditor"; 

}

QString SELatticeCreatorEditor::getText() const { 
	
	// SAMSON Element generator pro tip: modify this function to return a user-friendly string that will be displayed in menus

	return QObject::tr("SELatticeCreatorEditor"); 

}

QPixmap SELatticeCreatorEditor::getLogo() const {

	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/Icons/SELatticeCreatorEditorIcon.png"));

}

QKeySequence SELatticeCreatorEditor::getShortcut() const { 
	
	// SAMSON Element generator pro tip: modify this function to associate a tentative shortcut to your editor

	return QKeySequence(""); 

}

QString SELatticeCreatorEditor::getToolTip() const { 
	
	// SAMSON Element generator pro tip: modify this function to have your editor display a tool tip in the SAMSON GUI when the mouse hovers the editor's icon

	return QObject::tr("SAMSON Element generator pro tip: modify me"); 

}

void SELatticeCreatorEditor::beginEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function when your editor becomes active. 
	// Implement this function if you need to prepare some data structures in order to be able to handle GUI or SAMSON events.
  SBCamera * camera = SAMSON::getActiveCamera();
  camera->rightView();

  setLatticeType(LatticeType::Square);
}

void SELatticeCreatorEditor::endEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function immediately before your editor becomes inactive (for example when another editor becomes active). 
	// Implement this function if you need to clean some data structures.

}

void SELatticeCreatorEditor::getActions(SBVector<SBAction*>& actionVector) {

	// SAMSON Element generator pro tip: SAMSON calls this function to show the user actions associated to your editor in context menus.
	// Append actions to the actionVector if necessary.
	// Please refer to tutorials for examples.

}

void SELatticeCreatorEditor::display() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

  SEConfig& config = SEConfig::GetInstance();

  if (display_) {
    SBPosition3 currentPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());

    if (positions_.positionsCounter == 1) {
      ADNDisplayHelper::displayLine(positions_.FirstPosition, currentPosition);
    }
    else if (positions_.positionsCounter == 2) {
      ADNDisplayHelper::displayLine(positions_.FirstPosition, positions_.SecondPosition);
      ADNDisplayHelper::displayLine(positions_.SecondPosition, currentPosition);

      positions_.ThirdPosition = currentPosition;
    }

    tempPart_ = generateLattice(true);

    if (tempPart_ != nullptr) {

      glEnable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      displayLattice();

      glDisable(GL_BLEND);
      glDisable(GL_DEPTH_TEST);

    }
  }

}

void SELatticeCreatorEditor::displayForShadow() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to compute shadows. 
	// Implement this function if your editor displays things in viewports, so that your editor can cast shadows
	// to other objects in SAMSON, for example thanks to the utility
	// functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

}

void SELatticeCreatorEditor::displayInterface() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to display the editor 2D interface in viewports. 
	// Implement this function if your editor displays a 2D user interface. For example, a rectangle selection editor would display a 2D rectangle in the active viewport. 
	// You may use utility functions provided by SAMSON (e.g. displayLinesOrtho and displayTrianglesOrtho).

}

void SELatticeCreatorEditor::mousePressEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (positions_.positionsCounter == 0) {
    //SBCamera * camera = SAMSON::getActiveCamera();
    //camera->leftView();
    positions_.FirstPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.positionsCounter++;
  }
  else if (positions_.positionsCounter == 2) {
   
    positions_.ThirdPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.positionsCounter++;
    auto radius = (positions_.ThirdPosition - positions_.SecondPosition).norm();

    ADNPointer<ADNPart> part = generateLattice();
    sendPartToAdenita(part);

    DASCreatorEditors::resetPositions(positions_);
    display_ = false;
    tempPart_ == nullptr;
  }
}

void SELatticeCreatorEditor::mouseReleaseEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (positions_.positionsCounter == 1) {
    //SBCamera * camera = SAMSON::getActiveCamera();
    //camera->bottomView();
    positions_.SecondPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.positionsCounter++;
    
  }

}

void SELatticeCreatorEditor::mouseMoveEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.
  if (event->buttons() == Qt::LeftButton) {
    display_ = true;

    if (positions_.positionsCounter == 1) {
      positions_.SecondPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    }

    //SAMSON::requestViewportUpdate();
  }
  SAMSON::requestViewportUpdate();
}

void SELatticeCreatorEditor::mouseDoubleClickEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SELatticeCreatorEditor::wheelEvent(QWheelEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SELatticeCreatorEditor::keyPressEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SELatticeCreatorEditor::keyReleaseEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SELatticeCreatorEditor::onBaseEvent(SBBaseEvent* baseEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle base events

}

void SELatticeCreatorEditor::onDocumentEvent(SBDocumentEvent* documentEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle document events 

}

void SELatticeCreatorEditor::onDynamicalEvent(SBDynamicalEvent* dynamicalEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle dynamical events 

}

void SELatticeCreatorEditor::onStructuralEvent(SBStructuralEvent* documentEvent) {
	
	// SAMSON Element generator pro tip: implement this function if you need to handle structural events

}

void SELatticeCreatorEditor::setLatticeType(LatticeType type)
{
  vGrid_.CreateLattice(type);
}
