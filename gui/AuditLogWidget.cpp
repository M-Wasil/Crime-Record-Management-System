#include "AuditLogWidget.h"
#include <QDateTime>

AuditLogWidget::AuditLogWidget(UiCrimeAdapter* adapter, QWidget *parent)
    : QWidget(parent)
    , adapter_(adapter)
{
    setupUI();
    refresh();
}

void AuditLogWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QLabel* titleLabel = new QLabel(tr("<h2>Audit Log</h2>"), this);
    mainLayout->addWidget(titleLabel);
    
    logTextEdit_ = new QTextEdit(this);
    logTextEdit_->setReadOnly(true);
    mainLayout->addWidget(logTextEdit_);
}

void AuditLogWidget::refresh()
{
    // For now, show all crimes as audit log entries
    // In a production app, you'd track actual audit events
    QString log;
    log += tr("=== Audit Log ===\n");
    log += tr("Generated: %1\n\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    
    auto crimes = adapter_->getAllCrimes();
    log += tr("Total Crimes: %1\n\n").arg(crimes.size());
    
    for (const auto& crime : crimes) {
        QDateTime dt = QDateTime::fromSecsSinceEpoch(crime.epoch);
        log += tr("[%1] Crime %2: %3 (Severity: %4)\n")
               .arg(dt.toString("yyyy-MM-dd hh:mm:ss"))
               .arg(QString::fromStdString(crime.id))
               .arg(QString::fromStdString(crime.type))
               .arg(crime.severity);
    }
    
    logTextEdit_->setPlainText(log);
}

