#pragma once

#include <cstdint>

using ULONG = uint32_t;
using LONG = int32_t;
using undefined = int;
using undefined1 = int;
using undefined2 = int;
using undefined3 = int;
using undefined4 = int;
using uint = unsigned int;
using byte = char;
using u_char = unsigned char;

/*
* All of these functions are pseudo-functions and do not work, as they are only used to illustrate the documentation of the Playstation with the MIPS architecture.
* The documentation can be found here:
* https://psx.arthus.net/sdk/Psy-Q/DOCS/Devrefs/Libref.pdf
*/

namespace PSX {

	/// <summary>
	/// Time-code based CD-ROM disc position.
	/// <para>Defines a time-code position on a CD-ROM, based on the time needed to reach that position when playing the disc from the beginning at normal speed.</para>
	/// <para>The track member is not used at present.</para>
	/// <para>Use CdlSetloc command of CdControl() to set.</para>
	/// </summary>
	struct CdlLOC {
		/// <summary>
		/// Minute
		/// </summary>
		unsigned char minute;
		/// <summary>
		/// Second
		/// </summary>
		unsigned char second;
		/// <summary>
		/// Sector
		/// </summary>
		unsigned char sector;
		/// <summary>
		/// Track number
		/// </summary>
		unsigned char track;
	};

	/// <summary>
	/// ISO-9660 file system file descriptor.
	/// <para>Position and size of ISO-9660 CD-ROM file.</para>
	/// </summary>
	struct CdlFILE {
		/// <summary>
		/// File position
		/// </summary>
		PSX::CdlLOC pos;
		/// <summary>
		/// File size
		/// </summary>
		unsigned long size;
		/// <summary>
		/// File name
		/// </summary>
		char name[16];
	};

	/// <summary>
	/// Volume
	/// <para>Used in attributes that require L channel/R channel values when setting/getting each voice.</para>
	/// </summary>
	struct SpuVolume {
		/// <summary>
		/// L channel value
		/// </summary>
		short left;
		/// <summary>
		/// R channel value
		/// </summary>
		short right;
	};

	/// <summary>
	/// External input attributes.
	/// <para>Used when setting/checking CD and external digital input attributes.</para>
	/// </summary>
	struct SpuExtAttr {
		/// <summary>
		/// Volume
		/// </summary>
		SpuVolume volume;
		/// <summary>
		/// Reverb on/off
		/// </summary>
		long reverb;
		/// <summary>
		/// Mixing on/off
		/// </summary>
		long mix;
	};

	/// <summary>
	/// Common attributes.
	/// <para>Used when setting/checking common attributes. 
	/// The members needed for setting are set as bit values in mask.</para>
	/// </summary>
	struct SpuCommonAttr {
		/// <summary>
		/// Set mask
		/// </summary>
		unsigned long mask;
		/// <summary>
		/// Master volume
		/// </summary>
		PSX::SpuVolume mvol;
		/// <summary>
		/// Master volume mode
		/// </summary>
		PSX::SpuVolume mvolmode;
		/// <summary>
		/// Current master volume
		/// </summary>
		PSX::SpuVolume mvolx;
		/// <summary>
		/// Cd input attributes
		/// </summary>
		PSX::SpuExtAttr cd;
		/// <summary>
		/// External digital input attributes
		/// </summary>
		PSX::SpuExtAttr ext;
	};

	/// <summary>
	/// Frame buffer rectangular area.
	/// <para>Used by several library functions to specify a rectangular area of the frame buffer. 
	/// Neither negative values, nor values exceeding the size of the frame buffer(1024x512), may be specified.</para>
	/// </summary>
	struct RECT {
		/// <summary>
		/// Top left coordinates of the rectangular area
		/// </summary>
		short x, y;
		/// <summary>
		/// Width and height of the rectangular area
		/// </summary>
		short w, h;
	};

	/// <summary>
	/// Drawing environment modification primitive.
	/// <para>Changes the drawing environment (DRAWENV) while a primitive list is being drawn. 
	/// Use SetDrawEnv() to specify the new DRAWENV parameters.</para>
	/// <para>This primitive affects only the drawing environment, not the display environment (see DISPENV). 
	/// The entire drawing environment may be changed using this primitive; see also the DR_MODE primitive, 
	/// which sets a subset of the drawing environment.</para>
	/// </summary>
	struct DR_ENV {
		/// <summary>
		/// Pointer to the next primitive in primitive list
		/// </summary>
		ULONG* tag;
		/// <summary>
		/// New drawing environment information specified by SetDrawEnv()
		/// </summary>
		ULONG code[15];
	};

	/// <summary>
	/// Drawing environment.
	/// <para>Sets basic drawing parameters, such as drawing offset and drawing clip area.</para>
	/// <para>The GPU uses 8 bits for R, G, B internally; when writing to the frame buffer, each value is reduced to 5 bits. 
	/// When dtd is ON, a 4x4 dither matrix is used as follows : </para>
	/// <para>See Tables below</para>
	/// </summary>
	struct DRAWENV {
		/// <summary>
		/// Drawing area. Drawing is restricted to the area specified by clip. 
		/// It must be within the area area(0, 0) - (1023, 511).
		/// </summary>
		RECT clip;
		/// <summary>
		/// The offsets ofs[0] and ofs[1] are added to the X and Y values, 
		/// respectively, of all primitives before drawing. Note: Addresses after 
		/// adding offsets are wrapped around at(-1024, -1024) - (1023, 1023).
		/// </summary>
		short ofs[2];
		/// <summary>
		/// Texture window. Specifies a rectangle inside the texture page, to be 
		/// used for drawing textures.
		/// </summary>
		RECT tw;
		/// <summary>
		/// Initial value of texture page
		/// </summary>
		unsigned short tpage;
		/// <summary>
		/// Dithering processing flag. 0: off; 1: on
		/// </summary>
		unsigned char dtd;
		/// <summary>
		/// <para>0: drawing to display area is blocked</para>
		/// <para>1: drawing to display area is permitted</para>
		/// </summary>
		unsigned char dfe;
		/// <summary>
		/// <para>0: Does not clear drawing area when drawing environment is set.</para>
		/// <para>1: Paints entire clip area with brightness values (r0, g0, b0) when drawing environment is set.</para>
		/// </summary>
		unsigned char isbg;
		/// <summary>
		/// Background color. Valid only when isbg is 1.
		/// </summary>
		unsigned char r0, g0, b0;
		/// <summary>
		/// System reserved
		/// </summary>
		DR_ENV dr_env;
	};

