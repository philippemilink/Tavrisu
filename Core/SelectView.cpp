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

#include "SelectView.h"
#include "ui_SelectView.h"


SelectView::SelectView(QSettings* settings, QWidget *parent) : QDialog(parent), ui(new Ui::SelectView), m_settings(settings)
{
    ui->setupUi(this);

    QFont fontTitle;
    fontTitle.setBold(true);
    fontTitle.setPixelSize(22);
    ui->labelTitle->setFont(fontTitle);

    fillList(); // Populate listWidget
}


SelectView::~SelectView()
{
    delete ui;
}


void SelectView::fillList()
{
    QDir plugDir = QDir(qApp->applicationDirPath() + "/views/");
    foreach(QString folder, plugDir.entryList(QDir::Dirs|QDir::NoDot|QDir::NoDotDot|QDir::NoDotAndDotDot))
    {
        //QFileInfo fileInfo(plugDir, file);
        ui->listWidget->addItem(folder);
    }
}


void SelectView::on_listWidget_clicked(QModelIndex modelIndex)
{
    ViewPluginInterface* plugin = PluginLoader::pluginByName<ViewPluginInterface>(qApp->applicationDirPath() + "/views/" + modelIndex.data().toString() + "/" + modelIndex.data().toString() + ".dll");
    ui->labelTitle->setText(plugin->getName());

    QString stringLabelDescription = QString(tr("<strong>Description:</strong> ") + plugin->getDescription());
    stringLabelDescription += tr("<br /><strong>Version:</strong> ") + plugin->getVersion();
    stringLabelDescription += tr("<br /><strong>Auteur:</strong> ") + plugin->getCreator();
    ui->labelDescription->setText(stringLabelDescription);

    ui->labelPicture->setPixmap(QPixmap(plugin->getDescriptionImage()));

    ui->buttonSelect->setEnabled(true);
}


void SelectView::on_buttonSelect_clicked()
{
    m_settings->setValue("interface", ui->listWidget->selectionModel()->currentIndex().data().toString());
    this->destroy();
}
