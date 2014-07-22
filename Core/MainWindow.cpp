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

#include "MainWindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_base = new QSqliteCom("File");
    m_vlc = new QlibVLC();
    m_updater = new Updater();


    // Initialize the menubar:
    QMenu* menuFile = menuBar()->addMenu(tr("&Fichier"));

    m_actionNewArtist = menuFile->addAction(tr("Ajouter un artiste"));
    m_actionNewArtist->setEnabled(false);
    connect(m_actionNewArtist, SIGNAL(triggered()), this, SLOT(newArtist()));

    m_actionNewAlbum = menuFile->addAction(tr("Ajouter un album"));
    m_actionNewAlbum->setEnabled(false);
    connect(m_actionNewAlbum, SIGNAL(triggered()), this, SLOT(newAlbum()));

    m_actionNewMusic = menuFile->addAction(tr("Ajouter un fichier audio"));
    m_actionNewMusic->setEnabled(false);
    connect(m_actionNewMusic, SIGNAL(triggered()), this, SLOT(newMusic()));

    menuFile->addSeparator();

    m_actionExportDatabase = menuFile->addAction(tr("Exporter la base de données"));
    m_actionExportDatabase->setEnabled(false);
    connect(m_actionExportDatabase, SIGNAL(triggered()), this, SLOT(exportDatabase()));

    menuFile->addSeparator();

    QAction* actionQuit = menuFile->addAction(tr("Quitter"));
    connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

    QMenu* menuOptions = menuBar()->addMenu(tr("Options"));
    m_actionAffichage = menuOptions->addAction(tr("Choisir l'affichage"));
    m_actionAffichage->setEnabled(false);
    connect(m_actionAffichage, SIGNAL(triggered()), this, SLOT(showSelectView()));
    m_actionSettings = menuOptions->addAction(tr("Paramètres"));
    m_actionSettings->setEnabled(false);
    connect(m_actionSettings, SIGNAL(triggered()), this, SLOT(lauchSettings()));

    QMenu* menuAbout = menuBar()->addMenu(tr("&?"));
    m_actionUpdater = menuAbout->addAction(tr("Chercher des mises à jour"));
    m_actionUpdater->setEnabled(false);
    connect(m_actionUpdater, SIGNAL(triggered()), this, SLOT(lauchUpdater()));
    QAction* actionAbout = menuAbout->addAction(tr("A propos..."));
    connect(actionAbout, SIGNAL(triggered()), &m_aboutDialog, SLOT(show()));


    // Initialize settings, saved in the register (in Windows)
    m_settings = new QSettings("Tavrisu", "Tavrisu");

    if(m_settings->value("Version", 24) != 24)
        m_settings->setValue("Version", "0.1b1");

    if(m_settings->value("FirstUse", 24) == 24) // It is the first use of Tavrisu: starting of wizard to create the database file
    {
        FirstUse* utilisation = new FirstUse(this, m_base, m_settings);
        connect(utilisation, SIGNAL(cancel()), qApp, SLOT(quit()));
        connect(utilisation, SIGNAL(finish()), this, SLOT(loadInterface()));
    }
    else
    {
       // Open the database
       if(!m_base->open(m_settings->value("Chemin").toString()))
            setMainError(tr("<strong>Erreur</strong> lors de l'ouverture de la base de données:<br />") + m_base->lastError());
       else
       {
            loadInterface();

            if(m_settings->value("searchUpdate").toBool())
                QTimer::singleShot(700, this, SLOT(searchNewVersion()));
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::loadInterface()
{
    if(m_base->countEntries("pistes") >= 1)
    {
        // Load interface plugin:
        loadPlugin();
    }
    else
    {
        if(m_base->countEntries("pistes") == -1)
        {
            setMainError(tr("Erreur lors de la lecture de la base de données.<br />Redémarrez Tavrisu pour essayer de résoudre le problème."));
            return;
        }
        InitializeDatabase* initDB = new InitializeDatabase(this, m_base, m_settings);
        connect(initDB, SIGNAL(end()), this, SLOT(loadPlugin()));
        connect(initDB, SIGNAL(idChanged(QString, int)), this, SLOT(changeLastId(QString, int)));
    }
}


void MainWindow::loadPlugin()
{
    QString pluginPath = qApp->applicationDirPath() + "/views/" + m_settings->value("interface").toString() + "/" + m_settings->value("interface").toString() + ".dll";
    m_plugin = PluginLoader::pluginByName<ViewPluginInterface>(pluginPath);
    if(m_plugin == NULL)
    {
        setMainError(tr("Erreur lors du chargement de l'interface.<br />Redémarrez Tavrisu pour essayer de résoudre le problème."));
        return;
    }
    m_plugin->setBase(m_base);

    connect(m_plugin, SIGNAL(trackDelete(int, QString)), this, SLOT(deleteTrack(int, QString)));
    connect(m_plugin, SIGNAL(albumDelete(int, QString)), this, SLOT(deleteAlbum(int, QString)));
    connect(m_plugin, SIGNAL(artistDelete(int, QString)), this, SLOT(deleteArtist(int, QString)));
    connect(m_plugin, SIGNAL(artistEdit(int)), this, SLOT(editArtist(int)));
    connect(m_plugin, SIGNAL(trackEdit(int)), this, SLOT(editTrack(int)));
    connect(m_plugin, SIGNAL(albumEdit(int)), this, SLOT(editAlbum(int)));
    connect(m_plugin, SIGNAL(play(QString)), this, SLOT(play(QString)));
    connect(m_plugin, SIGNAL(pause()), this, SLOT(pause()));
    connect(m_plugin, SIGNAL(changePosition(int)), m_vlc, SLOT(changePosition(int)));
    connect(m_vlc, SIGNAL(positionChanged(int)), m_plugin, SLOT(positionChanged(int)));
    connect(m_vlc, SIGNAL(endTrack()), m_plugin, SLOT(endTrack()));

    setCentralWidget(m_plugin);

    m_actionNewArtist->setEnabled(true);
    m_actionNewAlbum->setEnabled(true);
    m_actionNewMusic->setEnabled(true);
    m_actionExportDatabase->setEnabled(true);
    m_actionAffichage->setEnabled(true);
    m_actionUpdater->setEnabled(true);
    m_actionSettings->setEnabled(true);
}


void MainWindow::newArtist()
{
    NewArtist* fenetre = new NewArtist(m_base, m_settings->value("lastArtistId").toInt(), this);
    connect(fenetre, SIGNAL(idChanged(QString, int)), this, SLOT(changeLastId(QString, int)));
    fenetre->show();
}


void MainWindow::changeLastId(QString key, int newId)
{
    m_settings->setValue(key, newId);
}


void MainWindow::newAlbum()
{
    AlbumDialog* fenetre = new AlbumDialog(m_base, m_settings->value("lastAlbumId").toInt(), m_settings->value("lastTrackId").toInt(), this);
    connect(fenetre, SIGNAL(idChanged(QString, int)), this, SLOT(changeLastId(QString, int)));
    connect(fenetre, SIGNAL(finish()), m_plugin, SLOT(updateList()));
    fenetre->show();
}


void MainWindow::showSelectView()
{
    SelectView* selectView = new SelectView(m_settings, this);
    selectView->show();
}


void MainWindow::newMusic()
{
    NewMusic* fenetre = new NewMusic(m_base, m_settings, this);
    connect(fenetre, SIGNAL(finish()), m_plugin, SLOT(updateList()));
    fenetre->start();
}


void MainWindow::deleteTrack(int id, QString name)
{
    if(NewMusic::deleteMusic(name, id, m_base))
        m_plugin->updateList();
}


void MainWindow::editTrack(int id)
{
    NewMusic* fenetre = new NewMusic(id, m_base, m_settings, this);
    connect(fenetre, SIGNAL(finish()), m_plugin, SLOT(updateList()));
    fenetre->show();
}


void MainWindow::deleteAlbum(int id, QString name)
{
    if(AlbumDialog::deleteAlbum(name, id, m_base))
        m_plugin->updateList();
}


void MainWindow::editAlbum(int id)
{
    AlbumDialog* fenetre = new AlbumDialog(id, m_base, m_settings->value("lastTrackId").toInt(), this);
    connect(fenetre, SIGNAL(finish()), m_plugin, SLOT(updateList()));
    connect(fenetre, SIGNAL(idChanged(QString, int)), this, SLOT(changeLastId(QString, int)));
    fenetre->show();
}


void MainWindow::deleteArtist(int id, QString name)
{
    if(NewArtist::deleteArtist(name, id, m_base))
        m_plugin->updateList();
}


void MainWindow::editArtist(int id)
{
    NewArtist* fenetre = new NewArtist(id, m_base, this);
    connect(fenetre, SIGNAL(finish()), m_plugin, SLOT(updateList()));
    fenetre->show();
}


void MainWindow::play(QString path)
{
    m_vlc->ejectMedia();
    m_vlc->setMedia(path);
    m_vlc->play();
}


void MainWindow::pause()
{
    if(m_vlc->isPlaying())
        m_vlc->pause(true);
    else
        m_vlc->pause(false);
}


void MainWindow::exportDatabase()
{
    ExportXML* exportXml = new ExportXML(m_base, m_settings, this);
    exportXml->show();
}


void MainWindow::setMainError(QString text)
{
    QLabel* label = new QLabel(text);
    label->setAlignment(Qt::AlignHCenter|Qt::AlignCenter);
    setCentralWidget(label);
}


void MainWindow::lauchUpdater()
{
    connect(m_updater, SIGNAL(exportDatabase()), this, SLOT(exportDatabase()));
    m_updater->display();
}


void MainWindow::searchNewVersion()
{
    m_updater->searchUpdate();
    if(m_updater->existsNewVersion())
    {
        int answer = QMessageBox::question(this,
                                           tr("Nouvelle version disponible"),
                                           tr("Une nouvelle version de Tavrisu est disponible.<br />Souhaitez-vous la télécharger ?"),
                                           QMessageBox::Yes, QMessageBox::No);
        if(answer == QMessageBox::Yes)
            lauchUpdater();
    }
}


void MainWindow::lauchSettings()
{
    Settings* settings = new Settings(m_settings);
    settings->show();
}


