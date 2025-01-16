#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setStyleSheet(R"(
    QWidget {
        background-color: #2D2D2D;
        color: #FFFFFF;
        font-family: "Segoe UI";
        font-size: 14px;
    }
    QPushButton {
        background-color: #FFA500; /* Orange */
        color: #000000; /* Black text */
        border-radius: 5px;
        padding: 10px;
    }
    QPushButton:hover {
        background-color: #CC8400; /* Darker orange for hover effect */
    }
    QLineEdit, QTextEdit {
        background-color: #1E1E1E;
        color: #FFFFFF;
        border: 1px solid #444444;
        border-radius: 5px;
        padding: 5px;
    }
)");

    MainWindow window;
    window.show();

    return app.exec();
}
