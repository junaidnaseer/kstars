/***************************************************************************
                          dms.cpp  -  K Desktop Planetarium
                             -------------------
    begin                : Sun Feb 11 2001
    copyright            : (C) 2001 by Jason Harris
    email                : jharris@30doradus.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// needed for sincos() in math.h
#define _GNU_SOURCE

#include <stdlib.h>

#include "skypoint.h"
#include "dms.h"
#include <qregexp.h>

//DMS_SPEED
void dms::setD( const double &x ) {
	D = x;
	scDirty = true;
	rDirty = true;
}

//DMS_SPEED
void dms::setD(const int &d, const int &m, const int &s, const int &ms) {
	D = (double)abs(d) + ((double)m + ((double)s + (double)ms/1000.)/60.)/60.;
	if (d<0) {D = -1.0*D;}
	scDirty = true;
	rDirty = true;
}

void dms::setH( const double &x ) {
  setD( x*15.0 );
}

//DMS_SPEED
void dms::setH(const int &h, const int &m, const int &s, const int &ms) {
  D = 15.0*((double)abs(h) + ((double)m + ((double)s + (double)ms/1000.)/60.)/60.);
  if (h<0) {D = -1.0*D;}
	scDirty = true;
	rDirty = true;
}

//DMS_SPEED
void dms::setRadians( const double &Rad ) {
	setD( Rad/DegToRad );
	Radians = Rad;
}

const int dms::arcmin( void ) const {
	int am = int( 60.0*( fabs(D) - abs( degree() ) ) );
	if ( D<0.0 && D>-1.0 ) { //angle less than zero, but greater than -1.0
		am = -1*am; //make minute negative
	}
	return am;
}

const int dms::arcsec( void ) const {
	int as = int( 60.0*( 60.0*( fabs(D) - abs( degree() ) ) - abs( arcmin() ) ) );
	//If the angle is slightly less than 0.0, give ArcSec a neg. sgn.
	if ( degree()==0 && arcmin()==0 && D<0.0 ) {
		as = -1*as;
	}
	return as;
}

const int dms::marcsec( void ) const {
	int as = int( 1000.0*(60.0*( 60.0*( fabs(D) - abs( degree() ) ) - abs( arcmin() ) ) - abs( arcsec() ) ) );
	//If the angle is slightly less than 0.0, give ArcSec a neg. sgn.
	if ( degree()==0 && arcmin()==0 && arcsec()== 0 && D<0.0 ) {
		as = -1*as;
	}
	return as;
}

const int dms::minute( void ) const {
	int hm = int( 60.0*( fabs( Hours() ) - abs( hour() ) ) );
	if ( Hours()<0.0 && Hours()>-1.0 ) { //angle less than zero, but greater than -1.0
		hm = -1*hm; //make minute negative
	}
	return hm;
}

const int dms::second( void ) const {
	int hs = int( 60.0*( 60.0*( fabs( Hours() ) - abs( hour() ) ) - abs( minute() ) ) );
	if ( hour()==0 && minute()==0 && Hours()<0.0 ) {
		hs = -1*hs;
	}
	return hs;
}

const int dms::msecond( void ) const {
	int hs = int( 1000.0*(60.0*( 60.0*( fabs( Hours() ) - abs( hour() ) ) - abs( minute() ) ) - abs( second() ) ) );
	if ( hour()==0 && minute()==0 && second()==0 && Hours()<0.0 ) {
		hs = -1*hs;
	}
	return hs;
}

//SPEED_DMS
void dms::SinCos( double &sina, double &cosa ) const {
	/**We have two versions of this function.  One is ANSI standard, but 
		*slower.  The other is faster, but is GNU only.
		*Using the __GLIBC_ and __GLIBC_MINOR_ constants to determine if the
		* GNU extension sincos() is defined.
		*/

	if ( scDirty ) {
                #ifdef __GLIBC__
                #if ( __GLIBC__ >= 2 && __GLIBC_MINOR__ >=1 )
		//GNU version
		sincos( radians(), &Sin, &Cos );
                #else 
		//For older GLIBC versions
	        Sin = ::sin( radians() );
		Cos = ::cos( radians() );
		#endif
		#else
		//ANSI-compliant version
		Sin = ::sin( radians() );
		Cos = ::cos( radians() );
		#endif
		scDirty = false;
	}
	sina = Sin;
	cosa = Cos;
}

