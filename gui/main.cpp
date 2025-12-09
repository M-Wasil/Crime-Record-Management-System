#include "MainWindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName("Crime Record Management System");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("CRMS");
    
    // Set style
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Create and show main window
    MainWindow window;
    window.show();
    
    return app.exec();
}

