#pragma once


#include "SBGWindowWidget.hpp" 
#include "ui_SEBottomUpEditorGUI.h"

class SEBottomUpEditor;

/// This class implements the GUI of the editor

// SAMSON Element generator pro tip: add GUI functionality in this class. 
// The non-GUI functionality (and the mouse and keyboard event handling methods that are specific to the editor) should go in the SEWireframeEditor class

class SEBottomUpEditorGUI : public SBGWindowWidget {

	Q_OBJECT

public:

	/// \name Constructors and destructors
	//@{

  SEBottomUpEditorGUI(SEBottomUpEditor* editor);																		///< Constructs a GUI for the editor
	virtual ~SEBottomUpEditorGUI();																										///< Destructs the GUI of the editor

	//@}

	/// \name Editor
	//@{

  SEBottomUpEditor*												getEditor() const;														///< Returns a pointer to the editor

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
  public slots:

    void onGenerateModel();
    void onRefreshPartList();
    void onSelectNode(int s);
    void onMinSliderChanged(int val);
    void onMaxSliderChanged(int val);
    void onAngleSliderChanged(int val);
    void onPreview(bool val);


private:

	Ui::SEBottomUpEditorGUIClass									ui;
  SEBottomUpEditor*												editor;

};

