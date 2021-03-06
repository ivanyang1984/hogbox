/*
 *  MSRegistry.cpp
 *  hogboxIPhone
 *
 *  Created by Thomas Hogarth on 01/10/2009.
 *  Copyright 2009 HogBox. All rights reserved.
 *
 */

#include <hogboxDB/HogBoxRegistry.h>
#include <hogboxDB/XmlClassWrapper.h>
#include <hogbox/Version.h>

using namespace hogboxDB;


//
//private constructor for singleton
//
HogBoxRegistry::HogBoxRegistry(void) 
	: osg::Referenced()
{
	//add the aliases for the standard osg and hogbox 
	//types to the multi purpose hogbox xml plugin which 
	//should always be available
	AddClassTypeAlias("node", "hogbox");
	AddClassTypeAlias("image", "hogbox");
	AddClassTypeAlias("texture", "hogbox");
	AddClassTypeAlias("shader", "hogbox");
	AddClassTypeAlias("program", "hogbox");
	AddClassTypeAlias("uniform", "hogbox");
	AddClassTypeAlias("hogboxobject", "hogbox");
	AddClassTypeAlias("hogboxmaterial", "hogbox");
	AddClassTypeAlias("hogboxlight", "hogbox");
	AddClassTypeAlias("featurelevel", "hogbox");
	AddClassTypeAlias("hogboxviewer", "hogbox");

	//temp, hogboxvision aliasas should be added by itself
	AddClassTypeAlias("videostream", "vision");
}

HogBoxRegistry::~HogBoxRegistry(void)
{
    OSG_NOTICE << "Deallocating HogBoxRegistry:." << std::endl;
	_xmlNodeManagers.clear();
	_dlList.clear();
}

//
//register a new object with the singleton
//
void HogBoxRegistry::AddXmlNodeManagerToRegistry(XmlClassManagerWrapper* object)
{
	//already exist by manager class name
	XmlClassManager* existing = GetXmlClassManager(object->className());
	if(existing){return;}

	_xmlNodeManagers.push_back(object);
}

//
//return any wrapper using name, if node exists return NULL
//
XmlClassManager* HogBoxRegistry::GetXmlClassManager(const std::string& managerName)
{
	for(unsigned int i=0; i<_xmlNodeManagers.size(); i++)
	{
		if(_xmlNodeManagers[i]->className() == managerName)
		{return _xmlNodeManagers[i]->GetPrototype();}
	}
	return NULL;
}

XmlClassManager* HogBoxRegistry::GetXmlClassManagerForClassType(const std::string& classType)
{
	//check already loaded managers
	for(unsigned int i=0; i<_xmlNodeManagers.size(); i++)
	{
		//check the wrapper contains a valid manager prototype
		if(_xmlNodeManagers[i].valid()){
			if(_xmlNodeManagers[i]->GetPrototype()){
				//does the classmanager accept the class type 
				if(_xmlNodeManagers[i]->GetPrototype()->AcceptsClassType(classType))
				{return _xmlNodeManagers[i]->GetPrototype();}
			}
		}
	}

	//if none of the loaded xml class manager can read the classType
	//try and load a library that can.
	std::string libraryName = CreateXmlLibraryNameForClassType(classType);
	//try to load the lib
	if(!libraryName.empty())
	{		
		osgDB::Registry::LoadStatus result = this->LoadLibrary(libraryName);
		
		if(result == osgDB::Registry::LOADED)
		{
			OSG_INFO << "XML Plugin INFO: Plugin '" << libraryName << "', was loaded successfully." << std::endl;
			//now a library claiming to handle the type has been loaded, try to read again
			return GetXmlClassManagerForClassType(classType);

		}else if(result == osgDB::Registry::PREVIOUSLY_LOADED) {

			OSG_WARN << "XML Plugin WARN: Library '" << libraryName << "' has already been loaded, but isn't accepting ClassType '" << classType << "'." << std::endl; 
			return NULL;
		}else{
			//failed to load the libray
			OSG_WARN << "XML Plugin WARN: Failed to load Library '" << libraryName << "' ClassType '" << classType << "' will not be handled." << std::endl; 
			return NULL;
		}
	}	
	return NULL;
}

