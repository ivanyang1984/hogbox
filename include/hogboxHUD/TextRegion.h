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

#pragma once

#include <hogboxHUD/StrokeRegion.h>

#include <osg/Group>
#include <osg/Depth>
#include <osgText/Text>

namespace hogboxHUD {


//
//TextRegion
//
class HOGBOXHUD_EXPORT TextRegion : public StrokeRegion
{
public:
    enum TEXT_ALIGN{
        NO_ALIGN_SET=0, 
        CENTER_ALIGN=1,
        RIGHT_ALIGN=2,
        LEFT_ALIGN=3
    };
    
    enum BACKDROP_TYPE{
        NO_BACKDROP_SET=0,
        NO_BACKDROP=1,
        DROP_SHADOW=2,
        STROKE=3
    };
    
    class TextRegionStyle : public StrokeRegionStyle{
    public:
        TextRegionStyle()
            : StrokeRegion::StrokeRegionStyle(),
            _label(""),
            _fontHeight(-1.0f),//indicates to comupte from region height
            _fontName("Fonts/arial.ttf"),
            _boarderPadding(5.0f),
            _alignmentMode(CENTER_ALIGN),
            _textColor(osg::Vec4(0.1f,0.1f,0.1f,1.0f)),
            _backdropType(NO_BACKDROP),
            _backdropColor(osg::Vec4(0.1f,0.1f,0.1f,0.7f))
        {
        }
        
        TextRegionStyle(const TextRegionStyle& args)
            : StrokeRegion::StrokeRegionStyle(args),
            _label(args._label),
            _fontHeight(args._fontHeight),
            _fontName(args._fontName),
            _boarderPadding(args._boarderPadding),
            _alignmentMode(args._alignmentMode),
            _textColor(args._textColor),
            _backdropType(args._backdropType),
            _backdropColor(args._backdropColor)
        {
        }
        
        std::string _label;
        float _fontHeight;
        //the name of the font being used
        std::string _fontName;
        //the padding to left or right of text
        float _boarderPadding;
        //the alignment mode for the text
        //relative to the local region space
        TEXT_ALIGN _alignmentMode;
        //the color of our text, including alpha
        //which should match the region alpha
        osg::Vec4 _textColor;
        //is the drop shadow being rendered
        BACKDROP_TYPE _backdropType;
        //color of drop shadow
        osg::Vec4 _backdropColor;
        
    protected:
        virtual ~TextRegionStyle(){
        }
    };
    
    //
    TextRegion(TextRegionStyle* style = new TextRegionStyle());
    
    TextRegion(osg::Vec2 corner, osg::Vec2 size, TextRegionStyle* style = new TextRegionStyle());
    
    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    TextRegion(const TextRegion& region,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
    
    META_Object(hogboxHUD,TextRegion);
    virtual RegionStyle* allocateStyleType(){return new TextRegionStyle();}
    
    //
    //overload create to allocate TextRegionStyle by default
    virtual bool Create(osg::Vec2 corner, osg::Vec2 size, RegionStyle* style = new TextRegionStyle());
    
    //
    //Convenience method to create a TextRegionStyle with label etc, which
    //is then passed to base Create
    virtual bool CreateWithLabel(osg::Vec2 corner, osg::Vec2 size, const std::string& asset, const std::string& label);
    
    
    virtual int HandleInputEvent(HudInputEvent& hudEvent);
    
    
    //overload set position
    virtual void SetPosition(const osg::Vec2& corner);
    
    //overload SetSize so we can set the texts pixelheight independantly
    virtual void SetSize(const osg::Vec2& size);
    
    //overload set alpha so we can also adust text color alpha
    virtual void SetAlpha(const float& alpha);
    
    //set the actual text string,
    virtual void SetText(const std::string& str);
    //return the current text vaule
    const std::string& GetText() const;
    
    //set font height in pixels
    void SetFontHeight(const float& fontHeight);
    const float& GetFontHeight() const;
    
    //set the fonts glyph res in pixels
    void SetFontResolution(const osg::Vec2& fontRes);
    
    //set the texts font type
    void SetFontType(const std::string& fontFile);
    const std::string& GetFontType() const;
    
    //set the number of pixels between
    //the edge of the text and the edge of the
    //region when in left or right alignments
    const float& GetBoarderPadding() const;
    void SetBoarderPadding(const float& padding);
    
    //Set the text alignment mode
    void SetAlignment(const TEXT_ALIGN& alignment);
    const TEXT_ALIGN& GetAlignment() const;
    
    //set the text font color
    void SetTextColor(const osg::Vec4& color);
    const osg::Vec4& GetTextColor() const;
    
    //set the drop shadow color
    void SetBackDropColor(const osg::Vec4& color);
    const osg::Vec4& GetBackDropColor() const;
    
    //enable/disable the texts drop shadow/stroke
    void SetBackDropType(const BACKDROP_TYPE& type);
    const BACKDROP_TYPE& GetBackDropType() const;
    
    //bit hacky but get/set the backdrop type using int
    //to conform to xml serialising requirements
    void SetBackDropTypeInt(const int& type);
    const int& GetBackDropTypeInt() const;
    
public:
    
    //
    //Funcs to register event callbacks
    void AddOnTextChangedCallbackReceiver(HudEventCallback* callback);
    
protected:
    
    virtual ~TextRegion(void);
    
    //
    //Text region loads the aditional assests
    //searches for a ttf file to use as a font
    virtual bool LoadAssest(RegionStyle* args);
    
protected:
    
    //the string to display
    std::string _string;
    
    //text displayed in the region
    osg::ref_ptr<osgText::Text> _text;
    
    //linear scale matrix for text
    osg::ref_ptr<osg::MatrixTransform> _textScale;
    
    //heigth of font in pixels/hud space
    float _fontHeight;
    
    //when text region is created, its height is used as a ref to scale text
    float _scaleHeightRef;
    
    //the name of the font being used
    std::string _fontName;
    
    //the padding to left or right of text
    float _boarderPadding;
    
    //the alignment mode for the text
    //relative to the local region space
    TEXT_ALIGN _alignmentMode;
    
    //the color of our text, including alpha
    //which should match the region alpha
    osg::Vec4 _textColor;
    
    //is the drop shadow being rendered
    BACKDROP_TYPE _backdropType;
    
    //color of drop shadow
    osg::Vec4 _backdropColor;
    
    //Callback events
    
    //on text changed called each time SetText is called to inform receivers of the change
    osg::ref_ptr<HudCallbackEvent> _onTextChangedEvent;
};

typedef osg::ref_ptr<TextRegion> TextRegionPtr;
};
