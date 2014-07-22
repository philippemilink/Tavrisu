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

#include "InitAnalyzeSelect.h"
#include "ui_InitAnalyzeSelect.h"


InitAnalyzeSelect::InitAnalyzeSelect(QSqliteCom *base, QWidget *parent) :
    QWidget(parent), ui(new Ui::InitAnalyzeSelect), m_base(base)
{
    ui->setupUi(this);

    m_label = new QLabel(ui->progressBar);
    m_label->setAlignment(Qt::AlignCenter);

    m_artistId = 0;
    m_albumId = 0;
    m_trackId = 0;

    m_vlc = new QlibVLC;

    /*m_base = new QSqliteCom("temp");
    m_base->createTemp();
    if(!m_base->simpleQuery("CREATE TABLE artistes (`id` int(11), `nom` varchar(100), `type` varchar(10), `photo` text)")
            || !m_base->simpleQuery("CREATE TABLE albums (`id` int(11), `titre` varchar(100), `artiste_id` int(11), `publication` int(11), `photo` text)")
            || !m_base->simpleQuery("CREATE TABLE pistes (`id` int(11), `titre` varchar(100), `artiste_id` int(11), `album_id` int(11), `numero` int(11), `path` text, `duration` int(11))")
            || !m_base->simpleQuery("INSERT INTO artistes VALUES(0, \"\", \"\", \"\")")
            || !m_base->simpleQuery("INSERT INTO albums VALUES(0, \"\", 0, 0, \"\")"))
    {
        QMessageBox::critical(this, tr("Erreur"), tr("Une erreur est survenue lors de la création de la base de données:<br />") + m_base->lastError());
        emit restart();
        return;
    }*/
}


InitAnalyzeSelect::~InitAnalyzeSelect()
{
    delete ui;
}


void InitAnalyzeSelect::on_buttonFolder_clicked()
{
    QString selectedPath = QFileDialog::getExistingDirectory(this, tr("Sélectionnez le dossier"), QDir::homePath());
    if(!selectedPath.isEmpty())
    {
        m_path = selectedPath;
        ui->lineEditFolder->setText(m_path);
        ui->buttonNext->setEnabled(true);
    }
}


int InitAnalyzeSelect::countFiles(QString path, QStringList filters)
{
    QDir* dir = new QDir(path);

    int number(0);

    QStringList files = dir->entryList(filters);
    QStringList::Iterator it = files.begin();
    while(it != files.end())
    {
        number++;
        ++it;
    }
    return number;
}


