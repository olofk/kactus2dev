//-----------------------------------------------------------------------------
// File: TransactionalPortsEditorConstructor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2019
//
// Description:
// Constructs transactional ports editor items.
//-----------------------------------------------------------------------------

#include "TransactionalPortsEditorConstructor.h"

#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/ports/PortsView.h>
#include <editors/ComponentEditor/ports/TransactionalPortColumns.h>
#include <editors/ComponentEditor/ports/TransactionalPortsFilter.h>
#include <editors/ComponentEditor/ports/TransactionalPortsDelegate.h>
#include <editors/ComponentEditor/ports/TransactionalPortsModel.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

//-----------------------------------------------------------------------------
// Function: TransactionalPortsEditorConstructor::constructModel()
//-----------------------------------------------------------------------------
PortsModel* TransactionalPortsEditorConstructor::constructModel(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<PortsInterface> portsInterface, QSharedPointer<PortAbstractionInterface> signalInterface,
    QSortFilterProxyModel* filter, QObject* parent) const
{
    TransactionalPortsModel* transactionalModel =
        new TransactionalPortsModel(parameterFinder, portsInterface, signalInterface, filter, parent);

    return transactionalModel;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsEditorConstructor::constructFilter()
//-----------------------------------------------------------------------------
PortsFilter* TransactionalPortsEditorConstructor::constructFilter(QSharedPointer<PortsInterface> portsInterface,
    QObject* parent) const
{
    TransactionalPortsFilter* transactionalFilter = new TransactionalPortsFilter(portsInterface, parent);
    return transactionalFilter;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsEditorConstructor::constructView()
//-----------------------------------------------------------------------------
PortsView* TransactionalPortsEditorConstructor::constructView(QString const& defaultPath,
    BusInterfaceInterface* busInterface, QWidget* parent) const
{
    PortsView* view = new PortsView(TransactionalPortColumns::NAME, busInterface, parent);

    view->setDefaultImportExportPath(defaultPath);
    view->setAllowImportExport(true);
    view->setAlternatingRowColors(false);
    view->setSortingEnabled(true);
    view->setItemsDraggable(false);

    return view;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsEditorConstructor::constructDelegate()
//-----------------------------------------------------------------------------
PortsDelegate* TransactionalPortsEditorConstructor::constructDelegate(QSharedPointer<Component> component,
    ParameterCompleter* parameterCompleter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<PortValidator> portValidator, QObject* parent) const
{
    TransactionalPortsDelegate* transactionalDelegate = new TransactionalPortsDelegate(
        component, parameterCompleter, parameterFinder, portValidator->getTypeValidator(), parent);

    return transactionalDelegate;
}
