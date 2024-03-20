#include "wardenfort.h"
#include "ui_wardenfort.h"

wardenfort::wardenfort(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::wardenfort)
{
    ui->setupUi(this);
}

wardenfort::~wardenfort()
{
    delete ui;
}
