//-----------------------------------------------------------------------------
// File: MemoryDesignerDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.07.2016
//
// Description:
// Declares the memory design diagram class.
//-----------------------------------------------------------------------------

#ifndef MEMORYDESIGNERDIAGRAM_H
#define MEMORYDESIGNERDIAGRAM_H

#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <designEditors/MemoryDesigner/ConnectivityGraphFactory.h>

#include <QGraphicsScene>
#include <QSharedPointer>

class MemoryDesignDocument;
class GraphicsColumnLayout;
class Design;
class DesignConfiguration;
class Component;
class View;
class LibraryInterface;
class MemoryColumn;
class ConnectivityGraph;
class ConnectivityComponent;
class MemoryItem;
class MainMemoryGraphicsItem;
class MemoryConnectionItem;
class MemoryMapGraphicsItem;

//-----------------------------------------------------------------------------
//! Declares the memory design diagram class.
//-----------------------------------------------------------------------------
class MemoryDesignerDiagram : public QGraphicsScene
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] library     The library interface.
     *      @param [in] parent      The parent widget.
     */    
    MemoryDesignerDiagram(LibraryInterface* library, MemoryDesignDocument* parent = 0);

    /*!
     *  The destructor.
     */
    ~MemoryDesignerDiagram();

    /*!
     *  Set the filtering for chained address space memory connections.
     *
     *      @param [in] filterChains    Value for the chained address space memory connection filtering.
     */
    void setFilterAddressSpaceChains(bool filterChains);

    /*!
     *  Check whether the chained address space memory connections are filtered or not.
     *
     *      @return True, if the chained address space memory connections are filtered, false otherwise.
     */
    bool addressSpaceChainsAreFiltered() const;

    /*!
     *  Set a new value for filtering the address space segments.
     *
     *      @param [in] filterSegments  Value for filtering address space segments.
     */
    void setFilterAddressSpaceSegments(bool filterSegments);

    /*!
     *  Check if the address space segments are filtered.
     *
     *      @return True, if the address space segments are filtered, false otherwise.
     */
    bool addressSpaceSegmentsAreFiltered() const;

    /*!
     *  Set a new value for filtering the address blocks.
     *
     *      @param [in] filterBlocks    Value for filtering address blocks.
     */
    void setFilterAddressBlocks(bool filterBlocks);

    /*!
     *  Check if the address blocks are filtered.
     *
     *      @return True, if the address blocks are filtered, false otherwise.
     */
    bool addressBlocksAreFiltered() const;

    /*!
     *  Set a new value for filtering the address block registers.
     *
     *      @param [in] filterSegments  Value for filtering address block registers.
     */
    void setFilterAddressBlockRegisters(bool filterRegisters);

    /*!
     *  Check if the address block registers are filtered.
     *
     *      @return True, if the address block registers are filtered, false otherwise.
     */
    bool addressBlockRegistersAreFiltered() const;

    /*!
     *  Load a design using the selected view.
     *
     *      @param [in] component   Component containing the view.
     *      @param [in] viewName    The name of the selected view.
     */
    bool loadDesignFromCurrentView(QSharedPointer<const Component> component, QString const& viewName);

    /*!
     *  Clear the graphics scene.
     */
    virtual void clearScene();

    /*!
     *  Handle the display of the diagram.
     */
    void onShow();

    /*!
     *  Handle the vertical scroll.
     *
     *      @param [in] y   The amount of vertical scrolling.
     */
    void onVerticalScroll(qreal y);

protected:

    /*!
     *  Handle the mouse wheel event.
     *
     *      @param [in] event   The wheel event.
     */
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event);

    /*!
     *  Draw the diagram background.
     *
     *      @param [in] painter     The used painter.
     *      @param [in] rect        The containing rectangle of the diagram.
     */
    virtual void drawBackground(QPainter *painter, const QRectF &rect);

