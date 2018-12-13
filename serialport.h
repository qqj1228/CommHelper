#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QComboBox>
#include "setup.h"

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QWidget *tab, Setup *setup, QObject *parent = nullptr);
    ~SerialPort();
    QString openSerialPort();
    QString closeSerialPort();
    QString sendData(const QByteArray &data);
    void recvData();
    QString timerUpdate();
    void updatePort();

signals:
    void bytesSended(qint64 bytes, QString address);
    void serialPortClosed();
    void hasRecved(QByteArray data, QString address, int enumTunnel);
    void errorOccurred(QString errorStr);

public slots:
    void handleError(QSerialPort::SerialPortError error);
    void onBytesSended(qint64 bytes);

private:
    void initComboBox();
    void initParameters();
    void setupSerial();

protected:
    QWidget *m_pParent = nullptr;
    Setup *m_pSetup = nullptr;
    QWidget *m_pTab = nullptr;
    QSerialPort *m_pSerial = nullptr;
    QTimer *m_pTimer = nullptr;
    QByteArray m_data;

public:
    QComboBox *m_pCboxPort = nullptr;
    QComboBox *m_pCboxBaud = nullptr;
    QComboBox *m_pCboxBit = nullptr;
    QComboBox *m_pCboxParity = nullptr;
    QComboBox *m_pCboxStop = nullptr;
    QComboBox *m_pCboxFlow = nullptr;
};

#endif // SERIALPORT_H
