#include "setup.h"
#include <QColorDialog>
#include <QIntValidator>

Setup::Setup(QWidget *tab, QObject *parent) : QObject(parent),
    m_pTab(tab)
{
    initUI();

    connect(m_pbtnSendClr, SIGNAL(clicked(bool)), this, SLOT(onSendClr()));
    connect(m_pbtnRecvClr, SIGNAL(clicked(bool)), this, SLOT(onRecvClr()));
    connect(m_pedtSize, &QLineEdit::editingFinished, [=](){
        this->m_fontSize = m_pedtSize->text();
    });
    connect(m_pedtHistory, &QLineEdit::editingFinished, [=](){
        this->m_iHistory = m_pedtHistory->text().toInt();
    });
}

void Setup::initUI() {
    this->m_plabSend = m_pTab->findChild<QLabel*>(QStringLiteral("labSend"));
    this->m_plabRecv = m_pTab->findChild<QLabel*>(QStringLiteral("labRecv"));
    this->m_pbtnSendClr = m_pTab->findChild<QPushButton*>(QStringLiteral("btnSendColor"));
    this->m_pbtnRecvClr = m_pTab->findChild<QPushButton*>(QStringLiteral("btnRecvColor"));
    this->m_pedtSize = m_pTab->findChild<QLineEdit*>(QStringLiteral("edtFontSize"));
    this->m_pedtHistory = m_pTab->findChild<QLineEdit*>(QStringLiteral("edtHistory"));
    this->m_pcbxTextCode = m_pTab->findChild<QComboBox*>(QStringLiteral("cbxTextCode"));

    this->m_pedtSize->setValidator(new QIntValidator(0, 20, this));
    this->m_pedtHistory->setValidator(new QIntValidator(0, 100, this));
    this->m_pcbxTextCode->addItem(QStringLiteral("ANSI"));
    this->m_pcbxTextCode->addItem(QStringLiteral("UTF-8"));
}

void Setup::updateUI() {
    m_plabSend->setStyleSheet(QString("color:%1").arg(m_qclrSend.name()));
    m_plabRecv->setStyleSheet(QString("color:%1").arg(m_qclrRecv.name()));
    m_pedtSize->setText(m_fontSize);
    m_pedtHistory->setText(QString("%1").arg(m_iHistory));
}

void Setup::showColor(bool bSend) {
    QColor *pclrSet;
    QLabel *plab;
    if (bSend) {
        pclrSet = &m_qclrSend;
        plab = m_plabSend;
    } else {
        pclrSet = &m_qclrRecv;
        plab = m_plabRecv;
    }
    QColor clrRet = QColorDialog::getColor(*pclrSet, m_pTab);
    if (clrRet.isValid()) {
        *pclrSet = clrRet;
        plab->setStyleSheet(QString("color:%1").arg(pclrSet->name()));
    }
}

void Setup::onSendClr() {
    showColor(true);
}

void Setup::onRecvClr() {
    showColor(false);
}
