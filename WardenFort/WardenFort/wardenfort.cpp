#include "WardenFort.h"
#include "ui_WardenFort.h"
#include <QColor>
#include <QTableWidgetItem>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <QApplication>
#include <QThread>
#include <limits>
#include <iphlpapi.h> 
#include <stdio.h>    
#include <ctime>
#include <QScrollBar>
#include <pcap.h>
#include <QDebug>

#pragma comment(lib, "Ws2_32.lib") // Add the following line to include Ws2_32.lib directly in your source code

int i = 0;

WardenFort::WardenFort(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::WardenFort)
{
    ui->setupUi(this);

    // Connect the clicked signal of dd buttons to the toggle function
    connect(ui->dd1, &QPushButton::clicked, this, &WardenFort::toggleButtons);
    connect(ui->dd2, &QPushButton::clicked, this, &WardenFort::toggleButtons);
    connect(ui->dd3, &QPushButton::clicked, this, &WardenFort::toggleButtons);
    connect(ui->dd4, &QPushButton::clicked, this, &WardenFort::toggleButtons);
    connect(ui->dd5, &QPushButton::clicked, this, &WardenFort::toggleButtons);
    connect(ui->dd6, &QPushButton::clicked, this, &WardenFort::toggleButtons);
    connect(ui->dd7, &QPushButton::clicked, this, &WardenFort::toggleButtons);
    connect(ui->dd8, &QPushButton::clicked, this, &WardenFort::toggleButtons);

    // Initially hide dd5 to dd8 buttons
    ui->dd5->setVisible(false);
    ui->dd6->setVisible(false);
    ui->dd7->setVisible(false);
    ui->dd8->setVisible(false);
}

WardenFort::~WardenFort()
{
    delete ui;
}


void WardenFort::setLabelText(const QString& text) {
    ui->label->setText(text);
}

QTableWidget* WardenFort::getTableWidget() {
    return ui->tableWidget; // Return the tableWidget
}

void WardenFort::setWelcomeText(const QString& text) {
    ui->welcome_text->setText(text);
}

// Define TCP header flags for Windows
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20

constexpr int MAX_EXPECTED_PAYLOAD_LENGTH = 1500; // Maximum expected payload length
constexpr int MIN_EXPECTED_PAYLOAD_LENGTH = 0;    // Minimum expected payload length (can be adjusted as needed)
constexpr int MAX_EXPECTED_PACKET_LENGTH = 70;

struct IPHeader {
    u_char  VersionAndHeaderLength;  // Version (4 bits) + Header length (4 bits)
    u_char  TypeOfService;           // Type of service
    u_short TotalLength;             // Total length
    u_short Identification;          // Identification
    u_short FlagsAndFragmentOffset;  // Flags (3 bits) + Fragment offset (13 bits)
    u_char  TimeToLive;              // Time to live
    u_char  Protocol;                // Protocol
    u_short HeaderChecksum;          // Header checksum
    u_long  Source;                  // Source address
    u_long  Destination;             // Destination address
};

struct ICMPHeader {
    uint8_t type;      // ICMP message type
    uint8_t code;      // ICMP message code
    uint16_t checksum; // ICMP message checksum
                       // Additional fields if needed
};

struct my_tcphdr {
    u_short th_sport;   // source port
    u_short th_dport;   // destination port
    u_int th_seq;       // sequence number
    u_int th_ack;       // acknowledgement number
    u_char th_offx2;    // data offset, rsvd
    u_char th_flags;
    u_short th_win;     // window
    u_short th_sum;     // checksum
    u_short th_urp;     // urgent pointer
};

bool isFilteredAdapter(pcap_if_t* adapter) {
    // Check if adapter is a WAN miniport or VirtualBox adapter by description
    if (adapter->description) {
        QString description = QString(adapter->description).toLower();
        if (description.contains("wan miniport") || description.contains("virtualbox"))
            return true;
    }

    // Check if adapter is a loopback adapter (npf_loopback)
    if (adapter->flags & PCAP_IF_LOOPBACK)
    {
        return true;
    }

    // Check if adapter is inactive
    if (!(adapter->flags & PCAP_IF_CONNECTION_STATUS_CONNECTED))
        {
        return true;
    }

    return false;
}

