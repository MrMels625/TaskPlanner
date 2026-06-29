#ifndef ACHIEVEMENTS_HPP
#define ACHIEVEMENTS_HPP
#include <QString>
#include <cmath>

namespace storage
{
  namespace xp
  {
    inline constexpr int TASK_LOW = 10;
    inline constexpr int TASK_MEDIUM = 25;
    inline constexpr int TASK_HARD = 50;
    inline constexpr int ON_TIME_BONUS = 25;
    inline constexpr int PERFECT_DAY = 75;
    inline constexpr int OVERDUE_MAX = 50;
    inline constexpr int OVERDUE_HOURS_DECAY = 2;
    inline constexpr int FORMULA_BASE = 100;
    inline constexpr double FORMULA_MULTIPLIER = 1.05;
    inline constexpr int MAX_LEVEL = 50;
  }

  inline int calculateXPForLevel(int level)
  {
    if (level <= 0 || level > xp::MAX_LEVEL)
    {
      return 0;
    }
    return static_cast< int >(xp::FORMULA_BASE * std::pow(xp::FORMULA_MULTIPLIER, level - 1));
  }

  inline int calculateTotalXPForLevel(int targetLevel)
  {
    if (targetLevel <= 1)
    {
      return 0;
    }

    if (targetLevel > xp::MAX_LEVEL)
    {
      targetLevel = xp::MAX_LEVEL;
    }

    int totalXP = 0;
    for (qsizetype level = 1; level < targetLevel; ++level)
    {
      totalXP += calculateXPForLevel(level);
    }

    return totalXP;
  }

  inline int calculateLevelFromXP(int totalXP)
  {
    if (totalXP <= 0)
    {
      return 1;
    }

    int level = 1;
    int currentXP = 0;
    while (level < xp::MAX_LEVEL && currentXP + calculateXPForLevel(level) <= totalXP)
    {
      currentXP += calculateXPForLevel(level);
      ++level;
    }

    return level;
  }

  struct Achievement
  {
    QString id;
    QString name;
    QString description;
    QString iconPath;
    int xpReward;
    QString type;
  };

  namespace achievements
  {
    inline constexpr Achievement LEVEL_1
    {
      "level_1",
      "Level 1 Reached",
      "Reached level 1",
      ":/icons/achievement_level_1.png",
      0,
      "level"
    };

    inline constexpr Achievement LEVEL_5
    {
      "level_5",
      "Level 5 Reached",
      "Reached level 5",
      ":/icons/achievement_level_5.png",
      250,
      "level"
    };

    inline constexpr Achievement LEVEL_10
    {
      "level_10",
      "Level 10 Reached",
      "Reached level 10",
      ":/icons/achievement_level_10.png",
      500,
      "level"
    };

    inline constexpr Achievement LEVEL_20
    {
      "level_20",
      "Level 20 Reached",
      "Reached level 20",
      ":/icons/achievement_level_20.png",
      1000,
      "level"
    };

    inline constexpr Achievement LEVEL_30
    {
      "level_30",
      "Level 30 Reached",
      "Reached level 30",
      ":/icons/achievement_level_30.png",
      2000,
      "level"
    };

    inline constexpr Achievement LEVEL_50
    {
      "level_50",
      "Level 50 Reached",
      "Reached level 50 (maximum)",
      ":/icons/achievement_level_50.png",
      5000,
      "level"
    };

    inline constexpr Achievement TASKS_10
    {
      "tasks_10",
      "Task Completion 10",
      "Completed 10 tasks",
      ":/icons/achievement_tasks_10.png",
      100,
      "task"
    };

    inline constexpr Achievement TASKS_50
    {
      "tasks_50",
      "Task Completion 50",
      "Completed 50 tasks",
      ":/icons/achievement_tasks_50.png",
      300,
      "task"
    };

    inline constexpr Achievement TASKS_100
    {
      "tasks_100",
      "Task Completion 100",
      "Completed 100 tasks",
      ":/icons/achievement_tasks_100.png",
      750,
      "task"
    };

    inline constexpr Achievement TASKS_250
    {
      "tasks_250",
      "Task Completion 250",
      "Completed 250 tasks",
      ":/icons/achievement_tasks_250.png",
      2000,
      "task"
    };

    inline constexpr Achievement ON_TIME_5
    {
      "on_time_5",
      "Punctuality 5",
      "Completed 5 tasks on time",
      ":/icons/achievement_on_time_5.png",
      100,
      "task"
    };

    inline constexpr Achievement ON_TIME_20
    {
      "on_time_20",
      "Punctuality 20",
      "Completed 20 tasks on time",
      ":/icons/achievement_on_time_20.png",
      300,
      "task"
    };

