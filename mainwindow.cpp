#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utility.h"
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_pUi(new Ui::MainWindow),
    m_pLStatus(new QLabel),
    m_pLRecv(new QLabel),
    m_pLSend(new QLabel)
{
    m_pUi->setupUi(this);
    m_pUi->statusBar->addWidget(m_pLStatus, 5);
    m_pUi->statusBar->addWidget(m_pLRecv, 1);
    m_pUi->statusBar->addWidget(m_pLSend, 1);
    this->showStatus(tr("Ready"));
    this->showBytes();
    m_pMySerial = new SerialPort(m_pUi->tabSerial, this);
    m_pMyTCP = new TCPApp(m_pUi->tabTCP, this);
    m_pMyUDP = new UDPApp(m_pUi->tabUDP, this);
    m_pMySetup = new Setup(m_pUi->tabSetup, this);
    m_pMyConfig = new Config(this);
    m_pMyConfig->loadHistory(m_pUi->cboxSend, CFG_SEC_SENDED);
    m_pMyConfig->loadHistory(m_pUi->cboxFilter, CFG_SEC_FILTER);
    m_pMyConfig->loadHistory(m_pUi->cbxDestIP, CFG_SEC_DESTIP);
    m_pMyConfig->loadHistory(m_pUi->cbxDestPort, CFG_SEC_DESTPORT);
    m_pMyConfig->loadHistory(m_pUi->cbxRecvIP, CFG_SEC_RECVIP);
    m_pMyConfig->loadHistory(m_pUi->cbxRecvPort, CFG_SEC_RECVPORT);
    m_pMyConfig->loadHistory(m_pUi->cbxHostIP, CFG_SEC_HOSTIP);
    m_pMyConfig->loadHistory(m_pUi->cbxHostPort, CFG_SEC_HOSTPORT);
    m_pMyConfig->loadHistory(m_pUi->cbxListenIP, CFG_SEC_LISTENIP);
    m_pMyConfig->loadHistory(m_pUi->cbxListenPort, CFG_SEC_LISTENPORT);
    m_pMyConfig->loadSerialPort(m_pMySerial);
    m_pMyTCP->updateUI();
    m_pMyConfig->loadTCP(m_pMyTCP);
    m_pMyUDP->updateUI();
    m_pMyConfig->loadUDP(m_pMyUDP);
    m_pMyConfig->loadSetup(m_pMySetup);

    connect(m_pMySerial, SIGNAL(bytesSended(qint64, QString)), this, SLOT(onSended(qint64, QString)));
    connect(m_pMySerial, &SerialPort::serialPortClosed, [&](){
        m_pUi->btnOpen->setChecked(false);
    });
    connect(m_pMySerial, SIGNAL(hasRecved(QByteArray, QString, int)), this, SLOT(onRecved(QByteArray, QString, int)));
    connect(m_pMySerial, SIGNAL(errorOccurred(QString)), this, SLOT(onShowError(QString)));
    connect(m_pMyUDP, SIGNAL(hasRecved(QByteArray, QString, int)), this, SLOT(onRecved(QByteArray, QString, int)));
    connect(m_pMyUDP, SIGNAL(errorOccurred(QString)), this, SLOT(onShowError(QString)));
    connect(m_pMyUDP, SIGNAL(bytesSended(qint64, QString)), this, SLOT(onSended(qint64, QString)));
    connect(m_pMyTCP, SIGNAL(errorOccurred(QString)), this, SLOT(onShowError(QString)));
    connect(m_pMyTCP, SIGNAL(stateChanged(QString)), this, SLOT(onShowError(QString)));
    connect(m_pMyTCP, SIGNAL(connected(QString)), this, SLOT(onTCPConnected(QString)));
    connect(m_pMyTCP, SIGNAL(disconnected(QString)), this, SLOT(onTCPDisconnected(QString)));
    connect(m_pMyTCP, SIGNAL(hasRecved(QByteArray, QString, int)), this, SLOT(onRecved(QByteArray, QString, int)));
    connect(m_pMyTCP, SIGNAL(bytesSended(qint64, QString)), this, SLOT(onSended(qint64, QString)));
}

