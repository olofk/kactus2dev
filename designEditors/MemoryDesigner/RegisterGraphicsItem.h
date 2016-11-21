//-----------------------------------------------------------------------------
// File: RegisterGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 19.09.2016
//
// Description:
// Graphics item for visualizing register in the memory designer.
//-----------------------------------------------------------------------------

#ifndef REGISTERGRAPHICSITEM_H
#define REGISTERGRAPHICSITEM_H

#include <designEditors/MemoryDesigner/MemoryDesignerChildGraphicsItem.h>

#include <common/graphicsItems/GraphicsItemTypes.h>

class MemoryItem;
class MemoryDesignerGraphicsItem;

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Graphics item for visualizing register in the memory designer.
//-----------------------------------------------------------------------------
class RegisterGraphicsItem : public MemoryDesignerChildGraphicsItem
{

public:
    //! Type of this graphics item.
    enum { Type = GFX_TYPE_MEMORY_ITEM };

    /*!
     *  The constructor.
     *
     *      @param [in] registerItem        Memory item containing the register data.
     *      @param [in] isEmptyRegister     Boolean value to represent empty registers.
     *      @param [in] registerWidth       Width of the register graphics item.
     *      @param [in] parentItem          The parent memory graphics item.
     */
    RegisterGraphicsItem(QSharedPointer<MemoryItem> registerItem, bool isEmptyRegister, qreal registerWidth,
        MemoryDesignerGraphicsItem* parentItem);

	/*!
     *  The destructor.
     */
    virtual ~RegisterGraphicsItem();

    /*!
     *  Get the graphics type.
     */
    int type() const { return Type; }

private:
    // Disable copying.
    RegisterGraphicsItem(RegisterGraphicsItem const& rhs);
    RegisterGraphicsItem& operator=(RegisterGraphicsItem const& rhs);

    /*!
     *  Get the register end.
     *
     *      @param [in] registerItem    Memory item containing register data.
     *
     *      @return The end range of the register item.
     */
    quint64 getRegisterEnd(QSharedPointer<MemoryItem> registerItem) const;

    /*!
     *  Setup the label positions.
     */
    virtual void setLabelPositions();
};

//-----------------------------------------------------------------------------

#endif // ADDRESSBLOCKGRAPHICSITEM_H
