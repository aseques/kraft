/***************************************************************************
  kraftdocfooteredit.h  - inherited class from designer generated class
                             -------------------
    begin                : Sept. 2006
    copyright            : (C) 2006 by Klaas Freitag
    email                : freitag@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef KRAFTDOCFOOTEREDIT_H
#define KRAFTDOCFOOTEREDIT_H

#include "docfooter.h"



class KraftDocFooterEdit : public DocFooterEdit
{
public:
  KraftDocFooterEdit( QWidget* );
public slots:
  void slotModified();
};

#endif

