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

#ifndef INITSELECTACTION_H
#define INITSELECTACTION_H

#include <QWidget>

namespace Ui
{
    class InitSelectAction;
}

class InitSelectAction : public QWidget
{
    Q_OBJECT
    
    public:
    explicit InitSelectAction(QWidget *parent = 0);
    ~InitSelectAction();


    public slots:
    void on_buttonNext_clicked();


    signals:
    void nothing();
    void importXML();
    void searchFolder();

    
    private:
    Ui::InitSelectAction *ui;
};

#endif // INITSELECTACTION_H
