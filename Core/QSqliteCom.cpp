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

#include "QSqliteCom.h"

using namespace std;

QSqliteCom::QSqliteCom(QString name) : m_name(name)
{

}


bool QSqliteCom::create(QString path)
{
    m_path = path;

    if(QFile::exists(m_path)) // If the file already exists: an error is created.
    {
        m_lastError = QObject::tr("Un fichier Tavrisu existe déjà dans ce dossier.");
        return false;
    }
    else // If it doesn't exist a file with the same name
    {
        QFile* base = new QFile(m_path);
        if(base->open(QIODevice::ReadWrite)) // If the creation works successfully
        {
            base->close();
            return true;
        }
        else // If the creation fails
        {
            m_lastError = QObject::tr("Une erreur s'est produite lors de la création du fichier de base de données.");
            return false;
        }
    }
}


bool QSqliteCom::createTemp()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", m_name);
    m_db.setDatabaseName(":memory:");
    if(m_db.open()) // If the database opens itself successfully
        return true;
    else
    {
        m_lastError = QObject::tr("Une erreur s'est produite lors de la création de la base de données.");
        return false;
    }
}


bool QSqliteCom::open(QString path)
{
    if(QFile::exists(path)) // If the file already exists: an error is created.
    {
        m_path = path;
        m_db = QSqlDatabase::addDatabase("QSQLITE", m_name);
        m_db.setDatabaseName(m_path);
        if(m_db.open()) // If the database opens itself successfully
        {
            if(simpleQuery("PRAGMA synchronous=OFF"))
                return true;
            else
                return false;
        }
        else
        {
            m_lastError = QObject::tr("Une erreur s'est produite lors de l'ouverture du fichier de base de données: l'ouverture a échoué.");
            return false;
        }
    }
    else
    {
        m_lastError = QObject::tr("Une erreur s'est produite lors de l'ouverture du fichier de base de données: le fichier n'existe pas.");
        return false;
    }
}


QString QSqliteCom::lastError()
{
    QString returned(m_lastError);
    m_lastError = "";
    return returned;
}


bool QSqliteCom::simpleQuery(QString query)
{
    QSqlQuery sqlQuery(m_db);
    if(!sqlQuery.exec(query))
    {
        m_lastError = QObject::tr("Une erreur s'est produite lors de la requête.") + sqlQuery.lastError().text();
        return false;
    }
    else
        return true;
}


bool QSqliteCom::preparedQuery(QString query, queue<QString> liste)
{
    int taille(liste.size());
    int i(0);
    int numeroParameter(1);
    QSqlQuery sqlQuery(m_db);

    if(!sqlQuery.prepare(query))
    {
        m_lastError = "Une erreur s'est produite lors de l'enregistrement.<br />" + sqlQuery.lastError().text();
        return false;
    }
    else
    {
        while(i < taille)
        {
            if(liste.front() == "STRING")
            {
                liste.pop();
                sqlQuery.addBindValue(liste.front());
                liste.pop();
                i = i+2;
            }
            else if(liste.front() == "INT")
            {
                liste.pop();
                sqlQuery.addBindValue(QString(liste.front()).toInt());
                liste.pop();
                i = i+2;
            }
            else // liste.front() == "EMPTY"
            {
                liste.pop();
                sqlQuery.addBindValue("");
                ++i;
            }
            ++numeroParameter;
        }

        if(sqlQuery.exec())
            return true;
        else
        {
            m_lastError = "Une erreur s'est produite lors de l'enregistrement.<br />" + sqlQuery.lastError().text();
            return false;
        }
    }
}


bool QSqliteCom::isset(QString sql, QString value)
{
    QSqlQuery sqlQuery(m_db);
    bool exist(false);


    sqlQuery.prepare(sql);
    sqlQuery.bindValue(":value", value);

    if(sqlQuery.exec())
    {
        int i = 0;
        while(sqlQuery.next())
            i++;
        if(i > 0)
            exist = true;
    }

    return exist;
}


