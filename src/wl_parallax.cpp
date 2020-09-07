#include "c_cvars.h"
#include "g_mapinfo.h"
#include "id_ca.h"
#include "wl_def.h"
#include "wl_agent.h"
#include "wl_play.h"
#include "wl_main.h"
#include "wl_draw.h"

extern fixed viewz;
extern int viewshift;

// Fill in a span of pixels. Could be potentially have a POT version but not
// sure if it's worthwhile.
static void DrawParallaxPlaneLoop(byte *vbuf, unsigned vbufPitch,
	const byte* src, int yshift, fixed h, fixed yStep, int y, int yend)
{
	vbuf += y*vbufPitch;

	for(fixed ty = (((y + yshift)) * yStep) % h; y < yend; vbuf += vbufPitch, ++y)
	{
		*vbuf = src[ty>>FRACBITS];

		if((ty += yStep) > h)
			ty -= h;
	}
}

// Draws one of the two sky planes: above or below wallheight
template<bool ceiling>
static void DrawParallaxPlane(byte *vbuf, unsigned vbufPitch,
	FTexture *skysource, int yshift,
	int midangle, fixed heightFactor, int skyheight, int skyscaledheight)
{
	const int w = skysource->GetWidth();
	const fixed h = (skysource->GetHeight())<<FRACBITS;
	const fixed yStep = h/skyscaledheight;

	int curtex = -1;
	const byte *skytex = NULL;
	for(int x = 0; x < viewwidth; x++)
	{
		int curang = pixelangle[x] + midangle;
		if(curang < 0) curang += FINEANGLES;
		else if(curang >= FINEANGLES) curang -= FINEANGLES;
		const int xtex = (FINEANGLES - curang - 1) * w / FINEANGLES;
		if(xtex != curtex)
		{
			curtex = xtex;

			skytex = skysource->GetColumn(xtex, NULL);
		}

		if(ceiling)
		{
			int yend = skyheight - ((wallheight[x]*heightFactor)>>FRACBITS);
			if(yend <= 0)
				continue;
			if(yend >= viewheight)
				yend = viewheight;

			DrawParallaxPlaneLoop(vbuf+x, vbufPitch, skytex, yshift, h, yStep, 0, yend);
		}
		else
		{
			int ystart = skyheight + ((wallheight[x]*heightFactor)>>FRACBITS);
			if(ystart < 0)
				ystart = 0;

			DrawParallaxPlaneLoop(vbuf+x, vbufPitch, skytex, yshift, h, yStep, ystart, viewheight);
		}
	}
}

void DrawParallax(byte *vbuf, unsigned vbufPitch)
{
	if(!levelInfo->ParallaxSky.isValid())
		return;

	const int midangle = (players[ConsolePlayer].camera->angle)>>ANGLETOFINESHIFT;
	const int skyheight = (viewheight >> 1) - viewshift;
	// We want to map the sky onto the upper and lower 100 pixels of the 320x200
	// canvas.  So we can use the psprite scale variables to determine the size.
	const int skyscaledheight = (100*pspriteyscale)>>FRACBITS;

	// Determines the offset to y when determining texel
	int yshift = skyscaledheight - (viewheight>>1) + viewshift;
	if(yshift < 0)
		yshift = skyscaledheight-((-yshift)%skyscaledheight);

	fixed planeheight = viewz+(map->GetPlane(0).depth<<FRACBITS);
	const fixed heightFactor = abs(planeheight)>>8;

	FTexture * const skysource = TexMan(levelInfo->ParallaxSky);

	DrawParallaxPlane<true>(vbuf, vbufPitch, skysource, yshift, midangle, heightFactor, skyheight, skyscaledheight);
	DrawParallaxPlane<false>(vbuf, vbufPitch, skysource, yshift, midangle, heightFactor, skyheight, skyscaledheight);
}
