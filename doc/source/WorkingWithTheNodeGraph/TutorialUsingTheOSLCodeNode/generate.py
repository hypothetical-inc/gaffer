# BuildTarget: images/plus.png

import shutil
import os

shutil.copyfile( os.path.join( os.environ["$GAFFER_ROOT"], "graphics", "plus.png" ), os.path.join( "images", "plus.png" ) )
