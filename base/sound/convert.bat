SET PATH=D:\Projects\IcedTech\tools\freac-1.0.32-bin

for /R D:\Projects\IcedTech\base\sound %%G IN (*.ogg) do (
	freaccmd -e WAVE %%G -o %%~pG%%~nG.wav
	del %%G )

pause