	/*
	* i = 8 bit brightness value + 1/2 * D - 4
	* D = Dither matrix [X%4][Y%4]
	* 
	* 4x4 Dither Matrix
	* +-----------------------------+
	* | 0       8       2       10  |
	* | 12      4       14      6   |
	* | 3       11      1       9   |
	* | 15      7       13      5   |
	* +-----------------------------+
	* 
	* 5 bit brightness value = 1 >> 3
	* The values which may be specified for the texture window are restricted to the following combinations:
	* +-------------------------------------------------------------------------------------------------+
	* | tw.w, tw.x                                                                                      |
	* +-------------------------------------------------------------------------------------------------+
	* | tw.w    0(=256)     8               16              32              64              128         |
	* | tw.x    0           Multiple of     Multiple of     Multiple of     Multiple of     Multiple of |
	* |                     8               16              32              64              128         |
	* +-------------------------------------------------------------------------------------------------+
	* | tw.w, tw.x                                                                                      |
	* +-------------------------------------------------------------------------------------------------+
	* | tw.h    0(=256)     8               16              32              64              128         |
	* | tw.y    0           Multiple of     Multiple of     Multiple of     Multiple of     Multiple of |
	* |                     8               16              32              64              128         |
	* +-------------------------------------------------------------------------------------------------+
	*/

	/// <summary>
	/// Display environment.
	/// <para>Specifies display parameters for screen display mode, frame buffer display value, and so on.</para>
	/// </summary>
	struct DISPENV {
		/// <summary>
		/// Display area within frame buffer. Width: 256, 320, 384, 512, or 640. Height: 240 or 480.
		/// </summary>
		RECT disp;
		/// <summary>
		/// Output screen display area. 
		/// It is calculated without regard to the value of disp, 
		/// using the standard monitor screen upper left - hand point(0, 0) and lower right - hand point(256, 240).
		/// </summary>
		RECT screen;
		/// <summary>
		/// Interlace mode flag. 0: non-interlace; 1: interlace
		/// </summary>
		unsigned char isinter;
		/// <summary>
		/// 24-bit mode flag. 0: 16-bit mode; 1: 24-bit mode
		/// </summary>
		unsigned char isrgb24;
		/// <summary>
		/// Reserved by system
		/// </summary>
		unsigned char pad0, pad1;
	};

	/// <summary>
	/// Sprite of any desired size.
	/// <para>Draws a texture-mapped rectangular area. Drawing speed for a SPRT primitive is faster than for a POLY_FT4.</para>
	/// <para>Only even numbers can be specified for u0 and w.</para>
	/// <para>Because the SPRT primitive has no tpage parameter, the texture page of the current drawing environment is used. 
	/// You can change the texture page by inserting a DR_TPAGE or DR_MODE primitive into the primitive list before your SPRT primitive.</para>
	/// </summary>
	struct SPRT {
		/// <summary>
		/// Pointer to next primitive (reserved)
		/// </summary>
		ULONG* tag;
		/// <summary>
		/// ; RGB color values for sprite
		/// </summary>
		unsigned char r0, g0, b0;
		/// <summary>
		/// Primitive code (reserved)
		/// </summary>
		unsigned char code;
		/// <summary>
		/// Position of sprite (top left coordinate)
		/// </summary>
		short x0, y0;
		/// <summary>
		/// Position of sprite texture within the texture page (top left coordinate). 
		/// u0 should be an even number.
		/// </summary>
		unsigned char u0, v0;
		/// <summary>
		/// CLUT ID used (for 4-bit/8-bit mode only)
		/// </summary>
		unsigned short clut;
		/// <summary>
		/// Width and height of sprite. w is an even number
		/// </summary>
		short w, h;
	};

	/// <summary>
	/// Texture page change primitive.
	/// <para>Changes the texture page parameter of the current drawing environment while a primitive list is being drawn. 
	/// See the tpage member of the DRAWENV structure for more information.Use SetDrawTPage() to specify the parameters to be used.</para>
	/// </summary>
	struct DR_TPAGE {
		/// <summary>
		/// Pointer to the next primitive in primitive list
		/// </summary>
		ULONG* tag;
		/// <summary>
		/// New texture page information specified by SetDrawTPage()
		/// </summary>
		ULONG* code[2];
	};

	/// <summary>
	/// Flat-shaded, texture-mapped triangle/ Flat-shaded, texture-mapped quad.
	/// <para>POLY_FT4 draws an area demarcated by (x0, y0) - (x1, y1) - (x3, y3) - (x2, y2) while mapping the area 
	/// demarcated by(u0, v0) - (u1, v1) - (u3, v3) - (u2, v2) in the texture pattern on the texture page tpage.</para>
	/// <para>The actual brightness value for drawn graphics are obtained by multiplying the RGB color values from the 
	/// texture pattern by the RGB color values given by r0, g0, b0.</para>
	/// <para>The texture coordinates are the coordinates (0 to 255) inside the texture page corresponding to the vertices of the triangle to be drawn. 
	/// if the texture mode is 4 - bit or 8 - bit, the texture coordinates and the actual frame buffer address are not 1 - to - 1. </para>
	/// <para>Texture page ID is given to tpage. Using GetTPage(), the texture page ID is obtained from the address (x, y) of the buffer frame where the texture page is located.</para>
	/// <para>A texture using CLUT gives CLUT ID to be set in clut. Using GetClut(), CLUT ID is obtained from the address(x, y) of the frame buffer where CLUT is located.</para>
	/// <para>The size of the texture page which can be used by one drawing command is 256 x 256. One primitive can only use one texture page.</para>
	/// <para>In the case of a quadrilateral primitive, the corners are specified in the order shown below. 
	/// The same applies to designation of(u, v) for a texture map rectangle, and (r, g, b) for a Gouraud shaded rectangle.</para>
	/// </summary>
	struct POLY_FT4 {
		/// <summary>
		/// Pointer to the next primitive
		/// </summary>
		ULONG* tag; 
			/// <summary>
			/// RGB color values
			/// </summary>
			unsigned char r0, g0, b0; 
			/// <summary>
			/// Primitive ID(reserved)
			/// </summary>
			unsigned char code; 
			/// <summary>
			/// Vertex coordinates
			/// </summary>
			short x0, y0; 
			/// <summary>
			/// Texture coordinates
			/// </summary>
			unsigned char u0, v0; 
			/// <summary>
			/// CLUT ID(color - look - up table for 4 - bit / 8 - bit mode only)
			/// </summary>
			unsigned short clut; 
			/// <summary>
			/// Vertex coordinates
			/// </summary>
			short x1, y1; 
			/// <summary>
			/// Texture coordinates
			/// </summary>
			unsigned char u1, v1; 
			/// <summary>
			/// Texture page ID
			/// </summary>
			unsigned short tpage;
			/// <summary>
			/// Vertex coordinates
			/// </summary>
			short x2, y2; 
			/// <summary>
			/// Texture coordinates
			/// </summary>
			unsigned char u2, v2; 
			/// <summary>
			/// Reserved by the system
			/// </summary>
			unsigned short pad1; 
			/// <summary>
			/// Vertex coordinates
			/// </summary>
			short x3, y3; 
			/// <summary>
			/// Texture coordinates
			/// </summary>
			unsigned char u3, v3; 
			/// <summary>
			/// Reserved by the system
			/// </summary>
			unsigned short pad2; 
	};

