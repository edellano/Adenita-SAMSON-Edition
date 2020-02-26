#pragma once


#include "SBGApp.hpp"
#include "ui_SEAdenitaCoreSEAppGUI.h"
#include <QMessageBox>
#include <QComboBox>
#include <QSpinBox>
#include <QToolButton>


class SEAdenitaCoreSEApp;

/// This class implements the GUI of the app

// SAMSON Element generator pro tip: add GUI functionality in this class. The non-GUI functionality should go in the SEAdenitaCoreSEApp class

class SEAdenitaCoreSEAppGUI : public SBGApp {

	Q_OBJECT

public:

	/// \name Constructors and destructors
	//@{

	SEAdenitaCoreSEAppGUI(SEAdenitaCoreSEApp* t);																									///< Constructs a GUI for the app
	virtual ~SEAdenitaCoreSEAppGUI();																										///< Destructs the GUI of the app

	//@}

	/// \name App
	//@{

	SEAdenitaCoreSEApp*												getApp() const;															///< Returns a pointer to the app

	//@}

	/// \name Identity
	//@{

	virtual SBCContainerUUID									getUUID() const;														///< Returns the widget UUID
	virtual QString												getName() const;														///< Returns the widget name (used as a title for the embedding window)
	virtual QPixmap												getLogo() const;														///< Returns the widget logo
	int															getFormat() const;														///< Returns the widget format
	virtual QString												getCitation() const;													///< Returns the citation information

	//@}

  virtual void keyPressEvent(QKeyEvent *event);

	///\name Settings
	//@{

	void														loadSettings(SBGSettings* settings);										///< Load GUI settings
	void														saveSettings(SBGSettings* settings);										///< Save GUI settings

	//@}



  // get selected scaffold
  std::string GetScaffoldFilename();

public slots:

  void onChangeSelector(int idx);
  // Main
  void onLoadFile();
  void onSaveAll();
  void onSaveSelection();
  void onExport();
  void onSetScaffold();
  void onCreateBasePair();
  void onGenerateSequence();
  void onSettings();
  void onSetStart();
  void onCalculateBindingProperties();
  // Editors
  void onBreakEditor();
  void onConnectEditor();
  void onDeleteEditor();
  void onDNATwistEditor();
  void onMergePartsEditor();
  void onCreateStrandEditor();
  void onNanotubeCreatorEditor();
  void onLatticeCreatorEditor();
  void onWireframeEditor();
  void onTaggingEditor();
  void onTwisterEditor();
  // Debug
  void onAddNtThreeP();
  void onCenterPart();
  void onCatenanes();
  void onKinetoplast();
  void onTestNeighbors();
  void onOxDNAImport();
  void onFromDatagraph();
  void onHighlightXOs();
  void onHighlightPosXOs();
  void onExportToCanDo();
  void onFixDesigns();

private slots:
  void CheckForLoadedParts();

private:
  void SetupUI();
  std::string IsJsonCadnano(QString filename);
  void HighlightEditor(QToolButton* b);

  std::vector<QToolButton*> menuButtons_;
  std::vector<QToolButton*> editSequencesButtons_;
  std::vector<QToolButton*> modelingButtons_;
  std::vector<QToolButton*> creatorsButtons_;
  std::vector<QPushButton*> debugButtons_;

  std::vector<QToolButton*> GetMenuButtons();
  std::vector<QToolButton*> GetEditSequencesButtons();
  std::vector<QToolButton*> GetModelingButtons();
  std::vector<QToolButton*> GetCreatorsButtons();
  std::vector<QPushButton*> GetDebugButtons();

	Ui::SEAdenitaCoreSEAppGUIClass									ui;

  QToolButton* highlightedEditor_ = nullptr;
};

