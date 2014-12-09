/***************************************************************************
 *   Copyright (C) 2006 by Zi Ree   *
 *   Zi Ree @ SecondLife   *
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

#ifndef ROTATION_H
#define ROTATION_H

#include <QString>

/**
@author Zi Ree
*/
/*
class Rotation
{
  public:
    Rotation();
    Rotation(double x,double y,double z);
    ~Rotation();

    static Rotation difference(const Rotation& rot1,const Rotation& rot2);

    double x;
    double y;
    double z;
};
*/
class Rotation
{
  public:
    Rotation();
    Rotation(double w,double x,double y,double z);
    ~Rotation();

    static Rotation difference(const Rotation& rot1,const Rotation& rot2);

    double w;
    double x;
    double y;
	double z;
};

class RotationLimits
{
  public:
    RotationLimits(const QString& jointName,double xMin,double xMax,
                                            double yMin,double yMax,
                                            double zMin,double zMax);
    ~RotationLimits();

    QString jointName;
    double xMin;
    double yMin;
    double zMin;
    double xMax;
    double yMax;
    double zMax;
};

class Position
{
  public:
    Position();
    Position(double x,double y,double z);
    ~Position();

    static Position difference(const Position& rot1,const Position& rot2);

    double x;
    double y;
    double z;
};

#endif
