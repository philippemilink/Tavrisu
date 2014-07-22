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

#ifndef VIEWPLUGININTERFACE_H
#define VIEWPLUGININTERFACE_H

#include <QtPlugin>
#include <QWidget>

#include "QSqliteCom.h"

class ViewPluginInterface : public QWidget
{
    Q_OBJECT

    public:
    virtual ~ViewPluginInterface() {}
    virtual void setBase(QSqliteCom* base) = 0;
    virtual QString getName() = 0;
    virtual QString getCreator() = 0;
    virtual QString getVersion() = 0;
    virtual QString getDescription() = 0;
    virtual QString getDescriptionImage() = 0;



    public slots:
    virtual void updateList() = 0;
    virtual void positionChanged(int position) = 0;
    virtual void endTrack() = 0;


    signals:
    void trackEdit(int id);
    void trackDelete(int id, QString name);
    void albumEdit(int id);
    void albumDelete(int id, QString name);
    void artistEdit(int id);
    void artistDelete(int id, QString name);
    void play(QString path);
    void pause();
    void changePosition(int position);



    protected:
    QString m_name;
    QString m_creator;
    QString m_version;
    QString m_description;
    QString m_descriptionImagePath;
    QSqliteCom* m_base;
};

Q_DECLARE_INTERFACE(ViewPluginInterface, "Tavrisu.ViewPluginInterface")

#endif // VIEWPLUGININTERFACE_H
