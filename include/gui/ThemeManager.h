#pragma once

#include <QObject>
#include <QString>
#include <QSettings>

// OOP Concept: Singleton / State Manager Pattern
// Manages light, dark, and system themes, motion preferences, and generates stylesheets from tokens.
class ThemeManager : public QObject {
    Q_OBJECT

public:
    enum class ThemeMode {
        Light,
        Dark,
        System
    };

    static ThemeManager& instance();

    ThemeMode currentTheme() const noexcept { return currentTheme_; }
    bool isDarkMode() const noexcept;
    bool isReduceMotion() const noexcept { return reduceMotion_; }

    void setTheme(ThemeMode mode);
    void setReduceMotion(bool reduce);

    QString generateStyleSheet() const;

signals:
    void themeChanged(ThemeMode mode);
    void reduceMotionChanged(bool reduce);

private:
    ThemeManager();
    ~ThemeManager() override = default;

    ThemeMode currentTheme_{ThemeMode::Light};
    bool reduceMotion_{false};
    void loadSettings();
    void saveSettings();
};