MainWindow::~MainWindow()
{
    delete m_pUi;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    //TODO: 在退出窗口之前，实现希望做的操作
    m_pMyConfig->saveSerialPort(m_pMySerial);
    m_pMyConfig->saveHistory(m_pUi->cboxSend, QString(CFG_SEC_SENDED));
    m_pMyConfig->saveHistory(m_pUi->cboxFilter, QString(CFG_SEC_FILTER));
    m_pMyConfig->saveHistory(m_pUi->cbxDestIP, QString(CFG_SEC_DESTIP));
    m_pMyConfig->saveHistory(m_pUi->cbxDestPort, QString(CFG_SEC_DESTPORT));
    m_pMyConfig->saveHistory(m_pUi->cbxRecvIP, QString(CFG_SEC_RECVIP));
    m_pMyConfig->saveHistory(m_pUi->cbxRecvPort, QString(CFG_SEC_RECVPORT));
    m_pMyConfig->saveHistory(m_pUi->cbxHostIP, QString(CFG_SEC_HOSTIP));
    m_pMyConfig->saveHistory(m_pUi->cbxHostPort, QString(CFG_SEC_HOSTPORT));
    m_pMyConfig->saveHistory(m_pUi->cbxListenIP, QString(CFG_SEC_LISTENIP));
    m_pMyConfig->saveHistory(m_pUi->cbxListenPort, QString(CFG_SEC_LISTENPORT));
    m_pMyConfig->saveTCP(m_pMyTCP);
    m_pMyConfig->saveUDP(m_pMyUDP);
    m_pMyConfig->saveSetup(m_pMySetup);
    event->accept();
}

void MainWindow::showStatus(QString message) {
    if (message.indexOf("Error") < 0) {
        m_pLStatus->setStyleSheet("color: black;");
    } else {
        m_pLStatus->setStyleSheet("color: red;");
    }
    if (message.size() > MAX_STATUS_SIZE) {
        message.resize(MAX_STATUS_SIZE);
    }
    m_pLStatus->setText(message);
}

void MainWindow::showBytes() {
    m_pLRecv->setText(tr("Recv all：%1 Bytes").arg(m_iRecved));
    m_pLSend->setText(tr("Send all：%1 Bytes").arg(m_iSended));
}

QString MainWindow::getDisplayMessage(const QByteArray &data, const QString &address, bool send, int enumTunnel) {
    QString message(QTime::currentTime().toString("HH:mm:ss "));
    QString qstrMessage = CommHelper::getDisplayString(data, m_pMySetup->m_pcbxTextCode->currentText());
    QString qstrHexMessage = CommHelper::getHexString(data);

    switch (enumTunnel) {
    case MainWindow::SerialTab:
        if (send) {
            message += tr("[Send to <SerialPort>\"%1\" - %2 Bytes]: %3").arg(address).arg(data.size()).arg(qstrMessage);
        } else {
            message += tr("[Recv from <SerialPort>\"%1\" - %2 Bytes]: %3").arg(address).arg(data.size()).arg(qstrMessage);
        }
        break;
    case MainWindow::TCPTab:
        if (send) {
            message += tr("[Send to <TCP>\"%1\" - %2 Bytes]: %3").arg(address).arg(data.size()).arg(qstrMessage);
        } else {
            message += tr("[Recv from <TCP>\"%1\" - %2 Bytes]: %3").arg(address).arg(data.size()).arg(qstrMessage);
        }
        break;
    case MainWindow::UDPTab:
        if (send) {
            message += tr("[Send to <UDP>\"%1\" - %2 Bytes]: %3").arg(address).arg(data.size()).arg(qstrMessage);
            this->addComboBoxItem(m_pUi->cbxDestIP);
            this->addComboBoxItem(m_pUi->cbxDestPort);
        } else {
            message += tr("[Recv from <UDP>\"%1\" - %2 Bytes]: %3").arg(address).arg(data.size()).arg(qstrMessage);
        }
        break;
    }

    message += tr("\n====== Hex: %1").arg(qstrHexMessage);
    if (m_pUi->btnFilter->isChecked()) {
        QString qstrFilterMessage = CommHelper::getFilterString(data, CommHelper::getFilterList(m_pUi->cboxFilter->currentText()));
        message += tr("\n++++++ Filter: %1").arg(qstrFilterMessage);
        QString str = m_pUi->cboxFilter->currentText().remove(QRegularExpression("[^\\d ,-]+"));
        m_pUi->cboxFilter->setCurrentText(str);
        this->addComboBoxItem(m_pUi->cboxFilter);
    }
    CommHelper::escapeHtml(message);
    if (send) {
        message.insert(0, QString("<p style=\"color:%1; font-size:%2pt;\">").arg(m_pMySetup->m_qclrSend.name()).arg(m_pMySetup->m_fontSize));
    } else {
        message.insert(0, QString("<p style=\"color:%1; font-size:%2pt;\">").arg(m_pMySetup->m_qclrRecv.name()).arg(m_pMySetup->m_fontSize));
    }
    message.append("</p>");
    return message;
}

