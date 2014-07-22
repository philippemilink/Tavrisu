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

#ifndef INITANALYZEARTIST_H
#define INITANALYZEARTIST_H

#include <QWidget>
#include <QStandardItemModel>
#include <QMessageBox>

#include "QSqliteCom.h"
#include "ButtonImage.h"

namespace Ui
{
    class InitAnalyzeArtist;
}

class InitAnalyzeArtist : public QWidget
{
    Q_OBJECT

    
    public:
    explicit InitAnalyzeArtist(QSqliteCom* base, QWidget *parent = 0);
    ~InitAnalyzeArtist();
    void fillList();
    void resizeEvent(QResizeEvent *);


    signals:
    void next();


    private slots:
    void on_table_clicked(QModelIndex index);
    void on_checkDelete_clicked(bool checked);
    void on_buttonSave_clicked();
    void on_lineEdit_textChanged(QString text);
    void on_buttonValid_clicked();

    
    private:
    Ui::InitAnalyzeArtist *ui;
    QSqliteCom* m_base;
    QStandardItemModel* m_model;
    ButtonImage* m_buttonImage;
    int m_selectedRow;
};

#endif // INITANALYZEArtist_H
