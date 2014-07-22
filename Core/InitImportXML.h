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

#ifndef INITIMPORTXML_H
#define INITIMPORTXML_H

#include <QWidget>
#include <QFileDialog>
#include <QDir>
#include <QtXml>
#include <QMessageBox>
#include <QLabel>

#include "QSqliteCom.h"

namespace Ui
{
    class InitImportXML;
}

class InitImportXML : public QWidget
{
    Q_OBJECT
    
    public:
    explicit InitImportXML(QSqliteCom* base, QSettings* settings, QWidget *parent = 0);
    ~InitImportXML();


    public slots:
    void on_buttonFile_clicked();
    void on_buttonNext_clicked();


    signals:
    void restart();
    void end();

    
    private:
    void parseXML(QDomNode node);
    Ui::InitImportXML *ui;
    QString m_path;
    QString m_parentNode;
    QDomElement m_domElement;
    QSqliteCom* m_base;
    QSettings* m_settings;
    QLabel* m_label;
};

#endif // INITIMPORTXML_H
