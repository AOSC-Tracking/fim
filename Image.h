#ifndef IMAGE_FBVI_H
#define IMAGE_FBVI_H
#include "fim.h"
namespace fim
{
/*
 *	A general rule in programming the Image methods is that
 *	of keeping them minimal.
 *	Combining them is matter of the invoking function.
 *	So, there is NO internal triggering  here to call ANY 
 *	display function.
 */
class Image
{
	struct ida_image *fimg    ;
	struct ida_image *simg    ;
	struct ida_image *img     ;
	float            scale    ;
	float            newscale ;
	int              c, editable , once ;
	int              need_read ;
	int              i, arg, key;
	int    new_image,invalid;		//the first time the image is loaded it is set to 1
	//int redraw;
	int only_first_rescale;		//TEMPORARY
	
	int steps,top,len,pos,left ;
	char             *line, *info, *desc;
	char *fname;

	public:
	char* getInfo();
	void Image::auto_scale();
	//void Image::auto_scale_();
	int Image::valid(){return invalid?0:1;}////////!!!!
	Image::Image(const char *fname_);

	void Image::pan_up();
	void Image::pan_down();
	void Image::pan_right();
	void Image::pan_left();
	void Image::reduce(float factor=1.322);	//FIX ME
	void Image::magnify(float factor=1.322);
	void Image::display();
	void Image::redisplay();
	int Image::onBottom();
	int Image::onRight();
	int Image::onLeft();
	int Image::onTop();
	void Image::auto_width_scale();
	void Image::auto_height_scale();
	private:
	void Image::reset();
	void scale_fix_top_left();

	int Image::rescale();
	void Image::load(const char *fname_);
	void Image::free_mem();
};
};
#endif
