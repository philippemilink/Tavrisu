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

#include "NewArtist.h"
#include "ui_NewArtist.h"

NewArtist::NewArtist(QSqliteCom *base, int lastArtistId, QWidget *parent, QString name) :
    QDialog(parent), m_base(base), m_lastArtistId(lastArtistId), ui(new Ui::NewArtist)
{
    ui->setupUi(this);

    ui->buttonSubmit->setEnabled(false);

    m_buttonImage = new ButtonImage(tr("Ouvrir l'image\nde l'artiste"), this, 10, 10, 91, 91, "");

    ui->lineEditNom->setText(name);

    connect(ui->buttonSubmit, SIGNAL(clicked()), this, SLOT(buttonSubmit_clicked_save()));
}

/*
 * Contructor of the class NewArtist to edit an artist
 * The id of the artist is passed as the first parameter.
 */
NewArtist::NewArtist(int id, QSqliteCom *base, QWidget *parent) : QDialog(parent), m_base(base), ui(new Ui::NewArtist), m_previousId(id)
{
    ui->setupUi(this);
    ui->buttonSubmit->setEnabled(true);
    this->setWindowTitle(tr("Editer l'artiste"));

    std::queue<QString> list;
    list.push("INT");
    list.push(QString::number(id));

    std::vector<Row> result;
    result = m_base->preparedQueryResult("SELECT * FROM artistes WHERE id=?;", list);

    m_previousName = result[0].row["nom"];

    ui->lineEditNom->setText(result[0].row["nom"]);
    int foundIndex = ui->selectType->findText(result[0].row["type"]);
    if(foundIndex != -1)
        ui->selectType->setCurrentIndex(foundIndex);

    m_buttonImage = new ButtonImage(tr("Ouvrir l'image\nde l'artiste"), this, 10, 10, 91, 91, result[0].row["photo"]);

    connect(ui->buttonSubmit, SIGNAL(clicked()), this, SLOT(buttonSubmit_clicked_update()));
}


NewArtist::~NewArtist()
{
    delete ui;
}



void NewArtist::on_buttonCancel_clicked()
{
    delete this;
}


void NewArtist::buttonSubmit_clicked_save()
{
    std::queue<QString> liste;
    liste.push("INT");
    liste.push(QString::number(m_lastArtistId+1));
    liste.push("STRING");
    liste.push(ui->lineEditNom->text());
    liste.push("STRING");
    liste.push(ui->selectType->currentText());
    if(m_buttonImage->path() != "")
    {
        liste.push("STRING");
        liste.push(m_buttonImage->path());
    }
    else
        liste.push("EMPTY");

    if(m_base->preparedQuery("INSERT INTO artistes VALUES(?, ?, ?, ?)", liste))
    {
        emit idChanged("lastArtistId", m_lastArtistId + 1);
        this->destroy();
    }
    else
        QMessageBox::critical(0, tr("Enregistrement de l'artiste"), tr("<strong>Erreur:</strong><br />") + m_base->lastError());
}


void NewArtist::buttonSubmit_clicked_update()
{
    std::queue<QString> liste;
    liste.push("STRING");
    liste.push(ui->lineEditNom->text());
    liste.push("STRING");
    liste.push(ui->selectType->currentText());
    if(m_buttonImage->path() != "")
    {
        liste.push("STRING");
        liste.push(m_buttonImage->path());
    }
    else
        liste.push("EMPTY");
    liste.push("STRING");
    liste.push(QString::number(m_previousId));

    if(m_base->preparedQuery("UPDATE artistes SET nom=?, type=?, photo=? WHERE id=?", liste))
    {
        emit finish();
        this->destroy();
    }
    else
        QMessageBox::critical(0, tr("Modification de l'ariste"), tr("<strong>Erreur:</strong><br />") + m_base->lastError());
}


void NewArtist::on_lineEditNom_textChanged(QString text)
{
    if(text.isEmpty())
        ui->buttonSubmit->setEnabled(false);
    else
    {
        if(text != m_previousName)
        {
            if(m_base->isset("SELECT * FROM artistes WHERE nom=:value", text))
            {
                ui->buttonSubmit->setEnabled(false);
                ui->labelError->setText(tr("<span style=\"color: red;\">Cet artiste existe déjà !</span>"));
            }
            else
            {
                ui->buttonSubmit->setEnabled(true);
                ui->labelError->setText("");
            }
        }
    }
}


bool NewArtist::deleteArtist(QString name, int id, QSqliteCom* base)
{
    int reponse;

    reponse = QMessageBox::question(0,
                                    tr("Supprimer une musique"),
                                    tr("Êtes-vous sûr de vouloir supprimer l'artiste ") + name + " ?",
                                    QMessageBox::Yes|QMessageBox::No);

    if(reponse == QMessageBox::Yes)
    {
        std::queue<QString> listTracks;
        listTracks.push("INT");
        listTracks.push(QString::number(id)); // Id of the artist

        if(!base->preparedQuery("DELETE FROM pistes WHERE artiste_id=?;", listTracks))
        {
            QMessageBox::critical(0,
                                  tr("Suppression de l'artiste."),
                                  tr("<strong>Erreur:</strong><br />Erreur lors de la suppression des pistes de l'artiste.") + base->lastError());
            return true;
        }

        std::queue<QString> listAlbum;
        listAlbum.push("INT");
        listAlbum.push(QString::number(id)); // Id of the artist

        if(!base->preparedQuery("DELETE FROM albums WHERE artiste_id=?;", listAlbum))
        {
            QMessageBox::critical(0,
                                  tr("Suppression de l'artiste."),
                                  tr("<strong>Erreur:</strong><br />Erreur lors de la suppression des albums de l'artiste.") + base->lastError());
            return true;
        }

        std::queue<QString> listArtist;
        listArtist.push("INT");
        listArtist.push(QString::number(id)); // Id of the track

        if(!base->preparedQuery("DELETE FROM artistes WHERE id=?;", listArtist))
        {
            QMessageBox::critical(0,
                                  tr("Suppression de l'artiste."),
                                  tr("<strong>Erreur:</strong><br />Erreur lors de la suppression de l'artiste.") + base->lastError());
            return true;
        }
        else
            return true;
    }
    else
        return false;
}
