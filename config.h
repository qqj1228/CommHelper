#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QSettings>
#include <QComboBox>
#include "serialport.h"
#include "setup.h"

#define CFG_APP_NAME "CommHelper"
#define CFG_SEC_SENDED "Sended"
#define CFG_KEY_SENDED "item"
#define CFG_SEC_FILTER "Filter"
#define CFG_KEY_FILTER "item"
#define CFG_SEC_SERIAL "SerialPort"
#define CFG_KEY_PORT "Port"
#define CFG_KEY_BAUD "Baud"
#define CFG_KEY_BIT "Bit"
#define CFG_KEY_PARITY "Parity"
#define CFG_KEY_STOP "Stop"
#define CFG_KEY_FLOW "Flow"
#define CFG_SEC_SETUP "Setup"
#define CFG_KEY_SENDCLR "SendColor"
#define CFG_KEY_RECVCLR "RecvColor"
#define CFG_KEY_FONTSIZE "FontSize"

class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = nullptr);
    ~Config();

    void setQString(const QString& section, const QString& key, const QString& val);
    void setInt(const QString& section, const QString& key, const int val);
    QString getQString(const QString& section, const QString& key, const QString& def);
    void setSended(const QComboBox* cmb, const QString& val);
    void setFilter(const QComboBox* cmb, const QString& val);
    void saveSended(const QComboBox* cmb);
    void loadSended(QComboBox* cmb);
    void loadFilter(QComboBox* cmb);
    void remove(const QString &topKey);
    void saveSerialPort(const SerialPort *pMySerialPort);
    void loadSerialPort(SerialPort *pMySerialPort);
    void saveSetup(const Setup *pMySetup);
    void loadSetup(Setup *pMySetup);

signals:

public slots:

private:
    QSettings *m_pConfig = nullptr;
};

#endif // CONFIG_H