void analyzeTCPPacket(const u_char* packet, int packetLength, WardenFort& wardenFort) {

    // Ensure the packet is large enough to contain IP and TCP headers
    if (packetLength < sizeof(IPHeader) + sizeof(struct my_tcphdr)) {
        qDebug() << "Packet is too small to contain IP and TCP headers";
        return;
    }

    // Extract IP header from packet
    const IPHeader* ipHeader = reinterpret_cast<const IPHeader*>(packet);

    // Extract TCP header from packet
    const struct my_tcphdr* tcpHeader = reinterpret_cast<const struct my_tcphdr*>(packet + sizeof(IPHeader));

    // Convert source and destination IP addresses from network to presentation format
    char sourceIp[INET_ADDRSTRLEN];
    char destIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ipHeader->Source), sourceIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ipHeader->Destination), destIp, INET_ADDRSTRLEN);

    // Convert source and destination port numbers from network to host byte order
    u_short sourcePort = ntohs(tcpHeader->th_sport);
    u_short destPort = ntohs(tcpHeader->th_dport);

    // Calculate TCP header length
    int tcpHeaderLength = (tcpHeader->th_offx2 >> 4) * 4;

    // Calculate payload length
    int payloadLength = packetLength - sizeof(IPHeader) - tcpHeaderLength;

    // Analyze payload length
    if (payloadLength > MAX_EXPECTED_PAYLOAD_LENGTH) {
        qDebug() << "Suspiciously long payload detected from" << sourceIp << ":" << sourcePort << "to" << destIp << ":" << destPort;
        // Take appropriate action, such as logging or alerting
    }
    else if (payloadLength < MIN_EXPECTED_PAYLOAD_LENGTH) {
        qDebug() << "Suspiciously short payload detected from" << sourceIp << ":" << sourcePort << "to" << destIp << ":" << destPort;
        // Take appropriate action, such as logging or alerting
    }

    // Analyze TCP flags
    if (tcpHeader->th_flags & TH_SYN && !(tcpHeader->th_flags & TH_ACK)) {
        qDebug() << "THREAT ALERT: SYN packet without ACK detected from" << sourceIp << ":" << sourcePort << "to" << destIp << ":" << destPort;
    }
    else {
        // Reset SYN packet counter for non-SYN packets
        // syn_count = 0; // Uncomment this line if syn_count is declared and defined elsewhere
    }
    // Analyze other aspects of the TCP packet as needed
    // For example, you could check sequence numbers, window size, etc.
}

