#include <hogboxHUD/HudRegion.h>

#include <osg/BlendEquation>
#include <osg/TexMat>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <hogbox/HogBoxUtils.h>
#include <hogbox/NPOTResizeCallback.h>

using namespace hogboxHUD;

HudRegion::HudRegion(bool isProcedural) 
	: osg::Object(),
	m_isProcedural(isProcedural),
	m_corner(osg::Vec2(0.0f,0.0f)),
	m_size(osg::Vec2(1.0f, 1.0f)),
	m_rotation(0.0f),
	//by default inherit all parent transforms
	m_transformInheritMask(INHERIT_ALL_TRANSFORMS),
	m_visible(true),
	m_stateChanged(false),
	m_depth(-0.5f),
	//Create our callbacks
	m_onMouseDownEvent(new CallbackEvent(this, "OnMouseDown")),
	m_onMouseUpEvent(new CallbackEvent(this, "OnMouseUp")),
	m_onMouseMoveEvent(new CallbackEvent(this, "OnMouseMove")),
	m_onMouseDragEvent(new CallbackEvent(this, "OnMouseDrag")),
	m_onDoubleClickEvent(new CallbackEvent(this, "OnDoubleClick")),
	m_onMouseEnterEvent(new CallbackEvent(this, "OnMouseEnter")),
	m_onMouseLeaveEvent(new CallbackEvent(this, "OnMouseLeave")),
	//keyboard events
	m_onKeyDownEvent(new CallbackEvent(this, "OnKeyown")),
	m_onKeyUpEvent(new CallbackEvent(this, "OnKeyUp"))
{

	m_root = new osg::MatrixTransform(osg::Matrix::identity());
	m_translate = new osg::MatrixTransform();
	m_rotate = new osg::MatrixTransform();
	m_scale = new osg::MatrixTransform();
	m_region = new osg::Group();
	m_childMount = new osg::MatrixTransform();

	//build the transform hierachy	
	m_root->addChild(m_translate);
	m_translate->addChild(m_rotate.get());
	m_rotate->addChild(m_scale.get());
	//attach region node for rendering
	m_scale->addChild(m_region.get());	

	//attach the child regions to rotate, so children
	//are affected by this regions translate and rotate
	//transforms directly, but scale is applied indepenatly
	//to allow for different resize modes etc
	m_rotate->addChild(m_childMount.get());
	
	m_stateset = new osg::StateSet(); 
	m_stateset->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
    //stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	m_region->setStateSet(m_stateset.get()); 

	m_material = new osg::Material();
	m_material->setColorMode(osg::Material::OFF);
	m_material->setDataVariance(osg::Object::DYNAMIC);
	m_stateset->setAttributeAndModes(m_material, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	SetColor(osg::Vec3(1.0f, 1.0f, 1.0f));
	SetAlpha(1.0f);
	EnableAlpha(false);

	//no parent by default
	m_p_parent=NULL;

	m_hovering = false;
}

/** Copy constructor using CopyOp to manage deep vs shallow copy.*/
HudRegion::HudRegion(const HudRegion& region,const osg::CopyOp& copyop)
	: osg::Object(region, copyop),
	m_corner(region.m_corner),
	m_size(region.m_size),
	m_rotation(region.m_rotation),
	m_visible(region.m_visible),
	m_stateChanged(region.m_stateChanged),
	m_depth(region.m_depth),
	m_color(region.m_color),
	m_alpha(region.m_alpha),
	m_alphaEnabled(region.m_alphaEnabled)
{
}

HudRegion::~HudRegion(void)
{
	for(unsigned int i=0; i<m_p_children.size(); i++)
	{
		m_childMount->removeChild(m_p_children[i]->GetRegion());
		m_p_children[i]=NULL;
	}
	m_p_children.clear();
	m_root = NULL;
	m_translate = NULL;
	m_rotate = NULL;
	m_scale = NULL;

	m_material = NULL;
}

//
// Create the region of size, in postion using a loaded model
//
bool HudRegion::Create(osg::Vec2 corner, osg::Vec2 size, const std::string& fileName)
{

	//load the assests that represent this region
	//i.e. geometry and textures and apply to the region node
	//for rendering
	this->LoadAssest(fileName); 

	//make the region identifiable by the picker
	//by setting the regions geometry names to the
	//unique ID
	setName(this->getName()); 

	//Set the regions position, size and rotation
	SetPosition(corner);SetSize(size);
	
	return true;
}

//
// Returns the root transform matrix
//
osg::MatrixTransform* HudRegion::GetRegion()
{
	return m_root.get();
}

//
// 0 not used, 1 used by this, 2 used by child
//
int HudRegion::HandleInputEvent(HudInputEvent& hudEvent)
{
	int ret = 0;
	
	//check for basic event and inform our callbacks if detected
	osg::notify(osg::WARN) << "EVENT" << std::endl;
	//handle the event type
    switch(hudEvent.GetEventType())
    {
		//key is pressed down
		case(ON_KEY_DOWN):
        {
			m_onKeyDownEvent->TriggerEvent(hudEvent);
			osg::notify(osg::WARN) << "KEY DOWN" << std::endl;
			break;
        }
			
		//key released
		case(ON_KEY_UP):
        {
			m_onKeyUpEvent->TriggerEvent(hudEvent);
			osg::notify(osg::WARN) << "KEY UP" << std::endl;
			break;
		}
			
		//mouse moving
		case(ON_MOUSE_MOVE):
        {
			//trigger our onMouseDown event
			m_onMouseMoveEvent->TriggerEvent(hudEvent);
			osg::notify(osg::WARN) << "MOUSE MOVE" << std::endl;
			break;
		}
			
		//mouse drag (moving with button held)
		case(ON_MOUSE_DRAG):
        {
			//trigger our onMouseDrag event
			m_onMouseDragEvent->TriggerEvent(hudEvent);
			osg::notify(osg::WARN) << "MOUSE DRAG" << std::endl;
			break;
        } 
			
		//mouse down
		case(ON_MOUSE_DOWN):
		{
			//trigger our onMouseDown event
			m_onMouseDownEvent->TriggerEvent(hudEvent);
			osg::notify(osg::WARN) << "MOUSE DOWN" << std::endl;
			break;
		}
			
		//mouse up
		case(ON_MOUSE_UP):
        {
			//trigger our onMouseUp event
			m_onMouseUpEvent->TriggerEvent(hudEvent);
			osg::notify(osg::WARN) << "MOUSE UP" << std::endl;
			break;
        } 
			
		//double click do down and up
		case(ON_DOUBLE_CLICK):
        {
			osg::notify(osg::WARN) << "DOUBLE CLICK" << std::endl;
			m_onDoubleClickEvent->TriggerEvent(hudEvent);
			break;
        } 
			
		//mouse enter
		case(ON_MOUSE_ENTER):
        {
			//trigger our onMouseUp event
			m_onMouseEnterEvent->TriggerEvent(hudEvent);
			osg::notify(osg::WARN) << "MOUSE ENTER" << std::endl;
			break;
        } 
			
		//mouse leave
		case(ON_MOUSE_LEAVE):
        {
			//trigger our onMouseUp event
			m_onMouseEnterEvent->TriggerEvent(hudEvent);
			osg::notify(osg::WARN) << "MOUSE LEAVE" << std::endl;
			break;
        } 
		default:break;
	}
	//hudEvent

	return ret;
}

//
//adds a child to this region which will then be transformed relative
// to the parent
//
void HudRegion::AddChild(HudRegion* region)
{
	m_childMount->addChild(region->GetRegion());
	m_p_children.push_back(region); 

	region->SetParent(this); 
}


//
// Checks if the name equal one of or children
//
bool HudRegion::IsChild(const std::string& uniqueID)
{
	for(unsigned int i=0; i<m_p_children.size(); i++)
	{
		if(uniqueID.compare( m_p_children[i]->getName()) == 0)
		{
			return true;
		}else{
			if(m_p_children[i]->IsChild(uniqueID))
			{return true;}
		}
	}
	return false;
}

//
// Pass the event on to all the children regions of this region
// example, a diloag region passes to it button to animate the button
// returns true if used by one of the children
//
bool HudRegion::HandleChildEvents(HudInputEvent& hudEvent)
{
	bool l_ret=false;
	//loop through all the children
	for(unsigned int i=0; i<m_p_children.size(); i++)
	{
		if( m_p_children[i]->HandleInputEvent(hudEvent) != 0)
		{l_ret = true;}
	}
	return l_ret;
}


//
//Load assest takes a folder name containing our assest
//each region type expects to find specificlty named assests in the folder
//Base implementation loads 
//geom.ive, used as render geometry (if not present then a quad is generated)
//base.png, used as the default texture
//rollover.png, used for mouse rollovers
//
bool HudRegion::LoadAssest(const std::string& folderName)
{
	//if it has no name, then no assets are needed
	if(folderName.size() == 0)
	{return true;}

	//try to load the file as an ive file
	osg::Node* geometry = osgDB::readNodeFile(folderName+"/geom.ive");
	if(geometry==NULL)
	{
		//create an xy quad of size 1,1 in its place
		osg::Geometry* geom = osg::createTexturedQuadGeometry(osg::Vec3(0.0f,0.0f,0.0f), 
															osg::Vec3(1.0f,0.0f,0.0f),
															osg::Vec3(0.0f, 1.0f, 0.0f));
		geom->setColorArray(NULL);//don't use color array as we set color via gl material
		osg::Geode* geode = new osg::Geode();
		geode->addDrawable(geom);
		m_region->addChild(geode);

	}else{
		//attach the geomtry to the region node
		m_region->addChild(geometry);
	}

	//now try to load a base texture
	std::string baseTextureFile = osgDB::findDataFile( folderName+"/base.png" );
	if(baseTextureFile.empty()){baseTextureFile = osgDB::findDataFile( folderName+"/base.bmp" );}
	//if(osgDB::fileExists(baseTextureFile) )
	{
		m_baseTexture = hogbox::LoadTexture2D(baseTextureFile);
		if(m_baseTexture.get())
		{
            m_baseTexture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
            m_baseTexture->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
            m_baseTexture->setResizeNonPowerOfTwoHint(false);
			
			//apply the base as default
			this->ApplyTexture(m_baseTexture.get());
		}
	}

	//rollover
	std::string rollOverTextureFile = folderName+"/rollover.png";
	if(osgDB::fileExists(rollOverTextureFile) )
	{m_rollOverTexture = hogbox::LoadTexture2D(rollOverTextureFile);}

	return true;
}

//
//Set the id and rename any asset geodes
//
void HudRegion::setName(const std::string& name)
{
	m_region->setName(name); 
	osg::Object::setName(name);
	MakeHudGeodes(m_region.get(), this);
}

//positioning

void HudRegion::SetPosition(const osg::Vec2& corner)
{
	m_corner = corner;
	m_translate->setMatrix( osg::Matrix::translate(osg::Vec3(	corner.x(),
																corner.y(), 
																m_depth
															))); 
}

const osg::Vec2& HudRegion::GetPosition() const
{
	return m_corner;
}

//
//get set roation around the z axis in degrees
//
void HudRegion::SetRotation(const float& rotate)
{
	m_rotation = rotate;
	m_rotate->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(m_rotation), osg::Vec3(0.0f,0.0f,1.0f)));
}

