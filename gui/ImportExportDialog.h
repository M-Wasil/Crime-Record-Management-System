#ifndef IMPORTEXPORTDIALOG_H
#define IMPORTEXPORTDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include "../adapter/UiCrimeAdapter.h"

class ImportExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportExportDialog(UiCrimeAdapter* adapter, QWidget *parent = nullptr);

private slots:
    void onImport();
    void onExport();

private:
    UiCrimeAdapter* adapter_;
    QTextEdit* logTextEdit_;
};

#endif // IMPORTEXPORTDIALOG_H

