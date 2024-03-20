#include "wardenfort.h"  
#include "ui_wardenfort.h"
#include <pcap.h>
#include <tchar.h>
#include <Winsock2.h>
#include <QThread>
#include <functional>


wardenfort::wardenfort(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::wardenfort)
{
    ui->setupUi(this);
    listModel = new QStringListModel(this);
    // Set the model for the listView
    ui->listPacket->setModel(listModel);
    set_LineEdit_Text();
    ui->listPacket->setModel(listModel);
    initialize_Devices();
    // Initialize the QStringListModel
    
    // Set up the combo box
    connect(ui->listBox, QOverload<int>::of(&QComboBox::activated), this, &wardenfort::on_listBox_currentIndexChanged);

    // Connect the returnPressed signal of the QLineEdit to the on_LineEdit_ReturnPressed() slot
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &wardenfort::on_LineEdit_ReturnPressed);
}

wardenfort::~wardenfort()
{
    delete ui;
}

void wardenfort::set_LineEdit_Text() {
    // Assuming your QLineEdit widget has an object name "lineEdit"
    ui->lineEdit->setText(""); // Set the text of the line edit widget
}

void wardenfort::on_LineEdit_ReturnPressed()
{
    // This function will be called when the return key is pressed in the QLineEdit
    QString text = ui->lineEdit->text();
    // Do whatever you want with the text, for example:
    qDebug() << "Text entered: " << text;
    updateList(text);
    set_LineEdit_Text();

}

void wardenfort::on_listBox_currentIndexChanged(int index) {
    // Ensure index is valid
    if (index < 0 || index >= ui->listBox->count())
        return;

    // Jump to the selected adapter
    pcap_if_t* d = alldevs;
    for (int i = 0; i < index; i++) {
        if (d == nullptr)
            return; // Error handling
        d = d->next;
    }

    // Now d points to the selected adapter, you can perform further operations with it
    qDebug() << "Selected adapter: " << d->name;
    

}

void wardenfort::initialize_Devices() {
    char errbuf[PCAP_ERRBUF_SIZE]; // Buffer to store error messages
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        QTextStream out(stdout);
        out << "Error in pcap_findalldevs: " << errbuf << "\n";
        out.flush();
        return;
    }

    // Add devices to comboBox
    for (pcap_if_t* dev = alldevs; dev != nullptr; dev = dev->next) {
        ui->listBox->addItem(dev->name);
    }

    // Free the device list
    pcap_freealldevs(alldevs);

}
void wardenfort::freeDeviceList() {
    if (alldevs != nullptr) {
        pcap_freealldevs(alldevs);
        alldevs = nullptr;
    }
}

void wardenfort::updateList(QString& text)
{
    // Retrieve the current list of strings from the model
    QStringList stringList = listModel->stringList();

    // Add the new text to the list
    stringList.append(text);

    // Update the model with the new list
    listModel->setStringList(stringList);
}