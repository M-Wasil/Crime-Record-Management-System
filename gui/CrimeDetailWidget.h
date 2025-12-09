#ifndef CRIMEDETAILWIDGET_H
#define CRIMEDETAILWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include "../adapter/UiCrimeAdapter.h"

class CrimeDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CrimeDetailWidget(UiCrimeAdapter* adapter, QWidget *parent = nullptr);
    void loadCrime(const CrimeDto& crime);

signals:
    void editRequested();
    void backToList();

private slots:
    void onEdit();
    void onBack();

private:
    void setupUI();
    void clearDetails();

    UiCrimeAdapter* adapter_;
    
    // Display labels
    QLabel* idLabel_;
    QLabel* typeLabel_;
    QLabel* severityLabel_;
    QLabel* dateLabel_;
    QLabel* locationLabel_;
    QLabel* suspectLabel_;
    QLabel* officerLabel_;
    QLabel* stageLabel_;
    QTextEdit* descriptionTextEdit_;
    QTextEdit* notesTextEdit_;
    
    QString currentCrimeId_;
};

#endif // CRIMEDETAILWIDGET_H

