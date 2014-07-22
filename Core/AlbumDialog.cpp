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

#include "AlbumDialog.h"


AlbumDialog::AlbumDialog(QSqliteCom *base, int lastAlbumId, int lastTrackId, QWidget *parent) :
    m_base(base), m_lastAlbumId(lastAlbumId), m_lastTrackId(lastTrackId), QDialog(parent)
{
    setWindowTitle(tr("Nouvel album"));

    m_wAlbum = new AlbumWidget(this);

    this->setMinimumSize(450, 340);
    this->setMaximumSize(450, 340);

    m_vlc = new QlibVLC();

    connect(m_wAlbum, SIGNAL(cancel()), this, SLOT(reject()));
    connect(m_wAlbum, SIGNAL(submit()), this, SLOT(submitSlot()));

    // Set artists of the database in the selector (QComboBox)
    std::vector<Row> result;
    result = m_base->simpleQueryResult("SELECT nom FROM artistes ORDER BY nom;");

    for(unsigned int i=1; i < result.size(); i++)
    {
        m_wAlbum->getArtistCombo()->addItem(result[i].row["nom"]);
    }
}


AlbumDialog::AlbumDialog(int id, QSqliteCom *base, int lastTrackId, QWidget *parent) :
    m_base(base), m_previousId(id), m_lastTrackId(lastTrackId), QDialog(parent)
{
    setWindowTitle(tr("Editer un album"));

    m_wAlbum = new AlbumWidget(this);

    this->setMinimumSize(450, 340);
    this->setMaximumSize(450, 340);

    m_vlc = new QlibVLC();

    // Set current values in fields:
    std::queue<QString> list; // /!\ This queue is used for another query, at down.
    list.push("INT");
    list.push(QString::number(m_previousId));

    std::vector<Row> result;
    result = m_base->preparedQueryResult("SELECT * FROM albums WHERE id=?;", list);

    m_wAlbum->setTitle(result[0].row["titre"]);
    m_wAlbum->setImagePath(result[0].row["photo"]);
    m_wAlbum->setDate(result[0].row["publication"].toInt());


    std::vector<Row> resultArtists;
    resultArtists = m_base->simpleQueryResult("SELECT id, nom FROM artistes ORDER BY nom;");

    for(unsigned int i=1; i < resultArtists.size(); i++)
    {
        m_wAlbum->getArtistCombo()->addItem(resultArtists[i].row["nom"]);
        if(resultArtists[i].row["id"] == result[0].row["artiste_id"]) {
            int foundIndex = m_wAlbum->getArtistCombo()->findText(resultArtists[i].row["nom"]);
            if(foundIndex != -1)
                m_wAlbum->getArtistCombo()->setCurrentIndex(foundIndex);
        }
    }


    // Set tracks in the QTableView:
    std::vector<Row> resultTracks;
    resultTracks = m_base->preparedQueryResult("SELECT path, titre FROM pistes WHERE album_id=? ORDER BY numero;", list);

    for(unsigned int i=0; i < resultTracks.size(); i++)
    {
        m_wAlbum->getTable()->setRowCount(m_wAlbum->getTable()->rowCount() + 1);
        //QFileInfo file(resultTracks[i].row["path"]);
        m_wAlbum->getTable()->setItem(m_wAlbum->getTable()->rowCount() - 1, 0, new QTableWidgetItem(resultTracks[i].row["titre"]));
        m_wAlbum->getTable()->setItem(m_wAlbum->getTable()->rowCount() - 1, 1, new QTableWidgetItem(resultTracks[i].row["path"]));
    }

    m_wAlbum->verifyButtonSubmit();

    connect(m_wAlbum, SIGNAL(submit()), this, SLOT(updateSlot()));
    connect(m_wAlbum, SIGNAL(cancel()), this, SLOT(reject()));
}


