//Author: Hunter Barton
#pragma once

#include "ctxgraf_pub.h"

namespace ctxgraf {

	class DrawingContext : public IDrawingContext {
	
	private:
		//virtual void DrawFlatTopTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3, Color c);
		//void DrawFlatTopTriangle(Vertex v1, Vertex v2, Vertex v3, Color c) const;
		//void DrawFlatBottomTriangle(Vertex v1, Vertex v2, Vertex v3, Color c) const;
		//virtual void DrawFlatBottomTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3, Color c);

	public:
		DrawingContext() {}
		~DrawingContext() {}
		/**
		* Draw a series of line segments that connect every adjacent pair of vertices.
		* That is, the first line segment will connect vertices[0] and vertices[1],
		* the second will connect vertices[1] and vertices[2], and so on.
		*
		* @param[in] drawingSurface The surface to draw into.
		* @param[in] vertices The array of vertices in the polyline.
		* @param[in] vertexCount The number of vertices in the array.
		*
		* @throws ParameterException if vertices is NULL, or vertexCount < 2.
		*/
		virtual void polyline(ISurface* drawingSurface, const Vertex* vertices, uint32_t vertexCount) const;

		/**
		* Set the constant line color.
		* This color is used for all lines if LINE_SHADING_MODE_CONSTANT is being used.
		*
		* @param[in] vertexColor The constant line color.
		*/
		virtual void setLineColor(VertexColor lineColor);

		/**
		* Return the current constant line color.
		*/
		virtual VertexColor getLineColor() const;

		/**
		* Set the line shading mode.
		* This mode is used for all polyline() commands until the next call to setLineShadingMode().
		*
		* @param[in] mode The new line shading mode.
		*
		* @throws ParameterException if mode is not a legal LineShadingMode value.
		*/
		virtual void setLineShadingMode(LineShadingMode mode);

		/**
		* Return the current line shading mode.
		*/
		virtual LineShadingMode getLineShadingMode() const;

		/**
		* Draw a single triangle.
		*
		* @param[in] drawingSurface The surface to draw into.
		* @param[in] zBuffer The Z buffer to use. (May be null for no Z buffering)
		* @param[in] v1 The first triangle vertex.
		* @param[in] v2 The second triangle vertex.
		* @param[in] v3 The third triangle vertex.
		*
		* @throws ParameterException if any of the parameters except zBuffer is NULL.
		*/
		virtual void triangle(ISurface* drawingSurface, IZBuffer* zBuffer, const Vertex* v1, const Vertex* v2, const Vertex* v3) const;

		Color getTexelbyWrapMode(ISurface * textureMap, TextureWrappingMode wrapMode, float s, float t) const;

		/**
		* Set the texture map to use for drawing triangles.
		* It defaults to NULL (i.e. no texture map).
		*
		* @throws ParameterException if textureSurface isn't supported as a texture map
		* (for example, if the pixel format isn't acceptable).
		*/
		virtual void setTextureMap(ISurface* textureSurface);

		/**
		* Set the texture wrapping mode.
		* It defaults to TEXTURE_WRAPPING_CLAMP.
		*
		* @throws ParameterException if wrapMode is invalid.
		*/
		virtual void setTextureWrappingMode(TextureWrappingMode wrapMode);

		/**
		* Get the current texture wrapping mode.
		*/
		virtual TextureWrappingMode getTextureWrappingMode() const;

		/**
		* Set the texture blending mode.
		* It defaults to TEXTURE_BLENDING_DECAL.
		*
		* @throws ParameterException if blendMode is invalid.
		*/
		virtual void setTextureBlendingMode(TextureBlendingMode blendMode);

		/**
		* Return the current texture blending mode.
		*/
		virtual TextureBlendingMode getTextureBlendingMode() const;

		/**
		* Set the texture filtering mode.
		* It defaults to TEXTURE_FILTERING_NEAREST.
		*
		* @throws ParameterException if filterMode is invalid.
		*/
		virtual void setTextureFilteringMode(TextureFilteringMode filterMode);

		/**
		* Return the current texture filtering mode.
		*/
		virtual TextureFilteringMode getTextureFilteringMode() const;

		//my functions
		virtual void drawLine(ISurface* drawingSurface, Vertex vA, Vertex vB) const;
		Color convertVertexColorToColor(VertexColor vColor) const;
		void incrementColorBySlope(VertexColor & color, VertexColor slope) const;

	protected:

			
		//my variables

LineShadingMode m_lineShadingMode;
VertexColor		m_lineColor;
ISurface* m_textureMap;
TextureWrappingMode m_wrapMode;
TextureBlendingMode m_blendMode;
TextureFilteringMode m_filterMode;

	};
}