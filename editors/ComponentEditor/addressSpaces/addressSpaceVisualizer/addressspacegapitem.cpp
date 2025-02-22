//-----------------------------------------------------------------------------
// File: addressspacegapitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 04.01.2013
//
// Description:
// The graphical item that represents a gap in between address space objects.
//-----------------------------------------------------------------------------

#include "addressspacegapitem.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QBrush>

//-----------------------------------------------------------------------------
// Function: AddressSpaceGapItem()
//-----------------------------------------------------------------------------
AddressSpaceGapItem::AddressSpaceGapItem(VisualizerItem::LabelLayout addrPos,
                                         QString const& addressSpaceWidth,
                                         QSharedPointer<ExpressionParser> expressionParser,
										 QGraphicsItem* parent):
AddressSpaceVisualizationItem(addressSpaceWidth, expressionParser, parent),
start_(0),
end_(0),
addrPosition_(addrPos) 
{
    setDefaultBrush(QBrush(Qt::white));

    setLayoutType(addrPos);
	setName("...");
    setToolTip("This memory block is unassigned.");
}

//-----------------------------------------------------------------------------
// Function: refresh()
//-----------------------------------------------------------------------------
void AddressSpaceGapItem::refresh() 
{
    if (addrPosition_ == AddressSpaceGapItem::ALIGN_LEFT)
    {
        setTopLabelText(start_);
        if (end_ != start_)
        {
            setBottomLabelText(end_);
        }
        else
        {
            VisualizerItem::setBottomLabelText("");
        }
    }

    else
    {
        setRightTopCorner(start_);
        if (end_ != start_)
        {
            setRightBottomCorner(end_);
        }
        else
        {
            VisualizerItem::setBottomLabelText("");
        }
    }

    setOverlappingTop(start_);
    setOverlappingBottom(end_);

    repositionLabels();
}

//-----------------------------------------------------------------------------
// Function: getOffset()
//-----------------------------------------------------------------------------
quint64 AddressSpaceGapItem::getOffset() const
{
	return start_;
}

//-----------------------------------------------------------------------------
// Function: getLastAddress()
//-----------------------------------------------------------------------------
quint64 AddressSpaceGapItem::getLastAddress() const
{
	return end_;
}

//-----------------------------------------------------------------------------
// Function: setStartAddress()
//-----------------------------------------------------------------------------
void AddressSpaceGapItem::setStartAddress( quint64 address, bool contains /*= true*/ )
{
	if (contains)
    {
		start_ = address;
	}
	else
    {
		start_ = ++address;
	}
	refresh();
}

//-----------------------------------------------------------------------------
// Function: setEndAddress()
//-----------------------------------------------------------------------------
void AddressSpaceGapItem::setEndAddress( quint64 address, bool contains /*= true*/ )
{
	if (contains)
    {
		end_ = address;
	}
	else
    {
		end_ = --address;
	}
	refresh();
}

//-----------------------------------------------------------------------------
// Function: setOverlappingTop()
//-----------------------------------------------------------------------------
void AddressSpaceGapItem::setOverlappingTop(quint64 const& address)
{
    firstFreeAddress_ = address;

    if (addrPosition_ == AddressSpaceGapItem::ALIGN_LEFT)
    {
        setTopLabelText(firstFreeAddress_);

        if (firstFreeAddress_ == lastFreeAddress_)
        {
            VisualizerItem::setBottomLabelText("");
        }
        else
        {
            setBottomLabelText(lastFreeAddress_);
        }
    }

    else //if (addrPosition == AddressSpaceGapItem::ALIGN_RIGHT)
    {
        setRightTopCorner(firstFreeAddress_);

        if (firstFreeAddress_ == lastFreeAddress_)
        {        
            VisualizerItem::setBottomLabelText("");
        }
        else
        {
            setRightBottomCorner(lastFreeAddress_);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: setOverlappingBottom()
//-----------------------------------------------------------------------------
void AddressSpaceGapItem::setOverlappingBottom(quint64 const& address)
{
    lastFreeAddress_ = address;

    if (addrPosition_ == AddressSpaceGapItem::ALIGN_LEFT)
    {
        if (firstFreeAddress_ == lastFreeAddress_)
        {
            VisualizerItem::setBottomLabelText("");
        }
        else
        {
            setBottomLabelText(lastFreeAddress_);
        }
    }

    else //if (addrPosition == AddressSpaceGapItem::ALIGN_RIGHT)
    {  
        if (firstFreeAddress_ == lastFreeAddress_)
        {
            VisualizerItem::setBottomLabelText("");
        }
        else
        {
            setRightBottomCorner(lastFreeAddress_);
        }
    }
}