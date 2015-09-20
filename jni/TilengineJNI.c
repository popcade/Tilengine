#include <jni.h>
#include "Tilengine.h"

struct
{
	JNIEnv* env;
	jobject obj;
	jmethodID method;
}
static callback;

struct
{
	jintArray jia;
	int pitch;
}
static target;

static void raster_callback (int line)
{
	(*callback.env)->CallVoidMethod (callback.env, callback.obj, callback.method, line);
}

JNIEXPORT void JNICALL Java_Tilengine_Init (JNIEnv* env, jobject thisobj, jint hres, jint vres, jint numlayers, jint numsprites, jint numanimations)
{
	TLN_Init (hres, vres, numlayers, numsprites, numanimations);
}

JNIEXPORT void JNICALL Java_Tilengine_Deinit (JNIEnv* env, jobject thisobj)
{
	TLN_Deinit ();
}

JNIEXPORT jint JNICALL Java_Tilengine_GetNumObjects (JNIEnv* env, jobject thisobj)
{
	return TLN_GetNumObjects ();
}

JNIEXPORT jint JNICALL Java_Tilengine_GetUsedMemory (JNIEnv* env, jobject thisobj)
{
	return TLN_GetUsedMemory ();
}

JNIEXPORT jint JNICALL Java_Tilengine_GetNumLayers (JNIEnv* env, jobject thisobj)
{
	return TLN_GetNumLayers ();
}

JNIEXPORT jint JNICALL Java_Tilengine_GetNumSprites (JNIEnv* env, jobject thisobj)
{
	return TLN_GetNumSprites ();
}

JNIEXPORT jint JNICALL Java_Tilengine_GetVersion (JNIEnv* env, jobject thisobj)
{
	return TLN_GetVersion ();
}

JNIEXPORT jboolean JNICALL Java_Tilengine_CreateWindow (JNIEnv* env, jobject thisobj, jstring filename, jint flags)
{
	jboolean retval;
	char* overlay = NULL;
	
	if (filename)
		overlay = (*env)->GetStringUTFChars (env, filename, NULL);
	retval = TLN_CreateWindow (overlay, flags);
	if (filename)
		(*env)->ReleaseStringUTFChars (env, filename, overlay);
	
	return retval;
}

JNIEXPORT jboolean JNICALL Java_Tilengine_CreateWindowThread (JNIEnv* env, jobject thisobj, jstring filename, jint flags)
{
	jboolean retval;
	char* overlay = NULL;
	
	if (filename)
		overlay = (*env)->GetStringUTFChars (env, filename, NULL);
	retval = TLN_CreateWindowThread (overlay, flags);
	if (filename)
		(*env)->ReleaseStringUTFChars (env, filename, overlay);
	
	return retval;
}

JNIEXPORT jboolean JNICALL Java_Tilengine_ProcessWindow (JNIEnv* env, jobject thisobj)
{
	return TLN_ProcessWindow ();
}

JNIEXPORT jboolean JNICALL Java_Tilengine_IsWindowActive (JNIEnv* env, jobject thisobj)
{
	return TLN_IsWindowActive ();
}

JNIEXPORT jboolean JNICALL Java_Tilengine_GetInput (JNIEnv* env, jobject thisobj, jint input)
{
	return TLN_GetInput (input);
}

JNIEXPORT void JNICALL Java_Tilengine_DrawFrame (JNIEnv* env, jobject thisobj, jint time)
{
	TLN_DrawFrame (time);
}

JNIEXPORT void JNICALL Java_Tilengine_WaitRedraw (JNIEnv* env, jobject thisobj)
{
	TLN_WaitRedraw ();
}

JNIEXPORT void JNICALL Java_Tilengine_DeleteWindow (JNIEnv* env, jobject thisobj)
{
	TLN_DeleteWindow ();
}

JNIEXPORT void JNICALL Java_Tilengine_EnableBlur (JNIEnv* env, jobject thisobj, jboolean mode)
{
	TLN_EnableBlur (mode);
}

JNIEXPORT jint JNICALL Java_Tilengine_GetTicks (JNIEnv* env, jobject thisobj)
{
	return TLN_GetTicks ();
}

