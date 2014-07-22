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

#include "InitializeDatabase.h"


InitializeDatabase::InitializeDatabase(QMainWindow *parent, QSqliteCom* base, QSettings *settings) :
    m_mainWindow(parent), m_base(base), m_settings(settings)
{
    beginSlot();
}


void InitializeDatabase::nothingSlot()
{
    emit idChanged("lastTrackId", 0);
    emit idChanged("lastAlbumId", 0);
    emit idChanged("lastArtistId", 0);
    emit end();
}


void InitializeDatabase::importXmlSlot()
{
    m_importXML = new InitImportXML(m_base, m_settings);
    connect(m_importXML, SIGNAL(restart()), this, SLOT(beginSlot()));
    connect(m_importXML, SIGNAL(end()), this, SIGNAL(end()));
    m_mainWindow->setCentralWidget(m_importXML);
}


void InitializeDatabase::beginSlot()
{
    InitSelectAction* selectAction = new InitSelectAction();
    connect(selectAction, SIGNAL(nothing()), this, SLOT(nothingSlot()));
    connect(selectAction, SIGNAL(importXML()), this, SLOT(importXmlSlot()));
    connect(selectAction, SIGNAL(searchFolder()), this, SLOT(selectFolderSlot()));

    m_mainWindow->setCentralWidget(selectAction);
}


void InitializeDatabase::selectFolderSlot()
{
    InitAnalyzeSelect* selectFolder = new InitAnalyzeSelect(m_base);
    connect(selectFolder, SIGNAL(next(int)), this, SLOT(analyzeFolderArtist(int)));
    connect(selectFolder, SIGNAL(restart()), this, SLOT(beginSlot()));

    m_mainWindow->setCentralWidget(selectFolder);
}


void InitializeDatabase::analyzeFolderArtist(int lastTrackId)
{
    m_lastTrackId = lastTrackId;

    InitAnalyzeArtist* analyzeArtist = new InitAnalyzeArtist(m_base, m_mainWindow);
    connect(analyzeArtist, SIGNAL(next()), this, SLOT(analyzeFolderAlbum()));

    m_mainWindow->setCentralWidget(analyzeArtist);
}


void InitializeDatabase::analyzeFolderAlbum()
{
    InitAnalyzeAlbum* analyzeAlbum = new InitAnalyzeAlbum(m_base, m_lastTrackId, m_mainWindow);
    connect(analyzeAlbum, SIGNAL(next(bool, int)), this, SLOT(endSlot(bool, int)));

    m_mainWindow->setCentralWidget(analyzeAlbum);
}


void InitializeDatabase::endSlot(bool deleteTracks, int lastTrackId)
{
    if(deleteTracks)
        m_base->simpleQuery("DELETE FROM pistes WHERE album_id==0;");

    emit idChanged("lastTrackId", lastTrackId);
    emit idChanged("lastAlbumId", m_base->countEntries("albums")-1);
    emit idChanged("lastArtistId", m_base->countEntries("artistes")-1);

    /*std::vector<Row> resultArtist;
    resultArtist = baseTemp->simpleQueryResult("SELECT * FROM artistes WHERE id!=0;");

    for(unsigned int i=0; i < resultArtist.size(); i++)
    {
        std::queue<QString> liste;
        liste.push("INT");
        liste.push(resultArtist[i].row["id"]);
        liste.push("STRING");
        liste.push(resultArtist[i].row["nom"]);
        liste.push("STRING");
        liste.push(resultArtist[i].row["type"]);
        if(!resultArtist[i].row["photo"].isEmpty())
        {
            liste.push("STRING");
            liste.push(resultArtist[i].row["photo"]);
        }
        else
            liste.push("EMPTY");

        m_base->preparedQuery("INSERT INTO artistes VALUES(?, ?, ?, ?)", liste);
    }


    std::vector<Row> resultAlbum;
    resultAlbum = baseTemp->simpleQueryResult("SELECT * FROM albums WHERE id != 0;");

    for(unsigned int i=0; i < resultAlbum.size(); i++)
    {
        std::queue<QString> liste;
        liste.push("INT");
        liste.push(resultAlbum[i].row["id"]); // Album id
        liste.push("STRING");
        liste.push(resultAlbum[i].row["titre"]); // Album title
        liste.push("INT");
        liste.push(resultAlbum[i].row["artiste_id"]); // Id of artist, find previously
        liste.push("INT");
        liste.push(resultAlbum[i].row["publication"]); // Date of publication
        if(!resultAlbum[i].row["photo"].isEmpty())
        {
            liste.push("STRING");
            liste.push(resultAlbum[i].row["photo"]); // Path for the picture of the album
        }
        else
            liste.push("EMPTY");
        m_base->preparedQuery("INSERT INTO albums VALUES(?, ?, ?, ?, ?)", liste);
    }

    std::vector<Row> resultTrack;
    resultTrack = baseTemp->simpleQueryResult("SELECT * FROM pistes WHERE album_id!=0;");

    for(unsigned int i=0; i < resultTrack.size(); i++)
    {
        std::queue<QString> listeTrack;
        listeTrack.push("INT");
        listeTrack.push(resultTrack[i].row["id"]); // Id of the track
        listeTrack.push("STRING");
        listeTrack.push(resultTrack[i].row["titre"]); // Title of the track
        listeTrack.push("INT");
        listeTrack.push(resultTrack[i].row["artiste_id"]); // Id of the artist
        listeTrack.push("INT"); // Id of the album
        listeTrack.push(resultTrack[i].row["album_id"]);
        listeTrack.push("STRING"); // Number of the track
        listeTrack.push(resultTrack[i].row["numero"]);
        listeTrack.push("STRING");
        listeTrack.push(resultTrack[i].row["path"]); // Path of the music
        listeTrack.push("INT");
        listeTrack.push(resultTrack[i].row["duration"]); // Duration of music

        m_base->preparedQuery("INSERT INTO pistes VALUES(?, ?, ?, ?, ?, ?, ?)", listeTrack);
    }*/

    emit end();

    /*m_lastTrackId = lastTrackId;

    InitAnalyzeTracks* analyzeTracks = new InitAnalyzeTracks(baseTemp, m_lastTrackId, m_mainWindow);
    //connect(analyzeTracks, SIGNAL(next(QSqliteCom*, int)), this, SLOT(analyzeFolderTracks(QSqliteCom*, int)));

    m_mainWindow->setCentralWidget(analyzeTracks);*/

}
