#include "matrices.h"
#include <iomanip>
#include <iostream>

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
int Matrix::getElement( complex <double> *dat, size_t col, size_t row)
{
	if( col >= Ncolumns || row >= Nrows )
		return -1;
	*dat = data[col][row];
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
			getElement( dat + ( i - sCol ) * ( eRow - sRow + 1 ) + j, i, j );
			 
	Matrix ret( dat, eCol - sCol + 1, eRow - sRow + 1 );
	return ret;
}

int Matrix::addRow( double *row_data, size_t size )
{
	if( size != Ncolumns )
	{
		cerr << "Row size mismatch." << endl;
		return -1;
	}

	for( size_t i = 0; i < size; i++ )
	{
		data[ i ].insert( pair< size_t, complex<double> >( i, complex<double>( *( row_data + i ), 0 ) ) );
		data[ i ][ Nrows ] = complex<double>( *( row_data + i ), 0 ); 				//Why?
	}
	Nrows++;
	return 0;
}

int Matrix::addRow( std::complex<double> *row_data, size_t size )
{
	if( size != Ncolumns )
	{
		cerr << "Row size mismatch." << endl;
		return -1;
	}

	for( size_t i = 0; i < size; i++ )
	{
		data[ i ].insert( pair< size_t, complex<double> >( i, *( row_data + i ) ) );
		data[ i ][ Nrows ] = *( row_data + i );  						//Why?
	}
	Nrows++;
	return 0;
}

int Matrix::addColumn( double *col_data, size_t size )
{
	if( size != Nrows )
	{
		cerr << "Column size mismatch." << endl;
		return -1;
	}

	map< size_t, complex<double> > temp ;
	for( size_t i = 0; i < size; i++ )
	{
		temp.insert( pair< size_t, complex<double> >( i, complex<double>(*( col_data + i ), 0 ) ) );
	}
	data.insert( pair< size_t, map< size_t, complex<double> > >( Ncolumns++, temp ) );
	return 0;
}

int Matrix::addColumn( std::complex<double> *col_data, size_t size )
{
	if( size != Nrows )
	{
		cerr << "Column size mismatch." << endl;
		return -1;
	}

	map< size_t, complex<double> > temp ;
	for( size_t i = 0; i < size; i++ )
	{
		temp.insert( pair< size_t, complex<double> >( i, *( col_data + i ) ) );
	}
	data.insert( pair< size_t, map< size_t, complex<double> > >( Ncolumns++, temp ) );
	return 0;
}

int Matrix::insertRow( double *row_data, size_t size, size_t position )
{
	/* TODO */


	return 0;
}

int Matrix::insertRow( std::complex<double> *row_data, size_t size, size_t position )
{
	/* TODO */


	return 0;
}

int Matrix::insertColumn( double *col_data, size_t size, size_t position )
{
	/* TODO */


	return 0;
}

int Matrix::insertColumn( std::complex<double> *col_data, size_t size, size_t position )
{
	/* TODO */


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

/* Operator +. */
Matrix Matrix::operator+ ( Matrix Mat2 )
{
	if( this->isempty() || Mat2.isempty() )
	{
		cerr << "Error adding an empty matrix." << endl;
		Matrix ret;
		return ret;
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

/* Operator *: Matrix multiplication. */
Matrix Matrix::operator* ( Matrix Mat2 )
{
	/* TODO */
	Matrix ret;
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
	/* TODO */
	Matrix ret;
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


