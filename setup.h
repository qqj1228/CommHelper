#ifndef SETUP_H
#define SETUP_H

#include <QObject>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

class Setup : public QObject
{
    Q_OBJECT
public:
    explicit Setup(QWidget *tab, QObject *parent = nullptr);
    void updateUI();

private:
    void initUI();
    void showColor(bool bSend);

signals:

public slots:
    void onSendClr();
    void onRecvClr();

public:
    QColor m_qclrSend;
    QColor m_qclrRecv;
    QString m_fontSize;
    int m_iHistory;
    QComboBox *m_pcbxTextCode = nullptr;
    int m_iRecvDelay;

protected:
    QWidget *m_pTab = nullptr;
    QLabel *m_plabSend = nullptr;
    QLabel *m_plabRecv = nullptr;
    QPushButton *m_pbtnSendClr = nullptr;
    QPushButton *m_pbtnRecvClr = nullptr;
    QLineEdit *m_pedtSize = nullptr;
    QLineEdit *m_pedtHistory = nullptr;
    QLineEdit *m_pedtRecvDelay = nullptr;
};

#endif // SETUP_H
