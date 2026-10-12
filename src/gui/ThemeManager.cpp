#include "gui/ThemeManager.h"
#include "gui/ThemeTokens.h"
#include <QApplication>

ThemeManager& ThemeManager::instance() {
    static ThemeManager mgr;
    return mgr;
}

ThemeManager::ThemeManager() {
    loadSettings();
}

bool ThemeManager::isDarkMode() const noexcept {
    if (currentTheme_ == ThemeMode::Dark) return true;
    return false; // Default to light for System unless configured
}

void ThemeManager::setTheme(ThemeMode mode) {
    if (currentTheme_ != mode) {
        currentTheme_ = mode;
        saveSettings();
        if (qApp) {
            qApp->setStyleSheet(generateStyleSheet());
        }
        emit themeChanged(mode);
    }
}

void ThemeManager::setReduceMotion(bool reduce) {
    if (reduceMotion_ != reduce) {
        reduceMotion_ = reduce;
        saveSettings();
        emit reduceMotionChanged(reduce);
    }
}

void ThemeManager::loadSettings() {
    QSettings s("AIInterviewer", "DesktopGUI");
    int themeVal = s.value("themeMode", static_cast<int>(ThemeMode::Light)).toInt();
    currentTheme_ = static_cast<ThemeMode>(themeVal);
    reduceMotion_ = s.value("reduceMotion", false).toBool();
}

void ThemeManager::saveSettings() {
    QSettings s("AIInterviewer", "DesktopGUI");
    s.setValue("themeMode", static_cast<int>(currentTheme_));
    s.setValue("reduceMotion", reduceMotion_);
}

QString ThemeManager::generateStyleSheet() const {
    bool dark = isDarkMode();
    const QString& bg = dark ? ThemeTokens::DarkBg : ThemeTokens::LightBg;
    const QString& surface = dark ? ThemeTokens::DarkSurface : ThemeTokens::LightSurface;
    const QString& border = dark ? ThemeTokens::DarkBorder : ThemeTokens::LightBorder;
    const QString& text = dark ? ThemeTokens::DarkTextPrimary : ThemeTokens::LightTextPrimary;

    return QString(R"(
        QWidget {
            background-color: %1;
            color: %2;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
            font-size: 14px;
        }
        QMainWindow, QDialog {
            background-color: %1;
        }
        QScrollArea, QStackedWidget {
            background-color: transparent;
            border: none;
        }
        QLabel {
            background-color: transparent;
            color: %2;
        }
        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: %3;
            color: %2;
            border: 1px solid %4;
            border-radius: 8px;
            padding: 8px 12px;
            selection-background-color: %5;
        }
        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {
            border: 2px solid %5;
        }
        QPushButton {
            border-radius: 8px;
            padding: 10px 16px;
            font-weight: bold;
        }
        QProgressBar {
            border: 1px solid %4;
            border-radius: 6px;
            background-color: %3;
            text-align: center;
        }
        QProgressBar::chunk {
            background-color: %5;
            border-radius: 5px;
        }
    )")
    .arg(bg)
    .arg(text)
    .arg(surface)
    .arg(border)
    .arg(ThemeTokens::ColorPrimary);
}
