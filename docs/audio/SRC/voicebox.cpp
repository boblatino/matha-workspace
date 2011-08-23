#include "voicebox.h"
#include <cmath>
#include <cstring>

#include <iostream>

using namespace std;

void estnoisem()
{

}


void specsub()
{

}


Matrix rfft( Matrix x )
{
	size_t col, row, d, n;
	x.size( &col, &row );

	if( length( x ) == 1 )
		return x;
	
	if( row > 1 )
	{
		d = 1;
		n = row;
	}
	else
	{
		d = 2;
		n = col;
	}

	Matrix y = fft( x, n, d );
	if( y.isempty() )
		return y;

	y = y.getSubMatrix( 0, d == 1? fix(n/2):col, 0, d==2? fix(n/2) : row );

	return y;
}

Matrix rfft( Matrix x, size_t n )
{
	size_t col, row;
	x.size( &col, &row );

	if( length( x ) == 1 )
		return x;
	
	size_t d = row > 1 ? 1 : 2;

	Matrix y = fft( x, n, d );
	if( y.isempty() )
		return y;

	y = y.getSubMatrix( 0, d == 1? 1 + fix(n/2):col, 0, d==2? 1+fix(n/2) : row );

	return y;
}

Matrix rfft( Matrix x, size_t n, size_t d)
{
	size_t col, row;
	x.size( &col, &row );

	if( length( x ) == 1 )
		return x;

	Matrix y = fft( x, n, d );
	if( y.isempty() )
		return y;

	y = y.getSubMatrix( 0, d == 2 ? fix(n/2) : col - 1, 0, d == 1 ? fix(n/2) : row - 1 );

	return y;
}

Matrix irfft( Matrix y )
{
/*
function x=irfft(y,n,d)

s=size(y);
ps=prod(s);
ns=length(s);
if ps==1
    x=y
else
    if nargin <3 || isempty(d)
        d=find(s>1);
        d=d(1);
    end
    m=s(d);
    k=ps/m;     % number of fft's to do
    if d==1
        v=reshape(y,m,k);
    else
        v=reshape(permute(y,[d:ns 1:d-1]),m,k);
    end
    if nargin<2 || isempty(n)
        n=2*m-2;        % default output length
    else
        mm=1+fix(n/2);          % expected input length
        if mm>m v=[v; zeros(mm-m,k)];   % zero pad
        elseif mm<m v(mm+1:m,:)=[];     % or truncate
        end
        m=mm;
    end
    if rem(n,2)		% odd output length
        x=real(ifft([v;conj(v(m:-1:2,:))],[],1));    % do it the long way
    else			% even output length
        v(m,:)=real(v(m,:));	% force nyquist element real
        w=ones(1,k);
        %  t=[cumprod([-0.5i; exp(2i*pi/n)*ones(m-2,1)]); 0.5i];
        t=-0.5i* exp((2i*pi/n)*(0:m-1)).';
        z=(t(:,w)+0.5).*(conj(flipud(v))-v)+v;
        z(m,:)=[];
        zz=ifft(z,[],1);
        x=zeros(n,k);
        x(1:2:n,:)=real(zz);
        x(2:2:n,:)=imag(zz);
    end
    s(d)=n;         % change output dimension
    if d==1
        x=reshape(x,s);
    else
        x=permute(reshape(x,s([d:ns 1:d-1])),[ns+2-d:ns 1:ns+1-d]);
    end
end

*/
}


Matrix irfft( Matrix y, size_t n )
{
/*
function x=irfft(y,n,d)

s=size(y);
ps=prod(s);
ns=length(s);
if ps==1
    x=y
else
    if nargin <3 || isempty(d)
        d=find(s>1);
        d=d(1);
    end
    m=s(d);
    k=ps/m;     % number of fft's to do
    if d==1
        v=reshape(y,m,k);
    else
        v=reshape(permute(y,[d:ns 1:d-1]),m,k);
    end
    if nargin<2 || isempty(n)
        n=2*m-2;        % default output length
    else
        mm=1+fix(n/2);          % expected input length
        if mm>m v=[v; zeros(mm-m,k)];   % zero pad
        elseif mm<m v(mm+1:m,:)=[];     % or truncate
        end
        m=mm;
    end
    if rem(n,2)		% odd output length
        x=real(ifft([v;conj(v(m:-1:2,:))],[],1));    % do it the long way
    else			% even output length
        v(m,:)=real(v(m,:));	% force nyquist element real
        w=ones(1,k);
        %  t=[cumprod([-0.5i; exp(2i*pi/n)*ones(m-2,1)]); 0.5i];
        t=-0.5i* exp((2i*pi/n)*(0:m-1)).';
        z=(t(:,w)+0.5).*(conj(flipud(v))-v)+v;
        z(m,:)=[];
        zz=ifft(z,[],1);
        x=zeros(n,k);
        x(1:2:n,:)=real(zz);
        x(2:2:n,:)=imag(zz);
    end
    s(d)=n;         % change output dimension
    if d==1
        x=reshape(x,s);
    else
        x=permute(reshape(x,s([d:ns 1:d-1])),[ns+2-d:ns 1:ns+1-d]);
    end
end

*/
}


