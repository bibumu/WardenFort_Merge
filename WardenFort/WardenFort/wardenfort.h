#ifndef WARDENFORT_H
#define WARDENFORT_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class wardenfort;
}
QT_END_NAMESPACE

class wardenfort : public QMainWindow
{
    Q_OBJECT

public:
    wardenfort(QWidget *parent = nullptr);
    ~wardenfort();

private:
    Ui::wardenfort *ui;
};
#endif // WARDENFORT_H
