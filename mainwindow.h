#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#define MAX_GRID_COLUMNS (5)

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

private:
    Ui::MainWindow *ui;

    QStringList detectUnrealEngineVersions(const QString& uePath);
    void addVersionCheckboxes(const QStringList& versions);
    void appendToConsole(const QString& text);
};

#endif // MAINWINDOW_H
