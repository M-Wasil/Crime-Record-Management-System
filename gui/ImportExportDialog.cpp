#include "ImportExportDialog.h"
#include <QFileDialog>
#include <QMessageBox>

ImportExportDialog::ImportExportDialog(UiCrimeAdapter* adapter, QWidget *parent)
    : QDialog(parent)
    , adapter_(adapter)
{
    setWindowTitle(tr("Import/Export Crimes"));
    setMinimumSize(500, 400);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QLabel* titleLabel = new QLabel(tr("<h3>Import/Export Crimes</h3>"), this);
    mainLayout->addWidget(titleLabel);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    QPushButton* importButton = new QPushButton(tr("&Import from JSON..."), this);
    connect(importButton, &QPushButton::clicked, this, &ImportExportDialog::onImport);
    
    QPushButton* exportButton = new QPushButton(tr("&Export to JSON..."), this);
    connect(exportButton, &QPushButton::clicked, this, &ImportExportDialog::onExport);
    
    buttonLayout->addWidget(importButton);
    buttonLayout->addWidget(exportButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    
    logTextEdit_ = new QTextEdit(this);
    logTextEdit_->setReadOnly(true);
    mainLayout->addWidget(logTextEdit_);
    
    QPushButton* closeButton = new QPushButton(tr("&Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    mainLayout->addWidget(closeButton);
}

void ImportExportDialog::onImport()
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
    
    QString log = tr("Import Results:\n");
    log += tr("Success: %1\n").arg(result.successCount);
    log += tr("Failures: %1\n").arg(result.failureCount);
    
    if (!result.errors.empty()) {
        log += tr("\nErrors:\n");
        for (const auto& error : result.errors) {
            log += QString::fromStdString(error) + "\n";
        }
    }
    
    logTextEdit_->setPlainText(log);
    
    if (result.failureCount == 0) {
        QMessageBox::information(this, tr("Import Success"), 
                                tr("Successfully imported %1 crimes.").arg(result.successCount));
    } else {
        QMessageBox::warning(this, tr("Import Warning"),
                            tr("Imported %1 crimes with %2 failures.").arg(result.successCount).arg(result.failureCount));
    }
}

void ImportExportDialog::onExport()
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
        logTextEdit_->setPlainText(tr("Export successful to: %1").arg(filepath));
        QMessageBox::information(this, tr("Export Success"), tr("Crimes exported successfully."));
    } else {
        logTextEdit_->setPlainText(tr("Export failed."));
        QMessageBox::warning(this, tr("Export Error"), tr("Failed to export crimes."));
    }
}

