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

QString Config::getQString(const QString& section, const QString& key, const QString& def)
{
    return m_pConfig->value(QString("%1/%2").arg(section).arg(key), def).toString();
}

void Config::setSended(const QComboBox* cmb, const QString& val) {
    int index = cmb->count() - 1;
    this->setQString(QString(CFG_SEC_SENDED), QString(CFG_VAL_SENDED).append("%1").arg(index), val);
}

void Config::saveSended(const QComboBox* cmb) {
    QString val;
    int index = cmb->count();
    m_pConfig->beginGroup(QString(CFG_SEC_SENDED));
    for (int i = 0; i < index; ++i) {
        val = cmb->itemText(i);
        m_pConfig->setValue(QString(CFG_VAL_SENDED).append("%1").arg(i), val);
    }
    m_pConfig->endGroup();
}

void Config::loadSended(QComboBox *cmb) {
    QString val;
    cmb->clear();
    m_pConfig->beginGroup(QString(CFG_SEC_SENDED));
    QStringList keys = m_pConfig->childKeys();
    int count = keys.size();
    for (int i = 0; i < count; ++i) {
        val = m_pConfig->value(QString(CFG_VAL_SENDED).append("%1").arg(i)).toString();
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
