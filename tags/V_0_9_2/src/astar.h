/* 

$Id

Copyright (C) 2009 Alexios Chouchoulas

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

*/


#ifndef __ASTAR_ASTAR_H
#define __ASTAR_ASTAR_H


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "astar_heap.h"


// The maximum number of directions
#define NUM_DIRS 8


// Directions are returned as an array of these values using the various DIR_x
// values above.
typedef uint8_t direction_t;


/*
 * The A* data structure itself.
 *
 * 0------------------------------------------------------------+
 * | GAME MAP                                                   |
 * |                                                            |
 * | (origin_x, origin_y)                                       |
 * |       \<- - - - - -  w - - - - - ->                        |
 * |        0--------------------------+ ^                      |
 * |        | A* GRID                  | |                      |
 * |        |                          |                        |
 * |        |    o....                 | |                      |
 * |        |         ...              |                        |
 * |        |            .             | h                      |
 * |        |             .            |                        |
 * |        |              ......o     | |                      |
 * |        |                          | v                      |
 * |        +--------------------------+                        |
 * |                                                            |
 * +------------------------------------------------------------+
 *
 * The A* search works on a subplane of the entire map, sized w x h. The origin
 * of this subplane is stored within the structure for easy reference.
 * 
 * All other co-ordinates in the structure are translated so that they are in
 * the co-ordinate system of the A* grid, not the game map. This means that,
 * e.g., column range [0, w) |--> [origin_x, origin_x + w) and row range
 * [0, h) |--> [origin_y, origin_y + h).
 *
 * Note that this is an oversimplification. Since game maps can be
 * topologically torroidal (if wrap-around is used), the actual mapping may
 * vary (e.g. may be modulo the size of the map). This algorithm won't handle
 * this case internally. The caller should have massaged numbers
 * appropriately.
 *
 * If your map is sufficiently small, you can find paths on the entire map by
 * setting the origin to (0,0) and making the width and height match those of
 * your entire map.
 *
 */

typedef struct {

	///////////////////////////////////////////////////////////////////////////////
	//
	// Parameters.
	//
	///////////////////////////////////////////////////////////////////////////////
	
	uint32_t    origin_x;   // X ordinate of the top-left corner.
	uint32_t    origin_y;   // Y ordinate of the top-left corner.

	uint32_t    x0, y0;     // Starting location.
	uint32_t    x1, y1;     // Destination location.

	uint32_t    w;          // Width (pitch) of the grid.
	uint32_t    h;		// Height of the grid.


	///////////////////////////////////////////////////////////////////////////////
	//
	// Configuration and costs.
	//
	///////////////////////////////////////////////////////////////////////////////

	// Stop calculating when the route incurs this much cost.

	uint32_t    max_cost;


	// Maximum search time in microseconds (1000000us=1s).

	uint32_t    timeout;

	// Arrays of 8 elements holding delta-x and delta-y pairs for the eight
	// directions.

	int32_t dx[NUM_DIRS];
	int32_t dy[NUM_DIRS];

	// An array of 8 elements holding the costs of moving in each of the
	// directions.

	int32_t mc[NUM_DIRS];

	// The steering penalty is added to the cost of any move that includes a change in
	// direction.
	
	int32_t steering_penalty;

	// The cost of a 'move' as returned by the heuristic. If slightly smaller than the
	// cost of moving along a cardinal dimension (e.g. mc[0]), the calculated route is
	// smoother.

	int32_t heuristic_factor;

	///////////////////////////////////////////////////////////////////////////////
	//
	// User functions
	//
	///////////////////////////////////////////////////////////////////////////////

	// The heuristic function. Given the source (x0,y0) and destination (x1,y1),
	// calculate the  heuristic distance. Leave it as-is  (or set it to  NULL) and the
	// built-in Manhattan distance will be used. The result of the heuristic will be
	// multiplied by the heuristic_factor above.
	
	uint32_t  (* heuristic) (const uint32_t x0, const uint32_t y0,
				 const uint32_t x1, const uint32_t y1);

	// The map initialisation function. Given co-ordinates (x,y) on the game map
	// (i.e. adjusted for the map origin (origin_x,origin_y) such that the X range is
	// [origin_x, origin_x + w) and Y range is [origin_y, origin_y + h), this function
	// must return a cost in the range [0,255] to denote how easy it is to reach this
	// map square. A cost of 255 is impassable. Any lower cost is treated as a
	// relative difficulty. The cost is added to the cost of moving along the chosen
	// direction, as described in array mc in this structure. The search grid may be
	// initialised either by calling astar_grid_init(), passing a get() function like
	// this, or by setting get() and allowing the algorithm to only request the map
	// squares it needs. The latter is preferable for large maps.

	uint8_t (*get) (const uint32_t x, const uint32_t y);

	///////////////////////////////////////////////////////////////////////////////
	//
	// Data needed to run the algorithm
	//
	///////////////////////////////////////////////////////////////////////////////

	uint32_t    ofs0;       // Starting location (as grid offset)
	uint32_t    ofs1;       // Destination (as grid offset)
	uint32_t    bestscore;  // Best H so far.
	asheap_t *  heap;	// The binary heap holds F |-> square_t mappings.
	square_t *  grid;	// The grid holds the actual square_t structs.

	// Bitfield holding search state.

	uint32_t  origin_set:1; // The origin has been set.
	uint32_t  must_reset:1; // A search has ran, must reset.
	uint32_t  grid_init:1;  // The grid has been initialised.
	uint32_t  grid_clean:1; // The grid is ready for use.
	uint32_t  have_route:1; // A (partial) route has been found.
	uint32_t  have_best:1;  // There's a compromise route.
        uint32_t  move_8way:1;   // Move along all 8 directions.
	
	struct timeval t0;      // Algorithm start time.

	///////////////////////////////////////////////////////////////////////////////
	//
	// Results
	//
	///////////////////////////////////////////////////////////////////////////////

	uint32_t    steps;	// Number of moves in the route.
	uint32_t    score;	// Score of the route.
	uint32_t    result;	// Result code of the routing.
	char *      str_result; // Stringified result code.
	uint32_t    usecs;      // Search time in microseconds.
	uint32_t    loops;      // Number of search loops.
	uint32_t    gets;       // Number of times get() was called.
	uint32_t    updates;    // Keeps track of heap updates (they're expensive).
	uint32_t    open;       // Number of open positions.
	uint32_t    closed;     // Number of closed positions.

	uint32_t    bestofs;    // If a route wasn't found, the best offset we could reach.
	uint32_t    bestx;      // Likewise, the X ordinate of the best ending point.
	uint32_t    besty;      // Likewise, the X ordinate of the best ending point.
} astar_t;


