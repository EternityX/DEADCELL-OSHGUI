#include "renderer.hpp"

using namespace renderer;

c_renderer::c_renderer()
	: m_renderer{ }, m_instance{ nullptr } { }

void c_renderer::init( IDirect3DDevice9 *device ) {
	m_renderer = std::make_unique< OSHGui::Drawing::Direct3D9Renderer >( device );

	// initialize oshgui with our renderer.
	OSHGui::Application::Initialize( std::move( m_renderer ) );

	m_instance = OSHGui::Application::InstancePtr();

	m_fonts.resize( FONT_MAX );

	m_fonts[ FONT_VERDANA_7PX ] = OSHGui::Drawing::FontManager::LoadFont( "Verdana", 7.f, true );
	m_fonts[ FONT_VERDANA_BOLD_7PX ] = OSHGui::Drawing::FontManager::LoadFont( "Verdana Bold", 7.f, true );
	m_fonts[ FONT_04B03_6PX ] = OSHGui::Drawing::FontManager::LoadFont( "04b03", 6.f, false );
	m_fonts[ FONT_LUCIDA_CONSOLE ] = OSHGui::Drawing::FontManager::LoadFont( "Tahoma Bold", 7.f, false );

	m_instance->SetDefaultFont( m_fonts[ FONT_VERDANA_7PX ] );
}

void c_renderer::start_drawing( IDirect3DDevice9 *device ) {
	if( !device )
		return;

	m_old_color_write_enable = -1;

	device->GetRenderState( D3DRS_COLORWRITEENABLE, &m_old_color_write_enable );
	device->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );

	m_geometry = m_instance->GetRenderer().CreateGeometryBuffer();
	if( !m_geometry )
		return;

	m_render_target = m_instance->GetRenderer().GetDefaultRenderTarget();
	if( !m_render_target )
		return;

	m_instance->GetRenderer().BeginRendering();
}

void c_renderer::end_drawing( IDirect3DDevice9 *device ) const {
	if( !m_render_target || !m_instance )
		return;

	m_render_target->Activate();

	m_render_target->Draw( *m_geometry );

	m_render_target->Deactivate();

	m_instance->Render();

	m_instance->GetRenderer().EndRendering();

	if( !m_old_color_write_enable )
		return;

	device->SetRenderState( D3DRS_COLORWRITEENABLE, m_old_color_write_enable );
}

void c_renderer::rect( const OSHGui::Drawing::Color &color, int x, int y, int width, int height ) const {
	OSHGui::Drawing::Graphics g( *m_geometry );

	g.DrawRectangle( color, OSHGui::Drawing::PointI( x, y ), OSHGui::Drawing::SizeI( width, height ) );
}

void c_renderer::filled_rect( const OSHGui::Drawing::Color &color, int x, int y, int width, int height ) const {
	OSHGui::Drawing::Graphics g( *m_geometry );

	g.FillRectangle( color, OSHGui::Drawing::PointI( x, y ), OSHGui::Drawing::SizeI( width, height ) );
}

void c_renderer::ansi_text( const OSHGui::Drawing::FontPtr &font, const OSHGui::Drawing::Color &color, const OSHGui::Drawing::Color &shadow_color, float x, float y, int flags, const std::string str, ... ) const {
	va_list va;
	int str_len;
	std::string buf;

	if( str.empty( ) )
		return;

	va_start( va, str );

	str_len = std::vsnprintf( nullptr, 0, str.c_str( ), va );
	if( str_len < 0 ) {
		va_end( va );
		return;
	}

	buf.resize( str_len );

	std::vsnprintf( &buf[ 0 ], str_len + 1, str.c_str( ), va );

	OSHGui::Drawing::Graphics g( *m_geometry );

	if( flags & CENTERED_X || flags & CENTERED_Y ) {
		OSHGui::Misc::TextHelper text_helper( font );
		text_helper.SetText( str );

		if( flags & CENTERED_X )
			x -= ( text_helper.GetSize( ).Width / 2.f );

		if( flags & CENTERED_Y )
			y -= text_helper.GetSize( ).Height / 2.f;
	}

	if( flags & DROPSHADOW )
		g.DrawString( buf, font, shadow_color, x + 1, y + 1 );

	if( flags & OUTLINED ) {
		g.DrawString( buf, font, shadow_color, x, y + 1 );
		g.DrawString( buf, font, shadow_color, x, y - 1 );
		g.DrawString( buf, font, shadow_color, x + 1, y );
		g.DrawString( buf, font, shadow_color, x - 1, y );
	}

	g.DrawString( buf, font, color, x, y );
}

void c_renderer::filled_rect_gradient( const OSHGui::Drawing::ColorRectangle &colors, int x, int y, int width, int height ) const {
	OSHGui::Drawing::Graphics g( *m_geometry );

	g.FillRectangleGradient( colors, OSHGui::Drawing::PointI( x, y ), OSHGui::Drawing::SizeI( width, height ) );
}

OSHGui::Drawing::FontPtr c_renderer::get_font( int index ) {
	return m_fonts[ index ];
}

OSHGui::Application *c_renderer::get_instance( ) const {
	return m_instance;
}

OSHGui::Drawing::Renderer &c_renderer::get_renderer( ) const {
	return m_instance->GetRenderer( );
}