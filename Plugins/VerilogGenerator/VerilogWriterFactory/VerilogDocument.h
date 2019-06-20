//-----------------------------------------------------------------------------
// File: VerilogDocument.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Storage for Verilog writers.
//-----------------------------------------------------------------------------

#ifndef VERILOGDOCUMENT_H
#define VERILOGDOCUMENT_H

#include "../veriloggeneratorplugin_global.h"

// #include <Plugins/PluginSystem/GeneratorPlugin/OutputControl.h>

#include <Plugins/PluginSystem/GeneratorPlugin/GenerationOutput.h>

#include <Plugins/VerilogGenerator/ComponentVerilogWriter/ComponentVerilogWriter.h>
#include <Plugins/VerilogGenerator/ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.h>
#include <Plugins/VerilogGenerator/VerilogHeaderWriter/VerilogHeaderWriter.h>

//-----------------------------------------------------------------------------
// Verilog document.
//-----------------------------------------------------------------------------
class VerilogDocument : public GenerationOutput
{
public:

    /*!
     *  The constructor.
     */
    VerilogDocument() = default;
    
    /*!
     *  The destructor.
     */
    virtual  ~VerilogDocument() = default;

    /*!
     *  Writes the content. 
     *
     *      @param [in] outputDirectory         The possible output directory.
     */
	virtual void write(QString const& outputDirectory) override;
    
    /*!
     *  Finds position for body text highlight in document, if any exists.
     *
	 *      @param [out] begin                  The position where the highlight begins, if successful.
     *      @param [out] end                    The position where the highlight ends, if successful.
     */
    virtual void getBodyHighlight(int& begin, int& end) const override;

    /*!
     *  Finds the module implementation in Verilog code.
     *
     *      @param [in] code		            The code that will be inspected.
	 *      @param [out] implementation         The found module implementation, if successful.
     *      @param [out] postModule		        Anything that exists after the module declaration.
     *      @param [out] error                  If fails, this will contain an error message.
     *
     *      @return True, if a single implementation could be found, else false.
     */
	bool selectImplementation(QString const& code, QString& implementation,
        QString& postModule, QString& error);

    /*!
     *  Get the module implementation.
     *
     *      @return The module implementation.
     */
    QString getImplementation() const;

    /*!
     *  Set the module implementation.
     *
     *      @param [in] newImplementation   The new module implementation.
     */
    void setImplementation(QString const& newImplementation);

    /*!
     *  Get the post module definition.
     *
     *      @return The post module definition.
     */
    QString getPostModule() const;

    /*!
     *  Set the post module definition.
     *
     *      @param [in] newPostModule   The new post module definition.
     */
    void setPostModule(QString const& newPostModule);

    /*!
     *  Set a new writer for generating the header.
     *
     *      @param [in] newHeaderWriter     The new writer for generating the file header.
     */
    void setHeaderWriter(QSharedPointer<VerilogHeaderWriter> newHeaderWriter);

    /*!
     *  Get the component writer.
     *
     *      @return The component writer.
     */
    QSharedPointer<ComponentVerilogWriter> getTopWriter() const;

    /*!
     *  Set a new writer for generating the component data.
     *
     *      @param [in] newTopWriter    The new component writer.
     */
    void setTopWriter(QSharedPointer<ComponentVerilogWriter> newTopWriter);

    /*!
     *  Setup the design writers.
     */
    void initializeDesignWriters();

    /*!
     *  Get the top default writers.
     *
     *      @return The top default writers.
     */
    QSharedPointer<WriterGroup> getTopDefaultWriters() const;

    /*!
     *  Get the interconnection writers.
     *
     *      @return The interconnection writers.
     */
    QSharedPointer<WriterGroup> getInterconnectionWriters() const;

    /*!
     *  Get the connection wire writers.
     *
     *      @return The connection wire writers.
     */
    QSharedPointer<WriterGroup> getConnectionWireWriters() const;

    /*!
     *  Get ad hoc wire writers.
     *
     *      @return Ad hoc wire writers.
     */
    QSharedPointer<WriterGroup> getAdHocWireWriters() const;

    /*!
     *  Get port wire writers.
     *
     *      @return The port wire writers.
     */
    QSharedPointer<WriterGroup> getPortWireWriters() const;

    /*!
     *  Get the top assignment writers.
     *
     *      @return The top assignment writers.
     */
    QSharedPointer<WriterGroup> getTopAssignmentWriters() const;

    /*!
     *  Get the component instance assignment writers.
     *
     *      @return The component instance assignment writers.
     */
    QSharedPointer<WriterGroup> getInstanceAssignmentWriters() const;

    /*!
     *  Add an component instance writer.
     *
     *      @param [in] newInstanceWriter   The selected new component instance writer.
     */
    void addInstanceWriter(QSharedPointer<ComponentInstanceVerilogWriter> newInstanceWriter);

    /*!
     *  Add a component instance header writer.
     *
     *      @param [in] instanceWriter  The selected component instance writer.
     *      @param [in] headerWriter    The new instance header writer.
     */
    void addInstanceHeaderWriter(QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter,
        QSharedPointer<Writer> headerWriter);

    /*!
     *  Add the writers to the top component writer.
     */
    void addWritersToTopComponent();

private:

    //! This will contain the found implementation, if success.
    QString implementation_;

    //! This may contain the text after the module definition, if success.
    QString postModule_;

    //! Writer for generating file header.
    QSharedPointer<VerilogHeaderWriter> headerWriter_;

    //! Writer for the component, is to contain everything that comes to document, except header writer.
    QSharedPointer<ComponentVerilogWriter> topWriter_;

    //! Writer for top defaults.
    QSharedPointer<WriterGroup> topDefaultWriters_;

    //! Writers for Verilog interconnections.
    QSharedPointer<WriterGroup> interconnectionWriters_;

    //! Writers for interconnection wires.
    QSharedPointer<WriterGroup> connectionWireWriters_;

    //! Writers for interconnection wires.
    QSharedPointer<WriterGroup> adHocWireWriters_;

    //! Writers for port assignment wires.
    QSharedPointer<WriterGroup> portWireWriters_;

    //! Writers for hierarchical port assignments.
    QSharedPointer<WriterGroup> topAssignmentWriters_;

    //! Writers for instance port assignments.
    QSharedPointer<WriterGroup> instanceAssignmentWriters_;

    //! Writers for Verilog instances.
    QList<QSharedPointer<ComponentInstanceVerilogWriter> > instanceWriters_;

    //! Writers for Verilog instances.
    QMap<QSharedPointer<ComponentInstanceVerilogWriter>, QSharedPointer<Writer> > instanceHeaderWriters_;

};

#endif // VERILOGDOCUMENT_H
