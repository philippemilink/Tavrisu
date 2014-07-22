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

#ifndef ALBUMWIDGET_H
#define ALBUMWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QDate>
#include <QComboBox>
#include <QCompleter>

#include "ButtonImage.h"
#include "QlibVLC.h"


namespace Ui
{
    class AlbumWidget;
}


class Track
{
    public:
    Track(QString path, int number);
    QString title();
    QString path();
    int number();
    bool isSmaller(Track const& b) const;

    private:
    QString m_title;
    QString m_path;
    int m_number;
};


class AlbumWidget : public QWidget
{
    Q_OBJECT
    
    public:
    explicit AlbumWidget(QWidget *parent = 0);
    ~AlbumWidget();
    QTableWidget* getTable();
    QComboBox* getArtistCombo();
    QString getTitle();
    int getDate();
    QString getImagePath();
    void setTitle(QString title);
    void setImagePath(QString path);
    void setDate(int date);


    public slots:
    void verifyButtonSubmit();
    void setEnabled(bool enabled);
    void hideCancelButton(bool hide);
    void unsetPicture();


    private slots:
    void on_boutonAjouter_clicked();
    void on_listeChanson_clicked();
    void on_boutonSupprimer_clicked();
    void on_boutonHaut_clicked();
    void on_boutonBas_clicked();
    void on_buttonEdit_clicked();
    void on_listeChanson_itemSelectionChanged();


    signals:
    void cancel();
    void submit();


    private:
    void moveItem(bool up);
    Ui::AlbumWidget *ui;
    ButtonImage* m_buttonImage;
    QString m_previousPath;
    QlibVLC* m_vlc;
    QCompleter* m_completerArtist;
};

#endif // ALBUMWIDGET_H
