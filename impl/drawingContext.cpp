#include "drawingContext.h"
#include <algorithm>
#include <iostream>

namespace ctxgraf {
	
	void DrawingContext::polyline(ISurface * drawingSurface, const Vertex * vertices, uint32_t vertexCount) const {
		Vertex vA; //creating variable vA of type Vertex
		Vertex vB; //creating variable vB of type Vertex
		for (int i = 0; i < vertexCount - 1; i++)
			//loop will iterate until it hits the # of vertexes - 1
			drawLine(drawingSurface, vertices[i], vertices[i + 1]); //calls drawline with paramaters drawingSurface, vertices[i], and vertices[i+1]

	}

	void DrawingContext::drawLine(ISurface * drawingSurface, Vertex vA, Vertex vB) const {
		//rasterization and interpolation stuff here

		vA.x = (int)((drawingSurface->getWidth() - 1) * ((vA.x + 1.0) / 2.0)); vA.y = (int)((drawingSurface->getWidth() - 1) * ((vA.y + 1.0) / 2.0)); //All this does is connect lines together. Vertice to vertice
		vB.x = (int)((drawingSurface->getWidth() - 1) * ((vB.x + 1.0) / 2.0)); vB.y = (int)((drawingSurface->getWidth() - 1) * ((vB.y + 1.0) / 2.0));

		Vertex stepVertex; //creating stepVertex of type Vertex
		Vertex lastPosition; //creating lastPostion of type Vertex
		Vertex currentPosition; //creating currentPostion of type Vertex.
		float dX = vA.x - vB.x; //difference between the vertices in x
		float dY = vA.y - vB.y; //difference between vertices in y
		float majorStart; //start of major axis
		float minorStart; //start of minor axis
		float majorEnd; //end of major axis
		float minorEnd; //end of minor axis
		float minorSlope; // minor slope
		float majorDirection; //direction of the loop
		float lastValue; //creating lastValue of type float
		float currentValue; //creating currentValue of type float
		bool drawPixel; //creating drawPixel of type bool

		if (fabs(dX) > fabs(dY)) {
			/*this will determine which axis is the major axis
			if the major axis = x */
			majorStart = vA.x; /*vertex A.x assigned to majorStart*/ majorEnd = vB.x; //vertex B.x assigned to major end.
			minorStart = vA.y; /*vertex A.y assigned to minor start. */ minorEnd = vB.y; //vertex B.y assigned to minor end.
			minorSlope = (minorEnd - minorStart) / (majorEnd - majorStart); /*this will determine the minor slope.
																			this is the same thing as (y2-y1)/(x2-x1). This is how to determine the slope between two points.	*/
			(dX >= 0) ? (majorDirection = -1) : (majorDirection = 1);
			//if  difference in x is greater than zero then majorDirection equals -1.
			//else major direction equals 1

		}
		else if (fabs(dX) <= fabs(dY)) { //if the major axis = y
			majorStart = vA.y; /*vertex A.y is assigned to major start*/ majorEnd = vB.y; //vertex B.y is asssigned to major end
			minorStart = vA.x; /*vertex A.x is assigned to minor start */ minorEnd = vB.x; //vertex B.x is assigned to minor end.
			minorSlope = (minorEnd - minorStart) / (majorEnd - majorStart);
			/*this will determine the minor slope.
			this is the same thing as (y2-y1)/(x2-x1). This is how to determine the slope between two points.	*/
			(dY > 0) ? (majorDirection = -1) : (majorDirection = 1);
			//if  difference in x is greater than zero then majorDirection equals -1.
			//else major direction equals 1

		}

		stepVertex.x = majorStart; stepVertex.y = minorStart; //setting stepvertex for x and y as the major start and minor start.

		VertexColor currentColor = vA.color; //sets currentColor = to vA.color
		VertexColor colorSlope; //creates variable colorSlope of type VertexColor

		colorSlope.red = (vB.color.red - vA.color.red) / (majorEnd - majorStart);
		colorSlope.green = (vB.color.green - vA.color.green) / (majorEnd - majorStart);
		colorSlope.blue = (vB.color.blue - vA.color.blue) / (majorEnd - majorStart);

		for (int i = majorStart - majorDirection; i != majorEnd - majorDirection; i += majorDirection) {
			Vertex pixel; //creating pixel of type Vertex.
			bool pixelDrawn = false; //sets pixelDrawn = false
			for (int j = 0; j < 10; j++) { //b will iterate from 0 through 10
				lastPosition.x = stepVertex.x - floor(stepVertex.x) - .5; /*the purpose of these two lines is to
																		  set the last relative pos before the current step.*/
				lastPosition.y = stepVertex.y - floor(stepVertex.y) - .5; //floor method rounds down

				lastValue = fabs(lastPosition.x) + fabs(lastPosition.y);

				stepVertex.x += .1 * majorDirection; stepVertex.y += minorSlope / 10.0 * majorDirection;

				currentPosition.x = stepVertex.x - floor(stepVertex.x) - .5; currentPosition.y = stepVertex.y - floor(stepVertex.y) - .5;

				currentValue = fabs(currentPosition.x) + fabs(currentPosition.y);

				lastValue = floor((lastValue * 100)) / 100;
				currentValue = floor((currentValue * 100)) / 100;

				if ((currentValue >= .5 || (currentValue == .5 && currentPosition.y > 0)) && (lastValue <= .5 || (lastValue == .5 && lastPosition.y < 0))) {
					pixel.x = stepVertex.x - (.1 * majorDirection); pixel.y = stepVertex.y - (minorSlope / 10.0 * majorDirection);
					pixelDrawn = true;
				}

			}

			Color color; //creates variable color of type Color

			if (!(m_lineShadingMode == LINE_SHADING_MODE_SMOOTH)) {
				color.red = m_lineColor.red * 255;
				color.green = m_lineColor.green * 255;
				color.blue = m_lineColor.blue * 255;

			}
			else if (m_lineShadingMode == LINE_SHADING_MODE_SMOOTH) {
				color.red = currentColor.red * 255;
				currentColor.red += colorSlope.red * majorDirection;

				color.green = currentColor.green * 255;
				currentColor.green += colorSlope.green * majorDirection;

				color.blue = currentColor.blue * 255;
				currentColor.blue += colorSlope.blue * majorDirection;
			}

			stepVertex.x = floor(stepVertex.x + .5);

			if (fabs(dX) > fabs(dY))
				//x is major axis in this case
				drawingSurface->drawPixel(floor(pixel.x), floor(pixel.y), color);
			else if (fabs(dX) <= fabs(dY)) //y is major axis in this case
				drawingSurface->drawPixel(floor(pixel.y), floor(pixel.x), color);
		}
	}

