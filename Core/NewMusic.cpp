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

#include "NewMusic.h"
#include "ui_NewMusic.h"


NewMusic::NewMusic(QSqliteCom *base, QSettings *settings, QWidget *parent) :
    m_base(base), QDialog(parent), ui(new Ui::NewMusic), m_settings(settings)
{
    ui->setupUi(this);

    m_vlc = new QlibVLC();

    // Set artists of the database in the selector (QComboBox)
    QStringList artists;
    std::vector<Row> result;
    result = m_base->simpleQueryResult("SELECT nom FROM artistes ORDER BY nom;");

    artists << tr("Artiste non précisé");
    ui->selectArtist->addItem(artists.first());
    for(unsigned int i=1; i < result.size(); i++)
    {
        artists << result[i].row["nom"];
        ui->selectArtist->addItem(result[i].row["nom"]);
    }

    /*ui->selectArtist->setEditable(true);

    m_completerArtist = new QCompleter(artists);
    m_completerArtist->setCaseSensitivity(Qt::CaseInsensitive);
    ui->selectArtist->setCompleter(m_completerArtist);*/

    connect(ui->lineEditTitle, SIGNAL(textChanged(QString)), this, SLOT(verifyButtonSubmit(QString)));
    connect(ui->buttonSubmit, SIGNAL(clicked()), this, SLOT(buttonSubmit_clicked_save()));
}


NewMusic::NewMusic(int id, QSqliteCom *base, QSettings *settings, QWidget *parent) :
    m_previousId(id), m_base(base), QDialog(parent), ui(new Ui::NewMusic), m_settings(settings)
{
    ui->setupUi(this);
    ui->buttonSubmit->setEnabled(true);
    this->setWindowTitle(tr("Editer une musique"));

    m_vlc = new QlibVLC();


    // Set current values in fields:
    std::queue<QString> list;
    list.push("INT");
    list.push(QString::number(m_previousId));

    std::vector<Row> result;
    result = m_base->preparedQueryResult("SELECT * FROM pistes WHERE id=?;", list);

    ui->lineEditTitle->setText(result[0].row["titre"]);
    ui->lineEditPath->setText(result[0].row["path"]);


    // Set artists of the database in the selector (QComboBox)
    //QStringList artists;
    std::vector<Row> resultArtist;
    resultArtist = m_base->simpleQueryResult("SELECT id, nom FROM artistes ORDER BY nom;");

    //artists << tr("Artiste non précisé");
    ui->selectArtist->addItem(tr("Artiste non précisé"));
    for(unsigned int i=1; i < resultArtist.size(); i++)
    {
       // artists << resultArtist[i].row["nom"];
        ui->selectArtist->addItem(resultArtist[i].row["nom"]);
        if (resultArtist[i].row["id"]== result[0].row["artiste_id"])
        {
            int foundIndex = ui->selectArtist->findText(resultArtist[i].row["nom"]);
            if(foundIndex != -1)
                ui->selectArtist->setCurrentIndex(foundIndex);
        }
    }

    connect(ui->lineEditTitle, SIGNAL(textChanged(QString)), this, SLOT(verifyButtonSubmit(QString)));
    connect(ui->buttonSubmit, SIGNAL(clicked()), this, SLOT(buttonSubmit_clicked_update()));
}


NewMusic::~NewMusic()
{
    delete ui;
}


void NewMusic::start()
{
    m_filePath = QFileDialog::getOpenFileName(this,
                                              tr("Sélectionnez un fichier audio"),
                                              QDir::homePath(),
                                              tr("Musiques (*.mp3 *.wav *.ogg *.flac *.acc)"));
    if(!m_filePath.isEmpty())
    {
        ui->lineEditPath->setText(m_filePath);
        ui->lineEditTitle->setText(QFileInfo(m_filePath).completeBaseName());

        m_vlc->setMedia(m_filePath);

        QString artist = m_vlc->artist();
        if(!artist.isNull()) {
            int foundIndex = ui->selectArtist->findText(artist);
            if(foundIndex != -1)
                ui->selectArtist->setCurrentIndex(foundIndex);
        }

        this->show();
    }
}


void NewMusic::verifyButtonSubmit(QString value)
{
    if(!value.isEmpty())
        ui->buttonSubmit->setEnabled(true);
    else
        ui->buttonSubmit->setDisabled(true);
}


