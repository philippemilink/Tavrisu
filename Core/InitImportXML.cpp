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

#include "InitImportXML.h"
#include "ui_InitImportXML.h"

InitImportXML::InitImportXML(QSqliteCom *base, QSettings *settings, QWidget *parent) :
    QWidget(parent), ui(new Ui::InitImportXML), m_base(base), m_settings(settings)
{
    ui->setupUi(this);

    ui->lineEditFile->setText(QDir::homePath());

    m_label = new QLabel(ui->progressBar);
    m_label->setAlignment(Qt::AlignCenter);
}


InitImportXML::~InitImportXML()
{
    delete ui;
}


void InitImportXML::on_buttonFile_clicked()
{
    QString selectedPath;

    selectedPath = QFileDialog::getOpenFileName(this, tr("Sélectionnez votre sauvegarde"),
                                                QDir::homePath(), tr("Fichier XML (*.xml)"));
    if(!selectedPath.isEmpty())
    {
        m_path = selectedPath;
        ui->lineEditFile->setText(m_path);
        ui->buttonNext->setEnabled(true);
    }
}


void InitImportXML::on_buttonNext_clicked()
{
    QDomDocument* dom = new QDomDocument("save");

    QFile* file = new QFile(m_path);
    if(file->open(QFile::ReadOnly|QFile::Text))
    {
        if(!dom->setContent(file))
        {
            QMessageBox::critical(this, tr("Erreur"), tr("Erreur lors du parsage du fichier XML: la sauvegarde semble incorrecte."));
            emit restart();
            return;
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Erreur"), tr("Erreur lors de l'ouverture du fichier XML."));
        emit restart();
        return;
    }

    m_domElement = dom->documentElement();
    ui->progressBar->setMaximum(m_domElement.attribute("items").toInt());
    QDomNode node = m_domElement.firstChild();
    parseXML(node);
    file->close();
    emit end();
}


void InitImportXML::parseXML(QDomNode node)
{
    m_label->setGeometry(0, 0, ui->progressBar->width(), ui->progressBar->height());
    while(!node.isNull())
    {
        if(!m_domElement.isNull())
        {
            if(node.hasChildNodes())
            {
                m_parentNode = node.nodeName();
                parseXML(node.firstChild());
            }
            else
            {
                QDomElement e = node.toElement();
                if(m_parentNode == "settings")
                {
                    m_label->setText(tr("Importation des paramètres"));
                    if(e.attribute("name") == "searchUpdate")
                    {
                        if(e.attribute("value") == "1")
                            m_settings->setValue("searchUpdate", true);
                        else
                            m_settings->setValue("searchUpdate", false);
                    }
                    else
                        m_settings->setValue(e.attribute("name"), e.attribute("value").toInt());
                    ui->progressBar->setValue(ui->progressBar->value()+1);
                }
                if(m_parentNode == "tracks")
                {
                    m_label->setText(tr("Importation de ") + e.attribute("title"));
                    std::queue<QString> listeTrack;
                    listeTrack.push("INT");
                    listeTrack.push(e.attribute("id")); // Id of the track
                    listeTrack.push("STRING");
                    listeTrack.push(e.attribute("title")); // Title of the track
                    listeTrack.push("INT");
                    listeTrack.push(e.attribute("artist_id")); // Id of the artist
                    listeTrack.push("INT"); // Id of the album
                    listeTrack.push(e.attribute("album_id"));
                    if(e.attribute("number") == "")
                        listeTrack.push("EMPTY");
                    else
                    {
                        listeTrack.push("INT"); // Number of the track
                        listeTrack.push(e.attribute("number"));
                    }
                    listeTrack.push("STRING");
                    listeTrack.push(e.attribute("path")); // Path of the music
                    listeTrack.push("INT");
                    listeTrack.push(e.attribute("duration")); // Duration of music
                    m_base->preparedQuery("INSERT INTO pistes VALUES(?, ?, ?, ?, ?, ?, ?)", listeTrack);
                    ui->progressBar->setValue(ui->progressBar->value()+1);
                }
                if(m_parentNode == "albums")
                {
                    m_label->setText(tr("Importation de ") + e.attribute("title"));
                    std::queue<QString> liste;
                    liste.push("INT");
                    liste.push(e.attribute("id")); // Album id
                    liste.push("STRING");
                    liste.push(e.attribute("title")); // Album title
                    liste.push("INT");
                    liste.push(e.attribute("artist_id")); // Id of artist, find previously
                    liste.push("INT");
                    liste.push(e.attribute("year")); // Date of publication
                    if(!e.attribute("picture").isEmpty())
                    {
                        liste.push("STRING");
                        liste.push(e.attribute("picture")); // Path for the picture of the album
                    }
                    else
                        liste.push("EMPTY");
                    m_base->preparedQuery("INSERT INTO albums VALUES(?, ?, ?, ?, ?)", liste);
                    ui->progressBar->setValue(ui->progressBar->value()+1);
                }
                if(m_parentNode == "artists")
                {
                    m_label->setText(tr("Importation de ") + e.attribute("name"));
                    std::queue<QString> liste;
                    liste.push("INT");
                    liste.push(e.attribute("id"));
                    liste.push("STRING");
                    liste.push(e.attribute("name"));
                    liste.push("STRING");
                    liste.push(e.attribute("type"));
                    if(e.attribute("picture") != "")
                    {
                        liste.push("STRING");
                        liste.push(e.attribute("picture"));
                    }
                    else
                        liste.push("EMPTY");
                    m_base->preparedQuery("INSERT INTO artistes VALUES(?, ?, ?, ?)", liste);
                    ui->progressBar->setValue(ui->progressBar->value()+1);
                }
            }
        }
        node = node.nextSibling();
    }
    m_label->setText(tr("Terminé"));
}
