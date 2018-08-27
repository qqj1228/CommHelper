#include "tcpapp.h"
#include "mainwindow.h"
#include <QNetworkInterface>
#include <QDebug>

TCPApp::TCPApp(QWidget *tab, QObject *parent) : QObject(parent),
    m_pTab(tab),
    m_pServer(new QTcpServer(this)),
    m_pClient(new QTcpSocket(this))
{
    initUI();
    connectSlot(m_pClient);
    connect(m_pServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    connect(m_pServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
}

void TCPApp::initUI() {
    this->m_pcbxHostIP = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxHostIP"));
    this->m_pcbxHostPort = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxHostPort"));
    this->m_pcbxListenIP = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxListenIP"));
    this->m_pcbxListenPort = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxListenPort"));
    this->m_plistTCPConn = m_pTab->findChild<QListWidget*>(QStringLiteral("listTCPConn"));

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

void TCPApp::connectSlot(QTcpSocket *pClient) {
    connect(pClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(pClient, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    connect(pClient, SIGNAL(readyRead()), this, SLOT(readPendingData()));
    connect(pClient, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesSended(qint64)));
}

void TCPApp::initClient() {
    m_pClient->connectToHost(QHostAddress(m_pcbxHostIP->currentText()), m_pcbxHostPort->currentText().toInt());
}

void TCPApp::closeClient() {
    m_pClient->disconnectFromHost();
}

QString TCPApp::initServer() {
    if (m_pServer->listen(QHostAddress(m_pcbxListenIP->currentText()), m_pcbxListenPort->currentText().toInt())) {
        return tr("Open TCP Server on \"%1:%2\"").arg(m_pServer->serverAddress().toString()).arg(m_pServer->serverPort());
    } else {
        return tr("[Error]Can not open TCP Server: %1").arg(m_pServer->errorString());
    }
}

QString TCPApp::closeServer() {
    m_pServer->close();
    for (QTcpSocket* pClient : m_qvClient) {
        pClient->disconnectFromHost();
    }
    return tr("Closed TCP Server");
}

void TCPApp::onError(QAbstractSocket::SocketError socketError) {
    QString qstrError;
    QTcpSocket* pClient = qobject_cast<QTcpSocket*>(sender());
    if (pClient == nullptr) {
        qstrError = tr("Server error: %1").arg(m_pServer->errorString());
    } else {
        qstrError = tr("Client error: %1").arg(pClient->errorString());
    }
    emit errorOccurred(tr("[Error]code: %1, description: %2").arg(socketError).arg(qstrError));
}

void TCPApp::onBytesSended(qint64 bytes) {
    QTcpSocket* pClient = qobject_cast<QTcpSocket*>(sender());
    QString address = QString("%1:%2").arg(pClient->peerAddress().toString()).arg(pClient->peerPort());
    emit bytesSended(bytes, address);
}

void TCPApp::onStateChanged(QAbstractSocket::SocketState socketState) {
    QString qstrState;
    QString qstrSender = "newClient";
    QTcpSocket* pClient = qobject_cast<QTcpSocket*>(sender());
    if (pClient == m_pClient) {
        qstrSender = "TCPClient";
    }
    switch (socketState) {
    case QAbstractSocket::UnconnectedState:
        qstrState = tr("TCP Client is disconnected");
        emit disconnected(qstrSender);
        if (qstrSender != "TCPClient") {
            listRemoveConn(pClient);
        }
        break;
    case QAbstractSocket::HostLookupState:
        qstrState = tr("TCP is performing a host name lookup");
        break;
    case QAbstractSocket::ConnectingState:
        qstrState = tr("TCP has started establishing a connection");
        break;
    case QAbstractSocket::ConnectedState:
        qstrState = tr("TCP connection \"%1:%2\" is established").arg(pClient->peerAddress().toString()).arg(pClient->peerPort());
        emit connected(qstrSender);
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

void TCPApp::readPendingData()
{
    QTcpSocket* pClient = qobject_cast<QTcpSocket*>(sender());
    while (pClient->bytesAvailable() > 0) {
        QByteArray data = pClient->readAll();
        if (data.size() > 0) {
            QString address = QString("%1:%2").arg(pClient->peerAddress().toString()).arg(pClient->peerPort());
            emit hasRecved(data, address, MainWindow::TCPTab);
        }
    }
}

QString TCPApp::sendData(const QByteArray &data) {
    qint64 ret = 0;
    qint64 size = 0;
    QString message, qstrFailed;
    if (m_pClient->state() == QAbstractSocket::ConnectedState) {
        ret = m_pClient->write(data);
        if (ret > -1) {
            message = tr("TCP Client Send Data Successfully: %1 Bytes").arg(ret);
        } else {
            message = tr("[Error]TCP Client Failed to Send Data: %1").arg(m_pClient->errorString());
        }
    } else if (m_pServer->isListening()) {
        QList<QListWidgetItem *> items = m_plistTCPConn->selectedItems();
        for (QListWidgetItem *item : items) {
            int index = m_plistTCPConn->row(item);
            ret = m_qvClient.at(index)->write(data);
            if (ret > -1) {
                size += ret;
            } else {
                qstrFailed += QString("\"%1:%2\": %3, ")
                        .arg(m_qvClient.at(index)->peerAddress().toString())
                        .arg(m_qvClient.at(index)->peerPort())
                        .arg(m_pClient->errorString());
            }
        }
        if (items.size() > 0) {
            if (qstrFailed.isEmpty()) {
                message = QString("TCP Server Send Data Successfully: %1 Bytes").arg(size);
            } else {
                message = "[Error]Failed to Send Data: " + qstrFailed;
            }
        } else {
            message = tr("[Error]No selected TCP Client connection");
        }
    } else {
        message = tr("[Error]TCP Client or Server not open");
    }
    return message;
}

void TCPApp::acceptConnection() {
    while (m_pServer->hasPendingConnections()) {
        QTcpSocket *pnewClient = m_pServer->nextPendingConnection();
        connectSlot(pnewClient);
        this->listAppendConn(pnewClient);
    }
}

void TCPApp::listAppendConn(QTcpSocket *pClient) {
    QString address = QString("%1:%2").arg(pClient->peerAddress().toString()).arg(pClient->peerPort());
    if (m_plistTCPConn->findItems(address, Qt::MatchFixedString).size() == 0) {
        m_plistTCPConn->addItem(address);
        m_qvClient.push_back(pClient);
    }
}

void TCPApp::listRemoveConn(QTcpSocket *pClient) {
    QString address = QString("%1:%2").arg(pClient->peerAddress().toString()).arg(pClient->peerPort());
    QList<QListWidgetItem *> itemList = m_plistTCPConn->findItems(address, Qt::MatchFixedString);
    for (QListWidgetItem *item : itemList) {
        int index = m_plistTCPConn->row(item);
        delete m_plistTCPConn->takeItem(index);
        m_qvClient.remove(index);
    }
}

QString TCPApp::listDisconnect() {
    QList<QListWidgetItem *> items = m_plistTCPConn->selectedItems();
    for (QListWidgetItem *item : items) {
        int index = m_plistTCPConn->row(item);
        m_qvClient.at(index)->disconnectFromHost();
    }
    return tr("All selected TCP Client has disconnected");
}
