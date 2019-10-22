#include "SEAdenitaCoreSettingsGUI.hpp"
#include <QFileDialog>
#include "SEConfig.hpp"

SEAdenitaCoreSettingsGUI::SEAdenitaCoreSettingsGUI(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

	ui.setupUi( this );

  SEConfig& c = SEConfig::GetInstance();

  ui.chkXODisplay->setChecked(c.display_possible_crossovers);
  ui.chkOverlay->setChecked(c.show_overlay);
  ui.chkInterpolateDim->setChecked(c.interpolate_dimensions);
  ui.chkAutoScaffold->setChecked(c.auto_set_scaffold_sequence);
  ui.lineNtthal->setText(QString::fromStdString(c.ntthal));
  ui.lineCustomScaffold->setText(QString::fromStdString(c.scaffCustomFilename));
  ui.cmbScaffold->setCurrentIndex(c.scaffType);

  QObject::connect(ui.cmbScaffold, SIGNAL(currentIndexChanged(int)), this, SLOT(onChangeScaffold(int)));
  QObject::connect(ui.dialogButtons, SIGNAL(accepted()), this, SLOT(accept()));
  QObject::connect(ui.dialogButtons, SIGNAL(rejected()), this, SLOT(reject()));
}

SEAdenitaCoreSettingsGUI::~SEAdenitaCoreSettingsGUI() {

}

void SEAdenitaCoreSettingsGUI::onChangeScaffold(int idx)
{
  if (idx == 2) {
    // custom scaffold
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose scaffold"), QDir::currentPath(), tr("(Sequences *.fasta)"));
    ui.lineCustomScaffold->setText(filename);
  }
}

void SEAdenitaCoreSettingsGUI::onSetPathNtthal()
{
  QString filename = QFileDialog::getOpenFileName(this, tr("Set path to ntthal executable"), QDir::currentPath(), tr("(ntthal.exe)"));
  ui.lineNtthal->setText(filename);
}

void SEAdenitaCoreSettingsGUI::accept()
{
  QDialog::accept();

  SEConfig& c = SEConfig::GetInstance();

  c.setScaffCustomFilename(ui.lineCustomScaffold->text().toStdString());
  c.setDisplayPossibleCrossovers(ui.chkXODisplay->isChecked());
  c.setShowOverlay(ui.chkOverlay->isChecked());
  c.setInterpolateDimensions(ui.chkInterpolateDim->isChecked());
  c.setAutoSetScaffoldSequence(ui.chkAutoScaffold->isChecked());
  c.setScaffType(ui.cmbScaffold->currentIndex());
  c.setNtthalExe(ui.lineNtthal->text().toStdString());
}
