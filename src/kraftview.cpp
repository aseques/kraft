 /***************************************************************************
                          kraftview.cpp  -
                             -------------------
    begin                : Mit Dez 31 19:24:05 CET 2003
    copyright            : (C) 2003 by Klaas Freitag
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

// include files for Qt
#include <qprinter.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qscrollview.h>
#include <qsizepolicy.h>
#include <qtextedit.h>
#include <qsignalmapper.h>
#include <qhbox.h>
#include <qvbox.h>

#include <kdebug.h>
#include <kdialogbase.h>
#include <kpushbutton.h>
#include <kcombobox.h>
#include <kdatewidget.h>
#include <knuminput.h>
#include <kactioncollection.h>
#include <kmessagebox.h>

#include <kabc/addressbook.h>
#include <kabc/stdaddressbook.h>
#include <kabc/addresseedialog.h>
#include <kabc/addressee.h>

// application specific includes
#include "kraftsettings.h"
#include "kraftview.h"
#include "kraftdoc.h"
#include "portal.h"
#include "docheader.h"
#include "positionviewwidget.h"
#include "docfooter.h"
#include "docposition.h"
#include "unitmanager.h"


KraftViewScroll::KraftViewScroll( QWidget *parent ):
QScrollView( parent )
{

}

void KraftViewScroll::viewportResizeEvent ( QResizeEvent *ev )
{
  int w = ev->size().width(); // visibleWidth()-1;
  resizeContents( w, contentsHeight () );
  QScrollView::viewportResizeEvent( ev );
}

void KraftViewScroll::resizeContents(  int w, int h )
{
  if( w < 400 ) {
    w = 400;
  }
  QScrollView::resizeContents( w, h );
  PositionViewWidget *wid;
  for ( wid = mWidgetList.first(); wid; wid = mWidgetList.next() ) {
    wid->resize( w, wid->height() );
  }
}

void KraftViewScroll::addChild( QWidget *child, int x, int y )
{
  mWidgetList.append( static_cast<PositionViewWidget*>(child) );
  QScrollView::addChild( child, x, y );
}

void KraftViewScroll::kraftRemoveChild( PositionViewWidget *child )
{
  removeChild( child ); // from the scrollview
  mWidgetList.removeRef( child );
}

// #########################################################

KraftView::KraftView(QWidget *parent, const char *name) :
 KDialogBase( TreeList, 0, parent, name,
              false /* modal */, i18n("Document"),
	      Ok|Cancel, Ok, true /* separator */ ),
       m_doc( 0 )
{
  mDeleteMapper = new QSignalMapper( this );
  connect( mDeleteMapper, SIGNAL( mapped(int)),
           this, SLOT( slotDeletePosition( int ) ) );

  mMoveUpMapper = new QSignalMapper( this );
  connect( mMoveUpMapper, SIGNAL( mapped(int)),
           this, SLOT( slotMovePositionUp( int  ) ) );

  mMoveDownMapper = new QSignalMapper( this );
  connect( mMoveDownMapper, SIGNAL( mapped(int)),
           this, SLOT( slotMovePositionDown( int ) ) );

  mLockPositionMapper = new QSignalMapper( this );
  connect( mLockPositionMapper, SIGNAL( mapped( int )),
           this, SLOT( slotLockPosition( int ) ) );

  mUnlockPositionMapper = new QSignalMapper( this );
  connect( mUnlockPositionMapper, SIGNAL( mapped( int )),
           this, SLOT( slotUnlockPosition( int ) ) );

  connect( this, SIGNAL( aboutToShowPage( QWidget* ) ),
           this, SLOT( slotAboutToShow( QWidget* ) ) );
}

KraftView::~KraftView()
{
}

void KraftView::setup( DocGuardedPtr doc )
{
  m_doc = doc;

  setupDocHeaderView();
  setupPositions();
  setupFooter();
  setCaption( m_doc->docIdentifier() );
}

void KraftView::setupDocHeaderView()
{
    QFrame *page = addPage( i18n("Header"), i18n("Document Header information") );
    QVBoxLayout *topLayout = new QVBoxLayout( page, 0, KDialog::spacingHint() );

    m_headerEdit = new DocHeaderEdit( page );
    topLayout->addWidget( m_headerEdit );

    m_headerEdit->m_cbType->insertItem( i18n("Offer") );
    m_headerEdit->m_cbType->insertItem( i18n("Invoice") );
    m_headerEdit->m_cbType->insertItem( i18n("Auftragsbestätigung") );

    connect( m_headerEdit->m_selectAddress, SIGNAL( clicked() ),
               this, SLOT( slotSelectAddress() ) );
    connect( m_headerEdit, SIGNAL( modified() ),
              this, SLOT( slotModifiedHeader() ) );
}

