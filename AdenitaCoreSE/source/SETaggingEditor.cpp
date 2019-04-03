#include "SETaggingEditor.hpp"
#include "SAMSON.hpp"


SETaggingEditor::SETaggingEditor() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

	propertyWidget = new SETaggingEditorGUI(this);
	propertyWidget->loadDefaultSettings();
	SAMSON::addWidget(propertyWidget);

}

SETaggingEditor::~SETaggingEditor() {

	// SAMSON Element generator pro tip: disconnect from signals you might have connected to.

	propertyWidget->saveDefaultSettings();
	delete propertyWidget;

}

SETaggingEditorGUI* SETaggingEditor::getPropertyWidget() const { return static_cast<SETaggingEditorGUI*>(propertyWidget); }

SBCContainerUUID SETaggingEditor::getUUID() const { return SBCContainerUUID("2A1B195E-3E38-BD20-1B61-70A3CA6761C8"); }

QString SETaggingEditor::getName() const { 

	// SAMSON Element generator pro tip: this name should not be changed

	return "SETaggingEditor"; 

}

QString SETaggingEditor::getText() const { 
	
	// SAMSON Element generator pro tip: modify this function to return a user-friendly string that will be displayed in menus

	return QObject::tr("SETaggingEditor"); 

}

QPixmap SETaggingEditor::getLogo() const {

	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/Icons/SETaggingEditorIcon.png"));

}

QKeySequence SETaggingEditor::getShortcut() const { 
	
	// SAMSON Element generator pro tip: modify this function to associate a tentative shortcut to your editor

	return QKeySequence(""); 

}

QString SETaggingEditor::getToolTip() const { 
	
	// SAMSON Element generator pro tip: modify this function to have your editor display a tool tip in the SAMSON GUI when the mouse hovers the editor's icon

	return QObject::tr("SAMSON Element generator pro tip: modify me"); 

}

void SETaggingEditor::beginEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function when your editor becomes active. 
	// Implement this function if you need to prepare some data structures in order to be able to handle GUI or SAMSON events.

  string iconPath = SB_ELEMENT_PATH + "/Resource/icons/cursor_tagging.png";
  SAMSON::setViewportCursor(QCursor(QPixmap(iconPath.c_str())));
}

void SETaggingEditor::endEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function immediately before your editor becomes inactive (for example when another editor becomes active). 
	// Implement this function if you need to clean some data structures.

}

void SETaggingEditor::getActions(SBVector<SBAction*>& actionVector) {

	// SAMSON Element generator pro tip: SAMSON calls this function to show the user actions associated to your editor in context menus.
	// Append actions to the actionVector if necessary.
	// Please refer to tutorials for examples.

}

void SETaggingEditor::display() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)
  SEConfig& config = SEConfig::GetInstance();

  if (display_) {

    if (shape_ == TaggingShape::Sphere) {
      if (positions_.positionsCounter == 1) {

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        displayActiveSphere();

        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);

      }
    }
    else if (shape_ == TaggingShape::Rod) {

      /*if (positions_.positionsCounter == 1) {

        positions_.SecondPosition = GetSnappedPosition();

        ADNDisplayHelper::displayLine(positions_.FirstPosition, positions_.SecondPosition);


      }

      if (config.preview_editor) {
        if (!circular_) tempPart_ = generateStrand(true);
        else tempPart_ = generateCircularStrand(true);
      }

      if (tempPart_ != nullptr) {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        displayStrand();

        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
      }*/
    }


  }
  else {
    displayInactiveSphere();

  }
}

void SETaggingEditor::displayActiveSphere()
{

  ADNArray<float> color = ADNArray<float>(4);
  color(0) = 1.0f;
  color(1) = 0.1f;
  color(2) = 0.3f;
  color(3) = opaqueness_;

  ADNDisplayHelper::displaySphere(GetSnappedPosition(), radius_, color);

}

