#pragma once

/*
 
 BEGIN_JUCE_MODULE_DECLARATION

  ID:               jcf_lime_juce
  vendor:           juce
  version:          4.2.1
  name:             JCF LIME JUCE
  description:      Utilities applicable to many JUCE application
  website:          http://www.juce.com/juce
  license:          Commerical

  dependencies:     juce_core juce_data_structures
  OSXFrameworks:    
  iOSFrameworks:    

 END_JUCE_MODULE_DECLARATION
 */

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace jcf
{
    using namespace juce;

    /**
     Loads a valuetree compatible XML file. Returna ValueTree::invalid if something goes wrong.
     */
    inline ValueTree loadValueTreeFromXml(const File & file)
    {
        ScopedPointer<XmlElement> xml = XmlDocument(file.loadFileAsString()).getDocumentElement();

        if (!xml)
            return ValueTree::invalid;

        auto tree = ValueTree::fromXml(*xml);
        return tree;
    }

    inline Result saveValueTreeToXml(const File & file, const ValueTree & tree)
    {
        auto string = tree.toXmlString();
        auto result = file.replaceWithText(string);

        if (!result)
            return Result::fail("Save failed to " + file.getFullPathName());

        return Result::ok();
    }


#include "ui/jcf_font_awesome.h"
};