void KraftView::setupPositions()
{
    QVBox *page = addVBoxPage( i18n( "Positions" ), i18n( "Positions of the document" ) );

    QHBox *hbox = new QHBox( page );
    KPushButton *button = new KPushButton( i18n("Add"), hbox );
    connect( button, SIGNAL( clicked() ), this, SLOT( slotAddPosition() ) );

    QWidget *spaceEater = new QWidget( hbox );
    spaceEater->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );

    m_positionScroll = new KraftViewScroll( page );
}

void KraftView::redrawDocument( )
{
    KraftDoc *doc = getDocument();
    if( !doc ) {
      kdDebug() << "ERR: No document available in view, return!" << endl;
    } else {
      kdDebug() << "** Starting redraw of document " << doc << endl;
    }

    /* header: date and document type */
    QDate date = doc->date();
    m_headerEdit->m_dateEdit->setDate( date );
    m_headerEdit->m_cbType->setCurrentText( doc->docType() );

    /* header: address */
    mContactUid  = doc->addressUid();
    QString address = doc->address();

    kdDebug() << "Loaded address uid from database " << mContactUid << endl;
    if( ! mContactUid.isEmpty() ) {
      KABC::AddressBook *adrBook =  KABC::StdAddressBook::self();
      KABC::Addressee contact;
      if( adrBook ) {
        contact = adrBook->findByUid( mContactUid );
        if( contact.isEmpty() ) {
          // the Uid is set, but the address is not in this addressbook
          KMessageBox::sorry( this, i18n("The addressbook does not contain the address identified by the ID in the document.").arg( mContactUid ), i18n("Addressbook out of sync") );
          m_headerEdit->m_labName->setText( i18n("--lost--") );
        } else {
          kdDebug() << "The loaded Contact has this realname: " << contact.realName() << endl;
          slotSelectAddress( contact );
        }
      }
    }
    if( !address.isEmpty() ) {
      // custom address stored in the document.
      // kdDebug() << "custom address came in: " << address << endl;
      m_headerEdit->m_postAddressEdit->setText( address );
    }

    if( !doc->salut().isEmpty() ) {
      m_headerEdit->m_letterHead->insertItem( doc->salut() );
      m_headerEdit->m_letterHead->setCurrentText( doc->salut() );
    }
    /* pre- and post text */
    m_headerEdit->m_teEntry->setText( doc->preText() );
    m_footerEdit->m_teSummary->setText( doc->postText() );

    redrawDocPositions( );
}

