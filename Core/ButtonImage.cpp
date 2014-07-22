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

#include "ButtonImage.h"

ButtonImage::ButtonImage(QString text, QWidget* parent, int x, int y, int width, int height, QString path): m_text(text), m_parent(parent), m_picturePath(path)
{
    m_bouton = new QPushButton(m_parent);
    m_bouton->setGeometry(x, y , width, height);

    m_boxButtonPicture = new QHBoxLayout(m_bouton);

    m_labelButtonPicture = new QLabel(m_text);
    m_labelButtonPicture->setAlignment(Qt::AlignCenter);
    m_boxButtonPicture->addWidget(m_labelButtonPicture);
    m_labelButtonPicture->setWordWrap(true);

    if(!m_picturePath.isEmpty())
        setPicture(m_picturePath);

    connect(m_bouton, SIGNAL(clicked()), this, SLOT(openDialog()));

    m_bouton->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_bouton, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualMenu(QPoint)));
}


ButtonImage::~ButtonImage()
{

}


void ButtonImage::openDialog()
{
    QString basePath;
    if(m_picturePath.isEmpty())
        basePath = QDir::homePath();
    else
    {
        QFileInfo info(m_picturePath);
        basePath = info.absolutePath();
    }

    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Ouvrir la photo de l'artiste"),
                                                basePath,
                                                tr("Images (*.png *.jpg *.jpeg)"));

    if(!path.isEmpty())
    {
        m_picturePath = path;
        setPicture(m_picturePath);
    }
}


QString ButtonImage::path()
{
    return m_picturePath;
}


void ButtonImage::setPicture(QString path)
{
    if(!path.isEmpty())
    {
        m_labelButtonPicture->setPixmap(QPixmap(QPixmap(path).scaled(m_bouton->width()-16,
                                                                     m_bouton->height()-16, Qt::KeepAspectRatio)));
        m_picturePath = path;
    }
}


void ButtonImage::contextualMenu(QPoint point)
{
    QMenu* menu = new QMenu();

    QAction* pictureDelete = menu->addAction(tr("Supprimer l'image"));
    connect(pictureDelete, SIGNAL(triggered()), this, SLOT(pictureDelete()));


    menu->popup(m_bouton->mapToGlobal(point));
}


void ButtonImage::pictureDelete()
{
    m_labelButtonPicture->clear();
    m_labelButtonPicture->setText(m_text);
    m_labelButtonPicture->setAlignment(Qt::AlignCenter);

    m_picturePath = "";
}


void ButtonImage::setEnabled(bool enabled)
{
    m_bouton->setEnabled(enabled);
    m_labelButtonPicture->setEnabled(enabled);
}
