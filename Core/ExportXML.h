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

#ifndef EXPORTXML_H
#define EXPORTXML_H

#include <QDialog>
#include <QFileDialog>
#include <QLabel>
#include <Qt>

#include "QSqliteCom.h"

namespace Ui
{
    class ExportXML;
}

class ExportXML : public QDialog
{
    Q_OBJECT
    
    public:
    explicit ExportXML(QSqliteCom* base, QSettings* settings, QWidget *parent = 0);
    ~ExportXML();


    public slots:
    void on_buttonPath_clicked();
    void on_buttonSave_clicked();

    
    private:
    bool exportXML();
    Ui::ExportXML *ui;
    QSqliteCom* m_base;
    QSettings* m_settings;
    QString m_path;
    int m_totalItems;
    bool m_canQuit;
    QLabel* m_label;
};

#endif // EXPORTXML_H
