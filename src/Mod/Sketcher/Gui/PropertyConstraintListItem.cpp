/***************************************************************************
* Copyright (c) 2014 Abdullah Tahiri <abdullah.tahiri.yo@gmail.com>        *
*                                                                          *
* This file is part of the FreeCAD CAx development system.                 *
*                                                                          *
* This library is free software; you can redistribute it and/or            *
* modify it under the terms of the GNU Library General Public              *
* License as published by the Free Software Foundation; either             *
* version 2 of the License, or (at your option) any later version.         *
*                                                                          *
* This library is distributed in the hope that it will be useful,          *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
* GNU Library General Public License for more details.                     *
*                                                                          *
* You should have received a copy of the GNU Library General Public        *
* License along with this library; see the file COPYING.LIB. If not,       *
* write to the Free Software Foundation, Inc., 59 Temple Place,            *
* Suite 330, Boston, MA 02111-1307, USA                                    *
*                                                                          *
***************************************************************************/

#include "PreCompiled.h"

#ifndef _PreComp_
# include <algorithm>
# include <QComboBox>
# include <QFontDatabase>
# include <QLayout>
# include <QLocale>
# include <QPixmap>
# include <QSpinBox>
# include <QTextStream>
# include <QTimer>
#endif

#include <Base/Tools.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <App/PropertyGeo.h>
#include <App/PropertyFile.h>
#include <App/PropertyUnits.h>
#include <Gui/Application.h>
#include <Gui/Control.h>
#include <Gui/Document.h>
#include <Gui/Selection.h>
#include <Gui/ViewProviderDocumentObject.h>
#include <Gui/Placement.h>
#include <Gui/FileDialog.h>
#include <Gui/DlgPropertyLink.h>
#include <Gui/QuantitySpinBox.h>

#include <Gui/propertyeditor/PropertyItem.h>

#include "../App/PropertyConstraintList.h"

#include "PropertyConstraintListItem.h"


using namespace SketcherGui;
using namespace Gui::PropertyEditor;

TYPESYSTEM_SOURCE(SketcherGui::PropertyConstraintListItem, Gui::PropertyEditor::PropertyItem);

PropertyConstraintListItem::PropertyConstraintListItem()
{
}

QVariant PropertyConstraintListItem::toString(const QVariant& prop) const
{
    QString str;
    for (std::vector<App::Property*>::iterator it = propertyItems.begin(); it != propertyItems.end(); ++it){
        
    }
   

    return QVariant(QString::fromUtf8(""));
}

QVariant PropertyConstraintListItem::value(const App::Property* prop) const
{
    assert(prop && prop->getTypeId().isDerivedFrom(Sketcher::PropertyConstraintList::getClassTypeId()));
    
    QString valuestr;
    fillInSubProperties(prop,valuestr);
    // double value = static_cast<const App::PropertyFloat*>(prop)->getValue();
    return QVariant(valuestr);
}

void PropertyConstraintListItem::setValue(const QVariant& value)
{
    ;
    /*if (!value.canConvert(QVariant::Double))
        return;
    double val = value.toDouble();
    QString data = QString::fromAscii("%1").arg(val,0,'f',decimals());
    setPropertyValue(data);*/
}

QWidget* PropertyConstraintListItem::createEditor(QWidget* parent, const QObject* receiver, const char* method) const
{
    QLineEdit *le = new QLineEdit(parent);
    le->setFrame(false);
    le->setReadOnly(true);
    return le;
}

void PropertyConstraintListItem::setEditorData(QWidget *editor, const QVariant& data) const
{
    QLineEdit* le = qobject_cast<QLineEdit*>(editor);
    const Base::Vector3d& value = data.value<Base::Vector3d>();
    QString text = QString::fromAscii("[%1 %2 %3]")
        .arg(QLocale::system().toString(value.x, 'f', 2))
        .arg(QLocale::system().toString(value.y, 'f', 2))
        .arg(QLocale::system().toString(value.z, 'f', 2));
    le->setText(text);
}