void SETaggingEditor::displayInactiveSphere()
{

  ADNArray<float> color = ADNArray<float>(4);
  color(0) = 0.0f;
  color(1) = 0.7f;
  color(2) = 0.7f;
  color(3) = opaqueness_;

  ADNDisplayHelper::displaySphere(GetSnappedPosition(), radius_, color);

}
void SETaggingEditor::displayForShadow() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to compute shadows. 
	// Implement this function if your editor displays things in viewports, so that your editor can cast shadows
	// to other objects in SAMSON, for example thanks to the utility
	// functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

}

void SETaggingEditor::displayInterface() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to display the editor 2D interface in viewports. 
	// Implement this function if your editor displays a 2D user interface. For example, a rectangle selection editor would display a 2D rectangle in the active viewport. 
	// You may use utility functions provided by SAMSON (e.g. displayLinesOrtho and displayTrianglesOrtho).

}

void SETaggingEditor::mousePressEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (positions_.positionsCounter == 0) {
    positions_.FirstPosition = GetSnappedPosition();
    positions_.positionsCounter++;

    positions_.FirstVector = SAMSON::getActiveCamera()->getBasisZ().normalizedVersion();
    positions_.vectorsCounter++;
  }
}

void SETaggingEditor::mouseReleaseEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (positions_.positionsCounter == 1) {
    //positions_.SecondPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());
    positions_.positionsCounter++;

    DASCreatorEditors::resetPositions(positions_);
    display_ = false;

  }
}

void SETaggingEditor::mouseMoveEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (event->buttons() == Qt::LeftButton) {
    display_ = true;
    //SAMSON::requestViewportUpdate();
  }

  SAMSON::requestViewportUpdate();
}

void SETaggingEditor::mouseDoubleClickEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SETaggingEditor::wheelEvent(QWheelEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SETaggingEditor::keyPressEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (display_) {
    SEConfig& config = SEConfig::GetInstance();

    if (event->key() == Qt::Key_Up) {
      opaqueness_ += 0.1f;
      if (opaqueness_ > 1.0f) opaqueness_ = 1.0f;
    }
    else if (event->key() == Qt::Key_Down) {
      opaqueness_ -= 0.1f;
      if (opaqueness_ < 0.0f) opaqueness_ = 0.0f;
    }
    else if (event->key() == Qt::Key_Left) {
      radius_ -= 10.0f;
      if (radius_ < 20.0f) radius_ = 20.0f;
    }
    else if (event->key() == Qt::Key_Right) {
      radius_ += 10.0f;
      if (radius_ > 1000.0f) radius_ = 1000.0f;
    }

    SAMSON::requestViewportUpdate();

  }
}

void SETaggingEditor::keyReleaseEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SETaggingEditor::onBaseEvent(SBBaseEvent* baseEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle base events

}

void SETaggingEditor::onDocumentEvent(SBDocumentEvent* documentEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle document events 

}

void SETaggingEditor::onDynamicalEvent(SBDynamicalEvent* dynamicalEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle dynamical events 

}

void SETaggingEditor::onStructuralEvent(SBStructuralEvent* documentEvent) {
	
	// SAMSON Element generator pro tip: implement this function if you need to handle structural events

}

SBPosition3 SETaggingEditor::GetSnappedPosition()
{
  SBPosition3 currentPosition = SAMSON::getWorldPositionFromViewportPosition(SAMSON::getMousePositionInViewport());

  auto highlightedBaseSegments = nanorobot_->GetHighlightedBaseSegments();
  auto highlightedBaseSegmentsFromNucleotides = nanorobot_->GetHighlightedBaseSegmentsFromNucleotides();
  auto highlightedAtoms = nanorobot_->GetHighlightedAtoms();

  if (highlightedAtoms.size() == 1) {
    currentPosition = highlightedAtoms[0]->getPosition();
  }
  else if (highlightedBaseSegments.size() == 1) {
    currentPosition = highlightedBaseSegments[0]->GetPosition();
  }
  else if (highlightedBaseSegmentsFromNucleotides.size() == 1) {
    currentPosition = highlightedBaseSegmentsFromNucleotides[0]->GetPosition();
  }


  return currentPosition;
}

