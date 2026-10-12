#include <QApplication>
#include "gui/ServiceContext.h"
#include "gui/views/MainWindow.h"
#include "gui/ThemeManager.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // High DPI scaling is default enabled in Qt 6
    QApplication app(argc, argv);
    app.setApplicationName("AI Interviewer");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("CollegePlacementPrep");

    // Apply active theme stylesheet before showing the window to avoid flash
    app.setStyleSheet(ThemeManager::instance().generateStyleSheet());

    try {
        // Instantiate ServiceContext holding all factories and services
        auto ctx = ServiceContext::createDefault();

        MainWindow window(ctx);
        window.show();

        return app.exec();
    } catch (const std::exception& e) {
        std::cerr << "Fatal GUI startup error: " << e.what() << "\n";
        return 1;
    }
}