const float& HudRegion::GetRotation() const
{
	return m_rotation;
}

void HudRegion::SetSize(const osg::Vec2& size)
{
	//resize this regions geometry
	m_scale->setMatrix( osg::Matrix::scale(osg::Vec3(size.x(), size.y(), 1.0f)));
	
	//resize children using the relative scale difference
	//between old and new size
	float xScaler = size.x()/m_size.x();
	float yScaler = size.y()/m_size.y();

	//then scale the relative position of any children
	for(unsigned int i=0; i<m_p_children.size(); i++)
	{
		//check if the child want to inherit
		if((m_p_children[i]->GetTransformInheritMask() & INHERIT_SIZE))
		{m_p_children[i]->SetSizeFromPercentage(xScaler, yScaler);}

		if((m_p_children[i]->GetTransformInheritMask() & INHERIT_POSITION))
		{m_p_children[i]->SetPositionFromPercentage(xScaler, yScaler);}
	}

	//store new size
	m_size = size;
}

const osg::Vec2& HudRegion::GetSize() const
{
	return m_size;
}

//
//set the size as a percentage/scale factor of the current size 
//i.e. <1 smaller, >1 bigger
//
void HudRegion::SetSizeFromPercentage(float xScaler, float yScaler)
{
	osg::Vec2 newSize;
	newSize.x() = m_size.x()*xScaler;
	newSize.y() = m_size.y()*yScaler;
	this->SetSize(newSize);
}

