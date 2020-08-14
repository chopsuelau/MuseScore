//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2010-2019 Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#include "libmscore/fret.h"
#include "libmscore/measure.h"
#include "libmscore/system.h"
#include "libmscore/score.h"
//#include "libmscore/stringdata.h"
#include "fretcanvas.h"
#include "preferences.h"
#include "libmscore/chord.h"
#include "libmscore/note.h"
#include "libmscore/segment.h"
#include "libmscore/undo.h"
#include "musescore.h"

namespace Ms {

//---------------------------------------------------------
//   FretCanvas
//---------------------------------------------------------

FretCanvas::FretCanvas(QWidget* parent)
   : QFrame(parent)
      {
      setAcceptDrops(true);
//      setFrameStyle(QFrame::Raised | QFrame::Panel);
      cstring = -2;
      cfret   = -2;
      _currentDtype = FretDotType::NORMAL;
      }

//---------------------------------------------------------
//   paintEvent
//---------------------------------------------------------

void FretCanvas::paintEvent(QPaintEvent* ev)
      {
      double mag        = 2.0;//1.5
      double _spatium   = 20.0 * mag;
      double lw1        = _spatium * 0.08;//0.08
      int fretOffset    = diagram->fretOffset();
      double lw2        = (fretOffset || !diagram->showNut()) ? lw1 : _spatium * 0.2;
      double stringDist = _spatium * .7;
      double fretDist   = _spatium * .8;
      int _strings      = diagram->strings();
      int _frets        = diagram->frets();
      double dotd       = stringDist * .6 + lw1;

      double w  = (_strings - 1) * stringDist;
      double xo = (width() - w) * .5;//.5
      double h  = (_frets * fretDist) + fretDist * .5;
      double yo = (height() - h) * .9;//.5

      QFont font("FreeSans");
      int size = lrint(18.0 * mag);
      font.setPixelSize(size);

      QPainter p(this);
      p.setRenderHint(QPainter::Antialiasing, preferences.getBool(PREF_UI_CANVAS_MISC_ANTIALIASEDDRAWING));
      p.setRenderHint(QPainter::TextAntialiasing, true);
      p.translate(xo, yo);

      QPen pen(p.pen());
      pen.setWidthF(lw2);
      pen.setCapStyle(Qt::FlatCap);
      p.setPen(pen);
      p.setBrush(pen.color());
      double x2 = (_strings-1) * stringDist;
      p.drawLine(QLineF(-lw1 * .5, 0.0, x2 + lw1 * .5, 0.0));

      pen.setWidthF(lw1);
      p.setPen(pen);
      double y2 = (_frets+1) * fretDist - fretDist*.5;

      QPen symPen(pen);
      symPen.setCapStyle(Qt::RoundCap);
      symPen.setWidthF(lw1 * 1.2);

      // Draw strings and frets
      for (int i = 0; i < _strings; ++i) {
            double x = stringDist * i;
            p.drawLine(QLineF(x, fretOffset ? -_spatium*.2 : 0.0, x, y2));
            }
      for (int i = 1; i <= _frets; ++i) {
            double y = fretDist * i;
            p.drawLine(QLineF(0.0, y, x2, y));
            }

      // Draw dots and markers
      for (int i = 0; i < _strings; ++i) {
            for (auto const& d : diagram->dot(i)) {
                  if (d.exists()) {
                        p.setPen(symPen);
                        int fret = d.fret;
                        double x = stringDist * i - dotd * .5;
                        double y = fretDist * (fret - 1) + fretDist * .5 - dotd * .5;

                        paintDotSymbol(p, symPen, x, y, dotd, d.dtype);
                        }
                  }
            p.setPen(pen);

            FretItem::Marker mark = diagram->marker(i);
            if (mark.exists()) {
                  p.setFont(font);
                  double x = stringDist * i;
                  double y = -fretDist * .1;
                  p.drawText(QRectF(x, y, 0.0, 0.0),
                     Qt::AlignHCenter | Qt::AlignBottom | Qt::TextDontClip, FretItem::markerToChar(mark.mtype));
                  }
            }

      // Draw barres
      p.setPen(pen);
      for (auto const& i : diagram->barres()) {
            int fret        = i.first;
            int startString = i.second.startString;
            int endString   = i.second.endString;

            qreal x1   = stringDist * startString;
            qreal newX2 = endString == -1 ? x2 : stringDist * endString;

            qreal y    = fretDist * (fret - 1) + fretDist * .5;
            pen.setWidthF(dotd * diagram->score()->styleD(Sid::barreLineWidth));      // don't use style barreLineWidth - why not?
            pen.setCapStyle(Qt::RoundCap);
            p.setPen(pen);
            p.drawLine(QLineF(x1, y, newX2, y));
            }

      // Draw 'hover' dot
      if ((cfret > 0) && (cfret <= _frets) && (cstring >= 0) && (cstring < _strings)) {
            FretItem::Dot cd = diagram->dot(cstring, cfret)[0];
            std::vector<FretItem::Dot> otherDots = diagram->dot(cstring);
            FretDotType dtype;
            symPen.setColor(Qt::lightGray);

            if (cd.exists()) {
                  dtype = cd.dtype;
                  symPen.setColor(Qt::red);
                  }
            else {
                  dtype = _automaticDotType ? FretDotType::NORMAL : _currentDtype;
                  }
            p.setPen(symPen);

            double x = stringDist * cstring - dotd * .5;
            double y = fretDist * (cfret-1) + fretDist * .5 - dotd * .5;
            p.setBrush(Qt::lightGray);
            paintDotSymbol(p, symPen, x, y, dotd, dtype);
            }

      if (fretOffset > 0) {
            qreal fretNumMag = 2.0; // TODO: get the value from Sid::fretNumMag
            QFont scaledFont(font);
            scaledFont.setPixelSize(font.pixelSize() * fretNumMag);
            p.setFont(scaledFont);
            p.setPen(pen);
            // Todo: make dependent from Sid::fretNumPos
            p.drawText(QRectF(-stringDist * .4, 0.0, 0.0, fretDist),
               Qt::AlignVCenter|Qt::AlignRight|Qt::TextDontClip,
               QString("%1").arg(fretOffset+1));
            p.setFont(font);
            }

      QFrame::paintEvent(ev);
      }


void FretCanvas::paintDotShape(QPainter& p, QPen& pen, qreal x, qreal y, qreal dotd, FretDotType dtype,qreal x1, qreal y1,qreal x2, qreal y2)
{
    switch (dtype) {
        case FretDotType::NORMAL:
            p.setBrush(pen.color());
            p.setPen(Qt::NoPen);
            p.drawEllipse(QRectF( x+(x1 * dotd), y+(y1 * dotd), dotd*x2, dotd*y2));
            break;
        case FretDotType::CROSS:
            p.setBrush(Qt::NoBrush);
            p.setPen(pen);
            p.drawLine(QLineF(x-(x1 * dotd), y-(y1 * dotd), x + dotd+(x2 * dotd), y + dotd+(y2 * dotd) ));//TL-BR
            p.drawLine(QLineF(x + dotd+(x1 * dotd), y-(y2 * dotd),        x-(x2 * dotd), y + dotd+(y2 * dotd) ));//BL-TR
            break;
        case FretDotType::SQUARE:
            p.setBrush(Qt::NoBrush);
            p.setPen(pen);
            p.drawRect(QRectF(x-(x1 * dotd), y-(y1 * dotd), dotd+(x2 * dotd), dotd+(y2 * dotd)));
            break;
        case FretDotType::TRIANGLE:
            p.setBrush(Qt::NoBrush);
            p.setPen(pen);
            p.drawLine(QLineF(x-(x1 * dotd), y + dotd+(y2 * dotd), x + (x2 * dotd), y-(y2 * dotd)));//L.BL-T
            p.drawLine(QLineF(x + (x2 * dotd), y-(y2 * dotd), x + dotd+(x1 * dotd), y + dotd+(y2 * dotd)));//R.T_BR
            p.drawLine(QLineF(x + dotd+(x1 * dotd), y + dotd+(y2 * dotd), x-(x1* dotd), y + dotd+(y2 * dotd)));//B.R-L
            break;
        case FretDotType::CIRCLE_OPTIONAL:
            p.setBrush(Qt::NoBrush);
            p.setPen(pen);
            p.drawEllipse(QRectF( x+(x1 * dotd), y+(y1 * dotd), dotd*x2, dotd*y2));
            break;
        default:
            break;
    }
            
}
    
//---------------------------------------------------------
//   paintDotSymbol
//---------------------------------------------------------

void FretCanvas::paintDotSymbol(QPainter& p, QPen& pen, qreal x, qreal y, qreal dotd, FretDotType dtype)
      {
      switch (dtype) {
            case FretDotType::NORMAL:
                 paintDotShape(p, pen, x, y, dotd, FretDotType::NORMAL, .0, .0, 1.0, 1.0);
                 break;
            case FretDotType::CROSS:
                 paintDotShape(p, pen, x, y, dotd, FretDotType::CROSS, .2, .2, .2, .2);
                 break;
            case FretDotType::SQUARE:
                 paintDotShape(p, pen, x, y, dotd, FretDotType::SQUARE, .0, .0, .0, .0);
                 break;
            case FretDotType::TRIANGLE:
                 paintDotShape(p, pen, x, y, dotd, FretDotType::TRIANGLE, .2, .5, .5, .2);
                 break;
            case FretDotType::CIRCLE_OPTIONAL:
                 paintDotShape(p, pen, x, y, dotd, FretDotType::CIRCLE_OPTIONAL, .0, .0, 1.0, 1.0);
                 break;
            case FretDotType::DOT_CROSS:
                 paintDotShape(p, pen, x, y, dotd, FretDotType::NORMAL, .0, .0, 1.0, 1.0);
                 paintDotShape(p, pen, x, y, dotd, FretDotType::CROSS, .2, .2, .2, .2);
                 break;
            case FretDotType::DOT_SQUARE:
                 paintDotShape(p, pen, x, y, dotd, FretDotType::NORMAL, .0, .0, 1.0, 1.0);
                 paintDotShape(p, pen, x, y, dotd, FretDotType::SQUARE, .2, .2, .4, .4);
                 break;
            case FretDotType::DOT_TRIANGLE:
                 paintDotShape(p, pen, x, y, dotd, FretDotType::NORMAL, .2, .4, .6, .6);
                 paintDotShape(p, pen, x, y, dotd, FretDotType::TRIANGLE, .2, .5, .5, .2);
                 break;
            case FretDotType::DOT_OPTIONAL:
                  paintDotShape(p, pen, x, y, dotd, FretDotType::NORMAL, .0, .0, 1.0, 1.0);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CIRCLE_OPTIONAL, -.4, -.4, 1.8, 1.8);
                  break;
            case FretDotType::CROSS_SQUARE:
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CROSS, -.2, -.2, -.2, -.2);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::SQUARE, .3, .3, .6, .6);
                  break;
            case FretDotType::CROSS_TRIANGLE:
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CROSS, -.3, -.3, -.3, -.3);
                  paintDotShape(p, pen, x, y-(dotd/3), dotd, FretDotType::TRIANGLE, .6, .5, .5, .6);//5.0
                  break;
            case FretDotType::CROSS_OPTIONAL:
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CROSS, .0, .0, .0, .0);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CIRCLE_OPTIONAL, -.4, -.4, 1.8, 1.8);
                  break;
            case FretDotType::SQUARE_TRIANGLE:
                  paintDotShape(p, pen, x, y+(dotd/7.5), dotd, FretDotType::SQUARE, -.2, -.2, -.4, -.4);//2.0
                  paintDotShape(p, pen, x, y-(dotd/3.0), dotd, FretDotType::TRIANGLE, .6, .5, .5, .6);//5.0
                  break;
            case FretDotType::SQUARE_OPTIONAL:
                  paintDotShape(p, pen, x, y, dotd, FretDotType::SQUARE, .0, .0, .0, .0);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CIRCLE_OPTIONAL, -.4, -.4, 1.8, 1.8);
                  break;
            case FretDotType::TRIANGLE_OPTIONAL:
                  paintDotShape(p, pen, x, y, dotd, FretDotType::TRIANGLE, -.1, .5, .5, -.1);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CIRCLE_OPTIONAL, -.5, -.5, 2.0, 2.0);
                  break;
            case FretDotType::DOT_CROSS_SQUARE:
                  paintDotShape(p, pen, x, y, dotd, FretDotType::NORMAL, .0, .0, 1.0, 1.0);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CROSS, .2, .2, .2, .2);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::SQUARE, .6, .6, 1.2, 1.2);
                  break;
            case FretDotType::DOT_CROSS_TRIANGLE:
                  paintDotShape(p, pen, x, y, dotd, FretDotType::NORMAL, .0, .0, 1.0, 1.0);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CROSS, .0, .0, .0, .0);
                  paintDotShape(p, pen, x, y-((2*dotd)/3), dotd, FretDotType::TRIANGLE, 1.2, .5, .5, 1.2);//10
                  break;
            case FretDotType::DOT_CROSS_OPTIONAL:
                  paintDotShape(p, pen, x, y, dotd, FretDotType::NORMAL, .0, .0, 1.0, 1.0);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CROSS, .2, .2, .2, .2);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CIRCLE_OPTIONAL, -1.0, -1.0, 3.0, 3.0);
                  break;
            case FretDotType::DOT_SQUARE_TRIANGLE:
                  paintDotShape(p, pen, x, y, dotd, FretDotType::NORMAL, .0, .0, 1.0, 1.0);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::SQUARE, .2, .2, .4, .4);
                  paintDotShape(p, pen, x, y-dotd, dotd, FretDotType::TRIANGLE, 1.2, .5, .5, 1.2);//14
                  break;
            case FretDotType::DOT_SQUARE_OPTIONAL:
                  paintDotShape(p, pen, x, y, dotd, FretDotType::NORMAL, .0, .0, 1.0, 1.0);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::SQUARE, .4, .4, .8, .8);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CIRCLE_OPTIONAL, -1.0, -1.0, 3.0, 3.0);
                  break;
            case FretDotType::DOT_TRIANGLE_OPTIONAL:
                  paintDotShape(p, pen, x, y, dotd, FretDotType::NORMAL, .0, .0, 1.0, 1.0);
                  paintDotShape(p, pen, x, y-(dotd/3.0), dotd, FretDotType::TRIANGLE, .6, .5, .5, .6);//5
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CIRCLE_OPTIONAL, -1.0, -1.0, 3.0, 3.0);
                  break;
            case FretDotType::CROSS_SQUARE_TRIANGLE:
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CROSS, .0, .0, .0, .0);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::SQUARE, .2, .2, .4, .4);
                  paintDotShape(p, pen, x, y-(dotd*.8), dotd, FretDotType::TRIANGLE, 1.2, .5, .5, 1.2);//12
               break;
            case FretDotType::CROSS_SQUARE_OPTIONAL:
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CROSS, -.2, -.2, -.2, -.2);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::SQUARE, .4, .4, .8, .8);
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CIRCLE_OPTIONAL, -1.0, -1.0, 3.0, 3.0);
               break;
            case FretDotType::CROSS_TRIANGLE_OPTIONAL:
                  paintDotShape(p, pen, x, y+(dotd/7.5), dotd, FretDotType::CROSS, -.2, -.2, -.2, -.2);//2
                  paintDotShape(p, pen, x, y-(dotd/3.0), dotd, FretDotType::TRIANGLE, .6, .5, .5, .6);//5
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CIRCLE_OPTIONAL, -1.0, -1.0, 3.0, 3.0);
               break;
            case FretDotType::SQUARE_TRIANGLE_OPTIONAL:
                  paintDotShape(p, pen, x, y, dotd, FretDotType::SQUARE, -.2, -.2, -.4, -.4);
                  paintDotShape(p, pen, x, y-(dotd/3.0), dotd, FretDotType::TRIANGLE, .6, .5, .5, .6);//5.0
                  paintDotShape(p, pen, x, y, dotd, FretDotType::CIRCLE_OPTIONAL, -1.0, -1.0, 3.0, 3.0);
               break;
            default:
