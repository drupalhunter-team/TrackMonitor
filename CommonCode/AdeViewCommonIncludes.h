// Pour precompilation
#ifndef ADEVIEW_COMMUN_INCLUDES_H
#define ADEVIEW_COMMUN_INCLUDES_H

// Minimum system required : Windows Server 2003 with SP1, Windows XP with SP2
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0502
#endif						

#include <windows.h>
#include <stdio.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <io.h>
#include <share.h>
#include <time.h>
#include <sstream>
#include <fstream>
#include <strstream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/unique_ptr.hpp>
#include <boost/smart_ptr.hpp>

#include "AdeNonCopyable.h"
#include "AdeGenMacros.h"
#include "AdeTrace.h"
#include "DefGlob.h"

#pragma warning(disable:4786)

#endif 