//
//set the position as a percentage/scale factor of the current position
//i.e. <1 smaller, >1 bigger
//
void HudRegion::SetPositionFromPercentage(float xScaler, float yScaler)
{
	osg::Vec2 newPos;
	newPos.x() = m_corner.x()*xScaler;
	newPos.y() = m_corner.y()*yScaler;
	this->SetPosition(newPos);
}

//
//move to a new layer
//

void HudRegion::SetLayer(const float& depth)
{
	osg::Vec2 pos = GetPosition();
	m_translate->setMatrix( osg::Matrix::translate( osg::Vec3(pos.x(),pos.y(),depth)));
	m_depth = depth;
}

const float& HudRegion::GetLayer() const
{
	return m_depth;
}


const bool& HudRegion::IsVisible() const
{
	return m_visible;
}

void HudRegion::SetVisible(const bool& visible)
{
	//set hud to invisable as default
	if(visible)
	{
		m_root->setNodeMask(0xFFFFFFFF);
	}else{
		m_root->setNodeMask(0x0);
	}
	m_visible=visible;
}

//
//set the texture used by default
//
void HudRegion::SetBaseTexture(osg::Texture* texture)
{
	m_baseTexture = NULL;
	m_baseTexture = texture;
}
//
//Set the texture used as rollover
//
void HudRegion::SetRolloverTexture(osg::Texture* texture)
{
	m_rollOverTexture = NULL;
	m_rollOverTexture = texture;
}

