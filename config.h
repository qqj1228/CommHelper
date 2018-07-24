#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QSettings>
#include <QComboBox>

#define CFG_APP_NAME "CommHelper"
#define CFG_SEC_SENDED "Sended"
#define CFG_VAL_SENDED "item"

class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = nullptr);
    ~Config();

    void setQString(const QString& section, const QString& key, const QString& val);
    QString getQString(const QString& section, const QString& key, const QString& def);
    void setSended(const QComboBox* cmb, const QString& val);
    void saveSended(const QComboBox* cmb);
    void loadSended(QComboBox* cmb);
    void remove(const QString &topKey);

signals:

public slots:

private:
    QSettings *m_pConfig = nullptr;
};

#endif // CONFIG_H
