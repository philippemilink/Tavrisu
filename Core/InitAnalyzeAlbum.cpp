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

#include "InitAnalyzeAlbum.h"
#include "ui_InitAnalyzeAlbum.h"


InitAnalyzeAlbum::InitAnalyzeAlbum(QSqliteCom *base, int lastTrackId, QWidget *parent) :
    QWidget(parent), ui(new Ui::InitAnalyzeAlbum), m_base(base), m_lastTrackId(lastTrackId)
{
    ui->setupUi(this);

    m_wAlbum = new AlbumWidget(ui->widgetAlbum);
    m_wAlbum->setEnabled(false);
    m_wAlbum->hideCancelButton(true);
    connect(m_wAlbum, SIGNAL(submit()), this, SLOT(submitSlot()));

    m_vlc = new QlibVLC();

    m_model = new QStandardItemModel(0, 7);

    ui->table->setModel(m_model);
    ui->table->verticalHeader()->hide();
    ui->table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->table->horizontalHeader()->setStretchLastSection(true);
    ui->table->horizontalHeader()->setMovable(true);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table->setSelectionMode(QAbstractItemView::SingleSelection);
    QStringList labels;
    labels << tr("Titre") << tr("Artiste") << tr("Date") << tr("Pochette");
    m_model->setHorizontalHeaderLabels(labels);
    ui->table->horizontalHeader()->hideSection(4);
    ui->table->horizontalHeader()->hideSection(5);
    ui->table->horizontalHeader()->hideSection(6);
    ui->table->horizontalHeader()->hideSection(7);

    ui->table->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    ui->table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->table->setWordWrap(true);

    fillList();

    // Set artists of the database in the selector (QComboBox)
    m_wAlbum->getArtistCombo()->addItem(tr("Artiste inconnu"));
    std::vector<Row> result;
    result = m_base->simpleQueryResult("SELECT nom FROM artistes ORDER BY nom;");
    for(unsigned int i=1; i < result.size(); i++)
    {
        m_wAlbum->getArtistCombo()->addItem(result[i].row["nom"]);
    }
}


InitAnalyzeAlbum::~InitAnalyzeAlbum()
{
    delete ui;
}


void InitAnalyzeAlbum::fillList()
{
    std::vector<Row> result;
    result = m_base->simpleQueryResult("SELECT albums.id AS alid, albums.titre AS title, albums.artiste_id AS arid, artistes.nom AS nom, albums.photo AS photo, albums.publication AS adate FROM artistes, albums WHERE albums.id != 0 AND albums.artiste_id = artistes.id ORDER BY title;");

    for(unsigned int i=0; i < result.size(); i++)
    {
        QList<QStandardItem*> row;
        QStandardItem* item = new QStandardItem(i, 1); // Title
        item->setText(result[i].row["title"]);
        item->setEditable(false);
        row.append(item);
        item = new QStandardItem(i, 2); // Artist
        if(result[i].row["arid"].toInt() == 0)
            item->setText(tr("Artiste inconnu"));
        else
            item->setText(result[i].row["nom"]);
        item->setEditable(false);
        row.append(item);
        item = new QStandardItem(i, 3); // Date
        item->setText(result[i].row["adate"]);
        item->setEditable(false);
        row.append(item);
        item = new QStandardItem(i, 4); // Picture
        item->setText(result[i].row["photo"]);
        item->setEditable(false);
        row.append(item);
        item = new QStandardItem(i, 5); // Id of album
        item->setText(result[i].row["alid"]);
        item->setEditable(false);
        row.append(item);
        item = new QStandardItem(i, 6); // Id of artist
        item->setText(result[i].row["arid"]);
        item->setEditable(false);
        row.append(item);
        item = new QStandardItem(i, 7); // Delete ? (0: no, 1: yes)
        item->setText("0");
        item->setEditable(false);
        row.append(item);

        m_model->appendRow(row);

        ui->table->verticalHeader()->resizeSection(i, 20);
    }
}


