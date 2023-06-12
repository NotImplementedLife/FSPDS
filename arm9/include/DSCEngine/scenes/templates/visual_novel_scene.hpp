/*!
* \file visual_novel_scene.hpp
* \brief Template for visual novel scenes
*/

#include "DSCEngine/scenes/scene.hpp"

namespace DSC::SceneTemplates
{
	class VisualNovelScene : public Scene
	{
	public:
		VisualNovelScene();
		virtual void init() override;		
		virtual void frame() override;
		~VisualNovelScene();
	};
 
}