//
//Apply the texture to the channel 0/diffuse
//
void HudRegion::ApplyTexture(osg::Texture* tex)
{
	osg::StateSet* stateset = m_region->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0,tex,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	
	//NOTE@tom, below isn't needed on platforms supporting glu
	//apply a non power of two rezie callback if require
	osg::ref_ptr<hogbox::NPOTResizeCallback> resizer = new hogbox::NPOTResizeCallback(tex, 0, stateset);
	//if the texture casts as a rect apply the tex rect scaling to texture coords
	osg::Texture2D* tex2D = dynamic_cast<osg::Texture2D*> (tex); 
	if(tex2D){if(resizer->useAsCallBack()){tex2D->setSubloadCallback(resizer.get());}}
}

void HudRegion::ApplyBaseTexture()
{
	this->ApplyTexture(this->m_baseTexture);
}

void HudRegion::ApplyRollOverTexture()
{
	this->ApplyTexture(this->m_rollOverTexture);
}

//
//set the material color of the region
//
void HudRegion::SetColor(const osg::Vec3& color)
{
	m_color = color;

	osg::Vec4 vec4Color = osg::Vec4(color, m_alpha);

	//set the materials color
	m_material->setAmbient(osg::Material::FRONT_AND_BACK, vec4Color ); 
	m_material->setDiffuse(osg::Material::FRONT_AND_BACK, vec4Color );
	m_material->setSpecular(osg::Material::FRONT_AND_BACK, vec4Color );
	m_material->setShininess(osg::Material::FRONT_AND_BACK, 128);
	m_material->setEmission(osg::Material::FRONT_AND_BACK, vec4Color);
	 

	//call for children
	for(unsigned int i=0; i<m_p_children.size(); i++)
	{
		m_p_children[i]->SetColor(color); 
	}
}

const osg::Vec3& HudRegion::GetColor() const
{
	return m_color;
}


void HudRegion::SetAlpha(const float& alpha)
{
	m_alpha = alpha;

	//set the materials alpha
	m_material->setAlpha(osg::Material::FRONT_AND_BACK, m_alpha);

	//call for children
	for(unsigned int i=0; i<m_p_children.size(); i++)
	{
		m_p_children[i]->SetAlpha(alpha); 
	}
}

const float& HudRegion::GetAlpha() const
{
	return m_alpha;
}