void packetHandler(WardenFort* wardenFort, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    qDebug() << "Packet captured. Length:" << pkthdr->len;

    // Convert timestamp to regular time format in Philippines time zone
    struct tm* timeinfo;
    char buffer[80];
    time_t packet_time = static_cast<time_t>(pkthdr->ts.tv_sec); // Explicit cast to time_t
    packet_time += 28800; // Adding 8 hours to convert to Philippines time zone (28800 seconds)
    timeinfo = gmtime(&packet_time);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    QString timestamp = QString(buffer);

    // Constructing textual info
    QString packetInfo = "Timestamp: " + timestamp + "\n";
    packetInfo += "Packet length: " + QString::number(pkthdr->len) + " bytes\n";
    packetInfo += "Captured length: " + QString::number(pkthdr->caplen) + " bytes\n";

    // Analyze TCP packet
    const IPHeader* ipHeader = reinterpret_cast<const IPHeader*>(packet);
    const struct my_tcphdr* tcpHeader = reinterpret_cast<const struct my_tcphdr*>(packet + sizeof(IPHeader));

    char sourceIp[INET_ADDRSTRLEN];
    char destIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ipHeader->Source), sourceIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ipHeader->Destination), destIp, INET_ADDRSTRLEN);

    QString sourcePort = QString::number(ntohs(tcpHeader->th_sport));
    QString destPort = QString::number(ntohs(tcpHeader->th_dport));
    QString flags;
    if (tcpHeader->th_flags & TH_SYN) flags += "SYN ";
    if (tcpHeader->th_flags & TH_ACK) flags += "ACK ";
    if (tcpHeader->th_flags & TH_FIN) flags += "FIN ";
    // Add other flags as needed

    // Calculate TCP header length
    int tcpHeaderLength = (tcpHeader->th_offx2 >> 4) * 4;

    // Calculate payload length
    int payloadLength = pkthdr->caplen - sizeof(IPHeader) - tcpHeaderLength; // Calculate payload length

    // Extracting the IP header from the packet

    // Populate tableWidget
    QTableWidget* tableWidget = wardenFort->getTableWidget(); // Assuming you have a getter method for tableWidget
    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);
    tableWidget->setItem(row, 0, new QTableWidgetItem(timestamp));
    tableWidget->setItem(row, 1, new QTableWidgetItem(QString(sourceIp)));
    tableWidget->setItem(row, 2, new QTableWidgetItem(QString(destIp)));
    tableWidget->setItem(row, 3, new QTableWidgetItem(sourcePort));
    tableWidget->setItem(row, 4, new QTableWidgetItem(destPort));
    tableWidget->setItem(row, 5, new QTableWidgetItem(flags));
    tableWidget->setItem(row, 6, new QTableWidgetItem(QString::number(pkthdr->caplen)));

    // Check for suspiciously long payload and change row color if detected
    if (payloadLength > MAX_EXPECTED_PAYLOAD_LENGTH) {
        i++;
        wardenFort->setLabelText(QString::number(i));
        for (int col = 0; col < tableWidget->columnCount(); ++col) {
            tableWidget->item(row, col)->setBackground(QColor(75, 44, 44)); // Change row color to red
        }
    }

    if (tcpHeader->th_flags & TH_SYN && !(tcpHeader->th_flags & TH_ACK)) {
        i++;
        wardenFort->setLabelText(QString::number(i));
        for (int col = 0; col < tableWidget->columnCount(); ++col) {
            tableWidget->item(row, col)->setBackground(QColor(75, 57, 44));
        }
    }

    if (payloadLength < MIN_EXPECTED_PAYLOAD_LENGTH) {
        i++;
        wardenFort->setLabelText(QString::number(i));
        for (int col = 0; col < tableWidget->columnCount(); ++col) {
            tableWidget->item(row, col)->setBackground(QColor(75, 44, 44)); // Change row color to red
        }
    }
    // Check if the packet is an ICMP packet
    if (ipHeader->Protocol == IPPROTO_ICMP) {
        // Calculate the total length of the packet
        int totalLength = ntohs(ipHeader->TotalLength);

        // Calculate the length of the ICMP header
        int icmpHeaderLength = sizeof(ICMPHeader);

        // Calculate the length of the ICMP payload
        int icmpPayloadLength = totalLength - sizeof(IPHeader) - icmpHeaderLength;

        // Check if the ICMP payload length exceeds a certain threshold
        constexpr int MAX_ICMP_PAYLOAD_LENGTH = 10; // Adjust as needed
        if (icmpPayloadLength > MAX_ICMP_PAYLOAD_LENGTH) {
            // If the ICMP payload is too large, log the event or take appropriate action
            qDebug() << "Large ICMP Echo Request detected. Payload Length:" << icmpPayloadLength;
            i++;
            wardenFort->setLabelText(QString::number(i));
            // Add code here to log the event or take appropriate action
            // Change row color to red
            for (int col = 0; col < tableWidget->columnCount(); ++col) {
                tableWidget->item(row, col)->setBackground(QColor(75, 44, 44));
            }
        }
    }

    // Port scanning detector
    if (tcpHeader->th_flags & TH_SYN && !(tcpHeader->th_flags & TH_ACK)) {
        // Check if the destination port is in a range commonly used for scanning
        if (ntohs(tcpHeader->th_dport) >= 1 && ntohs(tcpHeader->th_dport) <= 1024) {
            // Port scanning detected
            qDebug() << "Port Scanning Detected. Source:" << sourceIp << " Destination Port:" << destPort;
            i++;
            wardenFort->setLabelText(QString::number(i));
            // Add code here to log the event or take appropriate action
            // Change row color to another color to denote port scanning
            for (int col = 0; col < tableWidget->columnCount(); ++col) {
                tableWidget->item(row, col)->setBackground(QColor(145, 38, 38));
            }
        }
    }

    if (pkthdr->caplen > MAX_EXPECTED_PACKET_LENGTH) {
        // DoS attack suspected due to excessively large packet size
        qDebug() << "Possible Denial of Service (DoS) Attack Detected. Packet Length:" << pkthdr->caplen;
        i++;
        wardenFort->setLabelText(QString::number(i));
        // Add code here to log the event or take appropriate action
        // Change row color to another color to denote DoS attack
        for (int col = 0; col < tableWidget->columnCount(); ++col) {
            tableWidget->item(row, col)->setBackground(QColor(255, 0, 0)); // Change row color to red
        }
    }

    // Print overall packet information
    qDebug().noquote() << packetInfo << "Source IP:" << sourceIp << "Destination IP:" << destIp << "Source Port:" << sourcePort << "Destination Port:" << destPort << "Flags:" << flags << "Captured length:" << pkthdr->caplen;

    // Analyze the TCP packet
    analyzeTCPPacket(packet, pkthdr->len, *wardenFort);

    //always scroll to the bottom
    QMetaObject::invokeMethod(tableWidget->verticalScrollBar(), "setValue",
        Qt::QueuedConnection,
        Q_ARG(int, tableWidget->verticalScrollBar()->maximum()));
}

