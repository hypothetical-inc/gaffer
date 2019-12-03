# BuildTarget: images/pathChooser.png

import shutil
import os

shutil.copyfile( os.path.join( os.environ["$GAFFER_ROOT"], "graphics", "pathChooser.png" ), os.path.join( "images", "pathChooser.png" ) )

