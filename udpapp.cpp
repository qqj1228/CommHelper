#include "udpapp.h"
#include "mainwindow.h"
#include "config.h"
#include <QNetworkInterface>
#include <QNetworkDatagram>

UDPApp::UDPApp(QWidget *tab, QObject *parent) : QObject(parent),
    m_pTab(tab)
{
    initUI();
}

void UDPApp::initUI() {
    this->m_pcbxDestIP = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxDestIP"));
    this->m_pcbxDestPort = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxDestPort"));
    this->m_pcbxRecvIP = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxRecvIP"));
    this->m_pcbxRecvPort = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxRecvPort"));
    this->m_plistUDPConn = m_pTab->findChild<QListWidget*>(QStringLiteral("listUDPConn"));
    this->m_pcheckConn = m_pTab->findChild<QCheckBox*>(QStringLiteral("checkUDPConn"));

    this->m_pcbxDestIP->setValidator(new QRegExpValidator(QRegExp("[0-9.]+"), this));
    this->m_pcbxDestPort->setValidator(new QIntValidator(1, 65535, this));
    this->m_pcbxRecvIP->setValidator(new QRegExpValidator(QRegExp("[0-9.]+"), this));
    this->m_pcbxRecvPort->setValidator(new QIntValidator(1, 65535, this));
}

QString UDPApp::initUDP() {
    m_pUDP = new QUdpSocket(this);
    bool bResult = m_pUDP->bind(QHostAddress(m_pcbxRecvIP->currentText()), m_pcbxRecvPort->currentText().toInt());
    if (bResult) {
        connect(m_pUDP, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
        connect(m_pUDP, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
        connect(m_pUDP, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesSended(qint64)));
        return tr("Open UDP on \"%1:%2\"").arg(m_pUDP->localAddress().toString()).arg(m_pUDP->localPort());
    } else {
        return tr("[Error]Can not open UDP: %1").arg(m_pUDP->errorString());
    }
}

QString UDPApp::closeUDP() {
    if (m_pUDP != nullptr) {
        m_pUDP->close();
        m_pUDP->deleteLater();
        m_pUDP = nullptr;
    }
    return tr("Closed UDP");
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
    while (m_pUDP->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_pUDP->receiveDatagram();
        if (datagram.isValid()) {
            QString address = QString("%1:%2").arg(datagram.senderAddress().toString()).arg(datagram.senderPort());
            if (m_plistUDPConn->findItems(address, Qt::MatchFixedString).size() == 0) {
                m_plistUDPConn->addItem(address);
            }
            emit hasRecved(datagram.data(), address, MainWindow::UDPTab);
        }
    }
}

void UDPApp::onError(QAbstractSocket::SocketError socketError) {
    emit errorOccurred(tr("[Error]code: %1, description: %2").arg(socketError).arg(m_pUDP->errorString()));
}

QString UDPApp::sendData(const QByteArray &data) {
    QString address, message;
    qint64 ret = 0;
    if (m_pUDP != nullptr) {
        // 调用m_pUDP->writeDatagram的过程中已经触发了bytesWritten(qint64)信号，故先入队列
        m_sendData.enqueue(data);
        if (m_pcheckConn->isChecked()) {
            QListWidgetItem *pitem = m_plistUDPConn->currentItem();
            if (pitem == nullptr) {
                message = tr("[Error]No selected connection");
            }
            address = pitem->text();
            int pos = address.indexOf(':');
            ret = m_pUDP->writeDatagram(data, QHostAddress(address.left(pos)), address.mid(pos + 1).toInt());
        } else {
            ret = m_pUDP->writeDatagram(data, QHostAddress(m_pcbxDestIP->currentText()), m_pcbxDestPort->currentText().toInt());
        }
        if (ret > -1) {
            message = tr("UDP Send Data Successfully: %1 Bytes").arg(ret);
        } else {
            if (!m_sendData.isEmpty()) {
                m_sendData.pop_back();
            }
            message = tr("[Error]UDP Failed to Send Data: %1").arg(m_pUDP->errorString());
        }
    } else {
        message = tr("[Error]UDP not open");
    }
    return message;
}

void UDPApp::onBytesSended(qint64 bytes) {
    QString address;
    if (m_pcheckConn->isChecked()) {
        address = m_plistUDPConn->currentItem()->text();
    } else {
        address = QString("%1:%2").arg(m_pcbxDestIP->currentText()).arg(m_pcbxDestPort->currentText());
    }
    emit bytesSended(bytes, address);
}
