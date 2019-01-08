#pragma once


#include "SBGApp.hpp"
#include "ui_SEAdenitaCoreSEAppGUI.h"
#include <QMessageBox>
#include <QComboBox>
#include <QSpinBox>


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

  void onLoadFile();
  void onSaveFile();
  void onExport();
  void onSetScaffold();
  // Options
  void onChangeScaffold(int idx);
  // Debug
  void onCenterPart();
  void onDSRing();
  void onCatenanes();
  void onKinetoplast();
  void onCalculateBindingProperties();
  void onSetStart();
  void onMergeComponents();
  void onTestNeighbors();
  void onOxDNAImport();
  void onFromDatagraph();

private slots:
  void CheckForLoadedParts();

private:
  std::string IsJsonCadnano(QString filename);
  

	Ui::SEAdenitaCoreSEAppGUIClass									ui;
};

