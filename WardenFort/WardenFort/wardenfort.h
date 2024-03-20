#ifndef WARDENFORT_H
#define WARDENFORT_H

#include <QMainWindow>
#include <QStringListModel>
#include <pcap.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class wardenfort;
}
QT_END_NAMESPACE

class wardenfort : public QMainWindow
{
    Q_OBJECT

public:
    wardenfort(QWidget *parent = nullptr);
    ~wardenfort();
private slots:
    void on_LineEdit_ReturnPressed();
    void updateList(QString& text);
    void on_listBox_currentIndexChanged(int index);
private:
    Ui::wardenfort* ui;
    
    QStringListModel *listModel;
    pcap_if_t* alldevs = nullptr;
    pcap_t* adhandle = nullptr;
    char errbuf[PCAP_ERRBUF_SIZE]; // Buffer for error messages
    struct bpf_program fcode; // BPF program structure for packet filter
    u_int netmask; // Network mask

    u_int ip_len; // IP packet length
    u_short sport, dport; // Source and destination ports

    // Define IP address structure
    /* Define IP header structure */
    typedef struct ip_address {
        u_char byte1;
        u_char byte2;
        u_char byte3;
        u_char byte4;
    } ip_address;

    /* Define IPv4 header structure */
    typedef struct ip_header {
        u_char  ver_ihl;            // Version (4 bits) + IP header length (4 bits)
        u_char  tos;                // Type of service 
        u_short tlen;               // Total length 
        u_short identification;     // Identification
        u_short flags_fo;           // Flags (3 bits) + Fragment offset (13 bits)
        u_char  ttl;                // Time to live
        u_char  proto;              // Protocol
        u_short crc;                // Header checksum
        ip_address  saddr;          // Source address
        ip_address  daddr;          // Destination address
        u_int  op_pad;              // Option + Padding
    } ip_header;

    /* Define UDP header structure */
    typedef struct udp_header {
        u_short sport;  // Source port
        u_short dport;  // Destination port
        u_short len;    // Datagram length
        u_short crc;    // Checksum
    } udp_header;



    ip_header* ih; // IP header pointer
    pcap_if_t* dev;
    udp_header* uh; // UDP header pointer


    // Other member functions
    void initialize_Devices();
    void set_LineEdit_Text();
    bool loadNpcapDlls();
    void freeDeviceList();
};
#endif // WARDENFORT_H
