#include "DashboardWidget.h"
#include <QHeaderView>
#include <QDateTime>
#include <QGroupBox>

DashboardWidget::DashboardWidget(UiCrimeAdapter* adapter, QWidget *parent)
    : QWidget(parent)
    , adapter_(adapter)
{
    setupUI();
    refresh();
}

void DashboardWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel* titleLabel = new QLabel(tr("<h2>Dashboard</h2>"), this);
    mainLayout->addWidget(titleLabel);
    
    // Statistics section
    QGroupBox* statsGroup = new QGroupBox(tr("Statistics"), this);
    QGridLayout* statsLayout = new QGridLayout(statsGroup);
    
    totalCrimesLabel_ = new QLabel(tr("Total Crimes: 0"), this);
    totalOfficersLabel_ = new QLabel(tr("Total Officers: 0"), this);
    recentCrimesLabel_ = new QLabel(tr("Recent Crimes: 0"), this);
    
    statsLayout->addWidget(totalCrimesLabel_, 0, 0);
    statsLayout->addWidget(totalOfficersLabel_, 0, 1);
    statsLayout->addWidget(recentCrimesLabel_, 0, 2);
    
    mainLayout->addWidget(statsGroup);
    
    // Recent crimes section
    QGroupBox* recentGroup = new QGroupBox(tr("Recent Crimes"), this);
    QVBoxLayout* recentLayout = new QVBoxLayout(recentGroup);
    
    recentCrimesTable_ = new QTableWidget(this);
    recentCrimesTable_->setColumnCount(5);
    recentCrimesTable_->setHorizontalHeaderLabels({
        tr("ID"), tr("Type"), tr("Severity"), tr("Date"), tr("Location")
    });
    recentCrimesTable_->horizontalHeader()->setStretchLastSection(true);
    recentCrimesTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    recentCrimesTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    recentLayout->addWidget(recentCrimesTable_);
    mainLayout->addWidget(recentGroup);
    
    mainLayout->addStretch();
}

void DashboardWidget::refresh()
{
    updateStatistics();
    updateRecentCrimes();
}

void DashboardWidget::updateStatistics()
{
    int totalCrimes = adapter_->getTotalCrimes();
    totalCrimesLabel_->setText(tr("Total Crimes: %1").arg(totalCrimes));
    
    // Note: Officers count not available in adapter, set to 0 for now
    totalOfficersLabel_->setText(tr("Total Officers: 0"));
    
    auto recent = adapter_->getRecentCrimes(10);
    recentCrimesLabel_->setText(tr("Recent Crimes: %1").arg(recent.size()));
}

void DashboardWidget::updateRecentCrimes()
{
    recentCrimesTable_->setRowCount(0);
    
    auto recentIds = adapter_->getRecentCrimes(10);
    for (const auto& id : recentIds) {
        auto crime = adapter_->findCrime(id);
        if (!crime) continue;
        
        int row = recentCrimesTable_->rowCount();
        recentCrimesTable_->insertRow(row);
        
        recentCrimesTable_->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(crime->id)));
        recentCrimesTable_->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(crime->type)));
        recentCrimesTable_->setItem(row, 2, new QTableWidgetItem(QString::number(crime->severity)));
        
        QDateTime dt = QDateTime::fromSecsSinceEpoch(crime->epoch);
        recentCrimesTable_->setItem(row, 3, new QTableWidgetItem(dt.toString("yyyy-MM-dd hh:mm:ss")));
        
        recentCrimesTable_->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(crime->location)));
    }
    
    recentCrimesTable_->resizeColumnsToContents();
}

