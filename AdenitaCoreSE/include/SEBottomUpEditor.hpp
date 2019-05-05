#pragma once 

#include "SBGEditor.hpp"
#include "SEBottomUpEditorGUI.hpp"

#include "SBBaseEvent.hpp"
#include "SBDocumentEvent.hpp"
#include "SBDynamicalEvent.hpp"
#include "SBStructuralEvent.hpp"
#include "SBAction.hpp"

#include "SEAdenitaCoreSEApp.hpp"

/// This class implements an editor

class SEBottomUpEditor : public SBGEditor {

	SB_CLASS
	Q_OBJECT

public :

	/// \name Constructors and destructors
	//@{

  SEBottomUpEditor();																													///< Builds an editor					
	virtual ~SEBottomUpEditor();																											///< Destructs the editor

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

  SEBottomUpEditorGUI*								getPropertyWidget() const;												///< Returns the property widget of the editor

	//@}

  std::map<int, SBPointer<SBNode>> getPartsList();
  void SetSelected(int idx);
  void SetMaxCutOff(SBQuantity::length val);
  void SetMinCutOff(SBQuantity::length val);
  void SetAngleCutOff(double val);
  void SetPreview(bool val);
  void ResetPart();

  //! Builds a model for a indexed component
  ADNPointer<ADNPart> Build(double minCutOff, double maxCutOff, double maxAngle);

  void sendPartToAdenita(ADNPointer<ADNPart> part);

private:
  bool preview_ = false;
  int selected_ = 0;
  std::map<int, SBPointer<SBNode>> indexParts_;

  ADNPointer<ADNPart> part_ = nullptr;
  SBQuantity::length maxCutOff_;
  SBQuantity::length minCutOff_;
  double maxAngle_;
  bool changed_ = false;
};


SB_REGISTER_TYPE(SEBottomUpEditor, "SEBottomUpEditor", "1D465936-DCE4-E228-B110-8FA4FFF5F291");
SB_DECLARE_BASE_TYPE(SEBottomUpEditor, SBGEditor);