	void DrawingContext::triangle(ISurface * drawingSurface, IZBuffer * zBuffer, const Vertex * v1, const Vertex * v2, const Vertex * v3) const {

		//Here we are going to copy the pointer vertex objects to local vertex objects and convert them to surface coordinates.

		Vertex vertex1 = *v1; Vertex vertex2 = *v2; Vertex vertex3 = *v3;

		//check for invalid triangles by checking if slopes are the same (This formula is derived from the slope
		//formula between the three points) since floating point math is not perfect, we need a tiny margin of error
		if (floor((vertex2.y - vertex1.y)*(vertex3.x - vertex2.x) * 100000) == floor((vertex3.y - vertex2.y)*(vertex2.x - vertex1.x) * 100000))
			return; //all points lie on a line or on the same point, so do not draw.

		vertex1.x = (int)((drawingSurface->getWidth() - 1) * ((vertex1.x + 1.0) / 2.0)); //basically finding where each vertex is in the triangle (endpoint to surface values).
		vertex2.x = (int)((drawingSurface->getWidth() - 1) * ((vertex2.x + 1.0) / 2.0));
		vertex3.x = (int)((drawingSurface->getWidth() - 1) * ((vertex3.x + 1.0) / 2.0));
		vertex1.y = (int)((drawingSurface->getHeight() - 1) * ((vertex1.y + 1.0) / 2.0));
		vertex2.y = (int)((drawingSurface->getHeight() - 1) * ((vertex2.y + 1.0) / 2.0));
		vertex3.y = (int)((drawingSurface->getHeight() - 1) * ((vertex3.y + 1.0) / 2.0));
		vertex1.z = (65535)*((vertex1.z + 1.0) / 2.0); //this is for z buffering
		vertex2.z = (65535)*((vertex2.z + 1.0) / 2.0);
		vertex3.z = (65535)*((vertex3.z + 1.0) / 2.0);

		if (m_textureMap != nullptr) {
			vertex1.s = vertex1.s * m_textureMap->getWidth();
			vertex1.t = vertex1.t * m_textureMap->getHeight();

			vertex2.s = vertex2.s * m_textureMap->getWidth();
			vertex2.t = vertex2.t * m_textureMap->getHeight();

			vertex3.s = vertex3.s * m_textureMap->getWidth();
			vertex3.t = vertex3.t * m_textureMap->getHeight();
		}

		//calculate values for "bounding box" for the for loop
		float minX = std::min(vertex1.x, std::min(vertex2.x, vertex3.x));
		float maxX = std::max(vertex1.x, std::max(vertex2.x, vertex3.x));
		float minY = std::min(vertex1.y, std::min(vertex2.y, vertex3.y));
		float maxY = std::max(vertex1.y, std::max(vertex2.y, vertex3.y));

		Color drawColor;
		float a1, a2, a3, Z;

		for (float y = minY - .5; y < maxY + 1; y++) { // iterate through the bounding box by 1 pixel, starting on the halfway point of the pixel
			for (float x = minX - .5; x < maxX + 1; x++) { // to the top left and going to halfway through the pixel to the bottom right.

				const float denom = ((vertex2.y - vertex3.y)*(vertex1.x - vertex3.x) + (vertex3.x - vertex2.x)*(vertex1.y - vertex3.y));
				const float xdiff = (x - vertex3.x);
				const float ydiff = (y - vertex3.y);
				a1 = ((vertex2.y - vertex3.y)*xdiff + (vertex3.x - vertex2.x)*ydiff) / denom;
				a2 = ((vertex3.y - vertex1.y)*xdiff + (vertex1.x - vertex3.x)*ydiff) / denom;
				a3 = 1 - a1 - a2;

				Z = vertex1.z*a1 + vertex2.z*a2 + vertex3.z*a3;

				if ((a1 < -.0000001) || (a2 < -.0000001) || (a3 < -.0000001) || (((zBuffer != nullptr)) && (Z >= zBuffer->getZ(x, y)))) { continue; } //pixel not in triangle or Z value is greater so skip
				//if we get to this point then we set the Z, calculate the color, and draw the pixel.
				if (zBuffer != nullptr) { zBuffer->setZ(x, y, Z); }

				//This will calculate color from lamda values
				drawColor.red = (vertex1.color.red * 255 * a1 + vertex2.color.red * 255 * a2 + vertex3.color.red * 255 * a3);
				drawColor.green = (vertex1.color.green * 255 * a1 + vertex2.color.green * 255 * a2 + vertex3.color.green * 255 * a3);
				drawColor.blue = (vertex1.color.blue * 255 * a1 + vertex2.color.blue * 255 * a2 + vertex3.color.blue * 255 * a3);
				drawColor.alpha = (vertex1.color.alpha * 255 * a1 + vertex2.color.alpha * 255 * a2 + vertex3.color.alpha * 255 * a3);


				if (m_textureMap != nullptr) { // This will get your texture calculations

					float S = vertex1.s*a1 + vertex2.s*a2 + vertex3.s*a3; float T = vertex1.t*a1 + vertex2.t*a2 + vertex3.t*a3;
					Color textureColor;

					// This is the filter mode
					if (m_filterMode == TEXTURE_FILTERING_MODE_BILINEAR) {

						//this sets up values
						float dx = S - floor(S) - .5; float dy = T - floor(T) - .5;
						//dist from pix center
						float adx = fabs(dx); float ady = fabs(dy);
						float diffx = adx * (1 - ady); //for use in calculations
						float diffy = (1 - adx) * ady;
						float diffxy = adx * ady;
						float identical = (1 - adx) * (1 - ady);

						Color TL, TR, BL, BR; //color objects for top left, top right, bottom left, and bottom right texels
						//bulk of code starts here
						if (dy >= 0 && dx >= 0) { // TOP LEFT IS CURRENT TEXEL
												  // we set the colors to the appropriate pixels using the wrap mode method so that we can determine the correct pixel regardless of wrap mode
							TL = getTexelbyWrapMode(m_textureMap, m_wrapMode, S, T); TR = getTexelbyWrapMode(m_textureMap, m_wrapMode, S + 1, T); 
							BL = getTexelbyWrapMode(m_textureMap, m_wrapMode, S, T + 1);  BR = getTexelbyWrapMode(m_textureMap, m_wrapMode, S + 1, T + 1); 
							
							//calculate the colors based on the formula
							textureColor.alpha = TL.alpha	* identical + TR.alpha	* diffx + BL.alpha	* diffy + BR.alpha	* diffxy;
							textureColor.red = TL.red	* identical + TR.red		* diffx + BL.red	* diffy + BR.red	* diffxy;
							textureColor.green = TL.green	* identical + TR.green	* diffx + BL.green	* diffy + BR.green	* diffxy;
							textureColor.blue = TL.blue	* identical + TR.blue	* diffx + BL.blue	* diffy + BR.blue	* diffxy;
						}
						else if (dy >= 0 && dx < 0) { //if this code runs then the top right is the current texel
							TL = getTexelbyWrapMode(m_textureMap, m_wrapMode, S - 1, T); TR = getTexelbyWrapMode(m_textureMap, m_wrapMode, S, T);
							BL = getTexelbyWrapMode(m_textureMap, m_wrapMode, S - 1, T + 1); BR = getTexelbyWrapMode(m_textureMap, m_wrapMode, S, T + 1);

							textureColor.alpha = TL.alpha	* diffx + TR.alpha	* identical + BL.alpha	* diffxy + BR.alpha	* diffy;
							textureColor.red = TL.red	* diffx + TR.red	* identical + BL.red		* diffxy + BR.red	* diffy;
							textureColor.green = TL.green	* diffx + TR.green	* identical + BL.green	* diffxy + BR.green	* diffy;
							textureColor.blue = TL.blue	* diffx + TR.blue	* identical + BL.blue	* diffxy + BR.blue	* diffy;
						}
						else if (dy < 0 && dx >= 0) { // if this code runs then bottom left is the current texel
							TL = getTexelbyWrapMode(m_textureMap, m_wrapMode, S, T - 1); TR = getTexelbyWrapMode(m_textureMap, m_wrapMode, S + 1, T - 1);
							BL = getTexelbyWrapMode(m_textureMap, m_wrapMode, S, T); BR = getTexelbyWrapMode(m_textureMap, m_wrapMode, S + 1, T);

							textureColor.alpha = TL.alpha	* diffy + TR.alpha	* diffxy + BL.alpha	* identical + BR.alpha	* diffx;
							textureColor.red = TL.red	* diffy + TR.red	* diffxy + BL.red	* identical + BR.red		* diffx;
							textureColor.green = TL.green	* diffy + TR.green	* diffxy + BL.green	* identical + BR.green	* diffx;
							textureColor.blue = TL.blue	* diffy + TR.blue	* diffxy + BL.blue	* identical + BR.blue	* diffx;
						}
						else if (dx < 0 && dy < 0) { // if this code runs then bottom right is the current texel.
							TL = getTexelbyWrapMode(m_textureMap, m_wrapMode, S - 1, T - 1);
							TR = getTexelbyWrapMode(m_textureMap, m_wrapMode, S, T - 1);
							BL = getTexelbyWrapMode(m_textureMap, m_wrapMode, S - 1, T);
							BR = getTexelbyWrapMode(m_textureMap, m_wrapMode, S, T);

							//color starts here
							textureColor.alpha = TL.alpha	* diffxy + TR.alpha	* diffy + BL.alpha	* diffx + BR.alpha	* identical;
							textureColor.red = TL.red	* diffxy + TR.red	* diffy + BL.red	* diffx + BR.red	* identical;
							textureColor.green = TL.green	* diffxy + TR.green	* diffy + BL.green	* diffx + BR.green	* identical;
							textureColor.blue = TL.blue	* diffxy + TR.blue	* diffy + BL.blue	* diffx + BR.blue	* identical;
						}
					}
					else { // this is the nearest mode
						textureColor = getTexelbyWrapMode(m_textureMap, m_wrapMode, S, T);
					}

					//blend mode starts here.
					switch (m_blendMode) { //got tired of if statements lol
					case TEXTURE_BLENDING_MODE_MODULATE:
						drawColor.alpha = textureColor.alpha * drawColor.alpha / 255;
						drawColor.red = textureColor.red * drawColor.red / 255;
						drawColor.green = textureColor.green * drawColor.green / 255;
						drawColor.blue = textureColor.blue * drawColor.blue / 255;
						break;

					case TEXTURE_BLENDING_MODE_DECAL:
					default:
						drawColor = textureColor;
						}						
					}
				drawingSurface->drawPixel(x, y, drawColor);
			}
		}
	}

