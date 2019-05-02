#pragma once 

#include "SBGEditor.hpp"
#include "SEDNATwisterEditorGUI.hpp"
#include "SEAdenitaCoreSEApp.hpp"

#include "SBBaseEvent.hpp"
#include "SBDocumentEvent.hpp"
#include "SBDynamicalEvent.hpp"
#include "SBStructuralEvent.hpp"
#include "SBAction.hpp"
#include "ADNNanorobot.hpp"
#include "DASBackToTheAtom.hpp"
#include "ADNDisplayHelper.hpp"

/// This class implements an editor

enum BendingType {
  UNTWIST,
  SPHEREVISIBILITY
};

class SEDNATwisterEditor : public SBGEditor {

	SB_CLASS
	Q_OBJECT

public :

	/// \name Constructors and destructors
	//@{

	SEDNATwisterEditor();																													///< Builds an editor					
	virtual ~SEDNATwisterEditor();																											///< Destructs the editor

	//@}

	/// \name Identity
	//@{

	virtual SBCContainerUUID									getUUID() const;														///< Returns the widget UUID
	virtual QString												getName() const;														///< Returns the class name
	virtual QString												getText() const;														///< Returns the menu item text
	virtual QPixmap												getLogo() const;														///< Returns the pixmap logo
	virtual QKeySequence										getShortcut() const;													///< Returns the shorcut
	virtual QString												getToolTip() const;														///< Returns the tool tip
  virtual QString	getDescription() const;	///< Returns the menu item text

	//@}

	/// \name Editing
	//@{

	virtual void												beginEditing();															///< Called when editing is about to begin
	virtual void												endEditing();															///< Called when editing is about to end

	//@}

	/// \name Actions
	//@{

	virtual void												getActions(SBVector<SBAction*>& actionVector);							///< Gets the editor's actions

	//@}

	/// \name Rendering
	//@{

	virtual void												display();																///< Displays the editor
	virtual void												displayForShadow();														///< Displays the editor for shadow purposes
	virtual void												displayInterface();														///< Displays the 2D interface in the viewport

	//@}

	/// \name GUI Events
	//@{

	virtual void												mousePressEvent(QMouseEvent* event);									///< Handles mouse press event
	virtual void												mouseReleaseEvent(QMouseEvent* event);									///< Handles mouse release event
	virtual void												mouseMoveEvent(QMouseEvent* event);										///< Handles mouse move event
	virtual void												mouseDoubleClickEvent(QMouseEvent* event);								///< Handles mouse double click event

	virtual void												wheelEvent(QWheelEvent* event);											///< Handles wheel event

	virtual void												keyPressEvent(QKeyEvent* event);										///< Handles key press event
	virtual void												keyReleaseEvent(QKeyEvent* event);										///< Handles key release event

	//@}

	/// \name SAMSON Events
	//@{

	virtual void												onBaseEvent(SBBaseEvent* baseEvent);									///< Handles base events
	virtual void												onDynamicalEvent(SBDynamicalEvent* dynamicalEvent);						///< Handles dynamical events
	virtual void												onDocumentEvent(SBDocumentEvent* documentEvent);						///< Handles document events
	virtual void												onStructuralEvent(SBStructuralEvent* documentEvent);					///< Handles structural events

	//@}

	/// \name GUI
	//@{

	SEDNATwisterEditorGUI*											getPropertyWidget() const;												///< Returns the property widget of the editor
  void                                setBendingType(BendingType type);

private:
  SEAdenitaCoreSEApp*					        getAdenitaApp() const;															///< Returns a pointer to the app
  void                                untwisting();
  void                                makeInvisible();

	//@}
  bool                                  altPressed_;
  SBPosition3														spherePosition_;
  SBPosition3														textPosition_;
  string                                text_;
  SBQuantity::length										sphereRadius_;
  bool															    forwardActionSphereActive_;
  bool															    reverseActionSphereActive_;

  BendingType                           bendingType_ = BendingType::UNTWIST;
};


SB_REGISTER_TYPE(SEDNATwisterEditor, "SEDNATwisterEditor", "677B1667-7856-12E6-5901-E8EAC729501A");
SB_DECLARE_BASE_TYPE(SEDNATwisterEditor, SBGEditor);
