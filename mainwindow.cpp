#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QDir>
#include <QCheckBox>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

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

        // Detect versions and add checkboxes
        QStringList versions = detectUnrealEngineVersions(path);
        addVersionCheckboxes(versions);
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
    QString uePath = ui->editUEPath->text();
    QString pluginFile = ui->editPluginFile->text();
    QString packageFolder = ui->editPackageFolder->text();

    if (uePath.isEmpty() || pluginFile.isEmpty() || packageFolder.isEmpty()) {
        QMessageBox::critical(this, "Error", "Please select UE path, plugin file, and package folder.");
        return;
    }

    // Disable the build button to prevent multiple clicks
    ui->btnBuild->setEnabled(false);

    // Get selected versions
    QList<QCheckBox*> checkboxes = ui->versionGroupBox->findChildren<QCheckBox*>();
    for (QCheckBox* checkbox : checkboxes) {
        if (checkbox->isChecked()) {
            QString version = checkbox->text();
            QString versionedPackageFolder = packageFolder + "/" + QFileInfo(pluginFile).baseName() + "_" + version;

            // Create version-specific output folder
            QDir().mkpath(versionedPackageFolder);

            // Build command
            QString command = uePath + "/" + version + "/Engine/Build/BatchFiles/RunUAT.bat BuildPlugin -plugin=\"" + pluginFile + "\" -package=\"" + versionedPackageFolder + "\"";

            // Create a QProcess for this build
            QProcess* process = new QProcess(this);
            connect(process, &QProcess::readyReadStandardOutput, this, [this, process]() {
                appendToConsole(process->readAllStandardOutput());
            });
            connect(process, &QProcess::readyReadStandardError, this, [this, process]() {
                appendToConsole(process->readAllStandardError());
            });
            connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, process, version](int exitCode, QProcess::ExitStatus exitStatus) {
                if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                    appendToConsole("Build process completed for version: " + version);
                } else {
                    appendToConsole("Build process failed for version: " + version);
                }
                process->deleteLater(); // Clean up the process
            });

            // Start the build process
            process->start(command);
            if (!process->waitForStarted()) {
                QMessageBox::critical(this, "Error", "Failed to start the build process for version: " + version);
                process->deleteLater(); // Clean up the process
            }
        }
    }

    // Re-enable the build button after all processes are started
    ui->btnBuild->setEnabled(true);
    appendToConsole("Build process started for all selected versions.");
}

QStringList MainWindow::detectUnrealEngineVersions(const QString& uePath) {
    QDir ueDir(uePath);
    QStringList versions;

    // Look for directories starting with "UE_"
    QStringList filters;
    filters << "UE_*";
    ueDir.setNameFilters(filters);
    ueDir.setFilter(QDir::Dirs);

    for (const auto& dir : ueDir.entryList()) {
        versions.append(dir);
    }

    return versions;
}

void MainWindow::addVersionCheckboxes(const QStringList& versions) {
    // Clear existing checkboxes
    QLayout* layout = ui->versionGroupBox->layout();
    if (layout) {
        QLayoutItem* item;
        while ((item = layout->takeAt(0))) {
            delete item->widget();
            delete item;
        }
    }

    // Add new checkboxes
    for (const auto& version : versions) {
        QCheckBox* checkbox = new QCheckBox(version, this);
        ui->versionGroupBox->layout()->addWidget(checkbox);
    }
}

void MainWindow::appendToConsole(const QString& text) {
    ui->consoleOutput->append(text);
}
