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