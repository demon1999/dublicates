#ifndef SCANNER_H
#define SCANNER_H
#include <QMap>
#include <QString>
#include <QVector>
#include <QObject>
#include <functional>

class scanner : public QObject
{
        Q_OBJECT

private:
    volatile bool aborted_flag;
    int counter;
    int number_of_files;
    int now_percentage;
    QString dir;
    QMap<QString, bool> was;
    QMap<QString, QVector<QString> > data;
    void get_data(const QString &dir, bool load_files);
public:
    scanner(const QString &dir_name);
    void set_flag();
public slots:
    void run();
    QMap<QString, QVector<QString> > get_map_data();
signals:
    void percentage(int k);
    void done(const QMap<QString, QVector<QString> >  &_data, const QString &_dir);
    void finished();
};
#endif // SCANNER_H