//SPEED_DMS
const double& dms::radians( void ) const {
	if ( rDirty ) {
		Radians = D*DegToRad;
		rDirty = false;
	}
	
	return Radians;
}

const dms dms::reduce( void ) const {
	double a = D;
	while (a<0.0) {a += 360.0;}
	while (a>=360.0) {a -= 360.0;}
	return dms( a );
}

const QString dms::toDMSString(bool forceSign) const {
	QString dummy;
	char pm(' ');
	int dd = abs(degree());
	int dm = abs(arcmin());
	int ds = abs(arcsec());

	if ( Degrees() < 0.0 ) pm = '-';
	else if (forceSign && Degrees() > 0.0 ) pm = '+';

	QString format( "%c%3d%c %02d\' %02d\"" );
	if ( dd < 100 ) format = "%c%2d%c %02d\' %02d\"";
	if ( dd < 10  ) format = "%c%1d%c %02d\' %02d\"";

	return dummy.sprintf(format.local8Bit(), pm, dd, 176, dm, ds);
}

const QString dms::toHMSString() const {
	QString dummy;
	return dummy.sprintf("%02dh %02dm %02ds", hour(), minute(), second());
}

dms dms::fromString(QString & st, bool deg) {
	int d = 0, m = 0;
	double s = 0.0;
	dms dmsAng;
	bool valueFound = false, badEntry = false , checkValue = false;

	QString entry = st.stripWhiteSpace();

	//Try simplest cases: integer or double representation

	d = entry.toInt( &checkValue );
	if ( checkValue ) {
		if (deg) dmsAng.setD( d, 0, 0 );
		else dmsAng.setH( d, 0, 0 );
		valueFound = true;
		return dmsAng;
	} else {
		double x = entry.toDouble( &checkValue );
		if ( checkValue ) {
			if ( deg ) dmsAng.setD( x );
			else dmsAng.setH( x );
			valueFound = true;
			return dmsAng;
		}
	}

	//no success yet...try assuming multiple fields

	if ( !valueFound ) {
		QStringList fields;

		//check for colon-delimiters or space-delimiters
		if ( entry.contains(':') )
			fields = QStringList::split( ':', entry );
		else fields = QStringList::split( " ", entry );

		// If two fields we will add a third one, and then parse with
		// the 3-field code block. If field[1] is a double, convert
		// it to integer arcmin, and convert the remainder to arcsec

		if ( fields.count() == 2 ) {
			double mx = fields[1].toDouble( &checkValue );
			if ( checkValue ) {
				fields[1] = QString("%1").arg( int(mx) );
				fields.append( QString("%1").arg( int( 60.0*(mx - int(mx)) ) ) );
			} else {
				fields.append( QString( "0" ) );
			}
		}

		// Three fields space-delimited ( h/d m s );
		// ignore all after 3rd field

		if ( fields.count() >= 3 ) {
			fields[0].replace( QRegExp("h"), "" );
			fields[0].replace( QRegExp("d"), "" );
			fields[1].replace( QRegExp("m"), "" );
			fields[2].replace( QRegExp("s"), "" );

			//See if first two fields parse as integers.
			//
			d = fields[0].toInt( &checkValue );
			if ( !checkValue ) badEntry = true;
			m = fields[1].toInt( &checkValue );
			if ( !checkValue ) badEntry = true;
			s = fields[2].toDouble( &checkValue );
			if ( !checkValue ) badEntry = true;
		}

		if ( !badEntry ) {
			valueFound = true;
			double D = (double)abs(d) + (double)m/60.
					+ (double)s/3600.;
			if ( d <0 ) {D = -1.0*D;}

			if (deg) {
				return	dms( D );
			} else {
				dms h;
				h.setH (D);
				return	h;
			}
		}
	}

//	 if ( !valueFound )
//		KMessageBox::sorry( 0, errMsg.arg( "Angle" ), i18n( "Could Not Set Value" ) );


	return dmsAng;

}

// M_PI is defined in math.h
const double dms::PI = M_PI;
const double dms::DegToRad = PI/180.0;
