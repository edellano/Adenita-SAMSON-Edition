#include "SELatticeCreatorEditor.hpp"
#include "SAMSON.hpp"


SELatticeCreatorEditor::SELatticeCreatorEditor() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

	propertyWidget = new SELatticeCreatorEditorGUI(this);
	propertyWidget->loadDefaultSettings();
	SAMSON::addWidget(propertyWidget);

  setLatticeType(LatticeType::Honeycomb);

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
  SBPosition3 currentPos = positions_.SecondPosition - firstPos;

  SBPosition3 xPos = currentPos;
  xPos[1].setValue(0);
  SBPosition3 yPos = currentPos;
  yPos[2].setValue(0);
  
  auto x = (xPos).norm();
  auto y = (yPos).norm();
  auto z = SBQuantity::nanometer(3.4);

  if (positions_.positionsCounter == 2) {
    SBPosition3 currentPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    z = (currentPosition - firstPos).norm();
  }
  else if (positions_.positionsCounter == 3) {
    z = (positions_.ThirdPosition - firstPos).norm();
  }
  
  auto xNumStrands = round((x / SBQuantity::nanometer(ADNConstants::DH_DIAMETER)).getValue());
  auto yNumStrands = round((y / SBQuantity::nanometer(ADNConstants::DH_DIAMETER)).getValue());
  auto numBps = round((z / SBQuantity::nanometer(ADNConstants::BP_RISE)).getValue());

  if (xNumStrands < 1) xNumStrands = 1;
  if (yNumStrands < 1) yNumStrands = 1;

	//check ranges
	if (lType_ == LatticeType::Square) {
		if (xNumStrands > 55)  xNumStrands = 55;
		if (yNumStrands > 55)  yNumStrands = 55;
	}
	else if (lType_ == LatticeType::Honeycomb) {
		if (xNumStrands > 32)  xNumStrands = 32;
		if (yNumStrands > 30)  yNumStrands = 30;
	}
	
  if (numBps > 0) {
		xyText_ = "x: ";
		xyText_ += to_string(int(xNumStrands));
		xyText_ += " ds / ";
		xyText_ += to_string(int(SBQuantity::nanometer(x).getValue()));
		xyText_ += " nm; ";
		xyText_ += "y: ";
		xyText_ += to_string(int(yNumStrands));
		xyText_ += " ds / ";
		int yLen = int(SBQuantity::nanometer(y).getValue());
		if (lType_ == LatticeType::Honeycomb) yLen * 1.5;
		xyText_ += to_string(yLen);
		xyText_ += " nm; ";
		zText_ = "z: ";
		zText_ += to_string(int(numBps));
		zText_ += " bps / ";
		zText_ += to_string(int(SBQuantity::nanometer(z).getValue()));
		zText_ += " nm; ";

    part = new ADNPart();

    SBVector3 dir = SBVector3(1, 0, 0);
    for (int xt = 0; xt < xNumStrands; xt++) {
      for (int yt = 0; yt < yNumStrands; yt++) {
        auto pos = vGrid_.GetGridCellPos3D(0, xt, yt);
        pos += positions_.FirstPosition;

        int zLength = numBps;
        if (zPattern_ == TRIANGLE) {
          zLength = (xt / xNumStrands) * numBps;
        }
        if (zLength > 0) auto ds = DASCreator::CreateDoubleStrand(part, zLength, pos, dir, mock);
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
    lattice->SetName("Lattice Structure");

    SEAdenitaCoreSEApp* adenita = static_cast<SEAdenitaCoreSEApp*>(SAMSON::getApp(SBCContainerUUID("85DB7CE6-AE36-0CF1-7195-4A5DF69B1528"), SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));
    adenita->AddPartToActiveLayer(lattice);

    //DASCadnano cad = DASCadnano();
    //cad.CreateConformations(lattice);
    //adenita->AddConformationToActiveLayer(cad.Get3DConformation());
    //adenita->AddConformationToActiveLayer(cad.Get2DConformation());
    //adenita->AddConformationToActiveLayer(cad.Get1DConformation());

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

	return QObject::tr("Create dsDNA on a square or honeycomb lattice"); 

}

QString SELatticeCreatorEditor::getDescription() const
{
  return QObject::tr("Adenita | Lattice Editor");
}

void SELatticeCreatorEditor::beginEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function when your editor becomes active. 
	// Implement this function if you need to prepare some data structures in order to be able to handle GUI or SAMSON events.
  SBCamera * camera = SAMSON::getActiveCamera();
  camera->rightView();

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

		tempPart_ = generateLattice(true);

    if (tempPart_ != nullptr) {

      glEnable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      displayLattice();

      glDisable(GL_BLEND);
      glDisable(GL_DEPTH_TEST);

    }

		SBPosition3 offset = SBPosition3(
			SBQuantity::angstrom(5),
			SBQuantity::angstrom(5),
			SBQuantity::angstrom(5));

		if (positions_.positionsCounter == 1) {
			ADNDisplayHelper::displayLine(positions_.FirstPosition, currentPosition);
			
			SBPosition3 xyPos = currentPosition + offset;
			ADNDisplayHelper::displayText(xyPos, xyText_);
		}
		else if (positions_.positionsCounter == 2) {
			ADNDisplayHelper::displayLine(positions_.FirstPosition, positions_.SecondPosition);
			ADNDisplayHelper::displayLine(positions_.FirstPosition, currentPosition);

			SBPosition3 zPos = currentPosition + offset;
			ADNDisplayHelper::displayText(zPos, zText_);

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
    SBCamera * camera = SAMSON::getActiveCamera();
    camera->rightView();
    positions_.FirstPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.positionsCounter++;
  }
  else if (positions_.positionsCounter == 2) {
    positions_.ThirdPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.positionsCounter++;
    
    ADNPointer<ADNPart> part = generateLattice();
    sendPartToAdenita(part);

    DASCreatorEditors::resetPositions(positions_);
    display_ = false;
    tempPart_ == nullptr;

    /*SBCamera * camera = SAMSON::getActiveCamera();
    camera->rightView();*/
  }
}

void SELatticeCreatorEditor::mouseReleaseEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (positions_.positionsCounter == 1) {
    positions_.SecondPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.positionsCounter++;
    /*SBCamera * camera = SAMSON::getActiveCamera();
    camera->topView();*/
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
    else if (positions_.positionsCounter == 2) {
      positions_.ThirdPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
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

  if (event->key() == Qt::Key::Key_Escape) {
    display_ = false;
    DASCreatorEditors::resetPositions(positions_);
    SAMSON::requestViewportUpdate();
  } else if (event->key() == Qt::Key::Key_1) {
    SBCamera * camera = SAMSON::getActiveCamera();
    camera->leftView();
  }
  else if (event->key() == Qt::Key::Key_2) {
    SBCamera * camera = SAMSON::getActiveCamera();
    camera->rightView();
  }
  else if (event->key() == Qt::Key::Key_3) {
    SBCamera * camera = SAMSON::getActiveCamera();
    camera->frontView();
  }
  else if (event->key() == Qt::Key::Key_4) {
    SBCamera * camera = SAMSON::getActiveCamera();
    camera->backView();
  }
  else if (event->key() == Qt::Key::Key_5) {
    SBCamera * camera = SAMSON::getActiveCamera();
    camera->topView();
  }
  else if (event->key() == Qt::Key::Key_6) {
    SBCamera * camera = SAMSON::getActiveCamera();
    camera->bottomView();
  }

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
	lType_ = type;
  SBCamera * camera = SAMSON::getActiveCamera();
  camera->rightView();
}

void SELatticeCreatorEditor::setZPattern(ZLatticePattern pattern)
{
  zPattern_ = pattern;
}
