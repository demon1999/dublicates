#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMap>
#include <QVector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QCryptographicHash>
#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

main_window::main_window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    QCommonStyle style;
    ui->actionScan_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));
    ui->actionAbout->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::select_directory);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &main_window::show_about_dialog);
    QMap<QString, QVector<QString>> data;
    scan_directory(QDir::currentPath(), true, data);
}

main_window::~main_window()
{}

void main_window::select_directory()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                    QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QMap<QString, QVector<QString>> data;
    scan_directory(dir, true, data);
}

void main_window::scan_directory(QString const& dir, bool is_first, QMap<QString, QVector<QString> >& data)
{
    if (is_first) {
        ui->treeWidget->clear();
        setWindowTitle(QString("Directory Content - %1").arg(dir));
    }
    QDir d(dir);

    QFileInfoList list = d.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for (QFileInfo file_info : list)
    {
        if (file_info.isDir()) {
            //continue;
            scan_directory(file_info.absoluteFilePath(), false, data);
        } else {
            QCryptographicHash hs(QCryptographicHash::Algorithm::Sha256);
            //hs.addData(file_info.absoluteFilePath().toStdString().c_str(), file_info.absoluteFilePath().size());
            QFile file(file_info.absoluteFilePath());
            if(!file.open(QIODevice::ReadOnly)) {
                //file.setErrorString(QString("You're Fucking mothers son!"));
                QMessageBox::information(0, "error", "You're fucking mother's son!");
            }

            QTextStream in(&file);

            while(!in.atEnd()) {
                QString line = in.readLine();
                hs.addData(line.toStdString().c_str());
            }

            file.close();
            data[QString(hs.result().toHex())].append(file_info.absoluteFilePath());

        }
    }
    if (is_first == true) {
        int number_of_types = 0;
        for (auto v : data) {
            for (auto eq_files : v) {
                QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget);
                item->setText(0, eq_files);
                item->setText(1, "Type: " + QString::number(QFile(eq_files).size()));
                ui->treeWidget->addTopLevelItem(item);
            }
            number_of_types++;
        }
    }
}

void main_window::show_about_dialog()
{
    QMessageBox::aboutQt(this);
}
