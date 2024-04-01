// WardenFort.h
#ifndef WARDENFORT_H
#define WARDENFORT_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class WardenFort; }
QT_END_NAMESPACE

class WardenFort : public QMainWindow
{
    Q_OBJECT

public:
    void toggleButtonVisibility(QPushButton* buttonToHide, QPushButton* buttonToShow);
    WardenFort(QWidget* parent = nullptr);
    ~WardenFort();
    void setLabelText(const QString& text);
    QTableWidget* getTableWidget(); // Getter method for tableWidget
    void scanActiveLANAdapters(); // Corrected declaration
    void setWelcomeText(const QString& text);
    void toggleButtons();

private slots:
    

private:
    Ui::WardenFort* ui;
};

#endif // WARDENFORT_H
