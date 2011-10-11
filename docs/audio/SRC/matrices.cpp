#include "matrices.h"
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <mffm/complexFFT.H>

#define PRINT_WIDTH 6


using namespace std;


/* First constructor: Uninitialized matrix. */
Matrix::Matrix()
{
	/* Private variables initialization. */
	Ncolumns = 0;
	Nrows = 0;
	initialized = false;
}

/* Second constructor: +Initialization using double. */
Matrix::Matrix( double *mem, size_t numberOfColumns, size_t numberOfRows )
{
	/* Private variables initialization. */
	Ncolumns = numberOfColumns;
	Nrows = numberOfRows;
	initialized = true;
	/* Matrix data creation. */
	for( size_t i = 0; i < Ncolumns; i++ )
	{
		map< size_t, complex <double> > temp;
		for( size_t j = 0; j < Nrows; j++ )
		{
			complex <double> dat( *(mem + i * Nrows + j ), 0 );
			temp.insert( pair<size_t, complex <double> >( j, dat ) );
		}
		data.insert( pair< size_t, map< size_t, complex <double> > > ( i, temp ) );
	}
}


/* Third constructor: +Initialization using complex. */
Matrix::Matrix( complex <double> *mem, size_t numberOfColumns, size_t numberOfRows )
{
	/* Private variables initialization. */
	Ncolumns = numberOfColumns;
	Nrows = numberOfRows;
	initialized = true;
	/* Matrix data creation. */
	for( size_t i = 0; i < Ncolumns; i++ )
	{
		map< size_t, complex <double> > temp;
		for( size_t j = 0; j < Nrows; j++ )
		{
			complex <double> dat = *(mem + i * Nrows + j );
			temp.insert( pair<size_t, complex <double> >( j, dat ) );
		}
		data.insert( pair< size_t, map< size_t, complex <double> > > ( i, temp ) );
	}
}

/* Destructor. */
Matrix::~Matrix()
{
	this->clear();
}

/* Matrix inverse. */
Matrix Matrix::inverse()
{
	complex<double> det = determinant();
	if( isnan( det.real() ) )
	{
		cerr << "Error: inverting a singular matrix." << endl;
		Matrix ret;
		return ret;
	}

	Matrix ret = cofactors().transpose() / det;
	return ret;
}

/* Matrix transpose. */
Matrix Matrix::transpose()
{
	complex<double> dat[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Nrows; i++ )
		for( size_t j = 0; j < Ncolumns; j++ )
			dat[ i * Ncolumns + j ] = data[ j ][ i ];
	Matrix ret( dat, Nrows, Ncolumns );
	return ret;
}

/* Get corresponding cofactors matrix. */
Matrix Matrix::cofactors()
{
	complex<double> dat[ Nrows * Ncolumns ];
	Matrix temp;
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
		{
			temp = *this;
			temp.delColumn( i );
			temp.delRow( j );
			dat[ i * Nrows + j ] = pow( -1, i + j ) * temp.determinant();
		}
	Matrix ret( dat, Ncolumns, Nrows );
	return ret;
}

/* Matlab: lu(). */
void lu( Matrix a, Matrix *out_l, Matrix *out_u, Matrix *out_p )
{
	complex<double> * temp;
	size_t a_col, a_row, l_col, l_row, u_col, u_row;
	a.size( &a_col, &a_row );
	l_row = a_row;
	l_col = a_row < a_col ? a_row : a_col;
	u_row = a_col < a_row ? a_col : a_row;
	u_col = a_col;
	complex <double> *a_dat = (complex <double> *) malloc( a_col * a_row * sizeof( complex <double> ) );
	complex <double> *l_dat = (complex <double> *) malloc( l_col * l_row * sizeof( complex <double> ) );
	complex <double> *u_dat = (complex <double> *) malloc( u_col * u_row * sizeof( complex <double> ) );
	Matrix p = eye( a_row );

	for( size_t i = 0; i < a_col; i++ )
		for( size_t j = 0; j < a_row; j++ )
			a.getElement( a_dat + i * a_row + j, i, j );

/*
	size_t diag = a_col < a_row ? a_col : a_row;
	for( size_t i = 0; i < diag; i++ )
	{
		if( *( a_dat + i * a_row + i ) == complex <double> ( 0, 0 ) )
		{
			if( a_col == a_row && i == a_col - 1 )
				break;
			if( a_col >= a_row )
			{
				char inf[ a_col - i - 1 ];
				for( size_t k = i + 1; k < a_col; k++ )
				{
					if( *( a_dat + k * a_row + i ) == complex <double> ( 0, 0 ) )
					{
						inf[ k - i - 1 ] = -1;
						continue;
					}
					inf[ k - i - 1 ] = 0;
					for( size_t j = i + 1; j < a_row; j++ )
						if( *( a_dat + k * a_row + j ) == complex <double> ( 0, 0 ) )
							inf[ k - i - 1 ]++
				}
				//Find maximum zeros and exchange.
			}
			else
			{

			}
		}
	}
*/

	for( size_t j = 0; j < a_col; j++ )
	{
		for( size_t i = 0; i <= j; i++ )
		{
			temp = u_dat + j * u_row + i;
			*temp = *( a_dat + j * a_row + i );
			for( size_t k = 0; k < i; k++ )
				*temp -= *( l_dat + k * l_row + i ) * *( u_dat + j * u_row + k );
			*( l_dat + j * l_row + i ) = 0;
		}

		*( l_dat + j * l_row + j ) = 1;

		for( size_t i = j + 1; i < a_row; i++ )
		{
			temp = l_dat + j * l_row + i;
			*temp = *( a_dat + j * a_row + i );
			for( size_t k = 0; k < j; k++ )
				*temp -= *( l_dat + k * l_row + i ) * *( u_dat + j * u_row + k );
			*temp /= *( u_dat + j * u_row + j );
			*( u_dat + j * u_row + i ) = 0;
		}
	}

	if( out_l != NULL )
	{
		Matrix l( l_dat, l_col, l_row );
		*out_l = l;
	}
	if( out_u != NULL )
	{
		Matrix u( u_dat, u_col, u_row );
		*out_u = u;
	}
	if( out_p != NULL )
	{
		*out_p = p;
	}
}

/* Matrix determinant. */
complex<double> Matrix::determinant()
{
	if( Nrows != Ncolumns )
	{
		cerr << "Error cannot calculate the determinant of a non-square matrix." << endl;
		return NAN;
	}
	if( Nrows == 2 )
	{
		return data[0][0] * data[1][1] - data[1][0] * data[0][1];
	}

	Matrix u;
	lu( *this, NULL, &u, NULL );
	complex <double> ret( 1, 0 ), temp;
	for( size_t i = 0; i < Ncolumns; i++ )
	{
		u.getElement( &temp, i, i );
		ret *= temp;
	}
	return ret;
}

/*Deprecated.*/
/* Matrix determinant. */
/*
complex<double> Matrix::determinant() 
{
	if( Nrows != Ncolumns )
	{
		cerr << "Error cannot calculate the determinant of a non-square matrix." << endl;
		return NAN;
	}
	if( Nrows == 2 )
	{
		return data[0][0] * data[1][1] - data[1][0] * data[0][1];
	}

	complex<double> det = 0;
	for( size_t i = 0; i < Ncolumns; i++ )
	{
		Matrix temp;
		temp = *this;
		temp.delRow( 0 );
		temp.delColumn( i );
		det +=  data[i][0] * temp.determinant() * pow( -1 , i ); 
	}
	return det;
}
*/

