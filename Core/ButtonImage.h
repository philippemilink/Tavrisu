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

#ifndef BUTTONIMAGE_H
#define BUTTONIMAGE_H

#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QLabel>
#include <QHBoxLayout>
#include <QMenu>
#include <QFileInfo>


class ButtonImage : public QWidget
{
    Q_OBJECT

    public:
    ButtonImage(QString text, QWidget* parent, int x, int y, int width, int height, QString path = "");
    ~ButtonImage();
    QString path();


    public slots:
    void setEnabled(bool enabled);
    void setPicture(QString path);
    void pictureDelete();


    private slots:
    void openDialog();
    void contextualMenu(QPoint point);


    private:
    QString m_picturePath;
    QString m_text;
    QHBoxLayout* m_boxButtonPicture;
    QLabel* m_labelButtonPicture;
    QWidget* m_parent;
    QPushButton* m_bouton;
};

#endif // BUTTONIMAGE_H
