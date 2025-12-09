#ifndef CRIMELISTWIDGET_H
#define CRIMELISTWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QAbstractTableModel>
#include "../adapter/UiCrimeAdapter.h"

class CrimeTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CrimeTableModel(UiCrimeAdapter* adapter, QObject* parent = nullptr);
    
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    void refresh();
    QString getCrimeId(int row) const;

private:
    UiCrimeAdapter* adapter_;
    std::vector<CrimeDto> crimes_;
};

class CrimeListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CrimeListWidget(UiCrimeAdapter* adapter, QWidget *parent = nullptr);
    void refresh();
    QString getSelectedCrimeId() const;

signals:
    void crimeSelected(const QString& crimeId);
    void crimeDoubleClicked(const QString& crimeId);

private slots:
    void onSearchTextChanged(const QString& text);
    void onSeverityFilterChanged(int index);
    void onTableDoubleClicked(const QModelIndex& index);
    void onTableSelectionChanged();

private:
    void setupUI();
    void applyFilters();

    UiCrimeAdapter* adapter_;
    CrimeTableModel* model_;
    QTableView* tableView_;
    QLineEdit* searchLineEdit_;
    QComboBox* severityComboBox_;
    QLabel* statusLabel_;
};

#endif // CRIMELISTWIDGET_H

