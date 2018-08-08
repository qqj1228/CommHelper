#ifndef TCPAPP_H
#define TCPAPP_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QComboBox>

class TCPApp : public QObject
{
    Q_OBJECT
public:
    explicit TCPApp(QWidget *tab, QObject *parent = nullptr);
    void updateUI();
    void initClient();
    void closeClient();

signals:
    void errorOccurred(QString errorStr);
    void stateChanged(QString state);

public slots:
    void onError(QAbstractSocket::SocketError socketError);
    void onStateChanged(QAbstractSocket::SocketState socketState);

protected:
    void initUI();

    QWidget *m_pTab = nullptr;
    QTcpServer *m_pServer = nullptr;
    QTcpSocket *m_pClient = nullptr;

public:
    QComboBox *m_pcbxHostIP = nullptr;
    QComboBox *m_pcbxHostPort = nullptr;
    QComboBox *m_pcbxListenIP = nullptr;
    QComboBox *m_pcbxListenPort = nullptr;
};

#endif // TCPAPP_H
