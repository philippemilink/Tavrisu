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

#include "InitSelectAction.h"
#include "ui_InitSelectAction.h"


InitSelectAction::InitSelectAction(QWidget *parent) : QWidget(parent), ui(new Ui::InitSelectAction)
{
    ui->setupUi(this);
}


InitSelectAction::~InitSelectAction()
{
    delete ui;
}


void InitSelectAction::on_buttonNext_clicked()
{
    if(ui->radioImport->isChecked())
        emit importXML();
    if(ui->radioSearchFolder->isChecked())
        emit searchFolder();
    if(ui->radioNothing->isChecked())
        emit nothing();
}
