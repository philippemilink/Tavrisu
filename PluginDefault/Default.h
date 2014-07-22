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

#ifndef DEFAULT_H
#define DEFAULT_H

#include "../Core/ViewPluginInterface.h"
#include "../Core/QSqliteCom.h"

#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QPushButton>
#include <QMenu>

#include "Player.h"

#define PLUGIN_PATH QString(qApp->applicationDirPath() + "/views/Default/")

class Default : public ViewPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(ViewPluginInterface)
    //Q_PLUGIN_METADATA(IID "Tavrisu.ViewPluginInterface")

    public:
    Default();
    virtual ~Default();
    virtual void setBase(QSqliteCom* base);
    virtual QString getName();
    virtual QString getCreator();
    virtual QString getVersion();
    virtual QString getDescription();
    virtual QString getDescriptionImage();
    void fillList();
    void resizeEvent(QResizeEvent* event);


    public slots:
    virtual void updateList();
    virtual void positionChanged(int position);
    virtual void endTrack();
    void doubleClickSlot(QModelIndex index);
    void contextualMenu(QPoint point);
    void trackDeleteSlot();
    void albumDeleteSlot();
    void artistDeleteSlot();
    void artistEditSlot();
    void trackEditSlot();
    void albumEditSlot();
    void nextSlot();
    void previousSlot();
    void pauseSlot();
    void emptyPlay();
    void selectRow();


    private:
    QTableView* m_listView;
    QStandardItemModel* m_model;
    Player* m_player;
    QModelIndex m_indexSelection;
    int m_selectedRow;
    int m_currentId;
    QColor m_bgColor;
};

#endif // DEFAULT_H
