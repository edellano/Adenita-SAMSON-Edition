#pragma once 

#include "SBGEditor.hpp"
#include "SEConnectSSDNAEditorGUI.hpp"

#include "SBBaseEvent.hpp"
#include "SBDocumentEvent.hpp"
#include "SBDynamicalEvent.hpp"
#include "SBStructuralEvent.hpp"
#include "SBAction.hpp"
#include "ADNDisplayHelper.hpp"
#include "SEAdenitaCoreSEApp.hpp"

/// This class implements an editor

enum ConnectionMode {
  Crossover,
  Concatenate
};

class SEConnectSSDNAEditor : public SBGEditor {

	SB_CLASS
	Q_OBJECT

public :

	/// \name Constructors and destructors
	//@{

	SEConnectSSDNAEditor();																													///< Builds an editor					
	virtual ~SEConnectSSDNAEditor();																											///< Destructs the editor

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

	SEConnectSSDNAEditorGUI*											getPropertyWidget() const;												///< Returns the property widget of the editor

	//@}

  void SetMode(bool xo);
  void SetSequence(std::string seq);

private:
  SEAdenitaCoreSEApp*					          getAdenitaApp() const;															///< Returns a pointer to the app

  bool display_ = false;
  ADNPointer<ADNNucleotide> start_;

  ConnectionMode mode_ = Crossover;
  std::string sequence_ = "";
};


SB_REGISTER_TYPE(SEConnectSSDNAEditor, "SEConnectSSDNAEditor", "48FDCE78-A55E-FDA2-237E-319202E56080");
SB_DECLARE_BASE_TYPE(SEConnectSSDNAEditor, SBGEditor);
