#include "Model.h"



CModel::CModel( )
{
	ZeroMemory( this, sizeof( CModel ) );
}

bool CModel::Initialize( ID3D11Device * device )
{
	m_Texture = new CTexture( );
	if ( !m_Texture->Initialize( device, L"2DArt\\Ana.jpeg" ) )
		return false;
	HRESULT hr;
	SVertex vertices[ ] =
	{
		SVertex(-1.0f, 1.0f, 0.5f, 0.0f,0.0f ),
		SVertex( 1.0f, 1.0f, 0.5f, 1.0f, 0.0f ),
		SVertex( 1.0f,-1.0f, 0.5f, 1.0f, 1.0f ),
		SVertex(-1.0f,-1.0f, 0.5f, 0.0f, 1.0f ),
	};
	m_VertexCount = sizeof( vertices ) / sizeof( vertices[ 0 ] );
	DWORD indices[ ] =
	{
		0, 1, 2,
		0, 2, 3,
	};
	m_IndexCount = sizeof( indices ) / sizeof( indices[ 0 ] );
	m_World = DirectX::XMMatrixIdentity( );
	D3D11_BUFFER_DESC buffDesc = { 0 };
	D3D11_SUBRESOURCE_DATA buffData = { 0 };
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	buffDesc.ByteWidth = sizeof( vertices );
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	buffData.pSysMem = vertices;
	hr = device->CreateBuffer( &buffDesc, &buffData, &m_VertexBuffer );
	IFFAILED( hr, L"Couldn't create vertex buffer" );

	buffDesc.ByteWidth = sizeof( indices );
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	buffData.pSysMem = indices;
	hr = device->CreateBuffer( &buffDesc, &buffData, &m_IndexBuffer );
	IFFAILED( hr, L"Couldn't create index buffer" );

	return true;
}