int QSqliteCom::isset(QString sql, std::queue<QString> liste)
{
    int taille(liste.size());
    int i(0);
    QSqlQuery sqlQuery(m_db);
    bool exist(false);

    if(!sqlQuery.prepare(sql))
        m_lastError = "Une erreur s'est produite lors de l'enregistrement, pendant l'initiation de la requête.<br />" + sqlQuery.lastError().text();
    else
    {
        while(i < taille)
        {
            if(liste.front() == "STRING")
            {
                liste.pop();
                sqlQuery.addBindValue(liste.front());
                liste.pop();
                i = i+2;
            }
            else if(liste.front() == "INT")
            {
                liste.pop();
                sqlQuery.addBindValue(QString(liste.front()).toInt());
                liste.pop();
                i = i+2;
            }
            else // liste.front() == "EMPTY"
            {
                liste.pop();
                sqlQuery.addBindValue("");
                ++i;
            }
        }

        if(sqlQuery.exec())
        {
            int j = 0;
            while(sqlQuery.next())
                j++;
            if(j > 0)
                exist = true;
        }
        else
            m_lastError = "Une erreur s'est produite lors de l'enregistrement, pendant l'exécution de la requête.<br />" + sqlQuery.lastError().text() + sqlQuery.executedQuery() + sqlQuery.boundValue(1).toString();
    }
    return exist;
}


vector<Row> QSqliteCom::simpleQueryResult(QString sql)
{
    QSqlQuery sqlQuery(m_db);
    std::vector<Row> result;

    if(sqlQuery.exec(sql))
    {
        while(sqlQuery.next())
        {
            Row ligne;
            for(int x = 0; x < sqlQuery.record().count(); ++x)
            {
                ligne.row[sqlQuery.record().fieldName(x)] = sqlQuery.value(x).toString();
            }
            result.push_back(ligne);
        }
    }

    return result;
}


vector<Row> QSqliteCom::preparedQueryResult(QString query, std::queue<QString> liste)
{
    int taille(liste.size());
    int i(0);
    QSqlQuery sqlQuery(m_db);
    std::vector<Row> result;

    if(!sqlQuery.prepare(query))
    {
        m_lastError = "Une erreur s'est produite lors de l'enregistrement, pendant l'initiation de la requête.<br />" + sqlQuery.lastError().text();
    }
    else
    {
        while(i < taille)
        {
            if(liste.front() == "STRING")
            {
                liste.pop();
                sqlQuery.addBindValue(liste.front());
                liste.pop();
                i = i+2;
            }
            else if(liste.front() == "INT")
            {
                liste.pop();
                sqlQuery.addBindValue(QString(liste.front()).toInt());
                liste.pop();
                i = i+2;
            }
            else // liste.front() == "EMPTY"
            {
                liste.pop();
                sqlQuery.addBindValue("");
                ++i;
            }
        }

        if(sqlQuery.exec())
        {
            while(sqlQuery.next())
            {
                Row ligne;
                for(int x = 0; x < sqlQuery.record().count(); ++x)
                {
                    ligne.row[sqlQuery.record().fieldName(x)] = sqlQuery.value(x).toString();
                }
                result.push_back(ligne);
            }
        }
        else
            m_lastError = "Une erreur s'est produite lors de l'enregistrement, pendant l'exécution de la requête.<br />" + sqlQuery.lastError().text() + sqlQuery.executedQuery() + sqlQuery.boundValue(1).toString();
    }
    return result;
}


int QSqliteCom::countEntries(QString table)
{
    QSqlQuery sqlQuery(m_db);
    int items(-1);

    if(sqlQuery.exec("SELECT * FROM " + table + ";"))
    {
        items = 0;
        while(sqlQuery.next())
            items++;
    }
    return items;
}


void QSqliteCom::close()
{
    m_db.close();
    QSqlDatabase::removeDatabase(m_name);
}

