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

#include "Updater.h"
#include "ui_Updater.h"


Updater::Updater(QWidget *parent) :
    QWidget(parent), ui(new Ui::Updater)
{
    ui->setupUi(this);
}


Updater::~Updater()
{
    delete ui;
}


void Updater::searchUpdate()
{
    m_downloadXmlError = false;
    m_downloadUpdateError = false;
    m_downloadFinished = false;
    m_existsNewVersion = false;

    ui->buttonDownload->setText(tr("Télécharger"));

    QNetworkAccessManager *networkManager = new QNetworkAccessManager;
    m_networkXml = networkManager->get(QNetworkRequest(QUrl(UPDATE_PATH)));
    connect(m_networkXml, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(errorXMLFile(QNetworkReply::NetworkError)));
    connect(m_networkXml, SIGNAL(finished()), this, SLOT(saveXMLFile()));
}


void Updater::display()
{
    searchUpdate();
    show();
}


void Updater::errorXMLFile(QNetworkReply::NetworkError)
{
    ui->label->setText(tr("Erreur lors du téléchargement du fichier de mise à jour."));
}


void Updater::saveXMLFile()
{
    if(m_downloadXmlError)
        return;

    // Save XML file in temp
    QTemporaryFile* xml = new QTemporaryFile();
    if(!xml->open())
    {
        ui->label->setText(tr("Erreur lors de l'enregistrement du fichier de mise à jour."));
        return;
    }

    xml->write(m_networkXml->readAll());
    xml->seek(0); // Back to begin of file
    parseXML(xml); // Parsing

    xml->close();
    m_networkXml->deleteLater();
}


void Updater::parseXML(QTemporaryFile* xml)
{
    QDomDocument* dom = new QDomDocument("files");
    if(!dom->setContent(xml))
    {
        ui->label->setText(tr("Erreur lors de l'analyse du fichier de mise à jour."));
        return;
    }

    QDomNodeList nodeList = dom->firstChild().childNodes();

    for(int i=0; i<nodeList.count(); i++)
    {
        QDomElement e = nodeList.at(i).toElement();
        if(e.nodeName() == "id")
        {
            int id = UPDATE_ID
            if(e.text().toInt() == id)
            {
                ui->label->setText(tr("Pas de nouvelle version."));
                m_existsNewVersion = false;
                return;
            }
            else
            {
                m_existsNewVersion = true;
                ui->label->setText(tr("Une nouvelle version est disponible."));
            }
        }
        if(e.nodeName() == "version")
            m_newVersion = e.text();
        if(e.nodeName() == "type")
            m_newType = e.text();
        if(e.nodeName() == "path")
            m_updatePath = e.text();
        if(e.nodeName() == "md5")
            m_updateMd5 = e.text();
        if(e.nodeName() == "whatsnew")
        {
            QDomNodeList itemsList = e.childNodes();

            m_newWhatsnew.clear();
            for(int j=0; j<itemsList.count(); j++)
            {
                m_newWhatsnew.push_back(itemsList.at(j).toElement().text());
            }
        }
        if(e.nodeName() == "date")
            m_newDate = e.attribute("day") + "/" + e.attribute("month") + "/" + e.attribute("year");
    }

    ui->textAera->setEnabled(true);
    setInfosNewVersion();
    ui->buttonDownload->setEnabled(true);
}


void Updater::setInfosNewVersion()
{
    QString text;

    text += tr("<b>Version:</b> ") + m_newVersion + "<br />";
    text += tr("<b>Type:</b> ") + m_newType + "<br />";
    text += tr("<b>Date de sortie: </b>") + m_newDate + "<br /><br />";
    text += tr("<b>Nouveautés: </b><br />");

    for(unsigned int i=0; i<m_newWhatsnew.size(); i++)
        text += "- " + m_newWhatsnew.at(i) + "<br />";

    ui->textAera->setText(text);
}


void Updater::on_buttonCancel_clicked()
{
    close();
}


void Updater::on_buttonDownload_clicked()
{
    if(m_downloadFinished)
    {
        close();
        return;
    }

    m_destinationPath = QFileDialog::getExistingDirectory(this,
                                                          tr("Sélectionnez le dossier de destination de l'éxecutable"),
                                                          QDir::homePath());
    if(m_destinationPath.isEmpty())
        return;

    QNetworkAccessManager* networkManager = new QNetworkAccessManager;
    m_networkUpdate = networkManager->get(QNetworkRequest(QUrl(m_updatePath)));
    connect(m_networkUpdate, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(errorUpdateFile(QNetworkReply::NetworkError)));
    connect(m_networkUpdate, SIGNAL(finished()), this, SLOT(saveUpdateFile()));
    connect(m_networkUpdate, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(progressionUpdateDownload(qint64, qint64)));

    ui->buttonDownload->setEnabled(false);
    ui->progressBar->setEnabled(true);

    ui->label->setText("Téléchargement en cours...");
}


void Updater::errorUpdateFile(QNetworkReply::NetworkError)
{
    ui->label->setText(tr("Une erreur est survenue lors du téléchargement de la mise à jour."));
    ui->textAera->clear();
    ui->textAera->setEnabled(false);

    m_downloadUpdateError = true;
    m_downloadFinished = true;
}


void Updater::saveUpdateFile()
{
    if(m_downloadUpdateError)
        return;

    ui->label->setText(tr("Téléchargement terminé."));

    QString updatePath = m_destinationPath + "/" + QFileInfo(m_updatePath).fileName();
    QFile* update = new QFile(updatePath);
    if(!update->open(QFile::ReadWrite))
    {
        ui->label->setText(tr("Erreur lors de l'enregistrement de la mise à jour."));
        ui->textAera->clear();
        ui->textAera->setEnabled(false);
        return;
    }

    update->write(m_networkUpdate->readAll());
    m_networkUpdate->deleteLater();

    m_downloadFinished = true;
    ui->buttonDownload->setText(tr("Fermer"));
    ui->buttonDownload->setEnabled(true);

    update->seek(0);
    QByteArray hashed = QCryptographicHash::hash(update->readAll(), QCryptographicHash::Md5);
    update->close();
    QString hash(hashed.toHex().data());

    if(m_updateMd5 == hash)
        questionSaveData();
    else
    {
        ui->textAera->setEnabled(false);
        ui->label->setText(tr("La mise à jour téléchargée n'est pas conforme."));
        m_downloadUpdateError = true;
        QFile::remove(updatePath);
    }
}


void Updater::progressionUpdateDownload(qint64 bytesReceived, qint64 bytesTotal)
{
    if(m_downloadUpdateError)
    {
        ui->progressBar->setValue(0);
        return;
    }

    if (bytesTotal != -1)
    {
        ui->progressBar->setRange(0, bytesTotal);
        ui->progressBar->setValue(bytesReceived);
    }
}


void Updater::questionSaveData()
{
    int answer = QMessageBox::question(this,
                                       tr("Souhaitez-vous sauvegarder ?"),
                                       tr("Souhaitez-vous sauvegarder votre base de données Tavrisu avant d'installer la mise à jour ?<br />En installant la mise à jour, toutes vos données seront effacées."),
                                       QMessageBox::Yes, QMessageBox::No);

    if(answer == QMessageBox::Yes)
        emit exportDatabase();

    close();
}


bool Updater::existsNewVersion()
{
    return m_existsNewVersion;
}