// This denotes the cost of impassable blocks.
#define COST_BLOCKED 255


// A* Result Codes (as returned by astar_run and stored in astar_t.result).
#define ASTAR_FOUND                 0
#define ASTAR_NOTHING               1 // Nothing yet.
#define ASTAR_NOTFOUND              2 // Didn't reach destination (or reached the score limit)
#define ASTAR_TRIVIAL               3 // Nothing to do, already at destination.
#define ASTAR_TIMEOUT               4 // Reached the time limit.
#define ASTAR_GRID_NOT_INITIALISED  5 // The grid hasn't been initialised (and get() is unset)
#define ASTAR_GRID_NOT_INITIALIZED  ASTAR_GRID_NOT_INITIALISED // Merkin alias.
#define ASTAR_ORIGIN_NOT_SET        6 // astar_t.get() called, but the origin wasn't set.
#define ASTAR_EMBEDDED              7 // The origin is embedded in a blocked square, can't move.
#define ASTAR_AMONTILLADO           ASTAR_EMBEDDED // E. A. Poe alias.


// We use three bits to specify the direction of a square's 'parent'.
#define DIR_N  0
#define DIR_E  2
#define DIR_S  4
#define DIR_W  6
#define DIR_NE 1
#define DIR_SE 3
#define DIR_SW 5
#define DIR_NW 7

// Movement modes.
#define DIR_CARDINAL  0
#define DIR_8WAY      1


// This is only used in directions_t to signify the end of the directions (for
// added safety).
#define DIR_END 255

/** 
 * Initialise A*.
 * 
 * @param w The width of the search space in grid squares.
 *
 * @param h The height of the search space in grid squares.
 *
 * @param get A map cost getter. This must accept a set of <tt>uint32_t</tt>
 *        co-ordinates in X,Y order and must return a uint8_t number to signify
 *        the cost of stepping onto the specified square. Values for passable
 *        (non-blocked) squares range from 0 to 254. The special value
 *        <tt>COST_BLOCKED</tt> means the grid square is impassable (a wall).
 *
 * @param heuristic A heuristic function with the following signature:
 *        <code>uint32_t heuristic (uint32_t x0, uint32_t y0, uint32_t x1,
 *        uint32_t y1)</code>.
 *        It requires two sets of (X,Y) coordinates and
 *        returns a <tt>uint32_t</tt> number that corresponds to the estimated
 *        distance between the two co-ordinates, or estimated cost to travel
 *        from the first to the second. You may pass <tt>NULL</tt>, in which
 *        case the built-in Manhattan Distance heuristic is used. This
 *        parameter may be NULL, in which case the built-in
 *        manhattan_distance() heuristic is used instead.
 * 
 * @return A pointer to a new astar_t structure, an A* algorithm handle.
 */
