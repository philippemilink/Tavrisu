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

#ifndef INITIALIZEDATABASE_H
#define INITIALIZEDATABASE_H

#include <QMainWindow>

#include "InitSelectAction.h"
#include "InitImportXML.h"
#include "InitAnalyzeSelect.h"
#include "InitAnalyzeArtist.h"
#include "InitAnalyzeAlbum.h"
#include "QSqliteCom.h"

class InitializeDatabase : public QObject
{
    Q_OBJECT

    public:
    explicit InitializeDatabase(QMainWindow *parent, QSqliteCom* base, QSettings* settings);

    
    signals:
    void end();
    void idChanged(QString, int);

    
    public slots:
    void nothingSlot();
    void importXmlSlot();
    void beginSlot();
    void selectFolderSlot();
    void analyzeFolderArtist(int lastTrackId);
    void analyzeFolderAlbum();
    void endSlot(bool deleteTracks, int lastTrackId);


    private:
    QMainWindow* m_mainWindow;
    InitImportXML* m_importXML;
    QSqliteCom* m_base;
    QSettings* m_settings;
    int m_lastTrackId;
};

#endif // INITIALIZEDATABASE_H
