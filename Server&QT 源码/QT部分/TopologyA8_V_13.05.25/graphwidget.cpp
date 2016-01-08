/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "graphwidget.h"
#include "edge.h"
#include "node.h"

#include <QtGui>

#include <math.h>
#include <QDebug>
#include <QTime>
//#include <QSize>


//! [0]
GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), timerId(0)
{

    button = new QPushButton(this);
    button->setGeometry(695,310, 80,80);
    button->setFlat(true);
    pixmap.load("/usr/local/SmartHouse/401/images/topology/back.png");
    button->setIconSize(QSize(80,80));
    button->setIcon(pixmap);

    connect(button, SIGNAL(clicked()), this, SLOT(on_btnBack_clicked()));
    sceneView = new QGraphicsScene(this);
    sceneView->setItemIndexMethod(QGraphicsScene::NoIndex);
    sceneView->setSceneRect(-100, -50, 300, 150);
    setScene(sceneView);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(2.5), qreal(2.5));
    showFullScreen();
    //setMinimumSize(800, 400);
    setWindowTitle(tr("Topology"));
    hide();
//! [0]

//! [1]


    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(killNode()));
    timer->start(1000);




}
//! [1]

//! [2]
void GraphWidget::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}
//! [2]

//! [3]
void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        //centerNode->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        //centerNode->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        //centerNode->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        //centerNode->moveBy(20, 0);
        break;
    case Qt::Key_Plus:
        scaleView(qreal(1.2));
        break;
    case Qt::Key_Minus:
        scaleView(1 / qreal(1.2));
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        foreach (QGraphicsItem *item, scene()->items()) {
            if (qgraphicsitem_cast<Node *>(item))
                item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
        }
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}
//! [3]

//! [4]
void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    foreach (Node *node, nodes)
        node->calculateForces();

    bool itemsMoved = false;
    foreach (Node *node, nodes) {
        if (node->advance())
            itemsMoved = true;
    }

    if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
}
//! [4]

//! [5]
void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
//! [5]

//! [6]
void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersect(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    // Text
    QRectF textRect(sceneRect.left() + 8, sceneRect.top() + 8,
                    sceneRect.width(), sceneRect.height());
    QString message(tr("ChinaSofti \r\ntopology"));
    //"Click and drag the nodes around, and zoom with the mouse "
    //"wheel or the '+' and '-' keys"
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(18);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, message);
}
//! [6]

//! [7]
void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}
//! [7]
void GraphWidget::on_btnBack_clicked()
{
    emit closeWindow();
}

void GraphWidget::addNode(Node *node)
{
    nodeList << node;
}

QList<Node *> GraphWidget::nodes() const
{
    return nodeList;
}

void GraphWidget::buildNodeLink(QColor color,
                                QColor darkColor,
                                char *name,
                                int selfaddr,
                                int parentaddr)
{
    Node *myParent;
    myParent = findNode(parentaddr);
    if (myParent != 0){
        Node *child = new Node(this);
        child = findNode(selfaddr);
        if (child == 0){
            Node *node = new Node(this);
            nodeList.append(node);
            node->setColor(color, darkColor);
            node->setName(name);
            node->selfAddr = selfaddr;
            node->parentAddr = parentaddr;
//            qDebug()<<"selfaddr is $$$$$$$$$$$$$$$$$"<<selfaddr<<endl;
            node->setPos(qreal(selfaddr / 1000),qreal(selfaddr % 10));
            sceneView->addItem(node);
//            qDebug()<<"add a node in topology***********"<<endl;

                Edge *edge = new Edge(node, myParent);
                sceneView->addItem(edge);
                node->link = edge;
                myParent->childrenAddr << node->selfAddr;
//                qDebug()<<"find the parent&&&&&&&&&&&&&&"<<endl;
        }
        else{
                child->age = 0;
//                qDebug()<<"I am a little boy!^^^^^^^^^^^^^^^^"<<endl;
        }
    }


}

