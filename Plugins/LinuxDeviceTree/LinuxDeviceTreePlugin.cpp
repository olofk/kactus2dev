//-----------------------------------------------------------------------------
// File: LinuxDeviceTreePlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Copyright: Tampere University of Technology
// Author: Mikko Teuho
// Date: 08.05.2018
//
// Description:
// Generator plugin for Linux Device Tree.
//-----------------------------------------------------------------------------

#include "LinuxDeviceTreePlugin.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Design/Design.h>

#include <Plugins/common/HDLParser/HDLCommandLineParser.h>
#include <Plugins/PluginSystem/IPluginUtility.h>

#include <Plugins/LinuxDeviceTree/LinuxDeviceTreeDialog.h>
#include <Plugins/LinuxDeviceTree/LinuxDeviceTreeGenerator.h>

#include <editors/MemoryDesigner/ConnectivityGraphFactory.h>
#include <editors/MemoryDesigner/MasterSlavePathSearch.h>
#include <editors/MemoryDesigner/ConnectivityGraphFactory.h>

#include <library/LibraryInterface.h>

#include <QCoreApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::LinuxDeviceTreePlugin()
//-----------------------------------------------------------------------------
LinuxDeviceTreePlugin::LinuxDeviceTreePlugin()
{

}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::~LinuxDeviceTreePlugin()
//-----------------------------------------------------------------------------
LinuxDeviceTreePlugin::~LinuxDeviceTreePlugin()
{

}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getName()
//----------------------------------------------------------------------------
QString LinuxDeviceTreePlugin::getName() const
{
    return tr("Linux Device Tree Generator");
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getVersion()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreePlugin::getVersion() const
{
    return "0.1";
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getDescription()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreePlugin::getDescription() const
{
    return QString("Creates a Linux Device Tree from the selected document");
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getVendor()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreePlugin::getVendor() const
{
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getLicence()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreePlugin::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreePlugin::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* LinuxDeviceTreePlugin::getSettingsWidget()
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getSettingsModel()
//-----------------------------------------------------------------------------
PluginSettingsModel* LinuxDeviceTreePlugin::getSettingsModel()
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon LinuxDeviceTreePlugin::getIcon() const
{
    return QIcon(":icons/LinuxDeviceTreeGenerator.png");
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool LinuxDeviceTreePlugin::checkGeneratorSupport(QSharedPointer<Component const> component,
    QSharedPointer<Design const> /*design*/, QSharedPointer<DesignConfiguration const> /*designConfiguration*/) const
{
    return component != 0;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::runGenerator()
//-----------------------------------------------------------------------------
void LinuxDeviceTreePlugin::runGenerator(IPluginUtility* utility, QSharedPointer<Component> component,
    QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> /*designConfiguration*/)
{
    utility_ = utility;
    LibraryInterface* library = utility_->getLibraryInterface();
    utility->printInfo("Running Linux Device Tree Generator " + getVersion());

    QFileInfo targetInfo(library->getPath(component->getVlnv()));
    QString suggestedPath = library->getDirectoryPath(component->getVlnv());

    LinuxDeviceTreeDialog dialog(suggestedPath, component, design, library, utility_->getParentWidget());

    if (dialog.exec() == QDialog::Accepted)
    {
        QVector<QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> > acceptedContainers =
            dialog.getAcceptedContainers();
        if (!acceptedContainers.isEmpty())
        {
            generateDeviceTree(
                component, dialog.getSelectedView(), dialog.getOutputPath(), dialog.allowAddressBlocks(),
                dialog.getAcceptedContainers());

            if (dialog.saveFileToFileSet())
            {
                QString targetFileSet = dialog.getTargetFileSet();

                for (auto cpuContainer : acceptedContainers)
                {
                    QString relativePath =
                        General::getRelativePath(targetInfo.absoluteFilePath(), cpuContainer->filePath_);

                    saveFileToFileSet(component, targetFileSet, relativePath);

                    utility->getLibraryInterface()->writeModelToFile(component);
                    utility->printInfo("File " + relativePath + " stored in fileset " + targetFileSet +
                        " within component " + component->getVlnv().toString() + ".");
                }
            }
        }
    }
    else
    {
        utility->printInfo("Generation aborted.");
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::runGenerator()
//-----------------------------------------------------------------------------
void LinuxDeviceTreePlugin::runGenerator(IPluginUtility* utility, QSharedPointer<Component> component,
    QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConfiguration,
    QString const& viewName, QString const& outputDirectory)
{
    utility_ = utility;
    utility_->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));

    if (!component)
    {
        utility_->printError(tr("Invalid component given as a parameter."));
        return;
    }

    utility_->printInfo(tr("Running generation for %1 and view '%2'.").arg(component->getVlnv().toString(),
        viewName));

    QDir targetDirectory;
    if (!targetDirectory.mkpath(outputDirectory))
    {
        utility_->printError(tr("Could not create target directory: %1").arg(outputDirectory));
        return;
    }

    utility_->printInfo(tr("Target directory: %1").arg(outputDirectory));

    LibraryInterface* library = utility_->getLibraryInterface();
    ConnectivityGraphFactory graphFactory(library);
    MasterSlavePathSearch searchAlgorithm;

    QSharedPointer<ConnectivityGraph> graph = graphFactory.createConnectivityGraph(component, viewName);

    QVector < QSharedPointer<ConnectivityInterface> > masterRoots = searchAlgorithm.findMasterSlaveRoots(graph);

    QVector<QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> > cpuContainers =
        LinuxDeviceTreeCPUDetails::getCPUContainers(component->getVlnv().getName(), masterRoots, library);

    generateDeviceTree(component, viewName, outputDirectory, false, cpuContainers);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getOutputFormat()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreePlugin::getOutputFormat() const
{
    return QStringLiteral("dts");
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> LinuxDeviceTreePlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::createFileNamePath()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreePlugin::createFileNamePath(QString const& suggestedPath, QString const& componentName) const
{
    QString fileNamePath = suggestedPath;
    if (fileNamePath.endsWith(QLatin1Char('/')) == false)
    {
        fileNamePath.append(QLatin1Char('/'));
    }

    fileNamePath.append(componentName);
    fileNamePath.append(QStringLiteral(".dts"));

    return fileNamePath;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::generateDeviceTree()
//-----------------------------------------------------------------------------
void LinuxDeviceTreePlugin::generateDeviceTree(QSharedPointer<Component> component, QString const& activeView,
    QString const& folderPath, bool writeBlocks,
    QVector<QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer>> acceptedContainers)
{
    LinuxDeviceTreeGenerator generator(utility_->getLibraryInterface());
    if (generator.generate(component, activeView, writeBlocks, acceptedContainers, folderPath) == true)
    {
        utility_->printInfo("Generation successful.");
    }
    else
    {
        utility_->printError("Generation failed.");
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin:::onErrorReport()
//-----------------------------------------------------------------------------
void LinuxDeviceTreePlugin::onErrorReport(const QString& report)
{
    utility_->printError(report);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::saveFileToFileSet()
//-----------------------------------------------------------------------------
void LinuxDeviceTreePlugin::saveFileToFileSet(QSharedPointer<Component> component, QString const& fileSetName,
    QString const& filePath)
{
    QSharedPointer<FileSet> targetFileSet = getFileSet(component, fileSetName);
    if (targetFileSet)
    {
        QSettings settings;

        QSharedPointer<File> deviceTreeFile = targetFileSet->addFile(filePath, settings);
        deviceTreeFile->addFileType(QString("linuxDeviceTree"));
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getFileSet()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> LinuxDeviceTreePlugin::getFileSet(QSharedPointer<Component> component,
    QString const& fileSetName)
{
    foreach(QSharedPointer<FileSet> fileSet, *component->getFileSets())
    {
        if (fileSet->name().compare(fileSetName) == 0)
        {
            return fileSet;
        }
    }

    QSharedPointer<FileSet> newFileSet(new FileSet(fileSetName, QStringLiteral("generatedFiles")));
    component->getFileSets()->append(newFileSet);

    return newFileSet;
}
