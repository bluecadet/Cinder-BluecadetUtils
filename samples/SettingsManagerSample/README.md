#Settings Manager Sample 

###Example of extending the base SettingsManager with a project specific SettingsManager. 

####BluecadetUtils > SettingsManager
Holds standard settings like framerate, debug mode, drawing touches, reset timers. The loading of these values through standardAppSettings.json should be done after setting any properties through the command line arguments (also parsed in the SettingsManager).

####SampleSettingsManager
The naming of this file could be specific to your project. For example, NasmSettingsManager, and this extends SettingsManager. This file holds settings related to your project (colors, content specific, etc.)
