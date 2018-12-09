#include "SEDNATwisterEditor.hpp"
#include "SAMSON.hpp"
#include <QOpenGLFunctions_4_3_Core>


SEDNATwisterEditor::SEDNATwisterEditor() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

	propertyWidget = new SEDNATwisterEditorGUI(this);
	propertyWidget->loadDefaultSettings();
	SAMSON::addWidget(propertyWidget);

  untwistingSphereActive_ = false;
  twistingSphereActive_ = false;
  sphereRadius_ = SBQuantity::angstrom(20.0f);
  spherePosition_ = SBPosition3();
  textPosition_ = SBPosition3();
  altPressed_ = false;
  text_ = "Untwisting";
  SAMSON::requestViewportUpdate();


}

SEDNATwisterEditor::~SEDNATwisterEditor() {

	// SAMSON Element generator pro tip: disconnect from signals you might have connected to.

	propertyWidget->saveDefaultSettings();
	delete propertyWidget;

}

SEDNATwisterEditorGUI* SEDNATwisterEditor::getPropertyWidget() const { return static_cast<SEDNATwisterEditorGUI*>(propertyWidget); }

SBCContainerUUID SEDNATwisterEditor::getUUID() const { return SBCContainerUUID("BF86253A-9F66-9F3C-4039-A711891C8670"); }

QString SEDNATwisterEditor::getName() const { 

	// SAMSON Element generator pro tip: this name should not be changed

	return "SEDNATwisterEditor"; 

}

QString SEDNATwisterEditor::getText() const { 
	
	// SAMSON Element generator pro tip: modify this function to return a user-friendly string that will be displayed in menus

	return QObject::tr("aDeNitA Twister Editor"); 

}

QPixmap SEDNATwisterEditor::getLogo() const {

	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your editor.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/Icons/SEDNATwisterEditorIcon.png"));

}

QKeySequence SEDNATwisterEditor::getShortcut() const { 
	
	// SAMSON Element generator pro tip: modify this function to associate a tentative shortcut to your editor

	return QKeySequence(""); 

}

QString SEDNATwisterEditor::getToolTip() const { 
	
	// SAMSON Element generator pro tip: modify this function to have your editor display a tool tip in the SAMSON GUI when the mouse hovers the editor's icon

	return QObject::tr("Rotate double helix along its axis"); 

}

void SEDNATwisterEditor::beginEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function when your editor becomes active. 
	// Implement this function if you need to prepare some data structures in order to be able to handle GUI or SAMSON events.

}

void SEDNATwisterEditor::endEditing() {

	// SAMSON Element generator pro tip: SAMSON calls this function immediately before your editor becomes inactive (for example when another editor becomes active). 
	// Implement this function if you need to clean some data structures.

}

void SEDNATwisterEditor::getActions(SBVector<SBAction*>& actionVector) {

	// SAMSON Element generator pro tip: SAMSON calls this function to show the user actions associated to your editor in context menus.
	// Append actions to the actionVector if necessary.
	// Please refer to tutorials for examples.

}

void SEDNATwisterEditor::display() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)
  
  float position[3];
  float radius[1];
  float color[4];
  unsigned int flag[1];

  position[0] = spherePosition_[0].getValue();
  position[1] = spherePosition_[1].getValue();
  position[2] = spherePosition_[2].getValue();

  radius[0] = sphereRadius_.getValue();

  if (untwistingSphereActive_) {
    color[0] = 0.f;
    color[1] = 1.f;
    color[2] = 0.f;
    color[3] = 0.3f;
  }
  else if (twistingSphereActive_) {
    color[0] = 0.f;
    color[1] = 0.f;
    color[2] = 1.f;
    color[3] = 0.3f;
  }
  else {
    color[0] = 0.f;
    color[1] = 1.f;
    color[2] = 1.f;
    color[3] = 0.3f;
  }

  flag[0] = 0;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  SAMSON::displaySpheres(1, position, radius, color, flag);

  ADNDisplayHelper::displayText(textPosition_, text_);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
}

void SEDNATwisterEditor::displayForShadow() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to compute shadows. 
	// Implement this function if your editor displays things in viewports, so that your editor can cast shadows
	// to other objects in SAMSON, for example thanks to the utility
	// functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

}

void SEDNATwisterEditor::displayInterface() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to display the editor 2D interface in viewports. 
	// Implement this function if your editor displays a 2D user interface. For example, a rectangle selection editor would display a 2D rectangle in the active viewport. 
	// You may use utility functions provided by SAMSON (e.g. displayLinesOrtho and displayTrianglesOrtho).

}

