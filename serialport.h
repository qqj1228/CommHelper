#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QComboBox>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QWidget *tab, QObject *parent = nullptr);
    ~SerialPort();
    QString openSerialPort();
    QString closeSerialPort();
    QString sendData(const QByteArray &data);
    QString recvData();
    QString getPort();
    void updatePort();

signals:
    void bytesSended(qint64 bytes);
    void serialPortClosed();
    void hasRecved(QByteArray data, int enumTunnel);

public slots:
    void handleError(QSerialPort::SerialPortError error);

private:
    void initComboBox();
    void initParameters();

protected:
    QWidget *m_pParent = nullptr;
    QWidget *m_pTab = nullptr;
    QSerialPort *m_pSerial = nullptr;
public:
    QComboBox *m_pCboxPort = nullptr;
    QComboBox *m_pCboxBaud = nullptr;
    QComboBox *m_pCboxBit = nullptr;
    QComboBox *m_pCboxParity = nullptr;
    QComboBox *m_pCboxStop = nullptr;
    QComboBox *m_pCboxFlow = nullptr;
};

#endif // SERIALPORT_H