	/*
	* (x0, y0)                    (x1, y1)
	*    +----------------------------+
	*    |                            |
	*    |                            |
	*    |                            |
	*    |                            |
	*    |                            |
	*    |                            |
	*    |                            |
	*    +----------------------------+
	* (x2, y2)                    (x3, y3)
	*/

	/// <summary>
	/// Drawing mode modification primitive
	/// <para>Changes the texture page, texture window, dithering flag, and drawing flag parameters of the current drawing environment while a primitive list is being drawn. 
	/// See the tpage, tw, dtd, and dfe members of the DRAWENV structure for more information.Use SetDrawMode() to specify the parameters to be used.</para>
	/// </summary>
	struct DR_MODE {
		/// <summary>
		/// Pointer to the next primitive in primitive list
		/// </summary>
		ULONG* tag;
		/// <summary>
		/// New drawing environment information as specified by SetDrawMode()
		/// </summary>
		ULONG code[2];
	};

	/// <summary>
	/// Tile of any desired size.
	/// <para>Draws a rectangular area with the specified RGB color value (r0, g0, b0). No texture mapping or shading is done. 
	/// It is faster than the POLY_F4 primitive.</para>
	/// </summary>
	struct TILE {
		/// <summary>
		/// Pointer to next primitive(reserved)
		/// </summary>
		ULONG* tag;
		/// <summary>
		/// RGB color values for sprite
		/// </summary>
		u_char r0, g0, b0;
		/// <summary>
		/// Primitive code(reserved)
		/// </summary>
		u_char code;
		/// <summary>
		/// Position of sprite(top left coordinate)
		/// </summary>
		short x0, y0;
		/// <summary>
		/// Width and height of sprite.w is an even number
		/// </summary>
		short w, h;
	};

	/// <summary>
	/// Waits for vertical blank using the method specified by mode.<para/>
	/// Vsync() may generate a timeout if long blocking periods are specified. To prevent deadlocks, rather than
	/// using Vsync() to block for an especially long time(say more than 4 vertical blank periods), have your
	/// program poll VSync(-1) in a loop instead.
	/// </summary>
	/// <param name="mode">
	/// <para/>
	/// <para>0 - Blocks until vertical sync is generated</para>
	/// <para>1 - Returns time elapsed from the point VSync() processing is last completed when mode = 1 or n in horizontal sync units</para>
	/// <para>n (n>1) - Blocks from the point VSync() processing is last completed when mode = 1 or n until n number of vertical syncs are generated.</para>
	/// <para>-n (n>0) - Returns absolute time after program boot in vertical sync interval units.</para>
	/// </param>
	/// <returns>
	/// Mode value is as listed below:<para/>
	/// <para>mode>=0 - Time elapsed from the point that Vsync() processing is last completed when mode = 1 or n(horizontal blanking units)</para>
	/// <para>mode&lt;0 - Time elapsed after program boot (vertical blanking units)</para>
	/// </returns>
	int VSync(int mode) { return 0; };

	/// <summary>
	/// Initialize all callbacks.<para/>
	/// Initializes all system callbacks. Sets all callback functions to 0 (unregistered), and after securing the interrupt context stack, 
	/// sets up the environment for accepting interrupts.<para/>
	/// ResetCallback() must be called after program boot, before any other processing is performed.<para/> 
	/// The environment initialized by ResetCallback() remains valid until StopCallback() is called.<para/>
	/// It is acceptable to continuously call ResetCallback() without StopCallback(). However, the second and subsequent calls are ignored.
	/// </summary>
	void ResetCallback() {};

	/// <summary>
	/// Initialize drawing engine<para/>
	/// Resets the graphic system according to mode
	/// </summary>
	/// <param name="mode">
	/// <para>0 -  Complete reset. The drawing environment and display environment are initialized.</para>
	/// <para>1 - Cancels the current drawing and flushes the command buffer.</para>
	/// <para>3 - Initializes the drawing engine while preserving the current display environment (i.e.the screen is not cleared or the screen mode changed).</para>
	/// </param>
	/// <returns>not documented</returns>
	int ResetGraph(int mode) { return 0; }

	/// <summary>
	/// Initialize CD-ROM system.<para/>
	/// <para>Resets the CD-ROM subsystem. The mode parameter is not used by current versions of the library and should be set to 0. </para>
	/// <para>When initialization fails, up to four retries are performed. Since CdInit() and CdReset() reset the SPU sound volume and CD input volume to the SPU, 
	/// etc., they must be called before libspu / libsnd initialization and setting functions.</para>
	/// </summary>
	/// <param name="mode">Reset mode (use 0)</param>
	/// <returns>1 if initialization is successful; 0 on failure.</returns>
	void CdInit(int mode) {}

	/// <summary>
	/// Turn off Reverb
	/// <para>Turns off global Reverb</para>
	/// </summary>
	void SsUtReverbOff() {}

	/// <summary>
	/// Initialize a controller (for prototyping only).
	/// <para>Initializes the controller. Since this function supports only the 16-button controller, it should be used for prototyping purposes only.</para>
	/// </summary>
	/// <param name="mode">Always pass 0</param>
	void PadInit(int mode) {}

	/// <summary>
	/// Set debug level.
	/// <para>Set debug level for CD-ROM subsystem</para>
	/// </summary>
	/// <param name="level">
	/// <para>0: No checks performed</para>
	/// <para>1: Check primitive commands</para>
	/// <para>2: Print execution status of primitive commands</para>
	/// </param>
	/// <returns>Previously set debug mode</returns>
	int CdSetDebug(int level) { return 0; }

	/// <summary>
	/// Issue primitive command to CD-ROM system.
	/// <para>Issues the primitive command com to the CD-ROM system. param points to the arguments of the command, if any; 
	/// set param to 0 for commands that do not require arguments.result points to a buffer used to hold the return value; 
	/// if result is 0, the return value is not stored.</para>
	/// <para>The values of command (com), arguments (param), and return value (result) are listed below. For the functions that are non - blocking, 
	/// the actual transmission completion must be detected by CdSync()</para>
	/// </summary>
	/// <param name="com">Command code</param>
	/// <param name="param">Pointer to command arguments</param>
	/// <param name="result">Pointer to return value storage buffer (requires 8 bytes)</param>
	/// <returns>1 if the command is issued successfully; 0 if failed.</returns>
	int CdControl(unsigned char com, unsigned char* param, unsigned char* result) { return 0; }

