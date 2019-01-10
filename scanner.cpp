#include "scanner.h"
#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QCryptographicHash>

scanner::scanner(const QString &dir_name) {
    dir = dir_name;
}

void scanner::stop() {

}

void scanner::run() {
    data.clear();
    sha256.clear();
    get_data(dir);
    emit done(data, sha256, dir);
    emit finished();
}

void scanner::get_data(const QString &dir) {

    QDir d(dir);

    QFileInfoList list = d.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for (QFileInfo file_info : list)
    {
        if (file_info.isDir()) {
            if (QDir(file_info.absoluteFilePath()).isReadable())
                get_data(file_info.absoluteFilePath());
        } else {
            QCryptographicHash hs(QCryptographicHash::Algorithm::Sha256);
            QFile file(file_info.absoluteFilePath());

            if(!file.open(QIODevice::ReadOnly)) {
                //file.setErrorString(QString("You're Fucking mothers son!"));
                //QMessageBox::information(0, "error", "You're fucking mother's son!");
                continue;
            }

            if (!hs.addData(&file)) {
                file.close();
                continue;
            }

            file.close();

            QString SHA256 = QString(hs.result().toHex());

            sha256[file_info.absoluteFilePath()] = SHA256;
            data[SHA256].append(file_info.absoluteFilePath());
        }
    }
}

QMap<QString, QString> scanner::get_sha256() {
    return sha256;
}
QMap<QString, QVector<QString> > scanner::get_map_data() {
    return data;
}