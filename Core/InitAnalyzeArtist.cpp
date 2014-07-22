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

#include "InitAnalyzeArtist.h"
#include "ui_InitAnalyzeArtist.h"

InitAnalyzeArtist::InitAnalyzeArtist(QSqliteCom* base, QWidget *parent) :
    QWidget(parent), ui(new Ui::InitAnalyzeArtist), m_base(base)
{
    ui->setupUi(this);

    m_model = new QStandardItemModel(0, 5);

    ui->table->setModel(m_model);
    ui->table->verticalHeader()->hide();
    ui->table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->table->horizontalHeader()->setStretchLastSection(true);
    ui->table->horizontalHeader()->setMovable(true);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table->setSelectionMode(QAbstractItemView::SingleSelection);
    QStringList labels;
    labels << "Nom" << "Type" << "Photo";
    m_model->setHorizontalHeaderLabels(labels);
    ui->table->horizontalHeader()->hideSection(3);
    ui->table->horizontalHeader()->hideSection(4);

    ui->table->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    ui->table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->table->setWordWrap(true);

    fillList();

    ui->progressBar->setMaximum(m_model->rowCount());

    m_buttonImage = new ButtonImage(tr("Sélectionnez une photo"), ui->wButtonImage, 76, 0, 101, 101);
    m_buttonImage->setEnabled(false);
}


InitAnalyzeArtist::~InitAnalyzeArtist()
{
    delete ui;
}


void InitAnalyzeArtist::fillList()
{
    std::vector<Row> result;
    result = m_base->simpleQueryResult("SELECT * FROM artistes WHERE id!=0 ORDER BY nom;");

    for(unsigned int i=0; i < result.size(); i++)
    {
        QList<QStandardItem*> row;
        QStandardItem* item = new QStandardItem(i, 1); // Name
        item->setText(result[i].row["nom"]);
        item->setEditable(false);
        row.append(item);
        item = new QStandardItem(i, 2); // Type
        item->setText(result[i].row["type"]);
        item->setEditable(false);
        row.append(item);
        item = new QStandardItem(i, 3); // Picture
        item->setText(result[i].row["photo"]);
        item->setEditable(false);
        row.append(item);
        item = new QStandardItem(i, 4); // Id
        item->setText(result[i].row["id"]);
        item->setEditable(false);
        row.append(item);
        item = new QStandardItem(i, 5); // Delete ? (0: no, 1: yes)
        item->setText("0");
        item->setEditable(false);
        row.append(item);

        m_model->appendRow(row);

        ui->table->verticalHeader()->resizeSection(i, 20);
    }
}


void InitAnalyzeArtist::on_table_clicked(QModelIndex index)
{
    ui->checkDelete->setEnabled(true);
    ui->formLayout->setEnabled(true);
    m_buttonImage->setEnabled(true);
    ui->labelName->setEnabled(true);
    ui->lineEdit->setEnabled(true);
    ui->labelType->setEnabled(true);
    ui->selectType->setEnabled(true);
    ui->buttonSave->setEnabled(true);

    m_selectedRow = index.row();

    ui->table->selectRow(m_selectedRow);

    ui->lineEdit->setText(m_model->item(m_selectedRow, 0)->text());
    if(m_model->item(m_selectedRow, 1)->text().isEmpty()) {
        int foundIndex = ui->selectType->findText("Homme");
        if(foundIndex != -1)
            ui->selectType->setCurrentIndex(foundIndex);
    }
    else {
        int foundIndex = ui->selectType->findText(m_model->item(m_selectedRow, 1)->text());
        if(foundIndex != -1)
            ui->selectType->setCurrentIndex(foundIndex);
    }
    if(m_model->item(m_selectedRow, 2)->text().isEmpty())
        m_buttonImage->pictureDelete();
    else
        m_buttonImage->setPicture(m_model->item(m_selectedRow, 2)->text());
    if(m_model->item(m_selectedRow, 4)->text() == "1")
        ui->checkDelete->setChecked(true);
    else
        ui->checkDelete->setChecked(false);


}