	/// <summary>
	/// Issue primitive command to CD-ROM system (Blocking-type function).
	/// <para>Issues the primitive command com to the CD-ROM system. param points to the arguments of the command, if any; 
	/// set param to 0 for commands that do not require arguments.result points to a buffer used to hold the return value; 
	/// if result is 0, the return value is not stored.</para>
	/// <para>CdControlB() is identical to CdControl() except that it is blocking; that is, it waits for all commands to terminate before returning.
	/// For details, see CdControl().</para>
	/// </summary>
	/// <param name="com">Command code</param>
	/// <param name="param">Pointer to command arguments</param>
	/// <param name="result">Pointer to return value storage buffer (requires 8 bytes)</param>
	/// <returns>1 if issued successfully; 0 if failed</returns>
	int CdControlB(unsigned char com, unsigned char* param, unsigned char* result) { return 0; }

	/*
	* Commands for CdControl, CdControlB, CdControlF
	* 
	* Primitive Commands:
	* +----------------+--------+-------------+------------------------------------+
	* | Symbol         | Code   | Type        | Contents                           |
	* +----------------+--------+-------------+------------------------------------+
	* | CdlNop         | 0x01   | Blocking    | NOP (No Operation)                 |
	* | CdlSetloc      | 0x02   | Blocking    | Set the seek target position       |
	* | CdlPlay        | 0x03   | Blocking    | Commence CD-DA play                |
	* | CdlForward     | 0x04   | Blocking    | Forward                            |
	* | CdlBackward    | 0x05   | Blocking    | Rewind                             |
	* | CdlReadN       | 0x06   | Blocking    | Start data read (with retry)       |
	* | CdlStandby     | 0x07   | Nonblocking | Stand by with disk rotating        |
	* | CdlStop        | 0x08   | Nonblocking | Disk stopped                       |
	* | CdlPause       | 0x09   | Nonblocking | Pause at current position          |
	* | CdlMute        | 0x0b   | Blocking    | CD-DA mute                         |
	* | CdlDemute      | 0x0c   | Blocking    | Cancel mute                        |
	* | CdlSetfilter   | 0x0d   | Blocking    | Choose ADPCM play sector           |
	* | CdlSetmode     | 0x0e   | Blocking    | Set basic mode (see Table below)   |
	* | CdlGetparam    | 0x0f   | Blocking    | Gets current CD subsystem mode     |
	* | CdlGetlocL     | 0x10   | Blocking    | Get logical location (data sector) |
	* | CdlGetlocP     | 0x11   | Blocking    | Get physical location (audio sec.) |
	* | CdlSeekL       | 0x15   | Nonblocking | Logical seek (data sector seek)    |
	* | CdlSeekP       | 0x16   | Nonblocking | Physical seek (audio sector seek)  |
	* | CdlReadS       | 0x1b   | Blocking    | Commence data read (no retry)      |
	* +----------------+--------+-------------+------------------------------------+
	* 
	* Primitive Commands with Arguments
	* +----------------+-----------+--------------------------------+
	* | Symbol         | Argument  | Contents                       |
	* +----------------+-----------+--------------------------------+
	* | CdlSetloc      | CdlLOC    | Start sector location          |
	* | CdlReadN       | CdlLOC    | Start sector location          |
	* | CdlReadS       | CdlLOC    | Start sector location          |
	* | CdlPlay        | CdlLOC    | Start sector location          |
	* | CdlSetfilter   | CdlFILTER | Set ADPCM sector play          |
	* | CdlSetmode     | u_char    | Set basic mode                 |
	* +----------------+-----------+--------------------------------+
	* 
	* Return Values of Primitive Commands
	* +-------------+--------+-------+--------+--------+--------+--------+--------+--------+
	* | Symbol      | 0      | 1     | 2      | 3      | 4      | 5      | 6      | 7      |
	* +-------------+--------+-------+--------+--------+--------+--------+--------+--------+
	* | CdlGetparam | Status | Mode  |        |        |        |        |        |        |
	* | CdlGetlocL  | Min    | Sec   | Sector | Mode   | File   | Chan   |        |        |
	* | CdlGetlocP  | Track  | Index | Min    | Sec    | Frame  | Amin   | Asec   | Aframe |
	* | CdlSeekL    | Status | Btrack| Etrack |        |        |        |        |        |
	* | CdlSeekP    | Status | Min   | Sec    |        |        |        |        |        |
	* +-------------+--------+-------+--------+--------+--------+--------+--------+--------+
	* 
	* CD Modes (Command argument)
	* +-----------------+--------+----------------------------------------------+
	* | Symbol          | Code   | Details                                      |
	* +-----------------+--------+----------------------------------------------+
	* | CdlModeStream   | 0x100  | Normal streaming                             |
	* | CdlModeStream2  | 0x120  | SUB HEADER information includes              |
	* | CdlModeSpeed    | 0x80   | Transfer speed                               |
	* |                 |        | 0: Normal speed                              |
	* |                 |        | 1: Double speed                              |
	* | CdlModeRT       | 0x40   | ADPCM play                                   |
	* |                 |        | 0: ADPCM OFF                                 |
	* |                 |        | 1: ADPCM ON                                  |
	* | CdlModeSize1    | 0x20   | Sector size                                  |
	* |                 |        | 0: 2048 byte                                 |
	* |                 |        | 1: 2340 byte                                 |
	* | CdlModeSize0    | 0x10   | Sector size                                  |
	* |                 |        | 0: —                                         |
	* |                 |        | 1: 2328 byte                                 |
	* | CdlModeSF       | 0x08   | Subheader filter                             |
	* |                 |        | 0: Off                                       |
	* |                 |        | 1: On                                        |
	* | CdlModeRept     | 0x04   | Report mode                                  |
	* |                 |        | 0: Off                                       |
	* |                 |        | 1: On                                        |
	* | CdlModeAP       | 0x02   | Autopause                                    |
	* |                 |        | 0: Off                                       |
	* |                 |        | 1: On                                        |
	* | CdlModeDA       | 0x01   | CD-DA play                                   |
	* |                 |        | 0: CD-DA off                                 |
	* |                 |        | 1: CD-DA on                                  |
	* +-----------------+--------+----------------------------------------------+
	*/

	/// <summary>
	/// Get location and size from CD-ROM file name
	/// <para>Determines the position time code (minutes, seconds, sectors) and total length of the specified file on the CD - ROM.
	/// The result is stored in the CdlFILE structure pointed to by fp.</para>
	/// <para>name must be a complete path to the file.</para>
	/// <para>CdSearchFile() caches directory information, so subsequent consecutive calls for files in the same directory do not require additional CD - ROM reads.
	/// Only one directory is cached at a time, and reading information for a file in another directory invalidates the entire cache.</para>
	/// <para>For the best possible performance, include file location and size information in your program at compile time instead of using CdSearchFile().</para>
	/// </summary>
	/// <param name="fp">Pointer to CD-ROM file structure pointer</param>
	/// <param name="name">Pointer to a file name</param>
	/// <returns>Pointer to the CD-ROM file structure obtained; 0 if file not found.</returns>
	PSX::CdlFILE* CdSearchFile(CdlFILE* fp, char* name) { return 0; }

