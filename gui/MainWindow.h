#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include "../adapter/UiCrimeAdapter.h"

// Forward declarations
class DashboardWidget;
class CrimeListWidget;
class CrimeFormWidget;
class CrimeDetailWidget;
class ImportExportDialog;
class AuditLogWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // File menu
    void onNewCrime();
    void onImport();
    void onExport();
    void onExit();
    
    // Edit menu
    void onEditCrime();
    void onDeleteCrime();
    
    // View menu
    void onShowDashboard();
    void onShowCrimeList();
    void onShowAuditLog();
    
    // Help menu
    void onAbout();
    void onAboutQt();
    
    // Navigation
    void onCrimeSelected(const QString& crimeId);
    void onCrimeDoubleClicked(const QString& crimeId);
    void onBackToList();

private:
    void setupMenus();
    void setupStatusBar();
    void setupCentralWidget();
    void connectSignals();
    
    // UI Components
    QStackedWidget* stackedWidget_;
    DashboardWidget* dashboardWidget_;
    CrimeListWidget* crimeListWidget_;
    CrimeFormWidget* crimeFormWidget_;
    CrimeDetailWidget* crimeDetailWidget_;
    AuditLogWidget* auditLogWidget_;
    
    // Adapter
    std::unique_ptr<UiCrimeAdapter> adapter_;
    
    // Menu actions
    QAction* actionNewCrime_;
    QAction* actionImport_;
    QAction* actionExport_;
    QAction* actionExit_;
    QAction* actionEditCrime_;
    QAction* actionDeleteCrime_;
    QAction* actionDashboard_;
    QAction* actionCrimeList_;
    QAction* actionAuditLog_;
    QAction* actionAbout_;
    QAction* actionAboutQt_;
};

#endif // MAINWINDOW_H