	Color DrawingContext::getTexelbyWrapMode(ISurface * textureMap, TextureWrappingMode wrapMode, float s, float t) const {

		float width = m_textureMap->getWidth(); float height = m_textureMap->getHeight();
		float S = s; float T = t;

		if (wrapMode == TEXTURE_WRAPPING_MODE_MIRROR) {
			S = fabs(S); T = fabs(T);
			S = fmod(S, 2 * width); T = fmod(T, 2 * height);
			if (S > width) { S = width - (S - width); } if (T > height) { T = height - (T - height); }

			if (S == width) {S = 0;
		} if (T == height) { T = 0; }
			//mod by 2 then if greater than 1 width,  calculate reverse s and t vals. if equal to width or height, set 0.
		}
		else if (wrapMode == TEXTURE_WRAPPING_MODE_REPEAT) {
			while (S < 0) { S += width; } while (T < 0) { T += height; }
			S = fmod(S, width); T = fmod(T, height);
			//if we do not move it to positive coords (instead of modding), it will get flipped.
		}
		else { // clamp mode is default
			S = std::max(std::min(S, width - 1), 0.f); T = std::max(std::min(T, height - 1), 0.f);
		}
		return textureMap->getPixel(S, T);
	}

	Color DrawingContext::convertVertexColorToColor(VertexColor vColor) const {
		Color color;
		color.red = vColor.red * 255;
		color.green = vColor.green * 255;
		color.blue = vColor.blue * 255;
		color.alpha = vColor.alpha * 255;
		return color;
	}