	/// <summary>
	/// Translate time code to an absolute sector number.
	/// <para>Translates a minutes/seconds/sectors time code contained in a cdlLOC structure pointed to by the p parameter into an absolute sector value.</para>
	/// </summary>
	/// <param name="p">Pointer to a CdlLOC structure containing the position time code</param>
	/// <returns>Absolute sector number</returns>
	int CdPosToInt(CdlLOC* p) { return 0; }

	/// <summary>
	/// Initialize Memory Card BIOS.
	/// <para>Initializes the Memory Card BIOS and enters an idle state. val specifies whether or not there is sharing with the controller. (0: not shared; 1: shared.) </para>
	/// <para>When the BIOS is subsequently put into operation by StartCARD(), the low-level interface functions that begin with “ _card” can be used directly.</para>
	/// <para>The Memory Card file system uses these interfaces internally, so InitCARD() needs to be executed before _bu_init().</para>
	/// <para>There is no effect on the controller.</para>
	/// </summary>
	/// <param name="val">Specify sharing with controller</param>
	void InitCARD(long val) {}

	/// <summary>
	/// Start Memory Card BIOS.
	/// <para>Changes the Memory Card BIOS initialized by InitCARD() to a run state.</para>
	/// <para>Performs ChangeClearPAD(1) internally.</para>
	/// </summary>
	void StartCARD() {}

	/// <summary>
	/// Initialize Memory Card file system.
	/// <para>Initializes the Memory Card file system. This file system is not initialized automatically, so it is necessary to call this function.</para>
	/// </summary>
	void _bu_init() {}

	/// <summary>
	/// Set automatic format function.
	/// <para>When val is 0, the automatic format function is disabled; when val is 1, it is enabled.</para>
	/// <para>This function should be used for testing purposes only.</para>
	/// </summary>
	/// <param name="val">Indicates automatic formatting</param>
	/// <returns>Previously set automatic format value.</returns>
	long _card_auto(long val) { return 0; }

	/// <summary>
	/// Disable interrupts.
	/// <para>Disables interrupts (enters a critical section).</para>
	/// <para>Executes an internal system call and destroys the interrupt context. 
	/// However, does not call the main function from the event handler callback interrupt context.</para>
	/// </summary>
	/// <returns>0 when this function is called in a critical section, 1 otherwise</returns>
	void EnterCriticalSection() {}

	/// <summary>
	/// Enable interrupts.
	/// <para>Enables interrupts (exits from a critical section).</para>
	/// <para>Executes an internal system call and destroys the interrupt context. 
	/// However, it does not call the main function from the event handler callback interrupt context.</para>
	/// </summary>
	void ExitCriticalSection() {}

	/// <summary>
	/// Open an event.
	/// <para>Secures an EvCB for an event with the descriptor desc and event class spec. 
	/// Must be executed in a critical section.</para>
	/// </summary>
	/// <param name="desc">Cause descriptor</param>
	/// <param name="spec">Event type</param>
	/// <param name="mode">Mode</param>
	/// <param name="func">Pointer to the handler function</param>
	/// <returns>The event descriptor, if the function succeeds. On failure, it returns -1.</returns>
	long OpenEvent(unsigned long desc, long spec, long mode, long* func) { return 0; }

	/// <summary>
	/// Enable occurrence of an event.
	/// <para>Enables occurrence of an event specified by the descriptor event. 
	/// It changes the event status to EvStACTIVE(event not yet generated, generation possible).</para>
	/// </summary>
	/// <param name="event">Event descriptor</param>
	/// <returns>1 on success, 0 on failure.</returns>
	long EnableEvent(unsigned long event) { return 0; }

	/// <summary>
	/// Set sound system attributes
	/// <para>Sets sound system attributes. 
	/// Specify the attributes (members of attr) by ORing together the terms shown below in attr.mask.
	/// If attr.mask is 0, all attributes are set.</para>
	/// <para></para>
	/// </summary>
	/// <param name="attr">Pointer to attributes common to all voices</param>
	void SpuSetCommonAttr(PSX::SpuCommonAttr *attr) {}

	/*
	* SpuSetCommonAttr Table
	* +--------------------------+------------------------------------------+
	* | Attribute                | Description                              |
	* +--------------------------+------------------------------------------+
	* | SPU_COMMON_MVOLL         | Master volume (left)                     |
	* | SPU_COMMON_MVOLR         | Master volume (right)                    |
	* | SPU_COMMON_MVOLMODEL     | Master volume mode (left)                |
	* | SPU_COMMON_MVOLMODER     | Master volume mode (right)               |
	* | SPU_COMMON_CDVOLL        | CD input volume (left)                   |
	* | SPU_COMMON_CDVOLR        | CD input volume (right)                  |
	* | SPU_COMMON_CDREV         | CD input reverb ON/OFF                   |
	* | SPU_COMMON_CDMIX         | CD input ON/OFF                          |
	* | SPU_COMMON_EXTVOLL       | External digital input volume (left)     |
	* | SPU_COMMON_EXTVOLR       | External digital input volume (right)    |
	* | SPU_COMMON_EXTREV        | External digital input reverb ON/OFF     |
	* | SPU_COMMON_EXTMIX        | External digital input ON/OFF            |
	* +--------------------------+------------------------------------------+
	* 
	* The individual parameters that can be set are:
	* a) Master Volume (attr.mvol ) and Master Volume Mode (attr.mvolmode) Left and right are set independently. 
	*    The volume range obtainable and the various modes are the same as the settings for each voice; see Table 15–7 under SpuSetVoiceAttr(). 
	* b) CD Input Volume (attr.cd.volume) Set independently for left and right in the range -0x8000 - 0x7fff. If volume is negative, the phase is inverted.
	* c) CD Input Reverb On/Off (attr.cd.volume) SPU_ON = reverb on; SPU_OFF = reverb off
	* d) CD Input Mixing On/Off (attr.cd.mix) SPU_ON = mixing on; SPU_OFF = mixing off. CD input is not output unless mixing is on.
	* e) External Digital Input Volume (attr.ext.volume) Set independently for left and right in the range -0x8000 - 0x7fff. If volume is negative, the phase is inverted.
	* f) External Digital Input Reverb On/Off (attr.ext.reverb) SPU_ON = reverb on; SPU_OFF = reverb off.
	* g) External Digital Input Mixing On/Off (attr.cd.mix) SPU_ON = mixing on; SPU_OFF = mixing off. External digital input is not output unless mixing is on.
	*/