JNIEXPORT void JNICALL Java_Tilengine_SetBGColor (JNIEnv* env, jobject thisobj, jint r, jint g, jint b)
{
	TLN_SetBGColor (r, g, b);
}

JNIEXPORT void JNICALL Java_Tilengine_SetBGBitmap (JNIEnv* env, jobject thisobj, jint bitmap)
{
	TLN_SetBGBitmap ((TLN_Bitmap)bitmap);
}

JNIEXPORT void JNICALL Java_Tilengine_SetBGPalette (JNIEnv* env, jobject thisobj, jint palette)
{
	TLN_SetBGPalette ((TLN_Palette)palette);
}

JNIEXPORT void JNICALL Java_Tilengine_SetRasterCallback (JNIEnv* env, jobject thisobj, jobject obj, jstring methodname)
{
	/* borra anterior */
	TLN_SetRasterCallback (NULL);
	if (callback.obj)
		(*env)->DeleteGlobalRef (env, callback.obj);
	callback.obj = NULL;
	
	/* crea nuevo */
	if (obj && methodname)
	{
		char* strmethod;
		jclass cls;
		
		strmethod = (*env)->GetStringUTFChars (env, methodname, NULL);
		cls = (*env)->GetObjectClass (env, obj);
		callback.env = env;
		callback.obj = (*env)->NewGlobalRef (env, obj);
		callback.method = (*env)->GetMethodID (env, cls, strmethod, "(I)V");
		(*env)->ReleaseStringUTFChars (env, methodname, strmethod);
		(*env)->DeleteLocalRef (env, obj);
		TLN_SetRasterCallback (raster_callback);
	}
}

JNIEXPORT void JNICALL Java_Tilengine_SetRenderTarget (JNIEnv* env, jobject thisobj, jintArray jia, jint pitch)
{
	/* release previous */
	if (target.jia)
	{
		(*env)->DeleteGlobalRef (env, target.jia);
		target.jia = NULL;
		target.pitch = 0;
	}

	/* store new */
	if (jia)
	{
		target.jia = (*env)->NewGlobalRef (env, jia);
		target.pitch = pitch;
		(*env)->DeleteLocalRef (env, jia);
	}
}

JNIEXPORT void JNICALL Java_Tilengine_UpdateFrame (JNIEnv* env, jobject thisobj, jint time)
{
	jint* buffer;

	if (target.pitch)
	{
		buffer = (*env)->GetIntArrayElements (env, target.jia, NULL);
		TLN_SetRenderTarget ((BYTE*)buffer, target.pitch);
		TLN_UpdateFrame (time);
		(*env)->ReleaseIntArrayElements (env, target.jia, buffer, 0);
	}
}

JNIEXPORT jint JNICALL Java_Tilengine_LoadSpriteset (JNIEnv* env, jobject thisobj, jstring filename)
{
	TLN_Spriteset spriteset;
	char* name = NULL;

	if (!filename)
		return 0;

	name = (*env)->GetStringUTFChars (env, filename, NULL);
	spriteset = TLN_LoadSpriteset (name);
	(*env)->ReleaseStringUTFChars (env, filename, name);
	return (jint)spriteset;
}

JNIEXPORT jint JNICALL Java_Tilengine_CloneSpriteset (JNIEnv* env, jobject thisobj, jint src)
{
	return (jint)TLN_CloneSpriteset ((TLN_Spriteset)src);
}

JNIEXPORT jboolean JNICALL Java_Tilengine_GetSpriteInfo (JNIEnv* env, jobject thisobj, jint spriteset, jint entry, jobject info)
{
	TLN_SpriteInfo si;
	jclass clazz;
	jfieldID fieldID;

	if (!TLN_GetSpriteInfo ((TLN_Spriteset)spriteset, entry, &si))
		return false;

	clazz = (*env)->GetObjectClass (env, info);
	if (!clazz)
		return false;

	fieldID = (*env)->GetFieldID (env, clazz, "offset", "I");
	if (fieldID)
		(*env)->SetIntField (env, info, fieldID, si.offset);

	fieldID = (*env)->GetFieldID (env, clazz, "w", "I");
	if (fieldID)
		(*env)->SetIntField (env, info, fieldID, si.w);

	fieldID = (*env)->GetFieldID (env, clazz, "h", "I");
	if (fieldID)
		(*env)->SetIntField (env, info, fieldID, si.h);

	return true;
}

