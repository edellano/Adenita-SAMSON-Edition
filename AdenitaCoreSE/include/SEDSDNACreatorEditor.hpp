#pragma once 

#include "SBGEditor.hpp"
#include "SEDSDNACreatorEditorGUI.hpp"

#include "SBBaseEvent.hpp"
#include "SBDocumentEvent.hpp"
#include "SBDynamicalEvent.hpp"
#include "SBStructuralEvent.hpp"
#include "SBAction.hpp"

#include "SEAdenitaCoreSEApp.hpp"
#include "ADNPart.hpp"
#include "ADNDisplayHelper.hpp"
#include "DASCreator.hpp"

/// This class implements an editor

class SEDSDNACreatorEditor : public SBGEditor {

	SB_CLASS
	Q_OBJECT

public :

	/// \name Constructors and destructors
	//@{

	SEDSDNACreatorEditor();																													///< Builds an editor					
	virtual ~SEDSDNACreatorEditor();																											///< Destructs the editor

	//@}

	/// \name Identity
	//@{

	virtual SBCContainerUUID									getUUID() const;														///< Returns the widget UUID
	virtual QString												getName() const;														///< Returns the class name
	virtual QString												getText() const;														///< Returns the menu item text
	virtual QPixmap												getLogo() const;														///< Returns the pixmap logo
	virtual QKeySequence										getShortcut() const;													///< Returns the shorcut
	virtual QString												getToolTip() const;														///< Returns the tool tip

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

	SEDSDNACreatorEditorGUI*											getPropertyWidget() const;												///< Returns the property widget of the editor

	//@}

  void SetMode(bool m);

private:
  ADNPointer<ADNPart> generateStrand(bool mock = false);
  void displayStrand();
  void resetPositions();
  void sendPartToAdenita(ADNPointer<ADNPart> nanotube);

  bool dsMode_ = true;  // true for dsDNA, false for ssDNA

  struct Positions {
    SBPosition3 First;
    SBPosition3 Second;
    int cnt = 0;
  };

  Positions positions_;
  bool display_ = false;
  ADNPointer<ADNPart> tempPart_ = nullptr;
};


SB_REGISTER_TYPE(SEDSDNACreatorEditor, "SEDSDNACreatorEditor", "86204A08-DFD6-97A8-2BE2-4CFC8B4169A3");
SB_DECLARE_BASE_TYPE(SEDSDNACreatorEditor, SBGEditor);
