#include "MultiListBox.hpp"
#include "ScrollBar.hpp"
#include "../Misc/Intersection.hpp"
#include "ListBox.hpp"

namespace OSHGui {
	const Drawing::SizeI DefaultSize( 120, 106 );
	const Drawing::SizeI DefaultItemAreaPadding( 8, 8 );
	const int DefaultItemPadding( 2 );

	MultiListBox::MultiListBox() {
		SetSize( DefaultSize );
		maxVisibleItems_ = GetHeight() / ( GetFont()->GetFontHeight() + DefaultItemPadding );
	}

	MultiListBox::~MultiListBox() {
		
	}

	void MultiListBox::OnMouseClick( const MouseMessage &mouse )
	{
		Control::OnMouseClick( mouse );

		if( Intersection::TestRectangle( absoluteLocation_.OffsetEx( 4, 4 ), m_items_size, mouse.GetLocation() ) )
		{
			const auto clickedIndex = m_first_visible_item_index + ( mouse.GetLocation().Y - absoluteLocation_.Y - 4 ) / ( GetFont()->GetFontHeight() + DefaultItemPadding );
			if( clickedIndex < m_items.size() ) {
				SetSelected( clickedIndex );
			}
		}
	}

	void MultiListBox::SetSelected( int index )
	{
		/*if( selectedIndex_ == index )
		{
			return;
		}*/

		m_items.at( index )->selected = !m_items.at( index )->selected;

		//selectedIndexChangedEvent_.Invoke( this );

		if( index - firstVisibleItemIndex_ >= maxVisibleItems_ || index - firstVisibleItemIndex_ < 0 )
		{
			for( firstVisibleItemIndex_ = 0; firstVisibleItemIndex_ <= index; firstVisibleItemIndex_ += maxVisibleItems_ );
			firstVisibleItemIndex_ -= maxVisibleItems_;
			if( firstVisibleItemIndex_ < 0 )
			{
				firstVisibleItemIndex_ = 0;
			}
			//scrollBar_->SetValue( firstVisibleItemIndex_ );
		}

		Invalidate();
	}

	void MultiListBox::DrawSelf( Drawing::RenderContext &context )
	{
		Control::DrawSelf( context );
	}
	//---------------------------------------------------------------------------
	void MultiListBox::PopulateGeometry()
	{
		using namespace Drawing;

		Graphics g( *geometry_ );

		g.FillRectangle( Color::FromARGB( 255, 40, 40, 50 ), PointF( 1, 1 ), GetSize() - SizeF( 2, 2 ) );

		const auto color = GetBackColor() + Color::FromARGB( 0, 54, 53, 60 );
		g.FillRectangle( Color::FromARGB( 255, 18, 18, 24 ), PointF( 0, 0 ), SizeF( GetWidth(), 1 ) );
		g.FillRectangle( Color::FromARGB( 255, 18, 18, 24 ), PointF( 0, 1 ), SizeF( 1, GetHeight() - 1 ) );
		g.FillRectangle( Color::FromARGB( 255, 18, 18, 24 ), PointF( GetWidth() - 1, 1 ), SizeF( 1, GetHeight() - 1 ) );
		g.FillRectangle( Color::FromARGB( 255, 18, 18, 24 ), PointF( 1, GetHeight() - 1 ), SizeF( GetWidth() - 2, 1 ) );

		const auto itemX = 4;
		const auto itemY = 5;
		const auto padding = GetFont()->GetFontHeight() + DefaultItemPadding;
		for( int i = 0; i < maxVisibleItems_ && i + firstVisibleItemIndex_ < static_cast<int>( m_items.size() ); ++i )
		{
			//if( firstVisibleItemIndex_ + i == selectedIndex_ )
			//{
			//	g.FillRectangle(Color::FromARGB( 255, 54, 54, 64 ), PointF(itemX - 1, itemY + i * padding - 1), SizeF(m_items_size.Width + 2, padding));
				g.DrawString( m_items[ firstVisibleItemIndex_ + i ]->GetItemText(), GetFont(), GetForeColor(), PointF( itemX + 1, itemY + i * padding ) );
		//	}
			//else
		//	{
				//g.DrawString( m_items[ firstVisibleItemIndex_ + i ]->get_item_text(), GetFont(), Color::FromARGB( 255, 201, 201, 218 ), PointF( itemX + 1, itemY + i * padding ) );
			//}
		}
	}

	void MultiListBox::SetSize( const Drawing::SizeI &size ) {
		Control::SetSize( size );

		m_items_size = size.InflateEx( -8, -8 );
	}

	void MultiListBox::AddItem( const Misc::AnsiString &text )
	{
		//AddItem( new StringListItem( text ) );
	}
	//---------------------------------------------------------------------------
	void MultiListBox::AddItem( MultiListItem *item )
	{
		InsertItem( !m_items.empty() ? m_items.size() : 0, item );
	}
	//---------------------------------------------------------------------------
	void MultiListBox::InsertItem( int index, const Misc::AnsiString &text )
	{
		InsertItem( index, new StringMultiListItem( text ) );
	}
	//---------------------------------------------------------------------------
	void MultiListBox::InsertItem( int index, MultiListItem *item )
	{
		m_items.insert( m_items.begin() + index, item );

		//CheckForScrollBar();

		//if( autoScrollEnabled_ )
		//{
		//	scrollBar_->SetValue( index );
		//}

		Invalidate();
	}

	StringMultiListItem::StringMultiListItem( const Misc::AnsiString &text ) : text( text ) {
		
	}

	const Misc::AnsiString & StringMultiListItem::GetItemText() const {
		return text;
	}
	//---------------------------------------------------------------------------
	/*void MultiListBox::RemoveItem( int index )
	{
#ifndef OSHGUI_DONTUSEEXCEPTIONS
		if( index < 0 || index >= ( int ) items_.size() )
		{
			throw Misc::ArgumentOutOfRangeException( "index" );
		}
#endif

		m_items.erase( m_items.begin() + index );

		if( scrollBar_->GetVisible() )
		{
			scrollBar_->SetMaximum( items_.size() - maxVisibleItems_ );
		}
		if( selectedIndex_ >= static_cast<int>( items_.size() ) )
		{
			selectedIndex_ = items_.size() - 1;

			selectedIndexChangedEvent_.Invoke( this );
		}

		Invalidate();
	}*/
}