void AlbumDialog::submitSlot()
{
    // First: we need id of the artist
    std::queue<QString> listeArtist;
    listeArtist.push("STRING");
    listeArtist.push(m_wAlbum->getArtistCombo()->currentText());

    std::vector<Row> result;
    result = m_base->preparedQueryResult("SELECT id FROM artistes WHERE nom=?;", listeArtist);
    QString chaineIdArtist = result[0].row["id"];


    // Second: add album in database
    std::queue<QString> liste;
    liste.push("INT");
    liste.push(QString::number(m_lastAlbumId+1)); // Album id
    liste.push("STRING");
    liste.push(m_wAlbum->getTitle()); // Album title
    liste.push("INT");
    liste.push(chaineIdArtist); // Id of artist, find previously
    liste.push("INT");
    liste.push(QString::number(m_wAlbum->getDate())); // Date of publication
    if(!m_wAlbum->getImagePath().isEmpty())
    {
        liste.push("STRING");
        liste.push(m_wAlbum->getImagePath()); // Path for the picture of the album
    }
    else
        liste.push("EMPTY");
    if(!m_base->preparedQuery("INSERT INTO albums VALUES(?, ?, ?, ?, ?)", liste))
    {
        QMessageBox::critical(0, tr("Enregistrement de l'album"), tr("<strong>Erreur:</strong><br />") + m_base->lastError());
        return;
    }
    qDebug("%d", m_lastTrackId);

    // Third: add tracks in database
    for(int i = 0; i < m_wAlbum->getTable()->rowCount(); ++i)
    {
        std::queue<QString> listeTrack;
        listeTrack.push("INT");
        listeTrack.push(QString::number(m_lastTrackId+1)); // Id of the track
        listeTrack.push("STRING");
        listeTrack.push(m_wAlbum->getTable()->item(i, 0)->text()); // Title of the track
        listeTrack.push("INT");
        listeTrack.push(chaineIdArtist); // Id of the artist
        listeTrack.push("INT");
        listeTrack.push(QString::number(m_lastAlbumId+1)); // Id of the album
        listeTrack.push("INT");
        listeTrack.push(QString::number(i+1)); // Number of the track
        listeTrack.push("STRING");
        listeTrack.push(m_wAlbum->getTable()->item(i, 1)->text()); // Path of the music
        listeTrack.push("INT");
        listeTrack.push(QString::number(m_vlc->getDuration(m_wAlbum->getTable()->item(i, 1)->text()))); // Duration of music

        if(!m_base->preparedQuery("INSERT INTO pistes VALUES(?, ?, ?, ?, ?, ?, ?)", listeTrack))
        {
            QMessageBox::critical(0, tr("Enregistrement des pistes de l'album"), tr("<strong>Erreur:</strong><br />") + m_base->lastError());

            // If an insert fails, we must delete all previous tracks of this album and the album
            std::queue<QString> listeDelete;
            listeDelete.push("INT");
            listeDelete.push(QString::number(m_lastAlbumId+1)); // Id of the album
            m_base->preparedQuery("DELETE FROM albums WHERE id=?;", listeDelete);
            m_base->preparedQuery("DELETE FROM pistes WHERE album_id=?;", listeDelete);
            return;
        }
        ++m_lastTrackId;
        emit idChanged("lastTrackId", m_lastTrackId);
    }

    emit idChanged("lastAlbumId", m_lastAlbumId+1);
    emit finish();
    destroy();
}