void packetHandlerWrapper(u_char* user, const struct pcap_pkthdr* pkt_header, const u_char* pkt_data) {
    WardenFort* wardenFort = reinterpret_cast<WardenFort*>(user);
    packetHandler(wardenFort, pkt_header, pkt_data);
}

void captureTCPPackets(pcap_if_t* adapter, WardenFort& wardenFort) {
    char errbuf[PCAP_ERRBUF_SIZE];

    // Open the adapter for live capturing
    pcap_t* pcapHandle = pcap_open_live(adapter->name, BUFSIZ, 1, 1000, errbuf);
    if (pcapHandle == nullptr) {
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
    if (pcap_loop(pcapHandle, 0, packetHandlerWrapper, reinterpret_cast<u_char*>(&wardenFort)) == -1) {
        qDebug() << "Error capturing packets:" << pcap_geterr(pcapHandle);
    }

    // Close the capture handle when done
    pcap_close(pcapHandle);
}

class CaptureThread : public QThread {
public:
    explicit CaptureThread(pcap_if_t* adapter, WardenFort& wardenFort) : adapter(adapter), wardenFort(wardenFort) {}

protected:
    void run() override {
        captureTCPPackets(adapter, wardenFort);
    }

private:
    pcap_if_t* adapter;
    WardenFort& wardenFort;
};

void WardenFort::scanActiveLANAdapters() { // Corrected definition
    char errbuf[PCAP_ERRBUF_SIZE];

    // Get a list of all available network adapters
    pcap_if_t* allAdapters;
    if (pcap_findalldevs(&allAdapters, errbuf) == -1) {
        qDebug() << "Error finding adapters:" << errbuf;
        return;
    }

    // Iterate over the list of adapters
    for (pcap_if_t* adapter = allAdapters; adapter != nullptr; adapter = adapter->next) {
        if (adapter->name != nullptr && !isFilteredAdapter(adapter)) {
            // Display the name and description of the LAN adapter
            qDebug() << "Active LAN adapter found:" << adapter->name << "Description:" << (adapter->description ? adapter->description : "No Description");
            

            // Start capturing TCP packets from this adapter in a separate thread
            CaptureThread* thread = new CaptureThread(adapter, *this); // Pass reference to this object
            thread->start();
        }
    }

    // Free the list of adapters
    pcap_freealldevs(allAdapters);
}

void WardenFort::toggleButtonVisibility(QPushButton* buttonToHide, QPushButton* buttonToShow)
{
    buttonToHide->setVisible(false);
    buttonToShow->setVisible(true);
}

void WardenFort::toggleButtons()
{
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton)
        return; // Safety check

    if (clickedButton == ui->dd1) {
        toggleButtonVisibility(ui->dd1, ui->dd5);
    }
    else if (clickedButton == ui->dd2) {
        toggleButtonVisibility(ui->dd2, ui->dd6);
    }
    else if (clickedButton == ui->dd3) {
        toggleButtonVisibility(ui->dd3, ui->dd7);
    }
    else if (clickedButton == ui->dd4) {
        toggleButtonVisibility(ui->dd4, ui->dd8);
    }
    else if (clickedButton == ui->dd5) {
        toggleButtonVisibility(ui->dd5, ui->dd1);
    }
    else if (clickedButton == ui->dd6) {
        toggleButtonVisibility(ui->dd6, ui->dd2);
    }
    else if (clickedButton == ui->dd7) {
        toggleButtonVisibility(ui->dd7, ui->dd3);
    }
    else if (clickedButton == ui->dd8) {
        toggleButtonVisibility(ui->dd8, ui->dd4);
    }
}
