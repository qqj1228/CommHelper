#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include "serialport.h"
#include "config.h"

class QLabel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_btnOpen_clicked(bool checked);
    void on_btnSend_clicked();
    void onSended(qint64 bytes);
    void on_btnClear_clicked();
    void on_tabWidget_currentChanged(int index);
    void onRecved(QByteArray data);

private:
    void showStatus(const QString &message);
    void showBytes();
    QString getDisplayMessage(QByteArray &data, bool send);
    bool hasSended();

    Ui::MainWindow *m_pUi = nullptr;
    QLabel *m_pLStatus = nullptr;
    QLabel *m_pLRecv = nullptr;
    QLabel *m_pLSend = nullptr;
    SerialPort *m_pMySerial = nullptr;
    Config *m_pMyConfig = nullptr;
    QByteArray m_send;
    QString m_qstrMessage;
    QString m_qstrHexMessage;
    qint64 m_iRecved = 0;
    qint64 m_iSended = 0;
};

#endif // MAINWINDOW_H
