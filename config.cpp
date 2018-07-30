#include "config.h"
#include <QFileInfo>
#include <QDir>

Config::Config(QObject *parent) : QObject(parent)
{
    QString path(QDir::currentPath());
    if (!QFileInfo(path).isWritable() || path == QDir::homePath ())
    {
        QDir dir(QDir::home());
        dir.mkdir("." CFG_APP_NAME);
        if (dir.cd("." CFG_APP_NAME))
            path = dir.absolutePath();
    }
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, path);
    m_pConfig = new QSettings(QSettings::IniFormat, QSettings::UserScope, CFG_APP_NAME);
}

Config::~Config() {
    delete m_pConfig;
}

void Config::setQString(const QString& section, const QString& key, const QString& val)
{
    m_pConfig->setValue(QString("%1/%2").arg(section).arg(key), val);
}

void Config::setInt(const QString& section, const QString& key, const int val)
{
    m_pConfig->setValue(QString("%1/%2").arg(section).arg(key), val);
}

QString Config::getQString(const QString& section, const QString& key, const QString& def)
{
    return m_pConfig->value(QString("%1/%2").arg(section).arg(key), def).toString();
}

void Config::setSended(const QComboBox* cmb, const QString& val) {
    int index = cmb->count() - 1;
    this->setQString(CFG_SEC_SENDED, QString(CFG_KEY_SENDED).append("%1").arg(index), val);
}

void Config::setFilter(const QComboBox* cmb, const QString& val) {
    int index = cmb->count() - 1;
    this->setQString(CFG_SEC_FILTER, QString(CFG_KEY_FILTER).append("%1").arg(index), val);
}

void Config::saveSended(const QComboBox* cmb) {
    QString val;
    int index = cmb->count();
    m_pConfig->beginGroup(CFG_SEC_SENDED);
    for (int i = 0; i < index; ++i) {
        val = cmb->itemText(i);
        m_pConfig->setValue(QString(CFG_KEY_SENDED).append("%1").arg(i), val);
    }
    m_pConfig->endGroup();
}

void Config::loadSended(QComboBox *cmb) {
    QString val;
    cmb->clear();
    m_pConfig->beginGroup(CFG_SEC_SENDED);
    QStringList keys = m_pConfig->childKeys();
    int count = keys.size();
    for (int i = 0; i < count; ++i) {
        val = m_pConfig->value(QString(CFG_KEY_SENDED).append("%1").arg(i)).toString();
        if (val != "") {
            cmb->addItem(val);
        }
    }
    m_pConfig->endGroup();
    cmb->setCurrentIndex(-1);
}

void Config::loadFilter(QComboBox *cmb) {
    QString val;
    cmb->clear();
    m_pConfig->beginGroup(CFG_SEC_FILTER);
    QStringList keys = m_pConfig->childKeys();
    int count = keys.size();
    for (int i = 0; i < count; ++i) {
        val = m_pConfig->value(QString(CFG_KEY_FILTER).append("%1").arg(i)).toString();
        if (val != "") {
            cmb->addItem(val);
        }
    }
    m_pConfig->endGroup();
    cmb->setCurrentIndex(-1);
}

void Config::remove(const QString &topKey) {
    m_pConfig->remove(topKey);
}

void Config::saveSerialPort(const SerialPort *pMySerialPort) {
    m_pConfig->beginGroup(CFG_SEC_SERIAL);
    m_pConfig->setValue(CFG_KEY_PORT, pMySerialPort->m_pCboxPort->currentIndex());
    m_pConfig->setValue(CFG_KEY_BAUD, pMySerialPort->m_pCboxBaud->currentText());
    m_pConfig->setValue(CFG_KEY_BIT, pMySerialPort->m_pCboxBit->currentIndex());
    m_pConfig->setValue(CFG_KEY_PARITY, pMySerialPort->m_pCboxParity->currentIndex());
    m_pConfig->setValue(CFG_KEY_STOP, pMySerialPort->m_pCboxStop->currentIndex());
    m_pConfig->setValue(CFG_KEY_FLOW, pMySerialPort->m_pCboxFlow->currentIndex());
    m_pConfig->endGroup();
}

void Config::loadSerialPort(SerialPort *pMySerialPort) {
    m_pConfig->beginGroup(CFG_SEC_SERIAL);
    pMySerialPort->m_pCboxPort->setCurrentIndex(m_pConfig->value(CFG_KEY_PORT, -1).toInt());
    pMySerialPort->m_pCboxBaud->setCurrentText(m_pConfig->value(CFG_KEY_BAUD, "").toString());
    pMySerialPort->m_pCboxBit->setCurrentIndex(m_pConfig->value(CFG_KEY_BIT, -1).toInt());
    pMySerialPort->m_pCboxParity->setCurrentIndex(m_pConfig->value(CFG_KEY_PARITY, -1).toInt());
    pMySerialPort->m_pCboxStop->setCurrentIndex(m_pConfig->value(CFG_KEY_STOP, -1).toInt());
    pMySerialPort->m_pCboxFlow->setCurrentIndex(m_pConfig->value(CFG_KEY_FLOW, -1).toInt());
    m_pConfig->endGroup();
}

void Config::saveSetup(const Setup *pMySetup) {
    m_pConfig->beginGroup(CFG_SEC_SETUP);
    m_pConfig->setValue(CFG_KEY_SENDCLR, pMySetup->m_qclrSend.name());
    m_pConfig->setValue(CFG_KEY_RECVCLR, pMySetup->m_qclrRecv.name());
    m_pConfig->setValue(CFG_KEY_FONTSIZE, pMySetup->m_fontSize);
    m_pConfig->endGroup();
}

void Config::loadSetup(Setup *pMySetup) {
    m_pConfig->beginGroup(CFG_SEC_SETUP);
    pMySetup->m_qclrSend = m_pConfig->value(CFG_KEY_SENDCLR, "blue").value<QColor>();
    pMySetup->m_qclrRecv = m_pConfig->value(CFG_KEY_RECVCLR, "green").value<QColor>();
    pMySetup->m_fontSize = m_pConfig->value(CFG_KEY_FONTSIZE, "10").toString();
    m_pConfig->endGroup();
}