void InitAnalyzeSelect::getFiles(QString path, QStringList filters)
{
    QDir* dir = new QDir(path);
    QFileInfo file;
    int artistId = 0, albumId = 0;

    QStringList files = dir->entryList(filters);
    QStringList::Iterator it = files.begin();
    while(it != files.end())
    {
        ui->progressBar->setValue(ui->progressBar->value()+1);
        m_label->setText(*it);

        m_trackId++;
        artistId = 0;
        albumId = 0;

        file = QFileInfo(path + "/" + *it);

        m_vlc->ejectMedia();
        m_vlc->setMedia(file.absoluteFilePath());

        if(!m_vlc->artist().isEmpty())
        {
            if(m_base->isset("SELECT * FROM artistes WHERE nom=:value", m_vlc->artist()))
            {
                std::queue<QString> list;
                list.push("STRING");
                list.push(m_vlc->artist());
                std::vector<Row> result;
                result = m_base->preparedQueryResult("SELECT id FROM artistes WHERE nom=?;", list);
                artistId = result[0].row["id"].toInt();
            }
            else
            {
                m_artistId++;
                std::queue<QString> liste;
                liste.push("INT");
                liste.push(QString::number(m_artistId));
                liste.push("STRING");
                liste.push(m_vlc->artist());
                liste.push("EMPTY");
                liste.push("EMPTY");
                m_base->preparedQuery("INSERT INTO artistes VALUES(?, ?, ?, ?)", liste);
                artistId = m_artistId;
            }

            if(!m_vlc->album().isEmpty())
            {
                std::queue<QString> issetAlbumList;
                issetAlbumList.push("INT");
                issetAlbumList.push(QString::number(artistId));
                issetAlbumList.push("STRING");
                issetAlbumList.push(m_vlc->album()); // Album title
                if(m_base->isset("SELECT * FROM albums WHERE artiste_id=? AND titre=?", issetAlbumList))
                {
                    std::queue<QString> list;
                    list.push("INT");
                    list.push(QString::number(artistId));
                    list.push("STRING");
                    list.push(m_vlc->album());
                    std::vector<Row> result;
                    result = m_base->preparedQueryResult("SELECT * FROM albums WHERE artiste_id=? AND titre=?;", list);
                    albumId = result[0].row["id"].toInt();
                }
                else
                {
                    m_albumId++;
                    std::queue<QString> liste;
                    liste.push("INT");
                    liste.push(QString::number(m_albumId)); // Album id
                    liste.push("STRING");
                    liste.push(m_vlc->album()); // Album title
                    liste.push("INT");
                    liste.push(QString::number(artistId)); // Id of artist, find previously
                    if(m_vlc->year() == 0)
                        liste.push("EMPTY");
                    else
                    {
                        liste.push("INT");
                        liste.push(QString::number(m_vlc->year())); // Date of publication
                    }
                    liste.push("EMPTY");
                    m_base->preparedQuery("INSERT INTO albums VALUES(?, ?, ?, ?, ?)", liste);
                    albumId = m_albumId;
                }
            }
        }

        std::queue<QString> listeTrack;
        listeTrack.push("INT");
        listeTrack.push(QString::number(m_trackId)); // Id of the track
        listeTrack.push("STRING");
        listeTrack.push(file.completeBaseName()); // Title of the track
        listeTrack.push("INT");
        listeTrack.push(QString::number(artistId)); // Id of the artist
        listeTrack.push("INT"); // Id of the album
        listeTrack.push(QString::number(albumId));
        if(m_vlc->trackNumber() == -1)
            listeTrack.push("EMPTY"); // Number of the track
        else
        {
            listeTrack.push("INT"); // Id of the album
            listeTrack.push(QString::number(m_vlc->trackNumber()));
        }
        listeTrack.push("STRING");
        listeTrack.push(file.absoluteFilePath()); // Path of the music
        listeTrack.push("INT");
        listeTrack.push(QString::number(m_vlc->duration())); // Duration of music

        m_base->preparedQuery("INSERT INTO pistes VALUES(?, ?, ?, ?, ?, ?, ?)", listeTrack);

        ++it;
    }
}


void InitAnalyzeSelect::on_buttonNext_clicked()
{
    QStringList filters;
    filters << "*.mp3" << "*.wav" << "*.ogg" << "*.flac" << "*.acc";
    int number(0);

    number = countFiles(m_path, filters);
    QDirIterator* dirIt = new QDirIterator(m_path, QDir::Dirs|QDir::NoDotDot|QDir::NoDot, QDirIterator::Subdirectories);
    while(dirIt->hasNext())
    {
        number = number + countFiles(dirIt->next(), filters);
    }

    if(number == 0)
    {
        QMessageBox::information(this, tr("Erreur"), tr("Il n'y a pas de fichiers audio dans ce dossier."));
        return;
    }

    ui->progressBar->setMaximum(number);
    m_label->setGeometry(0, 0, ui->progressBar->width(), ui->progressBar->height());


    getFiles(m_path, filters);
    dirIt = new QDirIterator(m_path, QDir::Dirs|QDir::NoDotDot|QDir::NoDot, QDirIterator::Subdirectories);
    while(dirIt->hasNext())
    {
        getFiles(dirIt->next(), filters);
    }

    emit next(m_trackId);
}
