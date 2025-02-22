//-----------------------------------------------------------------------------
// File: FieldValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.11.2015
//
// Description:
// Validator for ipxact:Field.
//-----------------------------------------------------------------------------

#include "FieldValidator.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/Component/FieldReset.h>
#include <IPXACTmodels/Component/ResetType.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: FieldValidator::FieldValidator()
//-----------------------------------------------------------------------------
FieldValidator::FieldValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator,
    QSharedPointer<ParameterValidator> parameterValidator):
expressionParser_(expressionParser),
enumeratedValueValidator_(enumeratedValueValidator),
parameterValidator_(parameterValidator),
availableResetTypes_()
{

}

//-----------------------------------------------------------------------------
// Function: FieldValidator::componentChange()
//-----------------------------------------------------------------------------
void FieldValidator::componentChange(QSharedPointer<Component> newComponent)
{
    availableResetTypes_.clear();
    if (newComponent)
    {
        availableResetTypes_ = newComponent->getResetTypes();
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::getEnumeratedValueValidator()
//-----------------------------------------------------------------------------
QSharedPointer<EnumeratedValueValidator> FieldValidator::getEnumeratedValueValidator() const
{
    return enumeratedValueValidator_;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::validate()
//-----------------------------------------------------------------------------
bool FieldValidator::validate(QSharedPointer<Field> field) const
{
    return hasValidName(field) && hasValidIsPresent(field) && hasValidBitOffset(field) &&
        hasValidResets(field) && hasValidWriteValueConstraint(field) &&
        hasValidWriteValueConstraint(field) && hasValidReserved(field) && hasValidBitWidth(field) &&
        hasValidEnumeratedValues(field) && hasValidParameters(field) && hasValidAccess(field);
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidName()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidName(QSharedPointer<Field> field) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern(QStringLiteral("^\\s*$"));
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(field->name());

    if (field->name().isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidIsPresent(QSharedPointer<Field> field) const
{
    if (!field->getIsPresent().isEmpty())
    {
        QString solvedValue = expressionParser_->parseExpression(field->getIsPresent());

        bool toIntOk = true;
        int intValue = solvedValue.toInt(&toIntOk);

        if (!toIntOk || intValue < 0 || intValue > 1)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidBitOffset()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidBitOffset(QSharedPointer<Field> field) const
{
    QString solvedValue = expressionParser_->parseExpression(field->getBitOffset());

    bool changeOk = true;
    solvedValue.toULongLong(&changeOk);

    return changeOk;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidResetTypeReference()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidResetTypeReference(QSharedPointer<FieldReset> fieldReset) const
{
    if (fieldReset->getResetTypeReference().isEmpty())
    {
        return true;
    }

    else if (availableResetTypes_)
    {
        for (auto resetType : *availableResetTypes_)
        {
            if (resetType->name() == fieldReset->getResetTypeReference())
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidResetValue()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidResetValue(QSharedPointer<FieldReset> fieldReset) const
{
    if (fieldReset->getResetValue().isEmpty() == false)
    {
        return isBitExpressionValid(fieldReset->getResetValue());
    }
    else if (fieldReset->getResetValue().isEmpty() &&
        (!fieldReset->getResetTypeReference().isEmpty() || !fieldReset->getResetMask().isEmpty()))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidResetMask()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidResetMask(QSharedPointer<FieldReset> fieldReset) const
{
    if (fieldReset->getResetMask().isEmpty())
    {
        return true;
    }
    else
    {
        return isBitExpressionValid(fieldReset->getResetMask());
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidWriteValueConstraint()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidWriteValueConstraint(QSharedPointer<Field> field) const
{
    if (field->getWriteConstraint())
    {
        QSharedPointer<WriteValueConstraint> writeConstraint = field->getWriteConstraint();

        if (writeConstraint->getType() == WriteValueConstraint::TYPE_COUNT)
        {
            return false;
        }
        else if (writeConstraint->getType() == WriteValueConstraint::MIN_MAX)
        {
            int minimumInt = expressionParser_->parseExpression(writeConstraint->getMinimum()).toInt();
            int maximumInt = expressionParser_->parseExpression(writeConstraint->getMaximum()).toInt();

            return !writeConstraint->getMinimum().isEmpty() && !writeConstraint->getMaximum().isEmpty() &&
                isBitExpressionValid(writeConstraint->getMinimum()) &&
                isBitExpressionValid(writeConstraint->getMaximum()) &&
                minimumInt <= maximumInt;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidReserved()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidReserved(QSharedPointer<Field> field) const
{
    if (!field->getReserved().isEmpty())
    {
        bool isValidReserved = false;
        QString solvedValue = expressionParser_->parseExpression(field->getReserved(), &isValidReserved);
        bool canConvert = false;
        int intValue = solvedValue.toInt(&canConvert);

	    if (isValidReserved == false || canConvert ==false || intValue < 0 || intValue > 1)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidBitWidth()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidBitWidth(QSharedPointer<Field> field) const
{
    QString solvedValue = expressionParser_->parseExpression(field->getBitWidth());

    bool changeOk = true;
    quint64 intValue = solvedValue.toULongLong(&changeOk);

    if (changeOk && intValue > 0)
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidEnumeratedValues()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidEnumeratedValues(QSharedPointer<Field> field) const
{
    if (!field->getEnumeratedValues()->isEmpty())
    {
        QStringList enumeratedValueNames;

        bool constraintUsesEnums = false;
        if (field->getWriteConstraint())
        {
            if (field->getWriteConstraint()->getType() == WriteValueConstraint::USE_ENUM)
            {
                constraintUsesEnums = true;
            }
        }

        bool enumerationWriteUsage = false;
        for (QSharedPointer<EnumeratedValue> const& enumeratedValue : *field->getEnumeratedValues())
        {
            if (!enumerationWriteUsage && constraintUsesEnums &&
                (enumeratedValue->getUsage() == EnumeratedValue::WRITE ||
                enumeratedValue->getUsage() == EnumeratedValue::READWRITE))
            {
                enumerationWriteUsage = true;
            }

            if (!enumeratedValueValidator_->validate(enumeratedValue) ||
                enumeratedValueNames.contains(enumeratedValue->name()))
            {
                return false;
            }
            else
            {
                enumeratedValueNames.append(enumeratedValue->name());
            }
        }

        if (constraintUsesEnums && !enumerationWriteUsage)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidParameters()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidParameters(QSharedPointer<Field> field) const
{
    if (!field->getParameters()->isEmpty())
    {
        QStringList parameterNames;

        for (QSharedPointer<Parameter> const& parameter : *field->getParameters())
        {
            if (parameterNames.contains(parameter->name()) || !parameterValidator_->validate(parameter))
            {
                return false;
            }
            else
            {
                parameterNames.append(parameter->name());
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidAccess()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidAccess(QSharedPointer<Field> field) const
{
    if (field->getAccess() == AccessTypes::READ_ONLY)
    {
        return field->getModifiedWrite() == General::MODIFIED_WRITE_COUNT;
    }
    else if (field->getAccess() == AccessTypes::WRITE_ONLY || field->getAccess() == AccessTypes::WRITEONCE)
    {
        return field->getReadAction() == General::READ_ACTION_COUNT;
    }
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Field> field, QString const& context)
    const
{
    findErrorsInName(errors, field, context);
    findErrorsInIsPresent(errors, field, context);
    findErrorsInBitOffset(errors, field, context);
    findErrorsInResets(errors, field, context);
    findErrorsInWriteValueConstraint(errors, field, context);
    findErrorsInReserved(errors, field, context);
    findErrorsInBitWidth(errors, field, context);
    findErrorsInEnumeratedValues(errors, field);
    findErrorsInParameters(errors, field);
    findErrorsInAccess(errors, field, context);
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInName(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (!hasValidName(field))
    {
        errors.append(QObject::tr("Invalid name specified for %1 within %2").arg(field->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInIsPresent(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (!hasValidIsPresent(field))
    {
        errors.append(QObject::tr("Invalid isPresent value specified for %1 within %2. Value should evaluate to "
            "0 or 1.").
            arg(field->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInBitOffset()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInBitOffset(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (!hasValidBitOffset(field))
    {
        errors.append(QObject::tr("Invalid bit offset set for field %1 within %2").arg(field->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInResets()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInResets(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    QString resetContext = QStringLiteral("field %1 in %2").arg(field->name(), context);

    if (!hasValidResetsTypeReferences(field))
    {
        errors.append(QObject::tr("Multiple references to default reset type in %1.").arg(resetContext));
    }

    for (auto fieldReset : *field->getResets())
    {
        findErrorsInResetTypeReference(errors, fieldReset, resetContext);
        findErrorsInResetValue(errors, fieldReset, resetContext);
        findErrorsInResetMask(errors, fieldReset, resetContext);
    }
}


//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInResetTypeReference()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInResetTypeReference(QVector<QString>& errors,
    QSharedPointer<FieldReset> fieldReset, QString const& context) const
{
    if (!hasValidResetTypeReference(fieldReset))
    {
        errors.append(QObject::tr("Reset type '%1' referenced in %2 does not exist.")
            .arg(fieldReset->getResetTypeReference()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInResetValue()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInResetValue(QVector<QString>& errors, QSharedPointer<FieldReset> fieldReset,
    QString const& context) const
{
    if (!hasValidResetValue(fieldReset))
    {
        errors.append(QObject::tr("Invalid reset value set within %1").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInResetMask()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInResetMask(QVector<QString>& errors, QSharedPointer<FieldReset> fieldReset,
    QString const& context) const
{
    if (!hasValidResetMask(fieldReset))
    {
        errors.append(QObject::tr("Invalid reset mask set within %1").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInWriteValueConstraint()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInWriteValueConstraint(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (!hasValidWriteValueConstraint(field))
    {
        QSharedPointer<WriteValueConstraint> writeConstaint = field->getWriteConstraint();
        if (writeConstaint->getType() == WriteValueConstraint::TYPE_COUNT)
        {
            errors.append(QObject::tr("Invalid write value constraint set for field %1 within %2").
                arg(field->name()).arg(context));
        }
        else
        {
            bool minimumIsValid = isBitExpressionValid(writeConstaint->getMinimum());
            bool maximumIsValid = isBitExpressionValid(writeConstaint->getMaximum());

            if (!minimumIsValid)
            {
                errors.append(QObject::tr(
                    "Invalid minimum value set for write value constraint in field %1 within %2").
                    arg(field->name()).arg(context));
            }
            if (!maximumIsValid)
            {
                errors.append(QObject::tr(
                    "Invalid maximum value set for write value constraint in field %1 within %2").
                    arg(field->name()).arg(context));
            }

            if (minimumIsValid && maximumIsValid)
            {
                int minimumInt = expressionParser_->parseExpression(writeConstaint->getMinimum()).toInt();
                int maximumInt = expressionParser_->parseExpression(writeConstaint->getMaximum()).toInt();

                if (minimumInt > maximumInt)
                {
                    errors.append(QObject::tr(
                        "Maximum value must be greater than or equal to the minimum value in write value "
                        "constraint set for field %1 within %2").arg(field->name()).arg(context));
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInReserved()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInReserved(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (!hasValidReserved(field))
    {
        errors.append(QObject::tr("Invalid reserved set for field %1 within %2").arg(field->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInBitWidth()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInBitWidth(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (!hasValidBitWidth(field))
    {
        errors.append(QObject::tr("Invalid bit width set for field %1 within %2").arg(field->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInEnumeratedValues()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInEnumeratedValues(QVector<QString>& errors, QSharedPointer<Field> field) const
{
    if (!field->getEnumeratedValues()->isEmpty())
    {
        QString context = QStringLiteral("field ") + field->name();

        bool useEnumerations = false;
        if (field->getWriteConstraint())
        {
            if (field->getWriteConstraint()->getType() == WriteValueConstraint::USE_ENUM)
            {
                useEnumerations = true;
            }
        }

        bool writeEnumerationWasFound = false;
        QStringList enumeratedValueNames;
        for (QSharedPointer<EnumeratedValue> const& enumeratedValue : *field->getEnumeratedValues())
        {
            enumeratedValueValidator_->findErrorsIn(errors, enumeratedValue, context);

            if (enumeratedValueNames.contains(enumeratedValue->name()))
            {
                errors.append(QObject::tr("Name %1 of enumerated values in %2 is not unique.").
                    arg(enumeratedValue->name()).arg(context));
            }
            else
            {
                enumeratedValueNames.append(enumeratedValue->name());
            }

            if (useEnumerations && !writeEnumerationWasFound &&
                (enumeratedValue->getUsage() == EnumeratedValue::WRITE ||
                enumeratedValue->getUsage() == EnumeratedValue::READWRITE))
            {
                writeEnumerationWasFound = true;
            }
        }

        if (useEnumerations && !writeEnumerationWasFound)
        {
            errors.append(QObject::tr("Write value constraint of Use enumerated values needs an enumerated value "
                "with usage of write or read-write. Such an enumerated value was not found in field %1")
                .arg(field->name()));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInParameters()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInParameters(QVector<QString>& errors, QSharedPointer<Field> field) const
{
    if (!hasValidParameters(field))
    {
        QString context = QStringLiteral("field ") + field->name();

        QStringList parameterNames;
        for (QSharedPointer<Parameter> const& parameter : *field->getParameters())
        {
            parameterValidator_->findErrorsIn(errors, parameter, context);

            if (parameterNames.contains(parameter->name()))
            {
                errors.append(QObject::tr("Name %1 of parameters in %2 is not unique.").arg(parameter->name()).
                    arg(context));
            }
            else
            {
                parameterNames.append(parameter->name());
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInAccess()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInAccess(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (field->getAccess() == AccessTypes::READ_ONLY)
    {
        if (field->getModifiedWrite() != General::MODIFIED_WRITE_COUNT)
        {
            errors.append(QObject::tr("In field %1 within %2, access type readOnly does not allow a field "
                "to include a modified write value.").arg(field->name()).arg(context));
        }
    }
    else if (field->getAccess() == AccessTypes::WRITE_ONLY || field->getAccess() == AccessTypes::WRITEONCE)
    {
        if (field->getReadAction() != General::READ_ACTION_COUNT)
        {
            errors.append(QObject::tr("In field %1 within %2, access type write only and write once do not "
                "allow a field to include a read action value.").arg(field->name()).arg(context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidResets()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidResets(QSharedPointer<Field> field) const
{
    if (!hasValidResetsTypeReferences(field))
    {
        return false;
    }

    for (auto fieldReset : *field->getResets())
    {
        if (!hasValidResetValue(fieldReset) || !hasValidResetMask(fieldReset) ||
            !hasValidResetTypeReference(fieldReset))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidResetsTypeReferences()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidResetsTypeReferences(QSharedPointer<Field> field) const
{
    for (int i = 0; i < field->getResets()->size(); ++i)
    {
        QSharedPointer<FieldReset> fieldReset = field->getResets()->at(i);
        if (fieldReset->getResetTypeReference().isEmpty())
        {
            for (int j = i + 1; j < field->getResets()->size(); ++j)
            {
                if (field->getResets()->at(j)->getResetTypeReference().isEmpty())
                {
                    return false;
                }
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::validateBitExpression()
//-----------------------------------------------------------------------------
bool FieldValidator::isBitExpressionValid(QString const& expression) const
{
    QString solvedValue = expressionParser_->parseExpression(expression);

    QRegularExpression bitExpression(QStringLiteral("^([0-9]+|[1-9]+[0-9]*'([bB][01_]+|[hH][0-9a-fA-F_]+))$"));
    return bitExpression.match(expression).hasMatch() || bitExpression.match(solvedValue).hasMatch();
}

