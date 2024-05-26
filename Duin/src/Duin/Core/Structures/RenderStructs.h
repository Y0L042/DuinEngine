#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/Maths/DuinMaths.h"

namespace Duin
{
	/**********************************************************************************************
	*
	*   raylib v5.1-dev - A simple and easy-to-use library to enjoy videogames programming (www.raylib.com)
	*
	*   FEATURES:
	*       - NO external dependencies, all required libraries included with raylib
	*       - Multiplatform: Windows, Linux, FreeBSD, OpenBSD, NetBSD, DragonFly,
	*                        MacOS, Haiku, Android, Raspberry Pi, DRM native, HTML5.
	*       - Written in plain C code (C99) in PascalCase/camelCase notation
	*       - Hardware accelerated with OpenGL (1.1, 2.1, 3.3, 4.3 or ES2 - choose at compile)
	*       - Unique OpenGL abstraction layer (usable as standalone module): [rlgl]
	*       - Multiple Fonts formats supported (TTF, XNA fonts, AngelCode fonts)
	*       - Outstanding texture formats support, including compressed formats (DXT, ETC, ASTC)
	*       - Full 3d support for 3d Shapes, Models, Billboards, Heightmaps and more!
	*       - Flexible Materials system, supporting classic maps and PBR maps
	*       - Animated 3D models supported (skeletal bones animation) (IQM)
	*       - Shaders support, including Model shaders and Postprocessing shaders
	*       - Powerful math module for Vector, Matrix and Quaternion operations: [raymath]
	*       - Audio loading and playing with streaming support (WAV, OGG, MP3, FLAC, XM, MOD)
	*       - VR stereo rendering with configurable HMD device parameters
	*       - Bindings to multiple programming languages available!
	*
	*   NOTES:
	*       - One default Font is loaded on InitWindow()->LoadFontDefault() [core, text]
	*       - One default Texture2D is loaded on rlglInit(), 1x1 white pixel R8G8B8A8 [rlgl] (OpenGL 3.3 or ES2)
	*       - One default Shader is loaded on rlglInit()->rlLoadShaderDefault() [rlgl] (OpenGL 3.3 or ES2)
	*       - One default RenderBatch is loaded on rlglInit()->rlLoadRenderBatch() [rlgl] (OpenGL 3.3 or ES2)
	*
	*   DEPENDENCIES (included):
	*       [rcore] rglfw (Camilla L�wy - github.com/glfw/glfw) for window/context management and input (PLATFORM_DESKTOP)
	*       [rlgl] glad (David Herberth - github.com/Dav1dde/glad) for OpenGL 3.3 extensions loading (PLATFORM_DESKTOP)
	*       [raudio] miniaudio (David Reid - github.com/mackron/miniaudio) for audio device/context management
	*
	*   OPTIONAL DEPENDENCIES (included):
	*       [rcore] msf_gif (Miles Fogle) for GIF recording
	*       [rcore] sinfl (Micha Mettke) for DEFLATE decompression algorithm
	*       [rcore] sdefl (Micha Mettke) for DEFLATE compression algorithm
	*       [rtextures] stb_image (Sean Barret) for images loading (BMP, TGA, PNG, JPEG, HDR...)
	*       [rtextures] stb_image_write (Sean Barret) for image writing (BMP, TGA, PNG, JPG)
	*       [rtextures] stb_image_resize (Sean Barret) for image resizing algorithms
	*       [rtext] stb_truetype (Sean Barret) for ttf fonts loading
	*       [rtext] stb_rect_pack (Sean Barret) for rectangles packing
	*       [rmodels] par_shapes (Philip Rideout) for parametric 3d shapes generation
	*       [rmodels] tinyobj_loader_c (Syoyo Fujita) for models loading (OBJ, MTL)
	*       [rmodels] cgltf (Johannes Kuhlmann) for models loading (glTF)
	*       [rmodels] Model3D (bzt) for models loading (M3D, https://bztsrc.gitlab.io/model3d)
	*       [raudio] dr_wav (David Reid) for WAV audio file loading
	*       [raudio] dr_flac (David Reid) for FLAC audio file loading
	*       [raudio] dr_mp3 (David Reid) for MP3 audio file loading
	*       [raudio] stb_vorbis (Sean Barret) for OGG audio loading
	*       [raudio] jar_xm (Joshua Reisenauer) for XM audio module loading
	*       [raudio] jar_mod (Joshua Reisenauer) for MOD audio module loading
	*
	*
	*   LICENSE: zlib/libpng
	*
	*   raylib is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
	*   BSD-like license that allows static linking with closed source software:
	*
	*   Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
	*
	*   This software is provided "as-is", without any express or implied warranty. In no event
	*   will the authors be held liable for any damages arising from the use of this software.
	*
	*   Permission is granted to anyone to use this software for any purpose, including commercial
	*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
	*
	*     1. The origin of this software must not be misrepresented; you must not claim that you
	*     wrote the original software. If you use this software in a product, an acknowledgment
	*     in the product documentation would be appreciated but is not required.
	*
	*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
	*     as being the original software.
	*
	*     3. This notice may not be removed or altered from any source distribution.
	*
	**********************************************************************************************/

