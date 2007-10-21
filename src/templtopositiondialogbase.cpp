/***************************************************************************
       templtopositiondialogbase.cpp  - base dialog template to doc
                             -------------------
    begin                : Mar 2007
    copyright            : (C) 2007 Klaas Freitag
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

#include <qcombobox.h>

#include <kdialogbase.h>
#include <kdebug.h>
#include <klocale.h>

#include "templtopositiondialogbase.h"
#include "docposition.h"

TemplToPositionDialogBase::TemplToPositionDialogBase( QWidget *w )
  : KDialogBase( w, "TEMPL_DIALOG", true, i18n( "Create Position from Template" ),
                 Ok | Cancel )
{
  enableButtonSeparator( true );
}

TemplToPositionDialogBase::~TemplToPositionDialogBase()
{

}

void TemplToPositionDialogBase::setPositionList( DocPositionList list, int intendedPos )
{
  DocPositionBase *dpb;
  if ( ! getPositionCombo() ) {
    kdError() << "Can not get a ptr to the position combo" << endl;
    return;
  }
  QStringList strList;
  strList << i18n( "the Header of the Document as first position" );

  for ( dpb = list.first(); dpb; dpb = list.next() ) {
    DocPosition *dp = static_cast<DocPosition*>( dpb );
    QString h = QString( "%1. %2" ).arg( list.posNumber( dp ) ).arg( dp->text() );
    if ( h.length() > 50 ) {
      h = h.left( 50 );
      h += i18n( "..." );
    }
    strList.append( h );
  }

  getPositionCombo()->insertStringList( strList );
  if ( intendedPos > 0 ) --intendedPos;
  getPositionCombo()->setCurrentItem( intendedPos );
}

int TemplToPositionDialogBase::insertAfterPosition()
{
  int itemPos = getPositionCombo()->currentItem();
  kdDebug() << "Current item selected: " << itemPos << endl;

  return itemPos;
}

#include "templtopositiondialogbase.moc"
