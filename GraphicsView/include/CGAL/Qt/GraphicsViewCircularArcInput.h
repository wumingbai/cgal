// Copyright (c) 2008  GeometryFactory Sarl (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Andreas Fabri <Andreas.Fabri@geometryfactory.com>
//                 Laurent Rineau <Laurent.Rineau@geometryfactory.com>

#ifndef CGAL_QT_GRAPHICS_VIEW_CIRCULAR_ARC_INPUT_H
#define CGAL_QT_GRAPHICS_VIEW_CIRCULAR_ARC_INPUT_H

#include <QGraphicsView>
#include <QRectF>
#include <QPointF>
#include <QGraphicsItem>
#include <QGraphicsEllipseItem> 
#include <QGraphicsLineItem> 
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include <CGAL/Qt/Converter.h>
#include <CGAL/Qt/GraphicsViewInput.h>
#include <CGAL/Qt/CircularArcGraphicsItem.h>

#include <CGAL/array.h>

namespace CGAL {
namespace Qt {

template <typename K>
class GraphicsViewCircularArcInput : public GraphicsViewInput
{
public:
  GraphicsViewCircularArcInput(QObject *parent, QGraphicsScene* s); 

protected:
    
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  virtual void keyPressEvent(QKeyEvent *event);
  
  bool eventFilter(QObject *obj, QEvent *event);
  

  

private:
  typedef typename K::Circular_arc_2 Circular_arc_2;
  typedef typename K::Point_2 Point_2;
  int count;
  QGraphicsLineItem *qline;
  CircularArcGraphicsItem<K> *qcarc;
  QPointF qp, qq, qr;
  Point_2 p, q, r;
  QGraphicsScene *scene_;  
  Converter<K> convert;
};


template <typename K>
GraphicsViewCircularArcInput<K>::GraphicsViewCircularArcInput(QObject *parent, QGraphicsScene* s)
  : GraphicsViewInput(parent), qline(new QGraphicsLineItem()), qcarc(new CircularArcGraphicsItem<K>()), scene_(s), count(0)
{
  qline->hide();
  qcarc->hide();
  s->addItem(qline);
  s->addItem(qcarc);
}


template <typename K>
void 
GraphicsViewCircularArcInput<K>::mousePressEvent(QGraphicsSceneMouseEvent *event)
{ 
  if(count == 0){
    qp = event->scenePos();
    p = convert(qp);
    qline->setLine(QLineF(qp, qp));
    qline->show();
    count = 1;
  } else if(count == 1){
    qq = event->scenePos();
    qline->setLine(QLineF(qp, qq));
    q = convert(qq);
    count = 2;
  } else if(count == 2){
    qr  = event->scenePos();
    r = convert(qr);
    typename K::Collinear_2 collinear;
    if(! collinear(p,q,r)){
      qcarc->hide();
      if(CGAL::orientation(p, r, q) == CGAL::COUNTERCLOCKWISE){
	emit generate(CGAL::make_object(Circular_arc_2(q,r,p)));
      } else {
	emit generate(CGAL::make_object(Circular_arc_2(p,r,q)));
      }
      count = 0;
    }
  }
}


template <typename K>
void 
GraphicsViewCircularArcInput<K>::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if(count == 0){
    qcarc->hide();
    qline->hide();
    return;
  } else if(count == 1) {
    qq = event->scenePos();
    q = convert(qq);
    qline->setLine(QLineF(qp, qq));
  } else if(count == 2){
    qline->hide();
    qr = event->scenePos();
    r = convert(qr);
    typename K::Collinear_2 collinear;
    if(collinear(p,q,r)){
      qcarc->hide();
      return;
    } else {
      if(CGAL::orientation(p, r, q) == CGAL::COUNTERCLOCKWISE) {
	qcarc->setArc(Circular_arc_2(q,r,p));
      } else {
	qcarc->setArc(Circular_arc_2(p,r,q));
      }
      qcarc->show();
    }
  }
}


template <typename K>
void 
GraphicsViewCircularArcInput<K>::keyPressEvent ( QKeyEvent * event ) 
{
  if(event->key() == ::Qt::Key_Delete){
    if(count>0){
      --count;
    }
  }
}



template <typename K>
bool 
GraphicsViewCircularArcInput<K>::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::GraphicsSceneMousePress) {
    QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
    mousePressEvent(mouseEvent);
    return true;
  } else if (event->type() == QEvent::GraphicsSceneMouseMove) {
    QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
    mouseMoveEvent(mouseEvent);
    return true;
  } else if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    keyPressEvent(keyEvent);
    return true;
  } else{
    // standard event processing
    return QObject::eventFilter(obj, event);
  }
} 

} // namespace Qt
} // namespace CGAL

#endif // CGAL_QT_GRAPHICS_VIEW_CIRCULAR_ARC_INPUT_H
