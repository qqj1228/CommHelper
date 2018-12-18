#ifndef TCPAPP_H
#define TCPAPP_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QComboBox>
#include <QListWidget>
#include <QVector>
#include <QQueue>

class TCPApp : public QObject
{
    Q_OBJECT
public:
    explicit TCPApp(QWidget *tab, QObject *parent = nullptr);
    void updateUI();
    void initClient();
    void closeClient();
    QString initServer();
    QString closeServer();
    QString sendData(const QByteArray &data);
    QString listDisconnect();

signals:
    void errorOccurred(QString errorStr);
    void stateChanged(QString state);
    void connected(QString qstrSender);
    void disconnected(QString qstrSender);
    void hasRecved(QByteArray data, QString address, int enumTunnel);
    void bytesSended(qint64 bytes, QString address);

public slots:
    void onError(QAbstractSocket::SocketError socketError);
    void onBytesSended(qint64 bytes);
    void onStateChanged(QAbstractSocket::SocketState socketState);
    void readPendingData();
    void acceptConnection();

protected:
    void initUI();
    void connectSlot(QTcpSocket *pClient);
    void listAppendConn(QTcpSocket *pClient);
    void listRemoveConn(QTcpSocket *pClient);

    QWidget *m_pTab = nullptr;
    QTcpServer *m_pServer = nullptr;
    QTcpSocket *m_pClient = nullptr;
    QVector<QTcpSocket*> m_qvClient;

public:
    QComboBox *m_pcbxHostIP = nullptr;
    QComboBox *m_pcbxHostPort = nullptr;
    QComboBox *m_pcbxListenIP = nullptr;
    QComboBox *m_pcbxListenPort = nullptr;
    QListWidget *m_plistTCPConn = nullptr;
    QQueue<QByteArray> m_sendData;
};

#endif // TCPAPP_H
