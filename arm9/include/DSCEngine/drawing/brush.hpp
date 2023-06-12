#pragma once

namespace DSC
{
	class Brush
	{
	public:		
		virtual int get_color(int x, int y) const = 0;
		virtual ~Brush();
	};
	
	class SolidColorBrush : public Brush
	{
	private:
		int color;
	public:		
		SolidColorBrush(int color);
		virtual int get_color(int x, int y) const override;
		~SolidColorBrush();
	};
}