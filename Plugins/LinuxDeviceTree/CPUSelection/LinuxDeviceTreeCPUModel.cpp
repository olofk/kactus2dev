//-----------------------------------------------------------------------------
// File: LinuxDeviceTreeCPUModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.10.2021
//
// Description:
// The model to manage the Linux Device Tree CPU details.
//-----------------------------------------------------------------------------

#include "LinuxDeviceTreeCPUModel.h"
#include "LinuxDeviceTreeCPUColumns.h"

#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/Component.h>

#include <common/KactusColors.h>

#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/ConnectivityComponent.h>

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUModel::LinuxDeviceTreeCPUModel()
//-----------------------------------------------------------------------------
LinuxDeviceTreeCPUModel::LinuxDeviceTreeCPUModel(QObject *parent):
QAbstractTableModel(parent),
cpus_()
{
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUModel::setupCPUDetails()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeCPUModel::setupCPUDetails(
    QVector<QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer>> cpuDetails)
{
    beginResetModel();

    cpus_.clear();
    cpus_ = cpuDetails;

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUModel::getCPUDetails()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> > LinuxDeviceTreeCPUModel::getCPUDetails() const
{
    QVector<QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> > acceptedContainers;
    for (auto cpuContainer : cpus_)
    {
        if (cpuContainer->createFile_)
        {
            acceptedContainers.append(cpuContainer);
        }
    }

    return acceptedContainers;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUModel::rowCount()
//-----------------------------------------------------------------------------
int LinuxDeviceTreeCPUModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return cpus_.count();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUModel::columnCount()
//-----------------------------------------------------------------------------
int LinuxDeviceTreeCPUModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return LinuxDeviceTreeCPUColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags LinuxDeviceTreeCPUModel::flags(QModelIndex const& index) const
{
	if (!index.isValid() || index.column() == LinuxDeviceTreeCPUColumns::CPUS ||
        index.column() == LinuxDeviceTreeCPUColumns::CONTAINING_COMPONENT)
    {
		return Qt::NoItemFlags;
	}

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == LinuxDeviceTreeCPUColumns::CREATEDEVICETREE)
    {
        flags |= Qt::ItemIsUserCheckable;
    }
    else
    {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUModel::headerData()
//-----------------------------------------------------------------------------
QVariant LinuxDeviceTreeCPUModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}

	if (role == Qt::DisplayRole)
    {
        if (section == LinuxDeviceTreeCPUColumns::CREATEDEVICETREE)
        {
            return "Create\nDTS";
        }
        else if (section == LinuxDeviceTreeCPUColumns::FILE_NAME)
        {
            return "File name";
        }
        else if (section == LinuxDeviceTreeCPUColumns::CPUS)
        {
            return "CPUs";
        }
        else if (section == LinuxDeviceTreeCPUColumns::CONTAINING_COMPONENT)
        {
            return "Component";
        }
	}
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUModel::data()
//-----------------------------------------------------------------------------
QVariant LinuxDeviceTreeCPUModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= cpus_.size())
    {
		return QVariant();
	}

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return valueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        return tooltipForIndex(index);
    }
	else if (role == Qt::ForegroundRole)
    {
        if (index.column() == LinuxDeviceTreeCPUColumns::CPUS ||
            index.column() == LinuxDeviceTreeCPUColumns::CONTAINING_COMPONENT)
        {
            return KactusColors::DISABLED_TEXT;
        }
        else
        {
            return KactusColors::REGULAR_TEXT;
        }
    }
    else if (Qt::CheckStateRole == role)
    {
        if (index.column() == LinuxDeviceTreeCPUColumns::CREATEDEVICETREE)
        {
            QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> indexedCPU = cpus_.at(index.row());
            if (indexedCPU->createFile_)
            {
                return Qt::Checked;
            }
            else
            {
                return Qt::Unchecked;
            }
        }
        else
        {
            return QVariant();
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUModel::setData()
//-----------------------------------------------------------------------------
bool LinuxDeviceTreeCPUModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= cpus_.size())
    {
		return false;
	}

    QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> indexedCPU = cpus_.at(index.row());
    if (Qt::EditRole == role)
    {
        if (index.column() == LinuxDeviceTreeCPUColumns::FILE_NAME)
        {
            indexedCPU->fileName_ = value.toString();
        }

        emit dataChanged(index, index);
        return true;
    }
    else if (role == Qt::CheckStateRole)
    {
        if (index.column() == LinuxDeviceTreeCPUColumns::CREATEDEVICETREE)
        {
            indexedCPU->createFile_ = value.toBool();
        }

        emit dataChanged(index, index);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant LinuxDeviceTreeCPUModel::valueForIndex(QModelIndex const& index) const
{
    QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> indexedContainer = cpus_.at(index.row());

    if (index.column() == LinuxDeviceTreeCPUColumns::FILE_NAME)
    {
        return indexedContainer->fileName_;
    }
    else if (index.column() == LinuxDeviceTreeCPUColumns::CPUS)
    {
        QString cpuText = "";
        for (auto currentCPU : indexedContainer->interfacedCPUs_)
        {
            cpuText += currentCPU->name();

            if (currentCPU != indexedContainer->interfacedCPUs_.last())
            {
                cpuText += ", ";
            }
        }

        return cpuText;
    }
    else if (index.column() == LinuxDeviceTreeCPUColumns::CONTAINING_COMPONENT)
    {
        if (indexedContainer->rootInterfaces_.size() > 0)
        {
            return indexedContainer->rootInterfaces_.first()->getInstance()->getVlnv();
        }
        else
        {
            return "";
        }
    }
   
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUModel::tooltipForIndex()
//-----------------------------------------------------------------------------
QVariant LinuxDeviceTreeCPUModel::tooltipForIndex(QModelIndex const& index) const
{
    if (index.column() == LinuxDeviceTreeCPUColumns::CPUS)
    {
        QString cpuTooltip = "CPUs:\n";

        QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> indexedContainer = cpus_.at(index.row());
        for (auto currentCPU : indexedContainer->interfacedCPUs_)
        {
            cpuTooltip += currentCPU->name();

            if (currentCPU != indexedContainer->interfacedCPUs_.last())
            {
                cpuTooltip += "\n";
            }
        }

        return cpuTooltip;
    }
    else
    {
        return valueForIndex(index);
    }

    return QVariant();
}
