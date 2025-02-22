/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: expandableitem.cpp
 *		Project: Kactus 2
 */

#include "expandableitem.h"
#include "graphicsexpandcollapseitem.h"

//-----------------------------------------------------------------------------
// Function: ExpandableItem::ExpandableItem()
//-----------------------------------------------------------------------------
ExpandableItem::ExpandableItem(QGraphicsItem* parent):
VisualizerItem(parent),
expandCollapseItem_(new GraphicsExpandCollapseItem(this)),
expansionArrow_(new QGraphicsPixmapItem(this))
{
	connect(expandCollapseItem_, SIGNAL(stateChanged(bool)),
        this, SLOT(onExpandStateChange(bool)), Qt::UniqueConnection);

    setShowExpandableItem(false);

    expandCollapseItem_->setRect(-GraphicsExpandCollapseItem::SIDE, 0,
        GraphicsExpandCollapseItem::SIDE, VisualizerItem::DEFAULT_HEIGHT);

    updateExpandArrow(false);

    expansionArrow_->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    expansionArrow_->setZValue(1);
    
    // Set the position for the expand/collapse item with the icon.
    expansionArrow_->setPos(-GraphicsExpandCollapseItem::SIDE, 
        GraphicsExpandCollapseItem::SIDE / 2 + VisualizerItem::CORNER_INDENTATION);
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::isExpanded()
//-----------------------------------------------------------------------------
bool ExpandableItem::isExpanded() const
{
	return expandCollapseItem_->isExpanded();
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::resizeToContent()
//-----------------------------------------------------------------------------
void ExpandableItem::resizeToContent()
{
    updateRectangle();
	repositionLabels();
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::onExpandStateChange()
//-----------------------------------------------------------------------------
void ExpandableItem::onExpandStateChange(bool expanded)
{
    // if there are children	
    for (QGraphicsItem* child : childItems())
    {
        Q_ASSERT(child);

        // if the item is visualizer item
        VisualizerItem* childItem = dynamic_cast<VisualizerItem*>(child);
        if (childItem)
        {
            childItem->setVisible(expanded && childItem->isPresent());
        }
    }

    updateExpandArrow(expanded);

    updateRectangle();

    emit expandStateChanged();
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::setShowExpandableItem()
//-----------------------------------------------------------------------------
void ExpandableItem::setShowExpandableItem(bool show)
{
    expansionArrow_->setVisible(show);
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::setDefaultBrush()
//-----------------------------------------------------------------------------
void ExpandableItem::setDefaultBrush(QBrush brush)
{
    VisualizerItem::setDefaultBrush(brush);
    setExpansionBrush(brush);
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::setExpansionBrush()
//-----------------------------------------------------------------------------
void ExpandableItem::setExpansionBrush(QBrush const& brush)
{
    expandCollapseItem_->setBrush(brush);
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::setExpansionPen()
//-----------------------------------------------------------------------------
void ExpandableItem::setExpansionPen(QPen const& pen)
{
    expandCollapseItem_->setPen(pen);
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::setExpansionRectVisible()
//-----------------------------------------------------------------------------
void ExpandableItem::setExpansionRectVisible(bool visible)
{
    expandCollapseItem_->setVisible(visible);
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::updateExpandArrow()
//-----------------------------------------------------------------------------
void ExpandableItem::updateExpandArrow(bool expanded)
{
    QString iconName;
    if (expanded)
    {
        iconName = QStringLiteral(":/icons/common/graphics/triangle_arrow_down.png");
    }
    else
    {
        iconName = QStringLiteral(":/icons/common/graphics/triangle_arrow_right.png");
    }

    QPixmap pic(iconName);
    QPixmap scaledPic = pic.scaled(GraphicsExpandCollapseItem::SIDE, GraphicsExpandCollapseItem::SIDE,
        Qt::KeepAspectRatio);
    expansionArrow_->setPixmap(scaledPic);
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::updateHeight()
//-----------------------------------------------------------------------------
void ExpandableItem::updateRectangle()
{
    // the rectangle that contains this item and children
    QRectF totalRect = itemTotalRect();

    // the rectangle is on the left side of the parent and children
    expandCollapseItem_->setRect(-GraphicsExpandCollapseItem::SIDE, 0, 
        GraphicsExpandCollapseItem::SIDE, totalRect.height());
}
