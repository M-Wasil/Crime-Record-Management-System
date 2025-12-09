#include "CrimeFormWidget.h"
#include <QMessageBox>
#include <QDateTime>
#include <QGroupBox>

CrimeFormWidget::CrimeFormWidget(UiCrimeAdapter* adapter, QWidget *parent)
    : QWidget(parent)
    , adapter_(adapter)
    , mode_(Mode::Add)
{
    setupUI();
    clearForm();
}

void CrimeFormWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel* titleLabel = new QLabel(tr("<h2>Crime Form</h2>"), this);
    mainLayout->addWidget(titleLabel);
    
    // Form layout
    QFormLayout* formLayout = new QFormLayout();
    
    idLineEdit_ = new QLineEdit(this);
    idLineEdit_->setPlaceholderText(tr("e.g., C1001"));
    formLayout->addRow(tr("Crime ID:"), idLineEdit_);
    
    typeComboBox_ = new QComboBox(this);
    typeComboBox_->setEditable(true);
    typeComboBox_->addItems({
        tr("Theft"), tr("Assault"), tr("Burglary"), tr("Vandalism"),
        tr("Fraud"), tr("Drug Offense"), tr("Traffic Violation"), tr("Other")
    });
    formLayout->addRow(tr("Type:"), typeComboBox_);
    
    severityComboBox_ = new QComboBox(this);
    severityComboBox_->addItems({tr("1"), tr("2"), tr("3"), tr("4"), tr("5")});
    severityComboBox_->setCurrentIndex(2); // Default to 3
    formLayout->addRow(tr("Severity (1-5):"), severityComboBox_);
    
    dateTimeEdit_ = new QDateTimeEdit(this);
    dateTimeEdit_->setDateTime(QDateTime::currentDateTime());
    dateTimeEdit_->setCalendarPopup(true);
    formLayout->addRow(tr("Date & Time:"), dateTimeEdit_);
    
    locationLineEdit_ = new QLineEdit(this);
    locationLineEdit_->setPlaceholderText(tr("Enter location"));
    formLayout->addRow(tr("Location:"), locationLineEdit_);
    
    suspectLineEdit_ = new QLineEdit(this);
    suspectLineEdit_->setPlaceholderText(tr("Enter suspect name or 'Unknown'"));
    formLayout->addRow(tr("Suspect Name:"), suspectLineEdit_);
    
    descriptionTextEdit_ = new QTextEdit(this);
    descriptionTextEdit_->setPlaceholderText(tr("Enter crime description"));
    descriptionTextEdit_->setMaximumHeight(100);
    formLayout->addRow(tr("Description:"), descriptionTextEdit_);
    
    notesTextEdit_ = new QTextEdit(this);
    notesTextEdit_->setPlaceholderText(tr("Additional notes (optional)"));
    notesTextEdit_->setMaximumHeight(100);
    formLayout->addRow(tr("Notes:"), notesTextEdit_);
    
    mainLayout->addLayout(formLayout);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    saveButton_ = new QPushButton(tr("&Save"), this);
    saveButton_->setDefault(true);
    connect(saveButton_, &QPushButton::clicked, this, &CrimeFormWidget::onSave);
    
    cancelButton_ = new QPushButton(tr("&Cancel"), this);
    connect(cancelButton_, &QPushButton::clicked, this, &CrimeFormWidget::onCancel);
    
    buttonLayout->addWidget(saveButton_);
    buttonLayout->addWidget(cancelButton_);
    mainLayout->addLayout(buttonLayout);
    
    mainLayout->addStretch();
}

void CrimeFormWidget::setMode(Mode mode)
{
    mode_ = mode;
    idLineEdit_->setEnabled(mode == Mode::Add);
    saveButton_->setText(mode == Mode::Add ? tr("&Add") : tr("&Update"));
}

void CrimeFormWidget::loadCrime(const CrimeDto& crime)
{
    setFormData(crime);
}

void CrimeFormWidget::clearForm()
{
    idLineEdit_->clear();
    typeComboBox_->setCurrentIndex(0);
    severityComboBox_->setCurrentIndex(2);
    dateTimeEdit_->setDateTime(QDateTime::currentDateTime());
    locationLineEdit_->clear();
    suspectLineEdit_->clear();
    descriptionTextEdit_->clear();
    notesTextEdit_->clear();
}

void CrimeFormWidget::onSave()
{
    if (!validateForm()) {
        return;
    }
    
    CrimeDto crime = getFormData();
    
    bool success = false;
    if (mode_ == Mode::Add) {
        success = adapter_->addCrime(crime);
        if (success) {
            QMessageBox::information(this, tr("Success"), tr("Crime added successfully."));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to add crime. ID may already exist."));
            return;
        }
    } else {
        success = adapter_->updateCrime(crime.id, crime);
        if (success) {
            QMessageBox::information(this, tr("Success"), tr("Crime updated successfully."));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to update crime."));
            return;
        }
    }
    
    if (success) {
        emit crimeSaved();
    }
}

void CrimeFormWidget::onCancel()
{
    emit cancelled();
}

bool CrimeFormWidget::validateForm()
{
    if (idLineEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Validation Error"), tr("Crime ID is required."));
        idLineEdit_->setFocus();
        return false;
    }
    
    if (typeComboBox_->currentText().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Validation Error"), tr("Crime type is required."));
        typeComboBox_->setFocus();
        return false;
    }
    
    if (locationLineEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Validation Error"), tr("Location is required."));
        locationLineEdit_->setFocus();
        return false;
    }
    
    return true;
}

CrimeDto CrimeFormWidget::getFormData() const
{
    CrimeDto crime;
    crime.id = idLineEdit_->text().trimmed().toStdString();
    crime.type = typeComboBox_->currentText().trimmed().toStdString();
    crime.severity = severityComboBox_->currentIndex() + 1;
    crime.epoch = dateTimeEdit_->dateTime().toSecsSinceEpoch();
    crime.location = locationLineEdit_->text().trimmed().toStdString();
    crime.suspectName = suspectLineEdit_->text().trimmed().isEmpty() 
        ? "Unknown" 
        : suspectLineEdit_->text().trimmed().toStdString();
    crime.description = descriptionTextEdit_->toPlainText().trimmed().toStdString();
    crime.notes = notesTextEdit_->toPlainText().trimmed().toStdString();
    crime.stage = CaseStage::Reported;
    
    return crime;
}

void CrimeFormWidget::setFormData(const CrimeDto& crime)
{
    idLineEdit_->setText(QString::fromStdString(crime.id));
    
    int typeIndex = typeComboBox_->findText(QString::fromStdString(crime.type));
    if (typeIndex >= 0) {
        typeComboBox_->setCurrentIndex(typeIndex);
    } else {
        typeComboBox_->setCurrentText(QString::fromStdString(crime.type));
    }
    
    severityComboBox_->setCurrentIndex(crime.severity - 1);
    dateTimeEdit_->setDateTime(QDateTime::fromSecsSinceEpoch(crime.epoch));
    locationLineEdit_->setText(QString::fromStdString(crime.location));
    suspectLineEdit_->setText(QString::fromStdString(crime.suspectName));
    descriptionTextEdit_->setPlainText(QString::fromStdString(crime.description));
    notesTextEdit_->setPlainText(QString::fromStdString(crime.notes));
}

