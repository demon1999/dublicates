#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QVector>
#include <memory>
#include <iterator>

namespace Ui {
class MainWindow;
}

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit main_window(QWidget *parent = 0);
    ~main_window();

private slots:
    void select_directory();
    void show_next_dublicates();
    void show_prev_dublicates();
    void scan_directory(QString const& dir, bool is_first);
    void show_about_dialog();

private:
    void show_current();
    QMap<QString, QVector<QString> >::iterator current;
    QMap<QString, QVector<QString> > data;
    std::unique_ptr<Ui::MainWindow> ui;
};

#endif // MAINWINDOW_H