void AlbumDialog::updateSlot()
{
    // First: we need id of the artist
    std::queue<QString> listeArtist;
    listeArtist.push("STRING");
    listeArtist.push(m_wAlbum->getArtistCombo()->currentText());

    std::vector<Row> result;
    result = m_base->preparedQueryResult("SELECT id FROM artistes WHERE nom=?;", listeArtist);
    QString chaineIdArtist = result[0].row["id"];


    // Second: update album in database
    std::queue<QString> liste;
    liste.push("STRING");
    liste.push(m_wAlbum->getTitle()); // Album title
    liste.push("INT");
    liste.push(chaineIdArtist); // Id of artist, found previously
    liste.push("INT");
    liste.push(QString::number(m_wAlbum->getDate())); // Date of publication
    if(!m_wAlbum->getImagePath().isEmpty())
    {
        liste.push("STRING");
        liste.push(m_wAlbum->getImagePath()); // Path for the picture of the album
    }
    else
        liste.push("EMPTY");
    liste.push("INT");
    liste.push(QString::number(m_previousId)); // Album id

    if(!m_base->preparedQuery("UPDATE albums SET titre=?, artiste_id=?, publication=?, photo=? WHERE id=?", liste))
    {
        QMessageBox::critical(0, tr("Edition de l'album"),
                              tr("<strong>Erreur:</strong><br />Erreur lors de l'édition de l'album.<br />") + m_base->lastError());
        return;

    }

    // Third: delete tracks in database
    std::queue<QString> listTracksDelete;
    listTracksDelete.push("INT");
    listTracksDelete.push(QString::number(m_previousId));

    if(!m_base->preparedQuery("DELETE FROM pistes WHERE album_id=?;", listTracksDelete))
    {
        QMessageBox::critical(0, tr("Edition de l'album"),
                              tr("<strong>Erreur:</strong><br />Erreur lors de la suppression des pistes de l'album.<br />") + m_base->lastError());
        return;
    }


    // Fourth: add tracks in database
    for(int i = 0; i < m_wAlbum->getTable()->rowCount(); ++i)
    {
        std::queue<QString> listeTrack;
        listeTrack.push("INT");
        listeTrack.push(QString::number(m_lastTrackId+1)); // Id of the track
        listeTrack.push("STRING");
        listeTrack.push(m_wAlbum->getTable()->item(i, 0)->text()); // Title of the track
        listeTrack.push("INT");
        listeTrack.push(chaineIdArtist); // Id of the artist
        listeTrack.push("INT");
        listeTrack.push(QString::number(m_previousId)); // Id of the album
        listeTrack.push("INT");
        listeTrack.push(QString::number(i+1)); // Number of the track
        listeTrack.push("STRING");
        listeTrack.push(m_wAlbum->getTable()->item(i, 1)->text()); // Path of the music
        listeTrack.push("INT");
        listeTrack.push(QString::number(m_vlc->getDuration(m_wAlbum->getTable()->item(i, 1)->text()))); // Duration of music



        if(!m_base->preparedQuery("INSERT INTO pistes VALUES(?, ?, ?, ?, ?, ?, ?)", listeTrack))
        {
            QMessageBox::critical(0, tr("Edition de l'album"),
                                  tr("<strong>Erreur:</strong><br />Erreur lors de l'ajout des pistes de l'album.<br />") + m_base->lastError());

            // If an insert fails, we must delete all previous tracks of this album and the album
            std::queue<QString> listeDelete;
            listeDelete.push("INT");
            listeDelete.push(QString::number(m_previousId)); // Id of the album
            m_base->preparedQuery("DELETE FROM albums WHERE id=?;", listeDelete);
            m_base->preparedQuery("DELETE FROM pistes WHERE album_id=?;", listeDelete);
            return;
        }
        ++m_lastTrackId;
        emit idChanged("lastTrackId", m_lastTrackId);
    }
    emit finish();
    destroy();
}


bool AlbumDialog::deleteAlbum(QString name, int id, QSqliteCom* base)
{
    int reponse = QMessageBox::question(0,
                                        tr("Supprimer un album"),
                                        tr("Êtes-vous sûr de vouloir supprimer l'album ") + name + " ?",
                                        QMessageBox::Yes|QMessageBox::No);

    if(reponse == QMessageBox::Yes)
    {
        QString chaineIdAlbum;
        chaineIdAlbum.sprintf("%d", id);

        std::queue<QString> listTracks;
        listTracks.push("INT");
        listTracks.push(chaineIdAlbum); // Id of the track

        if(!base->preparedQuery("DELETE FROM pistes WHERE album_id=?;", listTracks))
        {
            QMessageBox::critical(0, tr("Suppression de l'album"), tr("<strong>Erreur:</strong><br />Erreur lors de la suppression des pistes de l'album.<br />") + base->lastError());
            return true;
        }

        std::queue<QString> listAlbum;
        listAlbum.push("INT");
        listAlbum.push(chaineIdAlbum); // Id of the album

        if(!base->preparedQuery("DELETE FROM albums WHERE id=?;", listAlbum))
        {
            QMessageBox::critical(0, tr("Suppression de l'album"), tr("<strong>Erreur:</strong><br />Erreur lors de la suppression de l'album.<br />") + base->lastError());
            return true;
        }
        else
            return true;
    }
    else
        return false;
}
