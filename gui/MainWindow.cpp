#include "MainWindow.h"
#include "DashboardWidget.h"
#include "CrimeListWidget.h"
#include "CrimeFormWidget.h"
#include "CrimeDetailWidget.h"
#include "AuditLogWidget.h"
#include "ImportExportDialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>
#include <QMenuBar>
#include <QKeySequence>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , stackedWidget_(new QStackedWidget(this))
    , adapter_(std::make_unique<UiCrimeAdapter>())
{
    setWindowTitle(tr("Crime Record Management System"));
    setMinimumSize(1000, 700);
    resize(1200, 800);
    
    setupMenus();
    setupStatusBar();
    setupCentralWidget();
    connectSignals();
    
    // Show dashboard by default
    onShowDashboard();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupMenus()
{
    // File menu
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    actionNewCrime_ = fileMenu->addAction(tr("&New Crime..."), this, &MainWindow::onNewCrime);
    actionNewCrime_->setShortcut(QKeySequence::New);
    fileMenu->addSeparator();
    actionImport_ = fileMenu->addAction(tr("&Import..."), this, &MainWindow::onImport);
    actionImport_->setShortcut(QKeySequence::Open);
    actionExport_ = fileMenu->addAction(tr("&Export..."), this, &MainWindow::onExport);
    actionExport_->setShortcut(QKeySequence::SaveAs);
    fileMenu->addSeparator();
    actionExit_ = fileMenu->addAction(tr("E&xit"), this, &MainWindow::onExit);
    actionExit_->setShortcut(QKeySequence::Quit);
    
    // Edit menu
    QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
    actionEditCrime_ = editMenu->addAction(tr("&Edit Crime..."), this, &MainWindow::onEditCrime);
    actionEditCrime_->setShortcut(QKeySequence("Ctrl+E"));
    actionDeleteCrime_ = editMenu->addAction(tr("&Delete Crime..."), this, &MainWindow::onDeleteCrime);
    actionDeleteCrime_->setShortcut(QKeySequence::Delete);
    
    // View menu
    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    actionDashboard_ = viewMenu->addAction(tr("&Dashboard"), this, &MainWindow::onShowDashboard);
    actionDashboard_->setShortcut(QKeySequence("Ctrl+D"));
    actionCrimeList_ = viewMenu->addAction(tr("&Crime List"), this, &MainWindow::onShowCrimeList);
    actionCrimeList_->setShortcut(QKeySequence("Ctrl+L"));
    viewMenu->addSeparator();
    actionAuditLog_ = viewMenu->addAction(tr("&Audit Log"), this, &MainWindow::onShowAuditLog);
    
    // Help menu
    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    actionAbout_ = helpMenu->addAction(tr("&About"), this, &MainWindow::onAbout);
    actionAboutQt_ = helpMenu->addAction(tr("About &Qt"), this, &MainWindow::onAboutQt);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::setupCentralWidget()
{
    // Create widgets
    dashboardWidget_ = new DashboardWidget(adapter_.get(), this);
    crimeListWidget_ = new CrimeListWidget(adapter_.get(), this);
    crimeFormWidget_ = new CrimeFormWidget(adapter_.get(), this);
    crimeDetailWidget_ = new CrimeDetailWidget(adapter_.get(), this);
    auditLogWidget_ = new AuditLogWidget(adapter_.get(), this);
    
    // Add to stacked widget
    stackedWidget_->addWidget(dashboardWidget_);
    stackedWidget_->addWidget(crimeListWidget_);
    stackedWidget_->addWidget(crimeFormWidget_);
    stackedWidget_->addWidget(crimeDetailWidget_);
    stackedWidget_->addWidget(auditLogWidget_);
    
    setCentralWidget(stackedWidget_);
}

void MainWindow::connectSignals()
{
    // Connect crime list signals
    connect(crimeListWidget_, &CrimeListWidget::crimeSelected,
            this, &MainWindow::onCrimeSelected);
    connect(crimeListWidget_, &CrimeListWidget::crimeDoubleClicked,
            this, &MainWindow::onCrimeDoubleClicked);
    
    // Connect form signals
    connect(crimeFormWidget_, &CrimeFormWidget::crimeSaved,
            this, &MainWindow::onBackToList);
    connect(crimeFormWidget_, &CrimeFormWidget::cancelled,
            this, &MainWindow::onBackToList);
    
    // Connect detail signals
    connect(crimeDetailWidget_, &CrimeDetailWidget::editRequested,
            this, &MainWindow::onEditCrime);
    connect(crimeDetailWidget_, &CrimeDetailWidget::backToList,
            this, &MainWindow::onBackToList);
}

void MainWindow::onNewCrime()
{
    crimeFormWidget_->setMode(CrimeFormWidget::Mode::Add);
    crimeFormWidget_->clearForm();
    stackedWidget_->setCurrentWidget(crimeFormWidget_);
    statusBar()->showMessage(tr("Adding new crime..."));
}

void MainWindow::onImport()
{
    QString filepath = QFileDialog::getOpenFileName(
        this,
        tr("Import Crimes"),
        "",
        tr("JSON Files (*.json);;All Files (*)")
    );
    
    if (filepath.isEmpty()) {
        return;
    }
    
    auto result = adapter_->importFromJson(filepath.toStdString());
    
    QString message = tr("Import completed: %1 succeeded, %2 failed")
                     .arg(result.successCount)
                     .arg(result.failureCount);
    
    if (result.failureCount > 0 && !result.errors.empty()) {
        message += "\n\nErrors:\n" + QString::fromStdString(result.errors[0]);
    }
    
    QMessageBox::information(this, tr("Import Result"), message);
    
    // Refresh views
    dashboardWidget_->refresh();
    crimeListWidget_->refresh();
}

void MainWindow::onExport()
{
    QString filepath = QFileDialog::getSaveFileName(
        this,
        tr("Export Crimes"),
        "",
        tr("JSON Files (*.json);;All Files (*)")
    );
    
    if (filepath.isEmpty()) {
        return;
    }
    
    if (adapter_->exportToJson(filepath.toStdString())) {
        QMessageBox::information(this, tr("Export"), tr("Crimes exported successfully."));
    } else {
        QMessageBox::warning(this, tr("Export Error"), tr("Failed to export crimes."));
    }
}

void MainWindow::onExit()
{
    close();
}

void MainWindow::onEditCrime()
{
    QString crimeId = crimeListWidget_->getSelectedCrimeId();
    if (crimeId.isEmpty()) {
        QMessageBox::information(this, tr("Edit Crime"), tr("Please select a crime to edit."));
        return;
    }
    
    auto crime = adapter_->findCrime(crimeId.toStdString());
    if (!crime) {
        QMessageBox::warning(this, tr("Edit Crime"), tr("Crime not found."));
        return;
    }
    
    crimeFormWidget_->setMode(CrimeFormWidget::Mode::Edit);
    crimeFormWidget_->loadCrime(*crime);
    stackedWidget_->setCurrentWidget(crimeFormWidget_);
    statusBar()->showMessage(tr("Editing crime: %1").arg(crimeId));
}

void MainWindow::onDeleteCrime()
{
    QString crimeId = crimeListWidget_->getSelectedCrimeId();
    if (crimeId.isEmpty()) {
        QMessageBox::information(this, tr("Delete Crime"), tr("Please select a crime to delete."));
        return;
    }
    
    auto crime = adapter_->findCrime(crimeId.toStdString());
    if (!crime) {
        QMessageBox::warning(this, tr("Delete Crime"), tr("Crime not found."));
        return;
    }
    
    int ret = QMessageBox::warning(
        this,
        tr("Delete Crime"),
        tr("Are you sure you want to delete crime %1?\n\nThis action cannot be undone.")
            .arg(crimeId),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    
    if (ret == QMessageBox::Yes) {
        if (adapter_->deleteCrime(crimeId.toStdString())) {
            QMessageBox::information(this, tr("Delete Crime"), tr("Crime deleted successfully."));
            crimeListWidget_->refresh();
            dashboardWidget_->refresh();
            statusBar()->showMessage(tr("Crime deleted: %1").arg(crimeId));
        } else {
            QMessageBox::warning(this, tr("Delete Crime"), tr("Failed to delete crime."));
        }
    }
}

void MainWindow::onShowDashboard()
{
    stackedWidget_->setCurrentWidget(dashboardWidget_);
    dashboardWidget_->refresh();
    statusBar()->showMessage(tr("Dashboard"));
}

void MainWindow::onShowCrimeList()
{
    stackedWidget_->setCurrentWidget(crimeListWidget_);
    crimeListWidget_->refresh();
    statusBar()->showMessage(tr("Crime List"));
}

void MainWindow::onShowAuditLog()
{
    stackedWidget_->setCurrentWidget(auditLogWidget_);
    auditLogWidget_->refresh();
    statusBar()->showMessage(tr("Audit Log"));
}

void MainWindow::onAbout()
{
    QMessageBox::about(
        this,
        tr("About CRMS"),
        tr("<h2>Crime Record Management System</h2>"
           "<p>Version 1.0.0</p>"
           "<p>A desktop application for managing criminal records.</p>")
    );
}

void MainWindow::onAboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::onCrimeSelected(const QString& crimeId)
{
    statusBar()->showMessage(tr("Selected: %1").arg(crimeId));
}

void MainWindow::onCrimeDoubleClicked(const QString& crimeId)
{
    auto crime = adapter_->findCrime(crimeId.toStdString());
    if (!crime) {
        return;
    }
    
    crimeDetailWidget_->loadCrime(*crime);
    stackedWidget_->setCurrentWidget(crimeDetailWidget_);
    statusBar()->showMessage(tr("Viewing crime: %1").arg(crimeId));
}

void MainWindow::onBackToList()
{
    stackedWidget_->setCurrentWidget(crimeListWidget_);
    crimeListWidget_->refresh();
    dashboardWidget_->refresh();
    statusBar()->showMessage(tr("Crime List"));
}