	/// <summary>
	/// Transmit font pattern.
	/// <para>Transmits the built-in text font used for debugging text output to the frame buffer. 
	/// It loads the basic font pattern(4 - bit, 256x128) and initializes all the print streams.</para>
	/// <para>FntLoad() must always be executed before FntOpen() and FntFlush(). 
	/// The font area must not clash with the frame buffer area used by the application.
	/// Font data is located at the upper left of the texture page for FntFlush().
	/// Font data is treated as a RECT(0, 0, 32, 32) area consisting of 128 characters, each 128 x 32. 
	/// As this is similar to the texture page area, tx is restricted to a multiple of 64 and ty is restricted to a multiple of 256. </para>
	/// <para>Loads the Clut to location (tx, ty+128).</para>
	/// </summary>
	/// <param name="tx">Font pattern frame buffer address</param>
	/// <param name="ly">Font pattern frame buffer address</param>
	void FntLoad(long tx, long ly) {}

	/// <summary>
	/// Open a print stream.
	/// <para>Opens the stream for on-screen printing. 
	/// After this, character strings up to n characters long can be drawn in the(x, y) - (x + w, y + h) rectangular area of the frame buffer, 
	/// using FntPrint().If isbg is 1, the background is cleared when a character string is drawn.</para>
	/// <para>Up to 8 streams can be opened at once. However, once a stream is opened, it cannot be closed until the next time FntLoad() is called.</para>
	/// <para>n specifies the maximum number of characters. Up to 1024 characters can be specified together in 8 streams</para>
	/// </summary>
	/// <param name="x">Display start location</param>
	/// <param name="y">Display start location</param>
	/// <param name="w">Display area</param>
	/// <param name="h">Display area</param>
	/// <param name="isbg">
	/// Automatic clearing of background
	/// <para>- 0: Clear background to (0, 0, 0) when display is performed</para>
	/// <para>- 1: Do not clear background to (0, 0, 0) when display is performed</para>
	/// </param>
	/// <param name="n">Maximum number of characters</param>
	/// <returns>The stream ID.</returns>
	long FntOpen(long x, long y, long w, long h, long isbg, long n) { return 0; }

	/// <summary>
	/// Define stream for onscreen dump
	/// <para>Sets the print stream for debug printing. 
	/// The output of the debug printing functions can then be carried out in relation to the stream specified in id.</para>
	/// <para>The actual display is executed by FntFlush().</para>
	/// </summary>
	/// <param name="id">Print stream ID</param>
	void SetDumpFnt(long id) {}

	/// <summary>
	/// Calculate value of member tpage in a primitive.
	/// <para>Calculates the texture page ID, and returns it.</para>
	/// <para>The semitransparent rate is also effective for polygons on which texture mapping is not performed.</para>
	/// <para>The texture page address is limited to a multiple of 64 in the X direction and a multiple of 256 in the Y direction.</para>
	/// </summary>
	/// <param name="tp">
	/// Texture mode
	/// <para>- 0: 4bitCLUT</para>
	/// <para>- 1 : 8bitCLUT</para>
	/// <para>- 2 : 16bitDirect</para>
	/// </param>
	/// <param name="abr">
	/// Semitransparency rate
	/// <para>- 0: 0.5 x Back + 0.5 x Forward</para>
	/// <para>- 1: 1.0 x Back + 1.0 x Forward</para>
	/// <para>- 2: 1.0 x Back - 1.0 x Forward</para>
	/// <para>- 3: 1.0 x Back + 0.25 x Forward</para>
	/// </param>
	/// <param name="x">Texture page address</param>
	/// <param name="y">Texture page address</param>
	/// <returns>Texture page ID</returns>
	unsigned short GetTPage(int tp, int abr, int x, int y) { return 0; }

	/// <summary>
	/// Calculate value of the CLUT member in a primitive.
	/// <para>Calculates and returns the texture CLUT ID.</para>
	/// <para>The CLUT address is limited to multiples of 16 in the x direction.</para>
	/// </summary>
	/// <param name="x">Frame buffer address of CLUT</param>
	/// <param name="y">Frame buffer address of CLUT</param>
	/// <returns>CLUT ID.</returns>
	unsigned short GetClut(int x, int y) { return 0; }

	/// <summary>
	/// Translate an absolute sector number to a minute/seconds/sector time code.
	/// <para>Calculate value for minute/second/sector from absolute sector number.</para>
	/// </summary>
	/// <param name="i">Absolute sector number</param>
	/// <param name="p">Pointer to a CdlLOC structure that will be set to the position time code</param>
	/// <returns>p</returns>
	CdlLOC* CdIntToPos(int i, CdlLOC* p) { return 0; }

	/// <summary>
	/// Read multiple sectors from the CD-ROM.
	/// <para>Reads one or more sectors of data from the CD-ROM to the specified buffer in memory. 
	/// The starting position for the read is the position last specified for CdlSetloc.
	/// Each CdRead() requires a separate CdlSetloc command.</para>
	/// <para>CdRead() is non-blocking. Check for completion using CdReadSync() or CdReadCallback(). 
	/// CdRead() uses CdReadyCallback() internally, so that function cannot be used with CdRead().</para>
	/// <para>The return code from CdRead() only indicates if the command was issued successfully or not. 
	/// For information about CD - ROM errors which occur during reading, check the result array of CdReadSync().</para>
	/// </summary>
	/// <param name="sectors">Read sector count</param>
	/// <param name="buf">Pointer to read buffer</param>
	/// <param name="mode">CD-ROM subsystem mode, as defined for CdlSetMode command of CdControl() </param>
	/// <returns>1 if command issued successfully, otherwise 0.</returns>
	int CdRead(int sectors, ULONG* buf, int mode) { return 0; }

	/// <summary>
	/// Check completion of CdRead() and related functions.
	/// <para>Checks the current status of a data read operation initiated by CdRead(), CdReadFile(), 
	/// and related functions.If mode is 0, the function waits for the operation to complete.If mode is 1, 
	/// it returns the current status immediately.</para>
	/// </summary>
	/// <param name="mode">Await read completion</param>
	/// <param name="result">Pointer to status storage buffer of command most recently completed</param>
	/// <returns>Number of sectors remaining. If operation completed, 0 is returned. On error, -1 is returned.</returns>
	int CdReadSync(int mode, unsigned char* result) { return 0; }

	/// <summary>
	/// Set the drawing environment.
	/// <para>The basic drawing parameters s(uch as the drawing offset and the drawing clip area) 
	/// are set according to the values specified in env</para>
	/// <para>The drawing environment is effective until the next time PutDrawEnv() is executed, 
	/// or until the DR_ENV primitive is executed.</para>
	/// </summary>
	/// <param name="env">Pointer to drawing environment start address</param>
	/// <returns>A pointer to the drawing environment set. On failure, returns 0.</returns>
	DRAWENV* SetDefDrawEnv(DRAWENV* env) { return 0; }

