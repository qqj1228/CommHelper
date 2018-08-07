#ifndef UTILITY_H
#define UTILITY_H

#include <QtGlobal>
#include <QString>
#include <QRegularExpression>
#include <QDebug>

namespace CommHelper {

uchar str2Byte(const char *str) {
    char high = str[0];
    char low = str[1];

    if (high >= '0' && high <= '9') {
        high = high - '0';
    } else if (high >= 'A' && high <= 'F') {
        high = high - 'A' + 10;
    } else if (high >= 'a' && high <= 'f') {
        high = high - 'a' + 10;
    } else {
        throw QString("无法转换为十六进制数据！");
    }

    if (low >= '0' && low <= '9') {
        low = low - '0';
    } else if (low >= 'A' && low <= 'F') {
        low = low - 'A' + 10;
    } else if (low >= 'a' && low <= 'f') {
        low = low - 'a' + 10;
    } else {
        throw QString("无法转换为十六进制数据！");
    }

    return high << 4 | low;
}

QStringList doSplit(QString &str) {
    QStringList list;
    QString item;
    int itemLen = 0;
    list = str.split(QRegularExpression("[ ,]+"), QString::SkipEmptyParts);
    int listLen = list.size();
    for (int i = 0; i < listLen; ++i) {
        item = list.at(i);
        itemLen = list.at(i).size();
        if (itemLen < 2) {
            list[i] = "0" + item;
        } else if (itemLen > 2) {
            list.removeAt(i);
            for (int j = 0; itemLen - j * 2 > 0; ++j) {
                list.insert(i + j, item.mid(j * 2, 2));
            }
            if (itemLen % 2 > 0) {
                list[i + itemLen / 2] = "0" + list[i + itemLen / 2];
            }
        }
    }
    return list;
}

QByteArray convert2Raw(const QString &text, const QChar &start = '[', const QChar &end = ']') {
    QByteArray ret;
    QString strHex;
    QStringList strList;
    int pos1 = 0;
    int pos2 = -1;

    // escape "\[" and "\]"
    QString data = text;
    data.replace("\\[", "[5B]");
    data.replace("\\]", "[5D]");

    while (pos1 > -1) {
        pos1 = data.indexOf(start, pos1);
        if (pos1 > -1) {
            ret += data.mid(pos2 + 1, pos1 - pos2 - 1).toLocal8Bit();
            pos2 = data.indexOf(end, pos1 + 1);
            if (pos2 > -1) {
                strHex = data.mid(pos1 + 1, pos2 - pos1 - 1);
                strList = doSplit(strHex);
                for (int i = 0; i < strList.size(); ++i) {
                    ret += str2Byte(strList.at(i).toLocal8Bit().constData());
                }
                pos1 = pos2 + 1;
            } else {
                ret += data.mid(pos1 + 1);
                break;
            }
        } else {
            ret += data.mid(pos2 + 1);
        }
    }
    return ret;
}

QString getDisplayString(QByteArray data) {
    for (int i = 0; i < data.size(); ++i) {
        if (data.at(i) < 0x20 || data.at(i) > 0x7E) {
            data[i] = 0x5E;
            data.insert(i, '\\');
        }
    }
    QString ret = QString::fromLocal8Bit(data);
    return ret;
}

QString getHexString(QByteArray data) {
    QString ret = "[";
    QStringList strList;
    for (ushort item : data) {
        item = item & 0x00ff;
        strList.append(QString("%1").arg(item, 2, 16, QChar('0')));
    }
    ret += strList.join(" ");
    ret += "]";
    return ret;
}

QString getFilterString(QByteArray data, QStringList filter) {
    QString ret;
    for (QString &item : filter) {
        if (item.contains('-')) {
            if (item.startsWith('-')) {
                ret += getHexString(data.left(item.remove('-').toInt() + 1)) + " ";
            } else if (item.endsWith('-')) {
                ret += getHexString(data.mid(item.remove('-').toInt())) + " ";
            } else {
                QStringList l = item.split('-', QString::SkipEmptyParts);
                int first = l.first().toInt();
                int last = l.last().toInt();
                ret += getHexString(data.mid(first, last - first + 1)) + " ";
            }
        } else {
            ret += getHexString(data.mid(item.toInt(), 1)) + " ";
        }
    }
    ret.remove(ret.size() - 1, 1);
    return ret;
}

QStringList getFilterList(const QString &text) {
    QStringList filter;
    QStringList result;
    QString str;
    filter = text.split(QRegularExpression("[ ,]+"), QString::SkipEmptyParts);
    for (QString &item : filter) {
        str = item.remove(QRegularExpression("[^\\d-]+"));
        if (str != "") {
            result << str;
        }
    }
    return result;
}

void escapeHtml(QString &str) {
    // 注意这几行代码的顺序不能乱，否则会造成多次替换
    str.replace("&", "&amp;");
    str.replace(">", "&gt;");
    str.replace("<", "&lt;");
    str.replace("\"", "&quot;");
    str.replace("\'", "&#39;");
    str.replace(" ", "&nbsp;");
    str.replace("\n", "<br>");
    str.replace("\r", "<br>");
}

}

#endif // UTILITY_H