JNIEXPORT jint JNICALL Java_Tilengine_GetSpritesetPalette (JNIEnv* env, jobject thisobj, jint spriteset)
{
	return (jint)TLN_GetSpritesetPalette ((TLN_Spriteset)spriteset);
}

JNIEXPORT void JNICALL Java_Tilengine_DeleteSpriteset (JNIEnv* env, jobject thisobj, jint spriteset)
{
	TLN_DeleteSpriteset ((TLN_Spriteset)spriteset);
}

JNIEXPORT jint JNICALL Java_Tilengine_CreateTileset (JNIEnv* env, jobject thisobj, jint numtiles, jint width, jint height, jint palette)
{
	return (jint)TLN_CreateTileset (numtiles, width, height, (TLN_Palette)palette);
}

JNIEXPORT jint JNICALL Java_Tilengine_LoadTileset (JNIEnv* env, jobject thisobj, jstring filename)
{
	TLN_Tileset tileset;
	char* name;

	if (!filename)
		return 0;
	
	name = (*env)->GetStringUTFChars (env, filename, NULL);
	tileset = TLN_LoadTileset (name);
	(*env)->ReleaseStringUTFChars (env, filename, name);
	return (jint)tileset;
}	

JNIEXPORT jint JNICALL Java_Tilengine_CloneTileset (JNIEnv* env, jobject thisobj, jint src)
{
	return (jint)TLN_CloneTileset ((TLN_Tileset)src);
}

JNIEXPORT jboolean JNICALL Java_Tilengine_SetTilesetPixels (JNIEnv* env, jobject thisobj, jint tileset, jint entry, jbyteArray srcdata, jint pitch)
{
	jbyte* buffer;
	jboolean retval;

	buffer = (*env)->GetByteArrayElements (env, srcdata, NULL);
	retval = TLN_SetTilesetPixels ((TLN_Tileset)tileset, entry, buffer, pitch);
	(*env)->ReleaseByteArrayElements (env, srcdata, buffer, 0);

	return retval;
}

JNIEXPORT jint JNICALL Java_Tilengine_GetTileWidth (JNIEnv* env, jobject thisobj, jint tileset)
{
	return TLN_GetTileWidth ((TLN_Tileset)tileset);
}

JNIEXPORT jint JNICALL Java_Tilengine_GetTileHeight (JNIEnv* env, jobject thisobj, jint tileset)
{
	return TLN_GetTileHeight ((TLN_Tileset)tileset);
}

JNIEXPORT jint JNICALL Java_Tilengine_GetTilesetPalette (JNIEnv* env, jobject thisobj, jint tileset)
{
	return (jint)TLN_GetTilesetPalette ((TLN_Tileset)tileset);
}

JNIEXPORT void JNICALL Java_Tilengine_DeleteTileset (JNIEnv* env, jobject thisobj, jint tileset)
{
	TLN_DeleteTileset ((TLN_Tileset)tileset);
}

JNIEXPORT jint JNICALL Java_Tilengine_LoadTilemap (JNIEnv* env, jobject thisobj, jstring filename, jstring layername)
{
	char* name;
	char* layer;
	TLN_Tilemap tilemap;

	if (!filename || !layername)
		return 0;

	name = (*env)->GetStringUTFChars (env, filename, NULL);
	layer = (*env)->GetStringUTFChars (env, layername, NULL);

	tilemap = TLN_LoadTilemap (name, layer);
	(*env)->ReleaseStringUTFChars (env, layername, layer);
	(*env)->ReleaseStringUTFChars (env, filename, name);
	return (jint)tilemap;
}

JNIEXPORT jint JNICALL Java_Tilengine_CloneTilemap (JNIEnv* env, jobject thisobj, jint src)
{
	return (jint)TLN_CloneTilemap ((TLN_Tilemap)src);
}

JNIEXPORT jint JNICALL Java_Tilengine_GetTilemapRows (JNIEnv* env, jobject thisobj, jint tilemap)
{
	return TLN_GetTilemapRows ((TLN_Tilemap)tilemap);
}

JNIEXPORT jint JNICALL Java_Tilengine_GetTilemapCols (JNIEnv* env, jobject thisobj, jint tilemap)
{
	return TLN_GetTilemapCols ((TLN_Tilemap)tilemap);
}

