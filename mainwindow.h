#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QMap>
#include <QString>
#include <QVector>
#include <functional>
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
    void delete_element(QTreeWidgetItem *deleted);
    void prepare_menu(const QPoint &pos);
    void select_directory();
    void show_next_dublicates();
    void show_prev_dublicates();
    void scan_directory(QString const& dir, bool is_first);
    void show_about_dialog();

private:
    void pluss();
    void minuss();
    void try_to_show(std::function<void()>);
    void show_current();
    QMap<QString, QVector<QString> >::iterator current;
    QMap<QString, QVector<QString> > data;
    QMap<QString, QString> sha256;
    std::unique_ptr<Ui::MainWindow> ui;
};

#endif // MAINWINDOW_H
