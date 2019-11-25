#pragma once 

#include "SBGEditor.hpp"
#include "SETaggingEditorGUI.hpp"

#include "SBBaseEvent.hpp"
#include "SBDocumentEvent.hpp"
#include "SBDynamicalEvent.hpp"
#include "SBStructuralEvent.hpp"
#include "SBAction.hpp"
#include "DASCreator.hpp"
#include "SEAdenitaCoreSEApp.hpp"
#include "MSVDisplayHelper.hpp"

/// This class implements an editor

class SETaggingEditor : public SBGEditor {

	SB_CLASS
	Q_OBJECT

public :
  //enum TaggingShape {
  //  Sphere,
  //  Rod,
  //  HisTag
  //};
  enum TaggingMode {
    Tags = 0,
    Base = 1
  };

	/// \name Constructors and destructors
	//@{

	SETaggingEditor();																													///< Builds an editor					
	virtual ~SETaggingEditor();																											///< Destructs the editor

	//@}

	/// \name Identity
	//@{

	virtual SBCContainerUUID									getUUID() const;														///< Returns the widget UUID
	virtual QString												getName() const;														///< Returns the class name
	virtual QString												getText() const;														///< Returns the menu item text
	virtual QPixmap												getLogo() const;														///< Returns the pixmap logo
	virtual QKeySequence										getShortcut() const;													///< Returns the shorcut
	virtual QString												getToolTip() const;														///< Returns the tool tip
  virtual QString getDescription() const;
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

	SETaggingEditorGUI*											getPropertyWidget() const;												///< Returns the property widget of the editor
  ADNPointer<ADNNucleotide>               GetHighlightedNucleotide();
	//@}

  void changeMode(int mode);

private:
  SEAdenitaCoreSEApp*					          getAdenitaApp() const;															///< Returns a pointer to the app
  DNABlocks GetNtType(QPoint numSteps);

  //TaggingShape shape_ = TaggingShape::Sphere;
  TaggingMode mode_ = TaggingMode::Tags;
  DNABlocks ntType_ = DNABlocks::DI;

  DASCreatorEditors::UIData positions_;

  float opaqueness_ = 0.5f;
  float radius_ = 100.0f;

};


SB_REGISTER_TYPE(SETaggingEditor, "SETaggingEditor", "473D2F88-5D06-25F5-EB58-053661504C43");
SB_DECLARE_BASE_TYPE(SETaggingEditor, SBGEditor);
