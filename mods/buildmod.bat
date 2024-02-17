set "filename=%1"

for %%F in ("%filename%") do (
  set "basename=%%~nF"
)

cl /I..\lib /I..\include\libconfig-1.7.3\lib /LD /EHsc  ..\Debug\xbot.lib ..\include\libconfig.lib %filename% 
link /DLL /out:.\%basename%.dll %basename%.obj ..\Debug\xbot.lib ..\include\libconfig.lib
