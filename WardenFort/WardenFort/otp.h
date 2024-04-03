#ifndef OTP_H
#define OTP_H

#include "qmainwindow.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class otp;
}
QT_END_NAMESPACE

class otp : public QDialog
{
    Q_OBJECT

public:
    otp(QWidget *parent = nullptr);
    ~otp();

private:
    Ui::otp *ui;
};
#endif // OTP_H