Matrix irfft( Matrix y, size_t n, size_t d )
{

	size_t col, row;

	y.size( &col ,&row );
	size_t ps = col * row;
	size_t ns = 2; //length( s );

	if( length( y ) == 1 )
		return y;

	size_t m = d == 1 ? row : col;
	size_t k = ps / m;     // number of fft's to do

	Matrix v, x;
	if( d == 1 )
		v = y;	//v=reshape(y,m,k);
	else
		v = y.transpose();	//v=reshape(permute(y,[d:ns 1:d-1]),m,k);

//	if nargin<2 || isempty(n)
//		n=2*m-2;        // default output length
//	else
		size_t mm = 1 + fix( n / 2 );          // expected input length
		if( mm > m )
		{
			double zer[ col ];
			bzero( zer, col * sizeof( double ) );
			//v = [v; zeros(mm-m,k)];   // zero pad
			for( size_t i = m; i <= mm; i++ )
				v.addRow( zer, col );
		}
		else if( mm < m )
			v = v.getSubMatrix( 0, col - 1, 0, mm - 1 );	//v(mm+1:m,:)=[];     // or truncate
		m = mm;
//	end

	if( n % 2 )		// odd output length
	{
		Matrix temp = v, temp2 = conj( v );
		complex <double> dat[ col ];
		size_t l, ll;
		v.size( &l, &ll );
		for( size_t i = m; i >= 2; i-- )
		{
			for( size_t j = 0; j < l; j++ )
				temp2.getElement( dat + j, j, i - 1  );
			temp.addRow( dat, l );
		}
		x = real( ifft( temp ) );    // do it the long way
	}
	else			// even output length
	{
		//v(m,:)=real(v(m,:));	// force nyquist element real
		complex <double> dat[ k ], dat2[ k ];
		for( size_t i = 0; i < col; i++ )
		{
			v.getElement( dat, i, m - 1 );
			v.setElement( real( *dat ), i, m - 1 );
		}
//		Matrix w = ones( 1, k );
		Matrix t = exp( seq( 0, m - 1 ) * complex<double> ( 0, 2 * M_PI / n ) ).transpose() * complex <double>( 0, -0.5 );
		t = repmat( t, 1, k );
		Matrix z = dotProduct( ( t + 0.5 ), ( conj( flipud( v ) ) - v ) ) + v;
		z.delRow( m - 1 );	//z(m,:)=[];
		z = ifft( z );
//		x = zeros( n, k );
		Matrix zr = real( z );
		Matrix zi = imag( z );
		for( size_t i = 0; i < n / 2; i++ )
		{
			for( size_t j = 0; j < k; j++ )
			{
				zr.getElement( dat + j, j, i );
				zi.getElement( dat2 + j, j, i );
			}
			x.addRow( dat, k );
			x.addRow( dat2, k );
		}
/*		x(1:2:n,:) = real( z );
		x(2:2:n,:) = imag( z );
*/
	}

/*	if( d == 1 )
	{
		x = reshape( x, n, col);
	}
	else
	{
		x = reshape( x, n, col ).transpose();;
	}
*/	
	if( d == 2 )
		x = x.transpose();

	return x;
}

Matrix enframe( Matrix x, Matrix win, size_t inc )
{
	size_t col, row, len;

	x.size( &col, &row );
	size_t nx = col * row;

	size_t nwin = length(win);
	if( nwin == 1 )
	{
		complex<double> temp;
		win.getElement( &temp, 0, 0 );
		len = (size_t) temp.real();
	}
	else
		len = nwin;

	size_t nf = fix((nx-len+inc)/inc);
	complex<double> dat[ len ];
	Matrix f;
	x = x.reshape( col * row, 1 );

	for( size_t i = 0; i < nf; i++ )
	{
		for( size_t j = 0; j < len; j++ )
			x.getElement( dat + j, 0, i * inc + j );

		f.addRow( dat, len );
	}

	if( nwin > 1 )
		f = dotProduct( f, repmat( reshape( win, len, 1 ), nf, 1 ) );

	return f;
}

Matrix enframe( Matrix x, Matrix win )
{
	size_t col, row, len;

	x.size( &col, &row );
	size_t nx = col * row;

	size_t nwin = length(win);
	if( nwin == 1 )
	{
		complex<double> temp;
		win.getElement( &temp, 0, 0 );
		len = (size_t) temp.real();
	}
	else
		len = nwin;

	size_t inc = len;
	size_t nf = fix((nx-len+inc)/inc);
	complex<double> dat[ len ];
	Matrix f;
	x = x.reshape( col * row, 1 );

	for( size_t i = 0; i < nf; i++ )
	{
		for( size_t j = 0; j < len; j++ )
			x.getElement( dat + j, 0, i * inc + j );

		f.addRow( dat, len );
	}

	if( nwin > 1 )
		f = dotProduct( f, repmat( reshape( win, len, 1 ), nf, 1 ) );

	return f;
}