JNIEXPORT jboolean JNICALL Java_Tilengine_GetTilemapTile (JNIEnv* env, jobject thisobj, jint tilemap, jint row, jint col, jobject info)
{
	Tile tile;
	jclass clazz;
	jfieldID fieldID;

	if (!TLN_GetTilemapTile ((TLN_Tilemap)tilemap, row, col, &tile))
		return false;

	clazz = (*env)->GetObjectClass (env, info);
	if (!clazz)
		return false;

	fieldID = (*env)->GetFieldID (env, clazz, "index", "S");
	if (fieldID)
		(*env)->SetShortField (env, info, fieldID, tile.index);

	fieldID = (*env)->GetFieldID (env, clazz, "flags", "S");
	if (fieldID)
		(*env)->SetShortField (env, info, fieldID, tile.flags);

	return true;
}

JNIEXPORT jboolean JNICALL Java_Tilengine_SetTilemapTile (JNIEnv* env, jobject thisobj, jint tilemap, jint row, jint col, jobject info)
{
	Tile tile = {0};
	jclass clazz;
	jfieldID fieldID;

	if (!TLN_GetTilemapTile ((TLN_Tilemap)tilemap, row, col, &tile))
		return false;

	clazz = (*env)->GetObjectClass (env, info);
	if (!clazz)
		return false;

	fieldID = (*env)->GetFieldID (env, clazz, "index", "S");
	if (fieldID)
		tile.index = (*env)->GetShortField (env, info, fieldID);

	fieldID = (*env)->GetFieldID (env, clazz, "flags", "S");
	if (fieldID)
		tile.flags = (*env)->GetShortField (env, info, fieldID);

	return TLN_SetTilemapTile ((TLN_Tilemap)tilemap, row, col, &tile);
}

JNIEXPORT void JNICALL Java_Tilengine_CopyTiles (JNIEnv* env, jobject thisobj, jint src, jint srcrow, jint srccol, jint rows, jint cols, jint dst, jint dstrow, jint dstcol)
{
	TLN_CopyTiles ((TLN_Tilemap)src, srcrow, srccol, rows, cols, (TLN_Tilemap)dst, dstrow, dstcol);
}

JNIEXPORT void JNICALL Java_Tilengine_DeleteTilemap (JNIEnv* env, jobject thisobj, jint tilemap)
{
	TLN_DeleteTilemap ((TLN_Tilemap)tilemap);
}

JNIEXPORT jint JNICALL Java_Tilengine_CreatePalette (JNIEnv* env, jobject thisobj, jint entries)
{
	return (jint)TLN_CreatePalette (entries);
}

JNIEXPORT jint JNICALL Java_Tilengine_LoadPalette (JNIEnv* env, jobject thisobj, jstring filename)
{
	char* name;
	TLN_Palette palette;

	if (!filename)
		return 0;

	name = (*env)->GetStringUTFChars (env, filename, NULL);
	palette = (TLN_Palette)TLN_LoadPalette (name);
	(*env)->ReleaseStringUTFChars (env, filename, name);
	return (jint)palette;
}

JNIEXPORT jint JNICALL Java_Tilengine_ClonePalette (JNIEnv* env, jobject thisobj, jint src)
{
	return (jint)TLN_ClonePalette ((TLN_Palette)src);
}

JNIEXPORT void JNICALL Java_Tilengine_DeletePalette (JNIEnv* env, jobject thisobj, jint palette)
{
	TLN_DeletePalette ((TLN_Palette)palette);
}

JNIEXPORT void JNICALL Java_Tilengine_SetPaletteColor (JNIEnv* env, jobject thisobj, jint palette , jint entry, jbyte r, jbyte g, jbyte b)
{
	TLN_SetPaletteColor ((TLN_Palette)palette, entry, r, g, b);
}

JNIEXPORT void JNICALL Java_Tilengine_MixPalettes (JNIEnv* env, jobject thisobj, jint src1, jint src2, jint dst, jbyte factor)
{
	TLN_MixPalettes ((TLN_Palette)src1, (TLN_Palette)src2, (TLN_Palette)dst, factor);
}

