#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_wardenfort.h"

class wardenfort : public QMainWindow
{
    Q_OBJECT

public:
    wardenfort(QWidget *parent = nullptr);
    ~wardenfort();

private:
    Ui::wardenfortClass ui;
};
