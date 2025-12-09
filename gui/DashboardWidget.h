#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTableWidget>
#include "../adapter/UiCrimeAdapter.h"

class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWidget(UiCrimeAdapter* adapter, QWidget *parent = nullptr);
    void refresh();

private:
    void setupUI();
    void updateStatistics();
    void updateRecentCrimes();

    UiCrimeAdapter* adapter_;
    
    // Statistics labels
    QLabel* totalCrimesLabel_;
    QLabel* totalOfficersLabel_;
    QLabel* recentCrimesLabel_;
    
    // Recent crimes table
    QTableWidget* recentCrimesTable_;
};

#endif // DASHBOARDWIDGET_H

