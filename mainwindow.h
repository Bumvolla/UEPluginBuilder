#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSelectUEPath();
    void onSelectPluginFile();
    void onSelectPackageFolder();
    void onBuildPlugin();
    void readProcessOutput();

private:
    Ui::MainWindow *ui;
    QProcess *buildProcess;
};

#endif // MAINWINDOW_H
