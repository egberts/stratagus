//   ___________		     _________		      _____  __
//   \_	  _____/______   ____   ____ \_   ___ \____________ _/ ____\/  |_
//    |    __) \_  __ \_/ __ \_/ __ \/    \  \/\_  __ \__  \\   __\\   __\ 
//    |     \   |  | \/\  ___/\  ___/\     \____|  | \// __ \|  |   |  |
//    \___  /   |__|    \___  >\___  >\______  /|__|  (____  /__|   |__|
//	  \/		    \/	   \/	     \/		   \/
//  ______________________                           ______________________
//			  T H E   W A R   B E G I N S
//	   FreeCraft - A free fantasy real time strategy game engine
//
/**@name tileset.h	-	The tileset headerfile. */
//
//	(c) Copyright 1998-2001 by Lutz Sammer
//
//	$Id$

#ifndef __TILESET_H__
#define __TILESET_H__

//@{

/*----------------------------------------------------------------------------
--	Documentation
----------------------------------------------------------------------------*/

/**
**	@struct _tileset_ tileset.h
**
**	\#include "tileset.h"
**
**	typedef struct _tileset_ Tileset;
**
**	This structure contains all informations about the tileset of the map.
**	It defines the look and properties of the tiles. Currently only one
**	tileset pro map is supported. In the future it is planned to support
**	multiple tilesets on the same map. Also is planned to support animated
**	tiles.
**	Currently the tilesize is fixed to 32x32 pixels, to support later other
**	sizes please use always the constants ::TileSizeX and ::TileSizeY.
**
**	The tileset structure members:
**
**	Tileset::Ident
**
**		FIXME: continue docu.
**
**	Tileset::Name
**
**	Tileset::File
**
**	Tileset::PaletteFile
**
**	Tileset::Table
**
**	Tileset::TileTypeTable
**
**	Tileset::AnimationTable
**
**	Tileset::ExtraTrees[6]
**
**	Tileset::TopOneTree
**
**	Tileset::MidOneTree
**
**	Tileset::BotOneTree
**
**	Tileset::RemovedTree
**
**	Tileset::GrowingTree[2]
**
**	Tileset::ExtraRocks[6]
**
**	Tileset::TopOneRock
**
**	Tileset::MidOneRock
**
**	Tileset::BotOneRock
**
**	Tileset::RemovedRock
**
*/

/*----------------------------------------------------------------------------
--	Declarations
----------------------------------------------------------------------------*/

#define TileSizeX	32		/// Size of a tile in X
#define TileSizeY	32		/// Size of a tile in Y

#define MaxTilesInTileset	3072	/// Current limit of tiles in tileset

/**
**   These are used for lookup tiles types
**   mainly used for the FOW implementation of the seen woods/rocks
*/
enum _tile_type_ {
    TileTypeUnknown,			/// unknown tile type
    TileTypeNoWood,			/// UNUSED: no wood tile
    TileTypeWood,			/// any wood tile
    TileTypeGrass,			/// any grass tile
    TileTypeNoRock,			/// UNUSED: no rock tile
    TileTypeRock,			/// any rock tile
    TileTypeCoast,			/// any coast tile
    TileTypeHumanWall,			/// any human wall tile
    TileTypeOrcWall,			/// any orc wall tile
    TileTypeNoWall,			/// UNUSED: no wall tile
    TileTypeWater,			/// any water tile
};

    ///	Tileset definition.
typedef struct _tileset_ {
    char*	Ident;			/// tileset identifier
    char*	Name;			/// name for future extensions
    char*	File;			/// file containing image data
    char*	PaletteFile;		/// file containing the global palette

    const unsigned short* Table;	/// pud to internal conversion table
    unsigned char* TileTypeTable;	/// for fast lookup of tile type
    // FIXME: currently unsupported.
    unsigned short* AnimationTable;	/// Tile Animation sequences

    unsigned	ExtraTrees[6];		/// extra tree tiles for removing
    unsigned	TopOneTree;		/// tile for one tree top
    unsigned	MidOneTree;		/// tile for one tree middle
    unsigned	BotOneTree;		/// tile for one tree bottom
    unsigned	RemovedTree;		/// tile placed where trees are gone
    unsigned	GrowingTree[2];		/// Growing tree tiles

    unsigned	ExtraRocks[6];		/// extra rock tiles for removing
    unsigned	TopOneRock;		/// tile for one rock top
    unsigned	MidOneRock;		/// tile for one rock middle
    unsigned	BotOneRock;		/// tile for one rock bottom
    unsigned	RemovedRock;		/// tile placed where rocks are gone
} Tileset;

// FIXME: this #define's should be removed

#define TilesetSummer		0	/// Reference number
#define TilesetWinter		1	/// Reference number
#define TilesetWasteland	2	/// Reference number
#define TilesetSwamp		3	/// Reference number

// FIXME: allow more tilesets

#define TilesetMax		4	/// Biggest supported tilset number

/*----------------------------------------------------------------------------
--	Variables
----------------------------------------------------------------------------*/

extern char** TilesetWcNames;		/// Mapping wc-number 2 symbol
extern Tileset Tilesets[TilesetMax];	/// Tileset information

/*----------------------------------------------------------------------------
--	Functions
----------------------------------------------------------------------------*/

extern void LoadTileset(void);		/// Load tileset definition
extern void SaveTileset(FILE*);		/// Save the tileset configuration
extern void CleanTileset(void);		/// Cleanup the tileset module

extern void TilesetCclRegister(void);	/// Register CCL features for tileset

/*----------------------------------------------------------------------------
--	Predicates
----------------------------------------------------------------------------*/

    /// True if this is the fog color. FIXME: fgp compatibility hack!
#define COLOR_FOG_P(x) ((x) == 239 || (x) == 228)
    /// Solid fog color number in global palette
#define COLOR_FOG (0)

//@}

#endif // !__TILESET_H__
