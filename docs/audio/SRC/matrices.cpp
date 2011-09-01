#include "matrices.h"
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

/* Matrix determinant. */
complex<double> Matrix::determinant()
{
	if( Nrows != Ncolumns )
		return NAN;
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

/* isempty(). */
bool Matrix::isempty()
{
	return !initialized;
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
	if( Ncolumns * Nrows - 1 < index )
		return -1;
	*dat = data[ index / Nrows ][ index % Nrows ];
	return 0;
}

/* getElement(). */
int Matrix::getElement( complex <double> *dat, size_t col, size_t row )
{
	if( col >= Ncolumns || row >= Nrows )
		return -1;
	*dat = data[col][row];
	return 0;
}

/* setElement(). */
int Matrix::setElement( complex <double> dat, size_t index )
{
	if( Ncolumns * Nrows - 1 < index )
		return -1;
	data[ index / Nrows ][ index % Nrows ] = dat;
	return 0;
}

/* setElement(): complex value. */
int Matrix::setElement( complex <double> dat, size_t col, size_t row)
{
	if( col >= Ncolumns || row >= Nrows )
		return -1;
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
		cerr << "Error adding an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( Ncolumns == 1 && Nrows == 1 )
	{
		complex<double> temp;
		getElement( &temp, 0, 0 );
		return Mat2 - temp;
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
			dat[ i * rows + j ] = log( dat[ i * rows + j ] ) / log( complex<double>( 2, 0 ) );
		}
	Matrix ret( dat, cols, rows );
	return ret;
}

/* Matlab: nextpow2(). */
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

/* Matlab: length(). */
size_t length( Matrix mat )
{
	size_t col, row;
	mat.size( &col, &row );
	return col > row ? col : row;
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
			dat[ i ] = temp1.real() > temp2.real() ? temp1 : temp2;
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
			dat[ i ] = temp1.real() > val ? temp1 : val;
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
			dat[ i ] = temp1.real() < temp2.real() ? temp1 : temp2;
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
			dat[ i ] = temp1.real() < val ? temp1 : val;
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
Matrix sum( Matrix mat, double dim )
{
	if( mat.isempty() )
	{
		cerr << "Error: Summing an empty matrix." << endl;
		Matrix ret;
		return ret;
	}
	if( dim > 2 || dim < 1 )
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
		complex<double> dat[ col ], temp;
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
		for( size_t i = 0; i < n; i++ )
			mat.getElement( dat + i, 0, i );
		fft = __fft( dat, n );
		ret.addColumn( fft, n );
	}
	else if( row == 1 )
	{
		complex<double> dat[ n ], *fft;
		for( size_t i = 0; i < n; i++ )
			mat.getElement( dat + i, i, 0 );
		fft = __fft( dat, n );
		ret.addRow( fft, n );
	}
	else
	{
		complex<double> dat[ n ], *temp;
		for( size_t i = 0; i < col; i++ )
		{
			for( size_t j = 0; j < n; j++ )
				mat.getElement( dat + j, i, j );
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
	complex<double> dat[ (long long) ceil( ( end - start + 1 ) / step ) ];
	size_t col = 0;
	for( double i = start; i <= end; i += step )
	{
		dat[ col++ ] = complex<double>( i, 0 );
	}
	Matrix ret( dat, col, 1 );
	return ret;
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

double rem( double val1, double val2 )
{
	return ( ( val1 / val2 ) - fix( val1 / val2 ) ) * val2 ;
}

