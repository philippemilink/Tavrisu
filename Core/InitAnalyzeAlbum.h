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

#ifndef INITANALYZEALBUM_H
#define INITANALYZEALBUM_H

#include <QWidget>
#include <QStandardItemModel>

#include "QSqliteCom.h"
#include "AlbumWidget.h"
#include "QlibVLC.h"


namespace Ui
{
    class InitAnalyzeAlbum;
}


class InitAnalyzeAlbum : public QWidget
{
    Q_OBJECT
    

    public:
    explicit InitAnalyzeAlbum(QSqliteCom* base, int lastTrackId, QWidget *parent = 0);
    ~InitAnalyzeAlbum();


    private slots:
    void on_table_clicked(QModelIndex index);
    void on_checkDeleteAlbum_clicked(bool checked);
    void submitSlot();
    void on_buttonNext_clicked();
    void resizeEvent(QResizeEvent *event);


    signals:
    void next(bool, int);
    

    private:
    void fillList();

    Ui::InitAnalyzeAlbum *ui;
    QSqliteCom* m_base;
    AlbumWidget* m_wAlbum;
    QStandardItemModel* m_model;
    int m_selectedRow;
    int m_lastTrackId;
    QlibVLC* m_vlc;
};

#endif // INITANALYZEALBUM_H
