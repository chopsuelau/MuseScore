//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2012 Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENSE.GPL
//=============================================================================

#include "inspectorFret.h"
#include "libmscore/fret.h"
#include "libmscore/score.h"

namespace Ms {

//---------------------------------------------------------
//   InspectorFretDiagram
//---------------------------------------------------------

InspectorFretDiagram::InspectorFretDiagram(QWidget* parent)
   : InspectorElementBase(parent)
      {
      f.setupUi(addWidget());

      const std::vector<InspectorItem> iiList = {
            { Pid::COLOR,        0, e.color,       e.resetColor       },
            { Pid::VISIBLE,      0, e.visible,     e.resetVisible     },
            { Pid::OFFSET,       0, e.offset,      e.resetOffset      },
            { Pid::MAG,          0, f.mag,         f.resetMag         },
            { Pid::PLACEMENT,    0, f.placement,   f.resetPlacement   },
            { Pid::FRET_STRINGS, 0, f.strings,     f.resetStrings     },
            { Pid::FRET_FRETS,   0, f.frets,       f.resetFrets       },
            { Pid::FRET_NUT,     0, f.showNut,     f.resetShowNut     },
            { Pid::ORIENTATION,  0, f.orientation, f.resetOrientation },
            };
      const std::vector<InspectorPanel> ppList = {
            { f.title, f.panel }
            };

      dotTypeButtons = {
            f.circleSelect,
            f.crossSelect,
            f.triangleSelect,
            f.squareSelect,
            f.circleOptionalSelect,
            f.dotCrossSelect,
            f.dotSquareSelect,
            f.dotTriangleSelect,
            f.dot_optionalSelect,
            f.cross_squareSelect,
            f.cross_triangleSelect,
            f.cross_optionalSelect,
            f.square_triangleSelect,
            f.square_optionalSelect,
            f.triangle_optionalSelect,
            f.dot_cross_squareSelect,
            f.dot_cross_triangleSelect,
            f.dot_cross_optionalSelect,
            f.dot_square_triangleSelect,
            f.dot_square_optionalSelect,
            f.dot_triangle_optionalSelect,
            f.cross_square_triangleSelect,
            f.cross_square_optionalSelect,
            f.cross_triangle_optionalSelect,
            f.square_triangle_optionalSelect
         
            };

      mapSignals(iiList, ppList);

      FretDiagram* diagram = toFretDiagram(inspector->element());
      int fretNumber = diagram->fretOffset() + 1;
      f.fretNumber->setValue(fretNumber);
      f.resetFretNumber->setEnabled(fretNumber != 1);

      connect(f.fretNumber,      SIGNAL(valueChanged(int)), SLOT(fretNumberChanged(int)));
      connect(f.resetFretNumber, SIGNAL(resetClicked()),    SLOT(resetFretNumber()));

      connect(f.circleSelect,   SIGNAL(toggled(bool)), SLOT(circleButtonToggled(bool)));
      connect(f.crossSelect,    SIGNAL(toggled(bool)), SLOT(crossButtonToggled(bool)));
      connect(f.triangleSelect, SIGNAL(toggled(bool)), SLOT(triangleButtonToggled(bool)));
      connect(f.squareSelect,   SIGNAL(toggled(bool)), SLOT(squareButtonToggled(bool)));
      connect(f.circleOptionalSelect,   SIGNAL(toggled(bool)), SLOT(circleOptionalButtonToggled(bool)));
      connect(f.dotCrossSelect,   SIGNAL(toggled(bool)), SLOT(dotCrossButtonToggled(bool)));
      connect(f.dotSquareSelect,    SIGNAL(toggled(bool)), SLOT(dotSquareButtonToggled(bool)));
      connect(f.dotTriangleSelect, SIGNAL(toggled(bool)), SLOT(dotTriangleButtonToggled(bool)));
      
      connect(f.dot_optionalSelect, SIGNAL(toggled(bool)), SLOT(dot_optionalButtonToggled(bool)));
      connect(f.cross_squareSelect, SIGNAL(toggled(bool)), SLOT(cross_squareButtonToggled(bool)));
      connect(f.cross_triangleSelect, SIGNAL(toggled(bool)), SLOT(cross_triangleButtonToggled(bool)));
      connect(f.cross_optionalSelect, SIGNAL(toggled(bool)), SLOT(cross_optionalButtonToggled(bool)));
      connect(f.square_triangleSelect, SIGNAL(toggled(bool)), SLOT(square_triangleButtonToggled(bool)));
      connect(f.square_optionalSelect, SIGNAL(toggled(bool)), SLOT(square_optionalButtonToggled(bool)));
      connect(f.triangle_optionalSelect, SIGNAL(toggled(bool)), SLOT(triangle_optionalButtonToggled(bool)));
      connect(f.dot_cross_squareSelect, SIGNAL(toggled(bool)), SLOT(dot_cross_squareButtonToggled(bool)));
      connect(f.dot_cross_triangleSelect, SIGNAL(toggled(bool)), SLOT(dot_cross_triangleButtonToggled(bool)));
      connect(f.dot_cross_optionalSelect, SIGNAL(toggled(bool)), SLOT(dot_cross_optionalButtonToggled(bool)));
      connect(f.dot_square_triangleSelect, SIGNAL(toggled(bool)), SLOT(dot_square_triangleButtonToggled(bool)));
      connect(f.dot_square_optionalSelect, SIGNAL(toggled(bool)), SLOT(dot_square_optionalButtonToggled(bool)));
      connect(f.dot_triangle_optionalSelect, SIGNAL(toggled(bool)), SLOT(dot_triangle_optionalButtonToggled(bool)));
      connect(f.cross_square_triangleSelect, SIGNAL(toggled(bool)), SLOT(cross_square_triangleButtonToggled(bool)));
      connect(f.cross_square_optionalSelect, SIGNAL(toggled(bool)), SLOT(cross_square_optionalButtonToggled(bool)));
      connect(f.cross_triangle_optionalSelect, SIGNAL(toggled(bool)), SLOT(cross_triangle_optionalButtonToggled(bool)));
      connect(f.square_triangle_optionalSelect, SIGNAL(toggled(bool)), SLOT(square_triangle_optionalButtonToggled(bool)));
          
      connect(f.toggleBarre,    SIGNAL(toggled(bool)), SLOT(barreButtonToggled(bool)));
      connect(f.toggleMultidot, SIGNAL(toggled(bool)), SLOT(multidotButtonToggled(bool)));
      connect(f.clearButton,    SIGNAL(clicked()),     SLOT(clearButtonClicked()));
      }

//---------------------------------------------------------
//   valueChanged
//---------------------------------------------------------

void InspectorFretDiagram::valueChanged(int idx)
      {
      InspectorElementBase::valueChanged(idx);
      FretDiagram* fd = toFretDiagram(inspector->element());
      f.diagram->setFretDiagram(fd);
      }

//---------------------------------------------------------
//   setElement
//---------------------------------------------------------

void InspectorFretDiagram::setElement()
      {
      InspectorElementBase::setElement();
      FretDiagram* fd = toFretDiagram(inspector->element());
      f.diagram->setFretDiagram(fd);
      }

//---------------------------------------------------------
//   fretNumberChanged
//---------------------------------------------------------

void InspectorFretDiagram::fretNumberChanged(int fretNumber)
      {
      FretDiagram* fd = toFretDiagram(inspector->element());
      fd->score()->startCmd();
      fd->undoChangeProperty(Pid::FRET_OFFSET, fretNumber - 1);
      fd->score()->endCmd();
      f.resetFretNumber->setEnabled(fretNumber != 1);
      f.diagram->setFretDiagram(fd);
      }

//---------------------------------------------------------
//   resetFretNumberClicked
//---------------------------------------------------------

void InspectorFretDiagram::resetFretNumber()
      {
      FretDiagram* fd = toFretDiagram(inspector->element());
      int fretNumber = 1;
      fd->score()->startCmd();
      fd->undoChangeProperty(Pid::FRET_OFFSET, fretNumber - 1);
      fd->score()->endCmd();
      f.fretNumber->setValue(fretNumber);
      f.resetFretNumber->setEnabled(false);
      f.diagram->setFretDiagram(fd);
      }

//---------------------------------------------------------
//   genericButtonToggled
//---------------------------------------------------------

void InspectorFretDiagram::genericButtonToggled(QPushButton* b, bool v, FretDotType dtype)
      {
      for (QPushButton* p : dotTypeButtons) {
            p->blockSignals(true);
            p->setChecked(false);
            }

      if (v) {
            f.diagram->setCurrentDotType(dtype);
            b->setChecked(true);
            }

      f.diagram->setAutomaticDotType(!v);

      for (QPushButton* p : dotTypeButtons)
            p->blockSignals(false);
      }
//---------------------------------------------------------
//   circleButtonToggled
//---------------------------------------------------------

void InspectorFretDiagram::circleButtonToggled(bool v)
      {
      genericButtonToggled(f.circleSelect, v, FretDotType::NORMAL);
      }

//---------------------------------------------------------
//   crossButtonToggled
//---------------------------------------------------------

void InspectorFretDiagram::crossButtonToggled(bool v)
      {
      genericButtonToggled(f.crossSelect, v, FretDotType::CROSS);
      }

//---------------------------------------------------------
//   squareButtonToggled
//---------------------------------------------------------

void InspectorFretDiagram::squareButtonToggled(bool v)
      {
      genericButtonToggled(f.squareSelect, v, FretDotType::SQUARE);
      }

//---------------------------------------------------------
//   triangleButtonToggled
//---------------------------------------------------------

void InspectorFretDiagram::triangleButtonToggled(bool v)
      {
      genericButtonToggled(f.triangleSelect, v, FretDotType::TRIANGLE);
      }