void KraftView::redrawDocPositions( )
{
    KraftDoc *doc = getDocument();
    kdDebug() << "** Starting to redraw the positions" << endl;

    if( ! doc ) {
      kdError() << "Empty document pointer" << endl;
      return;
    }

    DocPositionList list = doc->positions();
    kdDebug() << "starting to redraw " << list.count() << " positions!" << endl;

    DocPositionBase *dp = 0;

    for( dp = list.first(); dp; dp = list.next() ) {
        PositionViewWidget *w = mPositionWidgetList.widgetFromPosition( dp );
        if( !w ) {
            w = new PositionViewWidget( );
            int cw = m_positionScroll->contentsWidth();
            if ( cw < 400 ) cw = 400;

            w->resize( cw, w->height() );
            m_positionScroll->resizeContents( cw,
                                              list.count() * w->height()+1 );
            mDeleteMapper->setMapping( w, list.at() );
            mMoveUpMapper->setMapping( w, list.at() );
            mMoveDownMapper->setMapping( w, list.at() );
            mLockPositionMapper->setMapping( w, list.at() );
            mUnlockPositionMapper->setMapping( w, list.at() );

            connect( w, SIGNAL( deletePosition() ), mDeleteMapper, SLOT( map() ) );
            connect( w, SIGNAL( moveUp() ),         mMoveUpMapper, SLOT( map() ) );
            connect( w, SIGNAL( moveDown() ),       mMoveDownMapper, SLOT( map() ) );
            connect( w, SIGNAL( lockPosition() ),   mLockPositionMapper, SLOT( map() ) );
            connect( w, SIGNAL( unlockPosition() ), mUnlockPositionMapper, SLOT( map() ) );

            connect( w, SIGNAL( positionModified() ), this,
                          SLOT( slotModifiedPositions() ) );
            w->m_cbUnit->insertStringList( UnitManager::allUnits() );

            m_positionScroll->addChild( w, 0, 0 );
            // kdDebug() << "Adding a widget for position number " << cnt << endl;

            kdDebug() << "Creating an entry in the position map for " << dp->dbId().toInt() << endl;
            if( dp->dbId().toInt() < 0 ) {
              kdDebug() << "setting state to NEW" << endl;
              w->slotSetState( PositionViewWidget::New );
            }
            mPositionWidgetList.append( w );
            w->show();
        }
        kdDebug() << "now position " << dp->position() << endl;
        int y = list.at() * w->height();
        m_positionScroll->moveChild( w, 0, y );
        w->setPosition( dp );
        w->setOrdNumber( 1 + list.at() );
    }

    // now go through the positionWidgetMap and check if it contains elements
    // that are not any longer in the list of positions
    PositionViewWidget *w = 0;
    for( w = mPositionWidgetList.first(); w; w = mPositionWidgetList.next() ) {
      if( list.containsRef( w->position() ) == 0 ) {
        kdDebug() << "Removing this one: " << w << endl;
        m_positionScroll->kraftRemoveChild( w );
        mPositionWidgetList.remove();
        break;
      }
    }

    // repaint everything
    m_positionScroll->updateContents();
}

void KraftView::setupFooter()
{
    QFrame *page = addPage( i18n("Footer"), i18n("Document Footer Information") );
    QVBoxLayout *topLayout = new QVBoxLayout( page, 0, KDialog::spacingHint() );

    m_footerEdit = new DocFooterEdit( page );
    topLayout->addWidget( m_footerEdit );
    connect( m_footerEdit, SIGNAL( modified() ),
               this, SLOT( slotModifiedFooter() ) );
}

void KraftView::slotAboutToShow( QWidget* w )
{
  kdDebug() << "showing page " << w << endl;
}

/* This is the flow in the move up method:
         0    Bla1          0 Bla1                         0 Bla1
         1    Bla2 <- w2    1 Bla2   -> insert at pos-1 => 1 Bla3
 pos ->  2    Bla3 <- w1    2 Bla4                         2 Bla2
         3    Bla4                                         3 Bla4
 */
void KraftView::slotMovePositionUp( int pos )
{
  PositionViewWidget *w1 = 0;
  PositionViewWidget *w2 = 0;

  if( pos < 1 || (unsigned) pos > mPositionWidgetList.count() ) {
    kdDebug() << "ERR: position out of range: " << pos << endl;
    return;
  }
  w2 = mPositionWidgetList.at( pos-1 );
  w1 = mPositionWidgetList.take( pos );
  mPositionWidgetList.insert( pos-1, w1 );
  kdDebug() << "Found at pos " << pos << " the widgets " << w1 << " and " << w2 << endl;

  PositionViewWidget *vw = 0;
  for( vw = mPositionWidgetList.first(); vw; vw = mPositionWidgetList.next() ) {
    DocPositionBase* pb = vw->position();
    DocPosition *pos = static_cast<DocPosition*>(pb);
    if( ! pos ) {
      kdDebug() << "There is no position!" << endl;
    } else {
      kdDebug() << "Pos " << vw->ordNumber() << ": " << pos->text() << endl;
    }
  }

  if( w1 && w2 ) {
    w1->setOrdNumber( pos );  // note: ordnumbers start with 1, thus add one
    w2->setOrdNumber( pos+1 );
    mMoveUpMapper->removeMappings( w1 );
    mMoveUpMapper->setMapping( w1, pos-1 );
    mMoveUpMapper->removeMappings( w2 );
    mMoveUpMapper->setMapping( w2, pos );
    // int tmpX = m_positionScroll->childX( w1 );
    int tmpY = m_positionScroll->childY( w1 );

    m_positionScroll->moveChild( w1, 0, m_positionScroll->childY( w2 ) );
    m_positionScroll->moveChild( w2, 0, tmpY );
  } else {
    kdDebug() << "ERR: Did not find the two corresponding widgets!" << endl;
  }
}
/*
          0    Bla1          0 Bla1                         0 Bla1
  pos ->  1    Bla2 <- w1    1 Bla3                         1 Bla3
          2    Bla3 <- w2    2 Bla4   -> insert at pos+1 => 2 Bla2
          3    Bla4                                         3 Bla4
*/
void KraftView::slotMovePositionDown( int pos )
{
  PositionViewWidget *w1 = 0;
  PositionViewWidget *w2 = 0;

  if( pos < 0 || (unsigned) pos > mPositionWidgetList.count() -1 ) {
    kdDebug() << "ERR: position out of range: " << pos << endl;
  }

  w2 = mPositionWidgetList.at( pos+1 );
  w1 = mPositionWidgetList.take( pos );
  mPositionWidgetList.insert( pos+1, w1 );

  if( w1 && w2 ) {
    w1->setOrdNumber( pos+2  );
    w2->setOrdNumber( pos+1 );

    mMoveUpMapper->removeMappings( w1 );
    mMoveDownMapper->setMapping( w1, pos+1 );
    mMoveUpMapper->removeMappings( w2 );
    mMoveDownMapper->setMapping( w2, pos );

    int tmpY = m_positionScroll->childY( w1 );
    m_positionScroll->moveChild( w1, 0, m_positionScroll->childY( w2 ) );
    m_positionScroll->moveChild( w2, 0, tmpY );
  } else {
    kdDebug() << "ERR: Did not find the two corresponding widgets!" << endl;
  }
}

