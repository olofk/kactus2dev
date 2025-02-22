//-----------------------------------------------------------------------------
// File: componenteditorfileitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 16.05.2012
//
// Description:
// The item for a single file in component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorfileitem.h"

#include <common/utils.h>

#include <editors/ComponentEditor/fileSet/file/fileeditor.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <library/LibraryInterface.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Component/validators/FileValidator.h>

#include <QApplication>
#include <QDesktopServices>
#include <QFileInfo>
#include <QFileDialog>
#include <QFont>
#include <QProcess>
#include <QSettings>
#include <QStringList>
#include <QUrl>

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::ComponentEditorFileItem()
//-----------------------------------------------------------------------------
ComponentEditorFileItem::ComponentEditorFileItem(QSharedPointer<File> file,
    QSharedPointer<QList<QSharedPointer<File>>> availableFiles, FileInterface* fileInterface,
    ComponentEditorTreeModel* model, LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<FileValidator> validator, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ReferenceCounter> referenceCounter,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
file_(file),
availableFiles_(availableFiles),
fileInterface_(fileInterface),
validator_(validator),
editAction_(new QAction(tr("Edit"), this)),
editWithAction_(new QAction(tr("Edit/Run with..."), this)),
runAction_(new QAction(tr("Run"), this)),
openContainingFolderAction_(new QAction(tr("Open Containing Folder"), this) ),
expressionParser_(expressionParser)
{
    setParameterFinder(parameterFinder);
    setReferenceCounter(referenceCounter);

    connect(editAction_, SIGNAL(triggered(bool)), this, SLOT(openItem()), Qt::UniqueConnection);
    connect(editWithAction_, SIGNAL(triggered(bool)), this, SLOT(openWith()), Qt::UniqueConnection);
    connect(runAction_, SIGNAL(triggered(bool)), this, SLOT(run()), Qt::UniqueConnection);
    connect(openContainingFolderAction_, SIGNAL(triggered(bool)),
        this, SLOT(onOpenContainingFolder()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::~ComponentEditorFileItem()
//-----------------------------------------------------------------------------
ComponentEditorFileItem::~ComponentEditorFileItem()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorFileItem::text() const
{
    return file_->getFileName();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorFileItem::isValid() const
{
    if (!validator_->validate(file_))
    {
        return false;
    }

	QString filePath = file_->name();

    return isValidPath(filePath) || isValidURI(filePath);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorFileItem::editor()
{
	if (!editor_)
    {
        editor_ = new FileEditor(libHandler_, component_, file_->name().toStdString(), parameterFinder_,
            expressionParser_, fileInterface_, availableFiles_);
        editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
        connect(editor_, SIGNAL(editFile()), this, SLOT(openItem()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(runFile()), this, SLOT(run()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(openContainingFolder()), this, SLOT(onOpenContainingFolder()), Qt::UniqueConnection);

        connect(this, SIGNAL(fileRenamed(std::string const&, std::string const&)),
            editor_, SLOT(fileRenamed(std::string const&, std::string const&)), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorFileItem::getTooltip() const
{
	return tr("Specifies a single file associated with the file set");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::canBeOpened()
//-----------------------------------------------------------------------------
bool ComponentEditorFileItem::canBeOpened() const
{
	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::openItem()
//-----------------------------------------------------------------------------
void ComponentEditorFileItem::openItem()
{
    if (useKactusCSourceEditor())
    {
        emit openCSource(fileAbsolutePath(), component_);
    }
    else
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileAbsolutePath()));
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::openWith()
//-----------------------------------------------------------------------------
void ComponentEditorFileItem::openWith()
{
    QString applicationPath = QFileDialog::getOpenFileName(0, tr("Select Application"));

    if (QFileInfo(applicationPath).isExecutable())
    {
        runInApplication(applicationPath);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::run()
//-----------------------------------------------------------------------------
void ComponentEditorFileItem::run()
{
    if (runExecutableSet())
    {
        runInApplication(executablePath()); 
    }
    else
    {
        emit errorMessage(tr("No executable set for file type(s)."));
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::actions()
//-----------------------------------------------------------------------------
QList<QAction*> ComponentEditorFileItem::actions() const
{
    QList<QAction*> actionList;
    actionList.append(editAction_);
    actionList.append(editWithAction_);
	actionList.append(runAction_);
	actionList.append(openContainingFolderAction_);

    runAction_->setEnabled(runExecutableSet());

    return actionList;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::onEditorChanged()
//-----------------------------------------------------------------------------
void ComponentEditorFileItem::onEditorChanged()
{
	// on file also the grand parent must be updated
	if (parent() && parent()->parent())
    {
		emit contentChanged(parent()->parent());
	}

	// call the base class to update this and parent
	ComponentEditorItem::onEditorChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::absolutePath()
//-----------------------------------------------------------------------------
QString ComponentEditorFileItem::fileAbsolutePath() const
{
    const QString relPath = file_->name();
    const QString xmlPath = libHandler_->getPath(component_->getVlnv());

    return General::getAbsolutePath(xmlPath, relPath);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::runInApplication()
//-----------------------------------------------------------------------------
void ComponentEditorFileItem::runInApplication(QString const& applicationPath)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QStringList arguments(fileAbsolutePath());
    QProcess::startDetached(applicationPath, arguments);

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::useKactusCSourceEditor()
//-----------------------------------------------------------------------------
bool ComponentEditorFileItem::useKactusCSourceEditor() const
{
	QSettings settings;

	foreach(QString fileType, *file_->getFileTypes())
	{
		QString key = "FileTypes/" + fileType + "/EditInKactus";

		if (settings.value(key).toBool())
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::runExecutableSet()
//-----------------------------------------------------------------------------
bool ComponentEditorFileItem::runExecutableSet() const
{
    return !executablePath().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::executablePath()
//-----------------------------------------------------------------------------
QString ComponentEditorFileItem::executablePath() const
{
    QSettings settings;

    foreach(QString fileType, *file_->getFileTypes())
    {
        QString key = "FileTypes/" + fileType + "/Executable";
        QString executableName = settings.value(key).toString();

        executableName = resolveEnvironmentVariables(executableName);

        if (QFileInfo(executableName).isExecutable())
        {
            return executableName;
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::resolveEnvironmentVariables()
//-----------------------------------------------------------------------------
QString ComponentEditorFileItem::resolveEnvironmentVariables(QString const& text) const
{    
    QSettings settings; 
    QString parsed = text;

    QRegularExpression environmental("(\\$\\(?\\w+\\)?)");
    QRegularExpressionMatchIterator iter = environmental.globalMatch(text);
   
    while (iter.hasNext())
    {
        QRegularExpressionMatch match = iter.next();

        const QString variable = match.captured();

        QString variableName = variable;
        variableName.remove("$");
        
        // Remove enclosing parenthesis.
        if (variableName.startsWith("("))
        {
            variableName = variableName.mid(1);
            variableName.chop(1);
        }

        QString key = "K2Variables/" + variableName + "/value";
        QString variableValue = settings.value(key).toString();

        parsed.replace(variable, variableValue);
    }

    return parsed;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::onOpenContainingFolder()
//-----------------------------------------------------------------------------
void ComponentEditorFileItem::onOpenContainingFolder()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
	QString path = QFileInfo(fileAbsolutePath()).absolutePath();

	// Open the folder in the operating system's default file browser.
	QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::isValidPath()
//-----------------------------------------------------------------------------
bool ComponentEditorFileItem::isValidPath(QString const& filePath) const
{
    QString absolutePath;
    QFileInfo originalInfo(filePath);
    if (originalInfo.isRelative())
    {
        QString basePath = libHandler_->getPath(component_->getVlnv());
        absolutePath = General::getAbsolutePath(basePath, filePath);
    }
    // if the reference is directly absolute
    else
    {
        absolutePath = filePath;
    }

    // check if the file exists in the file system
    QFileInfo fileInfo(absolutePath);
    return fileInfo.exists();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::isValidURI()
//-----------------------------------------------------------------------------
bool ComponentEditorFileItem::isValidURI(QString const& fileURI) const
{                     
    return Utils::URL_VALIDITY_REG_EXP.match(fileURI).hasMatch();
}
