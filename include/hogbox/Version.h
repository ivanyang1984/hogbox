/* Written by Thomas Hogarth, (C) 2011
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
 */

#include <hogbox/Export.h>
#include <string>
#include <stdio.h>

extern "C" {

#define hogbox_MAJOR_VERSION    0
#define hogbox_MINOR_VERSION    2
#define hogbox_PATCH_VERSION    0
#define hogbox_SOVERSION        1

extern HOGBOX_EXPORT const char* hogboxGetVersion();

extern HOGBOX_EXPORT const char* hogboxGetSOVersion();

extern HOGBOX_EXPORT const char* hogboxGetLibraryName();

}
