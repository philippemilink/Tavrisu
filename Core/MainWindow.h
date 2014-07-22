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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>

#include "AboutDialog.h"
#include "QSqliteCom.h"
#include "FirstUse.h"
#include "NewArtist.h"
#include "NewMusic.h"
#include "SelectView.h"
#include "QlibVLC.h"
#include "ExportXML.h"
#include "InitializeDatabase.h"
#include "InitSelectAction.h"
#include "AlbumDialog.h"
#include "Updater.h"
#include "Settings.h"


namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    private slots:
    void newArtist();
    void newAlbum();
    void newMusic();
    void showSelectView();
    void changeLastId(QString key, int newId);
    void loadInterface();
    void loadPlugin();
    void deleteTrack(int id, QString name);
    void deleteAlbum(int id, QString name);
    void deleteArtist(int id, QString name);
    void editArtist(int id);
    void editTrack(int id);
    void editAlbum(int id);
    void play(QString path);
    void pause();
    void exportDatabase();
    void setMainError(QString text);
    void lauchUpdater();
    void searchNewVersion();
    void lauchSettings();

    
    private:
    Ui::MainWindow *ui;
    AboutDialog m_aboutDialog;
    QSqliteCom* m_baseFile;
    QSqliteCom* m_base;
    QSettings* m_settings;
    ViewPluginInterface* m_plugin;
    QlibVLC* m_vlc;
    QAction* m_actionNewArtist;
    QAction* m_actionNewAlbum;
    QAction* m_actionNewMusic;
    QAction* m_actionExportDatabase;
    QAction* m_actionAffichage;
    QAction* m_actionUpdater;
    QAction* m_actionSettings;
    Updater* m_updater;
};

#endif // MAINWINDOW_H
