//-----------------------------------------------------------------------------
// File: BusDefinitionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 20.6.2011
//
// Description:
// Bus defition editor is an editor for Bus- and Abstraction definitions.
//-----------------------------------------------------------------------------

#include "BusDefinitionEditor.h"

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/BusDefinition/validators/BusDefinitionValidator.h>
#include <IPXACTmodels/AbstractionDefinition/validators/AbstractionDefinitionValidator.h>

#include <common/dialogs/newObjectDialog/newobjectdialog.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>
#include <editors/BusDefinitionEditor/interfaces/PortAbstractionInterface.h>

#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::BusDefinitionEditor()
//-----------------------------------------------------------------------------
BusDefinitionEditor::BusDefinitionEditor(QWidget *parent, LibraryInterface* libHandler,
    QSharedPointer<BusDefinition> busDef, QSharedPointer<AbstractionDefinition> absDef, bool disableBusDef):
TabDocument(parent, DOC_PROTECTION_SUPPORT),
libHandler_(libHandler),
busDef_(busDef),
absDef_(absDef),
busDefGroup_(libHandler, this),
absDefGroup_(libHandler, createPortAbstractionInterface(), createPortAbstractionInterface(), this),
expressionParser_(new SystemVerilogExpressionParser()),
busDefinitionValidator_(new BusDefinitionValidator(libHandler, expressionParser_)),
absDefinitionValidator_(new AbstractionDefinitionValidator(libHandler, expressionParser_)),
busDefinitionSaved_(false)
{
    if (absDef_)
    {
        absDefGroup_.setAbsDef(absDef_);
    }
	else
    {
		absDefGroup_.setDisabled(true);
	}

    if (busDef_)
    {
        busDefGroup_.setBusDef(busDef_);
        absDefGroup_.setBusDef(busDef_);
    } 
    
	busDefGroup_.setDisabled(!busDef_ || disableBusDef);

    setDocumentType(tr("Bus Definition"));

    if (busDef_)
    {
        VLNV vlnv = busDef_->getVlnv();
        setDocumentName(vlnv.getName() + " (" + vlnv.getVersion() + ")");

        // Open in unlocked mode by default only if the version is draft.
        setProtection(vlnv.getVersion() != "draft");
    }
    else
    {
        setProtection(true);
    }    

    setupLayout();

    connect(&busDefGroup_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&absDefGroup_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&absDefGroup_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(&absDefGroup_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
    connect(&absDefGroup_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), 
        this, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::~BusDefinitionEditor()
//-----------------------------------------------------------------------------
BusDefinitionEditor::~BusDefinitionEditor()
{

}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::getIdentifyingVLNV()
//-----------------------------------------------------------------------------
VLNV BusDefinitionEditor::getIdentifyingVLNV() const
{
    return getDocumentVLNV();
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::refresh()
//-----------------------------------------------------------------------------
void BusDefinitionEditor::refresh()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    if (absDef_)
    {
        absDef_ = libHandler_->getModel(absDef_->getVlnv()).dynamicCast<AbstractionDefinition>();
        absDefGroup_.setAbsDef(absDef_);
    }

    if (busDef_)
    {
        busDef_ = libHandler_->getModel(busDef_->getVlnv()).dynamicCast<BusDefinition>();
        busDefGroup_.setBusDef(busDef_);
        absDefGroup_.setBusDef(busDef_);
    } 

    // The document is no longer modified.
    setModified(false);
    TabDocument::refresh();

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: setProtection()
//-----------------------------------------------------------------------------
void BusDefinitionEditor::setProtection(bool locked)
{
    TabDocument::setProtection(locked);
    setEnabled(!locked);
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::getDocumentVLNV()
//-----------------------------------------------------------------------------
VLNV BusDefinitionEditor::getDocumentVLNV() const
{
    // If abstraction definition is being edited then use it as the identifier.
    if (absDef_)
    {
        return absDef_->getVlnv();
    }
    // If only bus definition is being edited then use it as identifier.
    else if (busDef_)
    {
        return busDef_->getVlnv();	
    }
    else
    {
        return VLNV();
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::setBusDef()
//-----------------------------------------------------------------------------
void BusDefinitionEditor::setBusDef(QSharedPointer<BusDefinition> busDef)
{
    Q_ASSERT_X(busDef, "BusDefinitionEditor setBusDef", "Null Bus Definition pointer given as parameter");

    busDef_ = busDef;

    busDefGroup_.setBusDef(busDef_);
    absDefGroup_.setBusDef(busDef_);
    busDefGroup_.setDisabled(false);

    VLNV vlnv = busDef_->getVlnv();
    setDocumentName(vlnv.getName() + " (" + vlnv.getVersion() + ")");
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::setAbsDef()
//-----------------------------------------------------------------------------
void BusDefinitionEditor::setAbsDef(QSharedPointer<AbstractionDefinition> absDef)
{
    absDef_ = absDef;

    if (absDef_) 
    {
        absDefGroup_.setAbsDef(absDef_);
    }

    absDefGroup_.setDisabled(!absDef_);    
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::validate()
//-----------------------------------------------------------------------------
bool BusDefinitionEditor::validate(QVector<QString>& errorList)
{    
    // if bus definition is being edited
    if (busDefGroup_.isEnabled())
    {
        QVector<QString> busDefinitionErrors;
        busDefinitionValidator_->findErrorsIn(busDefinitionErrors, busDef_);

        if (busDefinitionErrors.isEmpty())
        {
            libHandler_->writeModelToFile(busDef_);
            busDefinitionSaved_ = true;
        }
        else
        {
            errorList.append(busDefinitionErrors);
        }
    }

    // if abstraction definition is being edited
    if (absDefGroup_.isEnabled())
    {
        absDefinitionValidator_->findErrorsIn(errorList, absDef_);
    }

    return errorList.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::save()
//-----------------------------------------------------------------------------
bool BusDefinitionEditor::save()
{
    // If bus definition is being edited, save it.
    if (busDefGroup_.isEnabled())
    {
        if (busDefinitionSaved_ == false)
        {
            libHandler_->writeModelToFile(busDef_);
        }

        busDefinitionSaved_ = false;
    }
    
    // If abstraction definition is being edited, save it.
	if (absDefGroup_.isEnabled())
    {
		absDefGroup_.save();
		libHandler_->writeModelToFile(absDef_);
	}

	return TabDocument::save();
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::saveAs()
//-----------------------------------------------------------------------------
bool BusDefinitionEditor::saveAs()
{
    // Ask the user for a new VLNV along with the directory.
    VLNV vlnv;

    VLNV busDefVLNV;
    QString busDirectory;

    VLNV absDefVLNV;
    QString absDirectory;

    if (!NewObjectDialog::saveAsDialog(this, libHandler_, busDef_->getVlnv(), vlnv, busDirectory))
    {
        return false;
    }

    busDefVLNV = vlnv;
    busDefVLNV.setType(VLNV::BUSDEFINITION);
    busDef_->setVlnv(busDefVLNV);

    // If also Abstraction Definition is being edited, save as that too.
    if (absDef_)
    {
        // Remove the possible .busDef from the end of the name field.
        QString absDefName = busDefVLNV.getName();
        absDefName = absDefName.remove(".busDef", Qt::CaseInsensitive);

        absDefGroup_.save();

        // Create automatically vlnv for the abstraction definition.
        absDefVLNV = VLNV(VLNV::ABSTRACTIONDEFINITION, vlnv.getVendor(), vlnv.getLibrary(),
            absDefName + ".absDef", vlnv.getVersion());

        // By default, both bus def and abs def are saved in same directory.
        absDirectory = busDirectory;

        // if the automatic abs def vlnv is taken
        if (libHandler_->contains(absDefVLNV))
        {            

            if (!NewObjectDialog::saveAsDialog(this, libHandler_, absDefVLNV, vlnv, absDirectory))
            {
                return false;
            }
            // Save the created abstraction definition vlnv
            absDefVLNV = vlnv;
        }

        absDef_->setVlnv(absDefVLNV);
        absDef_->setBusType(busDefVLNV);
        
        libHandler_->writeModelToFile(absDirectory, absDef_);                
    }
    
    libHandler_->writeModelToFile(busDirectory, busDef_);
    setDocumentName(vlnv.getName() + " (" + vlnv.getVersion() + ")");
    return TabDocument::saveAs();
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::showEvent()
//-----------------------------------------------------------------------------
void BusDefinitionEditor::showEvent(QShowEvent* event)
{
    TabDocument::showEvent(event);
    emit helpUrlRequested("definitions/busdefinition.html");
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::setupLayout()
//-----------------------------------------------------------------------------
void BusDefinitionEditor::setupLayout()
{    
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);    

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical, scrollArea);
    verticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verticalSplitter->addWidget(&busDefGroup_);
    verticalSplitter->addWidget(&absDefGroup_);
    verticalSplitter->setStretchFactor(0, 1);
    verticalSplitter->setStretchFactor(1, 300);
    verticalSplitter->setContentsMargins(2, 2, 2, 2);
    verticalSplitter->setHandleWidth(10);

    QSplitterHandle* handle = verticalSplitter->handle(1);
    QVBoxLayout* handleLayout = new QVBoxLayout(handle);
    handleLayout->setSpacing(0);
    handleLayout->setMargin(0);

    QFrame* line = new QFrame(handle);
    line->setLineWidth(2);
    line->setMidLineWidth(2);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    handleLayout->addWidget(line);

    scrollArea->setWidget(verticalSplitter);
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::()
//-----------------------------------------------------------------------------
PortAbstractionInterface* BusDefinitionEditor::createPortAbstractionInterface()
{
    PortAbstractionInterface* portInterface(new PortAbstractionInterface());
    return portInterface;
}
