#include "udpapp.h"
#include "mainwindow.h"
#include "config.h"
#include <QNetworkInterface>
#include <QNetworkDatagram>

UDPApp::UDPApp(QWidget *tab, QObject *parent) : QObject(parent),
    m_pTab(tab),
    m_pSend(new QUdpSocket(this)),
    m_pRecv(new QUdpSocket(this))
{
    initUI();
    connect(m_pRecv, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
    connect(m_pRecv, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(m_pSend, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(m_pSend, SIGNAL(bytesWritten(qint64)), this, SIGNAL(bytesSended(qint64)));
}

UDPApp::~UDPApp() {
//    delete m_pRecv;
//    delete m_pSend;
}

void UDPApp::initUI() {
    this->m_pcbxDestIP = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxDestIP"));
    this->m_pcbxDestPort = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxDestPort"));
    this->m_pcbxRecvIP = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxRecvIP"));
    this->m_pcbxRecvPort = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxRecvPort"));

    this->m_pcbxDestIP->setValidator(new QRegExpValidator(QRegExp("[0-9.]+"), this));
    this->m_pcbxDestPort->setValidator(new QIntValidator(1, 65535, this));
    this->m_pcbxRecvIP->setValidator(new QRegExpValidator(QRegExp("[0-9.]+"), this));
    this->m_pcbxRecvPort->setValidator(new QIntValidator(1, 65535, this));
}

QString UDPApp::initRecv() {
    bool bResult = m_pRecv->bind(QHostAddress(m_pcbxRecvIP->currentText()), m_pcbxRecvPort->currentText().toInt());
    if (bResult) {
        return QString("已打开UDP接收端口：%1:%2").arg(m_pRecv->localAddress().toString()).arg(m_pRecv->localPort());
    } else {
        return QString("[Error]无法打开UDP接收端口: %1").arg(m_pRecv->errorString());
    }
}

QString UDPApp::closeRecv() {
    m_pRecv->close();
    return QString("已关闭UDP接收端口");
}

void UDPApp::updateUI() {
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for (const QHostAddress &item : list)
    {
        if (QAbstractSocket::IPv4Protocol == item.protocol())
        {
            if (m_pcbxDestIP->findText(item.toString()) == -1) {
                m_pcbxDestIP->addItem(item.toString());
            }
            if (m_pcbxRecvIP->findText(item.toString()) == -1) {
                m_pcbxRecvIP->addItem(item.toString());
            }
        }
    }
    if (m_pcbxDestIP->findText("0.0.0.0") == -1) {
        m_pcbxDestIP->addItem("0.0.0.0");
    }
    if (m_pcbxRecvIP->findText("0.0.0.0") == -1) {
        m_pcbxRecvIP->addItem("0.0.0.0");
    }
}

void UDPApp::readPendingDatagrams()
{
    while (m_pRecv->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_pRecv->receiveDatagram();
        m_recvAddress = QString("%1:%2").arg(datagram.senderAddress().toString()).arg(datagram.senderPort());
        emit hasRecved(datagram.data(), MainWindow::UDPTab);
    }
}

QString UDPApp::getAddress(bool bSend) {
    if (bSend) {
        return m_sendAddress;
    } else {
        return m_recvAddress;
    }
}

void UDPApp::onError(QAbstractSocket::SocketError socketError) {
    QString qstrError;
    QString recvErr = m_pRecv->errorString();
    QString sendErr = m_pSend->errorString();
    if (recvErr != "") {
        qstrError += QString("Listen error: %1, ").arg(recvErr);
    }
    if (sendErr != "") {
        qstrError += QString("Send error: %2, ").arg(sendErr);
    }
    emit errorOccurred(QString("[Error]code: %1, description: %2").arg(socketError).arg(qstrError));
}

QString UDPApp::sendData(const QByteArray &data) {
    m_sendAddress = QString("%1:%2").arg(m_pcbxDestIP->currentText()).arg(m_pcbxDestPort->currentText());
    qint64 ret = m_pSend->writeDatagram(data, QHostAddress(m_pcbxDestIP->currentText()), m_pcbxDestPort->currentText().toInt());
    if (ret > -1) {
        return QString("UDP发送数据成功: %1字节").arg(ret);
    } else {
        return QString("[Error]UDP发送数据失败: %1").arg(m_pRecv->errorString());
    }
}
