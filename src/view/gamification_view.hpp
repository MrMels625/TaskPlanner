#ifndef GAMIFICATION_VIEW_HPP
#define GAMIFICATION_VIEW_HPP

#include "igamification_view.hpp"
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QDialog>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

namespace view
{

  class GamificationView: public QWidget, public IGamificationView
  {
    Q_OBJECT

  public:
    explicit GamificationView(QWidget *parent = nullptr);
    ~GamificationView() override = default;

    void showUserLevel(int level, int currentXP, int xpToNext) override;
    void showStreak(int days) override;
    void showUserTitle(const QString &title) override;
    void showXPNotification(int amount, const QString &reason) override;
    void showAchievementUnlocked(const storage::Achievement &achievement) override;
    void showAchievementsList(const QList< storage::Achievement > &achievements) override;
    void showCampusMap(const QList< QString > &unlockedLocations) override;
    void showLocationUnlocked(const QString &locationName) override;
    void showLevelUpAnimation(int newLevel, const QString &newTitle) override;

    void setLevelLabel(QLabel *label);
    void setProgressBar(QProgressBar *progressBar);
    void setExpValueLabel(QLabel *label);
    void setStreakLabel(QLabel *label);
    void setUserRankLabel(QLabel *label);
    void setAchievementLabels(const QList< QLabel* > &labels);

  signals:
    void gamificationViewReady() override;
    void achievementsRequested() override;
    void mapRequested() override;

  private:
    QLabel *m_levelLabel;
    QProgressBar *m_progressBar;
    QLabel *m_expValueLabel;
    QLabel *m_streakLabel;
    QLabel *m_userRankLabel;
    QList< QLabel* > m_achievementLabels;

    QPropertyAnimation *m_xpAnimation;
    QPropertyAnimation *m_levelAnimation;
    QGraphicsOpacityEffect *m_opacityEffect;
  };

}

#endif
