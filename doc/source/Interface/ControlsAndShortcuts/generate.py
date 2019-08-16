# BuildTarget: images/nodeSetStandardSet.png

import shutil
import os

shutil.copyfile(os.path.join( os.environ["GAFFER_ROOT"], "graphics", "nodeSetStandardSet.png" ), os.path.join( "images", "nodeSetStandardSet.png" ) )
