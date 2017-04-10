/****************************************************************************

  PHelp.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010-2016 by Andreas Suter                              *
 *   andreas.suter@psi.ch                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QtWidgets>
#include <QWebEngineView>
#include <QNetworkProxyFactory>

#include <QtDebug>

#include "PHelp.h"

//---------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param url help url
 */
PHelp::PHelp(const QString &url, const bool isDarkTheme) :
  fDarkTheme(isDarkTheme)
{
  fProgress = 0;

  QString iconName("");
  if (fDarkTheme)
    iconName = QString(":/icons/musrfit-help-dark.svg");
  else
    iconName = QString(":/icons/musrfit-help-plain.svg");
  setWindowIcon( QIcon( QPixmap(iconName) ) );

  QNetworkProxyFactory::setUseSystemConfiguration(true);

  fView = new QWebEngineView(this);
  fView->load(QUrl(url));
  connect(fView, SIGNAL(loadFinished(bool)), SLOT(adjustLocation()));
  connect(fView, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
  connect(fView, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
  connect(fView, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

  fLocationEdit = new QLineEdit(this);
  fLocationEdit->setSizePolicy(QSizePolicy::Expanding, fLocationEdit->sizePolicy().verticalPolicy());
  connect(fLocationEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));

  QToolBar *toolBar = addToolBar(tr("Navigation"));
  toolBar->addAction(fView->pageAction(QWebEnginePage::Back));
  toolBar->addAction(fView->pageAction(QWebEnginePage::Forward));
  toolBar->addAction(fView->pageAction(QWebEnginePage::Reload));
  toolBar->addAction(fView->pageAction(QWebEnginePage::Stop));
  toolBar->addWidget(fLocationEdit);

  QMenu *exitMenu = menuBar()->addMenu(tr("&File"));
  exitMenu->addAction("&Exit", this, SLOT(done()), QKeySequence(tr("Ctrl+Q")));

  setCentralWidget(fView);
  setUnifiedTitleAndToolBarOnMac(true);
}

//---------------------------------------------------------------------------
/**
 * <p>Destructor
 */
PHelp::~PHelp()
{
  if (fView) {
    delete fView;
    fView = 0;
  }

  if (fLocationEdit) {
    delete fLocationEdit;
    fLocationEdit = 0;
  }
}

//---------------------------------------------------------------------------
/**
 * <p>Called on exit (SLOT). Closes the web-browser help GUI.
 */
void PHelp::done()
{
  close();
}

//---------------------------------------------------------------------------
/**
 * <p>Called after an url is loaded (SLOT). Adjusts the url address.
 */
void PHelp::adjustLocation()
{
  fLocationEdit->setText(fView->url().toString());
}

//---------------------------------------------------------------------------
/**
 * <p>Called when the url address in the line edit changed (SLOT). Will execute
 * the load of the new url.
 */
void PHelp::changeLocation()
{
  QUrl url = QUrl(fLocationEdit->text());
  fView->load(url);
  fView->setFocus();
}

//---------------------------------------------------------------------------
/**
 * <p>Called when the title changed (SLOT). Will change the window title according
 * to the state of the web-browser.
 */
void PHelp::adjustTitle()
{
  if (fProgress <= 0 || fProgress >= 100)
    setWindowTitle(fView->title());
  else
    setWindowTitle(QString("%1 (%2%)").arg(fView->title()).arg(fProgress));
}

//---------------------------------------------------------------------------
/**
 * <p>Adjusts the progess value while loading an url. Also calls adjustTitle().
 */
void PHelp::setProgress(int p)
{
  fProgress = p;
  adjustTitle();
}

//---------------------------------------------------------------------------
/**
 * <p>Ajusts the title when an url has been loaded.
 */
void PHelp::finishLoading(bool)
{
  fProgress = 100;
  adjustTitle();
}

//---------------------------------------------------------------------------
// end
//---------------------------------------------------------------------------
