#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QLabel>
#include <QMouseEvent>
#include "signup.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class login;
}
QT_END_NAMESPACE



class login : public QMainWindow
{
    Q_OBJECT

public:
    explicit login(QWidget* parent = nullptr);
    ~login();

private slots:
    void on_loginButton_released(); // Declaration of the login button click slot
    void on_signup_released();

private:
    Ui::login* ui;
    signup* signupWindow;

    bool isSignalConnected(const QObject* sender, const QMetaMethod& signal,
        const QObject* receiver, const QMetaMethod& slot);
};

#endif // LOGIN_H
