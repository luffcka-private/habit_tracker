#include "DatabaseManager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QtGlobal>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "../common/AppException.h"

/**
 * @file DatabaseManager.cpp
 * @brief Реализация доступа к PostgreSQL через Qt SQL (драйвер QPSQL).
 *
 * @details Все запросы — параметризованные (`:name`), что защищает от
 * SQL-инъекций и корректно экранирует пользовательский ввод из UI.
 */

namespace {

/// @brief Имя соединения в реестре QSqlDatabase (уникально для приложения)
constexpr auto kConnectionName = "habit_tracker_pg";

/**
 * @brief Бросает DatabaseException с деталями последней ошибки Qt SQL.
 * @param context Русское описание операции ("Не удалось подключиться...").
 * @param db      Соединение, у которого читаем lastError().
 */
[[noreturn]] void throwDb(const QString& context, const QSqlDatabase& db)
{
    const QString text = QStringLiteral("%1: %2")
    .arg(context, db.lastError().text());
    qWarning().noquote() << "[DB]" << text;
    throw DatabaseException(text.toStdString());
}

/**
 * @brief Бросает DatabaseException по ошибке конкретного запроса.
 * @param context Описание операции.
 * @param query   Выполненный QSqlQuery с ошибкой.
 */
[[noreturn]] void throwQuery(const QString& context, const QSqlQuery& query)
{
    const QString text = QStringLiteral("%1: %2")
    .arg(context, query.lastError().text());
    qWarning().noquote() << "[DB]" << text;
    throw DatabaseException(text.toStdString());
}
}

/**
 * @brief Ленивая инициализация единственного экземпляра.
 * @return Ссылка на DatabaseManager.
 */
DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager self{configFromEnvironment()}; ///< Единственный экземпляр
    return self;
}

/**
 * @brief Конструктор: сохраняет конфиг, открывает соединение, гарантирует схему.
 * @param config Параметры подключения.
 */
DatabaseManager::DatabaseManager(const DbConfig& config)
    : QObject(nullptr)
    , m_config(config)
{
    openOrThrow();
    ensureSchema();
}

/**
 * @brief Проверка активности соединения.
 * @return true, если QSqlDatabase::isOpen().
 */
bool DatabaseManager::isOpen() const
{
    return m_db.isOpen();
}

/**
 * @brief Читает параметры из переменных окружения HT_DB_*.
 * @return Заполненный DbConfig.
 */
DbConfig DatabaseManager::configFromEnvironment()
{
    DbConfig cfg; ///< значения по умолчанию уже заданы в структуре

    if (qEnvironmentVariableIsSet("HT_DB_HOST"))
        cfg.hostName = qEnvironmentVariable("HT_DB_HOST");
    if (qEnvironmentVariableIsSet("HT_DB_PORT"))
        cfg.port = static_cast<quint16>(qEnvironmentVariable("HT_DB_PORT").toUShort());
    if (qEnvironmentVariableIsSet("HT_DB_NAME"))
        cfg.database = qEnvironmentVariable("HT_DB_NAME");
    if (qEnvironmentVariableIsSet("HT_DB_USER"))
        cfg.userName = qEnvironmentVariable("HT_DB_USER");

    /// Пароль намеренно НЕ имеет значения по умолчанию — только из окружения.
    cfg.password = qEnvironmentVariable("HT_DB_PASSWORD");

    return cfg;
}

/**
 * @brief Настраивает и открывает соединение QPSQL.
 * @throws DatabaseException при отсутствии драйвера или отказе сервера.
 */
