#include "stdafx.h"
#include "CWord.h"

CWord::CWord
( 
	const CString word, 
	const vector<DIRECTION> directions,
	const SIZE grid_size, 
	vector<vector<CString>> * const p_grid, 
	const CString null_character 
)
	: m_word(word), m_directions(directions), m_grid_size(grid_size), m_found(false), m_p_grid(p_grid), m_null_character(null_character), m_placed(false)
{
	for( int row = 0; row < grid_size.cy; row++ )
		for( int col = 0; col < grid_size.cx; col++ )
		{
			// y = row, x = col
			POINT p = { col, row };
			m_start_locations.push_back( p );
		}

	shuffle();

	TRACE("word total directions  = %i \n", m_directions.size() );
}

void CWord::shuffle()
{
	random_shuffle( m_start_locations.begin(), m_start_locations.end() );
	random_shuffle( m_directions.begin(), m_directions.end() );	
}

bool CWord::can_place()
{
	//m_character_positions.clear();

	m_current_start_postion	= m_start_locations.begin();
	m_current_direction		= m_directions.begin();	

	while(m_current_start_postion != m_start_locations.end() )
	{
		while(m_current_direction != m_directions.end() )
		{
			//static int attempt = 0;
			//TRACE("\nattempt = %d", attempt++);

			if( grid_placement_ok() )
			{
				//attempt = 0;
				return true;			
			}

			m_current_direction++;
		}
		//TRACE("\nnext position");
		m_current_start_postion++; // ++m
		m_current_direction = m_directions.begin();
	}

	TRACE("\nunnable to place word - %S", m_word);

	//if( attempt_all_placements() )
		//return true;

	return false;
}

bool CWord::attempt_all_placements()
{
	return false;
	/*
	if(m_current_start_postion == m_start_locations.end() )
	{
		TRACE("\ncannot place - %S", m_word);

		return false;
	}

	if(m_current_direction == m_directions.end() )
	{
		TRACE("\nall directions attempted");

		++m_current_start_postion;
		m_current_direction = m_directions.begin();
	}

	static int attempt = 0;
	TRACE("\nattempt = %d", attempt);

	if( grid_placement_ok() )
		return true;

	// 5*10*8=400
	++attempt;
	
	TRACE("\nnext direction");

	++m_current_direction;
	attempt_all_placements();	
	*/
}

// valid solution = grid_spaces <= total_characters // ( += word_list.lengths() )
	// else too many characters
bool CWord::within_bounds()
{
	int				word_length		= m_word.GetLength() - 1;
	int				start_row		= (*m_current_start_postion).y;
	int				start_col		= (*m_current_start_postion).x;
	DIRECTION		test_direction	= (*m_current_direction);	

	//TRACE("\nstart_pos: y=%d x=%d\t vert=%d horz=%d\t", start_row, start_col, test_direction.vertical, test_direction.horizontal);
	//TRACE("word length = %d\n", m_word.GetLength());

	// y = row
	// x = col
	int bounds_row = start_row + (word_length * test_direction.vertical);
	int bounds_col = start_col + (word_length * test_direction.horizontal);

	if( bounds_row >= 0 && bounds_row < m_grid_size.cy && bounds_col >= 0 && bounds_col < m_grid_size.cx )
	{	
		//TRACE("word = %S - within bounds: y=%d x=%d\n", m_word, bounds_row, bounds_col);
		return true; 
	}
	else
	{
		//TRACE("word = %S - outside bounds: y=%d x=%d\n", m_word, bounds_row, bounds_col);
		return false;
	}	
}

bool CWord::grid_placement_ok()
{
	if( within_bounds() )
	{		
		//TRACE("\nword - %S - WITHIN bounds", m_word);
		bool overlapped_once = false;

		for( int char_index = 0; char_index < m_word.GetLength(); char_index++ )
		{			
			// first  = y = row
			// second = x = col
			int test_row = (*m_current_start_postion).y + ( char_index * (*m_current_direction).vertical );
			int test_col = (*m_current_start_postion).x + ( char_index * (*m_current_direction).horizontal );

			//TRACE("\ntest grid at y=%d x=%d\t", test_row, test_col);

			CString grid_char = (*m_p_grid).at(test_row).at(test_col);

			//TRACE("\ngrid char = %S \t", grid_char);
			
			if( grid_char != m_null_character) //if grid position not empty                      
			{
				//if(overlapping)

				if( grid_char != m_word.Mid(char_index, 1) ) // is grid char not the same 
					return false;
				else										// character is the same
					if( ! overlapped_once )			// if not already overlapped
					{
						overlapped_once = true;		// one overlap allowed
						TRACE("\noverlap char = %S at  y=%d x=%d", grid_char, test_row, test_col);
					}
				
				// goto to next word character
			}
		}
		return true;
	}

	//TRACE("\ngrid != null == %S", grid_char);
	//TRACE("\nword - %S - NOT within bounds", m_word);

	return false;
}
/*
bool CWord::positions_clash(CString test_word, vector<POINT> test_character_positions)
{
	vector<POINT>::const_iterator test_char_pos_it = test_character_positions.begin();
	vector<POINT>::const_iterator this_char_pos_it = m_character_positions.begin();

	//this = CAT, word = BIRD;
	//this = FERRET, word = DOG;

	// if this word is shorter than test word 
	if( m_word.GetLength() <= test_word.GetLength() )
	{
		// then iterator through this words' positions
		while( this_char_pos_it != m_character_positions.end() )
		{
			if( (*this_char_pos_it).x == (*test_char_pos_it).x && (*this_char_pos_it).y == (*test_char_pos_it).y )
				return true;

			++this_char_pos_it;
			++test_char_pos_it;
		}
	}
	else
	// test word is shorter
	{
		while( test_char_pos_it != test_character_positions.end() )
		{
			if( (*this_char_pos_it).x == (*test_char_pos_it).x && (*this_char_pos_it).y == (*test_char_pos_it).y )
				return true;

			++this_char_pos_it;
			++test_char_pos_it;
		}
	}

	return false;
}
*/
void CWord::place()
{	
	for( int char_index = 0; char_index < m_word.GetLength(); char_index++ )
	{
		// first  = y = row
		// second = x = col
		int char_row = (*m_current_start_postion).y + ( char_index * (*m_current_direction).vertical  );
		int char_col = (*m_current_start_postion).x + ( char_index * (*m_current_direction).horizontal );

		//TRACE("\ty=%d x=%d\t", char_row, char_col);

		POINT char_position = { char_col, char_row };
		m_character_positions.push_back( char_position );

		(*m_p_grid).at(char_row).at(char_col) = m_word.Mid(char_index, 1);
	}

	m_placed = true;
	TRACE("\nplaced word - %S\t\t", m_word);
}

void CWord::remove() 
{
	if( ! m_character_positions.empty() )
	{
		vector<POINT>::const_iterator pos_it = m_character_positions.begin();

		for( int char_index = 0; char_index < m_word.GetLength(); char_index++ )
		{
			int char_row = (*pos_it).y;
			int char_col = (*pos_it).x;

			//TRACE("\nremoving grid char = %S \t", (*m_p_grid).at(char_row).at(char_col));

			(*m_p_grid).at(char_row).at(char_col) = m_null_character;		

			++pos_it;
		}
	}

	m_character_positions.clear();

	m_placed = false;
	TRACE("\nremoved word - %S", m_word);
	shuffle();
}

void CWord::reset()
{
	//m_character_positions.clear();
	//
}

CWord::~CWord() 
{
	m_character_positions.clear();
	m_start_locations.clear();	
	m_directions.clear();
}