//
//get set enable alpha
//
void HudRegion::EnableAlpha(const bool& enable)
{
	if(enable)
	{
		osg::BlendEquation* blendEquation = new osg::BlendEquation(osg::BlendEquation::FUNC_ADD);
		m_stateset->setAttributeAndModes(blendEquation,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
		//tell to sort the mesh before displaying it
		m_stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		m_stateset->setMode(GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE); 

	}else{
		m_stateset->setMode(GL_BLEND, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	}
	m_alphaEnabled = enable;
}

const bool& HudRegion::IsAlphaEnabled() const
{
	return m_alphaEnabled;
}

void HudRegion::SetStateSet(osg::StateSet* stateSet)
{
	if(m_region.get()!=NULL)
	{
		m_region->setStateSet(stateSet);
	}
}

osg::StateSet* HudRegion::GetStateSet()
{
	if(m_region.get()==NULL)
	{return NULL;}

	return m_region->getOrCreateStateSet();
}


//state changes
bool HudRegion::StateChanged()
{
	if(m_stateChanged)
	{
		m_stateChanged=false;
		return true;
	}
	return false;
}


//
//convert corrds into the regions local system with corner at the origin
//
osg::Vec2 HudRegion::GetRegionSpaceCoords(osg::Vec2 spCoords)
{
	//get the vector from corner to spCoord
	osg::Vec2 coordVec = spCoords-GetAbsoluteCorner();
	//rotate the coord vec into local space
	float invRot = -osg::DegreesToRadians(GetAbsoluteRotation());
	osg::Vec2 regionSpaceCoord;
	regionSpaceCoord.x() = cos(invRot)*coordVec.x() - sin(invRot)*coordVec.y();
	regionSpaceCoord.y() = sin(invRot)*coordVec.x() + cos(invRot)*coordVec.y();
	return regionSpaceCoord;
}

//
//recersive through parents to get screen space corner
//
osg::Vec2 HudRegion::GetAbsoluteCorner()
{
	osg::Vec2 ret = m_corner;
	if(m_p_parent != NULL)
	{
		//now rotate our corner by the parents rotation
		float rot = osg::DegreesToRadians(m_p_parent->GetRotation());
		osg::Vec2 oldCoord = ret;
		ret.x() = cos(rot)*oldCoord.x() - sin(rot)*oldCoord.y();
		ret.y() = sin(rot)*oldCoord.x() + cos(rot)*oldCoord.y();
		ret+=m_p_parent->GetAbsoluteCorner();
	}
	return ret;
}

//recurse through parents and find the regions absolute rotation
float HudRegion::GetAbsoluteRotation()
{
	float ret = m_rotation;
	if(m_p_parent != NULL)
	{
		//now rotate our corner by the parents rotation
		ret+=m_p_parent->GetAbsoluteRotation();
	}
	return ret;
}

void HudRegion::TriggerStateChanged()
{
	m_stateChanged = true;
}

//
//add an updatecallback, this is done by creating a new node
//attaching the update callback to it, then attaching the new node
//to the root for updating
//
bool HudRegion::AddUpdateCallback(osg::NodeCallback* callback)
{
	//create the new node to handle the callback
	osg::Node* callbackNode = NULL;
	callbackNode = new osg::Node();
	if(callbackNode)
	{
		callbackNode->setUpdateCallback(callback);
		if(m_root){m_root->addChild(callbackNode);}
	}else{
		return false;
	}
	return true;	
}

void HudRegion::SetChildrenList(const HudRegionList& list)
{
	for(unsigned int i=0; i<list.size(); i++)
	{
		this->AddChild(list[i]);
	}
}


//
//Funcs to register event callbacks
void HudRegion::AddOnMouseDownCallbackReceiver(HudEventCallback* callback)
{
	m_onMouseDownEvent->AddCallbackReceiver(callback);
}

void HudRegion::AddOnMouseUpCallbackReceiver(HudEventCallback* callback)
{
	m_onMouseUpEvent->AddCallbackReceiver(callback);
}

void HudRegion::AddOnMouseMoveCallbackReceiver(HudEventCallback* callback)
{
	m_onMouseMoveEvent->AddCallbackReceiver(callback);
}

void HudRegion::AddOnMouseDragCallbackReceiver(HudEventCallback* callback)
{
	m_onMouseDragEvent->AddCallbackReceiver(callback);
}

void HudRegion::AddOnDoubleClickCallbackReceiver(HudEventCallback* callback)
{
	m_onDoubleClickEvent->AddCallbackReceiver(callback);
}

void HudRegion::AddOnMouseEnterCallbackReceiver(HudEventCallback* callback)
{
	m_onMouseEnterEvent->AddCallbackReceiver(callback);
}

void HudRegion::AddOnMouseLeaveCallbackReceiver(HudEventCallback* callback)
{
	m_onMouseLeaveEvent->AddCallbackReceiver(callback);
}

//keyboard
void HudRegion::AddOnKeyDownCallbackReceiver(HudEventCallback* callback)
{
	m_onKeyDownEvent->AddCallbackReceiver(callback);
}

void HudRegion::AddOnKeyUpCallbackReceiver(HudEventCallback* callback)
{
	m_onKeyUpEvent->AddCallbackReceiver(callback);
}


//helper func to rename geodes and attach user data
void hogboxHUD::MakeHudGeodes(osg::Node* node, HudRegion* region)
{
	//find geomtry in geodes
	if(dynamic_cast<osg::Geode*> (node))
	{
		//loop all geometry nodes
		osg::Geode* geode = static_cast<osg::Geode*> (node);
		geode->setName(region->getName()) ;
		geode->setUserData(region);
	}
	
	// Traverse any group node
	if(dynamic_cast<osg::Group*> (node))
	{
		osg::Group* group = static_cast<osg::Group*> (node);
		for(unsigned int i=0; i < group->getNumChildren(); i++)
		{	hogboxHUD::MakeHudGeodes(group->getChild(i), region);}
	}
}
