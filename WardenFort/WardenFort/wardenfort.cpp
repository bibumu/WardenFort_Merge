#include "wardenfort.h"  
#include "ui_wardenfort.h"
#include <pcap.h>
#include <tchar.h>
#include <Winsock2.h>
#include <QThread>
#include <ws2tcpip.h> 
#include <functional>
#include <QString>
int i = 0;
// Define TCP header flags for Windows
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20

constexpr int MAX_EXPECTED_PAYLOAD_LENGTH = 1500; // Maximum expected payload length
constexpr int MIN_EXPECTED_PAYLOAD_LENGTH = 0;    // Minimum expected payload length (can be adjusted as needed)
struct my_tcphdr {
    u_short th_sport;  // source port
    u_short th_dport;  // destination port
    u_int th_seq;      // sequence number
    u_int th_ack;      // acknowledgement number
    u_char th_offx2;   // data offset, rsvd
    u_char th_flags;
    u_short th_win;     // window
    u_short th_sum;     // checksum
    u_short th_urp;     // urgent pointer
};



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


bool isFilteredAdapter(pcap_if_t* adapter) {
    // Check if adapter is a WAN miniport or VirtualBox adapter by description
    if (adapter->description) {
        QString description = QString(adapter->description).toLower();
        if (description.contains("wan miniport") || description.contains("virtualbox"))
            return true;
    }

    // Check if adapter is a loopback adapter (npf_loopback)
    if (adapter->flags & PCAP_IF_LOOPBACK)
        return true;

    // Check if adapter is inactive
    if (!(adapter->flags & PCAP_IF_CONNECTION_STATUS_CONNECTED))
        return true;

    return false;
}

void analyzeTCPPacket(const u_char* packet, int packetLength, wardenfort& wardenfort) {


    // Ensure the packet is large enough to contain a TCP header
    if (packetLength < sizeof(struct my_tcphdr)) {
        qDebug() << "Packet is too small to contain TCP header";
        return;
    }

    // Extract TCP header from packet
    const struct my_tcphdr* tcpHeader = reinterpret_cast<const struct my_tcphdr*>(packet);

    // Calculate TCP header length
    int tcpHeaderLength = (tcpHeader->th_offx2 >> 4) * 4;

    // Calculate payload length
    int payloadLength = packetLength - tcpHeaderLength;

    // Analyze payload length
    if (payloadLength > MAX_EXPECTED_PAYLOAD_LENGTH) {
        i++;
        qDebug() << "Suspiciously long payload detected!";
        qDebug() << i;
        wardenfort.ui->label->setText(QString::fromStdString(std::to_string(i)) + " Threat");        // Take appropriate action, such as logging or alerting

    }
    else if (payloadLength < MIN_EXPECTED_PAYLOAD_LENGTH) {
        qDebug() << "Suspiciously short payload detected!";
        // Take appropriate action, such as logging or alerting
    }

    // Analyze TCP flags
    if (tcpHeader->th_flags & TH_SYN && !(tcpHeader->th_flags & TH_ACK)) {
        qDebug() << "THREAT ALERT: SYN packet without ACK detected!";
        // You may want to implement threatCounter and syn_count here
    }
    else {
        // Reset SYN packet counter for non-SYN packets
        // syn_count = 0; // Uncomment this line if syn_count is declared and defined elsewhere
    }

    // Analyze other aspects of the TCP packet as needed
    // For example, you could check sequence numbers, window size, etc.
}

void packetHandler(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    wardenfort& wardenFort = *reinterpret_cast<wardenfort*>(userData);
    qDebug() << "Packet captured. Length:" << pkthdr->len;
    analyzeTCPPacket(packet, pkthdr->len, wardenFort);
}

void captureTCPPackets(pcap_if_t* adapter, wardenfort& wardenfort) {
    char errbuf[PCAP_ERRBUF_SIZE];

    // Open the adapter for live capturing
    pcap_t* pcapHandle = pcap_open_live(adapter->name, BUFSIZ, 1, 1000, errbuf);
    if (pcapHandle == nullptr) {
        qDebug() << adapter;
        qDebug() << "Error opening adapter for capturing:" << errbuf;
        return;
    }

    // Apply a filter to capture only TCP packets
    struct bpf_program filter;
    if (pcap_compile(pcapHandle, &filter, "tcp", 0, PCAP_NETMASK_UNKNOWN) == -1) {
        qDebug() << "Error compiling filter:" << pcap_geterr(pcapHandle);
        pcap_close(pcapHandle);
        return;
    }
    if (pcap_setfilter(pcapHandle, &filter) == -1) {
        qDebug() << "Error setting filter:" << pcap_geterr(pcapHandle);
        pcap_freecode(&filter);
        pcap_close(pcapHandle);
        return;
    }
    pcap_freecode(&filter);

    // Start capturing packets
    pcap_loop(pcapHandle, 0, packetHandler, reinterpret_cast<u_char*>(&wardenfort));

    // Close the capture handle when done
    pcap_close(pcapHandle);
}


class CaptureThread : public QThread {
public:
    explicit CaptureThread(pcap_if_t* adapter, wardenfort& wardenFort) : adapter(adapter), wardenFort(wardenFort) {}

protected:
    void run() override {
        captureTCPPackets(adapter, wardenFort);
    }

private:
    pcap_if_t* adapter;
    wardenfort& wardenFort;
};