 //---------------------------------------------------------
 //   dotCrossButtonToggled
 //---------------------------------------------------------
 
void InspectorFretDiagram::dotCrossButtonToggled(bool v)
      {
        genericButtonToggled(f.dotCrossSelect, v, FretDotType::DOT_CROSS);
      }
 
 //---------------------------------------------------------
 //   dotSquareButtonToggled
 //---------------------------------------------------------
 
void InspectorFretDiagram::dotSquareButtonToggled(bool v)
      {
        genericButtonToggled(f.dotSquareSelect, v, FretDotType::DOT_SQUARE);
      }

//---------------------------------------------------------
//   circleOptionalButtonToggled
//---------------------------------------------------------

void InspectorFretDiagram::circleOptionalButtonToggled(bool v)
      {
         genericButtonToggled(f.circleOptionalSelect, v, FretDotType::CIRCLE_OPTIONAL);
      }

 //---------------------------------------------------------
 //   dotTriangleButtonToggled
 //---------------------------------------------------------

   
void InspectorFretDiagram::dotTriangleButtonToggled(bool v)
      {
         genericButtonToggled(f.dotTriangleSelect, v, FretDotType::DOT_TRIANGLE);
      }
   
void InspectorFretDiagram::dot_optionalButtonToggled(bool v)
      {
         genericButtonToggled(f.dot_optionalSelect, v, FretDotType::DOT_OPTIONAL);
      }
void InspectorFretDiagram::cross_squareButtonToggled(bool v)
      {
         genericButtonToggled(f.cross_squareSelect, v, FretDotType::CROSS_SQUARE);
      }
void InspectorFretDiagram::cross_triangleButtonToggled(bool v)
      {
         genericButtonToggled(f.cross_triangleSelect, v, FretDotType::CROSS_TRIANGLE);
      }
void InspectorFretDiagram::cross_optionalButtonToggled(bool v)
      {
         genericButtonToggled(f.cross_optionalSelect, v, FretDotType::CROSS_OPTIONAL);
      }
void InspectorFretDiagram::square_triangleButtonToggled(bool v)
      {
         genericButtonToggled(f.square_triangleSelect, v, FretDotType::SQUARE_TRIANGLE);
      }
void InspectorFretDiagram::square_optionalButtonToggled(bool v)
      {
         genericButtonToggled(f.square_optionalSelect, v, FretDotType::SQUARE_OPTIONAL);
      }
void InspectorFretDiagram::triangle_optionalButtonToggled(bool v)
      {
         genericButtonToggled(f.triangle_optionalSelect, v, FretDotType::TRIANGLE_OPTIONAL);
      }
void InspectorFretDiagram::dot_cross_squareButtonToggled(bool v)
      {
         genericButtonToggled(f.dot_cross_squareSelect, v, FretDotType::DOT_CROSS_SQUARE);
      }
void InspectorFretDiagram::dot_cross_triangleButtonToggled(bool v)
      {
         genericButtonToggled(f.dot_cross_triangleSelect, v, FretDotType::DOT_CROSS_TRIANGLE);
      }
void InspectorFretDiagram::dot_cross_optionalButtonToggled(bool v)
      {
         genericButtonToggled(f.dot_cross_optionalSelect, v, FretDotType::DOT_CROSS_OPTIONAL);
      }
void InspectorFretDiagram::dot_square_triangleButtonToggled(bool v)
      {
         genericButtonToggled(f.dot_square_triangleSelect, v, FretDotType::DOT_SQUARE_TRIANGLE);
      }
void InspectorFretDiagram::dot_square_optionalButtonToggled(bool v)
      {
         genericButtonToggled(f.dot_square_optionalSelect, v, FretDotType::DOT_SQUARE_OPTIONAL);
      }
void InspectorFretDiagram::dot_triangle_optionalButtonToggled(bool v)
      {
         genericButtonToggled(f.dot_triangle_optionalSelect, v, FretDotType::DOT_TRIANGLE_OPTIONAL);
      }
void InspectorFretDiagram::cross_square_triangleButtonToggled(bool v)
      {
         genericButtonToggled(f.cross_square_triangleSelect, v, FretDotType::CROSS_SQUARE_TRIANGLE);
      }
void InspectorFretDiagram::cross_square_optionalButtonToggled(bool v)
      {
         genericButtonToggled(f.cross_square_optionalSelect, v, FretDotType::CROSS_SQUARE_OPTIONAL);
      }
void InspectorFretDiagram::cross_triangle_optionalButtonToggled(bool v)
      {
         genericButtonToggled(f.cross_triangle_optionalSelect, v, FretDotType::CROSS_TRIANGLE_OPTIONAL);
      }
void InspectorFretDiagram::square_triangle_optionalButtonToggled(bool v)
      {
         genericButtonToggled(f.square_triangle_optionalSelect, v, FretDotType::SQUARE_TRIANGLE_OPTIONAL);
      }


   
   
   
   
   
   
   
   
//---------------------------------------------------------
//   barreButtonToggled
//---------------------------------------------------------

void InspectorFretDiagram::barreButtonToggled(bool v)
      {
      f.diagram->setBarreMode(v);
      }

//---------------------------------------------------------
//   multidotButtonToggled
//---------------------------------------------------------

void InspectorFretDiagram::multidotButtonToggled(bool v)
      {
      f.diagram->setMultidotMode(v);
      }

//---------------------------------------------------------
//   clearButtonClicked
//---------------------------------------------------------

void InspectorFretDiagram::clearButtonClicked()
      {
      f.diagram->clear();
      }
}