void MainWindow::addComboBoxItem(QComboBox *cmb) {
    if (cmb->findText(cmb->currentText()) == -1) {
        cmb->addItem(cmb->currentText());
        if (cmb->count() > m_pMySetup->m_iHistory) {
            cmb->removeItem(0);
            cmb->setCurrentIndex(cmb->count() - 1);
        }
    }
}

// Qt自动默认命名槽函数的话无需手动connect，否则会触发两次
void MainWindow::on_btnOpen_clicked(bool checked)
{
    QString message;
    if (checked) {
        message = m_pMySerial->openSerialPort();
    } else {
        message = m_pMySerial->closeSerialPort();
    }
    this->showStatus(message);
    if (message.indexOf("Error") >= 0) {
         m_pUi->btnOpen->setChecked(false);
    }
}

void MainWindow::on_btnSend_clicked()
{
    QString message;
    try {
        m_send = CommHelper::convert2Raw(m_pUi->cboxSend->currentText(), m_pMySetup->m_pcbxTextCode->currentText());
        switch (m_pUi->tabWidget->currentIndex()) {
        case MainWindow::SerialTab:
            message = m_pMySerial->sendData(m_send);
            break;
        case MainWindow::TCPTab:
            message = m_pMyTCP->sendData(m_send);
            break;
        case MainWindow::UDPTab:
            message = m_pMyUDP->sendData(m_send);
            break;
        }
    } catch (QString &ex) {
        message = "[Error]" + ex;
    }
    this->showStatus(message);
}

void MainWindow::onSended(qint64 bytes, QString address) {
    QString message;

    switch (m_pUi->tabWidget->currentIndex()) {
    case MainWindow::SerialTab:
        message = this->getDisplayMessage(m_send, address, true, MainWindow::SerialTab);
        break;
    case MainWindow::TCPTab:
        message = this->getDisplayMessage(m_send, address, true, MainWindow::TCPTab);
        break;
    case MainWindow::UDPTab:
        message = this->getDisplayMessage(m_send, address, true, MainWindow::UDPTab);
        break;
    }

    m_pUi->textBrowser->append(message);
    m_iSended += bytes;
    this->showBytes();
    this->addComboBoxItem(m_pUi->cboxSend);
}

void MainWindow::on_btnClear_clicked()
{
    m_pUi->cboxSend->clear();
    m_pMyConfig->remove(QString(CFG_SEC_SENDED));
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case MainWindow::SerialTab:
        m_pMySerial->updatePort();
        break;
    case MainWindow::TCPTab:
        m_pMyTCP->updateUI();
        break;
    case MainWindow::UDPTab:
        m_pMyUDP->updateUI();
        break;
    case MainWindow::SetupTab:
        m_pMySetup->updateUI();
        break;
    default:
        break;
    }
}

