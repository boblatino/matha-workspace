#ifndef __MATRICES_H__
#define __MATRICES_H__

#include <cstddef>
#include <map>
#include <complex>

class Matrix
{
	public:
		/* Constructor(s)/destructor. */
		Matrix();								/* Uninitialized matrix. */
		Matrix( std::complex <double> *data, size_t numberOfColumns, size_t numberOfRows );		/* Initialize the matrix. */
		Matrix( double *data, size_t numberOfColumns, size_t numberOfRows );		/* Initialize the matrix. */
		virtual ~Matrix();

		/* Data insertion and removal methods. */
		/* Data addition: */
		int addRow( double *data, size_t size );					/* Add row of double. */
		int addRow( std::complex<double> *data, size_t size );				/* Add row of complex. */
		int addColumn( double *data, size_t size );					/* Add column of double. */
		int addColumn( std::complex<double> *data, size_t size );				/* Add column of complex. */
		/* Data insertion: */
		int insertRow( double *data, size_t size, size_t position );			/* Insert row of double. */;
		int insertRow( std::complex<double> *data, size_t size, size_t position );		/* Insert row of complex. */
		int insertColumn( double *data, size_t size, size_t position );			/* Insert column of double. */
		int insertColumn( std::complex<double> *data, size_t size, size_t position );	/* Insert column of complex. */
		/* Data Removal: */
		int delRow( size_t );								/* Delete row. */
		int delColumn( size_t );							/* Delete column. */

		/* Arithmatic operators. */
		Matrix operator= ( Matrix );
		Matrix operator+ ( Matrix );
		Matrix operator+ ( std::complex<double> );
		Matrix operator+ ( double );
		Matrix operator- ( Matrix );
		Matrix operator- ( std::complex<double> );
		Matrix operator- ( double );
		Matrix operator* ( Matrix );
		Matrix operator* ( std::complex<double> );
		Matrix operator* ( double );
		Matrix operator/ ( Matrix );
		Matrix operator/ ( std::complex<double> );
		Matrix operator/ ( double );

		template< typename T >
		Matrix operator+= ( T x ) { return *this = *this + x; }
		template< typename T >
		Matrix operator-= ( T x ) { return *this = *this - x; }
		template< typename T >
		Matrix operator*= ( T x ) { return *this = *this * x; }
		template< typename T >
		Matrix operator/= ( T x ) { return *this = *this / x; }

		/* Logic operations. */
		Matrix operator== ( Matrix );
		Matrix operator== ( std::complex<double> );
		Matrix operator== ( double );
		Matrix operator!= ( Matrix );
		Matrix operator!= ( std::complex<double> );
		Matrix operator!= ( double );
		Matrix operator> ( Matrix );
		Matrix operator> ( std::complex<double> );
		Matrix operator> ( double );
		Matrix operator< ( Matrix );
		Matrix operator< ( std::complex<double> );
		Matrix operator< ( double );

		/* Matrix operations. */
		Matrix inverse();
		Matrix transpose();
		Matrix cofactors();
		std::complex<double> determinant();

		/* Element manipulation. */
		int getElement( std::complex <double> *, size_t col, size_t row);
		int setElement( std::complex <double>, size_t col, size_t row);
		int setElement( double, size_t col, size_t row);
		Matrix getSubMatrix( size_t start_col, size_t end_col, size_t start_row, size_t end_row );

		/* Miscellaneous. */
		void size( size_t *, size_t* );
		bool isempty();
		void printout();
		void clear();
		Matrix reshape( size_t, size_t );
		
	private:
		bool initialized;
		size_t Ncolumns;
		size_t Nrows;
		std::map< size_t, std::map< size_t, std::complex<double> > > data;

};



/* Arithmatic functions. */
Matrix dotProduct( Matrix , Matrix  );
Matrix dotDivision( Matrix , Matrix  );

/* Matlab: exp(). */
Matrix exp( Matrix );

/* Matlab: round(). */
Matrix round( Matrix );

/* Matlab: max(). */
Matrix max( Matrix, Matrix );
Matrix max( Matrix );

/* Matlab: min(). */
Matrix min( Matrix, Matrix );
Matrix min( Matrix );

/* Matlab: repmat(). */
Matrix repmat( Matrix, size_t n );
Matrix repmat( Matrix, size_t y, size_t x );

/* Matlab: length(). */
size_t length( Matrix );

/* Matlab: sum(). */
Matrix sum( Matrix );

/* Matlab: sqrt(). */
Matrix sqrt( Matrix );

/* Matlab: find(). */
Matrix find( Matrix );

/* Matlab: pow2(). */
Matrix pow2( Matrix );

/* Matlab: nextpow2. */
Matrix nextpow2( Matrix );

/* Matlab: conj(). */
Matrix conj( Matrix );

/* Matlab: real(). */
Matrix real( Matrix );

/* Matlab: imag(). */
Matrix imag( Matrix );

/* Matlab: log10(). */
Matrix log10( Matrix );

/* Matlab: floor(). */
Matrix floor( Matrix );

/* Matlab: ceil(). */
Matrix ceil( Matrix );

/* Matlab: log2(). */
Matrix log2( Matrix );

/* Creating a matrix of ones. */
Matrix ones( size_t );
Matrix ones( size_t, size_t );

/* Creating a matrix of zeros. */
Matrix zeros( size_t );
Matrix zeros( size_t, size_t );

/* Creating an identity matrix. */
Matrix eye( size_t );
Matrix eye( size_t, size_t );

/* matlab: hamming(). */
Matrix hamming( size_t );

/* matlab: reshape(). */
Matrix reshape( Matrix, size_t, size_t );

/* matlab: flipud(). */
Matrix flipud( Matrix );

/* matlab: fft(). */
Matrix fft( Matrix );
Matrix fft( Matrix, size_t );
Matrix fft( Matrix, size_t, size_t );

/* matlab: fft(). */
Matrix ifft( Matrix );
Matrix ifft( Matrix, size_t );
Matrix ifft( Matrix, size_t, size_t );

/* replacing matlab's (start:end). */
Matrix seq( double start, double end );

/* replacing matlab's (start:step:end). */
Matrix seq( double start, double step, double end );

/* Fix functions. */
double fix( double val );
std::complex<double> fix( std::complex<double> val );
Matrix fix( Matrix mat );

#endif //__MATRICES_H__