	/// <summary>
	/// Set standard drawing environment structure.
	/// <para>Sets the drawing area members of a DRAWENV (drawing environment) structure. 
	/// The new drawing area is specified using the coordinates within the frame buffer of the top left corner, 
	/// along with the width and height, of the desired rectangle.</para>
	/// <para>See table below</para>
	/// <para>This function does not actually change the drawing environment. 
	/// It merely sets the members of the specified structure as desired.Use PutDrawEnv() with this structure to change the actual environment.</para>
	/// </summary>
	/// <param name="SetDefDrawEnv">Pointer to drawing environment</param>
	/// <param name="x">Upper left corner of drawing area</param>
	/// <param name="y">Upper left corner of drawing area</param>
	/// <param name="w">Width and height of drawing area></param>
	/// <param name="h">Width and height of drawing area</param>
	/// <returns>The starting pointer of the drawing environment set.</returns>
	DRAWENV* SetDefDrawEnv(DRAWENV* SetDefDrawEnv, int x, int y, int w, int h) { return 0; }

	/*
	 * +-------------+-----------------------------------+-------------------------------------------+
	 * | Member      | Content                           | Value                                     |
	 * +-------------+-----------------------------------+-------------------------------------------+
	 * | clip        | Drawing area                      | (x, y, w, h)                              |
	 * | ofs[2]      | Drawing offset                    | (x, y)                                    |
	 * | tw          | Texture window                    | (0, 0, 0, 0)                              |
	 * | tpage       | Texture page (tp, abr, tx, ty)    | (0, 0, 640, 0)                            |
	 * | dtd         | Dither processing flag            | 1 (ON)                                    |
	 * | dfe         | Permission flag for drawing       | 1 (drawing on display area in inhibited)  |
	 * | isbg        | Draw area clear flag              | 0 (clear: OFF)                            |
	 * | r0, g0, b0  | Background color                  | (0, 0, 0)                                 |
	 * +-------------+-----------------------------------+-------------------------------------------+
	 */


	/// <summary>
	/// Set display environment structure members and screen display area.
	/// <para>Sets the members of a DISPENV (display environment) structure. 
	/// The new display area is specified using the coordinates within the frame buffer of the top left corner, 
	/// along with the width and height, of the desired rectangle.</para>
	/// <para>See table below</para>
	/// <para>This function does not actually change the display environment.  
	/// It merely sets the members of the specified structure as desired. 
	/// Use PutDispEnv() with this structure to change the actual environment.</para>
	/// <para>Note: While the screen width and height are set to (0, 0), internally they are processed as (256, 240).</para>
	/// </summary>
	/// <param name="disp">Pointer to display environment</param>
	/// <param name="x">Upper left corner of display area</param>
	/// <param name="y">Upper left corner of display area</param>
	/// <param name="w">Width and height of the display area</param>
	/// <param name="h">Width and height of the display area</param>
	/// <returns>Pointer to the display environment set</returns>
	DISPENV* SetDefDispEnv(DISPENV* disp, int x, int y, int w, int h) { return 0; };

	/*
	 * +---------+-----------------------+---------------+
	 * | Member  | Content               | Value         |
	 * +---------+-----------------------+---------------+
	 * | disp    | Display area          | (x, y, w, h)  |
	 * | screen  | Screen display area   | (0, 0)-(0, 0) |
	 * | isinter | Interlace flag        | 0             |
	 * | isrgb24 | 24-bit mode flag      | 0             |
	 * +---------+-----------------------+---------------+
	 */


	/// <summary>
	/// Set a root counter
	/// <para>Sets the root counter in spec to the target value in target, and the mode in mode.</para>
	/// </summary>
	/// <param name="spec">Root counter specification</param>
	/// <param name="target">Target value</param>
	/// <param name="mode">Mode</param>
	/// <returns>1 if it succeeds, and 0 otherwise. (0 is always returned if you specify RCntCNT3, since it can’t be set.)</returns>
	LONG SetRCnt(LONG spec, unsigned short target, LONG mode) { return 0; }

	/// <summary>
	/// Get value of a root counter.
	/// <para>Returns the current value of root counter spec. To be used when root counter spec has been set by 
	/// SetRCnt to a polling mode(RCntMdNOINTR).</para>
	/// </summary>
	/// <param name="spec">Root counter</param>
	/// <returns>The 32-bit unsigned expanded counter value. On failure, it returns -1.</returns>
	LONG GetRCnt(LONG spec) { return 0; }

	/// <summary>
	/// Define a callback function to be called when the GPU is finished executing a primitive list.
	/// <para>Defines a routine to be used as a callback when drawing is completed. When all requests in the queue 
	/// have terminated, the function func is called.If func is set to 0, then any previous callback routine is 
	/// disabled.</para>
	/// <para>Inside the callback, subsequent drawing termination interrupts are masked. Therefore, the callback routine 
	/// should return as soon as possible.Although the specified function is called during an interrupt, it is not an 
	/// interrupt handler; it should be written as a normal subroutine that is called by the main interrupt handler.</para>
	/// </summary>
	/// <param name="func">Pointer to callback function</param>
	void DrawSyncCallback(void* func) {}

	/// <summary>
	/// Wait for all drawing to terminate.
	/// <para>Waits for drawing to terminate.</para>
	/// <para>If DrawSync(0) is used, and execution of the primitive list takes an exceptionally long time (approximately longer than 8 Vsync) to complete, 
	/// a timeout is generated and the GPU is reset.Reasons why this might occur include an exceptionally long primitive list, 
	/// or one that renders exceptionally large numbers of pixels. 
	/// Another possibility is that the primitive list has been corrupted in some way.To avoid this, the application can use a loop such as:</para>
	/// <para>while (DrawSync(1)); </para>
	/// </summary>
	/// <param name="mode">
	/// <para>- 0 Wait for termination of all non-blocking functions registered in the queue</para>
	/// <para>- 1 Return the number of positions in the current queue</para>
	/// </param>
	/// <returns>Number of positions in the execution queue</returns>
	LONG DrawSync(LONG mode) { return 0; }

	/// <summary>
	/// Initialize an array to a linked list for use as an ordering table.
	/// <para>Walks the array specified by ot and sets each element to be a pointer to the following element, 
	/// except the last, which is set to a pointer to a special terminator value which the PlayStation uses to recognize the end 
	/// of a primitive list.n specifies the number entries in the array.</para>
	/// <para>To execute the OT initialized by ClearOTag(), call DrawOTag(ot).</para>
	/// </summary>
	/// <param name="ot">OT starting pointer</param>
	/// <param name="n">Number of entries in OT</param>
	ULONG* ClearOTag(ULONG *ot, int n) { return 0; }