void DatabaseManager::openOrThrow()
{
    if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QPSQL")))
    {
        const QString text = QStringLiteral(
            "Драйвер QPSQL недоступен. Убедитесь, что установлен пакет "
            "Qt6 SQL PostgreSQL plugin (libqt6sql6-psql / psqldriver).");
        qWarning().noquote() << "[DB]" << text;
        emit dbError(text);
        throw DatabaseException(text.toStdString());
    }

    m_db = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"),
                                     QStringLiteral(kConnectionName));
    m_db.setHostName(m_config.hostName);
    m_db.setPort(m_config.port);
    m_db.setDatabaseName(m_config.database);
    m_db.setUserName(m_config.userName);
    m_db.setPassword(m_config.password);

    if (!m_db.open())
    {
        emit dbError(QStringLiteral("Нет подключения к PostgreSQL '%1'")
                         .arg(m_config.database));
        throwDb(QStringLiteral("Не удалось подключиться к PostgreSQL"), m_db);
    }

    qDebug().noquote() << "[DB] Подключено:" << m_config.database
                       << "@" << m_config.hostName << ":" << m_config.port;
}

/**
 * @brief Идемпотентно создаёт таблицу habits.
 */
void DatabaseManager::ensureSchema()
{
    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS habits ("
            "  id      SERIAL PRIMARY KEY,"
            "  name    TEXT    NOT NULL,"
            "  is_done BOOLEAN NOT NULL DEFAULT FALSE"
            ")")))
    {
        throwQuery(QStringLiteral("Не удалось создать таблицу habits"), query);
    }
}

/**
 * @brief Выбирает все привычки, упорядоченные по названию.
 * @return Вектор Habit.
 */
QVector<Habit> DatabaseManager::getAllHabits()
{
    QVector<Habit> result;

    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral(
            "SELECT id, name, is_done FROM habits ORDER BY name COLLATE \"C\"")))
    {
        throwQuery(QStringLiteral("Ошибка SELECT habits"), query);
    }

    while (query.next())
    {
        Habit h;
        h.id     = query.value(0).toInt();
        h.name   = query.value(1).toString();
        h.isDone = query.value(2).toBool();
        result.append(h);
    }

    return result;
}

/**
 * @brief Добавляет привычку с RETURNING id.
 * @param name Название.
 * @return Новый идентификатор.
 */
int DatabaseManager::addHabit(const QString& name)
{
    const QString trimmed = name.trimmed();
    if (trimmed.isEmpty())
        throw DatabaseException("Название привычки не может быть пустым");

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral(
        "INSERT INTO habits (name, is_done) VALUES (:name, FALSE) RETURNING id"));
    query.bindValue(QStringLiteral(":name"), trimmed);

    if (!query.exec() || !query.next())
        throwQuery(QStringLiteral("Ошибка INSERT habits"), query);

    const int newId = query.value(0).toInt();
    emit habitsChanged(QStringLiteral("added"));
    return newId;
}

/**
 * @brief Меняет название привычки.
 */
bool DatabaseManager::updateHabitName(int id, const QString& newName)
{
    const QString trimmed = newName.trimmed();
    if (trimmed.isEmpty())
        throw DatabaseException("Новое название не может быть пустым");

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral(
        "UPDATE habits SET name = :name WHERE id = :id"));
    query.bindValue(QStringLiteral(":name"), trimmed);
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec())
        throwQuery(QStringLiteral("Ошибка UPDATE habits"), query);

    const bool touched = query.numRowsAffected() > 0;
    if (touched)
        emit habitsChanged(QStringLiteral("renamed"));
    return touched;
}

/**
 * @brief Инвертирует is_done у привычки.
 */
bool DatabaseManager::toggleHabitDone(int id)
{
    QSqlQuery query(m_db);
    query.prepare(QStringLiteral(
        "UPDATE habits SET is_done = NOT is_done WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec())
        throwQuery(QStringLiteral("Ошибка UPDATE is_done"), query);

    const bool touched = query.numRowsAffected() > 0;
    if (touched)
        emit habitsChanged(QStringLiteral("toggled"));
    return touched;
}

/**
 * @brief Удаляет привычку по id.
 */
bool DatabaseManager::removeHabit(int id)
{
    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("DELETE FROM habits WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec())
        throwQuery(QStringLiteral("Ошибка DELETE habits"), query);

    const bool touched = query.numRowsAffected() > 0;
    if (touched)
        emit habitsChanged(QStringLiteral("removed"));
    return touched;
}
