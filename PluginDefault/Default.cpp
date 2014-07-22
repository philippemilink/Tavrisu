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

#include "Default.h"

Default::Default()
{
    m_name = "Défaut";
    m_creator = "Tavrisu";
    m_version = "0.1 - bêta 1";
    m_description = "Interface par défaut de Tavrisu, sous forme de liste.";
    m_descriptionImagePath = PLUGIN_PATH + "Default.png";

    m_currentId = -1;
    m_selectedRow = -1;

    m_bgColor = QColor(206, 251, 219, 150);


    QHBoxLayout* mainLayout = new QHBoxLayout();
    this->setLayout(mainLayout);

    // Commands for the player:
    m_player = new Player(this);
    mainLayout->addWidget(m_player);

    connect(m_player, SIGNAL(pause()), this, SLOT(pauseSlot()));
    connect(m_player, SIGNAL(next()), this, SLOT(nextSlot()));
    connect(m_player, SIGNAL(previous()), this, SLOT(previousSlot()));
    connect(m_player, SIGNAL(positionChanged(int)), this, SIGNAL(changePosition(int)));
    connect(m_player, SIGNAL(play()), this, SLOT(emptyPlay()));



    // The table:
    m_model = new QStandardItemModel(0, 11);

    m_listView = new QTableView();
    m_listView->setModel(m_model);
    m_listView->verticalHeader()->hide();
    m_listView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_listView->horizontalHeader()->setStretchLastSection(true);
    m_listView->horizontalHeader()->setMovable(true);
    m_listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    QStringList labels;
    labels << "" << "Titre" << "Artiste" << "Album" << "Durée" << "" << "" << "";
    m_model->setHorizontalHeaderLabels(labels);
    m_listView->horizontalHeader()->hideSection(5);
    m_listView->horizontalHeader()->hideSection(6);
    m_listView->horizontalHeader()->hideSection(7);
    m_listView->horizontalHeader()->hideSection(8);
    m_listView->horizontalHeader()->hideSection(9);
    m_listView->horizontalHeader()->hideSection(10);


    m_listView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);

    m_listView->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_listView->setWordWrap(true);

    connect(m_listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClickSlot(QModelIndex)));
    connect(m_listView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualMenu(QPoint)));

    mainLayout->addWidget(m_listView);
}


Default::~Default()
{

}


void Default::setBase(QSqliteCom *base)
{
    m_base = base;
    fillList();
}


void Default::fillList()
{
    std::vector<Row> result;
    result = m_base->simpleQueryResult("SELECT pistes.id AS pid, pistes.titre AS ptitre, pistes.artiste_id AS parid, artistes.nom AS anom, pistes.album_id AS palid, albums.titre AS atitre, pistes.numero AS pnumero, pistes.path AS ppath, pistes.duration AS ptime, albums.photo AS photo FROM pistes, artistes, albums WHERE albums.id = pistes.album_id AND artistes.id = pistes.artiste_id ORDER BY anom, atitre, pnumero;");

    for(unsigned int i=0; i < result.size(); i++)
    {
        QList<QStandardItem*> row;
        QStandardItem* item = new QStandardItem(i, 1); // Number of track in the album
        item->setText(result[i].row["pnumero"]);
        item->setTextAlignment(Qt::AlignVCenter|Qt::AlignRight);
        item->setEditable(false);
        row.append(item);
        item = new QStandardItem(i, 2); // Title
        item->setText(result[i].row["ptitre"]);
        item->setEditable(false);
        row.append(item);
        item = new QStandardItem(i, 3); // Artist's name
        if(result[i].row["anom"].isEmpty())
            item->setText(tr("Artiste inconnu"));
        else
            item->setText(result[i].row["anom"]);
        item->setEditable(false);
        row.append(item);
        item = new QStandardItem(i, 4); // Album
        if(result[i].row["atitre"].isEmpty())
            item->setText(tr("Album inconnu"));
        else
            item->setText(result[i].row["atitre"]);
        item->setEditable(false);
        row.append(item);
        item = new QStandardItem(i, 5); // Time
        item->setText(convertTime(result[i].row["ptime"].toInt()));
        item->setEditable(false);
        row.append(item);
        item = new QStandardItem(i, 6); // Path
        item->setText(result[i].row["ppath"]);
        row.append(item);
        item = new QStandardItem(i, 7); // Id
        item->setText(result[i].row["pid"]);
        row.append(item);
        item = new QStandardItem(i, 8); // Album cover
        item->setText(result[i].row["photo"]);
        row.append(item);
        item = new QStandardItem(i, 9); // Artist id
        item->setText(result[i].row["parid"]);
        row.append(item);
        item = new QStandardItem(i, 10); // Album id
        item->setText(result[i].row["palid"]);
        row.append(item);
        item = new QStandardItem(i, 11); // Duration in ms
        item->setText(result[i].row["ptime"]);
        row.append(item);

        m_model->appendRow(row);

        if(result[i].row["pid"].toInt() == m_currentId)
        {
            m_selectedRow = i;
            if(m_player->isPlaying())
                m_model->item(m_selectedRow, 0)->setIcon(QIcon(PLUGIN_PATH + "play.png"));
            else
                m_model->item(m_selectedRow, 0)->setIcon(QIcon(PLUGIN_PATH + "pause.png"));

            for(int i=0; i<=4; i++)
                m_model->item(m_selectedRow, i)->setBackground(QBrush(m_bgColor));
        }

        m_listView->verticalHeader()->resizeSection(i, 20);
    }
}


