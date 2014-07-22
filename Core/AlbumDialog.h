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

#ifndef ALBUMDIALOG_H
#define ALBUMDIALOG_H

#include <QDialog>

#include "QSqliteCom.h"
#include "QlibVLC.h"
#include "AlbumWidget.h"


class AlbumDialog : public QDialog
{
    Q_OBJECT

    public:
    explicit AlbumDialog(QSqliteCom *base, int lastAlbumId, int lastTrackId, QWidget *parent = 0);
    explicit AlbumDialog(int id, QSqliteCom *base, int lastTrackId, QWidget *parent = 0); // To edit an album
    static bool deleteAlbum(QString name, int id, QSqliteCom* base);
    

    signals:
    void idChanged(QString, int);
    void finish();


    private slots:
    void submitSlot();
    void updateSlot();


    private:
    AlbumWidget* m_wAlbum;
    QSqliteCom* m_base;
    int m_lastAlbumId;
    int m_lastTrackId;
    QlibVLC* m_vlc;
    int m_previousId;
};

#endif // ALBUMDIALOG_H
