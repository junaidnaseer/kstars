/***************************************************************************
                          astrocalc.cpp  -  description
                             -------------------
    begin                : wed dec 19 16:20:11 CET 2002
    copyright            : (C) 2001-2002 by Pablo de Vicente
    email                : vicente@oan.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "astrocalc.h"
#include "astrocalc.moc"
#include "dms.h"
#include "dmsbox.h"
#include "modcalcjd.h"
#include "modcalcgeodcoord.h"
#include "modcalcgalcoord.h"
#include "modcalcsidtime.h"
#include "modcalcprec.h"
#include "modcalcapcoord.h"
#include "modcalcdaylength.h"
#include "modcalcazel.h"
#include "modcalcequinox.h"
#include "modcalcplanets.h"
#include "modcalceclipticcoords.h"

#include <klocale.h>
#include <qlistview.h>
#include <qtextview.h>

//#include <kapplication.h> ...already defined in astrocalc.h

AstroCalc::AstroCalc( QWidget* parent ) :
	KDialogBase( parent, "starscalculator", true,
	i18n("Calculator"), Close ) {

	split = new QSplitter ( this );
	setMainWidget(split);

	navigationPanel = new QListView (split,"NavigationPanel");
	splashScreen = new QTextView (i18n("<H2>KStars Astrocalculator</H2>"),"",split);

	splashScreen->setMaximumWidth(550);
	splashScreen->setMinimumWidth(400);

	rightPanel = GenText;

	navigationPanel->addColumn("Section");
	navigationPanel->setRootIsDecorated(1);

	QPixmap jdIcon = QPixmap ("jd.png");
	QPixmap geodIcon = QPixmap ("geodetic.png");
	QPixmap solarIcon = QPixmap ("geodetic.png");
	QPixmap sunsetIcon = QPixmap ("sunset.png");
	QPixmap timeIcon = QPixmap ("sunclock.png");

	QListViewItem * timeItem = new QListViewItem(navigationPanel,i18n("Time Calculators"));
	timeItem->setPixmap(0,timeIcon);

	QListViewItem * jdItem = new QListViewItem(timeItem,i18n("Julian Day"));
	jdItem->setPixmap(0,jdIcon);

	QListViewItem * stItem = new QListViewItem(timeItem,i18n("Sidereal Time"));
	QListViewItem * dayItem = new QListViewItem(timeItem,i18n("Day Duration"));
	QListViewItem * equinoxItem = new QListViewItem(timeItem,i18n("Equinoxes & Solstices"));
//	dayItem->setPixmap(0,sunsetIcon);

	QListViewItem * coordItem = new QListViewItem(navigationPanel,i18n("Coordinate Converters"));
	QListViewItem * galItem = new QListViewItem(coordItem,i18n("Equatorial/Galactic"));
	QListViewItem * precItem = new QListViewItem(coordItem,i18n("Precession"));
	QListViewItem * appItem = new QListViewItem(coordItem,i18n("Apparent Coordinates"));
	QListViewItem * azelItem = new QListViewItem(coordItem,i18n("Horizontal Coordinates"));
	QListViewItem * eclItem = new QListViewItem(coordItem,i18n("Ecliptic Coordinates"));

	QListViewItem * geoItem = new QListViewItem(navigationPanel,i18n("Earth Coordinates"));
	geoItem->setPixmap(0,geodIcon);
	QListViewItem * cartItem = new QListViewItem(geoItem,i18n("Geodetic Coordinates"));

	QListViewItem * solarItem = new QListViewItem(navigationPanel,i18n("Solar System"));
	solarItem->setPixmap(0,solarIcon);
	QListViewItem * planetsItem = new QListViewItem(solarItem,i18n("Planets Coordinates"));

	connect(navigationPanel, SIGNAL(clicked(QListViewItem *)), this,
		SLOT(slotItemSelection(QListViewItem *)));
}

AstroCalc::~AstroCalc()
{
}

void AstroCalc::slotItemSelection(QListViewItem *item)
{
	QString election;

	if (item==0L) return;

	election = item->text(0);
	if(!(election.compare(i18n("Time Calculators"))))
		genTimeText();
	if(!(election.compare(i18n("Sidereal Time"))))
		genSidFrame();
	if(!(election.compare(i18n("Coordinate Converters"))))
		genCoordText();
	if(!(election.compare(i18n("Julian Day"))))
		genJdFrame();
	if(!(election.compare(i18n("Equatorial/Galactic"))))
		genGalFrame();
	if(!(election.compare(i18n("Precession"))))
		genPrecFrame();
	if(!(election.compare(i18n("Apparent Coordinates"))))
		genAppFrame();
	if(!(election.compare(i18n("Horizontal Coordinates"))))
		genAzelFrame();
	if(!(election.compare(i18n("Ecliptic Coordinates"))))
		genEclFrame();
	if(!(election.compare(i18n("Earth Coordinates"))))
		genGeodText();
	if(!(election.compare(i18n("Geodetic Coordinates"))))
		genGeodCoordFrame();
	if(!(election.compare(i18n("Day Duration"))))
		genDayFrame();
	if(!(election.compare(i18n("Equinoxes & Solstices"))))
		genEquinoxFrame();
	if(!(election.compare(i18n("Solar System"))))
		genSolarText();
	if(!(election.compare(i18n("Planets Coordinates"))))
		genPlanetsFrame();

		previousElection = election;

}

void AstroCalc::genTimeText(void)
{

	delRightPanel();
	splashScreen = new QTextView ("","",split);
	splashScreen->setMaximumWidth(550);
	splashScreen->setMinimumWidth(400);
	splashScreen->show();
		
	splashScreen->setText(i18n("<QT>"
														 "Section which includes algorithms for computing time ephemeris"
														 "<UL><LI>"
														 "<B>Julian Day:</B> Julian Day/Calendar conversion"
														 "</LI><LI>"
														 "<B>Sidereal Time:</B> Sidereal/Universal time conversion"
														 "</LI><LI>"
														 "<B>Day duration:</B> Sunrise, Sunset and noon time and "
														 "positions for those events"
														 "</LI></LI>"
														"<B>Equinoxes & Solstices:</B> Equinoxes, Solstices and duration of the "
														"seasons"
														 "</LI></UL>"
														 "</QT>"));
	
	rightPanel=TimeText;
														 	
}

void AstroCalc::genCoordText(void)
{
	delRightPanel();
	splashScreen = new QTextView ("","",split);
	splashScreen->setMaximumWidth(550);
	splashScreen->setMinimumWidth(400);
	splashScreen->show();
	
	splashScreen->setText(i18n("<QT>"
														 "Section with algorithms for the conversion of "
														 "different astronomical systems of coordinates"
														 "<UL><LI>"
														 "<B>Precessor:</B> Precession of coordinates between epochs"
														 "</LI><LI>"
														 "<B>Galactic:</B> Galactic/Equatorial coordinates conversion"
														 "</LI><LI>"
														 "<B>Apparent:</B> Computation of current equatorial coordinates"
														 " from a given epoch"
														 "</LI><LI>"
														 "<B>Ecliptic:</B> Ecliptic/Equatorial coordinates conversion"
														 "</LI><LI>"
														 "<B>Horizontal:</B> Computation of azimuth and elevation for a "
														 "given source, time, and location on the Earth"
														 "</LI></UL>"
														 "</QT>"));

	rightPanel=CoordText;

}

void AstroCalc::genGeodText(void)
{
	delRightPanel();
	splashScreen = new QTextView ("","",split);
	splashScreen->setMaximumWidth(550);
	splashScreen->setMinimumWidth(400);
	splashScreen->show();
	
	splashScreen->setText(i18n("<QT>"
														 "Section with algorithms for the conversion of "
														 "systems of coordinates for the Earth"
														 "<UL><LI>"
														 "<B>Geodetic Coords:</B> Geodetic/XYZ coordinate conversion"
														 "</LI></UL>"
														 "</QT>"));

	rightPanel=GeoText;
}

void AstroCalc::genSolarText(void)
{
	delRightPanel();
	splashScreen = new QTextView ("","",split);
	splashScreen->setMaximumWidth(550);
	splashScreen->setMinimumWidth(400);
	splashScreen->show();
	
	splashScreen->setText(i18n("<QT>"
														 "Section with algorithms regarding information "
														 "on solar system bodies coordinates and times"
														 "<UL><LI>"
														 "<B>Planets Coords:</B> Coordinates for the planets, moon and sun "
														 " at a given time and from a given position on Earth "
														 "</LI></UL>"
														 "</QT>"));

	rightPanel=SolarText;
}


void AstroCalc::delRightPanel(void)
{
	if (rightPanel <= 3)
		delete splashScreen;
	else if (rightPanel == JD)
		delete JDFrame;
	else if (rightPanel == SidTime)
		delete SidFrame;
	else if (rightPanel == DayLength)
		delete DayFrame;
	else if (rightPanel == Equinox)
		delete EquinoxFrame;
	else if (rightPanel == GeoCoord)
		delete GeodCoordFrame;
	else if (rightPanel == Galactic)
		delete GalFrame;
	else if (rightPanel == Ecliptic)
		delete EclFrame;
	else if (rightPanel == Precessor)
		delete PrecFrame;
	else if (rightPanel == Apparent)
		delete AppFrame;
	else if (rightPanel == Azel)
		delete AzelFrame;
	else if (rightPanel == Planets)
		delete PlanetsFrame;

}

void AstroCalc::genJdFrame(void)
{
	delRightPanel();
	JDFrame = new modCalcJD(split,"JulianDay");
	rightPanel = JD;
}

void AstroCalc::genSidFrame(void)
{
	delRightPanel();
	SidFrame = new modCalcSidTime(split,"SiderealTime");
	rightPanel = SidTime;
}

void AstroCalc::genDayFrame(void)
{
	delRightPanel();
	DayFrame = new modCalcDayLength(split,"DayDuration");
	rightPanel = DayLength;
}

void AstroCalc::genEquinoxFrame(void)
{
	delRightPanel();
	EquinoxFrame = new modCalcEquinox(split,"Equinox");
	rightPanel = Equinox;
}

void AstroCalc::genGeodCoordFrame(void)
{
	delRightPanel();
	GeodCoordFrame = new modCalcGeodCoord(split,"GeoCoord");
	rightPanel = GeoCoord;
}

void AstroCalc::genGalFrame(void)
{
	delRightPanel();
	GalFrame = new modCalcGalCoord(split,"Galactic");
	rightPanel = Galactic;
}

void AstroCalc::genEclFrame(void)
{
	delRightPanel();
	EclFrame = new modCalcEclCoords(split,"Ecliptic");
	rightPanel = Ecliptic;
}

void AstroCalc::genPrecFrame(void)
{
	delRightPanel();
	PrecFrame = new modCalcPrec(split,"Precession");
	rightPanel = Precessor;
}

void AstroCalc::genAppFrame(void)
{
	delRightPanel();
	AppFrame = new modCalcApCoord(split,"Apparent");
	rightPanel = Apparent;
}

void AstroCalc::genAzelFrame(void)
{
	delRightPanel();
	AzelFrame = new modCalcAzel(split,"Horizontal");
	rightPanel = Azel;
}

void AstroCalc::genPlanetsFrame(void)
{
	delRightPanel();
	PlanetsFrame = new modCalcPlanets(split,"Horizontal");
	rightPanel = Planets;
}

QSize AstroCalc::sizeHint() const
{
  return QSize(640,430);
}
