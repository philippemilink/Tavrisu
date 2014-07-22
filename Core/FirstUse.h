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

#ifndef FIRSTUSE_H
#define FIRSTUSE_H

#include <QWizard>
#include <QWizardPage>
#include <QFont>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QSettings>

#include "QSqliteCom.h"


class FirstUse : public QObject
{
    Q_OBJECT

    public:
    FirstUse(QWidget* parent, QSqliteCom* base, QSettings* settings);


    signals:
    void cancel();
    void finish();


    private slots:
    void openDialogSelectFolder();
    void createDatabase();


    private:
    void saveNewPath();

    QWizard* m_wizard;
    QWizardPage* m_pageWelcome;
    QWizardPage* m_pageLicense;
    QWizardPage* m_pageSelectFolder;
    QString m_path;
    QLineEdit* m_lineEdit;
    QSqliteCom* m_base;
    QSettings* m_settings;
};

#endif // FISRTUSE_H