void MainWindow::onRecved(QByteArray data, QString address, int enumTunnel) {
    QString message = this->getDisplayMessage(data, address, false, enumTunnel);
    m_pUi->textBrowser->append(message);
    m_iRecved += data.size();
    this->showBytes();
}

void MainWindow::on_btnClearText_clicked()
{
    m_pUi->textBrowser->clear();
}

void MainWindow::on_btnUDP_clicked(bool checked)
{
    QString message;
    if (checked) {
        message = this->m_pMyUDP->initUDP();
        m_pUi->cbxRecvIP->setEnabled(false);
        m_pUi->cbxRecvPort->setEnabled(false);
    } else {
        message = this->m_pMyUDP->closeUDP();
        m_pUi->cbxRecvIP->setEnabled(true);
        m_pUi->cbxRecvPort->setEnabled(true);
        m_pUi->listUDPConn->clear();
    }
    this->showStatus(message);
    if (message.indexOf("Error") >= 0) {
        // error occurred
        m_pUi->btnOpen->setChecked(false);
        m_pUi->cbxRecvIP->setEnabled(true);
        m_pUi->cbxRecvPort->setEnabled(true);
    } else {
        // success
        this->addComboBoxItem(m_pUi->cbxRecvIP);
        this->addComboBoxItem(m_pUi->cbxRecvPort);
    }
}

void MainWindow::onShowError(QString qstrError) {
    this->showStatus(qstrError);
}

void MainWindow::on_checkUDPConn_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        m_pUi->cbxDestIP->setEnabled(false);
        m_pUi->cbxDestPort->setEnabled(false);
    } else {
        m_pUi->cbxDestIP->setEnabled(true);
        m_pUi->cbxDestPort->setEnabled(true);
    }
}

void MainWindow::on_btnTCPClient_clicked(bool checked)
{
    if (checked) {
        m_pMyTCP->initClient();
    } else {
        m_pMyTCP->closeClient();
    }
}

void MainWindow::onTCPConnected(QString qstrSender) {
    if (qstrSender == "TCPClient") {
        this->addComboBoxItem(m_pUi->cbxHostIP);
        this->addComboBoxItem(m_pUi->cbxHostPort);
        m_pUi->cbxHostIP->setEnabled(false);
        m_pUi->cbxHostPort->setEnabled(false);
        m_pUi->btnTCPServer->setEnabled(false);
    }
}

void MainWindow::onTCPDisconnected(QString qstrSender) {
    if (qstrSender == "TCPClient") {
        m_pUi->btnTCPClient->setChecked(false);
        m_pUi->cbxHostIP->setEnabled(true);
        m_pUi->cbxHostPort->setEnabled(true);
        m_pUi->btnTCPServer->setEnabled(true);
    }
}

void MainWindow::on_btnTCPServer_clicked(bool checked)
{
    QString message;
    if (checked) {
        message = m_pMyTCP->initServer();
        m_pUi->cbxListenIP->setEnabled(false);
        m_pUi->cbxListenPort->setEnabled(false);
        m_pUi->btnTCPClient->setEnabled(false);
    } else {
        message = m_pMyTCP->closeServer();
        m_pUi->cbxListenIP->setEnabled(true);
        m_pUi->cbxListenPort->setEnabled(true);
        m_pUi->btnTCPClient->setEnabled(true);
    }
    this->showStatus(message);
    if (message.indexOf("Error") >= 0) {
        // error occurred
        m_pUi->btnTCPServer->setChecked(false);
        m_pUi->cbxListenIP->setEnabled(true);
        m_pUi->cbxListenPort->setEnabled(true);
        m_pUi->btnTCPClient->setEnabled(true);
    } else {
        // success
        this->addComboBoxItem(m_pUi->cbxListenIP);
        this->addComboBoxItem(m_pUi->cbxListenPort);
    }
}

void MainWindow::on_btnDisconn_clicked()
{
    this->showStatus(m_pMyTCP->listDisconnect());
}
