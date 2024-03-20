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
    void on_loginButton_clicked(); // Declaration of the login button click slot
    void on_signup_clicked();

private:
    Ui::login* ui;
    signup* signupWindow;
};

#endif // LOGIN_H