JNIEXPORT jint JNICALL Java_Tilengine_CreateBitmap (JNIEnv* env, jobject thisobj, jint width, jint height, jint bpp)
{
	return (jint)TLN_CreateBitmap (width, height, bpp);
}

JNIEXPORT jint JNICALL Java_Tilengine_LoadBitmap (JNIEnv* env, jobject thisobj, jstring filename)
{
	char* name;
	TLN_Bitmap bitmap;

	if (!filename)
		return 0;
	
	name = (*env)->GetStringUTFChars (env, filename, NULL);
	bitmap = TLN_LoadBitmap (name);
	(*env)->ReleaseStringUTFChars (env, filename, name);
	return (jint)bitmap;
}

JNIEXPORT jint JNICALL Java_Tilengine_CloneBitmap (JNIEnv* env, jobject thisobj, jint src)
{
	return (jint)TLN_CloneBitmap ((TLN_Bitmap)src);
}

JNIEXPORT jint JNICALL Java_Tilengine_GetBitmapPalette (JNIEnv* env, jobject thisobj, jint bitmap)
{
	return (jint)TLN_GetBitmapPalette ((TLN_Bitmap)bitmap);
}

JNIEXPORT void JNICALL Java_Tilengine_DeleteBitmap (JNIEnv* env, jobject thisobj, jint bitmap)
{
	TLN_DeleteBitmap ((TLN_Bitmap)bitmap);
}

JNIEXPORT void JNICALL Java_Tilengine_SetLayer (JNIEnv* env, jobject thisobj, jint nlayer, jint tileset, jint tilemap)
{
	TLN_SetLayer (nlayer, (TLN_Tileset)tileset, (TLN_Tilemap)tilemap);
}

JNIEXPORT void JNICALL Java_Tilengine_SetLayerPalette (JNIEnv* env, jobject thisobj, jint nlayer, jint palette)
{
	TLN_SetLayerPalette (nlayer, (TLN_Palette)palette);
}

JNIEXPORT void JNICALL Java_Tilengine_SetLayerPosition (JNIEnv* env, jobject thisobj, jint nlayer, jint hstart, jint vstart)
{
	TLN_SetLayerPosition (nlayer, hstart, vstart);
}

JNIEXPORT void JNICALL Java_Tilengine_SetLayerScaling (JNIEnv* env, jobject thisobj, jint nlayer, jfloat sx, jfloat sy)
{
	TLN_SetLayerScaling (nlayer, sx, sy);
}

// TODO
JNIEXPORT void JNICALL Java_Tilengine_SetLayerAffineTransform (JNIEnv* env, jobject thisobj, jint nlayer, jobject aff)
{
}

JNIEXPORT void JNICALL Java_Tilengine_SetLayerTransform (JNIEnv* env, jobject thisobj, jint nlayer, jfloat angle, jfloat dx, jfloat dy, jfloat sx, jfloat sy)
{
	TLN_SetLayerTransform (nlayer, angle, dx, dy, sx, sy);
}

JNIEXPORT void JNICALL Java_Tilengine_SetLayerBlendMode (JNIEnv* env, jobject thisobj, jint nlayer, jint mode, jbyte factor)
{
	TLN_SetLayerBlendMode (nlayer, mode, factor);
}

JNIEXPORT void JNICALL Java_Tilengine_SetLayerColumnOffset (JNIEnv* env, jobject thisobj, jint nlayer, jintArray columns)
{
	jint* buffer;

	buffer = (*env)->GetIntArrayElements (env, columns, NULL);
	TLN_SetLayerColumnOffset (nlayer, buffer);
	(*env)->ReleaseIntArrayElements (env, columns, buffer, 0);
}

JNIEXPORT void JNICALL Java_Tilengine_ResetLayerMode (JNIEnv* env, jobject thisobj, jint nlayer)
{
	TLN_ResetLayerMode (nlayer);
}

JNIEXPORT void JNICALL Java_Tilengine_DisableLayer (JNIEnv* env, jobject thisobj, jint nlayer)
{
	TLN_DisableLayer (nlayer);
}

JNIEXPORT jint JNICALL Java_Tilengine_GetLayerPalette (JNIEnv* env, jobject thisobj, jint nlayer)
{
	return (jint)TLN_GetLayerPalette (nlayer);
}