bool CModel::Initialize( ID3D11Device * device, LPWSTR lpFilepath )
{
	size_t length = lstrlen( lpFilepath );
	wchar_t *extension;
	extension = lpFilepath + length - 3;
	if ( lstrcmp( extension, L"aba" ) != 0 )
	{
		OutputDebugString( L"Trying to read a file that is not Apa Borsec Asset\n" );
		return false;
	}
	std::wifstream ifCitire( lpFilepath );
	UINT ObjectCount;
	wchar_t ch = ( wchar_t ) 255;
	std::wstring word;
	ifCitire >> word;
	std::vector<std::wstring> Objects;
	bool bHasTexture = false, bHasNormals = false;
	/* Read file */
	if ( word == L"Objects:" )
	{
		ifCitire >> ObjectCount;
		while ( ch != '\n' )
			ifCitire.get( ch );
		while ( ObjectCount )
		{
			ifCitire >> word;
			Objects.push_back( word );
			ifCitire.get( ch );
			while ( ch != '\n' )
				ifCitire.get( ch );
			ifCitire >> word;
			if ( word == L"Vertices:" ) // Read info about vertices
			{
				ifCitire >> m_VertexCount;
				ifCitire >> word;
				if ( word == L"Texture:" )
				{
					ifCitire >> word;
					if ( word == L"ENABLED" )
						bHasTexture = true;
				}
				else
					break;
				ifCitire >> word;
				if ( word == L"Normals:" )
				{
					ifCitire >> word;
					if ( word == L"ENABLED" )
						bHasNormals = true;
				}
				else break;
				ifCitire.get( ch );
				while ( ch != '\n' )
					ifCitire.get( ch );
			}
			else
				break;
			// After we read data about vertices
			// We should read vertices
			SVertex vertex;
			for ( UINT i = 0; i < m_VertexCount; ++i )
			{
				ifCitire >> vertex.Position.x >> vertex.Position.y >> vertex.Position.z;
				if ( bHasTexture )
					ifCitire >> vertex.Texture.x >> vertex.Texture.y;
				if ( bHasNormals )
					ifCitire >> vertex.Normal.x >> vertex.Normal.y >> vertex.Normal.z;
				m_vecVertices.push_back( vertex );
			}
			ifCitire.get( ch );
			while ( ch != '}' )
				ifCitire.get( ch );
			ifCitire >> word;
			if ( word == L"Indices:" )
			{
				ifCitire >> m_IndexCount;
				ifCitire.get( ch );
				while ( ch != '\n' )
					ifCitire.get( ch );
				UINT index;
				m_vecIndices.resize( m_IndexCount );
				for ( UINT i = 0; i < m_IndexCount; ++i )
				{
					ifCitire >> index;
					//m_vecIndices.push_back( index );
					m_vecIndices[ m_vecIndices.size( ) - 1 - i ] = index;
				}
			}
			ifCitire.get( ch );
			while ( ch != '}' )
				ifCitire.get( ch );
			ifCitire >> word;
			if ( word == L"Material" )
			{
				ifCitire >> word; // Material name - useless
				ifCitire.get( ch );
				while ( ch != '\n' )
					ifCitire.get( ch );
				while ( ch != '}' )
				{
					ifCitire.get( ch );
					switch ( ch )
					{
					case 'A':
					ifCitire >> word;
					if ( word == L"mbient" )
					{
						ifCitire >> word;
						if ( word == L"color:" )
						{
							float r, g, b;
							ifCitire >> r >> g >> b;
						}
						else if ( word == L"map:" )
						{

						}
					}
					break;
					case 'D':
					ifCitire >> word;
					if ( word == L"iffuse" )
					{
						ifCitire >> word;
						if ( word == L"Color:" )
						{
							float r, g, b;
							ifCitire >> r >> g >> b;
						}
						else if ( word == L"map:" )
						{
							ifCitire >> word;
							m_Texture = new CTexture( );
							if ( !m_Texture->Initialize( device, ( LPWSTR ) word.c_str( ) ) )
							{
								wchar_t buffer[ 500 ];
								swprintf_s( buffer, L"Couldn't open file %ws\n", word.c_str( ) );
								OutputDebugString( buffer );
								return false;
							}
						}
					}
					break;
					case 'S':
					ifCitire >> word;
					if ( word == L"pecular" )
					{
						ifCitire >> word;
						if ( word == L"power:" )
						{
							float specPower;
							ifCitire >> specPower;
						}
						else if ( word == L"color:" )
						{
							float r, g, b;
							ifCitire >> r >> g >> b;
						}
						else if ( word == L"map:" )
						{
							ifCitire >> word;
						}
					}
					break;
					case 'B':
					ifCitire >> word;
					if ( word == L"ump" )
					{
						ifCitire >> word;
						if ( word == L"map" )
						{
							ifCitire >> word;
						}
					}
					break;
					default:
					break;
					}
				}
			}
			ObjectCount--;
		}
	}
	ifCitire.close( );


	/* Create buffers */
	HRESULT hr;
	D3D11_BUFFER_DESC buffDesc = { 0 };
	D3D11_SUBRESOURCE_DATA buffData = { 0 };
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	buffDesc.ByteWidth = sizeof( SVertex ) * m_vecVertices.size( );
	buffData.pSysMem = &m_vecVertices[ 0 ];
	hr = device->CreateBuffer( &buffDesc, &buffData, &m_VertexBuffer );
	IFFAILED( hr, L"Couldn't create a vertex buffer" );
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	buffDesc.ByteWidth = sizeof( DWORD ) * m_vecIndices.size( );
	buffData.pSysMem = &m_vecIndices[ 0 ];
	hr = device->CreateBuffer( &buffDesc, &buffData, &m_IndexBuffer );
	IFFAILED( hr, L"Couldn't create a index buffer" );

	m_World = DirectX::XMMatrixIdentity( );

	return true;
}

void CModel::Render( ID3D11DeviceContext * context )
{
	static UINT Stride = sizeof( SVertex );
	static UINT Offset = 0;
	context->IASetVertexBuffers( 0, 1, &m_VertexBuffer, &Stride, &Offset );
	context->IASetIndexBuffer( m_IndexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0 );
	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

void CModel::Shutdown( )
{
	if ( m_Texture )
	{
		m_Texture->Shutdown( );
		delete m_Texture;
		m_Texture = 0;
	}
	SAFE_RELEASE( m_VertexBuffer );
	SAFE_RELEASE( m_IndexBuffer );
}

CModel::~CModel( )
{
}

void* CModel::operator new( size_t count )
{
	return _aligned_malloc( count,16 );
}

void CModel::operator delete ( void* object )
{
	_aligned_free( object );
}