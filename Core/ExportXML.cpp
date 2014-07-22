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

#include "ExportXML.h"
#include "ui_ExportXML.h"


ExportXML::ExportXML(QSqliteCom *base, QSettings *settings, QWidget *parent) :
    QDialog(parent), ui(new Ui::ExportXML), m_base(base), m_settings(settings)
{
    ui->setupUi(this);

    setWindowTitle(tr("Sauvegarder votre base de données"));

    m_totalItems = m_base->countEntries("artistes") - 1 + m_base->countEntries("albums") - 1 + m_base->countEntries("pistes") + 9;
    ui->progressBar->setMaximum(m_totalItems);

    m_canQuit = false;

    m_label = new QLabel(ui->progressBar);
    m_label->setGeometry(0, 0, 332, 21);
    m_label->setAlignment(Qt::AlignCenter);

}


ExportXML::~ExportXML()
{
    delete ui;
}


void ExportXML::on_buttonPath_clicked()
{
    QString selectedPath = QFileDialog::getExistingDirectory(this, tr("Sélectionnez le dossier"), QDir::homePath());
    if(!selectedPath.isEmpty())
    {
        m_path = selectedPath;
        ui->lineEditPath->setText(m_path);
        ui->buttonSave->setEnabled(true);
    }
}


void ExportXML::on_buttonSave_clicked()
{
    if(m_canQuit)
        destroy();
    else
    {
        if(QFile::exists(m_path + "/Tavrisu-" + QDate::currentDate().toString("dd-MM-yyyy") + ".xml"))
        {
            int answer = QMessageBox::question(this,
                                               tr("Le fichier existe déjà"),
                                               tr("Un fichier avec le même nom existe déjà. Souhaitez-vous le remplacer ?"),
                                               QMessageBox::Yes|QMessageBox::No);
            if(answer != QMessageBox::Yes)
                return;
        }
        if(!exportXML())
            QMessageBox::warning(this, tr("Erreur"), tr("Une erreur s'est produite lors de l'exportation de la base de données."));
        else
        {
            QMessageBox::information(this, tr("Exportation réussie"), tr("L'exportation de la base de données de Tavrisu a réussi."));
            m_canQuit = true;
            ui->buttonSave->setText(tr("Fermer"));
        }
    }
}


bool ExportXML::exportXML()
{
    QFile file(m_path + "/Tavrisu-" + QDate::currentDate().toString("dd-MM-yyyy") + ".xml");

    if(!file.open(QIODevice::WriteOnly))
        return false;


    QTextStream stream(&file);
    stream.setCodec("UTF-8");


    m_label->setText(tr("Export des paramètres"));

    stream << "<Tavrisu items=\"" << m_totalItems - 6 << "\">\n\t<settings>\n";
    ui->progressBar->setValue(ui->progressBar->value()+1);
    stream << "\t\t<setting name=\"lastAlbumId\" value=\"" << m_settings->value("lastAlbumId").toString() << "\" />\n";
    ui->progressBar->setValue(ui->progressBar->value()+1);
    stream << "\t\t<setting name=\"lastArtistId\" value=\"" << m_settings->value("lastArtistId").toString() << "\" />\n";
    ui->progressBar->setValue(ui->progressBar->value()+1);
    stream << "\t\t<setting name=\"lastTrackId\" value=\"" << m_settings->value("lastTrackId").toString() << "\" />\n";
    ui->progressBar->setValue(ui->progressBar->value()+1);
    if(m_settings->value("searchUpdate").toBool())
        stream << "\t\t<setting name=\"searchUpdate\" value=\"1\" />\n";
    else
        stream << "\t\t<setting name=\"searchUpdate\" value=\"0\" />\n";
    ui->progressBar->setValue(ui->progressBar->value()+1);

    stream << "\t</settings>\n\t<tracks>\n";
    ui->progressBar->setValue(ui->progressBar->value()+1);
    std::vector<Row> result;
    result = m_base->simpleQueryResult("SELECT * FROM pistes;");
    for(unsigned int i=0; i < result.size(); i++)
    {
        m_label->setText(tr("Export de ") + result[i].row["titre"]);
        stream << "\t\t<track id=\"";
        stream << result[i].row["id"];
        stream << "\" title=\"";
        stream <<  QString(result[i].row["titre"])/*.toHtmlEscaped()*/.toUtf8();
        stream << "\" artist_id=\"";
        stream << result[i].row["artiste_id"];
        stream << "\" album_id=\"";
        stream << result[i].row["album_id"];
        stream << "\" number=\"";
        stream << result[i].row["numero"];
        stream << "\" path=\"";
        stream << result[i].row["path"]/*.toHtmlEscaped()*/.toUtf8();
        stream << "\" duration=\"";
        stream << result[i].row["duration"];
        stream << "\" />\n";
        ui->progressBar->setValue(ui->progressBar->value()+1);
    }
    stream << "\t</tracks>\n\t<albums>\n";
    ui->progressBar->setValue(ui->progressBar->value()+1);

    result = m_base->simpleQueryResult("SELECT * FROM albums WHERE id > 0;");
    for(unsigned int i=0; i < result.size(); i++)
    {
        m_label->setText(tr("Export de ") + result[i].row["titre"]);
        stream << "\t\t<album id=\"";
        stream << result[i].row["id"];
        stream << "\" title=\"";
        stream << result[i].row["titre"]/*.toHtmlEscaped()*/.toUtf8();
        stream << "\" artist_id=\"";
        stream << result[i].row["artiste_id"];
        stream << "\" year=\"";
        stream << result[i].row["publication"];
        stream << "\" picture=\"";
        stream << result[i].row["photo"]/*.toHtmlEscaped()*/.toUtf8();
        stream << "\" />\n";
        ui->progressBar->setValue(ui->progressBar->value()+1);
    }
    stream << "\t</albums>\n\t<artists>\n";
    ui->progressBar->setValue(ui->progressBar->value()+1);

    result = m_base->simpleQueryResult("SELECT * FROM artistes WHERE id > 0;");
    for(unsigned int i=0; i < result.size(); i++)
    {
        m_label->setText(tr("Export de ") + result[i].row["nom"]);
        stream << "\t\t<artist id=\"";
        stream << result[i].row["id"];
        stream << "\" name=\"";
        stream << result[i].row["nom"]/*.toHtmlEscaped()*/.toUtf8();
        stream << "\" type=\"";
        stream << result[i].row["type"]/*.toHtmlEscaped()*/.toUtf8();
        stream << "\" picture=\"";
        stream << result[i].row["photo"]/*.toHtmlEscaped()*/.toUtf8();
        stream << "\" />\n";
        ui->progressBar->setValue(ui->progressBar->value()+1);
    }
    stream << "\t</artists>\n</Tavrisu>";
    ui->progressBar->setValue(ui->progressBar->value()+1);
    m_label->setText(tr("Terminé"));

    file.close();

    return true;
}