astar_t *
astar_new (const uint32_t w, const uint32_t h,
	   uint8_t (*get) (const uint32_t, const uint32_t),
	   uint32_t  (*heuristic) (const uint32_t, const uint32_t,
				   const uint32_t, const uint32_t));


/** 
 * Initialise fully the A* grid.
 *
 * To interface your map with the (possibly smaller) map kept in the
 * <tt>astar_t</tt> structure, you can either let A* obtain only the map
 * squares it needs when it needs them, or you can fully initialise the map
 * beforehand (this also sets the origin and the map getter
 * callback). Depending on the structure of your program and how many times you
 * run astar_run() on the same <tt>astar_t</tt> structure, this may be a faster
 * option than obtaining map squares one by one, on demand. Profiling your code
 * may help decide which is the preferable method.
 *
 * @param as An initialised A* context.
 * @param origin_x The X origin (leftmost row) of the A* map on the game map.
 * @param origin_y The Y origin (topmost row) of the A* map on the game map.
 * @param get A map cost getter. This must accept a set of <tt>uint32_t</tt>
 *        co-ordinates in X,Y order and must return a uint8_t number to signify
 *        the cost of stepping onto the specified square. Values for passable
 *        (non-blocked) squares range from 0 to 254. The special value
 *        <tt>COST_BLOCKED</tt> means the grid square is impassable (a wall).
 */
	   
void astar_init_grid (astar_t * as,
		      uint32_t origin_x, uint32_t origin_y,
		      uint8_t(*get)(const uint32_t, const uint32_t));

/** 
 * Set cardinal or eight-way pathfinding mode.
 *
 * Setting this will affect any pending and subsequent invocations of
 * astar_run().
 * 
 * @param as An initialised A* context.
 *
 * @param movement_mode either <tt>DIR_CARDINAL</tt> (search for paths using
 * only the four cardinal directions) or <tt>DIR_8WAY</tt> (search for paths
 * using all eight directions).
 */

void astar_set_movement_mode (astar_t * as, int movement_mode);

/** 
 * Retrieve the path found my A*
 *
 * This function returns (by reference) an array of direction_t elements. Each
 * direction_t element represents one step from the starting location towards
 * the target location. For full paths, the directions eventually reach this
 * target location. Partial paths reach a compromise point as near the target
 * location as could be found by A*.
 *
 * A direction_t element has one of eight values (if the pathfinding mode is
 * <tt>DIR_CARDINAL</tt>, only the four first ones are used).
 *   - <tt>DIR_N</tt>
 *   - <tt>DIR_E</tt>
 *   - <tt>DIR_S</tt>
 *   - <tt>DIR_W</tt>
 *   - <tt>DIR_NE</tt>
 *   - <tt>DIR_SE</tt>
 *   - <tt>DIR_SW</tt>
 *   - <tt>DIR_NW</tt>
 *
 * The list of directions is terminated with a <tt>DIR_END</tt> value, much
 * like C strings are terminated with a '\0' char value. Additionally, the
 * number of steps in the directions (excluding the <tt>DIR_END</tt> marker) is
 * also returned by this function.
 *
 * Each represents a step in the corresponding direction. These may be
 * translated into (dx,dy) steps (e.g. <tt>DIR_NE</tt> will translate to a
 * (dx,dy) pair of (-1,1) using the default configuration) by calling
 * astar_get_dx() and astar_get_dy() respectively.
 * 
 * The directions may be used to move a unit step by step along the found path,
 * or they may be looped through to perform the move in one. The example code
 * (example.c) uses such a loop through the directions to annotate the found
 * path on a map.
 * 
 * @warning The function astar_free_directions() must be used with
 * <tt>directions</tt> as its argument to deallocate the directions array when
 * you are done processing it. If this is not done, a memory leak will occur.
 *
 * @param as An initialised A* context.
 *
 * @param directions An unallocated variable of type <tt>direction_t *</tt>
 * passed by reference. This will be set to point to a newly allocated array of
 * direction_t elements that signify steps from the starting point of the path
 * to the destination (for full paths) or best compromise (for partial paths).
 * 
 * @return The number of steps returned.
 *
 */

