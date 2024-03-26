#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>

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
    void on_loginButton_released();
    void on_eye_open_clicked();
    void on_eye_closed_clicked();

private:
    Ui::login* ui;


private slots:
    void on_typePASS_box_returnPressed();

};

#endif // LOGIN_H
