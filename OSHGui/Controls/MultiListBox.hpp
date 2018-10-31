#pragma once

#include "Control.hpp"

namespace OSHGui {
	class MultiListItem;

	class OSHGUI_EXPORT MultiListBox : public Control {
	private:
		std::vector< MultiListItem * > m_items;

		Drawing::RectangleI m_items_rect;
		Drawing::SizeI m_items_size;
		int m_first_visible_item_index;
		static const int DefaultItemPadding;

		int firstVisibleItemIndex_;
		int maxVisibleItems_;
	public:
		MultiListBox();
		virtual ~MultiListBox();

		virtual void OnMouseClick( const MouseMessage &mouse ) override;
		virtual void DrawSelf( Drawing::RenderContext &context ) override;
		virtual void PopulateGeometry() override;
		virtual void SetSize(const Drawing::SizeI &size) override;

		void SetSelected( int index );
		
		void AddItem( const Misc::AnsiString &text );
		void AddItem( MultiListItem *item );
		void InsertItem( int index, const Misc::AnsiString &text );
		void InsertItem( int index, MultiListItem *item );
		//void RemoveItem( int index );
	};

	class OSHGUI_EXPORT MultiListItem {
	public:
		bool selected = false;
		virtual ~MultiListItem() = default;
		virtual const Misc::AnsiString& GetItemText() const = 0;
	};

	class OSHGUI_EXPORT StringMultiListItem : public MultiListItem
	{
	public:
		bool selected = false;
		StringMultiListItem(const Misc::AnsiString &text);

		virtual const Misc::AnsiString& GetItemText() const override;

	protected:
		Misc::AnsiString text;
	};
}
