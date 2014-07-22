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

#ifndef INITANALYZESELECT_H
#define INITANALYZESELECT_H

#include <QWidget>
#include <QFileDialog>
#include <QLabel>

#include "QSqliteCom.h"
#include "QlibVLC.h"

namespace Ui
{
    class InitAnalyzeSelect;
}

class InitAnalyzeSelect : public QWidget
{
    Q_OBJECT
    
    public:
    explicit InitAnalyzeSelect(QSqliteCom* base, QWidget *parent = 0);
    ~InitAnalyzeSelect();
    int countFiles(QString path, QStringList filters);
    void getFiles(QString path, QStringList filters);


    signals:
    void next(int);
    void restart();


    private slots:
    void on_buttonFolder_clicked();
    void on_buttonNext_clicked();

    
    private:
    Ui::InitAnalyzeSelect *ui;
    QString m_path;
    QLabel* m_label;
    int m_artistId;
    int m_albumId;
    int m_trackId;
    QlibVLC* m_vlc;
    QSqliteCom* m_base;
};

#endif // INITANALYZESELECT_H
