# BuildTarget: images/gear.png

import shutil
import os

shutil.copyfile( os.path.join( os.environ["$GAFFER_ROOT"], "graphics", "gear.png" ), os.path.join( "images", "gear.png" ) )
shutil.copyfile( os.path.join( os.environ["$GAFFER_ROOT"], "graphics", "info.png" ), os.path.join( "images", "info.png" ) )
shutil.copyfile( os.path.join( os.environ["$GAFFER_ROOT"], "graphics", "plugAdder.png" ), os.path.join( "images", "plugAdder.png" ) )
shutil.copyfile( os.path.join( os.environ["$GAFFER_ROOT"], "graphics", "valueChanged.png" ), os.path.join( "images", "valueChanged.png" ) )