JNIEXPORT jboolean JNICALL Java_Tilengine_GetLayerTile (JNIEnv* env, jobject thisobj, jint nlayer, jint x, jint y, jobject info)
{
	TLN_TileInfo ti = {0};
	jclass clazz;
	jfieldID fieldID;

	if (!TLN_GetLayerTile (nlayer, x, y, &ti))
		return false;

	clazz = (*env)->GetObjectClass (env, info);
	if (!clazz)
		return false;

	fieldID = (*env)->GetFieldID (env, clazz, "index", "S");
	if (fieldID)
		(*env)->SetShortField (env, info, fieldID, ti.index);

	fieldID = (*env)->GetFieldID (env, clazz, "flags", "S");
	if (fieldID)
		(*env)->SetShortField (env, info, fieldID, ti.flags);

	fieldID = (*env)->GetFieldID (env, clazz, "xoffset", "I");
	if (fieldID)
		(*env)->SetIntField (env, info, fieldID, ti.xoffset);

	fieldID = (*env)->GetFieldID (env, clazz, "yoffset", "I");
	if (fieldID)
		(*env)->SetIntField (env, info, fieldID, ti.yoffset);

	return true;
}

JNIEXPORT void JNICALL Java_Tilengine_ConfigSprite (JNIEnv* env, jobject thisobj, jint nsprite, jint spriteset, jshort flags)
{
	TLN_ConfigSprite (nsprite, (TLN_Spriteset)spriteset, flags);
}

JNIEXPORT void JNICALL Java_Tilengine_SetSpriteSet (JNIEnv* env, jobject thisobj, jint nsprite, jint spriteset)
{
	TLN_SetSpriteSet (nsprite, (TLN_Spriteset)spriteset);
}

JNIEXPORT void JNICALL Java_Tilengine_SetSpriteFlags (JNIEnv* env, jobject thisobj, jint nsprite, jshort flags)
{
	TLN_SetSpriteFlags (nsprite, flags);
}

JNIEXPORT void JNICALL Java_Tilengine_SetSpritePosition (JNIEnv* env, jobject thisobj, jint nsprite, jint x, jint y)
{
	TLN_SetSpritePosition (nsprite, x,y);
}

JNIEXPORT void JNICALL Java_Tilengine_SetSpritePicture (JNIEnv* env, jobject thisobj, jint nsprite, jint index)
{
	TLN_SetSpritePicture (nsprite, index);
}

JNIEXPORT void JNICALL Java_Tilengine_SetSpritePalette (JNIEnv* env, jobject thisobj, jint nsprite, jint palette)
{
	TLN_SetSpritePalette (nsprite, (TLN_Palette)palette);
}

JNIEXPORT void JNICALL Java_Tilengine_SetSpriteBlendMode (JNIEnv* env, jobject thisobj, jint nsprite, jint mode, jbyte factor)
{
	TLN_SetSpriteBlendMode (nsprite, mode, factor);
}

JNIEXPORT void JNICALL Java_Tilengine_SetSpriteScaling (JNIEnv* env, jobject thisobj, jint nsprite, jfloat sx, jfloat sy)
{
	TLN_SetSpriteScaling (nsprite, sx, sy);
}

JNIEXPORT void JNICALL Java_Tilengine_ResetSpriteScaling (JNIEnv* env, jobject thisobj, jint nsprite)
{
	TLN_ResetSpriteScaling (nsprite);
}

JNIEXPORT jint JNICALL Java_Tilengine_GetSpritePicture (JNIEnv* env, jobject thisobj, jint nsprite)
{
	return TLN_GetSpritePicture (nsprite);
}

JNIEXPORT jint JNICALL Java_Tilengine_GetAvailableSprite (JNIEnv* env, jobject thisobj)
{
	return TLN_GetAvailableSprite ();
}

JNIEXPORT void JNICALL Java_Tilengine_DisableSprite (JNIEnv* env, jobject thisobj, jint nsprite)
{
	TLN_DisableSprite (nsprite);
}

JNIEXPORT jint JNICALL Java_Tilengine_GetSpritePalette (JNIEnv* env, jobject thisobj, jint nsprite)
{
	return (jint)TLN_GetSpritePalette (nsprite);
}