	// NOTE: MSVC C++ compiler does not support compound literals (C99 feature)
	// Plain structures in C++ (without constructors) can be initialized with { }
	// This is called aggregate initialization (C++11 feature)
	#if defined(__cplusplus)
	#define CLITERAL(type)      type
	#else
	#define CLITERAL(type)      (type)
	#endif

	// Some compilers (mostly macos clang) default to C++98,
	// where aggregate initialization can't be used
	// So, give a more clear error stating how to fix this
	#if !defined(_MSC_VER) && (defined(__cplusplus) && __cplusplus < 201103L)
	#error "C++11 or later is required. Add -std=c++11"
	#endif

	#define LIGHTGRAY  CLITERAL(Color){ 200, 200, 200, 255 }   // Light Gray
	#define GRAY       CLITERAL(Color){ 130, 130, 130, 255 }   // Gray
	#define DARKGRAY   CLITERAL(Color){ 80, 80, 80, 255 }      // Dark Gray
	#define YELLOW     CLITERAL(Color){ 253, 249, 0, 255 }     // Yellow
	#define GOLD       CLITERAL(Color){ 255, 203, 0, 255 }     // Gold
	#define ORANGE     CLITERAL(Color){ 255, 161, 0, 255 }     // Orange
	#define PINK       CLITERAL(Color){ 255, 109, 194, 255 }   // Pink
	#define RED        CLITERAL(Color){ 230, 41, 55, 255 }     // Red
	#define MAROON     CLITERAL(Color){ 190, 33, 55, 255 }     // Maroon
	#define GREEN      CLITERAL(Color){ 0, 228, 48, 255 }      // Green
	#define LIME       CLITERAL(Color){ 0, 158, 47, 255 }      // Lime
	#define DARKGREEN  CLITERAL(Color){ 0, 117, 44, 255 }      // Dark Green
	#define SKYBLUE    CLITERAL(Color){ 102, 191, 255, 255 }   // Sky Blue
	#define BLUE       CLITERAL(Color){ 0, 121, 241, 255 }     // Blue
	#define DARKBLUE   CLITERAL(Color){ 0, 82, 172, 255 }      // Dark Blue
	#define PURPLE     CLITERAL(Color){ 200, 122, 255, 255 }   // Purple
	#define VIOLET     CLITERAL(Color){ 135, 60, 190, 255 }    // Violet
	#define DARKPURPLE CLITERAL(Color){ 112, 31, 126, 255 }    // Dark Purple
	#define BEIGE      CLITERAL(Color){ 211, 176, 131, 255 }   // Beige
	#define BROWN      CLITERAL(Color){ 127, 106, 79, 255 }    // Brown
	#define DARKBROWN  CLITERAL(Color){ 76, 63, 47, 255 }      // Dark Brown

	#define WHITE      CLITERAL(Color){ 255, 255, 255, 255 }   // White
	#define BLACK      CLITERAL(Color){ 0, 0, 0, 255 }         // Black
	#define BLANK      CLITERAL(Color){ 0, 0, 0, 0 }           // Blank (Transparent)
	#define MAGENTA    CLITERAL(Color){ 255, 0, 255, 255 }     // Magenta


	// Color, 4 components, R8G8B8A8 (32bit)
	typedef struct DUIN_API Color {
		unsigned char r;        // Color red value
		unsigned char g;        // Color green value
		unsigned char b;        // Color blue value
		unsigned char a;        // Color alpha value
	} Color;

	// Image, pixel data stored in CPU memory (RAM)
	typedef struct DUIN_API Image {
		void* data;             // Image raw data
		int width;              // Image base width
		int height;             // Image base height
		int mipmaps;            // Mipmap levels, 1 by default
		int format;             // Data format (PixelFormat type)
	} Image;

	// Texture, tex data stored in GPU memory (VRAM)
	typedef struct DUIN_API Texture {
		unsigned int id;        // OpenGL texture id
		int width;              // Texture base width
		int height;             // Texture base height
		int mipmaps;            // Mipmap levels, 1 by default
		int format;             // Data format (PixelFormat type)
	} Texture;

	// Texture2D, same as Texture
	typedef Texture Texture2D;

	// TextureCubemap, same as Texture
	typedef Texture TextureCubemap;

	// RenderTexture, fbo for texture rendering
	typedef struct DUIN_API RenderTexture {
		unsigned int id;        // OpenGL framebuffer object id
		Texture texture;        // Color buffer attachment texture
		Texture depth;          // Depth buffer attachment texture
	} RenderTexture;

	// RenderTexture2D, same as RenderTexture
	typedef RenderTexture RenderTexture2D;

	// NPatchInfo, n-patch layout info
	typedef struct DUIN_API NPatchInfo {
		Rectangle source;       // Texture source rectangle
		int left;               // Left border offset
		int top;                // Top border offset
		int right;              // Right border offset
		int bottom;             // Bottom border offset
		int layout;             // Layout of the n-patch: 3x3, 1x3 or 3x1
	} NPatchInfo;