private:
    // Disable copying.
    MemoryDesignerDiagram(MemoryDesignerDiagram const& rhs);
    MemoryDesignerDiagram& operator=(MemoryDesignerDiagram const& rhs);

    /*!
     *  Clear the layout.
     */
    void clearLayout();

    /*!
     *  Create the connection graph the selected view.
     *
     *      @param [in] component   The component containing the selected view.
     *      @param [in] viewName    Name of the selected view.
     *
     *      @return True, if the connection graph could be created, false otherwise.
     */
    bool createConnectionGraph(QSharedPointer<const Component> component, QString const& viewName);

    /*!
     *  Get the design referenced by the selected view.
     *
     *      @param [in] component   Component containing the selected view.
     *      @param [in] viewName    The name of the selected view.
     *
     *      @return The design referenced by the selected view.
     */
    QSharedPointer<Design> getContainingDesign(QSharedPointer<const Component> component, QString const& viewName)
        const;

    /*!
     *  Get the design configuration referenced by the selected view.
     *
     *      @param [in] component   Component containing the selected view.
     *      @param [in] viewName    The name of the selected view.
     *
     *      @return The design configuration referenced by the selected view.
     */
    QSharedPointer<const DesignConfiguration> getContainingDesignConfiguration(
        QSharedPointer<const Component> component, QString const& viewName) const;

    /*!
     *  Create the initial memory columns.
     */
    void createInitialColumns();

    /*!
     *  Create an address space column.
     *
     *      @return The created address space column.
     */
    MemoryColumn* createAddressSpaceColumn();

    /*!
     *  Create the initial memory columns.
     */
    void createMemoryColumns();

    /*!
     *  Create a new memory overlap column.
     *
     *      @return The created memory overlap column.
     */
    MemoryColumn* createMemoryOverlapColumn();

    /*!
     *  Create the memory items found in the design.
     */
    void createMemoryItems();

    /*!
     *  Create an address space item.
     *
     *      @param [in] spaceItem           Memory item containing address space data.
     *      @param [in] containingInstance  The component instance containing the selected address space.
     *      @param [in] containingColumn    The address space column.
     */
    void createAddressSpaceItem(QSharedPointer<MemoryItem> spaceItem,
        QSharedPointer<ConnectivityComponent> containingInstance, MemoryColumn* containingColumn);

    /*!
     *  Create a memory map item.
     *
     *      @param [in] mapItem             Memory item containing memory map data.
     *      @param [in] containingInstance  The component instance containing the selected memory map.
     *      @param [in] containingColumn    The memory map column.
     */
    void createMemoryMapItem(QSharedPointer<MemoryItem> mapItem,
        QSharedPointer<ConnectivityComponent> containingInstance, MemoryColumn* containingColumn);

    /*!
     *  Create the memory connections.
     */
    void createMemoryConnections();

    /*!
     *  Get the selected memory column.
     *
     *      @param [in] columnName  Name of the selected memory column.
     *
     *      @return The found memory column.
     */
    MemoryColumn* findColumnByName(QString const& columnName) const;

    /*!
     *  Create a memory connection between an address space and a memory map.
     *
     *      @param [in] connectionPath      The path of the memory connection.
     *      @param [in] placedMapItems      A list of placed memory map items.
     *      @param [in] memoryMapColumn     The column containing the memory maps.
     *      @param [in] spaceYPlacement     The Y placement of the next address space item.
     *      @param [in] placedSpaceItems    A list of placed address space items.
     *      @param [in] spaceColumn         The column containing the address spaces.
     */
    void createConnection(QVector<QSharedPointer<ConnectivityInterface> > connectionPath,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn* memoryMapColumn,
        int& spaceYPlacement, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        MemoryColumn* spaceColumn);

    /*!
     *  Check and reposition a memory map to a memory map overlap column if needed.
     *
     *      @param [in] placedMaps              A list of placed memory map items.
     *      @param [in] memoryItem              The selected memory map item.
     *      @param [in] originalColumn          The original column of the memory map item.
     *      @param [in] startAddress            The start address of the connection.
     *      @param [in] endAddress              The end address of the connection.
     *      @param [in] connectionStartItem     The start item of the memory map connection.
     */
    void checkMemoryMapRepositionToOverlapColumn(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps,
        MainMemoryGraphicsItem* memoryItem, MemoryColumn* originalColumn, QString const& startAddress,
        QString const& endAddress, MainMemoryGraphicsItem* connectionStartItem);

    /*!
     *  Check if a selected memory map item overlaps another memory map item within the selected column.
     *
     *      @param [in] memoryColumn            The selected memory map column.
     *      @param [in] memoryItem              The selected memory map graphics item.
     *      @param [in] mapBaseAddress          Base address of the memory map.
     *      @param [in] mapLastAddress          Last address of the memory map.
     *      @param [in] memoryItemRect          Graphics rectangle of the memory map item.
     *      @param [in] memoryPenWidth          Pen Width of the memory map item.
     *      @param [in] placedMaps              List of memory maps that have already been placed.
     *      @param [in] connectedSpaceItems     List of the address space graphics items connected to the selected
     *                                          memory map graphics item.
     *
     *      @return True, if the memory map overlaps another memory map, false otherwise.
     */
    bool memoryMapOverlapsInColumn(MemoryColumn* memoryColumn, MainMemoryGraphicsItem* memoryItem,
        quint64 mapBaseAddress, quint64 mapLastAddress, QRectF memoryItemRect, int memoryPenWidth,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps,
        QVector<MainMemoryGraphicsItem*> connectedSpaceItems) const;

    /*!
     *  Check if a memory map overlaps another memory map in the same column.
     *
     *      @param [in] mapBaseAddress          Base address of the selected memory map.
     *      @param [in] lastAddress             Last address of the selected memory map.
     *      @param [in] selectedMapRect         Bounding rectangle of the selected memory map item.
     *      @param [in] selectedMapPenWidth     Line width of the selected memory map item.
     *      @param [in] comparisonMemoryItem    The compared memory map item.
     *
     *      @return True, if the selected memory map overlaps another memory map, false otherwise.
     */
    bool memoryMapOverlapsAnotherMemoryMap(quint64 mapBaseAddress, quint64 mapLastAddress, QRectF selectedMapRect,
        int selectedMapPenWidth, MainMemoryGraphicsItem* comparisonMemoryItem) const;

    /*!
     *  Reposition the selected memory map item.
     *
     *      @param [in] placedMapItems          List of the placed memory map items.
     *      @param [in] placedSpaceItems        List of the placed address space items.
     *      @param [in] startConnectionItem     The connection start item.
     *      @param [in] addressSpaceColumn      Column containing the address spaces.
     *      @param [in] endConnectionItem       The connection end item.
     *      @param [in] memoryMapYTransfer      Y transfer of the memory map.
     */
    void repositionMemoryMap(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        MainMemoryGraphicsItem* startConnectionItem, MemoryColumn* addressSpaceColumn,
        MainMemoryGraphicsItem* endConnectionItem, int memoryMapYTransfer);

    /*!
     *  Reposition an address space item to match a selected memory map.
     *
     *      @param [in] placedSpaceItems    List of the placed address space items.
     *      @param [in] startItem           The connection start item.
     *      @param [in] endItem             The connection end item.
     *      @param [in] spaceColumn         Column containing the address spaces.
     *      @param [in] memoryMapYTransfer  Y transfer of the memory map.
     */
    void repositionSpaceItemToMemoryMap(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        MainMemoryGraphicsItem* startItem, MainMemoryGraphicsItem* endItem, MemoryColumn* spaceColumn,
        int memoryMapYTransfer);

    /*!
     *  Redraw the memory connections.
     *
     *      @param [in] placedSpaceItems    List of the space items that have been placed in the diagram.
     */
    void reDrawConnections(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems);

    /*!
     *  Move an address space item.
     *
     *      @param [in] spaceItem           The selected address space item.
     *      @param [in] spaceColumn         Column containing the address spaces.
     *      @param [in] spaceYPlacement     Y placement of the address space item.
     */
    void moveAddressSpaceItem(MainMemoryGraphicsItem* spaceItem, MemoryColumn* spaceColumn, int& spaceYPlacement);

    /*!
     *  Place the address space item to another address space column.
     *
     *      @param [in] spaceItem       The selected address space item.
     *      @param [in] originalColumn  The original column of the selected address space item.
     *      @param [in] targetItem      The other item of the memory connection.
     *      @param [in] yTransfer       Y transfer of the address space item.
     */
    void placeSpaceItemToOtherColumn(MainMemoryGraphicsItem* spaceItem, MemoryColumn* originalColumn,
        MainMemoryGraphicsItem* targetItem, int yTransfer);

    /*!
     *  Extend a memory item.
     *
     *      @param [in] memoryGraphicsItem  The selected memory graphics item.
     *      @param [in] connectionItem      The connection of the selected memory graphics item.
     *      @param [in] spaceYplacement     Y placement of the address space item.
     */
    void extendMemoryItem(MainMemoryGraphicsItem* memoryGraphicsItem, MemoryConnectionItem* connectionItem,
        int& spaceYplacement);

    /*!
     *  Move the unconnected address space items to the bottom of the address space column.
     *
     *      @param [in] placedSpaceItems    List of the placed address space items.
     *      @param [in] spaceYPlacement     Y placement of the last address space item.
     *      @param [in] spaceColumn         Column containing the address space items.
     */
    void moveUnconnectedAddressSpaces(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        int& spaceYPlacement, MemoryColumn* spaceColumn);

    /*!
     *  Move the unconnected memory map items to the bottom of the memory map column.
     *
     *      @param [in] placedMapItems      List of the placed memory map items.
     *      @param [in] memoryMapColumn     Column containing the memory map items.
     */
    void moveUnconnectedMemoryMaps(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        MemoryColumn* memoryMapColumn);

    /*!
     *  Create markers for the overlapping connections.
     *
     *      @param [in] placedSpaceItems    List of the placed address space items.
     */
    void createOverlappingConnectionMarkers(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems);

    /*!
     *  Check if an items bounding rectangle collides with another item.
     *
     *      @param [in] firstRectangle      Bounding rectangle of the selected item.
     *      @param [in] firstLineWidth      Line width of the selected item.
     *      @param [in] secondRectangle     Bounding rectangle of the comparison item.
     *      @param [in] secondLineWidth     Line width of the comparison item.
     */
    bool itemCollidesWithAnotherItem(QRectF firstRectangle, int firstLineWidth, QRectF secondRectangle,
        int secondLineWidth) const;

    /*!
     *  Compress the graphics items.
     *
     *      @param [in] placedSpaceItems    A list of the placed address space items.
     *      @param [in] spaceYPlacement     The last y-coordinate of the address space items.
     *      @param [in] spaceColumn   [Description].
     */
    void compressGraphicsItems(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        int& spaceYPlacement, MemoryColumn* spaceColumn);

    /*!
     *  Reposition compressed memory maps to previous columns.
     *
     *      @param [in] placedMapItems      A list of the placed memory map items.
     *      @param [in] memoryMapColumn     The main memory map column.
     *      @param [in] spaceColumn         The address space column.
     */
    void repositionCompressedMemoryMaps(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        MemoryColumn* memoryMapColumn, MemoryColumn* spaceColumn);

    /*!
     *  Get the specified columns.
     *
     *      @param [in] columnSpecification     Identifier for the wanted columns.
     *
     *      @return A vector containing the specified columns.
     */
    QVector<MemoryColumn*> getSpecifiedColumns(QString const& columnSpecification) const;

    /*!
     *  Get the main graphics item corresponding to the selected connection interface.
     *
     *      @param [in] connectionInterface     The selected connection interface.
     *      @param [in] columnType              The column type of the main graphics item.
     *
     *      @return The found main graphics item.
     */
    MainMemoryGraphicsItem* getMainGraphicsItem(QSharedPointer<ConnectivityInterface> connectionInterface,
        QString columnType) const;

    /*!
     *  Create a connection between two address space items.
     *
     *      @param [in] connectionStartItem     Start item of the connection.
     *      @param [in] connectionMiddleItem    The second address space item.
     *      @param [in] newSpaceInterface       Interface of the second address space item.
     *      @param [in] spaceColumn             The address space column.
     *      @param [in] placedSpaceItems        List of the placed address space items.
     *      @param [in] spaceItemChain          Connection chain of address space items.
     *      @param [in] spaceYPlacement         Y coordinate of the address space placement.
     *
     *      @return The created memory connection item.
     */
    MemoryConnectionItem* createSpaceConnection(MainMemoryGraphicsItem* connectionStartItem,
        MainMemoryGraphicsItem* connectionMiddleItem, QSharedPointer<ConnectivityInterface> newSpaceInterface,
        MemoryColumn* spaceColumn, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        QVector<MainMemoryGraphicsItem*> spaceItemChain, int& spaceYPlacement);

    /*!
     *  Change the column of the master address space item.
     *
     *      @param [in] masterSpaceItem     The selected master address space item.
     *      @param [in] spaceItemY          Y-coordinate of the address space item.
     *      @param [in] originalColumn      Original column of the master address space item.
     *      @param [in] spaceItemChain      Connection chain of address space items.
     */
    void changeMasterAddressSpaceColumn(MainMemoryGraphicsItem* masterSpaceItem, qreal spaceItemY,
        GraphicsColumn* originalColumn, QVector<MainMemoryGraphicsItem*> spaceItemChain);

    /*!
     *  Check if an address space item collides with an other address space item.
     *
     *      @param [in] spaceItem   The selected address space item.
     *
     *      @return True, if the address space item collides with an other address space item, false otherwise.
     */
    bool spaceItemCollidesWithOtherSpaceItems(MainMemoryGraphicsItem* spaceItem) const;

    /*!
     *  Change the column of a colliding master address space item.
     *
     *      @param [in] currentColumn   The current column of the master address space item.
     *      @param [in] spaceItemChain  Connection chain of address space items.
     */
    void changeCollidingMasterAddressSpaceColumn(GraphicsColumn* currentColumn,
        QVector<MainMemoryGraphicsItem*> spaceItemChain);

    /*!
     *  Get an address space connection from placed start memory item and middle memory item.
     *
     *      @param [in] connectionStartItem     The memory connection start memory item.
     *      @param [in] connectionMiddleItem    The memory connection middle memory item.
     *
     *      @return The placed memory connection between an already placed start memory item and middle memory item.
     */
    MemoryConnectionItem* getAddressSpaceChainConnection(MainMemoryGraphicsItem* connectionStartItem,
        MainMemoryGraphicsItem* connectionMiddleItem) const;

    /*!
     *  Set the connection width for the memory connection items within a memory connection chain.
     *
     *      @param [in] connectionChain     Connection chain of address space items.
     */
    void setHeightForConnectionChain(QVector<MemoryConnectionItem*> connectionChain);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Document containing the memory design diagram.
    MemoryDesignDocument* parentDocument_;

    //! Layout of the memory design diagram.
    QSharedPointer<GraphicsColumnLayout> layout_;

    //! The used library interface.
    LibraryInterface* libraryHandler_;

    //! Factory for creating the connectivity graph.
    ConnectivityGraphFactory instanceLocator_;

    //! Width of the memory columns.
    static const int COLUMNWIDTH = 619;

    //! The created connection graph.
    QSharedPointer<ConnectivityGraph> connectionGraph_;

    //! Value for filtering the chained address space memory connections.
    bool filterAddressSpaceChains_;

    //! Value for filtering address space segments.
    bool filterAddressSpaceSegments_;

    //! Value for filtering memory map address blocks.
    bool filterAddressBlocks_;

    //! Value for filtering address block registers.
    bool filterRegisters_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYDESIGNERDIAGRAM_H
