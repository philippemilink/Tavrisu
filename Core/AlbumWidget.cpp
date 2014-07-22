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


#include "AlbumWidget.h"
#include "ui_AlbumWidget.h"


Track::Track(QString path, int number) : m_path(path), m_number(number)
{
    QFileInfo file(m_path);
    m_title = file.completeBaseName();
}


QString Track::title()
{
    return m_title;
}


int Track::number()
{
    return m_number;
}


QString Track::path()
{
    return m_path;
}


bool Track::isSmaller(Track const& b) const
{
    if(m_number == -1 && b.m_number != -1)
        return false;
    else if(m_number != -1 && b.m_number == -1)
        return true;
    else if(m_number != -1 && b.m_number != -1)
    {
        if(m_number < b.m_number)
            return true;
        else
            return false;
    }
    else // Comparaison des titres:
    {
         if(m_title.compare(b.m_title) < 0)
            return true;
         else
            return false;
    }
}


bool operator<(Track const& a, Track const& b)
{
    if(a.isSmaller(b))
        return true;
    else
        return false;
}


AlbumWidget::AlbumWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::AlbumWidget)
{
    ui->setupUi(this);

    m_buttonImage = new ButtonImage(tr("Ouvrir l'image de l'album"), this, 10, 10, 91, 91);

    m_previousPath = QDir::homePath();

    m_vlc = new QlibVLC();


    // Set date selector parameters
    ui->selectDate->setValue(QDate::currentDate().year());
    ui->selectDate->setMaximum(QDate::currentDate().year());


    // Configure the QTableView for audio tracks
    ui->listeChanson->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->listeChanson->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listeChanson->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->listeChanson->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->listeChanson->horizontalHeader()->hideSection(1);



    ui->boutonHaut->setIcon(QIcon("haut.png"));
    ui->boutonBas->setIcon(QIcon("bas.png"));
    ui->boutonSupprimer->setIcon(QIcon("moins.png"));
    ui->boutonAjouter->setIcon(QIcon("plus.png"));
    ui->buttonEdit->setIcon(QIcon("edit.png"));

    ui->buttonEdit->setEnabled(false);

    connect(ui->lineEditTitre, SIGNAL(textChanged(QString)), this, SLOT(verifyButtonSubmit()));
    connect(ui->buttonCancel, SIGNAL(clicked()), this, SIGNAL(cancel()));
    connect(ui->buttonSubmit, SIGNAL(clicked()), this, SIGNAL(submit()));
}


AlbumWidget::~AlbumWidget()
{
    delete ui;
}


void AlbumWidget::on_boutonAjouter_clicked()
{
    bool haveNumber = true;
    int row = ui->listeChanson->rowCount();
    std::vector<Track> trackArray;

    QStringList path = QFileDialog::getOpenFileNames(this,
                                                tr("Ouvrir une piste de l'album"),
                                                m_previousPath,
                                                tr("Musiques (*.mp3 *.wav *.ogg *.flac *.acc)"));

    if(!path.isEmpty())
    {
        // The next code manages information about selected files.
        QStringList::Iterator it = path.begin();

        while(it != path.end())
        {
            m_vlc->setMedia(*it);

            if(!m_vlc->album().isNull())
                ui->lineEditTitre->setText(m_vlc->album());
            if(m_vlc->year() != 0)
                ui->selectDate->setValue(m_vlc->year());
            if(!m_vlc->artist().isNull()) {
                int foundIndex = ui->selectArtiste->findText(m_vlc->artist());
                if(foundIndex != -1)
                    ui->selectArtiste->setCurrentIndex(foundIndex);
            }
            if(m_vlc->trackNumber() == -1)
            {
                if(haveNumber == true)
                {
                    QMessageBox::information(this, tr("Pas de numérotation"), tr("Un ou plusieurs fichiers audio n'ont pas de numéro de piste.<br />Les fichiers audio seront donc classés d'abord par ordre croissant avec leur numéro puis alphabétiquement pour ceux qui n'ont pas de numéro."));
                    haveNumber = false;
                }
            }
            trackArray.push_back(Track(*it, m_vlc->trackNumber()));
            ++it;
        }

        sort(trackArray.begin(), trackArray.end());

        // The next code adds files in the QTableView:
        ui->listeChanson->setRowCount(row + path.length());
        for(unsigned int i=0; i<trackArray.size(); ++i)
        {
            ui->listeChanson->setItem(row, 0, new QTableWidgetItem(trackArray[i].title()));
            ui->listeChanson->setItem(row, 1, new QTableWidgetItem(trackArray[i].path()));
            ++row;
        }
    }
    verifyButtonSubmit(); // Verify if all data are available to save the album
}


void AlbumWidget::on_listeChanson_clicked()
{
    ui->boutonBas->setEnabled(false);
    ui->boutonHaut->setEnabled(false);
    ui->boutonSupprimer->setEnabled(true);
    ui->buttonEdit->setEnabled(true);

    if(ui->listeChanson->rowCount() > 1)
    {
        // If there are more than one track selected, this code manages enabled buttons (move at top and move at bottom)
        QItemSelectionModel *selection = ui->listeChanson->selectionModel();
        QModelIndex indexElementSelectionne = selection->currentIndex();
        int trackSelected = indexElementSelectionne.row() + 1;

        if(trackSelected == 1)
            ui->boutonBas->setEnabled(true);
        else if(trackSelected == ui->listeChanson->rowCount())
            ui->boutonHaut->setEnabled(true);
        else
        {
            ui->boutonBas->setEnabled(true);
            ui->boutonHaut->setEnabled(true);
        }
    }
}


