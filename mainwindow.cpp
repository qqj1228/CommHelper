#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utility.h"
#include <QTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_pUi(new Ui::MainWindow),
    m_pLStatus(new QLabel),
    m_pLRecv(new QLabel),
    m_pLSend(new QLabel)
{
    m_pUi->setupUi(this);
//    m_pLStatus->setFrameShape(QFrame::StyledPanel);
//    m_pLRecv->setFrameShape(QFrame::StyledPanel);
//    m_pLSend->setFrameShape(QFrame::StyledPanel);
    m_pUi->statusBar->addWidget(m_pLStatus, 5);
    m_pUi->statusBar->addWidget(m_pLRecv, 1);
    m_pUi->statusBar->addWidget(m_pLSend, 1);
    this->showStatus(QString("准备就绪"));
    this->showBytes();
    m_pMySerial = new SerialPort(m_pUi->tabSerial, this);
    m_pMyConfig = new Config(this);
    m_pMyConfig->loadSended(m_pUi->cboxSend);

    connect(m_pMySerial, SIGNAL(bytesSended(qint64)), this, SLOT(onSended(qint64)));
    connect(m_pMySerial, &SerialPort::serialPortClosed, [=](){
        m_pUi->btnOpen->setChecked(false);
    });
    connect(m_pMySerial, SIGNAL(hasRecved(QByteArray)), this, SLOT(onRecved(QByteArray)));
}

MainWindow::~MainWindow()
{
    delete m_pUi;
    delete m_pMySerial;
    delete m_pMyConfig;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    //TODO: 在退出窗口之前，实现希望做的操作
    m_pMyConfig->saveSended(m_pUi->cboxSend);
    event->accept();
}

void MainWindow::showStatus(const QString &message) {
    if (message.indexOf("Error") < 0) {
        m_pLStatus->setStyleSheet("color: black;");
    } else {
        m_pLStatus->setStyleSheet("color: red;");
    }
    m_pLStatus->setText(message);
}

void MainWindow::showBytes() {
    m_pLRecv->setText(QString("总接收：%1 字节").arg(m_iRecved));
    m_pLSend->setText(QString("总发送：%1 字节").arg(m_iSended));
}

QString MainWindow::getDisplayMessage(QByteArray &data, bool send) {
    QString message(QTime::currentTime().toString("HH:mm:ss "));
    QString qstrMessage = CommHelper::getDisplayString(data);
    QString qstrHexMessage = CommHelper::getHexString(data);
    if (send) {
        message += QString("[Send to \"%1\" - Bytes: %2]: %3\n").arg(m_pMySerial->getPort()).arg(data.size()).arg(qstrMessage);
    } else {
        message += QString("[Recv from \"%1\" - Bytes: %2]: %3\n").arg(m_pMySerial->getPort()).arg(data.size()).arg(qstrMessage);
    }
    message += QString("====== Hex: %1").arg(qstrHexMessage);
    return message;
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
//        m_pMySerial->writeData(m_pUi->cboxSend->currentText().toLocal8Bit());
        m_send = CommHelper::convert2Raw(m_pUi->cboxSend->currentText());
        message = m_pMySerial->sendData(m_send);
    } catch (QString &ex) {
        message = "[Error]" + ex;
    }
    this->showStatus(message);
}

void MainWindow::onSended(qint64 bytes) {
    QString message = this->getDisplayMessage(m_send, true);
    m_pUi->textBrowser->append(message);
    m_iSended += bytes;
    this->showBytes();
    if (!hasSended()) {
        m_pUi->cboxSend->addItem(m_pUi->cboxSend->currentText());
        m_pMyConfig->setSended(m_pUi->cboxSend, m_pUi->cboxSend->currentText());
    }
}

bool MainWindow::hasSended() {
    int count = m_pUi->cboxSend->count();
    for (int i = 0; i < count; ++i) {
        if (m_pUi->cboxSend->itemText(i) == m_pUi->cboxSend->currentText()) {
            return true;
        }
    }
    return false;
}

void MainWindow::on_btnClear_clicked()
{
    m_pUi->cboxSend->clear();
    m_pMyConfig->remove(QString(CFG_SEC_SENDED));
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 0:
        // 串口
        m_pMySerial->updatePort();
        break;
    case 1:
        // TCP
        break;
    case 2:
        // UDP
        break;
    default:
        break;
    }
}

void MainWindow::onRecved(QByteArray data) {
    QString message = this->getDisplayMessage(data, false);
    m_pUi->textBrowser->append(message);
    m_iRecved += data.size();
    this->showBytes();
}
