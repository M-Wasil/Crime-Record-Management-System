#ifndef CRIMEFORMWIDGET_H
#define CRIMEFORMWIDGET_H

#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "../adapter/UiCrimeAdapter.h"

class CrimeFormWidget : public QWidget
{
    Q_OBJECT

public:
    enum class Mode { Add, Edit };
    
    explicit CrimeFormWidget(UiCrimeAdapter* adapter, QWidget *parent = nullptr);
    void setMode(Mode mode);
    void loadCrime(const CrimeDto& crime);
    void clearForm();

signals:
    void crimeSaved();
    void cancelled();

private slots:
    void onSave();
    void onCancel();

private:
    void setupUI();
    bool validateForm();
    CrimeDto getFormData() const;
    void setFormData(const CrimeDto& crime);

    UiCrimeAdapter* adapter_;
    Mode mode_;
    
    // Form fields
    QLineEdit* idLineEdit_;
    QComboBox* typeComboBox_;
    QComboBox* severityComboBox_;
    QDateTimeEdit* dateTimeEdit_;
    QLineEdit* locationLineEdit_;
    QLineEdit* suspectLineEdit_;
    QTextEdit* descriptionTextEdit_;
    QTextEdit* notesTextEdit_;
    
    // Buttons
    QPushButton* saveButton_;
    QPushButton* cancelButton_;
};

#endif // CRIMEFORMWIDGET_H