void AlbumWidget::on_boutonSupprimer_clicked()
{
    QItemSelectionModel *selection = ui->listeChanson->selectionModel();
    QModelIndex indexElementSelectionne = selection->currentIndex();
    int itemSelected = indexElementSelectionne.row();

    ui->listeChanson->removeRow(itemSelected);

    ui->boutonBas->setEnabled(false);
    ui->boutonHaut->setEnabled(false);
    ui->boutonSupprimer->setEnabled(false);
    ui->buttonEdit->setEnabled(false);

    verifyButtonSubmit(); // Verify if all data are available to save the album
}


void AlbumWidget::on_boutonHaut_clicked()
{
    moveItem(true);
}


void AlbumWidget::on_boutonBas_clicked()
{
    moveItem(false);
}


void AlbumWidget::moveItem(bool up)
{
    Q_ASSERT(ui->listeChanson->selectedItems().count() > 0);
    const int sourceRow = ui->listeChanson->row(ui->listeChanson->selectedItems().at(0));
    const int destRow = (up ? sourceRow-1 : sourceRow+1);
    Q_ASSERT(destRow >= 0 && destRow < ui->listeChanson->rowCount());

    // take whole rows
    QList<QTableWidgetItem*> sourceItems;
    for (int col = 0; col < ui->listeChanson->columnCount(); ++col)
    {
        sourceItems << ui->listeChanson->takeItem(sourceRow, col);
    }
    QList<QTableWidgetItem*> destItems;
    for (int col = 0; col < ui->listeChanson->columnCount(); ++col)
    {
        destItems << ui->listeChanson->takeItem(destRow, col);
    }

    // set back in reverse order
    for (int col = 0; col < ui->listeChanson->columnCount(); ++col)
    {
        ui->listeChanson->setItem(sourceRow, col, destItems.at(col));
    }
    for (int col = 0; col < ui->listeChanson->columnCount(); ++col)
    {
        ui->listeChanson->setItem(destRow, col, sourceItems.at(col));
    }

    ui->listeChanson->setCurrentCell(destRow, 0);
    if(destRow == 0)
        ui->boutonHaut->setEnabled(false);
    else
        ui->boutonHaut->setEnabled(true);
    if(destRow == ui->listeChanson->rowCount()-1)
        ui->boutonBas->setEnabled(false);
    else
        ui->boutonBas->setEnabled(true);
}


void AlbumWidget::verifyButtonSubmit()
{
    if(!ui->lineEditTitre->text().isEmpty() && ui->listeChanson->rowCount() > 0)
        ui->buttonSubmit->setEnabled(true);
    else
        // If the title is empty and/or there is no track in QTableView
        ui->buttonSubmit->setEnabled(false);
}


QTableWidget* AlbumWidget::getTable()
{
    return ui->listeChanson;
}


QComboBox* AlbumWidget::getArtistCombo()
{
    return ui->selectArtiste;
}


QString AlbumWidget::getTitle()
{
    return ui->lineEditTitre->text();
}


int AlbumWidget::getDate()
{
    return ui->selectDate->value();
}


QString AlbumWidget::getImagePath()
{
    return m_buttonImage->path();
}


void AlbumWidget::setTitle(QString title)
{
    ui->lineEditTitre->setText(title);
}


void AlbumWidget::setImagePath(QString path)
{
    m_buttonImage->setPicture(path);
}


void AlbumWidget::setDate(int date)
{
    ui->selectDate->setValue(date);
}


void AlbumWidget::setEnabled(bool enabled)
{
    m_buttonImage->setEnabled(enabled);
    ui->labelTitre->setEnabled(enabled);
    ui->lineEditTitre->setEnabled(enabled);
    ui->labelArtiste->setEnabled(enabled);
    ui->selectArtiste->setEnabled(enabled);
    ui->labelDate->setEnabled(enabled);
    ui->selectDate->setEnabled(enabled);
    ui->boutonAjouter->setEnabled(enabled);
    ui->listeChanson->setEnabled(enabled);
    ui->buttonCancel->setEnabled(enabled);
    ui->buttonSubmit->setEnabled(enabled);
}


void AlbumWidget::hideCancelButton(bool hide)
{
    if(hide)
        ui->buttonCancel->hide();
    else
        ui->buttonCancel->show();
}


void AlbumWidget::unsetPicture()
{
    m_buttonImage->pictureDelete();
}


void AlbumWidget::on_buttonEdit_clicked()
{
    QItemSelectionModel *selection = ui->listeChanson->selectionModel();
    QModelIndex indexElementSelectionne = selection->currentIndex();

    ui->listeChanson->editItem(ui->listeChanson->item(indexElementSelectionne.row(), 0));
}


void AlbumWidget::on_listeChanson_itemSelectionChanged()
{
    ui->boutonBas->setEnabled(false);
    ui->boutonHaut->setEnabled(false);
    ui->boutonSupprimer->setEnabled(false);
    ui->buttonEdit->setEnabled(false);
}
