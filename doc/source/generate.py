# BuildTarget: _static/GafferLogo.svg _static/GafferLogoMini.svg

import shutil
import os

shutil.copyfile( os.path.join( "..", "..", "resources", "GafferLogo.svg" ), os.path.join( "_static", "GafferLogo.svg" ) )
shutil.copyfile( os.path.join( "..", "..", "resources", "GafferLogoMini.svg" ), os.path.join( "_static", "GafferLogoMini.svg" ) )
