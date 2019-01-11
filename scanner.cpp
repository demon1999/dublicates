#include "scanner.h"
#include <QString>
//#include <QDebug>
#include <iostream>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QCryptographicHash>

scanner::scanner(const QString &dir_name) {
    dir = dir_name;
    aborted_flag = false;
    number_of_files = counter = 0;
    now_percentage = 0;
}

void scanner::run() {
    data.clear();
    get_data(dir, false);
    was.clear();
    if (number_of_files == 0) {
        now_percentage = 100;
        emit percentage(100);
    }
    was.clear();
    if (aborted_flag == false)
        get_data(dir, true);
    if (aborted_flag == false)
        emit done(data, dir);
    emit finished();
}

void scanner::get_data(const QString &dir, bool load_files) {
    if (aborted_flag) return;
    if (was[dir]) return;
    was[dir] = true;
    QDir d(dir);

    QFileInfoList list = d.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for (QFileInfo file_info : list)
    {
        if (aborted_flag) return;
        if (file_info.isSymLink()) {
            get_data(file_info.absoluteFilePath(), load_files);
        } else
        if (file_info.isDir()) {
            if (QDir(file_info.absoluteFilePath()).isReadable())
                get_data(file_info.absoluteFilePath(), load_files);
        } else {
            if (load_files) {
                QCryptographicHash hs(QCryptographicHash::Algorithm::Sha256);
                QFile file(file_info.absoluteFilePath());
                counter++;
                while (now_percentage < 100 && (now_percentage + 1) * number_of_files <= counter * 100) {
                    now_percentage++;
                    emit percentage(now_percentage);
                }
                if(!file.open(QIODevice::ReadOnly)) {
                    //ignore bad files
                    continue;
                }

                if (!hs.addData(&file)) {
                    file.close();
                    continue;
                }

                file.close();

                QString SHA256 = QString(hs.result().toHex());

                data[SHA256].append(file_info.absoluteFilePath());
            } else {
                number_of_files++;
            }
        }
    }
}

QMap<QString, QVector<QString> > scanner::get_map_data() {
    return data;
}

void scanner::set_flag() {
    aborted_flag = true;
}
