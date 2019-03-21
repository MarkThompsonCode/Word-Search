#pragma once

#include <vector>
#include <algorithm>

using std::vector;
using std::pair;
using std::make_pair;
using std::random_shuffle;

struct DIRECTION 
{
	int vertical;
	int horizontal;
}; //* PDIRECTION;

class CWord
{
public:
	CWord(const CString, const vector< DIRECTION >, const SIZE, vector< vector< CString > > * const = nullptr, const CString = _T( "." ) );
	~CWord();

	const CString	cstring() const	{ return m_word; }
	bool			can_place();
	bool			attempt_all_placements();
	bool			grid_placement_ok();
	void			place();	
	void			remove();
	void			set_found( const bool found )	{ m_found = found; } // function l and r values
	const bool		is_found() const				{ return m_found; }
	const bool		is_placed() const				{ return m_placed; }

	const vector<POINT>::const_iterator first_char_postition() const { return m_character_positions.begin(); }
	const vector<POINT>::const_iterator last_char_postition() const  { return m_character_positions.end(); }

	//bool		positions_clash(CString, vector<POINT>);
	
private:
	bool		within_bounds();
	void		shuffle();
	void		reset();

	CString								m_word;
	bool								m_found;
	bool								m_placed;

	vector<DIRECTION>					m_directions;
	vector<DIRECTION>::const_iterator	m_current_direction;

	SIZE								m_grid_size;
	vector<vector<CString>> *			m_p_grid;
	
	vector<POINT>						m_start_locations;
	vector<POINT>::const_iterator		m_current_start_postion;

	vector<POINT>						m_character_positions;
	bool								m_overlap;

	CString								m_null_character;
};	


	
