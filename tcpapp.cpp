#include "tcpapp.h"
#include <QNetworkInterface>

TCPApp::TCPApp(QWidget *tab, QObject *parent) : QObject(parent),
    m_pTab(tab),
    m_pServer(new QTcpServer(this)),
    m_pClient(new QTcpSocket(this))
{
    initUI();
    connect(m_pClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(m_pClient, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    connect(m_pServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
}

void TCPApp::initUI() {
    this->m_pcbxHostIP = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxHostIP"));
    this->m_pcbxHostPort = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxHostPort"));
    this->m_pcbxListenIP = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxListenIP"));
    this->m_pcbxListenPort = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxListenPort"));

    this->m_pcbxHostIP->setValidator(new QRegExpValidator(QRegExp("[0-9.]+"), this));
    this->m_pcbxHostPort->setValidator(new QIntValidator(1, 65535, this));
    this->m_pcbxListenIP->setValidator(new QRegExpValidator(QRegExp("[0-9.]+"), this));
    this->m_pcbxListenPort->setValidator(new QIntValidator(1, 65535, this));
}

void TCPApp::updateUI() {
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for (const QHostAddress &item : list)
    {
        if (QAbstractSocket::IPv4Protocol == item.protocol())
        {
            if (m_pcbxHostIP->findText(item.toString()) == -1) {
                m_pcbxHostIP->addItem(item.toString());
            }
            if (m_pcbxListenIP->findText(item.toString()) == -1) {
                m_pcbxListenIP->addItem(item.toString());
            }
        }
    }
    if (m_pcbxHostIP->findText("0.0.0.0") == -1) {
        m_pcbxHostIP->addItem("0.0.0.0");
    }
    if (m_pcbxListenIP->findText("0.0.0.0") == -1) {
        m_pcbxListenIP->addItem("0.0.0.0");
    }
}

void TCPApp::initClient() {
    m_pClient->connectToHost(QHostAddress(m_pcbxHostIP->currentText()), m_pcbxHostPort->currentText().toInt());
}

void TCPApp::closeClient() {
    m_pClient->disconnectFromHost();
}

void TCPApp::onError(QAbstractSocket::SocketError socketError) {
    QString qstrError;
    QString clientErr = m_pClient->errorString();
    QString serverErr = m_pServer->errorString();
    if (clientErr != "") {
        qstrError += tr("Client error: %1").arg(clientErr);
    }
    if (serverErr != "") {
        qstrError += tr("Server error: %1").arg(serverErr);
    }
    emit errorOccurred(tr("[Error]code: %1, description: %2").arg(socketError).arg(qstrError));
}

void TCPApp::onStateChanged(QAbstractSocket::SocketState socketState) {
    QString qstrState;
    switch (socketState) {
    case QAbstractSocket::UnconnectedState:
        qstrState = tr("TCP is disconnected");
        break;
    case QAbstractSocket::HostLookupState:
        qstrState = tr("TCP is performing a host name lookup");
        break;
    case QAbstractSocket::ConnectingState:
        qstrState = tr("TCP has started establishing a connection");
        break;
    case QAbstractSocket::ConnectedState:
        qstrState = tr("TCP connection: %1:%2 is established").arg(m_pClient->peerAddress().toString()).arg(m_pClient->peerPort());
        break;
    case QAbstractSocket::BoundState:
        qstrState = tr("TCP is bound to an address and port");
        break;
    case QAbstractSocket::ClosingState:
        qstrState = tr("TCP is about to close");
        break;
    case QAbstractSocket::ListeningState:
        qstrState = tr("TCP is listening");
        break;
    default:
        qstrState = tr("Unknow state");
        break;
    }
    emit stateChanged(qstrState);
}