/* reshape(). */
Matrix Matrix::reshape( size_t row, size_t col )
{
	if( row * col != Ncolumns * Nrows )
	{
		cerr << "Error: Dimensions mismatch for reshape." << endl;
		Matrix ret;
		return ret;
	}

	complex<double> dat[ row * col ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			dat[ i*Nrows + j ] = data[ i ][ j ];

	Matrix ret( dat, col, row );
	return ret;
}

/* size(). */ 
void Matrix::size( size_t *cols, size_t *rows )
{
	*cols = Ncolumns;
	*rows = Nrows;
	return;
}

/* isvector(). */
bool Matrix::isvector()
{
	if( Ncolumns == 1 || Nrows == 1 )
		return true;
	return false;
}

/* isempty(). */
bool Matrix::isempty()
{
	return !initialized;
}

/* isreal(). */
bool isreal( complex <double> val )
{
	if( val.imag() == 0 )
		return true;
	return false;
}

bool Matrix::isreal()
{
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			//if( !isreal( data [ i ][ j ] ) )
			if( data[ i ][ j ].imag() != 0 )
				return false;
	return true;
}

/* clear(). */
void Matrix::clear()
{
	if( !initialized )
		return;
	for( size_t i = 0; i < Ncolumns; i++ )
		data[i].clear();
	data.clear();
	Ncolumns = Nrows = 0;
	initialized = false;
}

/* printout(). */
void Matrix::printout()
{
	if( !initialized )
	{
		cout << "Matrix is uninitialized." << endl;
		return;
	}
	for( size_t i = 0; i < Nrows; i++ )
	{
		cout << "|   "; 
		for( size_t j = 0; j < Ncolumns; j++ )
		{
			complex<double> temp = data[j][i];
			cout << setw(PRINT_WIDTH) << right << temp.real() << " + i*" << left << setw(PRINT_WIDTH) << temp.imag() << "   ";
		}
		cout << "|" << endl;
	}
}

/* getElement(). */
int Matrix::getElement( complex <double> *dat, size_t index )
{
	if( !initialized )
	{
		cerr << "Error getting element from a not initialized matrix." << endl;
		*dat = complex<double>( NAN, NAN );
		return -1;
	}
	if( Ncolumns * Nrows - 1 < index )
	{
		cerr << "Error getting an out of boundary element." << endl;
		*dat = complex<double>( NAN, NAN );
		return -1;
	}
	*dat = data[ index / Nrows ][ index % Nrows ];
	return 0;
}

/* getElement(). */
int Matrix::getElement( complex <double> *dat, size_t col, size_t row )
{
	if( !initialized )
	{
		cerr << "Error getting element from a not initialized matrix." << endl;
		*dat = complex<double>( NAN, NAN );
		return -1;
	}
	if( col >= Ncolumns || row >= Nrows )
	{
		cerr << "Error getting an out of boundary element." << endl;
		*dat = complex<double>( NAN, NAN );
		return -1;
	}
	*dat = data[col][row];
	return 0;
}

/* getElement(). */
int Matrix::getElement( double *dat, size_t index )
{
	complex <double> temp;
	if( getElement( &temp, index ) < 0 )
		return -1;
	*dat = temp.real();
	return 0;
}

/* getElement(). */
int Matrix::getElement( double *dat, size_t col, size_t row )
{
	complex <double> temp;
	if( getElement( &temp, col, row ) < 0 )
		return -1;
	*dat = temp.real();
	return 0;
}

/* setElement(). */
int Matrix::setElement( complex <double> dat, size_t index )
{
	if( !initialized )
	{
		cerr << "Error setting element from a not initialized matrix." << endl;
		dat = complex<double>( NAN, NAN );
		return -1;
	}
	if( Ncolumns * Nrows - 1 < index )
	{
		cerr << "Error setting an out of boundary element." << endl;
		dat = complex<double>( NAN, NAN );
		return -1;
	}
	data[ index / Nrows ][ index % Nrows ] = dat;
	return 0;
}

/* setElement(). */
int Matrix::setElement( double dat, size_t index )
{
	return setElement( complex<double>( dat, 0 ), index );
}

/* setElement(): complex value. */
int Matrix::setElement( complex <double> dat, size_t col, size_t row)
{
	if( !initialized )
	{
		cerr << "Error setting element from a not initialized matrix." << endl;
		dat = complex<double>( NAN, NAN );
		return -1;
	}
	if( col >= Ncolumns || row >= Nrows )
	{
		cerr << "Error setting an out of boundary element." << endl;
		dat = complex<double>( NAN, NAN );
		return -1;
	}
	data[col][row] = dat;
	return 0;
}

/* setElement(): double value. */
int Matrix::setElement( double dat, size_t col, size_t row)
{
	return this->setElement( complex<double>( dat, 0 ), col, row );
}

/* getSubMatrix() */
Matrix Matrix::getSubMatrix( size_t sCol, size_t eCol, size_t sRow, size_t eRow )
{
	if( !initialized )
	{
		cerr << "Error getting subMatrix from a not initialized matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( eCol >= Ncolumns || sCol >= Ncolumns || eRow >= Nrows || sRow >= Nrows )
	{
		cerr << "Error getting an out of boundary subMatrix." << endl;
		Matrix ret;
		return ret;
	}
	complex<double> dat[ ( eCol - sCol + 1 ) * ( eRow - sRow + 1 ) /*+ 1*/];
	for( size_t i = sCol; i <= eCol; i++ )
		for( size_t j = sRow; j <= eRow; j++ )
			getElement( dat + ( i - sCol ) * ( eRow - sRow + 1 ) + j - sRow, i, j );
			 
	Matrix ret( dat, eCol - sCol + 1, eRow - sRow + 1 );
	return ret;
}

int Matrix::addRow( double *row_data, size_t size )
{
	if( initialized && size != Ncolumns )
	{
		cerr << "Row size mismatch." << endl;
		return -1;
	}

	for( size_t i = 0; i < size; i++ )
		data[ i ].insert( pair< size_t, complex<double> >( Nrows, complex<double>( *( row_data + i ), 0 ) ) );

	if( !initialized )
	{
		initialized = true;
		Nrows = 1;
		Ncolumns = size;
	}
	else
		Nrows++;

	return 0;
}

int Matrix::addRow( std::complex<double> *row_data, size_t size )
{
	if( initialized && size != Ncolumns )
	{
		cerr << "Row size mismatch." << endl;
		return -1;
	}

	for( size_t i = 0; i < size; i++ )
		data[ i ].insert( pair< size_t, complex<double> >( Nrows, *( row_data + i ) ) );

	if( !initialized )
	{
		initialized = true;
		Nrows = 1;
		Ncolumns = size;
	}
	else
		Nrows++;

	return 0;
}

int Matrix::addColumn( double *col_data, size_t size )
{
	if( initialized && size != Nrows )
	{
		cerr << "Column size mismatch." << endl;
		return -1;
	}

	map< size_t, complex<double> > temp ;
	for( size_t i = 0; i < size; i++ )
		temp.insert( pair< size_t, complex<double> >( i, complex<double>(*( col_data + i ), 0 ) ) );

	data.insert( pair< size_t, map< size_t, complex<double> > >( Ncolumns, temp ) );

	if( !initialized )
	{
		initialized = true;
		Nrows = size;
		Ncolumns = 1;
	}
	else
		Ncolumns++;

	return 0;
}

int Matrix::addColumn( std::complex<double> *col_data, size_t size )
{
	if( initialized && size != Nrows )
	{
		cerr << "Column size mismatch." << endl;
		return -1;
	}

	map< size_t, complex<double> > temp ;
	for( size_t i = 0; i < size; i++ )
		temp.insert( pair< size_t, complex<double> >( i, *( col_data + i ) ) );

	data.insert( pair< size_t, map< size_t, complex<double> > >( Ncolumns, temp ) );

	if( !initialized )
	{
		initialized = true;
		Nrows = size;
		Ncolumns = 1;
	}
	else
		Ncolumns++;

	return 0;
}

int Matrix::insertRow( double *row_data, size_t size, size_t position )
{
	if( size != Ncolumns )
	{
		cerr << "Row size mismatch." << endl;
		return -1;
	}

	for( size_t i = 0; i < size; i++ )
		data[ i ].insert( pair< size_t, complex<double> >( Nrows, data[ i ][ Nrows - 1 ] ) );

	for( size_t i = Nrows - 1; i > position; i-- )
		for( size_t j = 0; j < size; j++ )
			data[ j ][ i ] = data[ j ][ i - 1];

	for( size_t i = 0; i < size; i++ )
		data[ i ][ position ] = complex<double>( *( row_data + i ), 0 );

	Nrows++;
	return 0;

	return 0;
}

int Matrix::insertRow( std::complex<double> *row_data, size_t size, size_t position )
{
	if( size != Ncolumns )
	{
		cerr << "Row size mismatch." << endl;
		return -1;
	}

	for( size_t i = 0; i < size; i++ )
		data[ i ].insert( pair< size_t, complex<double> >( Nrows, data[ i ][ Nrows - 1 ] ) );

	for( size_t i = Nrows - 1; i > position; i-- )
		for( size_t j = 0; j < size; j++ )
			data[ j ][ i ] = data[ j ][ i - 1];

	for( size_t i = 0; i < size; i++ )
		data[ i ][ position ] = *( row_data + i );

	Nrows++;
	return 0;
}

int Matrix::insertColumn( double *col_data, size_t size, size_t position )
{
	if( size != Nrows )
	{
		cerr << "Column size mismatch." << endl;
		return -1;
	}

	map< size_t, complex<double> > temp ;
	for( size_t i = 0; i < size; i++ )
		temp.insert( pair< size_t, complex<double> >( i, data[ Ncolumns - 1 ][ i ] ) );
	data.insert( pair< size_t, map< size_t, complex<double> > >( Ncolumns, temp ) );

	for( size_t i = Ncolumns - 1; i > position; i-- )
		data[ i ] = data[ i - 1];

	for( size_t i = 0; i < size; i++ )
		data[ position ][ i ] = complex<double>( *(col_data + i ), 0 );

	Ncolumns++;
	return 0;
}

int Matrix::insertColumn( std::complex<double> *col_data, size_t size, size_t position )
{
	if( size != Nrows )
	{
		cerr << "Column size mismatch." << endl;
		return -1;
	}

	map< size_t, complex<double> > temp ;
	for( size_t i = 0; i < size; i++ )
		temp.insert( pair< size_t, complex<double> >( i, data[ Ncolumns - 1 ][ i ] ) );
	data.insert( pair< size_t, map< size_t, complex<double> > >( Ncolumns, temp ) );

	for( size_t i = Ncolumns - 1; i > position; i-- )
		data[ i ] = data[ i - 1];

	for( size_t i = 0; i < size; i++ )
		data[ position ][ i ] = *(col_data + i );

	Ncolumns++;
	return 0;
}

int Matrix::delRow( size_t row )
{
	for( size_t j = 0; j < Ncolumns; j++ )
	{
		for( size_t i = row; i < Nrows; i++ )
			data[ j ][ i ] = data[ j ][ i + 1 ];
		data[ j ].erase( Nrows - 1 );
	}
	Nrows--;
	return 0;
}

int Matrix::delColumn( size_t col )
{
	for( size_t i = col; i < Ncolumns; i++ )
		data[ i ] = data[ i + 1 ];
	data[ Ncolumns - 1 ].clear();
	Ncolumns--;
	return 0;
}

/* .* */
Matrix dotProduct( Matrix MATa, Matrix MATb )
{
	if( MATa.isempty() || MATb.isempty() )
	{
		cerr << "Error multiplying an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	size_t Ncolumns, Ncolumnsb, Nrows, Nrowsb;
	MATa.size( &Ncolumns, &Nrows );
	MATb.size( &Ncolumnsb, &Nrowsb );
	if( Ncolumns == 1 && Nrows == 1 )
	{
		complex<double> temp;
		MATa.getElement( &temp, 0, 0 );
		return MATb * temp;
	}
	if( Ncolumnsb == 1 && Nrowsb == 1 )
	{
		complex<double> temp;
		MATb.getElement( &temp, 0, 0 );
		return MATa * temp;
	}
	if( Ncolumns != Ncolumnsb || Nrows != Nrowsb )
	{
		cerr << "Error multiplying matrices with different dimensions." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
		{
			complex<double> t1, t2;
			MATa.getElement( &t1, i, j );
			MATb.getElement( &t2, i, j );
			complex_data[ i * Nrows + j] = t1 * t2;
		}

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

Matrix dotProduct( Matrix mat, complex<double> val )
{
	if( mat.isempty() )
	{
		cerr << "Error: Multiplying an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	size_t col, row;
	mat.size( &col, &row );
	complex<double> dat[ col * row ], temp;
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			dat[ i * row + j ] = temp * val;
		}
	Matrix ret( dat, col, row );
	return ret;
}

Matrix dotProduct( Matrix mat, double val )
{
	if( mat.isempty() )
	{
		cerr << "Error: Multiplying an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	size_t col, row;
	mat.size( &col, &row );
	complex<double> dat[ col * row ], temp;
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			dat[ i * row + j ] = temp * val;
		}
	Matrix ret( dat, col, row );
	return ret;
}

Matrix dotProduct( complex<double> val, Matrix mat )
{
	if( mat.isempty() )
	{
		cerr << "Error: Multiplying an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	size_t col, row;
	mat.size( &col, &row );
	complex<double> dat[ col * row ], temp;
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			dat[ i * row + j ] = temp * val;
		}
	Matrix ret( dat, col, row );
	return ret;
}

Matrix dotProduct( double val, Matrix mat )
{
	if( mat.isempty() )
	{
		cerr << "Error: Multiplying an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	size_t col, row;
	mat.size( &col, &row );
	complex<double> dat[ col * row ], temp;
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			dat[ i * row + j ] = temp * val;
		}
	Matrix ret( dat, col, row );
	return ret;
}

/* Operator /: Scalar division (double). */
Matrix operator/ ( double factor, Matrix mat )
{
	if( mat.isempty() )
	{
		cerr << "Error dividing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	size_t col, row;
	mat.size( &col, &row );
	complex <double> complex_data[ row * col ], temp;
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			complex_data[ i * row + j] = factor / temp;
		}

	Matrix ret( complex_data, col, row );
	return ret;
}

/* Operator /: Scalar division (complex). */
Matrix operator/ ( complex<double> factor, Matrix mat )
{
	if( mat.isempty() )
	{
		cerr << "Error dividing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	size_t col, row;
	mat.size( &col, &row );
	complex <double> complex_data[ row * col ], temp;
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			complex_data[ i * row + j] = factor / temp;
		}

	Matrix ret( complex_data, col, row );
	return ret;
}


/* ./ */
Matrix dotDivision( Matrix MATa, Matrix MATb )
{
	if( MATa.isempty() || MATb.isempty() )
	{
		cerr << "Error dividing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	size_t Ncolumns, Ncolumnsb, Nrows, Nrowsb;
	MATa.size( &Ncolumns, &Nrows );
	MATb.size( &Ncolumnsb, &Nrowsb );
	if( Ncolumns == 1 && Nrows == 1 )
	{
		complex<double> temp;
		MATa.getElement( &temp, 0, 0 );
		return temp / MATb;
	}
	if( Ncolumnsb == 1 && Nrowsb == 1 )
	{
		complex<double> temp;
		MATb.getElement( &temp, 0, 0 );
		return MATa / temp;
	}
	if( Ncolumns != Ncolumnsb || Nrows != Nrowsb )
	{
		cerr << "Error dividing matrices with different dimensions." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
		{
			complex<double> t1, t2;
			MATa.getElement( &t1, i, j );
			MATb.getElement( &t2, i, j );
			complex_data[ i * Nrows + j] = t1 / t2;
		}

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

Matrix dotDivision( Matrix mat, complex<double> val )
{
	if( mat.isempty() )
	{
		cerr << "Error: Dividing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	size_t col, row;
	mat.size( &col, &row );
	complex<double> dat[ col * row ], temp;
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			dat[ i * row + j ] = temp / val;
		}
	Matrix ret( dat, col, row );
	return ret;
}

Matrix dotDivision( Matrix mat, double val )
{
	if( mat.isempty() )
	{
		cerr << "Error: Dividing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	size_t col, row;
	mat.size( &col, &row );
	complex<double> dat[ col * row ], temp;
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			dat[ i * row + j ] = temp / val;
		}
	Matrix ret( dat, col, row );
	return ret;
}

Matrix dotDivision( complex<double> val, Matrix mat )
{
	if( mat.isempty() )
	{
		cerr << "Error: Dividing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	size_t col, row;
	mat.size( &col, &row );
	complex<double> dat[ col * row ], temp;
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			dat[ i * row + j ] = val / temp;
		}
	Matrix ret( dat, col, row );
	return ret;
}

Matrix dotDivision( double val, Matrix mat )
{
	if( mat.isempty() )
	{
		cerr << "Error: Dividing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	size_t col, row;
	mat.size( &col, &row );
	complex<double> dat[ col * row ], temp;
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			dat[ i * row + j ] = val / temp;
		}
	Matrix ret( dat, col, row );
	return ret;
}


Matrix Matrix::operator| ( Matrix Mat2 )
{
	if( this->isempty() || Mat2.isempty() )
	{
		cerr << "Error oring an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( Ncolumns == 1 && Nrows == 1 )
	{
		complex<double> temp;
		getElement( &temp, 0, 0 );
		if( temp.imag() != 0 )
		{
			cerr << "Error, Oring a complex number." << endl;
			Matrix ret;
			return ret;
		}
		return Mat2 | temp.real();
	}
	if( Mat2.Ncolumns == 1 && Mat2.Nrows == 1 )
	{
		complex<double> temp;
		Mat2.getElement( &temp, 0, 0 );
		if( temp.imag() != 0 )
		{
			cerr << "Error, Oring a complex number." << endl;
			Matrix ret;
			return ret;
		}
		return *this | temp.real();
	}
	if( this->Ncolumns != Mat2.Ncolumns || this->Nrows != Mat2.Nrows )
	{
		cerr << "Error comparing matrices with different dimensions." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> complex_data[ this->Nrows * this->Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
		{
			complex<double> t1, t2;
			this->getElement( &t1, i, j );
			Mat2.getElement( &t2, i, j );
			if( t1.imag() != 0  || t2.imag() != 0 )
			{
				cerr << "Error, Oring a complex number." << endl;
				Matrix ret;
				return ret;
			}
			complex_data[ i * Nrows + j] = t1.real() || t2.real();
		}

	Matrix ret( complex_data, this->Ncolumns, this-> Nrows );
	return ret;
}

Matrix Matrix::operator| ( double value )
{
	if( isempty() )
	{
		cerr << "Error oring an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j ] = data[ i ][ j ].real() || value;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

Matrix Matrix::operator& ( Matrix Mat2 )
{
	if( this->isempty() || Mat2.isempty() )
	{
		cerr << "Error anding an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( Ncolumns == 1 && Nrows == 1 )
	{
		complex<double> temp;
		getElement( &temp, 0, 0 );
		if( temp.imag() != 0 )
		{
			cerr << "Error, anding a complex number." << endl;
			Matrix ret;
			return ret;
		}
		return Mat2 & temp.real();
	}
	if( Mat2.Ncolumns == 1 && Mat2.Nrows == 1 )
	{
		complex<double> temp;
		Mat2.getElement( &temp, 0, 0 );
		if( temp.imag() != 0 )
		{
			cerr << "Error, anding a complex number." << endl;
			Matrix ret;
			return ret;
		}
		return *this & temp.real();
	}
	if( this->Ncolumns != Mat2.Ncolumns || this->Nrows != Mat2.Nrows )
	{
		cerr << "Error comparing matrices with different dimensions." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> complex_data[ this->Nrows * this->Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
		{
			complex<double> t1, t2;
			this->getElement( &t1, i, j );
			Mat2.getElement( &t2, i, j );
			if( t1.imag() != 0  || t2.imag() != 0 )
			{
				cerr << "Error, anding a complex number." << endl;
				Matrix ret;
				return ret;
			}
			complex_data[ i * Nrows + j] = t1.real() && t2.real();
		}

	Matrix ret( complex_data, this->Ncolumns, this-> Nrows );
	return ret;
}

Matrix Matrix::operator& ( double value )
{
	if( isempty() )
	{
		cerr << "Error anding an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j ] = data[ i ][ j ].real() && value;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

/* Operator =. */
Matrix Matrix::operator= ( Matrix Mat2 )
{
	this->clear();
	this->Nrows = Mat2.Nrows;
	this->Ncolumns = Mat2.Ncolumns;
	this->initialized = Mat2.initialized;

	for( size_t i = 0; i < Ncolumns; i++ )
	{
		map< size_t, complex <double> > temp;
		for( size_t j = 0; j < Nrows; j++ )
		{
			complex <double> dat;
			Mat2.getElement( &dat, i, j );
			temp.insert( pair<size_t, complex <double> >( j, dat ) );
		}
		data.insert( pair< size_t, map< size_t, complex <double> > > ( i, temp ) );
	}

	return *this;
}

Matrix Matrix::operator= ( complex<double> val )
{
	clear();
	Nrows = 1;
	Ncolumns = 1;
	initialized = true;
	map< size_t, complex<double> > temp;
	temp.insert( pair< size_t, complex<double> >( 0, val ) );
	data.insert( pair< size_t, map< size_t, complex <double> > >( 0, temp ) );
	return *this;
}

Matrix Matrix::operator= ( double val )
{
	return *this = complex<double>( val, 0 );
}

/* Operator +. */
Matrix Matrix::operator+ ( Matrix Mat2 )
{
	if( this->isempty() || Mat2.isempty() )
	{
		cerr << "Error adding an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( Ncolumns == 1 && Nrows == 1 )
	{
		complex<double> temp;
		this->getElement( &temp, 0, 0 );
		return Mat2 + temp;
	}
	if( Mat2.Ncolumns == 1 && Mat2.Nrows == 1 )
	{
		complex<double> temp;
		Mat2.getElement( &temp, 0, 0 );
		return *this + temp;
	}
	if( this->Ncolumns != Mat2.Ncolumns || this->Nrows != Mat2.Nrows )
	{
		cerr << "Error adding matrices with different dimensions." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> complex_data[ this->Nrows * this->Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
		{
			complex<double> t1, t2;
			this->getElement( &t1, i, j );
			Mat2.getElement( &t2, i, j );
			complex_data[ i * Nrows + j] = t1 + t2;
		}

	Matrix ret( complex_data, this->Ncolumns, this-> Nrows );
	return ret;
}

/* Operator -. */
Matrix Matrix::operator- ( Matrix Mat2 )
{
	if( this->isempty() || Mat2.isempty() )
	{
		cerr << "Error subtracting an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( Ncolumns == 1 && Nrows == 1 )
	{
		complex<double> temp;
		getElement( &temp, 0, 0 );
		return ( Mat2 - temp ) * -1;
	}
	if( Mat2.Ncolumns == 1 && Mat2.Nrows == 1 )
	{
		complex<double> temp;
		Mat2.getElement( &temp, 0, 0 );
		return *this - temp;
	}
	if( this->Ncolumns != Mat2.Ncolumns || this->Nrows != Mat2.Nrows )
	{
		cerr << "Error adding matrices with different dimensions." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> complex_data[ this->Nrows * this->Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
		{
			complex<double> t1, t2;
			this->getElement( &t1, i, j );
			Mat2.getElement( &t2, i, j );
			complex_data[ i * Nrows + j] = t1 - t2;
		}

	Matrix ret( complex_data, this->Ncolumns, this-> Nrows );
	return ret;
}

/* Operator ==: comparing to a matrix. */
Matrix Matrix::operator== ( Matrix Mat2 )
{
	if( this->isempty() || Mat2.isempty() )
	{
		cerr << "Error comparing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( Ncolumns == 1 && Nrows == 1 )
	{
		complex<double> temp;
		getElement( &temp, 0, 0 );
		return Mat2 == temp;
	}
	if( Mat2.Ncolumns == 1 && Mat2.Nrows == 1 )
	{
		complex<double> temp;
		Mat2.getElement( &temp, 0, 0 );
		return *this == temp;
	}
	if( this->Ncolumns != Mat2.Ncolumns || this->Nrows != Mat2.Nrows )
	{
		cerr << "Error comparing matrices with different dimensions." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> complex_data[ this->Nrows * this->Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
		{
			complex<double> t1, t2;
			this->getElement( &t1, i, j );
			Mat2.getElement( &t2, i, j );
			complex_data[ i * Nrows + j] = t1 == t2;
		}

	Matrix ret( complex_data, this->Ncolumns, this-> Nrows );
	return ret;
}

/* Operator ==: Scalar comparison (complex). */
Matrix Matrix::operator== ( complex<double> value )
{
	if( isempty() )
	{
		cerr << "Error comparing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j] = data[ i ][ j ] == value;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

/* Operator ==: Scalar comparison (double). */
Matrix Matrix::operator== ( double value )
{
	return *this == complex<double>( value, 0 );
}

/* Operator !=: comparing to a matrix. */
Matrix Matrix::operator!= ( Matrix Mat2 )
{
	if( this->isempty() || Mat2.isempty() )
	{
		cerr << "Error comparing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( Ncolumns == 1 && Nrows == 1 )
	{
		complex<double> temp;
		getElement( &temp, 0, 0 );
		return Mat2 != temp;
	}
	if( Mat2.Ncolumns == 1 && Mat2.Nrows == 1 )
	{
		complex<double> temp;
		Mat2.getElement( &temp, 0, 0 );
		return *this != temp;
	}
	if( this->Ncolumns != Mat2.Ncolumns || this->Nrows != Mat2.Nrows )
	{
		cerr << "Error comparing matrices with different dimensions." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> complex_data[ this->Nrows * this->Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
		{
			complex<double> t1, t2;
			this->getElement( &t1, i, j );
			Mat2.getElement( &t2, i, j );
			complex_data[ i * Nrows + j] = t1 != t2;
		}

	Matrix ret( complex_data, this->Ncolumns, this-> Nrows );
	return ret;
}

/* Operator !=: Scalar comparison (complex). */
Matrix Matrix::operator!= ( complex<double> value )
{
	if( isempty() )
	{
		cerr << "Error comparing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j] = data[ i ][ j ] != value;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

/* Operator !=: Scalar comparison (double). */
Matrix Matrix::operator!= ( double value )
{
	return *this != complex<double>( value, 0 );
}

/* Operator >: comparing to a matrix. */
Matrix Matrix::operator> ( Matrix Mat2 )
{
	if( this->isempty() || Mat2.isempty() )
	{
		cerr << "Error comparing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( Ncolumns == 1 && Nrows == 1 )
	{
		complex<double> temp;
		getElement( &temp, 0, 0 );
		return Mat2 < temp;
	}
	if( Mat2.Ncolumns == 1 && Mat2.Nrows == 1 )
	{
		complex<double> temp;
		Mat2.getElement( &temp, 0, 0 );
		return *this > temp;
	}
	if( this->Ncolumns != Mat2.Ncolumns || this->Nrows != Mat2.Nrows )
	{
		cerr << "Error comparing matrices with different dimensions." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> complex_data[ this->Nrows * this->Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
		{
			complex<double> t1, t2;
			this->getElement( &t1, i, j );
			Mat2.getElement( &t2, i, j );
			complex_data[ i * Nrows + j] = t1.real() > t2.real();
		}

	Matrix ret( complex_data, this->Ncolumns, this-> Nrows );
	return ret;
}

/* Operator >: Scalar comparison (double). */
Matrix Matrix::operator> ( double value )
{
	if( isempty() )
	{
		cerr << "Error comparing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j] = data[ i ][ j ].real() > value;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

/* Operator >: Scalar comparison (complex). */
Matrix Matrix::operator> ( complex<double> value )
{
	return *this > value.real() ;
}

/* Operator <: comparing to a matrix. */
Matrix Matrix::operator< ( Matrix Mat2 )
{
	if( this->isempty() || Mat2.isempty() )
	{
		cerr << "Error comparing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( Ncolumns == 1 && Nrows == 1 )
	{
		complex<double> temp;
		getElement( &temp, 0, 0 );
		return Mat2 > temp;
	}
	if( Mat2.Ncolumns == 1 && Mat2.Nrows == 1 )
	{
		complex<double> temp;
		Mat2.getElement( &temp, 0, 0 );
		return *this < temp;
	}
	if( this->Ncolumns != Mat2.Ncolumns || this->Nrows != Mat2.Nrows )
	{
		cerr << "Error comparing matrices with different dimensions." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> complex_data[ this->Nrows * this->Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
		{
			complex<double> t1, t2;
			this->getElement( &t1, i, j );
			Mat2.getElement( &t2, i, j );
			complex_data[ i * Nrows + j] = t1.real() < t2.real();
		}

	Matrix ret( complex_data, this->Ncolumns, this-> Nrows );
	return ret;
}

/* Operator <: Scalar comparison (double). */
Matrix Matrix::operator< ( double value )
{
	if( isempty() )
	{
		cerr << "Error comparing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j] = data[ i ][ j ].real() < value;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

/* Operator <: Scalar comparison (complex). */
Matrix Matrix::operator< ( complex<double> value )
{
	return *this < value.real() ;
}

/* Operator >=: comparing to a matrix. */
Matrix Matrix::operator>= ( Matrix Mat2 )
{
	if( this->isempty() || Mat2.isempty() )
	{
		cerr << "Error comparing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( Ncolumns == 1 && Nrows == 1 )
	{
		complex<double> temp;
		getElement( &temp, 0, 0 );
		return Mat2 >= temp;
	}
	if( Mat2.Ncolumns == 1 && Mat2.Nrows == 1 )
	{
		complex<double> temp;
		Mat2.getElement( &temp, 0, 0 );
		return *this >= temp;
	}
	if( this->Ncolumns != Mat2.Ncolumns || this->Nrows != Mat2.Nrows )
	{
		cerr << "Error comparing matrices with different dimensions." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> complex_data[ this->Nrows * this->Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
		{
			complex<double> t1, t2;
			this->getElement( &t1, i, j );
			Mat2.getElement( &t2, i, j );
			complex_data[ i * Nrows + j] = t1.real() >= t2.real();
		}

	Matrix ret( complex_data, this->Ncolumns, this-> Nrows );
	return ret;
}

/* Operator >=: Scalar comparison (double). */
Matrix Matrix::operator>= ( double value )
{
	if( isempty() )
	{
		cerr << "Error comparing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j] = data[ i ][ j ].real() >= value;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

/* Operator >=: Scalar comparison (complex). */
Matrix Matrix::operator>= ( complex<double> value )
{
	return *this >= value.real() ;
}

/* Operator <=: comparing to a matrix. */
Matrix Matrix::operator<= ( Matrix Mat2 )
{
	if( this->isempty() || Mat2.isempty() )
	{
		cerr << "Error comparing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( Ncolumns == 1 && Nrows == 1 )
	{
		complex<double> temp;
		getElement( &temp, 0, 0 );
		return Mat2 <= temp;
	}
	if( Mat2.Ncolumns == 1 && Mat2.Nrows == 1 )
	{
		complex<double> temp;
		Mat2.getElement( &temp, 0, 0 );
		return *this <= temp;
	}
	if( this->Ncolumns != Mat2.Ncolumns || this->Nrows != Mat2.Nrows )
	{
		cerr << "Error comparing matrices with different dimensions." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> complex_data[ this->Nrows * this->Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
		{
			complex<double> t1, t2;
			this->getElement( &t1, i, j );
			Mat2.getElement( &t2, i, j );
			complex_data[ i * Nrows + j] = t1.real() <= t2.real();
		}

	Matrix ret( complex_data, this->Ncolumns, this-> Nrows );
	return ret;
}

/* Operator <=: Scalar comparison (double). */
Matrix Matrix::operator<= ( double value )
{
	if( isempty() )
	{
		cerr << "Error comparing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j] = data[ i ][ j ].real() <= value;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

/* Operator <=: Scalar comparison (complex). */
Matrix Matrix::operator<= ( complex<double> value )
{
	return *this <= value.real() ;
}




/* Operator *: Matrix multiplication. */
Matrix Matrix::operator* ( Matrix Mat2 )
{
	size_t Ncolumns2, Nrows2;

	Mat2.size( &Ncolumns2, &Nrows2 );
	if( Ncolumns == 1 && Nrows == 1 )
	{
		complex<double> temp;
		getElement( &temp, 0, 0 );
		return Mat2 * temp;
	}
	if( Mat2.Ncolumns == 1 && Mat2.Nrows == 1 )
	{
		complex<double> temp;
		Mat2.getElement( &temp, 0, 0 );
		return *this * temp;
	}
	if( this -> Ncolumns != Nrows2 )
	{
		cerr << "Error wrong dimensions for multiplying matrices." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> dat [ Nrows * Ncolumns2 ], temp;
	for( size_t i = 0; i < Ncolumns2; i++ )
		for( size_t j = 0; j < Nrows; j++ )
		{
			dat[ i * Nrows + j ] = complex<double>( 0, 0 );
			for( size_t k = 0; k < Nrows2; k++ )
			{
				Mat2.getElement( &temp, i, k );
				dat[ i * Nrows + j ] += data[ k ][ j ] * temp;
			}
		}

	Matrix ret( dat, Ncolumns2, Nrows );
	return ret;
}

/* Operator *: Scalar multiplication (complex). */
Matrix Matrix::operator* ( complex<double> factor )
{
	if( isempty() )
	{
		cerr << "Error multiplying an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j] = data[ i ][ j ] * factor;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

/* Operator *: Scalar multiplication (double). */
Matrix Matrix::operator* ( double factor )
{
	if( isempty() )
	{
		cerr << "Error multiplying an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j] = data[ i ][ j ] * factor;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

/* Operator /: Matrix division. */
Matrix Matrix::operator/ ( Matrix Mat2 )
{
	Matrix ret = Mat2.inverse();
	if( Mat2.Ncolumns == 1 && Mat2.Nrows == 1 )
	{
		complex<double> temp;
		Mat2.getElement( &temp, 0, 0 );
		return *this / temp;
	}
	if( ret.isempty() )
	{
		Matrix ret;
		return ret;
	}
	ret = *this * ret;
	return ret;
}

/* Operator /: Scalar division (double). */
Matrix Matrix::operator/ ( double factor )
{
	if( isempty() )
	{
		cerr << "Error dividing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j] = data[ i ][ j ] / factor;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

/* Operator /: Scalar division (complex). */
Matrix Matrix::operator/ ( complex<double> factor )
{
	if( isempty() )
	{
		cerr << "Error dividing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j] = data[ i ][ j ] / factor;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

/* Operator +: Scalar addition (double). */
Matrix Matrix::operator+ ( double factor )
{
	if( isempty() )
	{
		cerr << "Error adding an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j] = data[ i ][ j ] + factor;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

/* Operator +: Scalar addition (complex). */
Matrix Matrix::operator+ ( complex<double> factor )
{
	if( isempty() )
	{
		cerr << "Error adding an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j] = data[ i ][ j ] + factor;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

/* Operator -: Scalar subtraction (double). */
Matrix Matrix::operator- ( double factor )
{
	if( isempty() )
	{
		cerr << "Error subtracting an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j] = data[ i ][ j ] - factor;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

/* Operator -: Scalar subtraction (complex). */
Matrix Matrix::operator- ( complex<double> factor )
{
	if( isempty() )
	{
		cerr << "Error subtracting an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	
	complex <double> complex_data[ Nrows * Ncolumns ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
			complex_data[ i * Nrows + j] = data[ i ][ j ] - factor;

	Matrix ret( complex_data, Ncolumns, Nrows );
	return ret;
}

/* Matlab: exp(). */
Matrix exp( Matrix mat )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = exp( dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

complex<double> round( complex<double> value)
{
	return complex<double>( round( value.real() ), round( value.imag() ) );

}

complex<double> ceil( complex<double> value)
{
	return complex<double>( ceil( value.real() ), ceil( value.imag() ) );

}

complex<double> floor( complex<double> value)
{
	return complex<double>( floor( value.real() ), floor( value.imag() ) );

}

/* Matlab: round(). */
Matrix round( Matrix mat )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = round( dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}


/* Matlab: sqrt(). */
Matrix sqrt( Matrix mat )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = sqrt( dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

/* Matlab: conj(). */
Matrix conj( Matrix mat )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = conj( dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

/* Matlab: real(). */
Matrix real( Matrix mat )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = real( dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

/* Matlab: imag(). */
Matrix imag( Matrix mat )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = imag( dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

/* Matlab: abs(). */
Matrix abs( Matrix mat )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = abs( dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

/* Matlab: ~). */
Matrix Matrix::operator! ()
{
	complex<double> dat[ Ncolumns * Nrows ];
	for( size_t i = 0; i < Ncolumns; i++ )
		for( size_t j = 0; j < Nrows; j++ )
		{
			getElement( dat + i * Nrows + j, i, j );
			if( dat[ i * Nrows + j ].imag() != 0 )
			{
				cerr << "Error: Applying the not operator to a complex number." << endl;
				Matrix ret;
				return ret;
			}
			dat[ i * Nrows + j ] = dat[ i * Nrows + j ].real() != 0 ? 0 : 1 ;
		}
	Matrix ret( dat, Ncolumns, Nrows );
	return ret;
}


/* Matlab: tan(). */
Matrix tan( Matrix mat )
{
	if( mat.isempty() )
	{
		cerr << "Error caluclating tan of an empty matrix" << endl;
		Matrix ret;
		return ret;
	}
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = tan( dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

/* Matlab: sin(). */
Matrix sin( Matrix mat )
{
	if( mat.isempty() )
	{
		cerr << "Error caluclating sin of an empty matrix" << endl;
		Matrix ret;
		return ret;
	}
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = sin( dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

/* Matlab: atan2(). */
complex<double> atan2( complex<double> val1, complex <double> val2 )
{
	return atan( val1.real() / val2.real() );
}

Matrix atan2( Matrix mat, complex <double> val )
{
	if( mat.isempty() )
	{
		cerr << "Error caluclating atan2 of an empty matrix" << endl;
		Matrix ret;
		return ret;
	}
	size_t col, row;
	mat.size( &col, &row );
	double dat[ col * row ];
	complex<double> temp;
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			dat[ i * row + j ] = atan( temp.real() / val.real() );
		}
	Matrix ret( dat, col, row );
	return ret;
}

Matrix atan2( complex <double> val, Matrix mat)
{
	if( mat.isempty() )
	{
		cerr << "Error caluclating atan2 of an empty matrix" << endl;
		Matrix ret;
		return ret;
	}
	size_t col, row;
	mat.size( &col, &row );
	double dat[ col * row ];
	complex<double> temp;
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			dat[ i * row + j ] = atan( val.real() / temp.real() );
		}
	Matrix ret( dat, col, row );
	return ret;
}

Matrix atan2( Matrix mat1, Matrix mat2 )
{
	if( mat1.isempty() || mat2.isempty() )
	{
		cerr << "Error caluclating atan2 of an empty matrix" << endl;
		Matrix ret;
		return ret;
	}
	size_t col1, row1, col2, row2;
	mat1.size( &col1, &row1 );
	mat2.size( &col2, &row2 );
	if( col1 == 1 && row1 == 1)
	{
		complex <double> temp;
		mat1.getElement( &temp, 0, 0 );
		return atan2( temp, mat2 );
	}
	else if( col2 == 1 && row2 == 1)
	{
		complex <double> temp;
		mat2.getElement( &temp, 0, 0 );
		return atan2( mat1, temp );
	}
	else if( col1 != col2 || row1 != row2 )
	{
		cerr << "Error: Atan2: Matrices dimensions mismatch." << endl;
		Matrix ret;
		return ret;
	}
	complex<double> temp, temp2;
	double dat[ col1 * row1 ];
	for( size_t i = 0; i < col1; i++ )
		for( size_t j = 0; j < row1; j++ )
		{
			mat1.getElement( &temp2, i, j );
			mat2.getElement( &temp, i, j );
			dat[ i * row1 + j ] = atan( temp2.real() / temp.real() );
		}
	Matrix ret( dat, col1, row1 );
	return ret;
}

/* Matlab: cos(). */
Matrix cos( Matrix mat )
{
	if( mat.isempty() )
	{
		cerr << "Error caluclating cos of an empty matrix" << endl;
		Matrix ret;
		return ret;
	}
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = cos( dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

/* Matlab: log10(). */
Matrix log10( Matrix mat )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = log10( dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

/* Matlab: mpower(). */
Matrix mpower( Matrix mat, double p )
{
	if( mat.isempty() )
	{
		cerr << "Error: power of an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	size_t col, row;
	mat.size( &col, &row );
	if( col != row )
	{
		cerr << "Error: power of a non-square matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( col == 1 )
	{
		complex<double> temp;
		mat.getElement( &temp, 0, 0 );
		temp = pow( temp, p );
		Matrix ret( &temp, 1, 1 );
		return ret;
	}
	if( p != fix( p ) )
	{
		cerr << "Warning: Matrix raised to a non-integer power. Rounding power." << endl;
		p = round( p );
	}
	Matrix ret = mat;
	for( size_t i = 1; i < p; i++ )
		ret = ret * mat;
	return ret;
}
/* Matlab: power() || .^ */
Matrix power( Matrix Mata, Matrix Matb )
{
	if( Mata.isempty() || Matb.isempty() )
	{
		cerr << "Error adding an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	size_t cola, colb, rowa, rowb;
	Mata.size( &cola, &rowa );
	Matb.size( &colb, &rowb );

	if( cola == 1 && rowa == 1 )
	{
		complex<double> temp;
		Mata.getElement( &temp, 0, 0 );
		return power( temp, Matb );
	}
	if( colb == 1 && rowb == 1 )
	{
		complex<double> temp;
		Matb.getElement( &temp, 0, 0 );
		return power( Mata, temp );
	}

	if( cola != colb || rowa != rowb )
	{
		cerr << "Error adding matrices with different dimensions." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> complex_data[ rowa * cola ];
	for( size_t i = 0; i < cola; i++ )
		for( size_t j = 0; j < rowa; j++ )
		{
			complex<double> t1, t2;
			Mata.getElement( &t1, i, j );
			Matb.getElement( &t2, i, j );
			complex_data[ i * rowa + j] = pow( t1, t2 );
		}

	Matrix ret( complex_data, cola, rowa );
	return ret;
}

Matrix power( Matrix mat, double val )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = pow( dat[ i * rows + j ], val );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

Matrix power( Matrix mat, std::complex<double> val )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = pow( dat[ i * rows + j ], val );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

Matrix power( double val, Matrix mat )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = pow( val, dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

Matrix power( std::complex<double> val, Matrix mat )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = pow( val, dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;

}

/* Matlab: pow2(). */
Matrix pow2( Matrix mat )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = pow( complex<double>( 2, 0 ), dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

/* Matlab: log2(). */
Matrix log2( Matrix mat )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			//dat[ i * rows + j ] = log( dat[ i * rows + j ] ) / log( complex<double>( 2, 0 ) );
			dat[ i * rows + j ] = log2( dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

complex <double> log2( complex <double> val )
{
	return log( val ) / log( complex<double>( 2, 0 ) );
}

double log2( double val )
{
	return log( val ) / log( 2 );
}

/* Matlab: nextpow2(). */
double nextpow2( double val )
{
	return ceil( log2( val ) );
}


Matrix nextpow2( Matrix mat )
{
	return ceil( log2( mat ) );
}

/* Matlab: ceil(). */
Matrix ceil( Matrix mat )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = ceil( dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

/* Matlab: floor(). */
Matrix floor( Matrix mat )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = floor( dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

/* Creating a square matrix of ones. */
Matrix ones( size_t n )
{
	complex<double> dat[ n * n ];
	for( size_t i = 0; i < n*n; i++ )
		dat[ i ] = complex<double>( 1, 0 );
	Matrix ret( dat, n, n );
	return ret;
}

/* Creating a matrix of ones. */
Matrix ones( size_t row, size_t col )
{
	complex<double> dat[ col * row ];
	for( size_t i = 0; i < col*row; i++ )
		dat[ i ] = complex<double>( 1, 0 );
	Matrix ret( dat, col, row );
	return ret;
}

/* Creating a square matrix of zeros. */
Matrix zeros( size_t n )
{
	complex<double> dat[ n * n ];
	for( size_t i = 0; i < n*n; i++ )
		dat[ i ] = complex<double>( 0, 0 );
	Matrix ret( dat, n, n );
	return ret;
}

/* Creating a matrix of zeros. */
Matrix zeros( size_t row, size_t col )
{
	complex<double> dat[ col * row ];
	for( size_t i = 0; i < col*row; i++ )
		dat[ i ] = complex<double>( 0, 0 );
	Matrix ret( dat, col, row );
	return ret;
}

/* Creating a square identity matrix. */
Matrix eye( size_t n )
{
	complex<double> dat[ n * n ];
	for( size_t i = 0; i < n; i++ )
		for( size_t j = 0; j < n; j++ )
			dat[ i * n + j ] = complex<double>( i == j, 0 );
	Matrix ret( dat, n, n );
	return ret;
}

/* Creating an identity matrix. */
Matrix eye( size_t row, size_t col )
{
	complex<double> dat[ col * row ];
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
			dat[ i * row + j ] = complex<double>( i == j, 0 );
	Matrix ret( dat, col, row );
	return ret;
}

/* Creating a square random (normal distribution) matrix. */
/* Internal function */
double randn()
{
	float u = (double) rand() / RAND_MAX;
	float v = (double) rand() / RAND_MAX;
	double s = pow( u, 2 ) + pow( v, 2 );
	double r = u * sqrt( -2 * log( s ) / s );
	if( !isnan( r ) )
		return r;
	r = v * sqrt( -2 * log( s ) / s );
	if( !isnan( r ) )
		return r;
	return randn();
}

Matrix randn( size_t n )
{
	return randn( n, n );
}

/* Creating an random (normal distribution) matrix. */
Matrix randn( size_t row, size_t col )
{
	//double dat[ col] [ row ];
	double *dat = (double *) malloc( col * row * sizeof( double ) );
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
			*( dat + i * col + j ) = randn();
	Matrix ret( dat, col, row );
	return ret;
}

/* Matlab: length(). */
size_t length( Matrix mat )
{
	size_t col, row;
	mat.size( &col, &row );
	return col > row ? col : row;
}

/* Matlab: size(). */
Matrix size( Matrix mat )
{
	size_t col, row;
	mat.size( &col, &row );
	double dat[ 2 ] = { col, row };
	Matrix ret( dat, 2, 1 );
	return ret;
}

size_t size( Matrix mat, size_t dim )
{
	size_t col, row;
	mat.size( &col, &row );
	if( dim == 1 )
		return row;
	else if( dim == 2 )
		return col;
	cerr << "Error getting size for an unsupported dimension." << endl;
	return 0;
}

/* Matlab: max(): One matrix. */
Matrix max( Matrix mat )
{
	size_t col, row;
	mat.size( &col, &row );
	complex<double> dat[ col ], temp;
	for( size_t i = 0; i < col; i++ )
	{
		dat[ i ] = complex<double>( 0, 0 );
		for( size_t j = 0; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			if( dat[ i ].real() < temp.real() )
				dat[ i ] = temp;
		}
	}
	Matrix ret( dat, col, 1 );
	return ret;
}

/* Matlab: min(): One matrix. */
Matrix min( Matrix mat )
{
	size_t col, row;
	mat.size( &col, &row );
	complex<double> dat[ col ], temp;
	for( size_t i = 0; i < col; i++ )
	{
		mat.getElement( &temp, i, 0 );
		dat[ i ] = temp;
		for( size_t j = 1; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			if( dat[ i ].real() > temp.real() )
				dat[ i ] = temp;
		}
	}
	Matrix ret( dat, col, 1 );
	return ret;
}

/* Matlab: max(): Two matrices. */
Matrix max( Matrix mat1, Matrix mat2 )
{
	size_t col1, row1, col2, row2;
	mat1.size( &col1, &row1 );
	mat2.size( &col2, &row2 );
	
	if( col1 == 1 && row1 == 1)
	{
		complex<double> temp;
		mat1.getElement( &temp, 0, 0 );
		return max( mat2, temp.real() );
	}
	if( col2 == 1 && row2 == 1)
	{
		complex<double> temp;
		mat2.getElement( &temp, 0, 0 );
		return max( mat1, temp.real() );
	}
	if( !( col1 == col2 && row1 == row2 ) )
	{
		cerr << "Error: Max: Dimensions mismatch." << endl;
		Matrix ret;
		return ret;
	}

	complex<double> dat[ col1 * row1 ], temp1, temp2;
	for( size_t i = 0; i < col1; i++ )
		for( size_t j = 0; j < row1; j++ )
		{
			mat1.getElement( &temp1, i, j );
			mat2.getElement( &temp2, i, j );
			dat[ i * row1 + j ] = temp1.real() > temp2.real() ? temp1 : temp2;
		}

	Matrix ret( dat, col1, row1 );
	return ret;
}

Matrix max( Matrix mat1, double val )
{
	size_t col1, row1;
	mat1.size( &col1, &row1 );
	if( mat1.isempty() )
	{
		cerr << "Error checking max values for an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	complex<double> dat[ col1 * row1 ], temp1;
	for( size_t i = 0; i < col1; i++ )
		for( size_t j = 0; j < row1; j++ )
		{
			mat1.getElement( &temp1, i, j );
			dat[ i * row1 + j ] = temp1.real() > val ? temp1 : val;
		}

	Matrix ret( dat, col1, row1 );
	return ret;
}

double max( double val1, double val2 )
{
	return val1 > val2? val1 : val2;
}

/* Matlab: min(): Two matrices. */
Matrix min( Matrix mat1, Matrix mat2 )
{
	size_t col1, row1, col2, row2;
	mat1.size( &col1, &row1 );
	mat2.size( &col2, &row2 );
	
	if( !( col1 == col2 && row1 == row2 ) )
	{
		cerr << "Error: Min: Dimensions mismatch." << endl;
		Matrix ret;
		return ret;
	}

	complex<double> dat[ col1 * row1 ], temp1, temp2;
	for( size_t i = 0; i < col1; i++ )
		for( size_t j = 0; j < row1; j++ )
		{
			mat1.getElement( &temp1, i, j );
			mat2.getElement( &temp2, i, j );
			dat[ i * row1 + j ] = temp1.real() < temp2.real() ? temp1 : temp2;
		}

	Matrix ret( dat, col1, row1 );
	return ret;

}

Matrix min( Matrix mat1, double val )
{
	size_t col1, row1;
	mat1.size( &col1, &row1 );
	if( mat1.isempty() )
	{
		cerr << "Error checking min values for an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	complex<double> dat[ col1 * row1 ], temp1;
	for( size_t i = 0; i < col1; i++ )
		for( size_t j = 0; j < row1; j++ )
		{
			mat1.getElement( &temp1, i, j );
			dat[ i * row1 + j ] = temp1.real() < val ? temp1 : val;
		}

	Matrix ret( dat, col1, row1 );
	return ret;
}

double min( double val1, double val2 )
{
	return val1 < val2? val1 : val2;
}

/* Matlab: sum(): with dimension. */
Matrix sum( Matrix mat )
{
	size_t col, row;
	mat.size( &col, &row );
	if( row == 1 )
		return sum( mat, 2 );
	return sum( mat, 1 );
}

/* Matlab: sum(): One matrix. */
Matrix sum( Matrix mat, size_t dim )
{
	if( mat.isempty() )
	{
		cerr << "Error: Summing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( dim != 2 && dim != 1 )
	{
		cerr << "Error: sum: Dimension not supported." << endl;
		Matrix ret;
		return ret;
	}
	size_t col, row;
	mat.size( &col, &row );
	if( dim == 1 )
	{
		complex<double> dat[ row ], temp;
		for( size_t i = 0; i < col; i++ )
		{
			dat[ i ] = complex<double>( 0, 0 );
			for( size_t j = 0; j < row; j++ )
			{
				mat.getElement( &temp, i, j );
				dat[ i ] += temp;
			}
		}
		Matrix ret( dat, col, 1 );
		return ret;
	}
	else if( dim == 2 )
	{
		complex<double> dat[ row ], temp;
		for( size_t i = 0; i < row; i++ )
		{
			dat[ i ] = complex<double>( 0, 0 );
			for( size_t j = 0; j < col; j++ )
			{
				mat.getElement( &temp, j, i );
				dat[ i ] += temp;
			}
		}
		Matrix ret( dat, 1, row );
		return ret;
	}
	/*Shouldn't reach here.*/
	Matrix ret;
	return ret;
}

/* Matlab: mean(): with dimension. */
Matrix mean( Matrix mat )
{
	size_t col, row;
	mat.size( &col, &row );
	if( row == 1 )
		return mean( mat, 2 );
	return mean( mat, 1 );
}

/* Matlab: mean(): One matrix. */
Matrix mean( Matrix mat, size_t dim )
{
	if( mat.isempty() )
	{
		cerr << "Error: getting the mean an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( dim != 2 && dim != 1 )
	{
		cerr << "Error: mean: Dimension not supported." << endl;
		Matrix ret;
		return ret;
	}
	size_t col, row;
	mat.size( &col, &row );

	if( dim == 1 )
		return sum( mat, 1 ) / row;
	else if( dim == 2 )
		return sum( mat, 2 ) / col;

	/*Shouldn't reach here.*/
	Matrix ret;
	return ret;
}

/* Matlab: repmat(): square. */
Matrix repmat( Matrix mat, size_t n )
{
	size_t	col, row;
	mat.size( &col, &row );

	complex<double> dat[ col * n * row * n ], temp;
	for( size_t k = 0; k < n; k++ )
		for( size_t i = 0; i < col; i++ )
			for( size_t l = 0; l < n; l++ )
				for( size_t j = 0; j < row; j++ )
				{
					mat.getElement( &temp, i, j );
					dat[ l * row + j + n * row * ( k * col + i ) ] = temp;
				}
	cout << "out" << endl;
	Matrix ret( dat, col * n, row * n );
	return ret;
}

/* Matlab: repmat(): m*n. */
Matrix repmat( Matrix mat, size_t m, size_t n )
{
	size_t	col, row;
	mat.size( &col, &row );
	
	complex<double> dat[ col * n * row * m ], temp;
	for( size_t k = 0; k < n; k++ )
		for( size_t i = 0; i < col; i++ )
			for( size_t l = 0; l < m; l++ )
				for( size_t j = 0; j < row; j++ )
				{
					mat.getElement( &temp, i, j );
					dat[ l * row + j + m * row * ( k * col + i ) ] = temp;
				}
	Matrix ret( dat, col * n, row * m );
	return ret;
}

/* Matlab: find(). */
Matrix find( Matrix mat )
{
	Matrix ret;
	size_t col, row;
	mat.size( &col, &row );

	complex<double> temp;
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			if( temp != complex<double> ( 0, 0 ) )
			{
				double val = i * row + j;
				ret.addRow( &val, 1 );
			}
		}
	return ret;
}

/* matlab: hamming(). */
Matrix hamming( size_t n )
{
	complex<double> dat[ n ];
	for( size_t i = 0; i < n; i++ )
		dat[ i ] = complex<double>( 0.54 - 0.46 * cos( 2 * M_PI * i / ( n - 1 ) ) , 0 );

	Matrix ret( dat, 1, n );
	return ret;
}

/* matlab: hann(). */
Matrix hann( size_t n )
{
	complex<double> dat[ n ];
	for( size_t i = 0; i < n; i++ )
		dat[ i ] = complex<double>( 0.5  * ( 1 - cos( 2 * M_PI * i / ( n - 1 ) ) ), 0 );

	Matrix ret( dat, 1, n );
	return ret;
}

/* matlab: hanning(). */
Matrix hanning( size_t n )
{
	complex<double> dat[ n ];
	for( size_t i = 0; i < n; i++ )
		dat[ i ] = complex<double>( 0.5  * ( 1 - cos( 2 * M_PI * ( i + 1 ) / ( n + 1 ) ) ), 0 );

	Matrix ret( dat, 1, n );
	return ret;
}

/* matlab: reshape(). */
Matrix reshape( Matrix mat, size_t row, size_t col )
{
	return mat.reshape( row, col );
}

/* Internal fft function. */
complex<double> *__fft( complex<double> *dat, size_t n )
{
	complex<double> *ret = new complex<double>[ n ];
	complexFFTData fftdat( n );
	complexFFT myfft( &fftdat );

	for( size_t i = 0; i < n; i++ )
	{
		c_re( fftdat.in[ i ] ) = dat[ i ].real();
		c_im( fftdat.in[ i ] ) = dat[ i ].imag();
	}

	myfft.fwdTransform();


	for( size_t i = 0; i < (size_t) n; i++ )
		ret[ i ] = complex<double>( c_re( fftdat.out[ i ] ), c_im( fftdat.out[ i ] ) );

	return ret;
}

/* Internal ifft function. */
complex<double> *__ifft( complex<double> *dat, size_t n )
{
	complex<double> *ret = new complex<double>[ n ];
	complexFFTData fftdat( n );
	complexFFT myfft( &fftdat );

	for( size_t i = 0; i < n; i++ )
	{
		c_re( fftdat.out[ i ] ) = dat[ i ].real();
		c_im( fftdat.out[ i ] ) = dat[ i ].imag();
	}

	myfft.invTransform();


	for( size_t i = 0; i < (size_t) n; i++ )
		ret[ i ] = complex<double>( c_re( fftdat.in[ i ] ), c_im( fftdat.in[ i ] ) );

	return ret;
}

/* matlab: fft(). */
Matrix fft( Matrix mat )
{
	size_t col, row;
	Matrix ret;

	mat.size( &col, &row );

	if( col == 1 )
	{
		complex<double> dat[ row ], *temp;
		for( size_t i = 0; i < row; i++ )
			mat.getElement( dat + i, 0, i );
		temp = __fft( dat, row );
		ret.addColumn( temp, row );
	}
	else if( row == 1 )
	{
		complex<double> dat[ col ], *temp;
		for( size_t i = 0; i < col; i++ )
			mat.getElement( dat + i, i, 0 );
		temp = __fft( dat, col );
		ret.addRow( temp, col );
	}
	else
	{
		complex<double> dat[ row ], *temp;
		for( size_t i = 0; i < col; i++ )
		{
			for( size_t j = 0; j < row; j++ )
				mat.getElement( dat + j, i, j );
			temp = __fft( dat, row );
			ret.addColumn( temp, row );
		}
	}

	return ret;
}

Matrix fft( Matrix mat, size_t n )
{
	size_t col, row;
	Matrix ret;
	mat.size( &col, &row );

	if( col == 1 )
	{
		complex<double> dat[ n ], *fft;
		for( size_t i = 0; i < ( row < n ? row : n ); i++ )
			mat.getElement( dat + i, 0, i );
		if( n > row )
			for( size_t i = row; i < n; i++ )
				dat[ i ] = complex<double>( 0, 0 );
		fft = __fft( dat, n );
		ret.addColumn( fft, n );
	}
	else if( row == 1 )
	{
		complex<double> dat[ n ], *fft;
		for( size_t i = 0; i < ( col < n ? col : n ); i++ )
			mat.getElement( dat + i, i, 0 );
		if( n > col )
			for( size_t i = col; i < n; i++ )
				dat[ i ] = complex<double>( 0, 0 );
		fft = __fft( dat, n );
		ret.addRow( fft, n );
	}
	else
	{
		complex<double> dat[ n ], *temp;
		for( size_t i = 0; i < col; i++ )
		{
			for( size_t j = 0; j < ( row < n ? row : n ); j++ )
				mat.getElement( dat + j, i, j );
			if( n > row )
				for( size_t j = row; j < n; j++ )
					dat[ j ] = complex<double>( 0, 0 );
			temp = __fft( dat, n );
			ret.addColumn( temp, n );
		}
	}

	return ret;
}

Matrix fft( Matrix mat, size_t n, size_t d )
{
	if( d == 1 )
		return fft( mat, n );
	else if( d == 2 )
		return fft( mat.transpose(), n ).transpose();
	else
	{
		cerr << "FFT: Error Dimension not supported. Only 2-D matrices are supported." << endl;
		Matrix ret;
		return ret;
	}
}

/* matlab: ifft(). */
Matrix ifft( Matrix mat )
{
	size_t col, row;
	Matrix ret;

	mat.size( &col, &row );

	if( col == 1 )
	{
		complex<double> dat[ row ], *temp;
		for( size_t i = 0; i < row; i++ )
			mat.getElement( dat + i, 0, i );
		temp = __ifft( dat, row );
		ret.addColumn( temp, row );
		ret /= (double) row;
	}
	else if( row == 1 )
	{
		complex<double> dat[ col ], *temp;
		for( size_t i = 0; i < col; i++ )
			mat.getElement( dat + i, i, 0 );
		temp = __ifft( dat, col );
		ret.addRow( temp, col );
		ret /= (double) col;
	}
	else
	{
		complex<double> dat[ row ], *temp;
		for( size_t i = 0; i < col; i++ )
		{
			for( size_t j = 0; j < row; j++ )
				mat.getElement( dat + j, i, j );
			temp = __ifft( dat, row );
			ret.addColumn( temp, row );
		}
		ret /= (double) row;
	}

	return ret;
}

Matrix ifft( Matrix mat, size_t n )
{
	size_t col, row;
	Matrix ret;
	mat.size( &col, &row );

	if( col == 1 )
	{
		complex<double> dat[ n ], *fft;
		for( size_t i = 0; i < n; i++ )
			mat.getElement( dat + i, 0, i );
		fft = __ifft( dat, n );
		ret.addColumn( fft, n );
	}
	else if( row == 1 )
	{
		complex<double> dat[ n ], *fft;
		for( size_t i = 0; i < n; i++ )
			mat.getElement( dat + i, i, 0 );
		fft = __ifft( dat, n );
		ret.addRow( fft, n );
	}
	else
	{
		complex<double> dat[ n ], *temp;
		for( size_t i = 0; i < col; i++ )
		{
			for( size_t j = 0; j < n; j++ )
				mat.getElement( dat + j, i, j );
			temp = __ifft( dat, n );
			ret.addColumn( temp, n );
		}
	}

	ret /= n;
	return ret;
}

Matrix ifft( Matrix mat, size_t n, size_t d )
{
	if( d == 1 )
		return ifft( mat, n );
	else if( d == 2 )
		return ifft( mat.transpose(), n ).transpose();
	else
	{
		cerr << "IFFT: Error Dimension not supported. Only 2-D matrices are supported." << endl;
		Matrix ret;
		return ret;
	}
}


/* replacing matlab's (start:end). */
Matrix seq( double start, double end )
{
	return seq( start, 1, end );
}

/* replacing matlab's (start:step:end). */
Matrix seq( double start, double step, double end )
{
	if( step == 0 )
	{
		cerr << "Error creating a squence with 0 step." << endl;
		Matrix ret;
		return ret;
	}
	complex<double> dat[ (long long) ceil( ( end - start + 1 ) / step ) ];
	size_t col = 0;
	for( double i = start; ( step > 0 && i <= end ) || ( step < 0 && i >= end ); i += step )
	{
		dat[ col++ ] = complex<double>( i, 0 );
	}
	Matrix ret( dat, col, 1 );
	return ret;
}

/* matlab: cat(). */
Matrix cat( size_t dim, Matrix mata, Matrix matb )
{
	size_t col, row;
	switch( dim )
	{
		case 1:
			if( size( mata, 2 ) != size( matb, 2 ) )
			{
				cerr << "Error concatinating matrices with inconsistent dimensions." << endl;
				Matrix ret;
				return ret;
			}
			matb.size( &col, &row );
			for( size_t i = 0; i < row; i++ )
			{
				complex <double> dat[ col ];
				for( size_t j = 0; j < col; j++ )
					matb.getElement( dat + j, j, i );
				mata.addRow( dat, col );
			}
			return mata;
			break;
		case 2:
			if( size( mata, 1 ) != size( matb, 1 ) )
			{
				cerr << "Error concatinating matrices with inconsistent dimensions." << endl;
				Matrix ret;
				return ret;
			}
			matb.size( &col, &row );
			for( size_t i = 0; i < col; i++ )
			{
				complex <double> dat[ row ];
				for( size_t j = 0; j < row; j++ )
					matb.getElement( dat + j, i, j );
				mata.addColumn( dat, row );
			}
			return mata;
			break;
		default:
			cerr << "Error concatinating matrices over an unsupported dimension" << endl;
			Matrix ret;
			return ret;
	}
}

/* Fix functions. */
double fix( double val )
{
	return copysign( floor( abs( val ) ), val );
}

complex<double> fix( complex<double> val )
{
	return complex<double>( fix( val.real() ), fix( val.imag() ) );
}

Matrix fix( Matrix mat )
{
	size_t cols, rows;
	mat.size( &cols, &rows );
	complex<double> dat[ cols * rows ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( dat + i * rows + j, i, j );
			dat[ i * rows + j ] = fix( dat[ i * rows + j ] );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

/* Matlab: flipud(). */
Matrix flipud( Matrix mat )
{
	Matrix ret;
	size_t row, col;
	mat.size( &col, &row );
	complex<double> dat[ col ];

	for( size_t i = row - 1; i > 0; i-- )
	{
		for( size_t j = 0; j < col; j++ )
			mat.getElement( dat + j, j, i );
		ret.addRow( dat, col );
	}

	for( size_t j = 0; j < col; j++ )
		mat.getElement( dat + j, j, 0 );
	ret.addRow( dat, col );

	return ret;
}

bool isnan( complex<double> val )
{
	if( isnan( val.real() ) || isnan( val.imag() ) )
		return true;
	return false;
}
/* matlab: any() */
bool any( Matrix mat )
{
	size_t col, row;
	mat.size( &col, &row );
	complex <double> temp;
	for( size_t i = 0; i < col; i++ )
		for( size_t j = 0; j < row; j++ )
		{
			mat.getElement( &temp, i, j );
			if( temp != complex<double>(0,0) && !isnan( temp ) )
				return true;
		}
	return false;
}

/* matlab: rem(). */
double rem( double val1, double val2 )
{
	return ( ( val1 / val2 ) - fix( val1 / val2 ) ) * val2 ;
}

Matrix rem( Matrix mat, double val )
{
	if( mat.isempty() )
	{
		cerr << "Error: calcualting remainder of an empty matrix." << endl;
		Matrix ret;
		return ret;
	}

	if( val == 0 )
	{
		cerr << "Error: rem: division by zero." << endl;
		Matrix ret;
		return ret;
	}

	size_t cols, rows;
	mat.size( &cols, &rows );

	complex <double> temp;
	double data[ rows * cols ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( &temp, i, j );
			if( temp.imag() != 0 )
			{
				cerr << "Error: calculating remainder of an imaginary number." << endl;
				Matrix ret;
				return ret;
			}
			data[ i * rows + j] = rem( temp.real(), val );
		}

	Matrix ret( data, cols, rows );
	return ret;
}

Matrix rem( double val, Matrix mat )
{
	if( mat.isempty() )
	{
		cerr << "Error: calcualting remainder of an empty matrix." << endl;
		Matrix ret;
		return ret;
	}

	size_t cols, rows;
	mat.size( &cols, &rows );

	complex <double> temp;
	double data[ rows * cols ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat.getElement( &temp, i, j );
			if( temp.imag() != 0 )
			{
				cerr << "Error: calculating remainder of an imaginary number." << endl;
				Matrix ret;
				return ret;
			}
			if( temp.real() == 0 )
			{
				cerr << "Error: rem: division by zero." << endl;
				Matrix ret;
				return ret;
			}
			data[ i * rows + j] = rem( val, temp.real() );
		}

	Matrix ret( data, cols, rows );
	return ret;
}

Matrix rem( Matrix mat1, Matrix mat2 )
{
	if( mat1.isempty() || mat2.isempty() )
	{
		cerr << "Error: calcualting remainder of an empty matrix." << endl;
		Matrix ret;
		return ret;
	}

	size_t cols, rows, cols2, rows2;
	mat1.size( &cols, &rows );
	mat2.size( &cols2, &rows2 );

	if( !( cols == cols2 && rows == rows2 ) )
	{
		cerr << "Error: rem: Matrices dimensions don't match." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> temp, temp2;
	double data[ rows * cols ];
	for( size_t i = 0; i < cols; i++ )
		for( size_t j = 0; j < rows; j++ )
		{
			mat1.getElement( &temp, i, j );
			mat2.getElement( &temp2, i, j );
			if( temp.imag() != 0 || temp2.imag() != 0 )
			{
				cerr << "Error: calculating remainder of an imaginary number." << endl;
				Matrix ret;
				return ret;
			}
			if( temp2.real() == 0 )
			{
				cerr << "Error: rem: division by zero." << endl;
				Matrix ret;
				return ret;
			}
			data[ i * rows + j] = rem( temp.real(), temp2.real() );
		}

	Matrix ret( data, cols, rows );
	return ret;

}

/* matlab:conv() */
Matrix conv( Matrix mata, Matrix matb )
{
	if( mata.isempty() || matb.isempty() )
	{
		cerr << "Error convolving an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( !mata.isvector() || !matb.isvector() )
	{
		cerr << "Error convolving non-vector." << endl;
		Matrix ret;
		return ret;
	}
	size_t cola, colb, rowa, rowb;
	mata.size( &cola, &rowa );
	matb.size( &colb, &rowb );
	if( !( cola == rowa ) && ( ( cola == 1 && colb != 1 ) || ( rowa == 1 && rowb != 1 ) ) )
		matb = matb.transpose();
	size_t n = length( mata ) + length( matb ) - 1;
	return ifft( dotProduct( fft( mata, n ), fft( matb, n ) ) );

}

/* Matlab: filter(). */
Matrix filter( Matrix b, Matrix a, Matrix x )
{
	if( !b.isvector() || !a.isvector() || !x.isvector() )
	{
		cerr << "Error: Only 1-D filter is supported. All inputs must be vectors." << endl;
		Matrix ret;
		return ret;
	}

	complex <double> temp;

	a.getElement( &temp, 0 );
	if( temp == complex<double>( 0, 0 ) )
	{
		cerr << "Error: First coefficent of the denominator is a zero." << endl;
		Matrix ret;
		return ret;
	}
	else if( temp != complex< double > ( 1, 0 ) )
	{
		b /= temp;
		a /= temp;
	}

	size_t len = length( x );
	complex <double> z( 0, 0 );
	complex <double> *x_data = (complex <double> *) malloc( len * sizeof( complex <double> ) );
	complex <double> *y_data = (complex <double> *) malloc( len * sizeof( complex <double> ) );
	complex <double> *a_data = (complex <double> *) malloc( length( a ) * sizeof( complex <double> ) );
	complex <double> *b_data = (complex <double> *) malloc( length( b ) * sizeof( complex <double> ) );
	for( size_t i = 0; i < len; i++ )
		x.getElement( x_data + i, i );
	for( size_t i = 0; i < length( b ); i++ )
		b.getElement( b_data + i, i );
	for( size_t i = 0; i < length( a ); i++ )
		a.getElement( a_data + i, i );

	for( size_t i = 0; i < len; i++ )
	{
		*( y_data + i ) = z;
		for( size_t j = 0; j < length( b ) && j <= i; j++ )
			*( y_data + i ) += *( b_data + j ) * *( x_data + i - j );

		for( size_t j = 0; j < length( a ) && j < i; j++ )
			*( y_data + i ) -= *( a_data + j + 1 ) * *( y_data + i - j - 1 );
	}
	
	Matrix y( y_data, size( x, 2 ), size( x, 1 ) );
	return y;
}

/* Matlab: toeplitz(). */
Matrix toeplitz( Matrix row )
{
	return toeplitz( row, row );
}

Matrix toeplitz( Matrix col, Matrix row )
{
	if( col.isempty() || row.isempty() )
	{
		cerr << "Error: Cannot create toeplitz matrix from an empty vector" << endl;
		Matrix ret;
		return ret;
	}

	col = reshape( col, 1, size( col, 1 ) * size( col, 2 ) );
	row = reshape( row, 1, size( row, 1 ) * size( row, 2 ) );

	size_t len_r = length( row );
	size_t len_c = length( col );
	complex <double> * c_data = ( complex<double> * ) malloc( len_c * sizeof( complex<double> ) );
	complex <double> * r_data = ( complex<double> * ) malloc( len_r * sizeof( complex<double> ) );
	complex <double> * o_data = ( complex<double> * ) malloc( len_r * len_c * sizeof( complex<double> ) );

	for( size_t i = 0; i < len_c; i++ )
		col.getElement( c_data + i, i );
	for( size_t i = 0; i < len_r; i++ )
		row.getElement( r_data + i, i );

	if( *r_data != *c_data )
		cerr << "Warning: Toeplitz: First elements of the row and column do not match." << endl; 

	for( size_t i = 0; i < len_r; i++ )
	{
		for( size_t j = 0; j < i; j++ )
			*( o_data + i * len_c + j ) = *( r_data + i - j );

		for( size_t j = 0; j < len_c - i; j++ )
			*( o_data + i * len_c + i + j ) = *( c_data + j );
	}

	Matrix out( o_data, length( row ), length( col ) );
	return out;
}

/* Matlab: xcorr. */
Matrix xcorr( Matrix x )
{
	if( !x.isvector() )
	{
		cerr << "Error: xcorr: Only vector auto-correlation is supported." << endl;
		Matrix ret;
		return ret;
	}

	Matrix c = fft( x, pow( 2, nextpow2( 2 * length( x ) - 1 ) ) );
	c = ifft( power( abs(c), 2 ));

	if( x.isreal() )
		c = real( c );

	size_t maxlag = length( x ) - 1;

	//c = cat( ,c(end-maxlag+1:end,:), c(1:maxlag+1,:) );
	size_t end = length( c );
	if( size( x, 1) == 1)
	{//[ 1 2 3]
		c = cat( 2, c.getSubMatrix( end - maxlag, end - 1, 0, 0 ), c.getSubMatrix( 0, maxlag, 0, 0 ) );
	}
	else
	{//[1; 2; 3 ]
		c = cat( 1, c.getSubMatrix( 0, 0, end - maxlag, end - 1), c.getSubMatrix( 0, 0, 0, maxlag ) );
	}

	return c;
}
