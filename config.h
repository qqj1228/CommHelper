#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QSettings>
#include <QComboBox>
#include "serialport.h"
#include "setup.h"
#include "tcpapp.h"
#include "udpapp.h"

#define CFG_APP_NAME "CommHelper"
#define CFG_SEC_SENDED "Sended"
#define CFG_KEY_ITEM "item"
#define CFG_SEC_FILTER "Filter"
#define CFG_SEC_SERIAL "SerialPort"
#define CFG_KEY_PORT "Port"
#define CFG_KEY_BAUD "Baud"
#define CFG_KEY_BIT "Bit"
#define CFG_KEY_PARITY "Parity"
#define CFG_KEY_STOP "Stop"
#define CFG_KEY_FLOW "Flow"
#define CFG_SEC_DESTIP "DestIP"
#define CFG_SEC_DESTPORT "DestPort"
#define CFG_SEC_RECVIP "RecvIP"
#define CFG_SEC_RECVPORT "RecvPort"
#define CFG_SEC_HOSTIP "HostIP"
#define CFG_SEC_HOSTPORT "HostPort"
#define CFG_SEC_LISTENIP "ListenIP"
#define CFG_SEC_LISTENPORT "ListenPort"
#define CFG_SEC_TCP "TCP"
#define CFG_KEY_HOSTIP "HostIP"
#define CFG_KEY_HOSTPORT "HostPort"
#define CFG_KEY_LISTENIP "ListenIP"
#define CFG_KEY_LISTENPORT "ListenPort"
#define CFG_SEC_UDP "UDP"
#define CFG_KEY_DESTIP "DestIP"
#define CFG_KEY_RECVIP "RecvIP"
#define CFG_KEY_DESTPORT "DestPort"
#define CFG_KEY_RECVPORT "RecvPort"
#define CFG_SEC_SETUP "Setup"
#define CFG_KEY_SENDCLR "SendColor"
#define CFG_KEY_RECVCLR "RecvColor"
#define CFG_KEY_FONTSIZE "FontSize"
#define CFG_KEY_HISTORY "MaxHistory"
#define CFG_KEY_TEXTCODE "TextCode"

class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = nullptr);
    ~Config();

    void setQString(const QString& section, const QString& key, const QString& val);
    void setInt(const QString& section, const QString& key, const int val);
    QString getQString(const QString& section, const QString& key, const QString& def);
    void setHistory(const QComboBox* cmb, const QString &section, const QString& val);
    void saveHistory(const QComboBox* cmb, const QString &section);
    void loadHistory(QComboBox* cmb, const QString &section);
    void remove(const QString &topKey);
    void saveSerialPort(const SerialPort *pMySerialPort);
    void loadSerialPort(SerialPort *pMySerialPort);
    void saveSetup(const Setup *pMySetup);
    void loadSetup(Setup *pMySetup);
    void saveUDP(const UDPApp *pMyUDP);
    void loadUDP(UDPApp *pMyUDP);
    void saveTCP(const TCPApp *pMyTCP);
    void loadTCP(TCPApp *pMyTCP);

signals:

public slots:

private:
    QSettings *m_pConfig = nullptr;
};

#endif // CONFIG_H
