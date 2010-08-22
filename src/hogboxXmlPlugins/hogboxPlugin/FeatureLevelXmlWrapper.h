#pragma once

#include <hogbox/SystemInfo.h>
#include <hogboxDB/XmlClassWrapper.h>


//
//Xml wrapper for osg::Node, used by OsgNodeManager
//Nodes in this context are entire sub graphs representing
//a 3d model, probably exported from 3ds max etc.
//Becuase they are always groups the contructor allocates
//and osg::Group to store the node.
//
//Xml Definition: 
//<Node uniqueID='MyNode1'>
//	<File>MyModelFile.ive</File>
//</Node>
//
//Nodes can use several methods for loading data, but the base
//Node class has no methods to store the data (i.e. fileName)
//OsgNodeXmlWrapper provides the means of temporarily storing
//info for conveniance load functions
//
class FeatureLevelXmlWrapper : public hogboxDB::XmlClassWrapper
{
public:


	//pass node representing the texture object
	FeatureLevelXmlWrapper(osgDB::XmlNode* node) 
			: hogboxDB::XmlClassWrapper(node, "FeatureLevel")
	{

		//allocate Group to repesent any loaded nodes
		hogbox::SystemFeatureLevel* featureLevel = new hogbox::SystemFeatureLevel();

		//add the temporary wrapper attributes
		m_xmlAttributes["GLVersion"] = new hogboxDB::TypedXmlAttribute<float>(&featureLevel->glVersion);

		m_xmlAttributes["GLSLVersion"] = new hogboxDB::TypedXmlAttribute<float>(&featureLevel->glslVersion);

		m_xmlAttributes["TextureUnits"] = new hogboxDB::TypedXmlAttribute<int>(&featureLevel->textureUnits);

		m_xmlAttributes["TextureCoordUnits"] = new hogboxDB::TypedXmlAttribute<int>(&featureLevel->textureCoordUnits);

		m_xmlAttributes["VertexAndFragmentShader"] = new hogboxDB::TypedXmlAttribute<bool>(&featureLevel->vertexAndFragmentShaders);

		m_xmlAttributes["GeometryShader"] = new hogboxDB::TypedXmlAttribute<bool>(&featureLevel->geometryShaders);

		m_xmlAttributes["ScreenResolution"] = new hogboxDB::TypedXmlAttribute<osg::Vec2>(&featureLevel->screenRes);


		p_wrappedObject = featureLevel;
	}

public:

protected:

	virtual ~FeatureLevelXmlWrapper(void){}

};

typedef osg::ref_ptr<FeatureLevelXmlWrapper> SystemFeatureLevelXmlWrapperPtr;


