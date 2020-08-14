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

#ifndef __INSPECTOR_FRET_H__
#define __INSPECTOR_FRET_H__

#include "inspector.h"
#include "inspectorBase.h"
#include "ui_inspector_fret.h"

namespace Ms {

//---------------------------------------------------------
//   InspectorFretDiagram
//---------------------------------------------------------

class InspectorFretDiagram : public InspectorElementBase {
      Q_OBJECT

      Ui::InspectorFretDiagram   f;
      virtual void setElement() override;
      void genericButtonToggled(QPushButton* b, bool v, FretDotType dtype);
      std::vector<QPushButton*> dotTypeButtons;

   private slots:
      virtual void valueChanged(int idx) override;
      void fretNumberChanged(int fretNumber);
      void resetFretNumber();

      void circleButtonToggled(bool v);
      void crossButtonToggled(bool v);
      void squareButtonToggled(bool v);
      void triangleButtonToggled(bool v);
    
      void circleOptionalButtonToggled(bool v);
      void dotCrossButtonToggled(bool v);
      void dotSquareButtonToggled(bool v);
      void dotTriangleButtonToggled(bool v);
    
      void dot_optionalButtonToggled(bool v);
      void cross_squareButtonToggled(bool v);
      void cross_triangleButtonToggled(bool v);
      void cross_optionalButtonToggled(bool v);
      void square_triangleButtonToggled(bool v);
      void square_optionalButtonToggled(bool v);
      void triangle_optionalButtonToggled(bool v);
      void dot_cross_squareButtonToggled(bool v);
      void dot_cross_triangleButtonToggled(bool v);
      void dot_cross_optionalButtonToggled(bool v);
      void dot_square_triangleButtonToggled(bool v);
      void dot_square_optionalButtonToggled(bool v);
      void dot_triangle_optionalButtonToggled(bool v);
      void cross_square_triangleButtonToggled(bool v);
      void cross_square_optionalButtonToggled(bool v);
      void cross_triangle_optionalButtonToggled(bool v);
      void square_triangle_optionalButtonToggled(bool v);
    
      void barreButtonToggled(bool v);
      void multidotButtonToggled(bool v);
      void clearButtonClicked();

   public:
      InspectorFretDiagram(QWidget* parent);
      };


} // namespace Ms
#endif