void Default::updateList()
{
    m_model->removeRows(0, m_model->rowCount());

    fillList();
}


QString Default::getName()
{
    return m_name;
}


QString Default::getCreator()
{
    return m_creator;
}


QString Default::getVersion()
{
    return m_version;
}


QString Default::getDescription()
{
    return m_description;
}


QString Default::getDescriptionImage()
{
    return m_descriptionImagePath;
}




void Default::doubleClickSlot(QModelIndex index)
{
    if(m_selectedRow!=-1)
    {
        m_model->item(m_selectedRow, 0)->setIcon(QIcon());
        for(int i=0; i<=4; i++)
            m_model->item(m_selectedRow, i)->setBackground(QBrush());
    }
    m_selectedRow = index.row();
    m_currentId = m_model->item(m_selectedRow, 6)->text().toInt();
    selectRow();
}


void Default::contextualMenu(QPoint point)
{
    QItemSelectionModel *selection = m_listView->selectionModel();
    m_indexSelection = selection->currentIndex();


    QMenu* menu = new QMenu();

    if(m_model->item(m_indexSelection.row(), 9)->text().toInt() == 0)
    {
        QAction* trackEdit = menu->addAction("Modifier le fichier audio");
        connect(trackEdit, SIGNAL(triggered()), this, SLOT(trackEditSlot()));
        QAction* trackDelete = menu->addAction("Supprimer le fichier audio");
        connect(trackDelete, SIGNAL(triggered()), this, SLOT(trackDeleteSlot()));
        if(m_indexSelection.row() == m_selectedRow)
            trackDelete->setDisabled(true);
    }
    if(m_model->item(m_indexSelection.row(), 9)->text().toInt() != 0)
    {
        menu->addSeparator();
        QAction* albumEdit = menu->addAction("Editer l'album");
        connect(albumEdit, SIGNAL(triggered()), this, SLOT(albumEditSlot()));
        QAction* albumDelete = menu->addAction("Supprimer l'album");
        connect(albumDelete, SIGNAL(triggered()), this, SLOT(albumDeleteSlot()));
        if(m_selectedRow!=-1)
        {
            if(m_model->item(m_indexSelection.row(), 9)->text() == m_model->item(m_selectedRow, 9)->text())
            {
                albumDelete->setDisabled(true);
                albumEdit->setDisabled(true);
            }
        }
    }
    if(m_model->item(m_indexSelection.row(), 8)->text().toInt() != 0)
    {
        menu->addSeparator();
        QAction* artistEdit = menu->addAction("Editer l'artiste");
        connect(artistEdit, SIGNAL(triggered()), this, SLOT(artistEditSlot()));
        QAction* artistDelete = menu->addAction("Supprimer l'artiste");
        connect(artistDelete, SIGNAL(triggered()), this, SLOT(artistDeleteSlot()));
        if(m_selectedRow!=-1)
        {
            if(m_model->item(m_indexSelection.row(), 2)->text() == m_model->item(m_selectedRow, 2)->text())
                artistDelete->setDisabled(true);
        }
    }

    menu->popup(m_listView->mapToGlobal(point));
}


