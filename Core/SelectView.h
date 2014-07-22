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

#ifndef SELECTVIEW_H
#define SELECTVIEW_H

#include <QDialog>
#include <QDir>
#include <QPluginLoader>
#include <QMessageBox>
#include <QModelIndex>
#include <QPixmap>

#include "ViewPluginInterface.h"
#include "PluginLoader.h"

namespace Ui
{
    class SelectView;
}

class SelectView : public QDialog
{
    Q_OBJECT
    

    public:
    explicit SelectView(QSettings *settings, QWidget *parent = 0);
    ~SelectView();

    private slots:
    void on_listWidget_clicked(QModelIndex modelIndex);
    void on_buttonSelect_clicked();
    

    private:
    void fillList();

    Ui::SelectView *ui;
    QSettings* m_settings;
};

#endif // SELECTVIEW_H