void SEDNATwisterEditor::mousePressEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (event->button() == Qt::MouseButton::LeftButton && !altPressed_) {
    untwistingSphereActive_ = true;
    twistingSphereActive_ = false;
    SAMSON::requestViewportUpdate();
  }
  else if (event->button() == Qt::MouseButton::LeftButton && altPressed_) {
    untwistingSphereActive_ = false;
    twistingSphereActive_ = true;
    SAMSON::requestViewportUpdate();
  }
}

void SEDNATwisterEditor::mouseReleaseEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (event->button() == Qt::MouseButton::LeftButton && !altPressed_) {
    untwistingSphereActive_ = false;
    SAMSON::requestViewportUpdate();
  }
  else if (event->button() == Qt::MouseButton::LeftButton && altPressed_) {
    twistingSphereActive_ = false;
    SAMSON::requestViewportUpdate();
  }
}

void SEDNATwisterEditor::mouseMoveEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.


  SBPosition3 nodePosition;
  SBNode* pickedNode = SAMSON::getNode(event->pos(), nodePosition);

  if (pickedNode == NULL) {
    spherePosition_ = SAMSON::getWorldPositionFromViewportPosition(event->pos());
  }
  else {
    spherePosition_ = SAMSON::getWorldPositionFromViewportPosition(event->pos(), nodePosition);
  }

  textPosition_ = spherePosition_;

  SBPosition3 offset = SBPosition3(
    SBQuantity::angstrom(0),
    SBQuantity::angstrom(sphereRadius_),
    SBQuantity::angstrom(0));

  textPosition_ -= offset;

  SAMSON::requestViewportUpdate();

  if (untwistingSphereActive_) {
    SBDocument* doc = SAMSON::getActiveDocument();
    SBNodeIndexer nodes;
    doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNBaseSegment")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

    DASBackToTheAtom btta;

    SB_FOR(SBNode* node, nodes) {

      ADNPointer<ADNBaseSegment> bs = static_cast<ADNBaseSegment*>(node);
      SBPosition3 pos = bs->GetPosition();
      SBPosition3 vectorFromSphereCenter = pos - spherePosition_;
     
      if (vectorFromSphereCenter.norm() < sphereRadius_) {
        vectorFromSphereCenter = vectorFromSphereCenter * (sphereRadius_ / vectorFromSphereCenter.norm());
        btta.UntwistNucleotidesPosition(bs);
        SAMSON::requestViewportUpdate();

      }
    }

  }
  else if (twistingSphereActive_) {
    SBDocument* doc = SAMSON::getActiveDocument();
    SBNodeIndexer nodes;
    doc->getNodes(nodes, (SBNode::GetClass() == std::string("ADNBaseSegment")) && (SBNode::GetElementUUID() == SBUUID("DDA2A078-1AB6-96BA-0D14-EE1717632D7A")));

    DASBackToTheAtom btta;

    SB_FOR(SBNode* node, nodes) {

      ADNPointer<ADNBaseSegment> bs = static_cast<ADNBaseSegment*>(node);
      SBPosition3 pos = bs->GetPosition();
      SBPosition3 vectorFromSphereCenter = pos - spherePosition_;

      if (vectorFromSphereCenter.norm() < sphereRadius_) {
        vectorFromSphereCenter = vectorFromSphereCenter * (sphereRadius_ / vectorFromSphereCenter.norm());
        btta.SetNucleotidePosition(bs, true);
        SAMSON::requestViewportUpdate();

      }
    }
  }
}

void SEDNATwisterEditor::mouseDoubleClickEvent(QMouseEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

}

void SEDNATwisterEditor::wheelEvent(QWheelEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  int angle = event->delta();
  sphereRadius_ = sphereRadius_ * pow(1.002, angle);
  SAMSON::requestViewportUpdate();

}

void SEDNATwisterEditor::keyPressEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.

  if (event->key() == Qt::Key::Key_Alt) {
    altPressed_ = true;
    text_ = "Twisting";
    SAMSON::requestViewportUpdate();
  }
}

void SEDNATwisterEditor::keyReleaseEvent(QKeyEvent* event) {

	// SAMSON Element generator pro tip: SAMSON redirects Qt events to the active editor. 
	// Implement this function to handle this event with your editor.
  if (event->key() == Qt::Key::Key_Alt) {
    altPressed_ = false;
    text_ = "Untwisting";
    SAMSON::requestViewportUpdate();
  }
}

void SEDNATwisterEditor::onBaseEvent(SBBaseEvent* baseEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle base events

}

void SEDNATwisterEditor::onDocumentEvent(SBDocumentEvent* documentEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle document events 

}

void SEDNATwisterEditor::onDynamicalEvent(SBDynamicalEvent* dynamicalEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle dynamical events 

}

void SEDNATwisterEditor::onStructuralEvent(SBStructuralEvent* documentEvent) {
	
	// SAMSON Element generator pro tip: implement this function if you need to handle structural events

}
