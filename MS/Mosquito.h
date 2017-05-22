#pragma once


#include "Model.h"

ALIGN16 class CMosquito sealed
{
	std::vector<CModel*> m_vecModels;
	UINT m_numStaticObjects;
	UINT m_numDynamicObjects;
	bool m_bAABBCalculated = false;
	DirectX::XMMATRIX m_StaticWorld;
	DirectX::XMMATRIX m_IdentityMatrix;
public:
	CMosquito( );
	~CMosquito( );
public:
	bool Initialize( ID3D11Device * device, LPWSTR lpList );
	void UpdateWings( );
	void Render( ID3D11DeviceContext * context, UINT objectIndex );
	void Shutdown( );
public:
	void CalculateAABB( );
	void CalculateCenter( );
public:
	inline UINT GetNumberOfObjects( ) { return m_vecModels.size( ); };
	inline CModel* GetModel( UINT index ) { return m_vecModels[ index ]; };
	inline DirectX::XMMATRIX GetModelWorld( UINT index )
	{
		if ( index < m_numStaticObjects )
			return m_StaticWorld;
		return m_vecModels[ index ]->GetWorld( );
	}
	inline void Identity( ) { m_StaticWorld = m_IdentityMatrix; };
	inline void Translate( float x, float y, float z ) { m_StaticWorld *= DirectX::XMMatrixTranslation( x, y, z ); };
	inline void RotateX( float theta ) { m_StaticWorld *= DirectX::XMMatrixRotationX( theta ); };
	inline void RotateY( float theta ) { m_StaticWorld *= DirectX::XMMatrixRotationY( theta ); };
	inline void RotateZ( float theta ) { m_StaticWorld *= DirectX::XMMatrixRotationZ( theta ); };
public:
	inline void * operator new( size_t size )
	{
		return _aligned_malloc( size,16 );
	};
	inline void operator delete ( void* object )
	{
		_aligned_free( object );
	};
};

