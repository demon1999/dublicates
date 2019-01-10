#ifndef SCANNER_H
#define SCANNER_H
#include <QMap>
#include <QString>
#include <QVector>
#include <QObject>

class scanner : public QObject
{
        Q_OBJECT

private:
    volatile bool flag;
    QString dir;
    QMap<QString, QVector<QString> > data;
    QMap<QString, QString> sha256;
    void get_data(const QString &dir);
public:
    scanner(const QString &dir_name);
    void set_flag();
public slots:
    void run();
    QMap<QString, QString> get_sha256();
    QMap<QString, QVector<QString> > get_map_data();
signals:
    void done(const QMap<QString, QVector<QString> >  &_data,
              const QMap<QString, QString> &_sha256, const QString &_dir);
    void finished();
};
#endif // SCANNER_H
