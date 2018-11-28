#pragma once


#include "SBGWindowWidget.hpp" 
#include "ui_SETwistHelixEditorGUI.h"

class SETwistHelixEditor;

/// This class implements the GUI of the editor

// SAMSON Element generator pro tip: add GUI functionality in this class. 
// The non-GUI functionality (and the mouse and keyboard event handling methods that are specific to the editor) should go in the SEConnectSSDNAEditor class

class SETwistHelixEditorGUI : public SBGWindowWidget {

	Q_OBJECT

public:

	/// \name Constructors and destructors
	//@{

  SETwistHelixEditorGUI(SETwistHelixEditor* editor);																		///< Constructs a GUI for the editor
	virtual ~SETwistHelixEditorGUI();																										///< Destructs the GUI of the editor

	//@}

	/// \name Editor
	//@{

  SETwistHelixEditor*												getEditor() const;														///< Returns a pointer to the editor

	//@}

	/// \name Identity
	//@{

	virtual SBCContainerUUID									getUUID() const;														///< Returns the widget UUID
	virtual QString												getName() const;														///< Returns the widget name (used as a title for the embedding window)
	virtual QPixmap												getLogo() const;														///< Returns the widget logo
	int															getFormat() const;														///< Returns the widget format
	virtual QString												getCitation() const;													///< Returns the citation information

	//@}

	///\name Settings
	//@{

	void														loadSettings(SBGSettings* settings);										///< Load GUI settings
	void														saveSettings(SBGSettings* settings);										///< Save GUI settings

	//@}

  //! Sets the mode to plus or minus, for use with Adenita App menu
  void CheckPlusOrMinus(bool plus);

public slots:
  void onMinus(bool checked);
  void onPlus(bool checked);
  void onTwistAngle(bool checked);
  void onTwistTurns(bool checked);
  void onTwistAngleChanged(double angle);
  void onTwistTurnsChanged(int turns);

private:

	Ui::SETwistHelixEditorGUIClass									ui;
  SETwistHelixEditor*												editor;

};