	/// <summary>
	/// Initialize a sprite primitive.
	/// <para>These functions initialize the primitives specified by p. Details are given below.</para>
	/// <para>The SPRT... primitives are faster than POLY_FT4. TILE is also faster than POLY_F4.</para>
	/// </summary>
	/// <param name="p">Initialize a SPRT primitive.</param>
	void SetSprt(SPRT* p) {}

	/// <summary>
	/// Inhibit shading function.
	/// <para>Sets the shading attribute of the primitive pointed to by p to the value specified by tge.</para>
	/// <para>When texture and shading are both ON, each pixel in the polygon is calculated as shown below from the pixel value T of the corresponding texture pattern, 
	/// and the brightness value L corresponding to the pixel value T.</para>
	/// <para><para>P = (T&amp;0xf8*L&amp;0xf8) / 128</para>
	/// <para>if (p > 255) p = 255</para>
	/// <para>if (p &lt; 0) p = 0 </para></para>
	/// <para>When L = 128, the brightness value of the texture pattern is drawn as it is. If the value results in an overflow, the pixel value is clipped to 255 </para>
	/// <para>When tge = 1, the brightness value is not divided, and the texture pattern value is used, as it is, as the pixel value.</para>
	/// <para>T, L are only effective for the upper 5 bits. The lower 3 bits are discarded.</para>
	/// <para>This function cannot be used for primitives other than POLY_FT3, POLY_FT4, SPRT, SPRT_8, and SPRT_16.</para>
	/// <para>Although the texture number of colors is saved at intensity level of 32 when using ShadeTex, the shading brightness value is decreased from an intensity level of 256 to 32. </para>
	/// </summary>
	/// <param name="p">Pointer to primitive</param>
	/// <param name="tge">Unshaded flag
	/// <para>- 0: Shading is performed</para>
	/// <para>- 1: Shading is not performed</para></param>
	void SetShadeTex(void* p, int tge) {}

	/// <summary>
	/// Set the semitransparent attribute of a primitive.
	/// <para>Sets the semitransparent attribute of the primitive specified by p to the value specified by abe. 
	/// If semitransparent mode is enabled, then semitransparent pixels are drawn as specified by the table below.</para>
	/// <para>Semitransparent pixels are calculated from the foreground pixels Pf and background pixels Pb as follows:</para>
	/// <para>P = F x Pf + B x Pb</para>
	/// <para>The rate (F, B) of semitransparency is designated by the member tpage in the primitive. 
	/// Drawing speed is reduced because semitransparency requires reading of background brightness values.Therefore, 
	/// do not draw primitives with semitransparent mode turned on unless they are to be displayed that way.</para>
	/// </summary>
	/// <param name="p">Pointer to primitive</param>
	/// <param name="abe">Semitransparent flag
	/// <para>- 0: semitransparent OFF</para>
	/// <para>- 1: semitransparent ON</para></param>
	void SetSemiTrans(void* p, int abe) {}

	/// <summary>
	/// Initialize texture page change primitive.
	/// <para>Initializes a texture page change primitive. By registering the initialized primitive in OT using AddPrim(), the texture page can be changed while drawing.</para>
	/// </summary>
	/// <param name="p">Texture page setting primitive</param>
	/// <param name="dfe">
	/// Flag for drawing to a display area
	/// <para>- 0: no drawing in display area</para>
	/// <para>- 1: drawing in display area</para>
	/// </param>
	/// <param name="dtd">
	/// Dither processing flag
	/// <para>- 0: dither processing not performed</para>
	/// <para>- 1: dither processing performed</para>
	/// </param>
	/// <param name="tpage">Texture page</param>
	void SetDrawTPage(DR_TPAGE* p, int dfe, int dtd, int tpage) {}

	/// <summary>
	/// Initialize a polygon primitive
	/// <para>These functions initialize the primitive specified by p.</para>
	/// </summary>
	/// <param name="p">Flat textured quadrangle primitive.</param>
	void SetPolyFT4(POLY_FT4* p) {}

	/// <summary>
	/// Transfer data to a frame buffer.
	/// <para>Transfers the contents of memory from the address p to the rectangular area in the frame buffer specified by recp</para>
	/// <para>Because LoadImage() is a non-blocking function, transmission termination must be detected by DrawSync().</para>
	/// <para>The source and destination areas are not affected by the drawing environment (clip, offset). 
	/// The destination area must be located within a drawable area(0, 0) - (1023, 511). 
	/// See the description of the DR_LOAD primitive.</para>
	/// </summary>
	/// <param name="recp">Pointer to destination rectangular area</param>
	/// <param name="p">Pointer to main memory address of source of transmission</param>
	/// <returns>Position of this command in the libgpu command queue.</returns>
	int LoadImage(RECT* recp, ULONG* p) { return 0; }

	/// <summary>
	/// Clear Frame Buffer at high speed.
	/// <para>Sets the rectangular area rect in the Frame Buffer to RGB color values (r, g, b).</para>
	/// <para>Because this is a non-blocking function, the end of the operation must be detected using DrawSync(). 
	/// The drawing area is not affected by the drawing environment(clip / offset).</para>
	/// <para>When in interlace mode, use ClearImage2() instead.</para>
	/// </summary>
	/// <param name="rect">Pointer to rectangular area to be cleared</param>
	/// <param name="r">Pixel values to be used for clearing</param>
	/// <param name="g">Pixel values to be used for clearing</param>
	/// <param name="b">Pixel values to be used for clearing</param>
	/// <returns>Position of this command in the libgpu command queue.</returns>
	int ClearImage(RECT* rect, unsigned char r, unsigned char g, unsigned b) { return 0; }

	/// <summary>
	/// Initialize a tile primitive.
	/// <para>These functions initialize the primitives specified by p. Details are given below.</para>
	/// <para>The SPRT primitives are faster than POLY_FT4. TILE is also faster than POLY_F4.</para>
	/// </summary>
	/// <param name="p">Initialize a TILE primitive</param>
	void SetTile(TILE* p) {}

	/// <summary>
	/// Initialize content of a drawing mode primitive.
	/// <para>Initializes a DR_MODE primitive. By using AddPrim() to insert a DR_MODE primitive into your primitive list, 
	/// it is possible to change part of your drawing environment in the middle of drawing.</para>
	/// <para>If tw is 0, the texture window is not changed.</para>
	/// </summary>
	/// <param name="p">Pointer to drawing mode primitive</param>
	/// <param name="dfe">0: drawing not allowed in display area, 1: drawing allowed in display area</param>
	/// <param name="dtd">0: dithering off, 1: dithering on.</param>
	/// <param name="tpage">Texture page</param>
	/// <param name="tw">Pointer to texture window</param>
	void SetDrawMode(DR_MODE* p, int dfe, int dtd, int tpage, RECT* tw) {}

}

