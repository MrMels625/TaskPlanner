#include "gamification_view.hpp"
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QLabel>

view::GamificationView::GamificationView(QWidget *parent):
  QWidget(parent),
  m_levelLabel(nullptr),
  m_progressBar(nullptr),
  m_expValueLabel(nullptr),
  m_streakLabel(nullptr),
  m_userRankLabel(nullptr),
  m_xpAnimation(nullptr),
  m_levelAnimation(nullptr),
  m_opacityEffect(nullptr)
{}

void view::GamificationView::setLevelLabel(QLabel *label)
{
  m_levelLabel = label;
}

void view::GamificationView::setProgressBar(QProgressBar *progressBar)
{
  m_progressBar = progressBar;
}

void view::GamificationView::setExpValueLabel(QLabel *label)
{
  m_expValueLabel = label;
}

void view::GamificationView::setStreakLabel(QLabel *label)
{
  m_streakLabel = label;
}

void view::GamificationView::setUserRankLabel(QLabel *label)
{
  m_userRankLabel = label;
}

void view::GamificationView::setAchievementLabels(const QList<QLabel*> &labels)
{
  m_achievementLabels = labels;
}

void view::GamificationView::showUserLevel(int level, int currentXP, int xpToNext)
{
  if (m_levelLabel)
  {
    m_levelLabel->setText("Lv." + QString::number(level));
  }

  if (m_progressBar)
  {
    m_progressBar->setMaximum(xpToNext);
    m_progressBar->setValue(currentXP);
  }

  if (m_expValueLabel)
  {
    m_expValueLabel->setText(QString("%1 / %2").arg(currentXP).arg(xpToNext));
  }
}

void view::GamificationView::showStreak(int days)
{
  if (m_streakLabel)
  {
    m_streakLabel->setText("🔥 " + QString::number(days) + " дней");
  }
}

void view::GamificationView::showUserTitle(const QString &title)
{
  if (m_userRankLabel)
  {
    m_userRankLabel->setText("🏅 " + title);
  }
}

void view::GamificationView::showXPNotification(int amount, const QString &reason)
{
  Q_UNUSED(reason);

  if (!m_opacityEffect)
  {
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(m_opacityEffect);
  }

  m_opacityEffect->setOpacity(1.0);

  if (m_xpAnimation)
  {
    m_xpAnimation->stop();
  }

  m_xpAnimation = new QPropertyAnimation(m_opacityEffect, "opacity", this);
  m_xpAnimation->setDuration(3000);
  m_xpAnimation->setStartValue(1.0);
  m_xpAnimation->setEndValue(0.0);
  m_xpAnimation->start();

  // TODO: Здесь будет отображение уведомления о получении XP
  // Например: QLabel с текстом "+{amount} XP" с анимацией исчезновения
}

void view::GamificationView::showAchievementUnlocked(const storage::Achievement &achievement)
{
  Q_UNUSED(achievement);

  // TODO: Здесь будет отображение уведомления о разблокировке достижения
  // Например: всплывающее окно с иконкой и названием достижения
}

void view::GamificationView::showAchievementsList(const QList<storage::Achievement> &achievements)
{
  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Достижения");
  dialog->setMinimumSize(400, 500);

  QVBoxLayout *layout = new QVBoxLayout(dialog);

  QListWidget *listWidget = new QListWidget(dialog);

  for (const storage::Achievement &achievement : achievements)
  {
    QListWidgetItem *item = new QListWidgetItem();

    QString text = achievement.name;
    if (achievement.isUnlocked)
    {
      text += " ✓";
    }
    else
    {
      text += " 🔒";
    }

    item->setText(text);

    if (!achievement.isUnlocked)
    {
      QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
      effect->setOpacity(0.5);
      item->setData(Qt::UserRole, QVariant::fromValue(effect));
    }

    listWidget->addItem(item);
  }

  layout->addWidget(listWidget);

  // TODO: Здесь будет более детальное отображение достижений
  // с иконками, описаниями и датами разблокировки

  dialog->exec();
}

void view::GamificationView::showCampusMap(const QList<QString> &unlockedLocations)
{
  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Карта кампуса");
  dialog->setMinimumSize(600, 400);

  QVBoxLayout *layout = new QVBoxLayout(dialog);

  QLabel *mapLabel = new QLabel("Карта кампуса", dialog);
  mapLabel->setAlignment(Qt::AlignCenter);

  QListWidget *locationList = new QListWidget(dialog);

  // TODO: Здесь будет изображение карты кампуса
  // mapLabel->setPixmap(QPixmap(":/images/campus_map.png"));

  for (const QString &location : unlockedLocations)
  {
    QListWidgetItem *item = new QListWidgetItem();
    item->setText(location + " ✓");
    locationList->addItem(item);
  }

  layout->addWidget(mapLabel);
  layout->addWidget(locationList);

  // TODO: Здесь будет интерактивная карта с маркерами локаций
  // Разблокированные локации - цветные, заблокированные - серые

  dialog->exec();
}

void view::GamificationView::showLocationUnlocked(const QString &locationName)
{
  Q_UNUSED(locationName);

  // TODO: Здесь будет уведомление о разблокировке новой локации
  // Например: всплывающее окно с названием локации
}

void view::GamificationView::showLevelUpAnimation(int newLevel, const QString &newTitle)
{
  Q_UNUSED(newTitle);

  if (m_levelLabel)
  {
    m_levelLabel->setText("Lv." + QString::number(newLevel));
  }

  if (!m_opacityEffect)
  {
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(m_opacityEffect);
  }

  m_opacityEffect->setOpacity(1.0);

  if (m_levelAnimation)
  {
    m_levelAnimation->stop();
  }

  m_levelAnimation = new QPropertyAnimation(m_opacityEffect, "opacity", this);
  m_levelAnimation->setDuration(5000);
  m_levelAnimation->setStartValue(1.0);
  m_levelAnimation->setEndValue(0.0);
  m_levelAnimation->start();

  // TODO: Здесь будет более сложная анимация повышения уровня
  // Например: модальное окно с анимацией "Уровень повышен!"
}
