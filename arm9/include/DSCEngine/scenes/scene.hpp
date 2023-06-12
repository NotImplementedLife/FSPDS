/*!
 * \file scene.hpp
 * \brief Scene logic and transitions
 */
 
#pragma once

#include "DSCEngine/events/event.hpp"

namespace DSC
{	
	struct SceneCom;
	
	/*! \brief An active game execution part	
	 */
	class Scene
	{
	public:
		/*! \brief Creates a new Scene instance		
		 * 	\warning Do \b NOT use the constructor for anything but loading the constructor parameters!
		 *	Use Scene::init() for the out-of-class initialization purposes.			
		 */
		Scene() = default;				
		
		/*! \brief Closes this scene and clears all its resources
			\returns an internal inter-scene communication which can load the next scene
			
			\warning 
				- Due to the way close() is implemented, don't try to use statically declared Scenes. Only use pointer Scenes with their specialized api (dsc_launch(), SceneCom::next())
						
				- Don't access any of the Scene's properies after close()-ing it! close() literally deletes the object in order to make room for the next Scene to load.
				Don't do:
				\code{.cpp}
					class MyScene : public DSC::Scene
					{
					public:
						int my_property = 123;

						void next_scene() 
						{
							this->close()->next(new MyOtherScene(my_property));                        
							// After close(), my_property is no longer available
							// Therefore, the line above leads to runtime error
						}
					};
				\endcode				
				Do it this way:				
				\code{.cpp}
					class MyScene : public DSC::Scene
					{
					public:
						int my_property = 123;

						void next_scene() 
						{
							// save my_property to stack
							int my_backup = my_property;
							// Even though closing the Scene deletes its properties,
							// we can still retrieve our valuable variable from the
							// local scope
							this->close()->next(new MyOtherScene(my_backup));
						}
					};
				\endcode
				
				- Due to limited hardware resources, only one scene should exist at a time.
				  Is is recommended not to create your next Scene before closing the previous one.
				\code{.cpp}
					// Potentially buggy:
					
					MyNewScene* new_scene = new MyNewScene();
					this->close()->next(new_scene); // "this" and "new_scene" exist at the same time
					
					// Safer approach:
					
					auto* scene_com = this->close(); // "this" is now gone
					MyNewScene* new_scene = new MyNewScene(); // it is the only active scene
					scene_com->next(new_scene); // Ok
				\endcode				
				
		 */
		SceneCom* close();
		
		/*! \brief Contains Scene initialization routines (setting variables, loading VRAM etc.)
		 */
		virtual void init();
		
		/*! \brief The Scene main loop logic. This method is executed at VBlank.
		 */
		virtual void frame();
				
		__attribute__((noinline))
		virtual void run();
						
		/*! \brief Key down event */
		Event key_down;
		
		/*! \brief Key held event */
		Event key_held;
		
		/*! \brief Key up event */
		Event key_up;			
		
		virtual ~Scene() = default;
	};
	
	/*! \brief Inter-Scene communication object
	 *	\warning Do not attempt to create SceneCom instances! 
	 *  Only use the SceneCom object you get from Scene::close().
	 */
	struct SceneCom
	{				
		/*! \brief Loads the next scene
			\param [in] new_scene new Scene instance
		 */
		void next(Scene* new_scene);		
	};	
		
	
	extern Scene* __MAIN_SCENE__;	
	
	void init_main_scene();
	
	#ifdef DSC_INTERNAL	
	extern SceneCom __SceneComInstance__;	
	#endif
}

#define dsc_launch(scene_t,...) void DSC::init_main_scene(){DSC::__MAIN_SCENE__=new scene_t(__VA_ARGS__);}