	void DrawingContext::incrementColorBySlope(VertexColor & color, VertexColor slope) const {
		color.red += slope.red;
		color.green += slope.green;
		color.blue += slope.blue;
		color.alpha += slope.alpha;
	}

	void DrawingContext::setTextureMap(ISurface * textureSurface) {	m_textureMap = textureSurface;}

	void DrawingContext::setTextureWrappingMode(TextureWrappingMode wrapMode) {
		if (wrapMode >= TEXTURE_WRAPPING_MODE_COUNT) //if the wrap mode is greater than or equal to TEXTURE_WRAPPING_MODE_COUNT throw param exception.
			throw ParameterException("invalid wrap mode");
		m_wrapMode = wrapMode;
	}

	TextureWrappingMode DrawingContext::getTextureWrappingMode() const {return m_wrapMode;}
	

	void DrawingContext::setTextureBlendingMode(TextureBlendingMode blendMode) {
		if (blendMode >= TEXTURE_BLENDING_MODE_COUNT) //same as wrapMode
			throw ParameterException("invalid blend mode");
		m_blendMode = blendMode;
	}

	TextureBlendingMode DrawingContext::getTextureBlendingMode() const { return m_blendMode; }


	TextureFilteringMode DrawingContext::getTextureFilteringMode() const { return m_filterMode; }


	void DrawingContext::setTextureFilteringMode(TextureFilteringMode filterMode) {
			if (filterMode >= TEXTURE_FILTERING_MODE_COUNT) //same as wrapMode
				throw ParameterException("invalid filter mode");

			m_filterMode = filterMode;
		}

	void DrawingContext::setLineColor(VertexColor lineColor) {
		m_lineColor = lineColor;
	}

	VertexColor DrawingContext::getLineColor() const {
		return m_lineColor;
	}

	void DrawingContext::setLineShadingMode(LineShadingMode mode) {
		m_lineShadingMode = mode;
	}

	LineShadingMode DrawingContext::getLineShadingMode() const {
		return m_lineShadingMode;
	}

}