void KraftView::slotDeletePosition( int pos )
{
  PositionViewWidget *w1 = mPositionWidgetList.at( pos );
  if( w1 ) {
    w1->slotSetState( PositionViewWidget::Deleted );
  }
}

void KraftView::slotLockPosition( int pos )
{
  kdDebug() << "Locking Position " << pos << endl;

  PositionViewWidget *w1 = mPositionWidgetList.at( pos );
  if( w1 ) {
    w1->slotSetState( PositionViewWidget::Locked );
  }
}

void KraftView::slotUnlockPosition( int pos )
{
  kdDebug() << "Unlocking Position " << pos << endl;

  PositionViewWidget *w1 = mPositionWidgetList.at( pos );
  if( w1 ) {
    w1->slotSetState( PositionViewWidget::Active );
  }
}

void KraftView::slotSelectAddress( KABC::Addressee contact )
{
    if( contact.isEmpty() ) {
    	kdDebug() << "Select an address from KAdressbook" << endl;
    	contact = KABC::AddresseeDialog::getAddressee( this );
        kdDebug() << "Selected address UID is " << contact.uid() << endl;
        mContactUid = contact.uid();
    }

    if( ! contact.isEmpty() ) {
      m_headerEdit->m_labName->setText( contact.realName() );
      KABC::Address address = contact.address(64);

      QString adrStr = address.street() + "\n" + address.postalCode();
      adrStr = address.formattedAddress( contact.realName() );
      kdDebug() << "formatted address string: " << adrStr << endl;
      m_headerEdit->m_postAddressEdit->setText( adrStr );
      m_headerEdit->m_letterHead->clear();
      QStringList li = generateLetterHead( contact );

      m_headerEdit->m_letterHead->insertStringList( li );
    }
}

void KraftView::slotAddPosition()
{
    const DocPosition dp;
    int newPos = m_doc->slotAppendPosition( dp );
    slotFocusPosition( newPos );
}

void KraftView::slotModifiedPositions()
{
    const QString modStr = i18n(" (modified)");
#if 0
    QString t = m_positionLabel->text();
    if( ! t.endsWith( modStr ) ) {
      m_positionLabel->setText( t + modStr );
    }
#endif
    KraftDoc *doc = getDocument();
    if( !doc ) {
      kdDebug() << "ERR: No document available in view, return!" << endl;
    }

    doc->setModified( true );
}

void KraftView::slotModifiedHeader()
{
    kdDebug() << "Modified the header!" << endl;

    KraftDoc *doc = getDocument();
    if( !doc ) {
      kdDebug() << "ERR: No document available in view, return!" << endl;
    }

    doc->setModified( true );
}

void KraftView::slotModifiedFooter()
{
    kdDebug() << "Modified the footer!" << endl;

    KraftDoc *doc = getDocument();
    if( !doc ) {
      kdDebug() << "ERR: No document available in view, return!" << endl;
    }

    doc->setModified( true );
}

