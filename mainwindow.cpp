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
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    //connect(ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(prepare_menu(const QPoint &)));
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &main_window::prepare_menu);
    QCommonStyle style;
    ui->actionScan_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionPrev_Group_Of_Dublicates->setIcon(style.standardIcon(QCommonStyle::SP_ArrowLeft));
    ui->actionNext_Group_Of_Dublicates->setIcon(style.standardIcon(QCommonStyle::SP_ArrowRight));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));
    ui->actionAbout->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    connect(ui->actionPrev_Group_Of_Dublicates, &QAction::triggered, this, &main_window::show_prev_dublicates);
    connect(ui->actionNext_Group_Of_Dublicates, &QAction::triggered, this, &main_window::show_next_dublicates);
    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::select_directory);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &main_window::show_about_dialog);

    scan_directory(QString("/home/demon1999/code/cpp-course/dublicates/for_test"), true);
}

main_window::~main_window()
{}

void main_window::prepare_menu(const QPoint & pos) {
    QTreeWidget *tree = ui->treeWidget;

    QTreeWidgetItem *current_item = tree->itemAt( pos );
    QAction *delete_action = new QAction(QIcon(":/Resource/warning32.ico"), tr("&Delete"), this);

    connect(delete_action, &QAction::triggered, this, [current_item, this]() {
                delete_element(current_item);});


    QMenu menu(this);
    menu.addAction(delete_action);

    QPoint pt(pos);
    menu.exec( tree->mapToGlobal(pos) );
}

void main_window::try_to_show(std::function<void()> change) {
    if (data.empty()) {
        current = data.begin();
        show_current();
        return;
    }
    auto& v = *current;
    QVector<QString> new_arr(0);
    for (auto path : v) {
        if (QFile(path).exists())
            new_arr.append(path);
    }
    v = new_arr;
    if (v.size() > 1) {
        show_current();
    } else {
        auto it = current;
        it++;
        if (it == data.end() && current == data.begin()) {
            data.clear();
            current = data.begin();
            show_current();
            return;
        }
        it--;
        change();
        data.erase(it);
        try_to_show(change);
    }
}

void main_window::delete_element(QTreeWidgetItem *deleted) {
    QString path = deleted->text(0);
    QString SHA256 = sha256[path];
    if (!QFile(path).exists()) {
        QMessageBox::information(0, "error", "File doesn't exist.");
    } else
    if (QFile(path).remove()) {
        sha256.erase(sha256.find(path));
    } else {
        QMessageBox::information(0, "error", "Can't be deleted.");
    }
    try_to_show([this](){return this->pluss();});
}

void main_window::pluss() {
    current++;
    if (current == data.end())
        current = data.begin();
}

void main_window::minuss() {
    if (current == data.begin())
        current = data.end();
    current--;
}

void main_window::show_next_dublicates() {
    pluss();
    try_to_show([this](){return this->pluss();});
}

void main_window::show_prev_dublicates() {
    if (data.empty()) return;
    minuss();
    try_to_show([this](){return this->minuss();});
}

void main_window::select_directory()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                    QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    scan_directory(dir, true);
}

void main_window::scan_directory(QString const& dir, bool is_first)
{
    if (is_first) {
        ui->treeWidget->clear();
        data.clear();
        setWindowTitle(QString("Directory Content - %1").arg(dir));
    }
    QDir d(dir);

    QFileInfoList list = d.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for (QFileInfo file_info : list)
    {
        if (file_info.isDir()) {
            if (QDir(file_info.absoluteFilePath()).isReadable())
                scan_directory(file_info.absoluteFilePath(), false);
        } else {
            QCryptographicHash hs(QCryptographicHash::Algorithm::Sha256);
            QFile file(file_info.absoluteFilePath());
            if(!file.open(QIODevice::ReadOnly)) {
                //file.setErrorString(QString("You're Fucking mothers son!"));
                //QMessageBox::information(0, "error", "You're fucking mother's son!");
                continue;
            }

            QTextStream in(&file);

            while(!in.atEnd()) {
                QString line = in.readLine();
                hs.addData(line.toStdString().c_str());
            }

            file.close();

            QString SHA256 = QString(hs.result().toHex());

            sha256[file_info.absoluteFilePath()] = SHA256;
            data[SHA256].append(file_info.absoluteFilePath());

        }
    }
    if (is_first == true) {
        current = data.begin();
        try_to_show([this](){return this->pluss();});
    }
}

void main_window::show_current() {
    QString title = QWidget::windowTitle();
    ui->treeWidget->clear();
    setWindowTitle(QString("Directory Content - %1").arg(title));

    if (current == data.end()) {
        return;
    }
    auto v = *current;
    for (auto eq_files : v) {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, eq_files);
        item->setText(1, QString::number(QFile(eq_files).size()));
        ui->treeWidget->addTopLevelItem(item);
    }
}

void main_window::show_about_dialog()
{
    QMessageBox::aboutQt(this);
}
