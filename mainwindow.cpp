#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Initialize process
    buildProcess = new QProcess(this);
    connect(buildProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::readProcessOutput);

    // Connect buttons to slots
    connect(ui->btnSelectUEPath, &QPushButton::clicked, this, &MainWindow::onSelectUEPath);
    connect(ui->btnSelectPluginFile, &QPushButton::clicked, this, &MainWindow::onSelectPluginFile);
    connect(ui->btnSelectPackageFolder, &QPushButton::clicked, this, &MainWindow::onSelectPackageFolder);
    connect(ui->btnBuild, &QPushButton::clicked, this, &MainWindow::onBuildPlugin);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onSelectUEPath() {
    QString path = QFileDialog::getExistingDirectory(this, "Select Unreal Engine Installation Path", "C:\\Program Files\\Epic Games");
    if (!path.isEmpty()) {
        ui->editUEPath->setText(path);
    }
}

void MainWindow::onSelectPluginFile() {
    QString file = QFileDialog::getOpenFileName(this, "Select .uplugin File", "", "Plugin Files (*.uplugin)");
    if (!file.isEmpty()) {
        ui->editPluginFile->setText(file);
    }
}

void MainWindow::onSelectPackageFolder() {
    QString path = QFileDialog::getExistingDirectory(this, "Select Package Output Folder");
    if (!path.isEmpty()) {
        ui->editPackageFolder->setText(path);
    }
}

void MainWindow::onBuildPlugin() {
    if (buildProcess->state() == QProcess::Running) {
        QMessageBox::warning(this, "Warning", "Build already in progress!");
        return;
    }

    QString uePath = ui->editUEPath->text();
    QString pluginFile = ui->editPluginFile->text();
    QString packageFolder = ui->editPackageFolder->text();

    if (uePath.isEmpty() || pluginFile.isEmpty() || packageFolder.isEmpty()) {
        QMessageBox::critical(this, "Error", "Please select UE path, plugin file, and package folder.");
        return;
    }

    // Clear console
    ui->consoleOutput->clear();

    // Construct the command
    QString command = uePath + "/Engine/Build/BatchFiles/RunUAT.bat";
    QStringList arguments;
    arguments << "BuildPlugin" << "-plugin=" + pluginFile << "-package=" + packageFolder;

    // Start the process
    buildProcess->start(command, arguments);
    if (!buildProcess->waitForStarted()) {
        QMessageBox::critical(this, "Error", "Failed to start the build process.");
        return;
    }

    ui->consoleOutput->append("Build process started...\n");
}

void MainWindow::readProcessOutput() {
    QByteArray output = buildProcess->readAllStandardOutput();
    ui->consoleOutput->append(output);
}
