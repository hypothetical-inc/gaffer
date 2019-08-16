# BuildTarget: images/nodes.png

import shutil
import os

shutil.copyfile( os.path.join( os.environ["$GAFFER_ROOT"], "graphics", "nodes.png" ), os.path.join( "images", "nodes.png" ) )
shutil.copyfile( os.path.join( os.environ["$GAFFER_ROOT"], "graphics", "plug.png" ), os.path.join( "images", "plug.png" ) )
shutil.copyfile( os.path.join( os.environ["$GAFFER_ROOT"], "graphics", "values.png" ), os.path.join( "images", "values.png" ) )