void Default::trackDeleteSlot()
{
    emit trackDelete(m_model->item(m_indexSelection.row(), 6)->text().toInt(), m_model->item(m_indexSelection.row(), 1)->text());
}


void Default::trackEditSlot()
{
    emit trackEdit(m_model->item(m_indexSelection.row(), 6)->text().toInt());
}


void Default::albumDeleteSlot()
{
    emit albumDelete(m_model->item(m_indexSelection.row(), 9)->text().toInt(), m_model->item(m_indexSelection.row(), 3)->text());
}


void Default::albumEditSlot()
{
    emit albumEdit(m_model->item(m_indexSelection.row(), 9)->text().toInt());
}


void Default::artistDeleteSlot()
{
    emit artistDelete(m_model->item(m_indexSelection.row(), 8)->text().toInt(), m_model->item(m_indexSelection.row(), 2)->text());
}


void Default::artistEditSlot()
{
    emit artistEdit(m_model->item(m_indexSelection.row(), 8)->text().toInt());
}


void Default::positionChanged(int position)
{
    m_player->changePosition(position);
}


void Default::nextSlot()
{

    if(m_selectedRow < (m_model->rowCount()-1))
    {
        m_model->item(m_selectedRow, 0)->setIcon(QIcon());
        for(int i=0; i<=4; i++) {
            m_model->item(m_selectedRow, i)->setBackground(QBrush());
        }
        m_selectedRow++;
        m_currentId = m_model->item(m_selectedRow, 6)->text().toInt();
        selectRow();
    }
}


void Default::previousSlot()
{
    if(m_selectedRow > 0)
    {
        m_model->item(m_selectedRow, 0)->setIcon(QIcon());
        for(int i=0; i<=4; i++)
            m_model->item(m_selectedRow, i)->setBackground(QBrush());
        m_selectedRow--;
        m_currentId = m_model->item(m_selectedRow, 6)->text().toInt();
        selectRow();
    }
}


void Default::selectRow()
{
    if(QFile::exists(m_model->item(m_selectedRow, 5)->text()))
    {
        if(m_model->item(m_selectedRow, 3)->text().isEmpty())
            m_player->setInfos(m_model->item(m_selectedRow, 1)->text(), // title
                               m_model->item(m_selectedRow, 10)->text().toInt(), // duration
                               m_model->item(m_selectedRow, 2)->text()); // artist
        else
            m_player->setInfos(m_model->item(m_selectedRow, 1)->text(), // title
                               m_model->item(m_selectedRow, 10)->text().toInt(), // duration
                               m_model->item(m_selectedRow, 2)->text(), // artist
                               m_model->item(m_selectedRow, 7)->text(), // album cover
                               m_model->item(m_selectedRow, 3)->text()); // album

        emit play(m_model->item(m_selectedRow, 5)->text());
        m_model->item(m_selectedRow, 0)->setIcon(QIcon(PLUGIN_PATH + "play.png"));

        for(int i=0; i<=4; i++)
            m_model->item(m_selectedRow, i)->setBackground(QBrush(m_bgColor));

    }
    else
    {
        if(m_selectedRow < (m_model->rowCount()-1))
        {
            nextSlot();
            m_model->item(m_selectedRow-1, 0)->setIcon(QIcon(PLUGIN_PATH + "error_file.png"));
        }
        else
            m_model->item(m_selectedRow, 0)->setIcon(QIcon(PLUGIN_PATH + "error_file.png"));
    }
}


void Default::pauseSlot()
{
    if(m_player->isPlaying())
        m_model->item(m_selectedRow, 0)->setIcon(QIcon(PLUGIN_PATH + "play.png"));
    else
        m_model->item(m_selectedRow, 0)->setIcon(QIcon(PLUGIN_PATH + "pause.png"));

    emit(pause());
}


void Default::endTrack()
{
    nextSlot();
}


void Default::resizeEvent(QResizeEvent *event)
{
    m_listView->horizontalHeader()->setDefaultSectionSize((m_listView->width()-120)/3);
    m_listView->horizontalHeader()->resizeSection(0, 50); // Number
    m_listView->horizontalHeader()->resizeSection(4, 50); // Duration
}


void Default::emptyPlay()
{
    if(m_model->rowCount()>0)
        doubleClickSlot(m_model->index(0, 0));
}

Q_EXPORT_PLUGIN2("Default", Default)