    inline constexpr Achievement ON_TIME_50
    {
      "on_time_50",
      "Punctuality 50",
      "Completed 50 tasks on time",
      ":/icons/achievement_on_time_50.png",
      750,
      "task"
    };

    inline constexpr Achievement HARD_10
    {
      "hard_10",
      "Hard Tasks 10",
      "Completed 10 hard priority tasks",
      ":/icons/achievement_hard_10.png",
      200,
      "task"
    };

    inline constexpr Achievement HARD_50
    {
      "hard_50",
      "Hard Tasks 50",
      "Completed 50 hard priority tasks",
      ":/icons/achievement_hard_50.png",
      500,
      "task"
    };

    inline constexpr Achievement MEDIUM_30
    {
      "medium_30",
      "Medium Tasks 30",
      "Completed 30 medium priority tasks",
      ":/icons/achievement_medium_30.png",
      300,
      "task"
    };

    inline constexpr Achievement LOW_50
    {
      "low_50",
      "Low Tasks 50",
      "Completed 50 low priority tasks",
      ":/icons/achievement_low_50.png",
      250,
      "task"
    };

    inline constexpr Achievement BALANCED_ALL
    {
      "balanced_all",
      "Balanced Master",
      "Completed 10 tasks of each priority",
      ":/icons/achievement_balanced.png",
      250,
      "task"
    };

    inline constexpr Achievement PERFECT_DAY_1
    {
      "perfect_day_1",
      "Perfect Day 1",
      "Completed all tasks for 1 day",
      ":/icons/achievement_perfect_day_1.png",
      100,
      "special"
    };

    inline constexpr Achievement PERFECT_DAY_7
    {
      "perfect_day_7",
      "Perfect Days 7",
      "Completed all tasks for 7 days",
      ":/icons/achievement_perfect_day_7.png",
      300,
      "special"
    };

    inline constexpr Achievement PERFECT_DAY_30
    {
      "perfect_day_30",
      "Perfect Days 30",
      "Completed all tasks for 30 days",
      ":/icons/achievement_perfect_day_30.png",
      1000,
      "special"
    };

    inline constexpr Achievement STREAK_7
    {
      "streak_7",
      "Active Streak 7",
      "Active for 7 consecutive days",
      ":/icons/achievement_streak_7.png",
      150,
      "special"
    };

    inline constexpr Achievement STREAK_30
    {
      "streak_30",
      "Active Streak 30",
      "Active for 30 consecutive days",
      ":/icons/achievement_streak_30.png",
      500,
      "special"
    };

    inline constexpr Achievement STREAK_100
    {
      "streak_100",
      "Active Streak 100",
      "Active for 100 consecutive days",
      ":/icons/achievement_streak_100.png",
      2000,
      "special"
    };

    inline constexpr Achievement COMBO_NIGHTMARE
    {
      "combo_nightmare",
      "Combo Nightmare",
      "Completed 3 hard tasks in one day",
      ":/icons/achievement_nightmare.png",
      200,
      "special"
    };

    inline constexpr Achievement COMBO_MARATHON
    {
      "combo_marathon",
      "Combo Marathon",
      "Completed 15+ tasks in one day",
      ":/icons/achievement_marathon.png",
      300,
      "special"
    };

    inline constexpr Achievement LOCATION_5
    {
      "location_5",
      "Locations 5",
      "Unlocked 5 locations",
      ":/icons/achievement_location_5.png",
      200,
      "special"
    };

    inline constexpr Achievement LOCATION_10
    {
      "location_10",
      "Locations 10",
      "Unlocked 10 locations",
      ":/icons/achievement_location_10.png",
      500,
      "special"
    };

    inline constexpr Achievement LOCATION_ALL
    {
      "location_all",
      "Locations Master",
      "Unlocked all locations",
      ":/icons/achievement_location_all.png",
      1500,
      "special"
    };

    inline constexpr Achievement DELETE_5
    {
      "delete_5",
      "Deletions 5",
      "Deleted 5 tasks",
      ":/icons/achievement_delete_5.png",
      50,
      "special"
    };

    inline constexpr Achievement DELETE_20
    {
      "delete_20",
      "Deletions 20",
      "Deleted 20 tasks",
      ":/icons/achievement_delete_20.png",
      150,
      "special"
    };

    inline constexpr Achievement DELETE_100
    {
      "delete_100",
      "Deletions 100",
      "Deleted 100 tasks",
      ":/icons/achievement_delete_100.png",
      500,
      "special"
    };
  }
}

#endif