//                  p.setBrush(pen.color());
//                  p.setPen(Qt::NoPen);
//                  p.drawEllipse(QRectF(x, y, dotd, dotd));
                  break;
            }
      }

//---------------------------------------------------------
//   getPosition
//---------------------------------------------------------

void FretCanvas::getPosition(const QPointF& p, int* string, int* fret)
      {
      double mag = 2.0;//1.5
      double _spatium   = 20.0 * mag;
      int _strings      = diagram->strings();
      int _frets        = diagram->frets();
      double stringDist = _spatium * .7;
      double fretDist   = _spatium * .8;

      double w  = (_strings - 1) * stringDist;
      double xo = (width() - w) * .5;//.5
      double h  = (_frets * fretDist) + fretDist * .5;
      double yo = (height() - h) * .9;//.5
      *fret  = (p.y() - yo + fretDist) / fretDist;
      *string = (p.x() - xo + stringDist * .5) / stringDist;
      }

//---------------------------------------------------------
//   mousePressEvent
//---------------------------------------------------------

void FretCanvas::mousePressEvent(QMouseEvent* ev)
      {
      int string;
      int fret;
      getPosition(ev->pos(), &string, &fret);

      int _strings = diagram->strings();
      int _frets   = diagram->frets();
      if (fret < 0 || fret > _frets || string < 0 || string >= _strings)
            return;

      diagram->score()->startCmd();

      // Click above the fret diagram, so change the open/closed string marker
      if (fret == 0) {
            switch (diagram->marker(string).mtype) {
                  case FretMarkerType::CIRCLE:
                        diagram->undoSetFretMarker(string, FretMarkerType::DOT);
                        break;
                  case FretMarkerType::DOT:
                        diagram->undoSetFretMarker(string, FretMarkerType::CROSS);
                        break;
                  case FretMarkerType::CROSS:
                        diagram->undoSetFretMarker(string, FretMarkerType::SQUARE);
                        break;
                  case FretMarkerType::SQUARE:
                        diagram->undoSetFretMarker(string, FretMarkerType::TRIANGLE);
                        break;
                  case FretMarkerType::TRIANGLE:
                        diagram->undoSetFretMarker(string, FretMarkerType::CIRCLE_OPTIONAL);
                        break;
                  case FretMarkerType::CIRCLE_OPTIONAL:
                        diagram->undoSetFretMarker(string, FretMarkerType::DOT_CROSS);
                        break;
                  case FretMarkerType::DOT_CROSS:
                        diagram->undoSetFretMarker(string, FretMarkerType::DOT_SQUARE);
                        break;
                  case FretMarkerType::DOT_SQUARE:
                        diagram->undoSetFretMarker(string, FretMarkerType::DOT_TRIANGLE);
                        break;
                  case FretMarkerType::DOT_TRIANGLE:
                        diagram->undoSetFretMarker(string, FretMarkerType::DOT_OPTIONAL);
                        break;
                  case FretMarkerType::DOT_OPTIONAL:
                        diagram->undoSetFretMarker(string, FretMarkerType::CROSS_SQUARE);
                        break;
                  case FretMarkerType::CROSS_SQUARE:
                        diagram->undoSetFretMarker(string, FretMarkerType::CROSS_TRIANGLE);
                        break;
                  case FretMarkerType::CROSS_TRIANGLE:
                        diagram->undoSetFretMarker(string, FretMarkerType::CROSS_OPTIONAL);
                        break;
                  case FretMarkerType::CROSS_OPTIONAL:
                        diagram->undoSetFretMarker(string, FretMarkerType::SQUARE_TRIANGLE);
                        break;
                  case FretMarkerType::SQUARE_TRIANGLE:
                        diagram->undoSetFretMarker(string, FretMarkerType::SQUARE_OPTIONAL);
                        break;
                  case FretMarkerType::SQUARE_OPTIONAL:
                        diagram->undoSetFretMarker(string, FretMarkerType::TRIANGLE_OPTIONAL);
                        break;
                  case FretMarkerType::TRIANGLE_OPTIONAL:
                        diagram->undoSetFretMarker(string, FretMarkerType::DOT_CROSS_SQUARE);
                        break;
                  case FretMarkerType::DOT_CROSS_SQUARE:
                        diagram->undoSetFretMarker(string, FretMarkerType::DOT_CROSS_TRIANGLE);
                        break;
                  case FretMarkerType::DOT_CROSS_TRIANGLE:
                        diagram->undoSetFretMarker(string, FretMarkerType::DOT_CROSS_OPTIONAL);
                        break;
                  case FretMarkerType::DOT_CROSS_OPTIONAL:
                        diagram->undoSetFretMarker(string, FretMarkerType::DOT_SQUARE_TRIANGLE);
                        break;
                  case FretMarkerType::DOT_SQUARE_TRIANGLE:
                        diagram->undoSetFretMarker(string, FretMarkerType::DOT_SQUARE_OPTIONAL);
                        break;
                  case FretMarkerType::DOT_SQUARE_OPTIONAL:
                        diagram->undoSetFretMarker(string, FretMarkerType::DOT_TRIANGLE_OPTIONAL);
                        break;
                  case FretMarkerType::DOT_TRIANGLE_OPTIONAL:
                        diagram->undoSetFretMarker(string, FretMarkerType::CROSS_SQUARE_TRIANGLE);
                        break;
                  case FretMarkerType::CROSS_SQUARE_TRIANGLE:
                        diagram->undoSetFretMarker(string, FretMarkerType::CROSS_SQUARE_OPTIONAL);
                        break;
                  case FretMarkerType::CROSS_SQUARE_OPTIONAL:
                        diagram->undoSetFretMarker(string, FretMarkerType::CROSS_TRIANGLE_OPTIONAL);
                        break;
                  case FretMarkerType::CROSS_TRIANGLE_OPTIONAL:
                        diagram->undoSetFretMarker(string, FretMarkerType::SQUARE_TRIANGLE_OPTIONAL);
                        break;
                  case FretMarkerType::SQUARE_TRIANGLE_OPTIONAL:
                        diagram->undoSetFretMarker(string, FretMarkerType::NONE);
                        break;

                    
                  case FretMarkerType::NONE:
                       diagram->undoSetFretMarker(string, FretMarkerType::CIRCLE);
                  default:
//                        diagram->undoSetFretDot(string, 0);
//                        diagram->undoSetFretMarker(string, FretMarkerType::CIRCLE);
                        break;
                  }
            }
      // Otherwise, the click is on the fretboard itself
      else {
            FretItem::Dot thisDot = diagram->dot(string, fret)[0];
            bool haveShift = (ev->modifiers() & Qt::ShiftModifier) || _barreMode;
            bool haveCtrl  = (ev->modifiers() & Qt::ControlModifier) || _multidotMode;

            // Click on an existing dot
            if (thisDot.exists() && !haveShift)//Click on dot and barreMode=ON
//                ;
                  diagram->undoSetFretDot(string, haveCtrl ? fret : 0, haveCtrl);
          
            else {
                  // Shift adds a barre
                  if (haveShift)//If in barreMode remove barre?
//                      ;
                  
                        diagram->undoSetFretBarre(string, fret, haveCtrl);
                  else {
                        FretDotType dtype = FretDotType::NORMAL;
                        //_automaticDotType ? FretDotType::NORMAL && _multidotMode ON && dot TRUE
                        if (_automaticDotType && haveCtrl && diagram->dot(string)[0].exists()) {
                              dtype = FretDotType::SQUARE_TRIANGLE_OPTIONAL;
//                              dtype = FretDotType::TRIANGLE;

                              std::vector<FretDotType> dtypes {
                                    FretDotType::NORMAL,
                                    FretDotType::CROSS,
                                    FretDotType::SQUARE,
                                    FretDotType::TRIANGLE,
                                    FretDotType::DOT_CROSS,
                                    FretDotType::DOT_SQUARE,
                                    FretDotType::DOT_TRIANGLE,
                                    FretDotType::CIRCLE_OPTIONAL,
                                      FretDotType::DOT_OPTIONAL,
                                      FretDotType::CROSS_SQUARE,
                                      FretDotType::CROSS_TRIANGLE,
                                      FretDotType::CROSS_OPTIONAL,
                                      FretDotType::SQUARE_TRIANGLE,
                                      FretDotType::SQUARE_OPTIONAL,
                                      FretDotType::TRIANGLE_OPTIONAL,
                                      FretDotType::DOT_CROSS_SQUARE,
                                      FretDotType::DOT_CROSS_TRIANGLE,
                                      FretDotType::DOT_CROSS_OPTIONAL,
                                      FretDotType::DOT_SQUARE_TRIANGLE,
                                      FretDotType::DOT_SQUARE_OPTIONAL,
                                      FretDotType::DOT_TRIANGLE_OPTIONAL,
                                      FretDotType::CROSS_SQUARE_TRIANGLE,
                                      FretDotType::CROSS_SQUARE_OPTIONAL,
                                      FretDotType::CROSS_TRIANGLE_OPTIONAL,
                                      FretDotType::SQUARE_TRIANGLE_OPTIONAL //25
//                                    FretDotType::DOT //TODO add dot
                              };

                              // Find the lowest dot type that doesn't already exist on the string
                              // Cycles if there is a FretDotTypes present if not adds the dot type when you click on fret
                              for (size_t i = 0; i < dtypes.size(); i++) {
                                    FretDotType t = dtypes[i];

                                    bool hasThisType = false;
                                    for (auto const& dot : diagram->dot(string)) {
                                          if (dot.dtype == t) {
                                                hasThisType = true;
                                                break;
                                                }
                                          }//End for

                                    if (hasThisType)
                                          continue;

                                    dtype = t;
                                    break;
                                    }//End for
                              }//End if (_automaticDotType && haveCtrl && diagram->dot(string)[0].exists())
                        else if (!_automaticDotType)
                              dtype = _currentDtype;

                        // Ctrl adds a dot without removing other dots on a string
                        // Adds dot>1 if _multidotMode=ON to string
                        diagram->undoSetFretDot(string, fret, haveCtrl, dtype);
                        }//End else
                  }//End if (thisDot.exists() && !haveShift) not in barreMode
            }
      diagram->triggerLayout();
      diagram->score()->endCmd();
      update();
      }

//---------------------------------------------------------
//   mouseMoveEvent
//---------------------------------------------------------

void FretCanvas::mouseMoveEvent(QMouseEvent* ev)
      {
      int string;
      int fret;
      getPosition(ev->pos(), &string, &fret);
      if (string != cstring || cfret != fret) {
            cfret = fret;
            cstring = string;
            update();
            }
      }

//---------------------------------------------------------
//   setFretDiagram
//---------------------------------------------------------

void FretCanvas::setFretDiagram(FretDiagram* fd)
      {
      diagram = fd;
      update();
      }

//---------------------------------------------------------
//   clear
//---------------------------------------------------------

void FretCanvas::clear()
      {
      diagram->score()->startCmd();
      diagram->undoFretClear();
      diagram->score()->endCmd();
      update();
      }
} // namespace Ms
