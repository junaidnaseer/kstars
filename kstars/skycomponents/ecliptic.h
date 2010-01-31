/***************************************************************************
                          ecliptic.h  -  K Desktop Planetarium
                             -------------------
    begin                : 2007-08-09
    copyright            : (C) 2007 by James B. Bowlin
    email                : bowlin@mindspring.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ECLIPTIC_H
#define ECLIPTIC_H

#include "linelistindex.h"
#include "linelistlabel.h"

/**
 * @class Ecliptic
 * Represents the ecliptic on the sky map.
 *	
 * @author James B. Bowlin
 * @version 0.1
 */
class Ecliptic : public LineListIndex
{
public:

    /* @short Constructor
     * @p parent pointer to the parent SkyComponent object
     * name is the name of the subclass
     */
    Ecliptic( SkyComponent *parent );

    virtual void draw( QPainter& psky );
    virtual bool selected();

    virtual void init();
protected:
    virtual void updateLabelCandidates( const QPointF& o, LineList* lineList, int i ) {
        m_label.updateLabelCandidates( o.x(), o.y(), lineList, i );
    }

private:
    void drawLabel( QPainter& psky );
    LineListLabel m_label;
};

#endif