void InitAnalyzeAlbum::on_table_clicked(QModelIndex index)
{
    m_wAlbum->setEnabled(true);
    ui->checkDeleteAlbum->setEnabled(true);

    m_selectedRow = index.row();

    ui->table->selectRow(m_selectedRow);

    m_wAlbum->setTitle(m_model->item(m_selectedRow, 0)->text());
    int foundIndex = m_wAlbum->getArtistCombo()->findText(m_model->item(m_selectedRow, 1)->text());
    if(foundIndex != -1)
        m_wAlbum->getArtistCombo()->setCurrentIndex(foundIndex);

    if(m_model->item(m_selectedRow, 3)->text().isEmpty())
        m_wAlbum->unsetPicture();
    else
        m_wAlbum->setImagePath(m_model->item(m_selectedRow, 3)->text());
    if(m_model->item(m_selectedRow, 2)->text().isEmpty())
        m_wAlbum->setDate(QDate::currentDate().year());
    else
        m_wAlbum->setDate(m_model->item(m_selectedRow, 2)->text().toInt());
    if(m_model->item(m_selectedRow, 6)->text() == "1")
        ui->checkDeleteAlbum->setChecked(true);
    else
        ui->checkDeleteAlbum->setChecked(false);

    // Set tracks in the QTableView:
    for(unsigned int i=m_wAlbum->getTable()->rowCount(); i>0; i--)
        m_wAlbum->getTable()->removeRow(i-1);

    std::queue<QString> list;
    list.push("INT");
    list.push(m_model->item(m_selectedRow, 4)->text()); // Album id

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
}


void InitAnalyzeAlbum::on_checkDeleteAlbum_clicked(bool checked)
{
    QColor red(244, 144, 144, 150);
    if(checked)
    {
        m_model->item(m_selectedRow, 0)->setBackground(QBrush(red));
        m_model->item(m_selectedRow, 1)->setBackground(QBrush(red));
        m_model->item(m_selectedRow, 2)->setBackground(QBrush(red));
        m_model->item(m_selectedRow, 3)->setBackground(QBrush(red));
        m_model->setItem(m_selectedRow, 6, new QStandardItem("1"));

        if(m_selectedRow != (m_model->rowCount()-1))
            on_table_clicked(m_model->index(m_selectedRow+1, 0));
    }
    else
    {
        m_model->item(m_selectedRow, 0)->setBackground(QBrush());
        m_model->item(m_selectedRow, 1)->setBackground(QBrush());
        m_model->item(m_selectedRow, 2)->setBackground(QBrush());
        m_model->item(m_selectedRow, 3)->setBackground(QBrush());
        m_model->setItem(m_selectedRow, 6, new QStandardItem("0"));
    }
}


void InitAnalyzeAlbum::submitSlot()
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
    liste.push(m_model->item(m_selectedRow, 4)->text()); // Album id

    if(!m_base->preparedQuery("UPDATE albums SET titre=?, artiste_id=?, publication=?, photo=? WHERE id=?", liste))
    {
        QMessageBox::critical(0, tr("Edition de l'album"),
                              tr("<strong>Erreur:</strong><br />Erreur lors de l'édition de l'album.<br />") + m_base->lastError());
        return;

    }

    // Third: delete tracks in database
    std::queue<QString> listTracksDelete;
    listTracksDelete.push("INT");
    listTracksDelete.push(m_model->item(m_selectedRow, 4)->text());

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
        listeTrack.push(m_model->item(m_selectedRow, 4)->text()); // Id of the album
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
            listeDelete.push(m_model->item(m_selectedRow, 4)->text()); // Id of the album
            m_base->preparedQuery("DELETE FROM albums WHERE id=?;", listeDelete);
            m_base->preparedQuery("DELETE FROM pistes WHERE album_id=?;", listeDelete);
            return;
        }
        ++m_lastTrackId;
    }


    // Update fields in the view:
    m_model->setItem(m_selectedRow, 0, new QStandardItem(m_wAlbum->getTitle()));
    m_model->setItem(m_selectedRow, 1, new QStandardItem(m_wAlbum->getArtistCombo()->currentText()));
    m_model->setItem(m_selectedRow, 2, new QStandardItem(QString::number(m_wAlbum->getDate())));
    m_model->setItem(m_selectedRow, 3, new QStandardItem(m_wAlbum->getImagePath()));

    if(m_selectedRow != (m_model->rowCount()-1))
        on_table_clicked(m_model->index(m_selectedRow+1, 0));
}


void InitAnalyzeAlbum::on_buttonNext_clicked()
{
    for(int i=0; i<m_model->rowCount(); i++)
    {
        if(m_model->item(i, 6)->text() == QString("1"))
        {
            std::queue<QString> liste;
            liste.push("INT");
            liste.push(m_model->item(i, 4)->text()); // Album id
            m_base->preparedQuery("DELETE FROM albums WHERE id=?;", liste);
            m_base->preparedQuery("DELETE FROM pistes WHERE album_id=?;", liste);
        }
    }
    emit next(ui->checkDeleteTracks->isChecked(), m_lastTrackId);
}


void InitAnalyzeAlbum::resizeEvent(QResizeEvent* event)
{
    ui->table->horizontalHeader()->setDefaultSectionSize((ui->table->width()-50)/3);
    ui->table->horizontalHeader()->resizeSection(2, 50); // Date
}
