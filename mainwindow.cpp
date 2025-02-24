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
    QList<QCheckBox*> checkboxes = ui->gridLayout->findChildren<QCheckBox*>();
    bool bIsAnyTrue = false;

    for (QCheckBox* checkbox : checkboxes) {
        if (checkbox->isChecked()) {
            bIsAnyTrue = true;
            QString version = checkbox->text();
            QString versionedPackageFolder = packageFolder + "/" + QFileInfo(pluginFile).baseName() + "_" + version;

            // Create version-specific output folder
            QDir().mkpath(versionedPackageFolder);

            // Build command
            QString command = uePath + "/" + version + "/Engine/Build/BatchFiles/RunUAT.bat BuildPlugin -plugin=\"" + pluginFile + "\" -package=\"" + versionedPackageFolder + "\"";

            // Create a QProcess for this build
            QProcess* process = new QProcess(this);
            QString logFilePath = versionedPackageFolder + "/build_log.txt";
            QFile* logFile = new QFile(logFilePath, this);

            if (!logFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
                appendToConsole("Failed to create log file for version: " + version);
                delete logFile;
                continue;
            }

            // Connect process signals
            connect(process, &QProcess::readyReadStandardOutput, this, [this, process, logFile]() {
                QString output = process->readAllStandardOutput();
                appendToConsole(output);
                logFile->write(output.toUtf8());
            });

            connect(process, &QProcess::readyReadStandardError, this, [this, process, logFile]() {
                QString error = process->readAllStandardError();
                appendToConsole(error);
                logFile->write(error.toUtf8());
            });

            connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, process, version, logFile](int exitCode, QProcess::ExitStatus exitStatus) {
                logFile->close();
                delete logFile;

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
                logFile->close();
                delete logFile;
            }
        }
    }

    // Re-enable the build button after all processes are started
    ui->btnBuild->setEnabled(true);

    if (bIsAnyTrue) {
        appendToConsole("Build process started for all selected versions.\n");
    } else {
        appendToConsole("No version was selected.\n");
    }
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

    // Add new checkboxes in a grid layout
    QGridLayout* gridLayout = qobject_cast<QGridLayout*>(ui->versionGroupBox->layout());
    if (!gridLayout) {
        gridLayout = new QGridLayout(ui->versionGroupBox);
        ui->versionGroupBox->setLayout(gridLayout);
    }

    int row = 0;
    int col = 0;

    for (const auto& version : versions) {
        QCheckBox* checkbox = new QCheckBox(version, this);
        gridLayout->addWidget(checkbox, row, col, Qt::AlignCenter);

        col++;
        if (col >= MAX_GRID_COLUMNS) {
            col = 0;
            row++;
        }
    }
}

void MainWindow::appendToConsole(const QString& text) {
    QTextCharFormat format;
    format.setForeground(Qt::white);

    if (text.contains("error", Qt::CaseSensitive)) {
        format.setForeground(Qt::red);
    } else if (text.contains("warning", Qt::CaseSensitive)) {
        format.setForeground(Qt::darkYellow);
    }

    // Apply the format to the text
    QTextCursor cursor = ui->consoleOutput->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(text, format);
    ui->consoleOutput->setTextCursor(cursor);
    ui->consoleOutput->ensureCursorVisible();
}
