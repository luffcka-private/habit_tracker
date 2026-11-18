#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QVector>

#include "Habit.h"

/**
 * @file DatabaseManager.h
 * @class IDatabaseManager
 * @brief Интерфейс менеджера базы данных.
 *
 * Нужен для соблюдения принципа зависимости от абстракций:
 * слой логики (src/logic) будет принимать указатель на IDatabaseManager,
 * а не на конкретный PostgreSQL-класс. Это упрощает юнит-тестирование
 * (можно внедрить in-memory заглушку) и возможную смену СУБД.
 */
class IDatabaseManager
{
public:
    /**
     * @brief Виртуальный деструктор.
     *
     * @details Обязателен, т.к. объект удаляется через указатель на базовый класс.
     */
    virtual ~IDatabaseManager() = default;

    /**
     * @brief Загружает все привычки из таблицы habits.
     * @return Вектор привычек; пустой, если таблица пуста.
     * @throws DatabaseException при ошибке запроса.
     */
    virtual QVector<Habit> getAllHabits() = 0;

    /**
     * @brief Добавляет новую привычку.
     * @param name Отображаемое название привычки (не пусто).
     * @return Идентификатор созданной записи.
     * @throws DatabaseException при ошибке вставки или пустом имени.
     */
    virtual int addHabit(const QString& name) = 0;

    /**
     * @brief Меняет название существующей привычки.
     * @param id Идентификатор записи.
     * @param newName Новое название (не пусто).
     * @return true, если запись найдена и обновлена.
     * @throws DatabaseException при ошибке запроса.
     */
    virtual bool updateHabitName(int id, const QString& newName) = 0;

    /**
     * @brief Переключает признак выполнения «сегодня» (закрытие цикла).
     * @param id Идентификатор записи.
     * @return true, если запись найдена и обновлена.
     * @throws DatabaseException при ошибке запроса.
     */
    virtual bool toggleHabitDone(int id) = 0;

    /**
     * @brief Удаляет привычку по идентификатору.
     * @param id Идентификатор записи.
     * @return true, если запись найдена и удалена.
     * @throws DatabaseException при ошибке запроса.
     */
    virtual bool removeHabit(int id) = 0;
};

/**
 * @struct DbConfig
 * @brief Параметры подключения к PostgreSQL.
 *
 * Вынесены в структуру, чтобы не хранить секреты в коде:
 * значения читаются из переменных окружения, здесь лежат
 * только безопасные значения по умолчанию для разработки.
 */
struct DbConfig
{
    QString hostName = QStringLiteral("localhost");     ///< Хост сервера PostgreSQL
    quint16 port = 5432;                                ///< Порт сервера
    QString database = QStringLiteral("habit_tracker"); ///< Имя БД проекта
    QString userName = QStringLiteral("app_user");      ///< Пользователь
    QString password;                                   ///< Пароль
    QString appName = QStringLiteral("HabitTracker");   ///< Имя соединения внутри Qt
};

/**
 * @class DatabaseManager
 * @brief Реализация IDatabaseManager поверх QSqlDatabase (драйвер QPSQL).
 *
 * @details Типичное использование:
 * @code
 *     auto& db = DatabaseManager::instance();   // коннект устанавливается в ctor
 *     for (const Habit& h : db.getAllHabits())
 *         qDebug() << h.name << h.isDone;
 *     db.addHabit("Зарядка");
 * @endcode
 *
 * Освобождение ресурсов — в деструкторе (при завершении приложения).
 */
class DatabaseManager : public QObject, public IDatabaseManager
{
    Q_OBJECT

public:
    /**
     * @brief Возвращает единственный экземпляр менеджера (Singleton).
     * @return Ссылка на живой объект DatabaseManager.
     *
     * @details Ленивая инициализация: соединение с PostgreSQL открывается
     * при первом вызове. Потокобезопасно (C++11 static local).
     */
    static DatabaseManager& instance();

    /**
     * @brief Проверка успешности подключения к базе.
     * @return true, если соединение активно.
     */
    bool isOpen() const;

    // ---------- CRUD привычек (реализация IDatabaseManager) ----------

    /**
     * @brief Выбирает все строки таблицы habits.
     * @return Вектор Habit со всеми полями (id, name, is_done).
     * @throws DatabaseException, если запрос завершился с ошибкой.
     */
    QVector<Habit> getAllHabits() override;

    /**
     * @brief Вставляет новую привычку (is_done = false по умолчанию).
     * @param name Название привычки.
     * @return Новый id, выданный последовательностью PostgreSQL.
     * @throws DatabaseException при пустом имени или ошибке вставки.
     */
    int addHabit(const QString& name) override;

    /**
     * @brief Обновляет поле name по id.
     * @param id      Идентификатор записи.
     * @param newName Новое название.
     * @return true, если затронута хотя бы одна строка.
     * @throws DatabaseException при пустом имени или ошибке запроса.
     */
    bool updateHabitName(int id, const QString& newName) override;

    /**
     * @brief Инвертирует is_done: `SET is_done = NOT is_done`.
     * @param id Идентификатор записи.
     * @return true, если затронута хотя бы одна строка.
     * @throws DatabaseException при ошибке запроса.
     */
    bool toggleHabitDone(int id) override;

    /**
     * @brief Удаляет запись habits по id.
     * @param id Идентификатор записи.
     * @return true, если затронута хотя бы одна строка.
     * @throws DatabaseException при ошибке запроса.
     */
    bool removeHabit(int id) override;

signals:
    /**
     * @brief Сигнал об изменении набора привычек.
     *
     * @details Эмитится после add/update/toggle/remove. Модель списка
     * в UI подписывается на него и перезапрашивает данные — так список
     * на странице «Сегодня» всегда актуален (юзити-требование: мгновенный
     * отклик интерфейса на действия пользователя).
     * @param reason Краткая причина для отладки ("added", "toggled"...).
     */
    void habitsChanged(const QString& reason);

    /**
     * @brief Сигнал о ошибке уровня БД (для показа UserDialog в UI).
     * @param message Человекочитаемое описание проблемы.
     */
    void dbError(const QString& message);

private:
    /**
     * @brief Конструктор: формирует DbConfig и открывает соединение.
     * @param config Параметры подключения.
     *
     * Приватен, т.к. экземпляр создаётся только через instance().
     */
    explicit DatabaseManager(const DbConfig& config = DbConfig{});

    /// @brief Запрет копирования
    Q_DISABLE_COPY(DatabaseManager)

    /**
     * @brief Открывает соединение QPSQL; бросает DatabaseException при неудаче.
     * @throws DatabaseException, если драйвер недоступен или сервер отверг коннект.
     */
    void openOrThrow();

    /**
     * @brief Создаёт таблицу habits при её отсутствии (идемпотентно).
     *
     * @details `CREATE TABLE IF NOT EXISTS habits (id SERIAL PRIMARY KEY,
     * name TEXT NOT NULL, is_done BOOLEAN NOT NULL DEFAULT FALSE)`.
     * Позволяет запустить приложение на чистой БД без ручных миграций.
     */
    void ensureSchema();

    /**
     * @brief Читает параметры подключения из переменных окружения.
     * @return Заполненный DbConfig (HT_DB_HOST, HT_DB_PORT, HT_DB_NAME,
     *         HT_DB_USER, HT_DB_PASSWORD).
     *
     * @details Секреты не хардкодим: пароль берётся только из окружения.
     */
    static DbConfig configFromEnvironment();

    QSqlDatabase m_db; ///< Активное соединение Qt↔PostgreSQL
    DbConfig     m_config; ///< Использованные параметры подключения
};
