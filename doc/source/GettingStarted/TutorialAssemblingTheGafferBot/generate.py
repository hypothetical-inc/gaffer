# BuildTarget: images/expansion.png
import shutil
import os

shutil.copyfile( os.path.join( os.environ["GAFFER_ROOT"], "graphics", "expansion.png" ), os.path.join( "images", "expansion.png" ) )
shutil.copyfile( os.path.join( os.environ["GAFFER_ROOT"], "graphics", "plus.png" ), os.path.join( "images", "plug.png" ) )
shutil.copyfile( os.path.join( os.environ["GAFFER_ROOT"], "graphics", "nodeSetDriverNodeSelection.png" ), os.path.join( "images", "nodeSetDriverNodeSelection.png" ) )
shutil.copyfile( os.path.join( os.environ["GAFFER_ROOT"], "graphics", "nodeSetStandardSet.png" ), os.path.join( "images", "nodeSetStandardSet.png" ) )
shutil.copyfile( os.path.join( os.environ["GAFFER_ROOT"], "graphics", "layoutButton.png" ), os.path.join( "images", "layoutButton.png" ) )
shutil.copyfile( os.path.join( os.environ["GAFFER_ROOT"], "graphics", "objects.png" ), os.path.join( "images", "objects.png" ) )
shutil.copyfile( os.path.join( os.environ["GAFFER_ROOT"], "graphics", "addObjects.png" ), os.path.join( "images", "addObjects.png" ) )
shutil.copyfile( os.path.join( os.environ["GAFFER_ROOT"], "graphics", "replaceObjects.png" ), os.path.join( "images", "replaceObjects.png" ) )
shutil.copyfile( os.path.join( os.environ["GAFFER_ROOT"], "graphics", "collapsibleArrowRight.png" ), os.path.join( "images", "collapsibleArrowRight.png" ) )
shutil.copyfile( os.path.join( os.environ["GAFFER_ROOT"], "graphics", "timelinePlay.png" ), os.path.join( "images", "timelinePlay.png" ) )
shutil.copyfile( os.path.join( os.environ["GAFFER_ROOT"], "graphics", "removeObjects.png" ), os.path.join( "images", "removeObjects.png" ) )
shutil.copyfile( os.path.join( os.environ["GAFFER_ROOT"], "graphics", "gafferSceneUITranslateTool.png" ), os.path.join( "images", "gafferSceneUITranslateTool.png" ) )
shutil.copyfile( os.path.join( os.environ["GAFFER_ROOT"], "graphics", "gafferSceneUIRotateTool.png" ), os.path.join( "images", "gafferSceneUIRotateTool.png" ) )