void InitAnalyzeArtist::on_checkDelete_clicked(bool checked)
{
    QColor red(244, 144, 144, 150);
    if(checked)
    {
        m_model->item(m_selectedRow, 0)->setBackground(QBrush(red));
        m_model->item(m_selectedRow, 1)->setBackground(QBrush(red));
        m_model->item(m_selectedRow, 2)->setBackground(QBrush(red));
        m_model->setItem(m_selectedRow, 4, new QStandardItem("1"));

        if(m_selectedRow != (m_model->rowCount()-1))
            on_table_clicked(m_model->index(m_selectedRow+1, 0));
    }
    else
    {
        m_model->item(m_selectedRow, 0)->setBackground(QBrush());
        m_model->item(m_selectedRow, 1)->setBackground(QBrush());
        m_model->item(m_selectedRow, 2)->setBackground(QBrush());
        m_model->setItem(m_selectedRow, 4, new QStandardItem("0"));
    }
}


void InitAnalyzeArtist::on_buttonSave_clicked()
{
    m_model->setItem(m_selectedRow, 0, new QStandardItem(ui->lineEdit->text()));
    m_model->setItem(m_selectedRow, 1, new QStandardItem(ui->selectType->currentText()));
    m_model->setItem(m_selectedRow, 2, new QStandardItem(m_buttonImage->path()));

    if(m_selectedRow != (m_model->rowCount()-1))
        on_table_clicked(m_model->index(m_selectedRow+1, 0));
}


void InitAnalyzeArtist::on_lineEdit_textChanged(QString text)
{
    if(text.isEmpty())
        ui->buttonSave->setEnabled(false);
    else
        ui->buttonSave->setEnabled(true);
}


void InitAnalyzeArtist::on_buttonValid_clicked()
{
    bool allComplete = true;

    for(int i=0; i<m_model->rowCount(); i++)
    {
        if(m_model->item(i, 1)->text().isEmpty() && m_model->item(i, 4)->text()=="0")
            allComplete = false;
    }

    if(!allComplete)
    {
        int reponse = QMessageBox::warning(this,
                                            tr("Attention"), tr("Certains artistes n'ont pas été complétés. Si vous validez maintenant, leur nom sera gardé tel quel, leur type sera <i>Homme</i> et n'auront pas de photo.<br />Êtes-vous sûr de vouloir valider maintenant ?"),
                                            QMessageBox::Yes|QMessageBox::No);
        if(reponse == QMessageBox::No)
            return;
    }

    for(int i=0; i<m_model->rowCount(); i++)
    {
        if(m_model->item(i, 4)->text() == QString("1"))
        {
            std::queue<QString> liste;
            liste.push("INT");
            liste.push(m_model->item(i, 3)->text()); // Artist id
            m_base->preparedQuery("DELETE FROM artistes WHERE id=?;", liste);

            m_base->preparedQuery("UPDATE albums SET artiste_id=0 WHERE artiste_id=?;", liste);
            m_base->preparedQuery("UPDATE pistes SET artiste_id=0 WHERE artiste_id=?;", liste);
        }
        else
        {
            std::queue<QString> liste;
            liste.push("STRING");
            liste.push(m_model->item(i, 0)->text());
            liste.push("STRING");
            if(m_model->item(i, 1)->text().isEmpty())
                liste.push(tr("Homme"));
            else
                liste.push(m_model->item(i, 1)->text());
            if(!m_model->item(i, 2)->text().isEmpty())
            {
                liste.push("STRING");
                liste.push(m_model->item(i, 2)->text());
            }
            else
                liste.push("EMPTY");
            liste.push("INT");
            liste.push(m_model->item(i, 3)->text());
            m_base->preparedQuery("UPDATE artistes SET nom=?, type=?, photo=? WHERE id=?", liste);
        }
        ui->progressBar->setValue(ui->progressBar->value()+1);
    }
    emit next();
}


void InitAnalyzeArtist::resizeEvent(QResizeEvent *event)
{
    ui->table->horizontalHeader()->setDefaultSectionSize((ui->table->width()-100)/2);
    ui->table->horizontalHeader()->resizeSection(1, 100); // Type
}