uint32_t astar_get_directions (astar_t *as, direction_t ** directions);

/** 
 * Free memory used by a path.
 *
 * Call this function to deallocate the directions_t array yielded by
 * astar_get_directions(). Failure to do so will cause memory to leak.
 * 
 * @param directions An array of direction_t elements previously allocated by
 * astar_get_directions().
 */

void astar_free_directions (direction_t * directions);

/** 
 * Set the origin of the path finding map.
 *
 * A* may be used to locate a path in an area smaller than the entire game
 * map. To do so, the origin (top-left co-ordinates) of the A* map on the game
 * map must be set using this function. also, the width and height parameters
 * passed to astar_new() must be set to values smaller than the full game
 * map's.
 *
 * This function may be used to move the origin around. This is a useful
 * technique in conserving CPU cycles if a game unit is meant to plan in small
 * chunks. Using it, you may form small path finding plans (which are not
 * guaranteed to be optimal) and move a unit incrementally.
 * 
 * @param as An initialised A* context.
 * @param x The X co-ordinate of the origin.
 * @param y The Y co-ordinate of the origin.
 */

void astar_set_origin (astar_t * as, const uint32_t x, const uint32_t y);

void astar_set_max_cost (astar_t *as, const uint32_t max_cost);

void astar_set_timeout (astar_t *as, const uint32_t timeout);

void astar_set_dxy (astar_t *as, const uint8_t dir, const int dx, const int dy);

void astar_set_cost (astar_t *as, const uint8_t dir, const uint32_t cost);

void astar_set_steering_penalty (astar_t *as, const uint32_t steering_penalty);

void astar_set_heuristic_factor (astar_t *as, const uint32_t heuristic_factor);

/** 
 * Run the A* algorithm.
 *
 * The algorithm will run until the target location is reached, or until one of
 * the running constraints (cost, time, etc) are reached.
 * 
 * @param as An initialised A* context.
 * @param x0 The X ordinate of the starting location.
 * @param y0 The Y ordinate of the starting location.
 * @param x1 The X ordinate of the target location.
 * @param y1 The Y ordinate of the target location.
 * 
 * @return A <tt>ASTAR_</tt>x result code.
 *
 *   - <tt>ASTAR_FOUND</tt> if returned if a full route was found. The function
 *        astar_get_directions() may be used to retrieve the full path.
 *   - <tt>ASTAR_NOTFOUND</tt> means the target location could not be
 *        reached. A partial path to a location as near the target as possible
 *        may be available. Use astar_have_route() to check if one is
 *        available.
 *   - <tt>ASTAR_TRIVIAL</tt> denotes the starting location is the same as the
 *        target location. No work was done.
 *   - <tt>ASTAR_EMBEDDED</tt> denotes the starting location is on a 'wall'
 *        square (a square for which the cost is <tt>COST_BLOCKED</tt>).
 *        No work was done.
 *   - <tt>ASTAR_TIMEOUT</tt> is returned when a timeout was set and the
 *        allotted time ran out before a (full) route was found. A partial path
 *        to a location as near the target as possible may be available. Use
 *        astar_have_route() to check.
 *   - <tt>ASTAR_GRID_NOT_INITIALISED</tt> (or <tt>ASTAR_GRID_NOT_INITIALIZED</tt>)
 *        is an error condition thrown when a map getter function hasn't been
 *        set and astar_init_grid() hasn't been called in this astar_t context.
 *   - <tt>ASTAR_ORIGIN_NOT_SET</tt> is an error condition thrown when the map
 *        origin hasn't been set yet.
 *
 */
int astar_run (astar_t * as,
	       const uint32_t x0, const uint32_t y0,
	       const uint32_t x1, const uint32_t y1);

// Return the last A* result code.
#define astar_result(as) (as)->result

// Return the last A* result code (string version).
#define astar_str_result(as) (as)->str_result

// Return non-zero if the A* algorithm has a route. This is only a
// full route if ASTAR_FOUND is the result code.
#define astar_have_route(as) (as)->have_route

// Convert directions to dx, dy.
#define astar_get_dx(as,dir) ((as)->dx[dir])
#define astar_get_dy(as,dir) ((as)->dy[dir])


#ifdef ASTAR_DEBUG
void astar_print (astar_t * as);
#endif // ASTAR_DEBUG


// int astar_run (astar_t * as);



#ifdef __cplusplus
};
#endif // __cplusplus


#endif // _ASTAR_ASTAR_H

// End of file.
