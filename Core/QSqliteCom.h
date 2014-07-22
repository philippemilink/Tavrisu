/*
 * Tavrisu, an easy and modulable audio player.
    Copyright (C) 2014 - Philippe SWARTVAGHER

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QSQLITECOM_H
#define QSQLITECOM_H
#include <vector>
using namespace std;

#include <QObject>
#include <QFile>
#include <queue>
#include <QMessageBox>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>


struct Row
{
    std::map<QString, QString> row;
};


class QSqliteCom
{
    public:
    QSqliteCom(QString name);
    bool open(QString path);
    bool create(QString path);
    bool createTemp();
    QString lastError();
    bool simpleQuery(QString query);
    bool preparedQuery(QString query, std::queue<QString> liste);
    bool isset(QString sql, QString value);
    int isset(QString sql, std::queue<QString> liste);
    std::vector<Row> simpleQueryResult(QString sql);
    std::vector<Row> preparedQueryResult(QString query, std::queue<QString> liste);
    int countEntries(QString table);
    void close();

    private:
    QString m_name;
    QString m_path;
    QString m_lastError;
    QSqlDatabase m_db;
};



#endif // QSQLITECOM_H