//
//Add an alias for a classtype to the library that will load it
void HogBoxRegistry::AddClassTypeAlias(const std::string mapClassType, const std::string toLibraryName)
{
    _classTypeAliasMap[mapClassType] = toLibraryName;
}

//
//Create a library name from a classtype name, including extension
//and folder hogboxXmlPlugins/
//
std::string HogBoxRegistry::CreateXmlLibraryNameForClassType(const std::string& classtype)
{
    std::string lowercase_classtype;
    for(std::string::const_iterator sitr=classtype.begin();
        sitr!=classtype.end();
        ++sitr)
    {
        lowercase_classtype.push_back(tolower(*sitr));
    }

	//see if the requested classtype is mapped to another class name
    ClassTypeAliasMap::iterator itr=_classTypeAliasMap.find(lowercase_classtype);
    if (itr!=_classTypeAliasMap.end() && classtype != itr->second) return CreateXmlLibraryNameForClassType(itr->second);

	//set folder prepend
#if defined(OSG_JAVA_BUILD)
    static std::string prepend = std::string("hogboxPlugins-")+std::string(hogboxGetVersion())+std::string("/java");
#else
    static std::string prepend = std::string("hogboxPlugins-")+std::string(hogboxGetVersion())+std::string("/");
#endif

	//get debug postfix for win32 plugins
#if defined(_DEBUG ) && defined(WIN32)
	static std::string postfix = std::string("d");
#else
	static std::string postfix = std::string("");
#endif


#if defined(__CYGWIN__)
    return prepend+"cygwin_"+"hogboxxml_"+lowercase_classtype+postfix+".dll";
#elif defined(__MINGW32__)
    return prepend+"mingw_"+"hogboxxml_"+lowercase_classtype+postfix+".dll";
#elif defined(WIN32)
    return prepend+"hogboxxml_"+lowercase_classtype+postfix+".dll";
#elif macintosh
    return prepend+"hogboxxml_"+lowercase_classtype+postfix;
#else
    return prepend+"hogboxxml_"+lowercase_classtype+postfix+".so";//ADDQUOTES(OSG_PLUGIN_EXTENSION);
#endif

}

HogBoxRegistry::DynamicLibraryList::iterator HogBoxRegistry::GetLibraryItr(const std::string& fileName)
{
    DynamicLibraryList::iterator ditr = _dlList.begin();
    for(;ditr!=_dlList.end();++ditr)
    {
        if ((*ditr)->getName()==fileName) return ditr;
    }
    return _dlList.end();
}

//
//Load a library, which should register an plugin of some sort
//
osgDB::Registry::LoadStatus HogBoxRegistry::LoadLibrary(const std::string& fileName)
{
   // OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(_pluginMutex);

    DynamicLibraryList::iterator ditr = GetLibraryItr(fileName);
	if (ditr!=_dlList.end()) return osgDB::Registry::PREVIOUSLY_LOADED;

    //_openingLibrary=true;

	osgDB::DynamicLibrary* dl = osgDB::DynamicLibrary::loadLibrary(fileName);
    
	//_openingLibrary=false;

    if (dl)
    {
        _dlList.push_back(dl);
        return osgDB::Registry::LOADED;
    }
    return osgDB::Registry::NOT_LOADED;
}

//
//
XmlClassWrapperRegistryProxy::XmlClassWrapperRegistryProxy(XmlClassWrapper* wrapper, const char* managerName)
{
    //check the registry instance
    HogBoxRegistry* registry = HogBoxRegistry::Inst();
    if(registry)
    {
        XmlClassManager* manager = registry->GetXmlClassManager(managerName);
        if(manager){
            manager->SupportsClassType(wrapper->getClassType(), wrapper);
        }
    }
}
