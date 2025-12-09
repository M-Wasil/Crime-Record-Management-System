#ifndef AUDITLOGWIDGET_H
#define AUDITLOGWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include "../adapter/UiCrimeAdapter.h"

class AuditLogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AuditLogWidget(UiCrimeAdapter* adapter, QWidget *parent = nullptr);
    void refresh();

private:
    void setupUI();

    UiCrimeAdapter* adapter_;
    QTextEdit* logTextEdit_;
};

#endif // AUDITLOGWIDGET_H

