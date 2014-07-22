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

#ifndef NEWARTIST_H
#define NEWARTIST_H

#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QLabel>
#include <QHBoxLayout>

#include "QSqliteCom.h"
#include "ButtonImage.h"

namespace Ui
{
    class NewArtist;
}

class NewArtist : public QDialog
{
    Q_OBJECT
    
    public:
    explicit NewArtist(QSqliteCom *base, int lastArtistId, QWidget *parent = 0, QString name = 0);
    explicit NewArtist(int id, QSqliteCom *base, QWidget *parent = 0); // To modify an artist
    ~NewArtist();
    static bool deleteArtist(QString name, int id, QSqliteCom* base);


    private slots:
    void on_buttonCancel_clicked();
    void buttonSubmit_clicked_save();
    void on_lineEditNom_textChanged(QString text);
    void buttonSubmit_clicked_update();

    signals:
    void idChanged(QString, int);
    void finish();
    

    private:
    Ui::NewArtist *ui;
    int m_lastArtistId;
    QString m_picturePath;
    QHBoxLayout* m_boxButtonPicture;
    QLabel* m_labelButtonPicture;
    QSqliteCom* m_base;
    ButtonImage* m_buttonImage;
    QString m_previousName; // Used to edit an artist
    int m_previousId;
};

#endif // NEWARTIST_H