QVariant PropertyConstraintListItem::editorData(QWidget *editor) const
{
    QLineEdit *le = qobject_cast<QLineEdit*>(editor);
    return QVariant(le->text());
}

void PropertyConstraintListItem::buildUp(const App::Property* prop)
{
    this->reset();
    
    const std::vector< Sketcher::Constraint * > &vals = static_cast<const Sketcher::PropertyConstraintList*>(prop)->getValues();

    for(std::vector< Sketcher::Constraint * >::const_iterator it= vals.begin();it!=vals.end();++it){
        if (!(*it)->Name.empty() && // Named constraint
            ((*it)->Type == Sketcher::Distance || // Datum constraint
            (*it)->Type == Sketcher::DistanceX ||
            (*it)->Type == Sketcher::DistanceY ||
            (*it)->Type == Sketcher::Radius ||
            (*it)->Type == Sketcher::Angle)) {
            
        
            PropertyUnitItem* mp=static_cast<PropertyUnitItem*>(PropertyUnitItem::create());
            mp->setParent(this);
            mp->setPropertyName(QString::fromUtf8((*it)->Name.c_str()));
            this->appendChild(mp); 
            //propertyUnitItems.append(*mp);
            

           
        
            
            //propertyData.addProperty(this, #_prop_, &this->_prop_, (_group_),(_type_),(_Docu_));
            //App::PropertyDistance pd;
            //pd.setValue((*it)->Value);
            //pd.setContainer(this);
            //propertyData.addProperty(this,(*it)->Name.c_str(),&pd,"Sketcher Constraints",(App::PropertyType)(App::Prop_None),(*it)->Name.c_str());
        }

    }
    
}

void PropertyConstraintListItem::fillInSubProperties(const App::Property* prop, QString &valuestr) const
{   
    const std::vector< Sketcher::Constraint * > &vals = static_cast<const Sketcher::PropertyConstraintList*>(prop)->getValues();

    valuestr=QString::fromAscii("[");
    
    int i=0;
    for(std::vector< Sketcher::Constraint * >::const_iterator it= vals.begin();it!=vals.end();++it){
        if (!(*it)->Name.empty() && // Named constraint
            ((*it)->Type == Sketcher::Distance || // Datum constraint
            (*it)->Type == Sketcher::DistanceX ||
            (*it)->Type == Sketcher::DistanceY ||
            (*it)->Type == Sketcher::Radius ||
            (*it)->Type == Sketcher::Angle)) {
            i++;
        
            /*PropertyUnitItem* mp=static_cast<PropertyUnitItem*>(PropertyUnitItem::create());
            mp->setParent(this);
            mp->setPropertyName(QString::fromUtf8((*it)->Name.c_str()));
            this->appendChild(mp); */
            //propertyUnitItems.append(*mp);
            
            switch((*it)->Type){
                case Sketcher::Angle:
                    valuestr+= i==1? 
                        Base::Quantity((*it)->Value, Base::Unit::Angle).getUserString():
                        QString::fromAscii("  ") +
                        Base::Quantity((*it)->Value, Base::Unit::Angle).getUserString();
                    break;
                default:
                    valuestr+= i==1?
                        Base::Quantity((*it)->Value, Base::Unit::Length).getUserString():
                        QString::fromAscii("  ") +
                        Base::Quantity((*it)->Value, Base::Unit::Length).getUserString();
                    break;
            }
           
        
            
            //propertyData.addProperty(this, #_prop_, &this->_prop_, (_group_),(_type_),(_Docu_));
            //App::PropertyDistance pd;
            //pd.setValue((*it)->Value);
            //pd.setContainer(this);
            //propertyData.addProperty(this,(*it)->Name.c_str(),&pd,"Sketcher Constraints",(App::PropertyType)(App::Prop_None),(*it)->Name.c_str());
        }

    }
    valuestr+=QString::fromAscii("]");
}
