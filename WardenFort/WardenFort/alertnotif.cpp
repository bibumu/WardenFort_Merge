#include "alertnotif.h"
#include "ui_alertnotif.h"
#include <QVBoxLayout>
#include <QPushButton>

alertnotif::alertnotif(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::alertnotif)
{
    ui->setupUi(this);

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Create the checkbox
    QCheckBox *dd1 = new QCheckBox("Profile", this);
    dd1->setObjectName("dd1");
    dd1->setCheckable(true);
    layout->addWidget(dd1);

    // Create the options widget (e.g., QPushButton)
    QPushButton *option1 = new QPushButton("Option 1", this);
    option1->setObjectName("option1");
    option1->hide(); // Initially hide the options
    layout->addWidget(option1);

    // Connect the checkbox toggled signal to the slot for dropdown animation
    connect(dd1, &QCheckBox::toggled, this, &alertnotif::toggleDropDown);
}

alertnotif::~alertnotif()
{
    delete ui;
}

void alertnotif::toggleDropDown(bool checked)
{
    // Animation logic here
    // For example, rotating the checkbox if it's checked
    QCheckBox *checkBox = findChild<QCheckBox *>("dd1");
    if (checkBox && checked) {
        checkBox->setStyleSheet("QCheckBox#dd1::indicator {transform: rotate(90deg);}");
    } else if (checkBox && !checked) {
        checkBox->setStyleSheet("QCheckBox#dd1::indicator {transform: rotate(0deg);}");
    }
}