QStringList KraftView::generateLetterHead( KABC::Addressee adr )
{
    QStringList s;
    const QString famName = adr.familyName();
    const QString givenName = adr.givenName();

    s << i18n("Dear Mr. %1,").arg( famName );
    s << i18n("Dear Mrs. %1,").arg( famName );
    s << i18n("Dear Mrs. %1, Dear Mr. %2,").arg( famName ).arg( famName );
    s << i18n("Dear %1,").arg( givenName );
    return s;
}

KraftDoc *KraftView::getDocument() const
{
  return m_doc;
}

void KraftView::done( int r )
{
  kdDebug() << "View closed with ret value " << r << endl;
  KraftDoc *doc = getDocument();
  if( doc )
    doc->removeView( this );
  KDialogBase::done( r );
}

void KraftView::slotOk()
{
    kdDebug() << "Accept Slot hit!" << endl;

    KraftDoc *doc = getDocument();

    if( !doc ) {
      kdDebug() << "ERR: No document available in view, return!" << endl;
      return;
    }
    // transfer all values to the document
    doc->setDate( m_headerEdit->m_dateEdit->date() );
    doc->setAddressUid( mContactUid );
    doc->setAddress(  m_headerEdit->m_postAddressEdit->text() );
    doc->setDocType(  m_headerEdit->m_cbType->currentText() );
    doc->setPreText(  m_headerEdit->m_teEntry->text() );
    doc->setSalut(    m_headerEdit->m_letterHead->currentText() );
    doc->setPostText( m_footerEdit->m_teSummary->text() );
    doc->setGoodbye(  m_footerEdit->m_cbGreeting->currentText() );

    savePositions();

    doc->saveDocument( );

    KraftSettings::setDocViewSize( size() );
    KraftSettings::writeConfig();

    KDialogBase::slotOk(  );
}

// saves changes in the GUI to the underlying document pos
void KraftView::savePositions()
{
  PositionViewWidget *widget;
  for( widget = mPositionWidgetList.first(); widget; widget = mPositionWidgetList.next() ) {
    DocPositionBase *dpb = widget->position();
    DocPosition *pos = static_cast<DocPosition*>(dpb);

    QString h = QString::number( widget->ordNumber() );

    if( h != pos->position() ) {
      pos->setPosition( h );
    }

    if( widget->deleted() ) {
      pos->setToDelete( true );
      continue;
    }

    if( widget->modified() ) {
      kdDebug() << "Position " << pos->position() << " was modified" << endl;
      h = widget->m_teFloskel->text();
      if( h != pos->text() ) {
        pos->setText( h );
      }

      h = widget->m_cbUnit->currentText();
      int eId = UnitManager::getUnitIDSingular( h );
      Einheit e = UnitManager::getUnit( eId );
      if( e.id() != pos->unit().id() ) {
        pos->setUnit( e );
      }

      double v = widget->m_sbAmount->value();
      if( v != pos->amount() ) {
        pos->setAmount( v );
      }

      v = widget->m_sbUnitPrice->value();
      if( v != pos->unitPrice().toDouble() ) {
        pos->setUnitPrice( Geld( v ) );
      }
    } else {
      kdDebug() << "Position " << pos->position() << " was NOT modified" << endl;
    }
  }
}

void KraftView::slotFocusPosition( int pos )
{
  DocPositionList list = getDocument()->positions();

  PositionViewWidget *dpb = mPositionWidgetList.at( pos-1 );

  if( dpb && pos > 0) {
    int w = dpb->height();
    m_positionScroll->ensureVisible( 0, (pos-1)*w, 0, w );
  } else {
    m_positionScroll->ensureVisible( 0, 0 );
  }
  if( dpb ) {
    if( dpb->m_teFloskel->text().isEmpty() ) {
      dpb->m_teFloskel->setFocus();
    } else {
      dpb->m_sbAmount->setFocus();
    }
  }
}

void KraftView::slotCancel()
{
  // We need to reread the document
  KraftDoc *doc = getDocument();
  if( doc && doc->isModified() ) {
    kdDebug() << "Document refetch from database" << endl;
    doc->reloadDocument();
  }

  KDialogBase::slotCancel();
}

void KraftView::print(QPrinter * /* pPrinter */ )
{
    // create a archive document and start printing
    // ArchivedDoc *archDoc = doc->archive();

}

#include "kraftview.moc"
