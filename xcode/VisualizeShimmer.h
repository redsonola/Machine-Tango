#include "cinder/gl/Vbo.h"
#include "cinder/Camera.h"
#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "cinder/Text.h"
#include "cinder/Vector.h"

#define TRANS_INC 0.5f

class VisualizeShimmer
{
private: 
	std::vector<ci::vec3>	vData;
	ci::vec3				transVector;
	ci::ColorA				color;
	std::string				port;
	ci::TextLayout			txt; 
	ci::vec2				bounds; 

public: 
	VisualizeShimmer();
	virtual void update(std::vector<ShimmerData *> data);
	virtual void draw(); 
	inline void setColor(ci::ColorA c){ color = c; }; 
	void setPort(std::string p)
	{ 
		port = p; 
		std::stringstream ss; ss << "COM " << p; 
		txt.setColor(color);
		txt.setFont(ci::Font("Arial", 10));
        txt.addLine(ss.str());
	};
	inline void setTranslationVector(ci::vec3 tr){transVector = tr;};
	void translateUp(){ transVector.y += TRANS_INC; };
	void translateDown(){ transVector.y -= TRANS_INC; };
	void translateLeft(){ transVector.x -= TRANS_INC; };
	void translateRight(){ transVector.x += TRANS_INC; };
	inline void setBounds(ci::vec2 b){ bounds = b; };
};
VisualizeShimmer::VisualizeShimmer()
{
	color = ci::ColorA(0, 1, 0, 1); //random initial color, not really random, green.
    transVector = ci::vec3(0);
}
void VisualizeShimmer::update(std::vector<ShimmerData *> data)
{
	vData.clear();
	for (int i = 0; i < data.size(); i++)
	{
		ci::vec3 pos = data[i]->getAccelData();
        if( pos.x != NO_DATA ) //for handling android data
        {
            pos.x = pos.x / G_FORCE;
            pos.y = pos.y / G_FORCE;
            pos.z = pos.z / G_FORCE;

            
            //TODO -- fix constants??
            pos.x = pos.x * 6 - 3;
            pos.y = pos.y * 6 - 3;
            pos.z = pos.z * 6 - 3;

            pos = pos + transVector;
            vData.push_back(pos);
        }
	}
}
void VisualizeShimmer::draw()
{
	ci::gl::color(color);
	ci::gl::drawSphere(transVector, 0.05f);
    ci::vec2 pos = ci::vec2(transVector.x,transVector.y) ; pos.y -= 0.1;

	for (int i = 0; i < vData.size(); i++)
	{
		ci::gl::drawSphere(vData[i], 0.1f);
	}

	//ci::gl::setMatricesWindow(bounds);
	//ci::gl::enableAlphaBlending(false);
	//ci::gl::Texture tex = ci::gl::Texture( txt.render(true, false) );
	//pos.x = ((pos.x+1.0)/2.0) * bounds.x;
	//pos.y = ((pos.y + 1.0) / 2.0) * bounds.y;
	//ci::gl::draw(tex, pos);
	//ci::gl::disableAlphaBlending();

}