JNIEXPORT jint JNICALL Java_Tilengine_CreateSequence (JNIEnv* env, jobject thisobj, jstring name, jint delay, jint first, jint num_frames, jintArray data)
{
	TLN_Sequence sequence = (TLN_Sequence)1;
	char* text;
	jint* buffer;

	if (!name)
		return 0;

	text = (*env)->GetStringUTFChars (env, name, NULL);
	buffer = (*env)->GetIntArrayElements (env, data, NULL);
	sequence = TLN_CreateSequence (text, delay, first, num_frames, buffer);
	(*env)->ReleaseIntArrayElements (env, data, buffer, 0);
	(*env)->ReleaseStringUTFChars (env, name, text);
	return (jint)sequence;
}

JNIEXPORT jint JNICALL Java_Tilengine_CloneSequence (JNIEnv* env, jobject thisobj, jint src)
{
	return (jint)TLN_CloneSequence ((TLN_Sequence)src);
}

JNIEXPORT void JNICALL Java_Tilengine_DeleteSequence (JNIEnv* env, jobject thisobj, jint sequence)
{
	TLN_DeleteSequence ((TLN_Sequence)sequence);
}

JNIEXPORT jint JNICALL Java_Tilengine_LoadSequencePack (JNIEnv* env, jobject thisobj, jstring filename)
{
	char* name;
	TLN_SequencePack sp;

	if (!filename)
		return 0;

	name = (*env)->GetStringUTFChars (env, filename, NULL);
	sp = TLN_LoadSequencePack (name);
	(*env)->ReleaseStringUTFChars (env, filename, name);
	return (jint)sp;
}

JNIEXPORT jint JNICALL Java_Tilengine_FindSequence (JNIEnv* env, jobject thisobj, jint sp, jstring seqname)
{
	char* name;
	TLN_Sequence sequence;

	if (!seqname)
		return 0;

	name = (*env)->GetStringUTFChars (env, seqname, NULL);
	sequence = TLN_FindSequence ((TLN_SequencePack)sp, name);
	(*env)->ReleaseStringUTFChars (env, seqname, name);
	return (jint)sequence;
}

JNIEXPORT void JNICALL Java_Tilengine_DeleteSequencePack (JNIEnv* env, jobject thisobj, jint sp)
{
	TLN_DeleteSequencePack ((TLN_SequencePack)sp);
}

JNIEXPORT void JNICALL Java_Tilengine_SetPaletteAnimation (JNIEnv* env, jobject thisobj, jint index, jint palette, jint sequence, jboolean blend)
{
	TLN_SetPaletteAnimation (index, (TLN_Palette)palette, (TLN_Sequence)sequence, blend);
}

JNIEXPORT void JNICALL Java_Tilengine_SetPaletteAnimationSource (JNIEnv* env, jobject thisobj, jint index, jint palette)
{
	TLN_SetPaletteAnimationSource (index, (TLN_Palette)palette);
}

JNIEXPORT void JNICALL Java_Tilengine_SetTilemapAnimation (JNIEnv* env, jobject thisobj, jint index, jint nlayer, jint sequence)
{
	TLN_SetTilemapAnimation (index, nlayer, (TLN_Sequence)sequence);
}

JNIEXPORT void JNICALL Java_Tilengine_SetSpriteAnimation (JNIEnv* env, jobject thisobj, jint index, jint nsprite, jint sequence, jint loop)
{
	TLN_SetSpriteAnimation (index, nsprite, (TLN_Sequence)sequence, loop);
}

JNIEXPORT jboolean JNICALL Java_Tilengine_GetAnimationState (JNIEnv* env, jobject thisobj, jint index)
{
	return TLN_GetAnimationState (index);
}

JNIEXPORT void JNICALL Java_Tilengine_SetAnimationDelay (JNIEnv* env, jobject thisobj, jint index, jint delay)
{
	TLN_SetAnimationDelay (index, delay);
}

JNIEXPORT jint JNICALL Java_Tilengine_GetAvailableAnimation (JNIEnv* env, jobject thisobj)
{
	return TLN_GetAvailableAnimation ();
}

JNIEXPORT void JNICALL Java_Tilengine_DisableAnimation (JNIEnv* env, jobject thisobj, jint index)
{
	TLN_DisableAnimation (index);
}