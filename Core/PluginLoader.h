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

#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QList>
#include <QPluginLoader>
#include <QObject>
#include <QString>
#include <QDir>
#include <QObject>

namespace PluginLoader
{
    // C'est une méthode qui demande un argument template pour éviter de créer un chargeur par type de plugin.
    template<typename T> T* pluginByName(const QString& fileName)
    {
        QPluginLoader loader(fileName); // On charge le plugin.
        QObject *plugin = loader.instance(); // On prend l'instance.
        return qobject_cast<T*>(plugin); // Et on retourne le plugin casté.
    }

    // Toujours une méthode avec un argument template pour éviter qu'il y en ait 50.
    template<typename T> QList<T*> pluginByDir(const QString& dir)
    {
        QList<T*> ls; // On crée la liste où seront mis tous les plugins valides.
        QDir plugDir = QDir(dir);
        foreach(QString file, plugDir.entryList(QDir::Files))
        {
            // On prend la liste des fichiers.
            if(T* plugin = PluginLoader::pluginByName<T>(plugDir.absoluteFilePath(file)))
                // On vérifie si le plugin existe.
                ls.push_back(plugin); // On l'ajoute à la liste si oui.
        }
        return ls;
    }
}

#endif // PLUGINLOADER_H
