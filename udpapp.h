#ifndef UDPAPP_H
#define UDPAPP_H

#include <QObject>
#include <QUdpSocket>
#include <QComboBox>
#include <QListWidget>
#include <QCheckBox>

class UDPApp : public QObject
{
    Q_OBJECT
public:
    explicit UDPApp(QWidget *tab, QObject *parent = nullptr);
    void updateUI();
    QString getAddress(bool bSend);
    QString initUDP();
    QString closeUDP();
    QString sendData(const QByteArray &data);

signals:
    void hasRecved(QByteArray data, int enumTunnel);
    void errorOccurred(QString errorStr);
    void bytesSended(qint64 bytes);

public slots:
    void readPendingDatagrams();
    void onError(QAbstractSocket::SocketError socketError);

protected:
    void initUI();

    QWidget *m_pTab = nullptr;
    QUdpSocket *m_pUDP = nullptr;
    QString m_sendAddress;
    QString m_recvAddress;

public:
    QComboBox *m_pcbxDestIP = nullptr;
    QComboBox *m_pcbxDestPort = nullptr;
    QComboBox *m_pcbxRecvIP = nullptr;
    QComboBox *m_pcbxRecvPort = nullptr;
    QListWidget *m_plistUDPConn = nullptr;
    QCheckBox *m_pcheckConn = nullptr;
};

#endif // UDPAPP_H
