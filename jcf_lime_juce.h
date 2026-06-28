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

/**
 * Handy macro for cross-platform menu titles, e.g. Open In Explorer
 */
#ifdef _WIN32
#define EXPLORER_OR_FINDER "Explorer"
#else
#define EXPLORER_OR_FINDER "Finder"
#endif

namespace jcf
{

/** Platform-conventional name for the Command / Control modifier key.

    Returns the Unicode glyph on macOS and the spelled-out name elsewhere, for
    use in menus, tooltips and shortcut hints. The macOS glyphs are UTF-8, so
    these always build the String with String::fromUTF8 — callers use the result
    directly and never handle the raw bytes.

    @see altKey, shiftKey, keyCombo
*/
inline juce::String commandKey()
{
   #if JUCE_MAC
    return juce::String::fromUTF8 ("\xe2\x8c\x98"); // U+2318 PLACE OF INTEREST SIGN (Command)
   #else
    return "Ctrl";
   #endif
}

/** Platform-conventional name for the Alt / Option modifier key. @see commandKey */
inline juce::String altKey()
{
   #if JUCE_MAC
    return juce::String::fromUTF8 ("\xe2\x8c\xa5"); // U+2325 OPTION KEY
   #else
    return "Alt";
   #endif
}

/** Platform-conventional name for the Shift modifier key. @see commandKey */
inline juce::String shiftKey()
{
   #if JUCE_MAC
    return juce::String::fromUTF8 ("\xe2\x87\xa7"); // U+21E7 UPWARDS WHITE ARROW (Shift)
   #else
    return "Shift";
   #endif
}

/** Joins two modifier-key names the way each platform writes a chord: adjacent
    glyphs on macOS (e.g. the Command + Shift glyphs), "+"-separated names
    elsewhere (e.g. "Ctrl+Shift"). @see commandKey
*/
inline juce::String keyCombo (const juce::String& a, const juce::String& b)
{
   #if JUCE_MAC
    return a + b;
   #else
    return a + "+" + b;
   #endif
}

} // namespace jcf

namespace juce
{
inline bool operator< (const Identifier& a, const Identifier& b)
{
    return a.toString() < b.toString();
}
}

#include "ui/jcf_font_awesome.h"
#include "utils/other_utils.h"

#include "utils/pitch.h"
#include "crypto/jcf_blowfish_extended.h"
#include "crypto/jcf_secure_credentials.h"
#include "utils/lock_free_call_queue.h"
#include "utils/multi_async_updater.h"
#include "utils/app_options.h"