void NewMusic::buttonSubmit_clicked_save()
{
    /*if(!verifyArtist())
        return;*/

    // First: we need id of the artist
    QString chaineIdArtist;
    if(ui->selectArtist->currentText() != tr("Artiste non précisé"))
    {
        std::queue<QString> listeArtist;
        listeArtist.push("STRING");
        listeArtist.push(ui->selectArtist->currentText());

        std::vector<Row> result;
        result = m_base->preparedQueryResult("SELECT id FROM artistes WHERE nom=?;", listeArtist);
        chaineIdArtist = result[0].row["id"];
    }
    else
        chaineIdArtist = "0";

    // Second: add audio file in database
    std::queue<QString> listeTrack;
    listeTrack.push("INT");
    listeTrack.push(QString::number(m_settings->value("lastTrackId").toInt()+1)); // Id of the track
    listeTrack.push("STRING");
    listeTrack.push(ui->lineEditTitle->text()); // Title of the track
    listeTrack.push("INT");
    listeTrack.push(chaineIdArtist); // Id of the artist
    listeTrack.push("INT"); // Id of the album
    listeTrack.push("0");
    listeTrack.push("EMPTY"); // Number of the track
    listeTrack.push("STRING");
    listeTrack.push(m_filePath); // Path of the music
    listeTrack.push("INT");
    listeTrack.push(QString::number(m_vlc->getDuration(m_filePath))); // Duration of music

    if(!m_base->preparedQuery("INSERT INTO pistes VALUES(?, ?, ?, ?, ?, ?, ?)", listeTrack))
    {
        QMessageBox::critical(0, tr("Enregistrement du fichier audio"), tr("<strong>Erreur:</strong><br />") + m_base->lastError());
        return;
    }

    m_settings->setValue("lastTrackId", m_settings->value("lastTrackId").toInt()+1);
    emit finish();
    this->destroy();
}


void NewMusic::buttonSubmit_clicked_update()
{
    /*if(!verifyArtist())
        return;*/

    // First: we need id of the artist
    QString chaineIdArtist;
    if(ui->selectArtist->currentText() != tr("Artiste non précisé"))
    {
        std::queue<QString> listeArtist;
        listeArtist.push("STRING");
        listeArtist.push(ui->selectArtist->currentText());

        std::vector<Row> result;
        result = m_base->preparedQueryResult("SELECT id FROM artistes WHERE nom=?;", listeArtist);
        chaineIdArtist = result[0].row["id"];
    }
    else
        chaineIdArtist = "0";


    // Second: add audio file in database
    std::queue<QString> listeTrack;
    listeTrack.push("STRING");
    listeTrack.push(ui->lineEditTitle->text()); // Title of the track
    listeTrack.push("INT");
    listeTrack.push(chaineIdArtist); // Id of the artist
    listeTrack.push("INT");
    listeTrack.push(QString::number(m_previousId)); // Id of the track


    if(!m_base->preparedQuery("UPDATE pistes SET titre=?, artiste_id=? WHERE id=?", listeTrack))
    {
        QMessageBox::critical(0, tr("Edition du fichier audio"), tr("<strong>Erreur:</strong><br />") + m_base->lastError());
        return;
    }

    emit finish();
    this->destroy();
}


bool NewMusic::verifyArtist()
{
    if(ui->selectArtist->currentText().isEmpty())
    {
        QMessageBox::warning(this, tr("Erreur"), tr("Le champ <b>Artiste</b> est vide."));
        return false;
    }

    if(ui->selectArtist->currentText() == tr("Artiste non précisé"))
        return true;

    if(m_base->isset("SELECT nom FROM artistes WHERE nom=:value", ui->selectArtist->currentText()))
        return true;
    else
    {
        int answer = QMessageBox::question(this, tr("Artiste inexistant"),
                                           tr("Cet artiste n'existe pas. Souhaitez-vous le créer ?"),
                                           QMessageBox::Yes|QMessageBox::No);
        if(answer == QMessageBox::No)
            return false;
        else
        {
            NewArtist* newArtist = new NewArtist(m_base, m_settings->value("lastArtistId").toInt(), this, ui->selectArtist->currentText());

        }
    }
}


bool NewMusic::deleteMusic(QString name, int id, QSqliteCom* base)
{
    int reponse;

    reponse = QMessageBox::question(0,
                                    tr("Supprimer une musique"),
                                    tr("Êtes-vous sûr de vouloir supprimer ") + name + " ?",
                                    QMessageBox::Yes|QMessageBox::No);

    if(reponse == QMessageBox::Yes)
    {
        QString chaineIdTrack;
        chaineIdTrack.sprintf("%d", id);

        std::queue<QString> list;
        list.push("INT");
        list.push(chaineIdTrack); // Id of the track

        if(!base->preparedQuery("DELETE FROM pistes WHERE id=?;", list))
        {
            QMessageBox::critical(0, tr("Suppression du fichier audio"), tr("<strong>Erreur:</strong><br />") + base->lastError());
            return true;
        }
        else
            return true;
    }
    else
        return false;
}
