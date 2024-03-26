#ifndef ALERTNOTIF_H
#define ALERTNOTIF_H

#include <QDialog>
#include <QCheckBox> // Include necessary headers for QCheckBox

namespace Ui {
class alertnotif;
}

class alertnotif : public QDialog
{
    Q_OBJECT

public:
    explicit alertnotif(QWidget *parent = nullptr);
    ~alertnotif();

private slots:
    void toggleDropDown(bool checked); // Declaration of toggleDropDown function

private:
    Ui::alertnotif *ui; // Declaration of ui pointer
};

#endif // ALERTNOTIF_H
