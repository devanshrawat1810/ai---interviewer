#pragma once

#include <QString>
#include <QColor>

// OOP Concept: Design Token Structure (Encapsulation)
// Centralized visual constants ensuring WCAG AA contrast, consistent typography, radii, and elevation.
namespace ThemeTokens {
    // Spacing scale
    constexpr int Space4 = 4;
    constexpr int Space8 = 8;
    constexpr int Space12 = 12;
    constexpr int Space16 = 16;
    constexpr int Space24 = 24;
    constexpr int Space32 = 32;

    // Corner Radii
    constexpr int RadiusSm = 6;
    constexpr int RadiusMd = 8;   // Inputs, Buttons
    constexpr int RadiusLg = 12;  // Cards, Dialogs
    constexpr int RadiusPill = 999;

    // Animation Durations (ms)
    constexpr int AnimFast = 150;
    constexpr int AnimNormal = 250;
    constexpr int AnimSlow = 350;

    // Semantic Colors
    inline const QString ColorPrimary = "#4F46E5";       // Deep Indigo
    inline const QString ColorPrimaryHover = "#4338CA";
    inline const QString ColorAccent = "#0D9488";        // Teal
    inline const QString ColorAccentHover = "#0F766E";
    
    inline const QString ColorSuccess = "#16A34A";       // Green
    inline const QString ColorWarning = "#D97706";       // Amber
    inline const QString ColorDanger = "#DC2626";        // Red
    inline const QString ColorInfo = "#2563EB";          // Blue

    // Difficulty specific colors (always paired with text)
    inline const QString DifficultyEasy = "#16A34A";
    inline const QString DifficultyMedium = "#D97706";
    inline const QString DifficultyHard = "#DC2626";

    // Light Theme Palette
    inline const QString LightBg = "#F8FAFC";
    inline const QString LightSurface = "#FFFFFF";
    inline const QString LightBorder = "#E2E8F0";
    inline const QString LightTextPrimary = "#0F172A";
    inline const QString LightTextSecondary = "#475569";
    inline const QString LightTextMuted = "#94A3B8";

    // Dark Theme Palette
    inline const QString DarkBg = "#0F172A";
    inline const QString DarkSurface = "#1E293B";
    inline const QString DarkBorder = "#334155";
    inline const QString DarkTextPrimary = "#F8FAFC";
    inline const QString DarkTextSecondary = "#CBD5E1";
    inline const QString DarkTextMuted = "#64748B";
}
