1. INSTALLATION & Build-System
------------------------------

The template uses CMake as build-system. The following libraries are already supported in the template:
    - xsiteLib
    - OpenSceneGraph
    - VTK (can be enabled in CMakeLists.txt)

If you have these libraries but they are not installed or copied to the
default-paths (e.g. /usr/lib etc.) you may have to help CMake find your
installation. This can be done by setting environment-variables. They are named
XXX_ROOT (where XXX stands for the library you are looking for). These names are
(of course) case sensitive! Take a look at the Find-scripts in ./CMakeModules if
you have problems.

2. Modify the plugin
---------------------

1. Rename Template.h & Template.cpp to something more meaningful (let's
say "Foo")

2. Edit both files and rename the class to "Foo"

3. Edit the implementation Foo::createScene() and create your desired
scene. Attach the regular scene to the _root node. Attach HUD elements 
to _leftWallRoot, _frontWallRoot, rightWallRoot, and _floorRoot respectively. 

4. To scale, position, and orient the scene to a sensible default value, use
setSceneDefaultScale / setSceneDefaultPosition / setSceneDefaultOrientation.

5. You may want to modifiy lighting in Foo::setLight() 

6. Release all allocated memory (in destructor)! This is IMPORTANT as
the host is supposed to run more than 3 minutes.

7. Rewrite the Foo::simulate method to simulate the next step of your
simulation/animation. Publish every relevant state change using
xs::Event. You can utilize xs::Binary to store an arbitray number of
variables & types. An instance of xs::Binary can easily be published
with xs::Event.
Remember that you should not apply any state changes to the scenegraph
within this method. This is left for xs::PluginBase::update().

8. Modify Foo::update() method to update the scene with:
7.1 Modify Foo::parseEvents method to parse the Incoming Event Queue.
Every instance of your plugin shall recieve state changes here. You can
directly apply these state changes to your scene graph here too.
FlyStick data etc. is recieved here too.

9. Edit CMakeLists.txt and rename the project to whatever is
appropriate. Modify the PROJECT_SOURCES variable and set/add Foo.cpp.
Other source files are added like this:
set(PROJECT_SOURCES 
    Foo.cpp
    Bar.cpp
)

10. Run "cmake ." in the plugin directory.

11. If everything ran fine, you should be able to compile your plugin
with "make"

12. Run xsiteViewer & xsiteManagement (and possibly xsiteServer) and check your plugin

