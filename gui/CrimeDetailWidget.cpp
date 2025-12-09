#include "CrimeDetailWidget.h"
#include <QDateTime>
#include <QHBoxLayout>

CrimeDetailWidget::CrimeDetailWidget(UiCrimeAdapter* adapter, QWidget *parent)
    : QWidget(parent)
    , adapter_(adapter)
{
    setupUI();
    clearDetails();
}

void CrimeDetailWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Title and back button
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* titleLabel = new QLabel(tr("<h2>Crime Details</h2>"), this);
    QPushButton* backButton = new QPushButton(tr("&Back to List"), this);
    connect(backButton, &QPushButton::clicked, this, &CrimeDetailWidget::onBack);
    
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(backButton);
    mainLayout->addLayout(headerLayout);
    
    // Details form
    QFormLayout* formLayout = new QFormLayout();
    
    idLabel_ = new QLabel(this);
    formLayout->addRow(tr("Crime ID:"), idLabel_);
    
    typeLabel_ = new QLabel(this);
    formLayout->addRow(tr("Type:"), typeLabel_);
    
    severityLabel_ = new QLabel(this);
    formLayout->addRow(tr("Severity:"), severityLabel_);
    
    dateLabel_ = new QLabel(this);
    formLayout->addRow(tr("Date & Time:"), dateLabel_);
    
    locationLabel_ = new QLabel(this);
    formLayout->addRow(tr("Location:"), locationLabel_);
    
    suspectLabel_ = new QLabel(this);
    formLayout->addRow(tr("Suspect:"), suspectLabel_);
    
    officerLabel_ = new QLabel(this);
    formLayout->addRow(tr("Assigned Officer:"), officerLabel_);
    
    stageLabel_ = new QLabel(this);
    formLayout->addRow(tr("Case Stage:"), stageLabel_);
    
    descriptionTextEdit_ = new QTextEdit(this);
    descriptionTextEdit_->setReadOnly(true);
    descriptionTextEdit_->setMaximumHeight(100);
    formLayout->addRow(tr("Description:"), descriptionTextEdit_);
    
    notesTextEdit_ = new QTextEdit(this);
    notesTextEdit_->setReadOnly(true);
    notesTextEdit_->setMaximumHeight(100);
    formLayout->addRow(tr("Notes:"), notesTextEdit_);
    
    mainLayout->addLayout(formLayout);
    
    // Action buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton* editButton = new QPushButton(tr("&Edit"), this);
    connect(editButton, &QPushButton::clicked, this, &CrimeDetailWidget::onEdit);
    
    buttonLayout->addWidget(editButton);
    mainLayout->addLayout(buttonLayout);
    
    mainLayout->addStretch();
}

void CrimeDetailWidget::loadCrime(const CrimeDto& crime)
{
    currentCrimeId_ = QString::fromStdString(crime.id);
    
    idLabel_->setText(QString::fromStdString(crime.id));
    typeLabel_->setText(QString::fromStdString(crime.type));
    severityLabel_->setText(QString::number(crime.severity));
    
    QDateTime dt = QDateTime::fromSecsSinceEpoch(crime.epoch);
    dateLabel_->setText(dt.toString("yyyy-MM-dd hh:mm:ss"));
    
    locationLabel_->setText(QString::fromStdString(crime.location));
    suspectLabel_->setText(QString::fromStdString(crime.suspectName));
    
    QString officerText = crime.officerId.empty() ? tr("Not assigned") : QString::fromStdString(crime.officerId);
    officerLabel_->setText(officerText);
    
    QString stageText;
    switch (crime.stage) {
    case CaseStage::Reported: stageText = tr("Reported"); break;
    case CaseStage::Assigned: stageText = tr("Assigned"); break;
    case CaseStage::Investigation: stageText = tr("Investigation"); break;
    case CaseStage::Closed: stageText = tr("Closed"); break;
    }
    stageLabel_->setText(stageText);
    
    descriptionTextEdit_->setPlainText(QString::fromStdString(crime.description));
    notesTextEdit_->setPlainText(QString::fromStdString(crime.notes));
}

void CrimeDetailWidget::clearDetails()
{
    idLabel_->clear();
    typeLabel_->clear();
    severityLabel_->clear();
    dateLabel_->clear();
    locationLabel_->clear();
    suspectLabel_->clear();
    officerLabel_->clear();
    stageLabel_->clear();
    descriptionTextEdit_->clear();
    notesTextEdit_->clear();
    currentCrimeId_.clear();
}

void CrimeDetailWidget::onEdit()
{
    emit editRequested();
}

void CrimeDetailWidget::onBack()
{
    emit backToList();
}

