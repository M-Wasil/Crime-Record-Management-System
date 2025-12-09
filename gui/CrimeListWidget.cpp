#include "CrimeListWidget.h"
#include <QHeaderView>
#include <QDateTime>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>

// CrimeTableModel implementation
CrimeTableModel::CrimeTableModel(UiCrimeAdapter* adapter, QObject* parent)
    : QAbstractTableModel(parent)
    , adapter_(adapter)
{
    refresh();
}

int CrimeTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(crimes_.size());
}

int CrimeTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 7; // ID, Type, Severity, Date, Location, Suspect, Description
}

QVariant CrimeTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(crimes_.size())) {
        return QVariant();
    }
    
    const CrimeDto& crime = crimes_[index.row()];
    
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return QString::fromStdString(crime.id);
        case 1: return QString::fromStdString(crime.type);
        case 2: return crime.severity;
        case 3: {
            QDateTime dt = QDateTime::fromSecsSinceEpoch(crime.epoch);
            return dt.toString("yyyy-MM-dd hh:mm:ss");
        }
        case 4: return QString::fromStdString(crime.location);
        case 5: return QString::fromStdString(crime.suspectName);
        case 6: return QString::fromStdString(crime.description);
        default: return QVariant();
        }
    }
    
    return QVariant();
}

QVariant CrimeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return tr("ID");
        case 1: return tr("Type");
        case 2: return tr("Severity");
        case 3: return tr("Date");
        case 4: return tr("Location");
        case 5: return tr("Suspect");
        case 6: return tr("Description");
        default: return QVariant();
        }
    }
    return QVariant();
}

void CrimeTableModel::refresh()
{
    beginResetModel();
    crimes_ = adapter_->getAllCrimes();
    endResetModel();
}

QString CrimeTableModel::getCrimeId(int row) const
{
    if (row >= 0 && row < static_cast<int>(crimes_.size())) {
        return QString::fromStdString(crimes_[row].id);
    }
    return QString();
}

// CrimeListWidget implementation
CrimeListWidget::CrimeListWidget(UiCrimeAdapter* adapter, QWidget *parent)
    : QWidget(parent)
    , adapter_(adapter)
    , model_(new CrimeTableModel(adapter, this))
{
    setupUI();
    refresh();
}

void CrimeListWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel* titleLabel = new QLabel(tr("<h2>Crime List</h2>"), this);
    mainLayout->addWidget(titleLabel);
    
    // Filter bar
    QHBoxLayout* filterLayout = new QHBoxLayout();
    
    QLabel* searchLabel = new QLabel(tr("Search:"), this);
    searchLineEdit_ = new QLineEdit(this);
    searchLineEdit_->setPlaceholderText(tr("Search by ID, type, location..."));
    connect(searchLineEdit_, &QLineEdit::textChanged, this, &CrimeListWidget::onSearchTextChanged);
    
    QLabel* severityLabel = new QLabel(tr("Severity:"), this);
    severityComboBox_ = new QComboBox(this);
    severityComboBox_->addItem(tr("All"), -1);
    severityComboBox_->addItem(tr("1"), 1);
    severityComboBox_->addItem(tr("2"), 2);
    severityComboBox_->addItem(tr("3"), 3);
    severityComboBox_->addItem(tr("4"), 4);
    severityComboBox_->addItem(tr("5"), 5);
    connect(severityComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CrimeListWidget::onSeverityFilterChanged);
    
    filterLayout->addWidget(searchLabel);
    filterLayout->addWidget(searchLineEdit_);
    filterLayout->addWidget(severityLabel);
    filterLayout->addWidget(severityComboBox_);
    filterLayout->addStretch();
    
    mainLayout->addLayout(filterLayout);
    
    // Table view
    tableView_ = new QTableView(this);
    tableView_->setModel(model_);
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView_->horizontalHeader()->setStretchLastSection(true);
    tableView_->setSortingEnabled(true);
    
    connect(tableView_, &QTableView::doubleClicked, this, &CrimeListWidget::onTableDoubleClicked);
    connect(tableView_->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &CrimeListWidget::onTableSelectionChanged);
    
    mainLayout->addWidget(tableView_);
    
    // Status label
    statusLabel_ = new QLabel(this);
    mainLayout->addWidget(statusLabel_);
}

void CrimeListWidget::refresh()
{
    model_->refresh();
    applyFilters();
    statusLabel_->setText(tr("Total: %1 crimes").arg(model_->rowCount()));
}

QString CrimeListWidget::getSelectedCrimeId() const
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return QString();
    }
    return model_->getCrimeId(selected.first().row());
}

void CrimeListWidget::onSearchTextChanged(const QString& text)
{
    applyFilters();
}

void CrimeListWidget::onSeverityFilterChanged(int index)
{
    Q_UNUSED(index)
    applyFilters();
}

void CrimeListWidget::onTableDoubleClicked(const QModelIndex& index)
{
    QString crimeId = model_->getCrimeId(index.row());
    if (!crimeId.isEmpty()) {
        emit crimeDoubleClicked(crimeId);
    }
}

void CrimeListWidget::onTableSelectionChanged()
{
    QString crimeId = getSelectedCrimeId();
    if (!crimeId.isEmpty()) {
        emit crimeSelected(crimeId);
    }
}

void CrimeListWidget::applyFilters()
{
    // For now, we'll do simple filtering
    // In a production app, you might use QSortFilterProxyModel
    // For simplicity, we refresh and let the model handle it
    model_->refresh();
    
    QString searchText = searchLineEdit_->text().toLower();
    int severityFilter = severityComboBox_->currentData().toInt();
    
    // Filter logic would go here if using proxy model
    // For now, just refresh
    Q_UNUSED(searchText)
    Q_UNUSED(severityFilter)
}