Node *GraphWidget::findNode(int addr)
{
    if (nodeList.isEmpty()){
//        qDebug()<<"add a Coor in topology***********"<<endl;
        return (0);
    }
    else{
        Node *childNode;
        for (int i = 0; i < nodeList.size(); i ++){
            childNode = dynamic_cast<Node *>(nodeList.at(i));
            if (childNode->selfAddr == addr){
                return (childNode);
            }
        }
        return (0);
    }
}

void GraphWidget::nodeAge(int age)
{
    Node *nodeOld = new Node(this);
    for (int i = 1; i < nodeList.size(); i ++){
        nodeOld = dynamic_cast<Node *>(nodeList.at(i));
        nodeOld->age ++;
//        qDebug()<<"the age of node is: gggggggggggg"<<nodeOld->age<<QString::number(nodeOld->selfAddr, 16)<<endl;
        if (nodeOld->age == age){
            killOld_node(nodeOld);
            break;
        }
    }

//            qDebug()<<"kill the node ##################$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;


}

void GraphWidget::killNode()
{
    nodeAge(10);
}

void GraphWidget::buildCoor(QColor color, QColor darkColor, char *name, int selfaddr, int parentaddr)
{
    Node *node = new Node(this);
    nodeList.append(node);
    node->setColor(color, darkColor);
    node->setName(name);
    node->selfAddr = selfaddr;
    node->parentAddr = parentaddr;
//    qDebug()<<"selfaddr is $$$$$$$$$$$$$$$$$"<<selfaddr<<endl;
    node->setPos(qreal(selfaddr / 1000),qreal(selfaddr % 10));
    sceneView->addItem(node);
//    qDebug()<<"add a node in topology***********"<<endl;
}

int GraphWidget::killOld_node(Node *node){
    Node *child;
    int nodes;
    int i;
    int node_size;
    Node *myParent;
    node_size = node->childrenAddr.size();
    for (i = 0; i < node_size; i ++){//kill all child nodes
        for (nodes = 0; nodes < node->childrenAddr.size(); nodes ++){
            child = findNode(node->childrenAddr.at(nodes));
            if(child->childrenAddr.size() > 0){
//                qDebug()<<"*****************************"<<child->childrenAddr.size()<<endl;
                killOld_node(child);
                myParent = findNode(child->parentAddr);
                myParent->childrenAddr.removeOne(child->selfAddr);
//                qDebug()<<" ##################$$$$$$$$$$$$$$$$$$$$$$$$$$$remove child_route node old link"<<endl;
                sceneView->removeItem(child->link);
//                qDebug()<<" ##################$$$$$$$$$$$$$$$$$$$$$$$$$$$remove child_route node old"<<endl;
                sceneView->removeItem(child);
//                qDebug()<<" ##################$$$$$$$$$$$$$$$$$$$$$$$$$$$remove child_route one node old"<<endl;
                nodeList.removeOne(child);
            }
            else{
                myParent = findNode(child->parentAddr);
                myParent->childrenAddr.removeOne(child->selfAddr);
//                qDebug()<<" ##################$$$$$$$$$$$$$$$$$$$$$$$$$$$remove child node old link"<<endl;
                sceneView->removeItem(child->link);
//                qDebug()<<" ##################$$$$$$$$$$$$$$$$$$$$$$$$$$$remove child node old"<<endl;
                sceneView->removeItem(child);
//                qDebug()<<" ##################$$$$$$$$$$$$$$$$$$$$$$$$$$$remove child node old"<<endl;
                nodeList.removeOne(child);
            }
            break;//node->childrenAddr.size()变小，需重新读取node->childrenAddr.size()
        }

    }

    myParent = findNode(node->parentAddr);
    myParent->childrenAddr.removeOne(node->selfAddr);
//    qDebug()<<" ##################$$$$$$$$$$$$$$$$$$$$$$$$$$$remove nodeold link"<<endl;
    sceneView->removeItem(node->link);
//    qDebug()<<" ##################$$$$$$$$$$$$$$$$$$$$$$$$$$$remove nodeold"<<endl;
    sceneView->removeItem(node);
//    qDebug()<<" ##################$$$$$$$$$$$$$$$$$$$$$$$$$$$remove onenode old"<<endl;
    nodeList.removeOne(node);
    return 0;
}