	// GlyphInfo, font characters glyphs info
	typedef struct DUIN_API GlyphInfo {
		int value;              // Character value (Unicode)
		int offsetX;            // Character offset X when drawing
		int offsetY;            // Character offset Y when drawing
		int advanceX;           // Character advance position X
		Image image;            // Character image data
	} GlyphInfo;

	// Font, font texture and GlyphInfo array data
	typedef struct DUIN_API Font {
		int baseSize;           // Base size (default chars height)
		int glyphCount;         // Number of glyph characters
		int glyphPadding;       // Padding around the glyph characters
		Texture2D texture;      // Texture atlas containing the glyphs
		Rectangle* recs;        // Rectangles in texture for the glyphs
		GlyphInfo* glyphs;      // Glyphs info data
	} Font;

	// Camera, defines position/orientation in 3d space
	typedef struct DUIN_API Camera3D {
		Vector3 position;       // Camera position
		Vector3 target;         // Camera target it looks-at
		Vector3 up;             // Camera up vector (rotation over its axis)
		float fovy;             // Camera field-of-view aperture in Y (degrees) in perspective, used as near plane width in orthographic
		int projection;         // Camera projection: CAMERA_PERSPECTIVE or CAMERA_ORTHOGRAPHIC
	} Camera3D;

	typedef Camera3D Camera;    // Camera type fallback, defaults to Camera3D

	// Camera2D, defines position/orientation in 2d space
	typedef struct DUIN_API Camera2D {
		Vector2 offset;         // Camera offset (displacement from target)
		Vector2 target;         // Camera target (rotation and zoom origin)
		float rotation;         // Camera rotation in degrees
		float zoom;             // Camera zoom (scaling), should be 1.0f by default
	} Camera2D;

	// Mesh, vertex data and vao/vbo
	typedef struct DUIN_API Mesh {
		int vertexCount;        // Number of vertices stored in arrays
		int triangleCount;      // Number of triangles stored (indexed or not)

		// Vertex attributes data
		float* vertices;        // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
		float* texcoords;       // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
		float* texcoords2;      // Vertex texture second coordinates (UV - 2 components per vertex) (shader-location = 5)
		float* normals;         // Vertex normals (XYZ - 3 components per vertex) (shader-location = 2)
		float* tangents;        // Vertex tangents (XYZW - 4 components per vertex) (shader-location = 4)
		unsigned char* colors;      // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
		unsigned short* indices;    // Vertex indices (in case vertex data comes indexed)

		// Animation vertex data
		float* animVertices;    // Animated vertex positions (after bones transformations)
		float* animNormals;     // Animated normals (after bones transformations)
		unsigned char* boneIds; // Vertex bone ids, max 255 bone ids, up to 4 bones influence by vertex (skinning)
		float* boneWeights;     // Vertex bone weight, up to 4 bones influence by vertex (skinning)

		// OpenGL identifiers
		unsigned int vaoId;     // OpenGL Vertex Array Object id
		unsigned int* vboId;    // OpenGL Vertex Buffer Objects id (default vertex data)
	} Mesh;

	// Shader
	typedef struct DUIN_API Shader {
		unsigned int id;        // Shader program id
		int* locs;              // Shader locations array (RL_MAX_SHADER_LOCATIONS)
	} Shader;

	// MaterialMap
	typedef struct DUIN_API MaterialMap {
		Texture2D texture;      // Material map texture
		Color color;            // Material map color
		float value;            // Material map value
	} MaterialMap;

	// Material, includes shader and maps
	typedef struct DUIN_API Material {
		Shader shader;          // Material shader
		MaterialMap* maps;      // Material maps array (MAX_MATERIAL_MAPS)
		float params[4];        // Material generic parameters (if required)
	} Material;

	// Transform, vertex transformation data
	typedef struct DUIN_API Transform {
		Vector3 translation;    // Translation
		Quaternion rotation;    // Rotation
		Vector3 scale;          // Scale
	} Transform;

	// Bone, skeletal animation bone
	typedef struct DUIN_API BoneInfo {
		char name[32];          // Bone name
		int parent;             // Bone parent
	} BoneInfo;

	// Model, meshes, materials and animation data
	typedef struct DUIN_API Model {
		Matrix transform;       // Local transform matrix

		int meshCount;          // Number of meshes
		int materialCount;      // Number of materials
		Mesh* meshes;           // Meshes array
		Material* materials;    // Materials array
		int* meshMaterial;      // Mesh material number

		// Animation data
		int boneCount;          // Number of bones
		BoneInfo* bones;        // Bones information (skeleton)
		Transform* bindPose;    // Bones base transformation (pose)
	} Model;

	// ModelAnimation
	typedef struct DUIN_API ModelAnimation {
		int boneCount;          // Number of bones
		int frameCount;         // Number of animation frames
		BoneInfo* bones;        // Bones information (skeleton)
		Transform** framePoses; // Poses array by frame
		char name[32];          // Animation name
	} ModelAnimation;
/*********************************************************************************************/
}